#pragma once

#include <array>
#include <string_view>

namespace hgbridge {

struct HgServer {
  std::wstring_view id;
  std::wstring_view address;
  std::wstring_view ip;
  unsigned short port;
};

inline constexpr std::array<HgServer, 16> kHgServers = {{
    {L"111", L"158.69.144.21:5121", L"158.69.144.21", 5121},
    {L"211", L"158.69.144.21:5131", L"158.69.144.21", 5131},
    {L"212", L"158.69.144.21:5132", L"158.69.144.21", 5132},
    {L"213", L"158.69.144.21:5133", L"158.69.144.21", 5133},
    {L"214", L"158.69.144.21:5134", L"158.69.144.21", 5134},
    {L"215", L"158.69.144.21:5135", L"158.69.144.21", 5135},
    {L"311", L"158.69.144.21:5141", L"158.69.144.21", 5141},
    {L"312", L"158.69.144.21:5142", L"158.69.144.21", 5142},
    {L"313", L"158.69.144.21:5143", L"158.69.144.21", 5143},
    {L"314", L"158.69.144.21:5144", L"158.69.144.21", 5144},
    {L"315", L"158.69.144.21:5145", L"158.69.144.21", 5145},
    {L"411", L"158.69.144.21:5151", L"158.69.144.21", 5151},
    {L"412", L"158.69.144.21:5152", L"158.69.144.21", 5152},
    {L"413", L"158.69.144.21:5153", L"158.69.144.21", 5153},
    {L"414", L"158.69.144.21:5154", L"158.69.144.21", 5154},
    {L"415", L"158.69.144.21:5155", L"158.69.144.21", 5155},
}};

inline const HgServer* FindServerById(std::wstring_view id) {
  for (const auto& server : kHgServers) {
    if (server.id == id) {
      return &server;
    }
  }
  return nullptr;
}

inline const HgServer* FindServerByAddress(std::wstring_view address) {
  for (const auto& server : kHgServers) {
    if (server.address == address) {
      return &server;
    }
  }
  return nullptr;
}

inline bool IsKnownHgPort(unsigned short port) {
  for (const auto& server : kHgServers) {
    if (server.port == port) {
      return true;
    }
  }
  return false;
}

}  // namespace hgbridge
