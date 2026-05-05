#include <windows.h>
#include <tlhelp32.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "HgServers.h"

namespace {

constexpr WORD kMachineAmd64 = 0x8664;
constexpr wchar_t kSteamEeExe[] = L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\bin\\win32\\nwmain.exe";
constexpr wchar_t kSteamEeOriginalExe[] = L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\bin\\win32\\nwmain_org.exe";
constexpr wchar_t kDefaultDiamondConfigRoot[] = L"C:\\NWN\\Config";

struct Options {
  std::wstring target_exe;
  std::wstring dll_path;
  std::wstring server_id = L"111";
  std::wstring server_address;
  std::wstring server_host;
  std::wstring connect_address;
  std::wstring diamond_account = L"1";
  std::wstring diamond_config_root = kDefaultDiamondConfigRoot;
  std::wstring auto_password;
  std::wstring auto_password_source = L"default";
  std::wstring auto_character;
  std::wstring auto_use_object_id;
  std::wstring auto_use_object_type;
  std::wstring auto_use_object_name;
  std::wstring auto_use_object_delay_seconds;
  std::wstring auto_use_object_second_id;
  std::wstring auto_use_object_second_type;
  std::wstring auto_use_object_second_name;
  std::wstring auto_use_object_second_delay_seconds;
  std::wstring auto_use_area_id;
  std::wstring auto_door_transition_delay_ms;
  std::wstring auto_door_transition_second_delay_ms;
  std::wstring auto_open_inventory_delay_ms;
  unsigned short server_port = 0;
  std::vector<std::wstring> extra_args;
  bool list_servers = false;
  bool dry_run = false;
  bool inject_test = false;
  bool driver_only = false;
  bool skip_startup_movies = true;
  bool password_explicit = false;
  bool auto_use_allow_unuseable = false;
  bool auto_use_transition_click = false;
  bool auto_trigger_walk_probe = false;
  bool auto_door_open_first = false;
  bool auto_open_inventory = false;
};

struct UniqueHandle {
  HANDLE value = nullptr;

  UniqueHandle() = default;
  explicit UniqueHandle(HANDLE handle) : value(handle) {}
  ~UniqueHandle() { reset(); }

  UniqueHandle(const UniqueHandle&) = delete;
  UniqueHandle& operator=(const UniqueHandle&) = delete;

  UniqueHandle(UniqueHandle&& other) noexcept : value(other.value) { other.value = nullptr; }
  UniqueHandle& operator=(UniqueHandle&& other) noexcept {
    if (this != &other) {
      reset();
      value = other.value;
      other.value = nullptr;
    }
    return *this;
  }

  void reset(HANDLE handle = nullptr) {
    if (value != nullptr && value != INVALID_HANDLE_VALUE) {
      CloseHandle(value);
    }
    value = handle;
  }

  HANDLE get() const { return value; }
  HANDLE release() {
    HANDLE handle = value;
    value = nullptr;
    return handle;
  }
  explicit operator bool() const { return value != nullptr && value != INVALID_HANDLE_VALUE; }
};

std::wstring Win32Message(DWORD error) {
  wchar_t* buffer = nullptr;
  const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
  DWORD length = FormatMessageW(flags, nullptr, error, 0, reinterpret_cast<wchar_t*>(&buffer), 0, nullptr);
  std::wstring message = length != 0 ? std::wstring(buffer, length) : L"unknown error";
  if (buffer != nullptr) {
    LocalFree(buffer);
  }
  while (!message.empty() && (message.back() == L'\r' || message.back() == L'\n' || message.back() == L'.' || iswspace(message.back()))) {
    message.pop_back();
  }
  return message;
}

std::wstring FullPath(const std::wstring& path) {
  DWORD needed = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
  if (needed == 0) {
    return path;
  }
  std::wstring result(needed, L'\0');
  DWORD written = GetFullPathNameW(path.c_str(), needed, result.data(), nullptr);
  if (written == 0) {
    return path;
  }
  result.resize(written);
  return result;
}

bool FileExists(const std::wstring& path) {
  DWORD attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool DirectoryExists(const std::wstring& path) {
  DWORD attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

std::wstring ParentPath(const std::wstring& path) {
  size_t slash = path.find_last_of(L"\\/");
  if (slash == std::wstring::npos) {
    return L".";
  }
  return path.substr(0, slash);
}

std::wstring JoinPath(const std::wstring& left, const std::wstring& right) {
  if (left.empty()) {
    return right;
  }
  if (right.empty()) {
    return left;
  }
  if (left.back() == L'\\' || left.back() == L'/') {
    return left + right;
  }
  return left + L"\\" + right;
}

std::wstring GetSelfPath() {
  std::wstring path(MAX_PATH, L'\0');
  for (;;) {
    DWORD written = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    if (written == 0) {
      throw std::runtime_error("GetModuleFileNameW failed");
    }
    if (written < path.size() - 1) {
      path.resize(written);
      return path;
    }
    path.resize(path.size() * 2);
  }
}

std::wstring GetCurrentDirectoryString() {
  DWORD needed = GetCurrentDirectoryW(0, nullptr);
  if (needed == 0) {
    return L".";
  }
  std::wstring result(needed, L'\0');
  DWORD written = GetCurrentDirectoryW(needed, result.data());
  if (written == 0) {
    return L".";
  }
  result.resize(written);
  return result;
}

std::wstring GetEnvironmentVariableString(const wchar_t* name) {
  DWORD needed = GetEnvironmentVariableW(name, nullptr, 0);
  if (needed == 0) {
    return L"";
  }

  std::wstring result(needed, L'\0');
  DWORD written = GetEnvironmentVariableW(name, result.data(), needed);
  if (written == 0 || written >= needed) {
    return L"";
  }

  result.resize(written);
  return result;
}

std::wstring ResolveDefaultEePath() {
  const std::wstring workspace_copy = GetCurrentDirectoryString() + L"\\Neverwinter Nights EE\\bin\\win32\\nwmain.exe";
  const std::wstring candidates[] = {
      kSteamEeExe,
      kSteamEeOriginalExe,
      workspace_copy,
  };

  for (const auto& candidate : candidates) {
    if (FileExists(candidate)) {
      return candidate;
    }
  }
  return workspace_copy;
}

std::wstring QuoteArg(const std::wstring& arg) {
  if (arg.empty()) {
    return L"\"\"";
  }
  const bool needs_quotes = arg.find_first_of(L" \t\"") != std::wstring::npos;
  if (!needs_quotes) {
    return arg;
  }

  std::wstring out = L"\"";
  size_t backslashes = 0;
  for (wchar_t ch : arg) {
    if (ch == L'\\') {
      ++backslashes;
    } else if (ch == L'"') {
      out.append(backslashes * 2 + 1, L'\\');
      out.push_back(ch);
      backslashes = 0;
    } else {
      out.append(backslashes, L'\\');
      backslashes = 0;
      out.push_back(ch);
    }
  }
  out.append(backslashes * 2, L'\\');
  out.push_back(L'"');
  return out;
}

bool LooksLikeAddress(std::wstring_view value) {
  return value.find(L':') != std::wstring_view::npos;
}

bool ParsePort(std::wstring_view value, unsigned short* port) {
  if (value.empty()) {
    return false;
  }

  unsigned long parsed = 0;
  for (wchar_t ch : value) {
    if (ch < L'0' || ch > L'9') {
      return false;
    }
    parsed = parsed * 10 + static_cast<unsigned long>(ch - L'0');
    if (parsed > 65535) {
      return false;
    }
  }

  if (parsed == 0) {
    return false;
  }

  *port = static_cast<unsigned short>(parsed);
  return true;
}

bool ParseEndpoint(std::wstring_view value, std::wstring* host, unsigned short* port) {
  const size_t colon = value.rfind(L':');
  if (colon == std::wstring_view::npos || colon == 0 || colon + 1 >= value.size()) {
    return false;
  }

  unsigned short parsed_port = 0;
  if (!ParsePort(value.substr(colon + 1), &parsed_port)) {
    return false;
  }

  *host = std::wstring(value.substr(0, colon));
  *port = parsed_port;
  return true;
}

std::wstring FormatEndpoint(std::wstring_view host, unsigned short port) {
  return std::wstring(host) + L":" + std::to_wstring(port);
}

WORD ReadPeMachine(const std::wstring& path) {
  UniqueHandle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
  if (!file) {
    throw std::runtime_error("failed to open PE file");
  }

  IMAGE_DOS_HEADER dos{};
  DWORD read = 0;
  if (!ReadFile(file.get(), &dos, sizeof(dos), &read, nullptr) || read != sizeof(dos) || dos.e_magic != IMAGE_DOS_SIGNATURE) {
    throw std::runtime_error("invalid DOS header");
  }

  LARGE_INTEGER offset{};
  offset.QuadPart = dos.e_lfanew;
  if (!SetFilePointerEx(file.get(), offset, nullptr, FILE_BEGIN)) {
    throw std::runtime_error("failed to seek PE header");
  }

  DWORD signature = 0;
  if (!ReadFile(file.get(), &signature, sizeof(signature), &read, nullptr) || read != sizeof(signature) || signature != IMAGE_NT_SIGNATURE) {
    throw std::runtime_error("invalid PE signature");
  }

  IMAGE_FILE_HEADER file_header{};
  if (!ReadFile(file.get(), &file_header, sizeof(file_header), &read, nullptr) || read != sizeof(file_header)) {
    throw std::runtime_error("failed to read PE file header");
  }
  return file_header.Machine;
}

void RequireAmd64(const std::wstring& label, const std::wstring& path) {
  WORD machine = ReadPeMachine(path);
  if (machine != kMachineAmd64) {
    wchar_t buffer[256]{};
    swprintf_s(buffer, L"%s must be a 64-bit AMD64 PE file; machine=0x%04X path=%s", label.c_str(), machine, path.c_str());
    throw std::runtime_error("PE architecture mismatch");
  }
}

void PrintUsage() {
  wprintf(L"HG Bridge launcher\n\n");
  wprintf(L"Usage:\n");
  wprintf(L"  hgbridge_launcher.exe [--server ID|HOST:PORT] [--connect HOST:PORT] [--driver-only] [--diamond-account N] [--password VALUE] [--auto-character NAME] [--auto-use-object-name NAME] [--auto-open-inventory] [--keep-movies] [--ee PATH] [--dll PATH] [--dry-run] [--inject-test] [-- EXTRA_NWN_ARGS]\n\n");
  wprintf(L"Defaults:\n");
  wprintf(L"  --server 111\n");
  wprintf(L"  --diamond-account 1 from C:\\NWN\\Config\\1.nwn*.ini\n");
  wprintf(L"  --password <Diamond nwnplayer.ini Profile/Password, fallback a>\n");
  wprintf(L"  --connect HOST:PORT overrides the actual client destination while keeping --server compatibility context\n");
  wprintf(L"  --driver-only injects only the launch/test driver; protocol and resource compatibility must come from the proxy\n");
  wprintf(L"  driver-only mode keeps +connect/+password/CAppManager driving enabled while skipping compatibility hooks\n");
  wprintf(L"  --auto-character, --auto-use-*, and --auto-open-inventory are driver/test helpers only; they do not enable compatibility hooks\n");
  wprintf(L"  startup movies and intro splash disabled before launch\n");
  wprintf(L"  --ee Steam nwmain.exe, then Steam nwmain_org.exe, then the workspace EE copy\n");
  wprintf(L"  --dll <launcher directory>\\nwncx_hg.dll\n\n");
}

void ListServers() {
  for (const auto& server : hgbridge::kHgServers) {
    wprintf(L"%ls  %ls\n", std::wstring(server.id).c_str(), std::wstring(server.address).c_str());
  }
}

std::string TrimAscii(std::string_view value) {
  size_t first = 0;
  while (first < value.size() && (value[first] == ' ' || value[first] == '\t' || value[first] == '\r' || value[first] == '\n')) {
    ++first;
  }

  size_t last = value.size();
  while (last > first && (value[last - 1] == ' ' || value[last - 1] == '\t' || value[last - 1] == '\r' || value[last - 1] == '\n')) {
    --last;
  }

  return std::string(value.substr(first, last - first));
}

std::string LeadingWhitespace(std::string_view value) {
  size_t count = 0;
  while (count < value.size() && (value[count] == ' ' || value[count] == '\t')) {
    ++count;
  }
  return std::string(value.substr(0, count));
}

bool IsPortAssignment(std::string_view trimmed) {
  return trimmed.size() >= 5 && trimmed.substr(0, 4) == "port" &&
         [&]() {
           size_t index = 4;
           while (index < trimmed.size() && (trimmed[index] == ' ' || trimmed[index] == '\t')) {
             ++index;
           }
           return index < trimmed.size() && trimmed[index] == '=';
         }();
}

bool IsAssignment(std::string_view trimmed, std::string_view key) {
  if (trimmed.size() <= key.size() || trimmed.substr(0, key.size()) != key) {
    return false;
  }

  size_t index = key.size();
  while (index < trimmed.size() && (trimmed[index] == ' ' || trimmed[index] == '\t')) {
    ++index;
  }
  return index < trimmed.size() && trimmed[index] == '=';
}

std::vector<std::string> SplitLines(const std::string& text) {
  std::vector<std::string> lines;
  size_t position = 0;
  while (position < text.size()) {
    size_t end = text.find('\n', position);
    if (end == std::string::npos) {
      lines.push_back(text.substr(position));
      break;
    }

    size_t length = end - position;
    if (length > 0 && text[position + length - 1] == '\r') {
      --length;
    }
    lines.push_back(text.substr(position, length));
    position = end + 1;
  }
  return lines;
}

std::wstring ResolveEeSettingsPath() {
  const std::wstring user_profile = GetEnvironmentVariableString(L"USERPROFILE");
  if (user_profile.empty()) {
    return L"";
  }
  return user_profile + L"\\Documents\\Neverwinter Nights\\settings.tml";
}

std::wstring ResolveEeUserRoot() {
  const std::wstring settings_path = ResolveEeSettingsPath();
  return settings_path.empty() ? L"" : ParentPath(settings_path);
}

std::wstring ResolveEeIniPath() {
  const std::wstring user_root = ResolveEeUserRoot();
  return user_root.empty() ? L"" : user_root + L"\\nwn.ini";
}

std::wstring ResolveWorkspaceRoot() {
  const std::wstring current = GetCurrentDirectoryString();
  if (FileExists(current + L"\\NWN Diamond\\nwnplayer.ini")) {
    return current;
  }

  const std::wstring self_dir = ParentPath(GetSelfPath());
  const std::wstring build_dir = ParentPath(self_dir);
  const std::wstring repository_root = ParentPath(build_dir);
  if (FileExists(repository_root + L"\\NWN Diamond\\nwnplayer.ini")) {
    return repository_root;
  }

  return current;
}

std::wstring ResolveEeInstallRoot(const Options& options) {
  const std::wstring win32_dir = ParentPath(options.target_exe);
  const std::wstring bin_dir = ParentPath(win32_dir);
  const std::wstring install_root = ParentPath(bin_dir);
  return install_root.empty() ? ParentPath(options.target_exe) : install_root;
}

std::wstring ResolveAssetBundleRoot(const Options& options) {
  const std::wstring configured = GetEnvironmentVariableString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!configured.empty()) {
    return FullPath(configured);
  }

  const std::wstring install_root = ResolveEeInstallRoot(options);
  const std::wstring steam_bundle = JoinPath(install_root, L"hg-bridge-assets");
  if (DirectoryExists(steam_bundle)) {
    return FullPath(steam_bundle);
  }

  return FullPath(steam_bundle);
}

std::string EscapeTomlString(const std::string& value) {
  std::string escaped;
  escaped.reserve(value.size());
  for (char ch : value) {
    if (ch == '\\' || ch == '"') {
      escaped.push_back('\\');
    }
    escaped.push_back(ch);
  }
  return escaped;
}

std::wstring AsciiToWide(const std::string& value) {
  std::wstring result;
  result.reserve(value.size());
  for (unsigned char ch : value) {
    result.push_back(static_cast<wchar_t>(ch));
  }
  return result;
}

std::string WideToUtf8(const std::wstring& value) {
  if (value.empty()) {
    return {};
  }

  const int needed = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
  if (needed <= 0) {
    return {};
  }

  std::string result(static_cast<size_t>(needed), '\0');
  WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), result.data(), needed, nullptr, nullptr);
  return result;
}

bool ReadIniValue(const std::wstring& path, std::string_view section, std::string_view key, std::string* value) {
  if (value == nullptr) {
    return false;
  }

  std::ifstream input(std::filesystem::path(path), std::ios::binary);
  if (!input) {
    return false;
  }

  std::ostringstream buffer;
  buffer << input.rdbuf();
  bool inside_section = false;
  for (const std::string& line : SplitLines(buffer.str())) {
    const std::string trimmed = TrimAscii(line);
    if (trimmed.size() >= 2 && trimmed.front() == '[' && trimmed.back() == ']') {
      inside_section = trimmed.substr(1, trimmed.size() - 2) == section;
      continue;
    }

    if (!inside_section || !IsAssignment(trimmed, key)) {
      continue;
    }

    const size_t equals = trimmed.find('=');
    if (equals == std::string::npos) {
      return false;
    }

    *value = TrimAscii(std::string_view(trimmed).substr(equals + 1));
    return !value->empty();
  }

  return false;
}

std::wstring UpdateEeSettingsAssignment(const std::string& section, const std::string& key, const std::string& assignment) {
  const std::wstring settings_path = ResolveEeSettingsPath();
  if (settings_path.empty()) {
    throw std::runtime_error("USERPROFILE is not set");
  }

  std::ifstream input(std::filesystem::path(settings_path), std::ios::binary);
  if (!input) {
    throw std::runtime_error("failed to open EE settings.tml");
  }

  std::ostringstream buffer;
  buffer << input.rdbuf();
  std::string contents = buffer.str();
  const std::string newline = contents.find("\r\n") != std::string::npos ? "\r\n" : "\n";
  const bool had_final_newline = !contents.empty() && contents.back() == '\n';
  std::vector<std::string> lines = SplitLines(contents);

  const std::string section_header = "[" + section + "]";
  int found_section = -1;
  bool inside_section = false;
  bool updated = false;

  for (size_t index = 0; index < lines.size(); ++index) {
    const std::string trimmed = TrimAscii(lines[index]);
    if (trimmed.size() >= 2 && trimmed.front() == '[' && trimmed.back() == ']') {
      if (inside_section && !updated) {
        const std::string indent = found_section >= 0 ? LeadingWhitespace(lines[static_cast<size_t>(found_section)]) + "\t" : "";
        lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(index), indent + assignment);
        updated = true;
        break;
      }
      inside_section = trimmed == section_header;
      if (inside_section) {
        found_section = static_cast<int>(index);
      }
    } else if (inside_section && IsAssignment(trimmed, key)) {
      lines[index] = LeadingWhitespace(lines[index]) + assignment;
      updated = true;
      break;
    }
  }

  if (!updated) {
    if (inside_section && found_section >= 0) {
      const std::string indent = LeadingWhitespace(lines[static_cast<size_t>(found_section)]) + "\t";
      lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(found_section + 1), indent + assignment);
    } else {
      if (!lines.empty() && !lines.back().empty()) {
        lines.emplace_back();
      }
      lines.emplace_back(section_header);
      lines.emplace_back("\t" + assignment);
    }
  }

  std::ofstream output(std::filesystem::path(settings_path), std::ios::binary | std::ios::trunc);
  if (!output) {
    throw std::runtime_error("failed to write EE settings.tml");
  }

  for (size_t index = 0; index < lines.size(); ++index) {
    output << lines[index];
    if (index + 1 < lines.size() || had_final_newline || !updated) {
      output << newline;
    }
  }

  return settings_path;
}

std::wstring UpdateEeSettingsStringValue(const std::string& section, const std::string& key, const std::string& value) {
  return UpdateEeSettingsAssignment(section, key, key + " = \"" + EscapeTomlString(value) + "\"");
}

std::wstring UpdateEeSettingsBoolValue(const std::string& section, const std::string& key, bool value) {
  return UpdateEeSettingsAssignment(section, key, key + std::string(" = ") + (value ? "true" : "false"));
}

std::wstring UpdateIniValue(const std::wstring& path, const std::string& section, const std::string& key, const std::string& value) {
  if (path.empty()) {
    throw std::runtime_error("EE user directory is not set");
  }

  std::string contents;
  {
    std::ifstream input(std::filesystem::path(path), std::ios::binary);
    if (input) {
      std::ostringstream buffer;
      buffer << input.rdbuf();
      contents = buffer.str();
    }
  }

  const std::string newline = contents.find("\r\n") != std::string::npos ? "\r\n" : "\n";
  const bool had_final_newline = !contents.empty() && contents.back() == '\n';
  std::vector<std::string> lines = SplitLines(contents);
  const std::string section_header = "[" + section + "]";
  const std::string assignment = key + "=" + value;
  int found_section = -1;
  bool inside_section = false;
  bool updated = false;

  for (size_t index = 0; index < lines.size(); ++index) {
    const std::string trimmed = TrimAscii(lines[index]);
    if (trimmed.size() >= 2 && trimmed.front() == '[' && trimmed.back() == ']') {
      if (inside_section && !updated) {
        lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(index), assignment);
        updated = true;
        break;
      }
      inside_section = trimmed == section_header;
      if (inside_section) {
        found_section = static_cast<int>(index);
      }
    } else if (inside_section && IsAssignment(trimmed, key)) {
      lines[index] = assignment;
      updated = true;
      break;
    }
  }

  if (!updated) {
    if (inside_section && found_section >= 0) {
      lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(found_section + 1), assignment);
    } else {
      if (!lines.empty() && !lines.back().empty()) {
        lines.emplace_back();
      }
      lines.emplace_back(section_header);
      lines.emplace_back(assignment);
    }
  }

  std::filesystem::create_directories(std::filesystem::path(ParentPath(path)));
  std::ofstream output(std::filesystem::path(path), std::ios::binary | std::ios::trunc);
  if (!output) {
    throw std::runtime_error("failed to write EE nwn.ini");
  }

  for (size_t index = 0; index < lines.size(); ++index) {
    output << lines[index];
    if (index + 1 < lines.size() || had_final_newline || !updated) {
      output << newline;
    }
  }

  return path;
}

bool StageDiamondCdKey(const std::wstring& workspace_root, const std::wstring& ee_user_root) {
  const std::wstring source = workspace_root + L"\\NWN Diamond\\nwncdkey.ini";
  if (!FileExists(source) || ee_user_root.empty()) {
    return false;
  }

  std::filesystem::create_directories(std::filesystem::path(ee_user_root));
  std::filesystem::copy_file(
      std::filesystem::path(source),
      std::filesystem::path(ee_user_root + L"\\nwncdkey.ini"),
      std::filesystem::copy_options::overwrite_existing);
  std::filesystem::copy_file(
      std::filesystem::path(source),
      std::filesystem::path(ee_user_root + L"\\cdkey.ini"),
      std::filesystem::copy_options::overwrite_existing);
  return true;
}

bool StageDiamondCdKeyFile(const std::wstring& source, const std::wstring& ee_user_root) {
  if (!FileExists(source) || ee_user_root.empty()) {
    return false;
  }

  std::filesystem::create_directories(std::filesystem::path(ee_user_root));
  std::filesystem::copy_file(
      std::filesystem::path(source),
      std::filesystem::path(ee_user_root + L"\\nwncdkey.ini"),
      std::filesystem::copy_options::overwrite_existing);
  std::filesystem::copy_file(
      std::filesystem::path(source),
      std::filesystem::path(ee_user_root + L"\\cdkey.ini"),
      std::filesystem::copy_options::overwrite_existing);
  return true;
}

bool ImportDiamondPlayerNameFromFile(const std::wstring& source, std::string* player_name) {
  if (player_name == nullptr) {
    return false;
  }

  std::string value;
  if (!ReadIniValue(source, "Profile", "Player Name", &value)) {
    return false;
  }

  UpdateEeSettingsStringValue("client.identity", "name", value);
  *player_name = value;
  return true;
}

bool ImportDiamondPlayerName(const std::wstring& workspace_root, std::string* player_name) {
  const std::wstring source = workspace_root + L"\\NWN Diamond\\nwnplayer.ini";
  return ImportDiamondPlayerNameFromFile(source, player_name);
}

bool ReadDiamondProfilePasswordFromFile(const std::wstring& source, std::string* password) {
  if (password == nullptr) {
    return false;
  }

  std::string value;
  if (!ReadIniValue(source, "Profile", "Password", &value)) {
    return false;
  }

  *password = value;
  return true;
}

bool ReadDiamondProfilePassword(const std::wstring& workspace_root, std::string* password) {
  const std::wstring source = workspace_root + L"\\NWN Diamond\\nwnplayer.ini";
  return ReadDiamondProfilePasswordFromFile(source, password);
}

std::wstring ResolveDiamondAccountFile(
    const Options& options,
    const std::wstring& workspace_root,
    const wchar_t* account_suffix,
    const wchar_t* workspace_filename) {
  if (!options.diamond_account.empty() && !options.diamond_config_root.empty()) {
    const std::wstring account_path =
        options.diamond_config_root + L"\\" + options.diamond_account + account_suffix;
    if (FileExists(account_path)) {
      return account_path;
    }
  }

  const std::wstring workspace_path = workspace_root + L"\\NWN Diamond\\" + workspace_filename;
  if (FileExists(workspace_path)) {
    return workspace_path;
  }

  return L"";
}

std::wstring UpdateEeSettingsPort(unsigned short port) {
  const std::wstring settings_path = ResolveEeSettingsPath();
  if (settings_path.empty()) {
    throw std::runtime_error("USERPROFILE is not set");
  }

  std::ifstream input(std::filesystem::path(settings_path), std::ios::binary);
  if (!input) {
    throw std::runtime_error("failed to open EE settings.tml");
  }

  std::ostringstream buffer;
  buffer << input.rdbuf();
  std::string contents = buffer.str();
  const std::string newline = contents.find("\r\n") != std::string::npos ? "\r\n" : "\n";
  const bool had_final_newline = !contents.empty() && contents.back() == '\n';
  std::vector<std::string> lines = SplitLines(contents);

  const std::string port_text = std::to_string(port);
  int server_net_section = -1;
  bool inside_server_net = false;
  bool port_updated = false;

  for (size_t index = 0; index < lines.size(); ++index) {
    const std::string trimmed = TrimAscii(lines[index]);
    if (trimmed.size() >= 2 && trimmed.front() == '[' && trimmed.back() == ']') {
      if (inside_server_net && !port_updated) {
        const std::string indent = server_net_section >= 0 ? LeadingWhitespace(lines[static_cast<size_t>(server_net_section)]) + "\t" : "";
        lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(index), indent + "port = " + port_text);
        port_updated = true;
        break;
      }
      inside_server_net = trimmed == "[server.net]";
      if (inside_server_net) {
        server_net_section = static_cast<int>(index);
      }
    } else if (inside_server_net && IsPortAssignment(trimmed)) {
      lines[index] = LeadingWhitespace(lines[index]) + "port = " + port_text;
      port_updated = true;
      break;
    }
  }

  if (!port_updated) {
    if (inside_server_net && server_net_section >= 0) {
      const std::string indent = LeadingWhitespace(lines[static_cast<size_t>(server_net_section)]) + "\t";
      lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(server_net_section + 1), indent + "port = " + port_text);
    } else {
      if (!lines.empty() && !lines.back().empty()) {
        lines.emplace_back();
      }
      lines.emplace_back("[server.net]");
      lines.emplace_back("\tport = " + port_text);
    }
  }

  std::ofstream output(std::filesystem::path(settings_path), std::ios::binary | std::ios::trunc);
  if (!output) {
    throw std::runtime_error("failed to write EE settings.tml");
  }

  for (size_t index = 0; index < lines.size(); ++index) {
    output << lines[index];
    if (index + 1 < lines.size() || had_final_newline || !port_updated) {
      output << newline;
    }
  }

  return settings_path;
}

Options ParseArgs(int argc, wchar_t** argv) {
  Options options;
  const std::wstring self_dir = ParentPath(GetSelfPath());
  options.dll_path = self_dir + L"\\nwncx_hg.dll";
  options.target_exe = ResolveDefaultEePath();

  bool passthrough = false;
  for (int i = 1; i < argc; ++i) {
    std::wstring arg = argv[i];
    if (passthrough) {
      options.extra_args.push_back(arg);
    } else if (arg == L"--") {
      passthrough = true;
    } else if (arg == L"--help" || arg == L"-h") {
      PrintUsage();
      ExitProcess(0);
    } else if (arg == L"--list-servers") {
      options.list_servers = true;
    } else if (arg == L"--dry-run") {
      options.dry_run = true;
    } else if (arg == L"--inject-test") {
      options.inject_test = true;
    } else if (arg == L"--driver-only") {
      options.driver_only = true;
    } else if (arg == L"--keep-movies") {
      options.skip_startup_movies = false;
    } else if (arg == L"--auto-use-allow-unuseable") {
      options.auto_use_allow_unuseable = true;
    } else if (arg == L"--auto-use-transition-click") {
      options.auto_use_transition_click = true;
    } else if (arg == L"--auto-trigger-walk-probe") {
      options.auto_trigger_walk_probe = true;
    } else if (arg == L"--auto-door-open-first") {
      options.auto_door_open_first = true;
    } else if (arg == L"--auto-open-inventory") {
      options.auto_open_inventory = true;
    } else if ((arg == L"--ee" || arg == L"--dll" || arg == L"--server" || arg == L"--connect" || arg == L"--password" ||
                arg == L"--diamond-account" || arg == L"--diamond-config-root" || arg == L"--auto-character" ||
                arg == L"--auto-use-object-id" || arg == L"--auto-use-object-type" ||
                arg == L"--auto-use-object-name" || arg == L"--auto-use-object-delay" ||
                arg == L"--auto-use-object-second-id" || arg == L"--auto-use-object-second-type" ||
                arg == L"--auto-use-object-second-name" || arg == L"--auto-use-object-second-delay" ||
                arg == L"--auto-use-area-id" || arg == L"--auto-door-transition-delay-ms" ||
                arg == L"--auto-door-transition-second-delay-ms" ||
                arg == L"--auto-open-inventory-delay-ms") && i + 1 < argc) {
      std::wstring value = argv[++i];
      if (arg == L"--ee") {
        options.target_exe = value;
      } else if (arg == L"--dll") {
        options.dll_path = value;
      } else if (arg == L"--connect") {
        options.connect_address = value;
      } else if (arg == L"--password") {
        options.auto_password = value;
        options.auto_password_source = L"command-line";
        options.password_explicit = true;
      } else if (arg == L"--diamond-account") {
        options.diamond_account = value;
      } else if (arg == L"--diamond-config-root") {
        options.diamond_config_root = value;
      } else if (arg == L"--auto-character") {
        options.auto_character = value;
      } else if (arg == L"--auto-use-object-id") {
        options.auto_use_object_id = value;
      } else if (arg == L"--auto-use-object-type") {
        options.auto_use_object_type = value;
      } else if (arg == L"--auto-use-object-name") {
        options.auto_use_object_name = value;
      } else if (arg == L"--auto-use-object-delay") {
        options.auto_use_object_delay_seconds = value;
      } else if (arg == L"--auto-use-object-second-id") {
        options.auto_use_object_second_id = value;
      } else if (arg == L"--auto-use-object-second-type") {
        options.auto_use_object_second_type = value;
      } else if (arg == L"--auto-use-object-second-name") {
        options.auto_use_object_second_name = value;
      } else if (arg == L"--auto-use-object-second-delay") {
        options.auto_use_object_second_delay_seconds = value;
      } else if (arg == L"--auto-use-area-id") {
        options.auto_use_area_id = value;
      } else if (arg == L"--auto-door-transition-delay-ms") {
        options.auto_door_transition_delay_ms = value;
      } else if (arg == L"--auto-door-transition-second-delay-ms") {
        options.auto_door_transition_second_delay_ms = value;
      } else if (arg == L"--auto-open-inventory-delay-ms") {
        options.auto_open_inventory_delay_ms = value;
      } else {
        if (LooksLikeAddress(value)) {
          options.server_id.clear();
          options.server_address = value;
        } else {
          options.server_id = value;
          options.server_address.clear();
        }
      }
    } else {
      fwprintf(stderr, L"Unknown or incomplete argument: %ls\n", arg.c_str());
      PrintUsage();
      ExitProcess(2);
    }
  }

  if (options.server_address.empty()) {
    const hgbridge::HgServer* server = hgbridge::FindServerById(options.server_id);
    if (server == nullptr) {
      fwprintf(stderr, L"Unknown HG server id: %ls\n", options.server_id.c_str());
      fwprintf(stderr, L"Use --list-servers to see known shortcuts.\n");
      ExitProcess(2);
    }
    options.server_address = std::wstring(server->address);
    options.server_host = std::wstring(server->ip);
    options.server_port = server->port;
  } else if (!ParseEndpoint(options.server_address, &options.server_host, &options.server_port)) {
    fwprintf(stderr, L"Invalid endpoint for --server: %ls\n", options.server_address.c_str());
    fwprintf(stderr, L"Expected HOST:PORT, for example 158.69.144.21:5131.\n");
    ExitProcess(2);
  }

  if (!options.server_host.empty() && options.server_port != 0) {
    options.server_address = FormatEndpoint(options.server_host, options.server_port);
  }

  if (!options.connect_address.empty()) {
    std::wstring connect_host;
    unsigned short connect_port = 0;
    if (!ParseEndpoint(options.connect_address, &connect_host, &connect_port)) {
      fwprintf(stderr, L"Invalid endpoint for --connect: %ls\n", options.connect_address.c_str());
      fwprintf(stderr, L"Expected HOST:PORT, for example 192.168.1.103:5121.\n");
      ExitProcess(2);
    }
    options.connect_address = FormatEndpoint(connect_host, connect_port);
    options.server_host = connect_host;
    options.server_port = connect_port;
  }

  options.target_exe = FullPath(options.target_exe);
  options.dll_path = FullPath(options.dll_path);
  return options;
}

std::wstring BuildCommandLine(const Options& options, bool redact_password = false) {
  std::wstring command = QuoteArg(options.target_exe);
  if (!options.server_host.empty()) {
    command += L" +connect ";
    command += QuoteArg(options.server_host);
  }
  if (!options.auto_password.empty()) {
    command += L" +password ";
    command += QuoteArg(redact_password ? L"<redacted>" : options.auto_password);
  }
  for (const auto& arg : options.extra_args) {
    command += L" ";
    command += QuoteArg(arg);
  }
  return command;
}

void SetOptionalBridgeEnvironmentVariable(const wchar_t* name, const std::wstring& value) {
  if (!value.empty()) {
    SetEnvironmentVariableW(name, value.c_str());
  }
}

void SetBridgeEnvironment(
    const Options& options,
    const std::wstring& workspace_root,
    const std::wstring& asset_bundle_root,
    const std::wstring& diamond_cdkey_path,
    const std::wstring& diamond_player_path) {
  SetEnvironmentVariableW(L"HG_BRIDGE_SERVER_ID", options.server_id.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_SERVER_ADDRESS", options.server_address.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_SERVER_HOST", options.server_host.c_str());
  const std::wstring server_port = options.server_port != 0 ? std::to_wstring(options.server_port) : L"";
  SetEnvironmentVariableW(L"HG_BRIDGE_SERVER_PORT", server_port.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_PASSWORD", options.auto_password.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_PASSWORD_SOURCE", options.auto_password_source.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_WORKSPACE", workspace_root.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_ASSET_BUNDLE", asset_bundle_root.c_str());
  if (!asset_bundle_root.empty()) {
    const std::wstring diamond_root = JoinPath(asset_bundle_root, L"diamond");
    const std::wstring hg_asset_root = JoinPath(asset_bundle_root, L"hg-gui");
    SetEnvironmentVariableW(L"HG_BRIDGE_DIAMOND_ROOT", diamond_root.c_str());
    SetEnvironmentVariableW(L"HG_BRIDGE_HG_ASSET_ROOT", hg_asset_root.c_str());
  }
  SetEnvironmentVariableW(L"HG_BRIDGE_DIAMOND_ACCOUNT", options.diamond_account.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_DIAMOND_CDKEY_PATH", diamond_cdkey_path.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_DIAMOND_PLAYER_PATH", diamond_player_path.c_str());
  SetEnvironmentVariableW(L"HG_BRIDGE_DRIVER_ONLY", options.driver_only ? L"1" : nullptr);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_CHARACTER", options.auto_character);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_OBJECT_ID", options.auto_use_object_id);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_ID", options.auto_use_object_id);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_OBJECT_TYPE", options.auto_use_object_type);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_TYPE", options.auto_use_object_type);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_OBJECT_NAME", options.auto_use_object_name);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_NAME", options.auto_use_object_name);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS", options.auto_use_object_delay_seconds);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_DELAY", options.auto_use_object_delay_seconds);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID", options.auto_use_object_second_id);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE2_ID", options.auto_use_object_second_id);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE", options.auto_use_object_second_type);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE2_TYPE", options.auto_use_object_second_type);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME", options.auto_use_object_second_name);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE2_NAME", options.auto_use_object_second_name);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS", options.auto_use_object_second_delay_seconds);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE2_DELAY", options.auto_use_object_second_delay_seconds);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_USE_AREA_ID", options.auto_use_area_id);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_AREA_ID", options.auto_use_area_id);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS", options.auto_door_transition_delay_ms);
  SetOptionalBridgeEnvironmentVariable(L"HG_BRIDGE_AUTO_DOOR_DELAY_MS", options.auto_door_transition_delay_ms);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_DOOR_TRANSITION_SECOND_DELAY_MS", options.auto_door_transition_second_delay_ms);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_DOOR2_DELAY_MS", options.auto_door_transition_second_delay_ms);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS", options.auto_open_inventory_delay_ms);
  SetOptionalBridgeEnvironmentVariable(
      L"HG_BRIDGE_AUTO_INVENTORY_DELAY_MS", options.auto_open_inventory_delay_ms);
  if (options.auto_use_allow_unuseable) {
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE", L"1");
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_ALLOW_UNUSEABLE", L"1");
  }
  if (options.auto_use_transition_click) {
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_USE_TRANSITION_CLICK", L"1");
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_TRANSITION_CLICK", L"1");
  }
  if (options.auto_trigger_walk_probe) {
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE", L"1");
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_TRIGGER_WALK", L"1");
  }
  if (options.auto_door_open_first) {
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_DOOR_OPEN_FIRST", L"1");
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_DOOR_OPEN", L"1");
  }
  if (options.auto_open_inventory) {
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_OPEN_INVENTORY", L"1");
    SetEnvironmentVariableW(L"HG_BRIDGE_AUTO_INVENTORY_OPEN", L"1");
  }
  if (options.driver_only) {
    SetEnvironmentVariableW(L"HG_BRIDGE_DISABLE_AUTO_CONNECT", nullptr);
  }
}

bool EqualsIgnoreCase(std::wstring_view left, std::wstring_view right) {
  if (left.size() != right.size()) {
    return false;
  }
  return CompareStringOrdinal(
             left.data(),
             static_cast<int>(left.size()),
             right.data(),
             static_cast<int>(right.size()),
             TRUE) == CSTR_EQUAL;
}

uint8_t* FindRemoteModuleBase(DWORD process_id, const std::wstring& module_path) {
  const std::wstring expected_path = FullPath(module_path);

  for (int attempt = 0; attempt < 40; ++attempt) {
    UniqueHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id));
    if (!snapshot) {
      Sleep(50);
      continue;
    }

    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (Module32FirstW(snapshot.get(), &entry)) {
      do {
        const std::wstring actual_path = FullPath(entry.szExePath);
        if (EqualsIgnoreCase(actual_path, expected_path)) {
          return entry.modBaseAddr;
        }
        entry.dwSize = sizeof(entry);
      } while (Module32NextW(snapshot.get(), &entry));
    }

    Sleep(50);
  }

  return nullptr;
}

uintptr_t GetExportRva(const std::wstring& dll_path, const char* export_name) {
  HMODULE module = LoadLibraryExW(dll_path.c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
  if (module == nullptr) {
    throw std::runtime_error("LoadLibraryExW for export lookup failed");
  }

  FARPROC export_address = GetProcAddress(module, export_name);
  if (export_address == nullptr) {
    FreeLibrary(module);
    throw std::runtime_error("bridge DLL does not export HgBridgeInit");
  }

  uintptr_t rva = reinterpret_cast<uintptr_t>(export_address) - reinterpret_cast<uintptr_t>(module);
  FreeLibrary(module);
  return rva;
}

void CallRemoteBridgeInit(HANDLE process, uint8_t* remote_module_base, uintptr_t init_rva) {
  auto remote_init = reinterpret_cast<LPTHREAD_START_ROUTINE>(remote_module_base + init_rva);
  UniqueHandle thread(CreateRemoteThread(process, nullptr, 0, remote_init, nullptr, 0, nullptr));
  if (!thread) {
    const DWORD error = GetLastError();
    fwprintf(stderr, L"CreateRemoteThread(HgBridgeInit) failed: %ls\n", Win32Message(error).c_str());
    throw std::runtime_error("CreateRemoteThread(HgBridgeInit) failed");
  }

  DWORD wait = WaitForSingleObject(thread.get(), 30000);
  if (wait != WAIT_OBJECT_0) {
    throw std::runtime_error("remote HgBridgeInit timed out");
  }

  DWORD exit_code = 0;
  if (!GetExitCodeThread(thread.get(), &exit_code) || exit_code == 0) {
    throw std::runtime_error("remote HgBridgeInit failed");
  }
}

void InjectDll(HANDLE process, const std::wstring& dll_path) {
  const uintptr_t init_rva = GetExportRva(dll_path, "HgBridgeInit");
  const size_t bytes = (dll_path.size() + 1) * sizeof(wchar_t);
  void* remote_path = VirtualAllocEx(process, nullptr, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (remote_path == nullptr) {
    throw std::runtime_error("VirtualAllocEx failed");
  }

  SIZE_T written = 0;
  if (!WriteProcessMemory(process, remote_path, dll_path.c_str(), bytes, &written) || written != bytes) {
    VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
    throw std::runtime_error("WriteProcessMemory failed");
  }

  HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
  auto load_library = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(kernel32, "LoadLibraryW"));
  if (load_library == nullptr) {
    VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
    throw std::runtime_error("GetProcAddress(LoadLibraryW) failed");
  }

  UniqueHandle thread(CreateRemoteThread(process, nullptr, 0, load_library, remote_path, 0, nullptr));
  if (!thread) {
    const DWORD error = GetLastError();
    VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
    fwprintf(stderr, L"CreateRemoteThread failed: %ls\n", Win32Message(error).c_str());
    throw std::runtime_error("CreateRemoteThread failed");
  }

  DWORD wait = WaitForSingleObject(thread.get(), 30000);
  if (wait != WAIT_OBJECT_0) {
    VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);
    throw std::runtime_error("remote LoadLibraryW timed out");
  }

  VirtualFreeEx(process, remote_path, 0, MEM_RELEASE);

  uint8_t* const remote_module_base = FindRemoteModuleBase(GetProcessId(process), dll_path);
  if (remote_module_base == nullptr) {
    throw std::runtime_error("loaded bridge DLL was not found in remote module list");
  }

  CallRemoteBridgeInit(process, remote_module_base, init_rva);
}

int Run(Options options) {
  if (options.list_servers) {
    ListServers();
    return 0;
  }

  if (!FileExists(options.target_exe)) {
    fwprintf(stderr, L"EE nwmain.exe not found: %ls\n", options.target_exe.c_str());
    return 2;
  }
  if (!FileExists(options.dll_path)) {
    fwprintf(stderr, L"Bridge DLL not found: %ls\n", options.dll_path.c_str());
    return 2;
  }

  try {
    RequireAmd64(L"Target EXE", options.target_exe);
    RequireAmd64(L"Bridge DLL", options.dll_path);
  } catch (const std::exception& ex) {
    fwprintf(stderr, L"Architecture validation failed: %S\n", ex.what());
    return 2;
  }

  const std::wstring workspace_root = ResolveWorkspaceRoot();
  const std::wstring asset_bundle_root = ResolveAssetBundleRoot(options);
  const std::wstring diamond_cdkey_path =
      ResolveDiamondAccountFile(options, workspace_root, L".nwncdkey.ini", L"nwncdkey.ini");
  const std::wstring diamond_player_path =
      ResolveDiamondAccountFile(options, workspace_root, L".nwnplayer.ini", L"nwnplayer.ini");

  if (!options.password_explicit) {
    std::string diamond_password;
    if (ReadDiamondProfilePasswordFromFile(diamond_player_path, &diamond_password)) {
      options.auto_password = AsciiToWide(diamond_password);
      options.auto_password_source = options.diamond_account.empty()
          ? L"Diamond profile"
          : L"Diamond account " + options.diamond_account;
    } else {
      options.auto_password = L"a";
      options.auto_password_source = L"default";
    }
  }

  const std::wstring command_line = BuildCommandLine(options);
  const std::wstring working_dir = ParentPath(options.target_exe);
  const std::wstring display_command_line = BuildCommandLine(options, true);

  wprintf(L"Target: %ls\n", options.target_exe.c_str());
  wprintf(L"DLL:    %ls\n", options.dll_path.c_str());
  wprintf(L"Server: %ls %ls\n", options.server_id.c_str(), options.server_address.c_str());
  if (!options.connect_address.empty()) {
    wprintf(L"Connect override: %ls\n", options.connect_address.c_str());
  }
  wprintf(L"Host:   %ls\n", options.server_host.c_str());
  wprintf(L"Port:   %hu\n", options.server_port);
  wprintf(L"Driver only: %ls\n", options.driver_only ? L"yes" : L"no");
  wprintf(L"Diamond account: %ls\n", options.diamond_account.empty() ? L"<workspace>" : options.diamond_account.c_str());
  wprintf(L"Diamond player source: %ls\n", diamond_player_path.empty() ? L"<not found>" : diamond_player_path.c_str());
  wprintf(L"Diamond CD key source: %ls\n", diamond_cdkey_path.empty() ? L"<not found>" : diamond_cdkey_path.c_str());
  wprintf(L"Asset bundle: %ls%ls\n",
      asset_bundle_root.c_str(),
      DirectoryExists(asset_bundle_root) ? L"" : L" <not found>");
  wprintf(L"Diamond asset root: %ls\n", JoinPath(asset_bundle_root, L"diamond").c_str());
  wprintf(L"HG asset root: %ls\n", JoinPath(asset_bundle_root, L"hg-gui").c_str());
  wprintf(
      L"Auto password: length=%zu source=%ls\n",
      options.auto_password.size(),
      options.auto_password_source.c_str());
  if (!options.auto_character.empty()) {
    wprintf(L"Auto character: %ls\n", options.auto_character.c_str());
  }
  if (!options.auto_use_object_id.empty() || !options.auto_use_object_type.empty() ||
      !options.auto_use_object_name.empty() || !options.auto_use_object_second_id.empty() ||
      !options.auto_use_object_second_type.empty() || !options.auto_use_object_second_name.empty()) {
    wprintf(
        L"Auto-use: stage0 id='%ls' type='%ls' name='%ls' delay='%ls'; stage1 id='%ls' type='%ls' name='%ls' delay='%ls'\n",
        options.auto_use_object_id.c_str(),
        options.auto_use_object_type.c_str(),
        options.auto_use_object_name.c_str(),
        options.auto_use_object_delay_seconds.c_str(),
        options.auto_use_object_second_id.c_str(),
        options.auto_use_object_second_type.c_str(),
        options.auto_use_object_second_name.c_str(),
        options.auto_use_object_second_delay_seconds.c_str());
  }
  if (options.auto_open_inventory) {
    wprintf(
        L"Auto inventory: yes delay='%ls'\n",
        options.auto_open_inventory_delay_ms.empty() ? L"<default>" : options.auto_open_inventory_delay_ms.c_str());
  }
  wprintf(L"Startup movies: %ls\n", options.skip_startup_movies ? L"disable before launch" : L"leave unchanged");
  wprintf(L"Cmd:    %ls\n", display_command_line.c_str());

  if (options.dry_run) {
    return 0;
  }

  SetBridgeEnvironment(options, workspace_root, asset_bundle_root, diamond_cdkey_path, diamond_player_path);
  try {
    const std::wstring ee_user_root = ResolveEeUserRoot();
    if (options.driver_only) {
      wprintf(L"EE identity: driver-only mode left EE CD key files unchanged\n");
    } else if (StageDiamondCdKeyFile(diamond_cdkey_path, ee_user_root)) {
      wprintf(L"EE identity: staged Diamond CD key file for account %ls in %ls\n",
          options.diamond_account.empty() ? L"<workspace>" : options.diamond_account.c_str(),
          ee_user_root.c_str());
    } else {
      wprintf(L"EE identity: Diamond CD key file not found; leaving EE key state unchanged\n");
    }

    std::string player_name;
    if (ImportDiamondPlayerNameFromFile(diamond_player_path, &player_name)) {
      wprintf(L"EE identity: set player name to %ls\n", AsciiToWide(player_name).c_str());
    } else {
      wprintf(L"EE identity: Diamond player name not found; leaving EE player name unchanged\n");
    }

    const std::wstring settings_path = UpdateEeSettingsPort(options.server_port);
    wprintf(L"EE port: set server.net.port=%hu in %ls\n", options.server_port, settings_path.c_str());
    const std::string auto_password_utf8 = WideToUtf8(options.auto_password);
    UpdateEeSettingsStringValue("server.login", "player-password", auto_password_utf8);
    const std::wstring ini_path = ResolveEeIniPath();
    UpdateIniValue(ini_path, "Profile", "Password", auto_password_utf8);
    UpdateIniValue(ini_path, "Profile", "Remember Password", "1");
    UpdateIniValue(ini_path, "Server Options", "PlayerPassword", auto_password_utf8);
    wprintf(
        L"EE login: set player password length=%zu in %ls and %ls\n",
        options.auto_password.size(),
        settings_path.c_str(),
        ini_path.c_str());
    if (options.skip_startup_movies) {
      UpdateEeSettingsBoolValue("graphics.movies", "enabled", false);
      UpdateEeSettingsBoolValue("graphics.movies.intro", "enabled", false);
      const std::wstring movie_settings_path = UpdateEeSettingsBoolValue("graphics.intro.splash", "enabled", false);
      wprintf(L"EE startup: disabled movies, intro movies, and splash in %ls\n", movie_settings_path.c_str());
    } else {
      wprintf(L"EE startup: leaving movie settings unchanged\n");
    }
  } catch (const std::exception& ex) {
    fwprintf(stderr, L"Failed to update EE launch settings: %S\n", ex.what());
    return 1;
  }

  STARTUPINFOW startup{};
  startup.cb = sizeof(startup);
  PROCESS_INFORMATION process_info{};
  std::wstring mutable_command = command_line;

  if (!CreateProcessW(
          options.target_exe.c_str(),
          mutable_command.data(),
          nullptr,
          nullptr,
          FALSE,
          CREATE_SUSPENDED,
          nullptr,
          working_dir.c_str(),
          &startup,
          &process_info)) {
    fwprintf(stderr, L"CreateProcessW failed: %ls\n", Win32Message(GetLastError()).c_str());
    return 1;
  }

  UniqueHandle process(process_info.hProcess);
  UniqueHandle thread(process_info.hThread);

  try {
    InjectDll(process.get(), options.dll_path);
  } catch (const std::exception& ex) {
    fwprintf(stderr, L"Injection failed: %S\n", ex.what());
    TerminateProcess(process.get(), 1);
    return 1;
  }

  if (options.inject_test) {
    TerminateProcess(process.get(), 0);
    WaitForSingleObject(process.get(), 5000);
    wprintf(L"Injection test completed pid=%lu; terminated before resuming main thread\n", process_info.dwProcessId);
    return 0;
  }

  ResumeThread(thread.get());
  wprintf(L"Launched pid=%lu and injected nwncx_hg.dll\n", process_info.dwProcessId);
  return 0;
}

}  // namespace

int wmain(int argc, wchar_t** argv) {
  try {
    Options options = ParseArgs(argc, argv);
    return Run(options);
  } catch (const std::exception& ex) {
    fwprintf(stderr, L"Fatal error: %S\n", ex.what());
    return 1;
  }
}
