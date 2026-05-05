#include <winsock2.h>
#include <windows.h>

#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <intrin.h>
#include <exception>
#include <string>
#include <utility>
#include <vector>

#include "HgServers.h"

namespace {

constexpr wchar_t kBridgeVersion[] = L"0.1.16";
constexpr uint16_t kEeBuildField = 0x0003;
constexpr uint16_t kDiamondBncsBuildField = 0x0003;
constexpr uint16_t kSinfarBncsBuildField = 0x05F8;
constexpr uint32_t kNwn169PrivateBuild = 8109;
constexpr size_t kConnectToSessionActiveOffset = 0x28A56A8;
constexpr size_t kDirectConnectStatePasswordOffset = 0x130;
constexpr size_t kEeClientModuleLoadRva = 0x7CD2C0;
constexpr size_t kEeClientModuleLoadResourcesRva = 0x7CDD20;
constexpr size_t kEeClientModuleLoadResourceCountReturnRva = 0x7CD55F;
constexpr size_t kEeClientModuleLoadLegacyTailRva = 0x7CD6E8;
constexpr size_t kEeClientAreaLoadBeginRva = 0x7DBA70;
constexpr size_t kEeClientAreaLoadEndRva = 0x7DEA0D;
constexpr size_t kEeClientAreaLoadNameStringFlagReturnRva = 0x7DC063;
constexpr size_t kEeClientAreaPostStaticSoundCountReturnRva = 0x7DDDFA;
constexpr size_t kEeClientAreaPostStaticCreatureCountReturnRva = 0x7DDF02;
constexpr size_t kEeClientServerToPlayerMessageBeginRva = 0x7A41A0;
constexpr size_t kEeClientServerToPlayerMessageEndRva = 0x7A4C26;
constexpr size_t kEeClientInputChangeDoorStateRva = 0x7BF860;
constexpr size_t kEeClientInputUseObjectRva = 0x7C07C0;
constexpr size_t kEeClientInputWalkToWaypointRva = 0x7C0970;
constexpr size_t kEeClientSendPlayerToServerMessageRva = 0x7C25C0;
constexpr size_t kEeClientLiveGameObjectUpdateBeginRva = 0x79BCE0;
constexpr size_t kEeClientLiveGameObjectUpdateEndRva = 0x79C044;
constexpr size_t kEeClientLiveGameObjectUpdateSubOpcodeReturnRva = 0x79BE0D;
constexpr size_t kEeClientLiveGameObjectUpdateMaterialShaderParamCountReturnRva = 0x79C44C;
constexpr size_t kEeClientLiveObjectUpdateMaskReturnRva = 0x7B8478;
constexpr size_t kEeClientLiveCreatureUpdateTailByte0ReturnRva = 0x785341;
constexpr size_t kEeClientLiveCreatureUpdateTailByte0PairReturnRva = 0x785356;
constexpr size_t kEeClientLiveCreatureUpdateTailByte1SingleReturnRva = 0x7853C4;
constexpr size_t kEeClientLiveCreatureUpdateTailByte1ReturnRva = 0x785417;
constexpr size_t kEeClientLiveCreatureUpdateTailByte2ReturnRva = 0x78542C;
constexpr size_t kEeClientLiveCreatureUpdateTailDwordReturnRva = 0x7854E1;
constexpr size_t kEeClientLiveCreatureUpdateTailWordReturnRva = 0x7854F1;
constexpr size_t kEeClientLiveCreatureUpdateTailBool1ReturnRva = 0x7854FD;
constexpr size_t kEeClientLiveCreatureUpdateTailBool2ReturnRva = 0x785509;
constexpr size_t kEeClientLiveCreatureUpdateTailBool3ReturnRva = 0x78552F;
constexpr size_t kEeAppManagerGetGameObjectRva = 0x6ABB60;
constexpr size_t kEeClientGetInventoryPanelRva = 0x6ABBE0;
constexpr size_t kEeClientOpenInventoryPanelRva = 0x857CD0;
constexpr size_t kEeNwcObjectSetAppearanceStateRva = 0x764300;
constexpr size_t kEeClientLiveDoorAddRva = 0x796DD0;
constexpr size_t kEeClientLiveTriggerAddRva = 0x7B1670;
constexpr size_t kEeClientLivePlaceableAddRva = 0x7A7800;
constexpr size_t kEeClientPlaceableSetUseableRva = 0x7668F0;
constexpr size_t kEeClientLiveDoorNameLocStringFlagReturnRva = 0x796F54;
constexpr size_t kEeClientLivePlaceableNameLocStringFlagReturnRva = 0x7A792E;
constexpr size_t kEeClientLiveTriggerPostNamePrimaryBoolReturnRva = 0x7B17DC;
constexpr size_t kEeClientLiveTriggerPostNameSecondaryBoolReturnRva = 0x7B17E9;
constexpr size_t kEeClientLiveTriggerPostNameCursorByteReturnRva = 0x7B1802;
constexpr size_t kEeClientLiveTriggerPostNameOptionalBoolReturnRva = 0x7B1811;
constexpr size_t kEeClientLiveDoorPostNameAbsentBoolReturnRva = 0x79700B;
constexpr size_t kEeClientLiveDoorPostNameFlag1ReturnRva = 0x797024;
constexpr size_t kEeClientLiveDoorPostNameFlag2ReturnRva = 0x79702F;
constexpr size_t kEeClientLiveDoorPostNameFlag3ReturnRva = 0x79703A;
constexpr size_t kEeClientLiveDoorPostNameFlag4ReturnRva = 0x797045;
constexpr size_t kEeClientLiveDoorUpdateEeOnlyExtraFlagReturnRva = 0x7978AC;
constexpr size_t kEeClientLiveDoorUpdateNameModeFlagReturnRva = 0x79794E;
constexpr size_t kEeClientLivePlaceablePostNameAbsentBoolReturnRva = 0x7A7A2A;
constexpr size_t kEeClientLivePlaceablePostNameFlag1ReturnRva = 0x7A7A5D;
constexpr size_t kEeClientLivePlaceablePostNameFlag2ReturnRva = 0x7A7A80;
constexpr size_t kEeClientLivePlaceablePostNameFlag3ReturnRva = 0x7A7A8B;
constexpr size_t kEeClientLivePlaceablePostNameFlag4ReturnRva = 0x7A7A96;
constexpr size_t kEeClientLivePlaceablePostNameFlag5ReturnRva = 0x7A7AA1;
constexpr size_t kEeClientLivePlaceablePostNameFlag6ReturnRva = 0x7A7AAC;
constexpr size_t kEeClientLivePlaceablePostNameFlag7ReturnRva = 0x7A7AB7;
constexpr size_t kEeClientLivePlaceablePostNameFlag8ReturnRva = 0x7A7AC2;
constexpr size_t kEeClientLivePlaceablePostNameEeOnlyFinalFlagReturnRva = 0x7A7ACD;
constexpr size_t kEeClientLiveDoorUpdateNameStringReturnRva = 0x7979BF;
constexpr size_t kEeClientLiveGenericObjectUpdateTrailingWordReturnRva = 0x79CB6F;
constexpr size_t kEeClientItemAppearanceReadRva = 0x79FAC0;
constexpr size_t kEeClientLiveItemAppearanceExtendedArmorByteReturnRva = 0x79FD43;
constexpr size_t kEeClientLiveItemLegacyAbsentExtraBoolReturnRva = 0x76BF7A;
constexpr size_t kEeClientPlaceableUpdateFlag1ReturnRva = 0x7A85A0;
constexpr size_t kEeClientPlaceableUpdateFlag2ReturnRva = 0x7A85AB;
constexpr size_t kEeClientPlaceableUpdateFlag3ReturnRva = 0x7A85B6;
constexpr size_t kEeClientPlaceableUpdateFlag4ReturnRva = 0x7A85C1;
constexpr size_t kEeClientPlaceableUpdateFlag5ReturnRva = 0x7A85CC;
constexpr size_t kEeClientPlaceableUpdateNameModeFlagReturnRva = 0x7A8662;
constexpr size_t kEeClientPlaceableUpdateNameStringReturnRva = 0x7A86D3;
constexpr size_t kEeClientGameObjectUpdateBeginRva = 0x7A8C60;
constexpr size_t kEeClientGameObjectUpdateEndRva = 0x7AA172;
constexpr size_t kEeClientGameObjectUpdateEePlayerInfoFlagReturnRva = 0x7A9905;
constexpr size_t kEeClientGameObjectUpdateEePlayerInfoByteReturnRva = 0x7A991B;
constexpr size_t kEeClientGameObjectUpdateEePlayerInfoStringReturnRva = 0x7A9931;
constexpr size_t kEeClientGameObjectUpdateEePortraitWordReturnRva = 0x7A9AA3;
constexpr size_t kEeClientGameObjectUpdateUnderflowCheckReturnRva = 0x7AA11D;
constexpr size_t kEeClientServerToPlayerUnderflowCheckReturnRva = 0x7A4B3C;
constexpr size_t kEeClientReadCExoLocStringClientBeginRva = 0x9735F0;
constexpr size_t kEeClientReadCExoLocStringClientEndRva = 0x9737B4;
constexpr size_t kEeClientReadCExoLocStringClientTlkFlagReturnRva = 0x973668;
constexpr size_t kEeClientReadCExoLocStringClientStringReturnRva = 0x973789;
constexpr size_t kEeClientRawReadDwordRva = 0x9737C0;
constexpr size_t kEeClientRawReadDwordEndRva = 0x9737E8;
constexpr size_t kEeClientInventoryInitialObjectIdReturnRva = 0x79F222;
constexpr size_t kEeClientVisualTransformLerpFeatureReturnRva = 0x972C91;
constexpr size_t kEeClientVisualTransformMapFeatureReturnRva = 0x9731B1;
constexpr size_t kEeClientVisualTransformReadRva = 0x973160;
constexpr size_t kEeClientCreatureAddVisualTransformReturnRva = 0x77FA10;
constexpr size_t kEeVisualTransformTreeBuyHeadNodeRva = 0x368530;
constexpr size_t kEeClientSendChatTalkRva = 0x7BA7E0;
constexpr size_t kEePregameCharacterListModalCallbackRva = 0x713770;
constexpr size_t kEeDriverQuitConfirmationPromptRva = 0x6DE2C0;
constexpr size_t kEeDriverQuitConfirmationYesRva = 0x6D76B0;
constexpr size_t kEeServerSatisfiesBuildBridgeRva = 0x7C4AB0;
constexpr size_t kAbsoluteJumpSize = 12;
constexpr size_t kTrampolineReturnJumpSize = 13;
constexpr uint32_t kEeHakAliasAddFlags = 0x01D905C0;
constexpr uint32_t kEeHakInstallAliasAddFlags = 0x01C9C380;
constexpr uint32_t kEeLegacyOverrideDirectoryAddFlags = 0x05E69EC0;
constexpr uint32_t kDiamondBaseKeyTableAddFlags = 0x0007A120;

struct BridgeState {
  HMODULE module = nullptr;
  volatile LONG initialized = 0;
};

struct CExoStringView {
  char* data = nullptr;
  uint32_t length = 0;
  uint32_t capacity = 0;
};

constexpr size_t kCResRefTextBytes = 16;
constexpr size_t kCResRefStorageBytes = kCResRefTextBytes + 1;

struct CResRefView {
  char data[kCResRefStorageBytes]{};
};

static_assert(sizeof(CResRefView) == kCResRefStorageBytes, "CResRef is 16 chars plus a trailing terminator byte");

struct Vector3f {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

struct PendingLegacyGamePacket {
  uint32_t connection_id = 0;
  uint32_t flags = 0;
  std::vector<unsigned char> bytes;
};

struct DiamondTexturePackMount {
  const wchar_t* file_name;
  const char* alias_name;
  uint32_t priority;
};

struct TrackedResObject {
  void* object = nullptr;
  CResRefView resref{};
  uint16_t type = 0;
  DWORD tick = 0;
};

struct ObjectNameRegistryEntry {
  uint32_t object_id = 0;
  uint8_t object_type = 0;
  int useable = -1;
  uint32_t transition_target = 0x7F000000;
  bool has_position = false;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  bool has_bounds = false;
  float min_x = 0.0f;
  float min_y = 0.0f;
  float min_z = 0.0f;
  float max_x = 0.0f;
  float max_y = 0.0f;
  float max_z = 0.0f;
  std::string name;
  DWORD tick = 0;
};

struct LiveObjectPointerRegistryEntry {
  uint32_t object_id = 0;
  uint8_t object_type = 0;
  void* object = nullptr;
  DWORD tick = 0;
};

using StartAddressTranslationFn = void(__fastcall*)(void*, const CExoStringView*);
using CExoStringFromCharFn = CExoStringView*(__fastcall*)(CExoStringView*, const char*);
using CExoStringInitFromCharArrayFn = CExoStringView*(__fastcall*)(CExoStringView*, const char*, uint32_t);
using CExoStringDestructorFn = void(__fastcall*)(CExoStringView*);
using CExoEncryptStringFn = CExoStringView*(__fastcall*)(void*, CExoStringView*, const CExoStringView*);
using CAppManagerConnectToServerFn = void(__fastcall*)(void*, CExoStringView*, int);
using GetPublicPartFromCdKeyFn = CExoStringView*(__fastcall*)(void*, CExoStringView*, const CExoStringView*);
using AppManagerGetObjectByIdFn = void*(__fastcall*)(void*, uint32_t);
using NwcObjectAsClientObjectFn = void*(__fastcall*)(void*);
using NwcObjectSetVectorFn = void(__fastcall*)(void*, const Vector3f*);
using NwcObjectSetAppearanceStateFn = void(__fastcall*)(void*, uint16_t);
using DirectConnectStateUpdateFn = void(__fastcall*)(void*);
using StartEnumerateSessionsInternalFn = int(__fastcall*)(void*, uint32_t*, int, void*, int);
using OpenStandardConnectionInternalFn = int(__fastcall*)(void*, int, CExoStringView*, int);
using SendDirectMessageFn = int(__fastcall*)(void*, uint32_t, unsigned char*, uint32_t, uint32_t);
using ExoNetSendMessageFn = int(__fastcall*)(void*, uint32_t, unsigned char*, uint32_t, uint32_t);
using SetNetworkAddressDataFn = uint32_t*(__fastcall*)(void*, uint32_t*, const void*, int, const char*);
using HandleBnerMessageFn = int(__fastcall*)(void*, uint32_t, void*);
using NonWindowMessagesFn = int(__fastcall*)(void*, uint32_t, unsigned char*, uint32_t);
using SendToFn = int(WSAAPI*)(SOCKET, const char*, int, int, const sockaddr*, int);
using RecvFromFn = int(WSAAPI*)(SOCKET, char*, int, int, sockaddr*, int*);
using WsaGetLastErrorFn = int(WSAAPI*)();
using GetUdpPortFn = unsigned int(__fastcall*)(void*);
using SendBncrMessageFn = int(__fastcall*)(void*, uint32_t, unsigned char, unsigned int);
using SendBncsMessageFn = int(__fastcall*)(
    void*,
    uint32_t,
    unsigned char,
    int,
    const CExoStringView*,
    const CExoStringView*,
    const CExoStringView*);
using SendBnvsMessageFn = int(__fastcall*)(
    void*,
    const CExoStringView*,
    const CExoStringView*,
    const CExoStringView*);
using NetLayerWindowFrameReceiveFn = int64_t(__fastcall*)(void*, unsigned char*, uint32_t);
using NetLayerWindowUnpacketizeFullMessagesFn = int(__fastcall*)(void*, int);
using NetLayerInternalUncompressMessageFn = int(__fastcall*)(void*, uint32_t, unsigned char*, uint32_t);
using NetLayerInternalDisconnectFromSessionFn = int(__fastcall*)(void*);
using StartConnectToSessionInternalFn = int(__fastcall*)(
    void*,
    uint32_t,
    const CExoStringView*,
    int,
    const CExoStringView*,
    uint32_t,
    uint32_t,
    const CExoStringView*,
    const CExoStringView*,
    const CExoStringView*);
using StartConnectToSessionWrapperFn = int(__fastcall*)(
    void*,
    uint32_t,
    const CExoStringView*,
    int,
    const CExoStringView*,
    uint32_t,
    uint32_t,
    const CExoStringView*,
    const CExoStringView*,
    const CExoStringView*);
using ClientModuleFn = uint32_t(__fastcall*)(void*);
using ClientAreaLoadFn = uint32_t(__fastcall*)(void*, void*, void*);
using ServerToPlayerMessageFn = int64_t(__fastcall*)(void*, unsigned char*, uint32_t);
using ClientSendPlayerToServerMessageFn = int64_t(__fastcall*)(void*, unsigned char, unsigned char, unsigned char*, uint32_t);
using ClientInputChangeDoorStateFn = int64_t(__fastcall*)(void*, uint32_t, uint16_t);
using ClientInputUseObjectFn = int64_t(__fastcall*)(void*, uint32_t, int, int);
using ClientInputWalkToWaypointFn =
    int64_t(__fastcall*)(void*, uint32_t, float, float, float, unsigned char, int, int, unsigned char, uint32_t);
using ServerToPlayerGameObjectUpdateFn = int64_t(__fastcall*)(void*, unsigned char);
using LiveObjectAddHandlerFn = int64_t(__fastcall*)(void*);
using PlaceableSetUseableFn = void(__fastcall*)(void*, int);
using ClientSendChatTalkFn = int64_t(__fastcall*)(void*, CExoStringView*);
using ServerExoAppGetClientObjectByObjectIdFn = void*(__fastcall*)(void*, uint32_t);
using PlayerInventoryGuiSetOpenFn = void(__fastcall*)(void*, int, int);
using PlayerInventoryGuiSetOwnerFn = void(__fastcall*)(void*, uint32_t);
using ClientAppGetInventoryPanelFn = void*(__fastcall*)(void*);
using ClientInventoryPanelOpenFn = void(__fastcall*)(void*, int, int, uint32_t);
using CnwMessageLookupMajorMinorNameFn = const char*(__fastcall*)(unsigned char, unsigned char);
using VisualTransformReadFn = void*(__fastcall*)(void*, void*);
using VisualTransformTreeBuyHeadNodeFn = void*(__fastcall*)(void*);
using CnwMessageCreateWriteMessageFn = void(__fastcall*)(void*, uint32_t, uint32_t, int);
using CnwMessageGetWriteMessageFn = int(__fastcall*)(void*, unsigned char**, uint32_t*);
using CnwMessageWriteBoolFn = void(__fastcall*)(void*, int);
using CnwMessageWriteByteFn = void(__fastcall*)(void*, unsigned char, int, int);
using CnwMessageWriteDwordFn = void(__fastcall*)(void*, uint32_t, int);
using CnwMessageWriteIntFn = void(__fastcall*)(void*, int, int);
using CnwMessageWriteWordFn = void(__fastcall*)(void*, uint16_t, int);
using CnwMessageRawReadDwordFn = uint32_t(__fastcall*)(void*);
using TlkTableOpenFileFn = int(__fastcall*)(void*, CExoStringView*);
using TlkTableOpenFileAlternateFn = int(__fastcall*)(void*, const CExoStringView*);
using TlkTableFetchInternalFn = int(__fastcall*)(void*, uint32_t, void*, int);
using ServerSatisfiesBuildBridgeFn = int(__fastcall*)(void*, int, int, int);
using GetBaseItemFn = void*(__fastcall*)(void*, int);
using ItemAppearanceReadFn = int64_t(__fastcall*)(void*, uint32_t*, void*, void*, void*, uint8_t*, void*);
using CnwMessageFlagFn = int(__fastcall*)(void*);
using CnwMessageReadBoolFn = int(__fastcall*)(void*);
using CnwMessageReadCharFn = char(__fastcall*)(void*, int);
using CnwMessageReadByteFn = unsigned char(__fastcall*)(void*, int, int);
using CnwMessageReadShortFn = short(__fastcall*)(void*, int);
using CnwMessageReadWordFn = unsigned short(__fastcall*)(void*, int);
using CnwMessageReadDwordFn = uint32_t(__fastcall*)(void*, int);
using CnwMessageReadIntFn = int(__fastcall*)(void*, int);
using CnwMessageReadFloatFn = float(__fastcall*)(void*, float, int);
using CnwMessageReadCExoStringFn = CExoStringView*(__fastcall*)(void*, CExoStringView*, int);
using CnwMessageReadCResRefFn = CResRefView*(__fastcall*)(void*, CResRefView*, int);
using CnwMessageSetReadMessageFn = int(__fastcall*)(void*, unsigned char*, uint32_t, uint32_t, int);
using PregameCharacterListModalCallbackFn = int64_t(__fastcall*)(void*, int);
using PregameLeaveScreenFn = void(__fastcall*)(void*);
using GuiModalPanelDeactivateFn = void(__fastcall*)(void*, int);
using GuiPanelNoArgFn = void(__fastcall*)(void*);
using DriverQuitConfirmationPromptFn = void(__fastcall*)(void*);
using DriverQuitConfirmationYesFn = void(__fastcall*)(void*);
using QuitFn = void(__fastcall*)();
using ResManExistsFn = int(__fastcall*)(void*, const CResRefView*, uint16_t, uint32_t*);
using ResManGetResObjectFn = void*(__fastcall*)(void*, const CResRefView*, uint16_t);
using ResManSetResObjectFn = void(__fastcall*)(void*, const CResRefView*, uint16_t, void*);
using ResManDemandFn = void*(__fastcall*)(void*, void*);
using ResManAddFileFn = int(__fastcall*)(void*, const CExoStringView*, uint32_t);
using ResManAddResourceDirectoryFn = int(__fastcall*)(void*, const CExoStringView*, uint32_t, int, void*);
using ResManRemoveKeyTableFn = int(__fastcall*)(void*, const CExoStringView*, uint32_t, int);
using ResManClearOverridesFn = void(__fastcall*)(void*);
using ExoAliasListAddFn = void(__fastcall*)(void*, const CExoStringView*, const CExoStringView*);
using RulesReloadAllFn = void(__fastcall*)(void*);
using TileSetManagerGetTileSetFn = void*(__fastcall*)(void*, const CResRefView*);
using NwTileSetLoadTileSetFn = int(__fastcall*)(void*);
using NwsAreaSetTilesetFn = int(__fastcall*)(void*, const CResRefView*);

BridgeState g_state;
StartAddressTranslationFn g_start_address_translation_original = nullptr;
DirectConnectStateUpdateFn g_direct_connect_state_update_original = nullptr;
StartEnumerateSessionsInternalFn g_start_enumerate_sessions_internal = nullptr;
OpenStandardConnectionInternalFn g_open_standard_connection_internal_original = nullptr;
SendDirectMessageFn g_send_direct_message_original = nullptr;
ExoNetSendMessageFn g_exonet_send_message_original = nullptr;
SetNetworkAddressDataFn g_set_network_address_data_original = nullptr;
HandleBnerMessageFn g_handle_bner_message_original = nullptr;
HandleBnerMessageFn g_handle_bncr_message_original = nullptr;
HandleBnerMessageFn g_handle_bnvr_message_original = nullptr;
HandleBnerMessageFn g_handle_bnvs_message_original = nullptr;
HandleBnerMessageFn g_handle_bnxr_message_original = nullptr;
NonWindowMessagesFn g_non_window_messages_original = nullptr;
SendToFn g_sendto_original = nullptr;
RecvFromFn g_recvfrom_original = nullptr;
WsaGetLastErrorFn g_wsa_get_last_error = nullptr;
SendBncrMessageFn g_send_bncr_message_original = nullptr;
SendBncsMessageFn g_send_bncs_message_original = nullptr;
SendBnvsMessageFn g_send_bnvs_message_original = nullptr;
NetLayerWindowFrameReceiveFn g_netlayer_window_frame_receive_original = nullptr;
NetLayerWindowUnpacketizeFullMessagesFn g_netlayer_window_unpacketize_full_messages_original = nullptr;
NetLayerInternalUncompressMessageFn g_netlayer_internal_uncompress_message_original = nullptr;
NetLayerInternalDisconnectFromSessionFn g_netlayer_internal_disconnect_from_session_original = nullptr;
StartConnectToSessionInternalFn g_start_connect_to_session_internal_original = nullptr;
ClientModuleFn g_client_module_load_original = nullptr;
ClientModuleFn g_client_module_load_resources_original = nullptr;
ClientAreaLoadFn g_client_area_load_original = nullptr;
ServerToPlayerMessageFn g_server_to_player_message_original = nullptr;
ClientSendPlayerToServerMessageFn g_client_send_player_to_server_message_original = nullptr;
ServerToPlayerGameObjectUpdateFn g_live_game_object_update_original = nullptr;
ServerToPlayerGameObjectUpdateFn g_game_object_update_original = nullptr;
LiveObjectAddHandlerFn g_live_door_add_original = nullptr;
LiveObjectAddHandlerFn g_live_trigger_add_original = nullptr;
LiveObjectAddHandlerFn g_live_placeable_add_original = nullptr;
PlaceableSetUseableFn g_placeable_set_useable_original = nullptr;
ClientSendChatTalkFn g_client_send_chat_talk = nullptr;
CnwMessageLookupMajorMinorNameFn g_lookup_major_minor_name = nullptr;
VisualTransformReadFn g_visual_transform_read_original = nullptr;
VisualTransformTreeBuyHeadNodeFn g_visual_transform_tree_buy_head_node = nullptr;
CnwMessageCreateWriteMessageFn g_message_create_write_message_original = nullptr;
CnwMessageGetWriteMessageFn g_message_get_write_message_original = nullptr;
CnwMessageWriteBoolFn g_message_write_bool_original = nullptr;
CnwMessageWriteByteFn g_message_write_byte_original = nullptr;
CnwMessageWriteDwordFn g_message_write_dword_original = nullptr;
CnwMessageWriteIntFn g_message_write_int_original = nullptr;
CnwMessageWriteWordFn g_message_write_word_original = nullptr;
CnwMessageRawReadDwordFn g_raw_read_dword_original = nullptr;
TlkTableOpenFileFn g_tlk_table_open_file = nullptr;
TlkTableOpenFileAlternateFn g_tlk_table_open_file_alternate = nullptr;
TlkTableFetchInternalFn g_tlk_table_fetch_internal_original = nullptr;
ServerSatisfiesBuildBridgeFn g_server_satisfies_build_bridge_original = nullptr;
GetBaseItemFn g_get_base_item = nullptr;
ItemAppearanceReadFn g_item_appearance_read_original = nullptr;
CnwMessageFlagFn g_message_read_overflow_original = nullptr;
CnwMessageFlagFn g_message_read_underflow_original = nullptr;
CnwMessageReadBoolFn g_message_read_bool_original = nullptr;
CnwMessageReadCharFn g_message_read_char_original = nullptr;
CnwMessageReadByteFn g_message_read_byte_original = nullptr;
CnwMessageReadShortFn g_message_read_short_original = nullptr;
CnwMessageReadWordFn g_message_read_word_original = nullptr;
CnwMessageReadDwordFn g_message_read_dword_original = nullptr;
CnwMessageReadIntFn g_message_read_int_original = nullptr;
CnwMessageReadFloatFn g_message_read_float_original = nullptr;
CnwMessageReadCExoStringFn g_message_read_cexo_string_original = nullptr;
CnwMessageReadCResRefFn g_message_read_cresref_original = nullptr;
CnwMessageSetReadMessageFn g_message_set_read_message = nullptr;
ResManExistsFn g_resman_exists_original = nullptr;
ResManGetResObjectFn g_resman_get_res_object_original = nullptr;
ResManSetResObjectFn g_resman_set_res_object_original = nullptr;
ResManDemandFn g_resman_demand_original = nullptr;
ResManAddFileFn g_resman_add_encapsulated_resource_file_original = nullptr;
ResManAddFileFn g_resman_add_fixed_key_table_file_original = nullptr;
ResManAddResourceDirectoryFn g_resman_add_resource_directory_original = nullptr;
ResManRemoveKeyTableFn g_resman_remove_key_table_original = nullptr;
ResManClearOverridesFn g_resman_clear_overrides_original = nullptr;
TileSetManagerGetTileSetFn g_tileset_manager_get_tileset_original = nullptr;
NwTileSetLoadTileSetFn g_nw_tileset_load_tileset_original = nullptr;
NwsAreaSetTilesetFn g_nws_area_set_tileset_original = nullptr;
DriverQuitConfirmationPromptFn g_driver_quit_confirmation_prompt_original = nullptr;
volatile LONG g_address_translation_hook_active = 0;
volatile LONG g_target_address_translation_seen = 0;
volatile LONG g_direct_connect_state_observations = 0;
volatile LONG g_direct_connect_password_observations = 0;
volatile LONG g_session_peer_observations = 0;
volatile LONG g_open_standard_observations = 0;
volatile LONG g_send_direct_observations = 0;
volatile LONG g_exonet_send_observations = 0;
volatile LONG g_exonet_failed_m_observations = 0;
volatile LONG g_outbound_game_packet_detail_observations = 0;
volatile LONG g_object_name_registry_observations = 0;
volatile LONG g_live_object_pointer_registry_observations = 0;
volatile LONG g_set_network_address_observations = 0;
volatile LONG g_bner_observations = 0;
volatile LONG g_bncr_handle_observations = 0;
volatile LONG g_bnvr_handle_observations = 0;
volatile LONG g_bnvs_handle_observations = 0;
volatile LONG g_bnxr_handle_observations = 0;
volatile LONG g_non_window_message_observations = 0;
volatile LONG g_sendto_observations = 0;
volatile LONG g_sendto_hg_m_observations = 0;
volatile LONG g_sendto_failed_m_observations = 0;
volatile LONG g_packet_dump_send_observations = 0;
volatile LONG g_packet_dump_recv_observations = 0;
volatile LONG g_recvfrom_observations = 0;
volatile LONG g_recvfrom_hg_observations = 0;
volatile LONG g_bncr_send_observations = 0;
volatile LONG g_bncs_send_observations = 0;
volatile LONG g_bnvs_send_observations = 0;
volatile LONG g_legacy_bncr_challenge_observations = 0;
volatile LONG g_legacy_bncr_non_challenge_observations = 0;
volatile LONG g_legacy_bnvs_send_observations = 0;
volatile LONG g_legacy_game_packet_gate_begin_observations = 0;
volatile LONG g_legacy_game_packet_queue_observations = 0;
volatile LONG g_legacy_game_packet_suppressed_observations = 0;
volatile LONG g_legacy_game_packet_replay_observations = 0;
volatile LONG g_legacy_bnvr_response_observations = 0;
volatile LONG g_start_connect_observations = 0;
volatile LONG g_start_connect_wrapper_observations = 0;
volatile LONG g_client_module_load_observations = 0;
volatile LONG g_client_module_load_resources_observations = 0;
volatile LONG g_client_area_load_observations = 0;
volatile LONG g_server_satisfies_build_bridge_observations = 0;
volatile LONG g_module_message_read_observations = 0;
volatile LONG g_area_message_read_observations = 0;
volatile LONG g_module_message_bounds_observations = 0;
volatile LONG g_server_to_player_message_observations = 0;
volatile LONG g_driver_server_dispatch_log_observations = 0;
volatile LONG g_driver_frame_receive_observations = 0;
volatile LONG g_driver_unpacketize_observations = 0;
volatile LONG g_driver_uncompress_observations = 0;
volatile LONG g_driver_disconnect_from_session_observations = 0;
volatile LONG g_driver_quit_confirmation_prompt_observations = 0;
volatile LONG g_driver_auto_quit_confirm_observations = 0;
volatile LONG g_driver_auto_quit_confirm_triggered = 0;
volatile LONG g_server_chat_message_observations = 0;
volatile LONG g_area_load_dispatch_observations = 0;
volatile LONG g_area_misc_dispatch_observations = 0;
volatile LONG g_loadscreen_dispatch_observations = 0;
volatile LONG g_object_update_dispatch_observations = 0;
volatile LONG g_live_object_dispatch_observations = 0;
volatile LONG g_live_object_packet_summary_observations = 0;
volatile LONG g_party_packet_diagnostic_observations = 0;
volatile LONG g_object_useable_dispatch_observations = 0;
volatile LONG g_hg_password_gate_observations = 0;
volatile LONG g_hg_password_auto_speak_state = 0;
volatile LONG g_hg_password_auto_speak_attempts = 0;
volatile LONG g_hg_password_auto_speak_resolve_observations = 0;
volatile LONG g_hg_password_auto_speak_skip_observations = 0;
volatile LONG g_hg_password_auto_speak_retry_scheduled = 0;
void* g_hg_password_auto_speak_retry_message = nullptr;
LONG g_hg_password_auto_speak_retry_dispatch = 0;
ULONGLONG g_hg_password_auto_speak_retry_due_tick = 0;
volatile LONG g_live_message_read_observations = 0;
volatile LONG g_live_message_bounds_observations = 0;
volatile LONG g_live_game_object_update_observations = 0;
volatile LONG g_live_object_subopcode_observations = 0;
volatile LONG g_live_message_update_observations = 0;
volatile LONG g_live_message_raw_read_observations = 0;
volatile LONG g_diamond_archive_mount_state = 0;
volatile LONG g_diamond_archive_mount_observations = 0;
volatile LONG g_client_to_server_message_observations = 0;
volatile LONG g_client_to_server_area_loaded_observations = 0;
volatile LONG g_client_to_server_area_loaded_suppressed_observations = 0;
volatile LONG g_synthetic_area_loaded_observations = 0;
volatile LONG g_synthetic_area_loaded_send_depth = 0;
volatile LONG g_synthetic_loadbar_start_observations = 0;
volatile LONG g_synthetic_loadbar_end_observations = 0;
volatile LONG g_real_loadbar_start_observations = 0;
volatile LONG g_real_loadbar_end_observations = 0;
volatile LONG g_loadbar_outstanding = 0;
volatile LONG g_last_loadbar_stall_event_id = 2;
volatile LONG g_last_loadbar_start_dispatch = 0;
volatile LONG g_legacy_quickbar_translate_observations = 0;
volatile LONG g_legacy_quickbar_skip_observations = 0;
volatile LONG g_legacy_quickbar_active_prop_write_observations = 0;
volatile LONG g_legacy_quickbar_item_write_observations = 0;
volatile LONG g_legacy_quickbar_general_write_observations = 0;
volatile LONG g_legacy_quickbar_write_packet_observations = 0;
volatile LONG g_legacy_tlk_load_observations = 0;
volatile LONG g_legacy_tlk_alias_observations = 0;
volatile LONG g_tlk_fetch_observations = 0;
volatile LONG g_tlk_badstrref_observations = 0;
volatile LONG g_tlk_fetch_failure_observations = 0;
std::string g_legacy_dialog_tlk_loaded_path;
std::string g_legacy_custom_tlk_loaded_path;
volatile LONG g_message_create_write_observations = 0;
volatile LONG g_message_get_write_observations = 0;
volatile LONG g_legacy_object_update_field_observations = 0;
volatile LONG g_legacy_object_update_fragment_tail_observations = 0;
volatile LONG g_legacy_live_object_update_tail_observations = 0;
volatile LONG g_legacy_visual_transform_skip_observations = 0;
volatile LONG g_legacy_live_material_shader_param_skip_observations = 0;
volatile LONG g_legacy_live_extended_armor_table_skip_observations = 0;
volatile LONG g_legacy_live_creature_tail_byte_skip_observations = 0;
volatile LONG g_legacy_live_creature_tail_exact_end_suppressions = 0;
volatile LONG g_legacy_live_item_appearance_read_observations = 0;
volatile LONG g_legacy_live_item_extra_bool_skip_observations = 0;
volatile LONG g_legacy_live_inline_locstring_observations = 0;
volatile LONG g_legacy_live_bad_inline_locstring_skip_observations = 0;
volatile LONG g_legacy_live_bad_inline_locstring_rewind_observations = 0;
volatile LONG g_locstring_client_flag_observations = 0;
volatile LONG g_legacy_live_oversized_cexo_string_skip_observations = 0;
volatile LONG g_legacy_live_misaligned_cexo_string_recover_observations = 0;
volatile LONG g_legacy_live_subopcode_resync_observations = 0;
volatile LONG g_legacy_live_short_add_skip_observations = 0;
volatile LONG g_legacy_live_short_add_locstring_parse_observations = 0;
volatile LONG g_legacy_live_name_locstring_flag_observations = 0;
volatile LONG g_legacy_live_short_add_locstring_flag_observations = 0;
volatile LONG g_legacy_live_short_add_name_consume_observations = 0;
volatile LONG g_legacy_live_short_add_tail_bool_observations = 0;
volatile LONG g_legacy_live_placeable_add_absent_bool_skip_observations = 0;
volatile LONG g_legacy_live_placeable_add_optional_target_bool_observations = 0;
volatile LONG g_legacy_live_placeable_update_absent_bool_skip_observations = 0;
volatile LONG g_legacy_live_placeable_bool_field_observations = 0;
volatile LONG g_legacy_live_placeable_add_diagnostics_observations = 0;
volatile LONG g_legacy_live_placeable_add_bool_map_observations = 0;
volatile LONG g_legacy_live_update_mask_translate_observations = 0;
volatile LONG g_legacy_live_update_mask_observe_observations = 0;
volatile LONG g_legacy_live_update_absent_name_bool_skip_observations = 0;
volatile LONG g_legacy_live_update_absent_name_string_skip_observations = 0;
volatile LONG g_legacy_live_placeable_update_tail_observations = 0;
volatile LONG g_legacy_live_placeable_update_tail_apply_observations = 0;
volatile LONG g_legacy_live_scalar_tail_observations = 0;
volatile LONG g_legacy_live_scalar_tail_apply_observations = 0;
volatile LONG g_legacy_live_door_update_extra_bool_skip_observations = 0;
volatile LONG g_legacy_live_placeable_add_useable_remap_observations = 0;
volatile LONG g_legacy_live_generic_update_trailing_word_skip_observations = 0;
volatile LONG g_legacy_live_trigger_add_skip_observations = 0;
volatile LONG g_legacy_live_trigger_add_parse_observations = 0;
volatile LONG g_legacy_live_trigger_add_locstring_flag_observations = 0;
volatile LONG g_legacy_live_trigger_add_name_consume_observations = 0;
volatile LONG g_legacy_live_trigger_add_primary_bool_observations = 0;
volatile LONG g_legacy_live_trigger_add_synthetic_bool_observations = 0;
volatile LONG g_legacy_live_trigger_add_synthetic_cursor_observations = 0;
volatile LONG g_live_trigger_add_observations = 0;
volatile LONG g_placeable_set_useable_observations = 0;
volatile LONG g_legacy_live_update_name_recover_observations = 0;
volatile LONG g_legacy_inventory_equip_object_remap_observations = 0;
volatile LONG g_legacy_inventory_equip_raw_read_probe_observations = 0;
volatile LONG g_resman_lookup_observations = 0;
volatile LONG g_resman_add_file_observations = 0;
volatile LONG g_resman_add_directory_observations = 0;
volatile LONG g_resman_remove_key_table_observations = 0;
volatile LONG g_resman_clear_overrides_observations = 0;
volatile LONG g_resman_runtime_miss_observations = 0;
volatile LONG g_resman_runtime_hit_observations = 0;
volatile LONG g_resman_diagnostics_install_state = 0;
volatile LONG g_world_resource_diagnostics_install_state = 0;
volatile LONG g_resman_demand_observations = 0;
volatile LONG g_resman_runtime_demand_observations = 0;
volatile LONG g_resman_runtime_non_optional_demand_miss_observations = 0;
volatile LONG g_resman_runtime_targeted_demand_observations = 0;
volatile LONG g_legacy_live_parser_boundary_guard_observations = 0;
volatile LONG g_legacy_live_parser_boundary_guard_suppressions = 0;

struct PendingLegacyLiveInlineLocStringSkip {
  bool active = false;
  void* message = nullptr;
  LONG dispatch = 0;
  uint32_t buffer_ptr = 0;
  uint32_t fragments_ptr = 0;
  uint8_t fragment_bit_ptr = 0;
  uint8_t fragment_bit_size = 0;
  uint32_t declared_length = 0;
  uint32_t available_length = 0;
  uint32_t boundary_delta = 0;
};

thread_local PendingLegacyLiveInlineLocStringSkip g_pending_legacy_live_inline_locstring_skip;

struct PendingLegacyLiveParserBoundaryGuard {
  bool active = false;
  void* message = nullptr;
  LONG dispatch = 0;
  uint32_t source_buffer_ptr = 0;
  uint32_t boundary_buffer_ptr = 0;
  uint32_t declared_length = 0;
  uint32_t available_length = 0;
  uint32_t suppressed_reads = 0;
};

thread_local PendingLegacyLiveParserBoundaryGuard g_pending_legacy_live_parser_boundary_guard;
thread_local LONG g_last_legacy_live_backward_resync_dispatch = 0;
thread_local uint32_t g_last_legacy_live_backward_resync_from = 0;
thread_local uint32_t g_last_legacy_live_backward_resync_target = 0;

struct PendingLegacyLiveShortAddParse {
  bool active = false;
  void* message = nullptr;
  LONG dispatch = 0;
  uint8_t object_type = 0;
  uint32_t name_buffer_ptr = 0;
  uint32_t boundary_buffer_ptr = 0;
  uint32_t boundary_delta = 0;
  uint32_t fragments_ptr = 0;
  uint8_t fragment_bit_ptr = 0;
  uint8_t fragment_bit_size = 0;
  uint32_t strref = 0;
  bool locstring_flag_forced = false;
  bool name_consumed = false;
};

thread_local PendingLegacyLiveShortAddParse g_pending_legacy_live_short_add_parse;

struct PendingLegacyLivePlaceableAddDiagnostics {
  bool active = false;
  void* message = nullptr;
  LONG dispatch = 0;
  uint32_t object_id = 0;
  uint32_t record_start_buffer_ptr = 0;
  uint32_t boundary_buffer_ptr = 0;
  uint32_t tail_offset = 0;
  uint8_t appearance_type = 0;
  uint16_t appearance_id = 0;
  uint16_t bodybag = 0;
  uint32_t name_length = 0;
  std::string name_preview;
  std::array<int, 10> bool_values{};
  std::array<bool, 10> bool_seen{};
  std::array<bool, 10> bool_consumed{};
};

thread_local PendingLegacyLivePlaceableAddDiagnostics g_pending_legacy_live_placeable_add_diagnostics;

struct PendingLegacyLiveUpdateAbsentNameParse {
  bool active = false;
  void* message = nullptr;
  LONG dispatch = 0;
  uint8_t object_type = 0;
  uint32_t object_id = 0;
  uint32_t update_mask = 0;
  uint32_t record_start_buffer_ptr = 0;
  uint32_t boundary_buffer_ptr = 0;
  uint32_t fragments_ptr = 0;
  uint8_t fragment_bit_ptr = 0;
  uint8_t fragment_bit_size = 0;
};

thread_local PendingLegacyLiveUpdateAbsentNameParse g_pending_legacy_live_update_absent_name_parse;

struct PendingLegacyLiveTriggerAddParse {
  bool active = false;
  void* message = nullptr;
  LONG dispatch = 0;
  uint32_t object_id = 0;
  uint32_t name_buffer_ptr = 0;
  uint32_t boundary_buffer_ptr = 0;
  uint32_t boundary_delta = 0;
  uint32_t fragments_ptr = 0;
  uint8_t fragment_bit_ptr = 0;
  uint8_t fragment_bit_size = 0;
  uint32_t strref = 0;
  uint8_t legacy_primary_flag = 0;
  uint8_t vertex_count = 0;
  float height = 0.0f;
  bool locstring_flag_forced = false;
  bool name_consumed = false;
  bool legacy_primary_flag_consumed = false;
};

thread_local PendingLegacyLiveTriggerAddParse g_pending_legacy_live_trigger_add_parse;
volatile LONG g_tileset_manager_get_observations = 0;
volatile LONG g_tileset_load_observations = 0;
volatile LONG g_area_set_tileset_observations = 0;
volatile LONG g_legacy_hak_mount_observations = 0;
volatile LONG g_legacy_override_mount_observations = 0;
volatile LONG g_diamond_cd_keys_seeded = 0;
volatile LONG g_diamond_cd_key_public_diagnostics_logged = 0;
volatile ULONGLONG g_resman_runtime_diagnostics_until_tick = 0;
thread_local int g_client_module_load_depth = 0;
thread_local int g_client_module_resource_load_depth = 0;
thread_local int g_resman_hook_depth = 0;
thread_local int g_resman_demand_hook_depth = 0;
thread_local int g_tileset_hook_depth = 0;
thread_local int g_server_to_player_message_depth = 0;
thread_local LONG g_current_server_to_player_message_observation = 0;
thread_local unsigned char g_current_server_to_player_major = 0;
thread_local unsigned char g_current_server_to_player_minor = 0;
thread_local uint32_t g_current_server_to_player_size = 0;
thread_local bool g_current_server_to_player_live_opcode_known = false;
thread_local unsigned char g_current_server_to_player_live_opcode = 0;
thread_local int g_live_game_object_update_depth = 0;
thread_local int g_game_object_update_depth = 0;
bool g_module_read_diagnostics_enabled = false;
bool g_area_read_diagnostics_enabled = false;
bool g_live_message_read_diagnostics_enabled = false;
std::wstring g_auto_connect_host;
unsigned short g_auto_connect_port = 0;
uint8_t g_auto_connect_ip[4]{};
bool g_auto_connect_ip_known = false;
std::string g_auto_connect_password;
std::string g_auto_connect_password_source;
std::string g_auto_character_resref;
volatile LONG g_auto_character_state = 0;
volatile LONG g_auto_character_observations = 0;
volatile LONG g_auto_character_update_responses = 0;
volatile LONG g_auto_character_initial_request_scheduled = 0;
volatile LONG g_auto_character_fallback_play_scheduled = 0;
volatile LONG g_auto_character_ui_close_observations = 0;
PVOID volatile g_auto_character_last_client_message_context = nullptr;
volatile LONG g_driver_auto_inventory_open_scheduled = 0;
volatile LONG g_driver_auto_inventory_open_observations = 0;
volatile LONG g_driver_auto_inventory_local_open_observations = 0;
volatile LONG g_driver_player_self_candidate_observations = 0;
volatile LONG g_driver_player_self_object_id_bits = 0;
ULONGLONG g_driver_auto_inventory_open_due_tick = 0;
LONG g_driver_auto_inventory_open_trigger_observation = 0;
wchar_t g_driver_auto_inventory_open_reason[96]{};
std::vector<std::string> g_diamond_cd_keys;
std::vector<std::string> g_diamond_public_cd_keys;
SRWLOCK g_legacy_bncr_lock = SRWLOCK_INIT;
unsigned char g_legacy_bncr_status = 0;
std::string g_legacy_bncr_cd_key_challenge;
std::string g_legacy_bncr_mst_password_challenge;
SRWLOCK g_legacy_game_packet_gate_lock = SRWLOCK_INIT;
bool g_legacy_game_packet_gate_active = false;
bool g_legacy_bnvr_accepted = false;
bool g_legacy_game_packet_gate_rejected = false;
uint32_t g_legacy_game_packet_gate_connection_id = 0;
std::vector<PendingLegacyGamePacket> g_legacy_pending_game_packets;
SRWLOCK g_legacy_module_hak_lock = SRWLOCK_INIT;
std::vector<std::string> g_last_legacy_module_haks;
std::string g_last_legacy_module_resref;
SRWLOCK g_tracked_res_objects_lock = SRWLOCK_INIT;
TrackedResObject g_tracked_res_objects[512]{};
volatile LONG g_tracked_res_object_next = 0;
SRWLOCK g_object_name_registry_lock = SRWLOCK_INIT;
std::vector<ObjectNameRegistryEntry> g_object_name_registry;
SRWLOCK g_live_object_pointer_registry_lock = SRWLOCK_INIT;
std::vector<LiveObjectPointerRegistryEntry> g_live_object_pointer_registry;
volatile LONG g_auto_use_object_state = 0;
volatile LONG g_auto_use_object_observations = 0;
volatile LONG g_auto_use_object_stage = 0;
volatile LONG g_auto_use_object_replay_miss_observations = 0;
volatile LONG g_trigger_transition_click_rewrite_observations = 0;
uint32_t g_current_area_object_id = 0x7F000000;
LONG g_pending_auto_use_object_stage = 0;
uint32_t g_pending_auto_use_object_id = 0;
uint8_t g_pending_auto_use_object_type = 0;
ULONGLONG g_pending_auto_use_due_tick = 0;
volatile LONG g_pending_auto_use_door_open_phase = 0;
std::string g_pending_auto_use_object_name;

void BeginRuntimeResourceDiagnostics(const wchar_t* reason, DWORD seconds);
bool EnsureResourceManagerDiagnosticsInstalled(const wchar_t* reason);
bool EnsureWorldResourceDiagnosticsInstalled(const wchar_t* reason);
bool InstallResourceManagerDiagnostics();
bool InstallWorldResourceDiagnostics();
void RemountLegacyModuleResourcesAfterLoad(const wchar_t* reason);
void DumpResManKeyTables(void* resman, const wchar_t* reason);
std::wstring CResRefToWide(const CResRefView* value);
std::wstring FormatResType(uint16_t type);
const wchar_t* FormatResManKeyTableKind(uint32_t kind);
bool LooksLikeLegacyResourcePath(const std::wstring& path);
std::wstring FormatLiveDispatchContext();
void ClearLegacyGamePacketGate(uint32_t connection_id, const wchar_t* reason);
void RejectLegacyGamePacketGate(uint32_t connection_id, const wchar_t* reason);
int64_t __fastcall HookedClientSendPlayerToServerMessage(
    void* message,
    unsigned char major,
    unsigned char minor,
    unsigned char* payload,
    uint32_t payload_size);

struct NetPeerView {
  uint16_t family = 0;
  uint16_t port_network = 0;
  uint8_t address[4]{};
  uint8_t padding[8]{};
};

std::wstring GetModulePath(HMODULE module) {
  std::wstring path(MAX_PATH, L'\0');
  DWORD size = 0;
  for (;;) {
    size = GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size()));
    if (size == 0) {
      return L"";
    }
    if (size < path.size() - 1) {
      path.resize(size);
      return path;
    }
    path.resize(path.size() * 2);
  }
}

std::wstring ParentPath(const std::wstring& path) {
  const size_t slash = path.find_last_of(L"\\/");
  if (slash == std::wstring::npos) {
    return L".";
  }
  return path.substr(0, slash);
}

std::wstring GetEnvironmentString(const wchar_t* name) {
  DWORD needed = GetEnvironmentVariableW(name, nullptr, 0);
  if (needed == 0) {
    return L"";
  }
  std::wstring value(needed, L'\0');
  DWORD written = GetEnvironmentVariableW(name, value.data(), needed);
  if (written == 0) {
    return L"";
  }
  value.resize(written);
  return value;
}

std::wstring LowerAscii(std::wstring value) {
  for (wchar_t& ch : value) {
    if (ch >= L'A' && ch <= L'Z') {
      ch = static_cast<wchar_t>(ch - L'A' + L'a');
    }
  }
  return value;
}

bool IsTruthyEnvironmentFlag(const wchar_t* name) {
  const std::wstring value = LowerAscii(GetEnvironmentString(name));
  return value == L"1" || value == L"true" || value == L"yes" || value == L"on";
}

const wchar_t* GetEnvironmentAliasName(const wchar_t* name) {
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_ID") == 0) {
    return L"HG_BRIDGE_AUTO_USE_ID";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_TYPE") == 0) {
    return L"HG_BRIDGE_AUTO_USE_TYPE";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_NAME") == 0) {
    return L"HG_BRIDGE_AUTO_USE_NAME";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS") == 0) {
    return L"HG_BRIDGE_AUTO_USE_DELAY";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID") == 0) {
    return L"HG_BRIDGE_AUTO_USE2_ID";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE") == 0) {
    return L"HG_BRIDGE_AUTO_USE2_TYPE";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME") == 0) {
    return L"HG_BRIDGE_AUTO_USE2_NAME";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS") == 0) {
    return L"HG_BRIDGE_AUTO_USE2_DELAY";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_AREA_ID") == 0) {
    return L"HG_BRIDGE_AUTO_AREA_ID";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE") == 0) {
    return L"HG_BRIDGE_AUTO_ALLOW_UNUSEABLE";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_TRANSITION_CLICK") == 0) {
    return L"HG_BRIDGE_AUTO_TRANSITION_CLICK";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE") == 0) {
    return L"HG_BRIDGE_AUTO_TRIGGER_WALK";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS") == 0) {
    return L"HG_BRIDGE_AUTO_DOOR_DELAY_MS";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_DOOR_TRANSITION_SECOND_DELAY_MS") == 0) {
    return L"HG_BRIDGE_AUTO_DOOR2_DELAY_MS";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_DOOR_OPEN_FIRST") == 0) {
    return L"HG_BRIDGE_AUTO_DOOR_OPEN";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_OPEN_INVENTORY") == 0) {
    return L"HG_BRIDGE_AUTO_INVENTORY_OPEN";
  }
  if (wcscmp(name, L"HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS") == 0) {
    return L"HG_BRIDGE_AUTO_INVENTORY_DELAY_MS";
  }
  return nullptr;
}

std::wstring GetEnvironmentStringWithAlias(const wchar_t* name) {
  std::wstring value = GetEnvironmentString(name);
  if (!value.empty()) {
    return value;
  }
  const wchar_t* const alias = GetEnvironmentAliasName(name);
  return alias != nullptr ? GetEnvironmentString(alias) : L"";
}

bool IsTruthyEnvironmentFlagWithAlias(const wchar_t* name) {
  const std::wstring value = LowerAscii(GetEnvironmentStringWithAlias(name));
  return value == L"1" || value == L"true" || value == L"yes" || value == L"on";
}

bool FileExistsWide(const std::wstring& path) {
  if (path.empty()) {
    return false;
  }
  const DWORD attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

std::string ToUtf8(const std::wstring& value) {
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

std::wstring NarrowToWide(const std::string& value) {
  if (value.empty()) {
    return {};
  }

  int needed = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0);
  UINT code_page = CP_UTF8;
  DWORD flags = MB_ERR_INVALID_CHARS;
  if (needed <= 0) {
    code_page = CP_ACP;
    flags = 0;
    needed = MultiByteToWideChar(code_page, flags, value.data(), static_cast<int>(value.size()), nullptr, 0);
  }
  if (needed <= 0) {
    return L"<decode failed>";
  }

  std::wstring result(static_cast<size_t>(needed), L'\0');
  MultiByteToWideChar(code_page, flags, value.data(), static_cast<int>(value.size()), result.data(), needed);
  return result;
}

bool TryCopyBytes(const char* source, uint32_t length, char* destination) {
  if (source == nullptr || destination == nullptr) {
    return false;
  }

  __try {
    std::memcpy(destination, source, length);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

std::wstring CExoStringToWide(const CExoStringView* value) {
  if (value == nullptr) {
    return L"<null CExoString>";
  }
  if (value->data == nullptr || value->length == 0) {
    return L"";
  }

  constexpr uint32_t kMaxLoggedLength = 512;
  const uint32_t length = std::min(value->length, kMaxLoggedLength);
  char buffer[kMaxLoggedLength]{};
  if (!TryCopyBytes(value->data, length, buffer)) {
    return L"<read failed>";
  }

  std::string text(buffer, buffer + length);
  std::wstring wide = NarrowToWide(text);
  if (value->length > kMaxLoggedLength) {
    wide += L"...";
  }
  return wide;
}

uint32_t SafeCExoStringLength(const CExoStringView* value) {
  if (value == nullptr) {
    return 0;
  }

  __try {
    return value->length;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return 0;
  }
}

void SetEmptyCExoString(CExoStringView* value) {
  if (value == nullptr) {
    return;
  }

  __try {
    value->data = nullptr;
    value->length = 0;
    value->capacity = 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}

std::string CExoStringToString(const CExoStringView* value, uint32_t max_length = 128) {
  if (value == nullptr || value->data == nullptr || value->length == 0) {
    return {};
  }

  const uint32_t length = std::min(value->length, max_length);
  std::string result(length, '\0');
  if (!TryCopyBytes(value->data, length, result.data())) {
    return {};
  }
  return result;
}

bool ParseUnsignedShort(const std::wstring& value, unsigned short* parsed) {
  if (parsed == nullptr || value.empty()) {
    return false;
  }

  wchar_t* end = nullptr;
  unsigned long number = wcstoul(value.c_str(), &end, 10);
  if (end == value.c_str() || *end != L'\0' || number > 0xFFFFul) {
    return false;
  }

  *parsed = static_cast<unsigned short>(number);
  return true;
}

bool ParseUnsigned32(const std::wstring& value, uint32_t* parsed) {
  if (parsed == nullptr || value.empty()) {
    return false;
  }

  wchar_t* end = nullptr;
  unsigned long number = wcstoul(value.c_str(), &end, 10);
  if (end == value.c_str() || *end != L'\0' || number > 0xFFFFFFFFul) {
    return false;
  }

  *parsed = static_cast<uint32_t>(number);
  return true;
}

bool ParseUnsigned32Flexible(const std::wstring& value, uint32_t* parsed) {
  if (parsed == nullptr || value.empty()) {
    return false;
  }

  int base = 10;
  const wchar_t* start = value.c_str();
  if (value.size() > 2 && value[0] == L'0' && (value[1] == L'x' || value[1] == L'X')) {
    base = 16;
  }

  wchar_t* end = nullptr;
  unsigned long number = wcstoul(start, &end, base);
  if (end == start || *end != L'\0' || number > 0xFFFFFFFFul) {
    return false;
  }

  *parsed = static_cast<uint32_t>(number);
  return true;
}

bool ParseIpv4Bytes(const std::wstring& value, uint8_t* bytes) {
  if (bytes == nullptr || value.empty()) {
    return false;
  }

  const wchar_t* cursor = value.c_str();
  for (int index = 0; index < 4; ++index) {
    if (*cursor == L'\0') {
      return false;
    }

    wchar_t* end = nullptr;
    unsigned long octet = wcstoul(cursor, &end, 10);
    if (end == cursor || octet > 255ul) {
      return false;
    }

    bytes[index] = static_cast<uint8_t>(octet);
    if (index < 3) {
      if (*end != L'.') {
        return false;
      }
      cursor = end + 1;
    } else if (*end != L'\0') {
      return false;
    }
  }

  return true;
}

unsigned short ByteSwap16(unsigned short value) {
  return static_cast<unsigned short>((value << 8) | (value >> 8));
}

template <typename Fn>
Fn ResolveMainExport(const char* name) {
  HMODULE main_module = GetModuleHandleW(nullptr);
  if (main_module == nullptr) {
    return nullptr;
  }
  return reinterpret_cast<Fn>(GetProcAddress(main_module, name));
}

template <typename Fn>
Fn ResolveMainRva(size_t rva) {
  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    return nullptr;
  }
  return reinterpret_cast<Fn>(base + rva);
}

bool InstallMessageReadBoolHook(const wchar_t* log_name);
bool InstallMessageReadWordHook(const wchar_t* log_name);
bool InstallMessageReadDwordHook(const wchar_t* log_name);
bool SafeReadGlobalPointer(void** global, void** value);
bool AddRuntimeAlias(const char* alias_name, const std::wstring& target_directory, const wchar_t* reason);
std::vector<std::wstring> GetDiamondHakDirectories();
uint32_t ReadU32LeRaw(const uint8_t* bytes);
std::wstring FormatMainExecutableAddress(void* address);
int CallAddEncapsulatedResourceFile(
    void* resman,
    ResManAddFileFn add_file,
    CExoStringFromCharFn construct_string,
    CExoStringDestructorFn destroy_string,
    const char* path_text,
    uint32_t flags,
    bool* raised_exception);

void AppendLogLine(const std::wstring& message) {
  const std::wstring module_path = GetModulePath(g_state.module);
  const std::wstring log_path = ParentPath(module_path) + L"\\nwncx_hg.log";

  HANDLE file = CreateFileW(
      log_path.c_str(),
      FILE_APPEND_DATA,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return;
  }

  SYSTEMTIME now{};
  GetLocalTime(&now);

  wchar_t prefix[64]{};
  swprintf_s(
      prefix,
      L"[%04u-%02u-%02u %02u:%02u:%02u.%03u] ",
      now.wYear,
      now.wMonth,
      now.wDay,
      now.wHour,
      now.wMinute,
      now.wSecond,
      now.wMilliseconds);

  const std::string line = ToUtf8(std::wstring(prefix) + message + L"\r\n");
  DWORD written = 0;
  WriteFile(file, line.data(), static_cast<DWORD>(line.size()), &written, nullptr);
  CloseHandle(file);
}

void LogFormat(const wchar_t* format, ...) {
  wchar_t buffer[8192]{};
  va_list args;
  va_start(args, format);
  _vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, format, args);
  va_end(args);
  AppendLogLine(buffer);
}

std::wstring FormatBytes(const uint8_t* bytes, size_t length) {
  if (bytes == nullptr || length == 0) {
    return L"";
  }

  std::wstring result;
  result.reserve(length * 3);
  for (size_t index = 0; index < length; ++index) {
    wchar_t byte_text[4]{};
    swprintf_s(byte_text, L"%02X", static_cast<unsigned int>(bytes[index]));
    if (!result.empty()) {
      result.push_back(L' ');
    }
    result += byte_text;
  }
  return result;
}

std::wstring FormatLimitedBytes(const uint8_t* bytes, size_t length, size_t max_length) {
  const size_t logged_length = std::min(length, max_length);
  std::wstring result = FormatBytes(bytes, logged_length);
  if (length > logged_length) {
    result += L" ...";
  }
  return result;
}

std::wstring FormatLiveObjectOpcode(bool known, unsigned char opcode) {
  if (!known) {
    return L"opcode=<none>";
  }

  const wchar_t printable = opcode >= 0x20 && opcode <= 0x7E ? static_cast<wchar_t>(opcode) : L'.';
  wchar_t text[32]{};
  swprintf_s(text, L"opcode='%c'/0x%02X", printable, static_cast<unsigned int>(opcode));
  return text;
}

bool PacketDumpEnabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_PACKET_DUMP");
}

bool DriverOnlyWinsockDiagnosticsEnabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_DRIVER_WINSOCK_DIAGNOSTICS");
}

bool IsAsciiPrintableOrSpace(unsigned char value) {
  return value == '\t' || value == '\r' || value == '\n' || (value >= 0x20 && value <= 0x7E);
}

std::string ExtractAsciiPacketText(const unsigned char* bytes, uint32_t length, size_t max_length) {
  if (bytes == nullptr || length == 0 || max_length == 0) {
    return {};
  }

  std::string result;
  result.reserve(std::min<size_t>(length, max_length));
  bool in_separator = false;
  for (uint32_t index = 0; index < length && result.size() < max_length; ++index) {
    const unsigned char value = bytes[index];
    if (IsAsciiPrintableOrSpace(value)) {
      char ch = static_cast<char>(value);
      if (ch == '\r' || ch == '\n' || ch == '\t') {
        ch = ' ';
      }
      result.push_back(ch);
      in_separator = false;
      continue;
    }

    if (!result.empty() && !in_separator && result.size() + 1 < max_length) {
      result.push_back('|');
      in_separator = true;
    }
  }

  while (!result.empty() && result.back() == '|') {
    result.pop_back();
  }
  return result;
}

bool PacketContainsAsciiNoCase(const unsigned char* bytes, uint32_t length, const char* needle) {
  if (bytes == nullptr || needle == nullptr || needle[0] == '\0') {
    return false;
  }

  const size_t needle_length = std::strlen(needle);
  if (needle_length == 0 || needle_length > length) {
    return false;
  }

  for (uint32_t offset = 0; offset + needle_length <= length; ++offset) {
    bool matches = true;
    for (size_t index = 0; index < needle_length; ++index) {
      unsigned char hay = bytes[offset + index];
      unsigned char want = static_cast<unsigned char>(needle[index]);
      if (hay >= 'A' && hay <= 'Z') {
        hay = static_cast<unsigned char>(hay - 'A' + 'a');
      }
      if (want >= 'A' && want <= 'Z') {
        want = static_cast<unsigned char>(want - 'A' + 'a');
      }
      if (hay != want) {
        matches = false;
        break;
      }
    }
    if (matches) {
      return true;
    }
  }
  return false;
}

std::string TrimAsciiWhitespace(std::string value) {
  const auto is_space = [](unsigned char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
  };

  while (!value.empty() && is_space(static_cast<unsigned char>(value.front()))) {
    value.erase(value.begin());
  }
  while (!value.empty() && is_space(static_cast<unsigned char>(value.back()))) {
    value.pop_back();
  }
  return value;
}

std::string LowerAscii(std::string value) {
  for (char& ch : value) {
    if (ch >= 'A' && ch <= 'Z') {
      ch = static_cast<char>(ch - 'A' + 'a');
    }
  }
  return value;
}

bool ReadSmallTextFile(const std::wstring& path, std::string* contents) {
  if (contents == nullptr) {
    return false;
  }

  HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }

  LARGE_INTEGER size{};
  if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 || size.QuadPart > 1024 * 1024) {
    CloseHandle(file);
    return false;
  }

  std::string buffer(static_cast<size_t>(size.QuadPart), '\0');
  DWORD read = 0;
  const BOOL ok = ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr);
  CloseHandle(file);
  if (!ok) {
    return false;
  }

  buffer.resize(read);
  *contents = std::move(buffer);
  return true;
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

bool DirectoryExists(const std::wstring& path) {
  const DWORD attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

std::wstring GetFinalDirectoryPath(const std::wstring& path) {
  HANDLE handle = CreateFileW(
      path.c_str(),
      FILE_READ_ATTRIBUTES,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      nullptr,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS,
      nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    return L"";
  }

  std::wstring result(1024, L'\0');
  DWORD length = GetFinalPathNameByHandleW(handle, result.data(), static_cast<DWORD>(result.size()), FILE_NAME_NORMALIZED);
  CloseHandle(handle);
  if (length == 0) {
    return L"";
  }
  if (length >= result.size()) {
    result.resize(length + 1, L'\0');
    handle = CreateFileW(
        path.c_str(),
        FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
      return L"";
    }
    length = GetFinalPathNameByHandleW(handle, result.data(), static_cast<DWORD>(result.size()), FILE_NAME_NORMALIZED);
    CloseHandle(handle);
    if (length == 0 || length >= result.size()) {
      return L"";
    }
  }
  result.resize(length);

  constexpr wchar_t kNtPrefix[] = L"\\\\?\\";
  if (result.rfind(kNtPrefix, 0) == 0) {
    result.erase(0, 4);
  }
  return result;
}

std::vector<std::wstring> ListDirectoryFiles(const std::wstring& directory, const std::wstring& pattern) {
  std::vector<std::wstring> files;
  if (!DirectoryExists(directory)) {
    return files;
  }

  WIN32_FIND_DATAW data{};
  const std::wstring search = JoinPath(directory, pattern);
  HANDLE find = FindFirstFileW(search.c_str(), &data);
  if (find == INVALID_HANDLE_VALUE) {
    return files;
  }

  do {
    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
      files.emplace_back(data.cFileName);
    }
  } while (FindNextFileW(find, &data));

  FindClose(find);
  std::sort(files.begin(), files.end(), [](const std::wstring& a, const std::wstring& b) {
    return LowerAscii(a) < LowerAscii(b);
  });
  return files;
}

void LogFileListChunks(const std::wstring& context, const std::vector<std::wstring>& files, size_t max_files) {
  if (context.empty() || files.empty() || max_files == 0) {
    return;
  }

  const size_t count = std::min(files.size(), max_files);
  constexpr size_t kChunkSize = 12;
  for (size_t start = 0; start < count; start += kChunkSize) {
    const size_t end = std::min(count, start + kChunkSize);
    std::wstring line;
    for (size_t index = start; index < end; ++index) {
      if (!line.empty()) {
        line += L", ";
      }
      line += files[index];
    }
    LogFormat(L"%s files %zu-%zu/%zu: %s", context.c_str(), start + 1, end, files.size(), line.c_str());
  }
  if (files.size() > count) {
    LogFormat(L"%s files truncated: logged %zu of %zu", context.c_str(), count, files.size());
  }
}

void LogAssetDirectorySummary(
    const wchar_t* label,
    const std::wstring& root,
    const std::wstring& child,
    const std::wstring& pattern,
    size_t max_listed_files) {
  const std::wstring path = JoinPath(root, child);
  const bool exists = DirectoryExists(path);
  const std::wstring final_path = exists ? GetFinalDirectoryPath(path) : L"";
  const std::vector<std::wstring> files = ListDirectoryFiles(path, pattern);

  LogFormat(
      L"asset diagnostics: %s\\%s exists=%d path=%s final=%s pattern=%s files=%zu",
      label,
      child.c_str(),
      exists ? 1 : 0,
      path.c_str(),
      final_path.empty() ? L"<unavailable>" : final_path.c_str(),
      pattern.c_str(),
      files.size());
  LogFileListChunks(std::wstring(L"asset diagnostics: ") + label + L"\\" + child, files, max_listed_files);
}

void LogAssetFileSummary(
    const wchar_t* label,
    const std::wstring& directory,
    const std::wstring& pattern,
    size_t max_listed_files) {
  const bool exists = DirectoryExists(directory);
  const std::wstring final_path = exists ? GetFinalDirectoryPath(directory) : L"";
  const std::vector<std::wstring> files = ListDirectoryFiles(directory, pattern);

  LogFormat(
      L"asset diagnostics: %s exists=%d path=%s final=%s pattern=%s files=%zu",
      label,
      exists ? 1 : 0,
      directory.c_str(),
      final_path.empty() ? L"<unavailable>" : final_path.c_str(),
      pattern.c_str(),
      files.size());
  LogFileListChunks(std::wstring(L"asset diagnostics: ") + label, files, max_listed_files);
}

void LogAssetDiagnostics(const std::wstring& workspace) {
  const std::wstring asset_bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!asset_bundle.empty()) {
    LogFormat(L"asset diagnostics: bridge asset bundle=%s", asset_bundle.c_str());
    const std::wstring bundle_diamond = JoinPath(asset_bundle, L"diamond");
    LogFormat(L"asset diagnostics: Bundle Diamond root=%s", bundle_diamond.c_str());
    LogAssetFileSummary(L"Bundle Diamond root keys", bundle_diamond, L"*.key", 20);
    LogAssetFileSummary(L"Bundle Diamond root TLK", bundle_diamond, L"*.tlk", 20);
    LogAssetDirectorySummary(L"Bundle Diamond", bundle_diamond, L"data", L"*.bif*", 80);
    LogAssetDirectorySummary(L"Bundle Diamond", bundle_diamond, L"texturepacks", L"*.erf", 40);
    LogAssetDirectorySummary(L"Bundle Diamond", bundle_diamond, L"hak", L"*.hak", 80);
    LogAssetDirectorySummary(L"Bundle Diamond", bundle_diamond, L"tlk", L"*.tlk", 40);
    LogAssetDirectorySummary(L"Bundle Diamond", bundle_diamond, L"override", L"*", 40);
    LogAssetDirectorySummary(L"Bundle HG GUI", JoinPath(asset_bundle, L"hg-gui"), L"hak", L"*.hak", 40);
    LogAssetDirectorySummary(L"Bundle HG GUI", JoinPath(asset_bundle, L"hg-gui"), L"tlk", L"*.tlk", 20);
    LogAssetDirectorySummary(L"Bundle HG standard", JoinPath(asset_bundle, L"hg-std"), L"hak", L"*.hak", 40);
    LogAssetDirectorySummary(L"Bundle HG standard", JoinPath(asset_bundle, L"hg-std"), L"tlk", L"*.tlk", 20);
  }

  const std::wstring configured_diamond = GetEnvironmentString(L"HG_BRIDGE_DIAMOND_ROOT");
  if (!configured_diamond.empty()) {
    LogFormat(L"asset diagnostics: configured Diamond root=%s", configured_diamond.c_str());
  }
  const std::wstring configured_hg = GetEnvironmentString(L"HG_BRIDGE_HG_ASSET_ROOT");
  if (!configured_hg.empty()) {
    LogFormat(L"asset diagnostics: configured HG asset root=%s", configured_hg.c_str());
  }

  const std::wstring user_profile = GetEnvironmentString(L"USERPROFILE");
  const std::wstring ee_user_root = user_profile.empty()
      ? L""
      : JoinPath(user_profile, L"Documents\\Neverwinter Nights");
  if (!ee_user_root.empty()) {
    LogFormat(L"asset diagnostics: EE user root=%s", ee_user_root.c_str());
    LogAssetDirectorySummary(L"EE", ee_user_root, L"hak", L"*.hak", 80);
    LogAssetDirectorySummary(L"EE", ee_user_root, L"tlk", L"*.tlk", 40);
    LogAssetDirectorySummary(L"EE", ee_user_root, L"override", L"*", 40);
    LogAssetDirectorySummary(L"EE", ee_user_root, L"custom", L"*", 40);
  } else {
    LogFormat(L"asset diagnostics: USERPROFILE is empty; EE user root cannot be inferred");
  }

  if (!workspace.empty() && (asset_bundle.empty() || IsTruthyEnvironmentFlag(L"HG_BRIDGE_LOG_WORKSPACE_ASSET_DIAGNOSTICS"))) {
    const std::wstring diamond_root = JoinPath(workspace, L"NWN Diamond");
    LogFormat(L"asset diagnostics: workspace reference Diamond root=%s", diamond_root.c_str());
    LogAssetFileSummary(L"Diamond root keys", diamond_root, L"*.key", 20);
    LogAssetFileSummary(L"Diamond root TLK", diamond_root, L"*.tlk", 20);
    LogAssetDirectorySummary(L"Diamond", diamond_root, L"data", L"*.bif*", 80);
    LogAssetDirectorySummary(L"Diamond", diamond_root, L"texturepacks", L"*.erf", 40);
    LogAssetDirectorySummary(L"Diamond", diamond_root, L"hak", L"*.hak", 80);
    LogAssetDirectorySummary(L"Diamond", diamond_root, L"tlk", L"*.tlk", 40);
    LogAssetDirectorySummary(L"Diamond", diamond_root, L"override", L"*", 40);
    LogAssetDirectorySummary(L"Diamond", diamond_root, L"custom", L"*", 80);

    const std::wstring ee_install_root = JoinPath(workspace, L"Neverwinter Nights EE");
    LogFormat(L"asset diagnostics: workspace EE root=%s", ee_install_root.c_str());
    LogAssetDirectorySummary(L"Workspace EE", ee_install_root, L"data", L"*.key", 40);
    LogAssetDirectorySummary(L"Workspace EE", ee_install_root, L"data", L"*.bif", 80);
  } else if (!workspace.empty()) {
    LogFormat(
        L"asset diagnostics: workspace scan skipped because HG_BRIDGE_ASSET_BUNDLE is set; set HG_BRIDGE_LOG_WORKSPACE_ASSET_DIAGNOSTICS=1 to include Google Drive/reference paths");
  }
}

std::vector<std::string> ParseCdKeyIniValues(const std::string& contents) {
  std::vector<std::string> keys;
  bool in_cdkey_section = false;
  size_t cursor = 0;
  while (cursor <= contents.size()) {
    const size_t line_end = contents.find_first_of("\r\n", cursor);
    std::string line = line_end == std::string::npos
        ? contents.substr(cursor)
        : contents.substr(cursor, line_end - cursor);
    cursor = line_end == std::string::npos ? contents.size() + 1 : line_end + 1;

    line = TrimAsciiWhitespace(line);
    if (line.empty() || line[0] == ';' || line[0] == '#') {
      continue;
    }

    if (line.front() == '[' && line.back() == ']') {
      in_cdkey_section = LowerAscii(TrimAsciiWhitespace(line.substr(1, line.size() - 2))) == "cdkey";
      continue;
    }

    if (!in_cdkey_section) {
      continue;
    }

    const size_t equals = line.find('=');
    if (equals == std::string::npos) {
      continue;
    }

    const std::string name = LowerAscii(TrimAsciiWhitespace(line.substr(0, equals)));
    std::string value = TrimAsciiWhitespace(line.substr(equals + 1));
    if (name.rfind("key", 0) == 0 && !value.empty()) {
      keys.push_back(std::move(value));
    }
  }
  return keys;
}

void LoadDiamondCdKeys(const std::wstring& workspace) {
  std::string contents;
  std::wstring loaded_path;

  const std::wstring configured_cdkey_path = GetEnvironmentString(L"HG_BRIDGE_DIAMOND_CDKEY_PATH");
  if (!configured_cdkey_path.empty()) {
    if (ReadSmallTextFile(configured_cdkey_path, &contents)) {
      loaded_path = configured_cdkey_path;
    } else {
      LogFormat(L"Diamond CD key seed configured path unreadable: %s", configured_cdkey_path.c_str());
    }
  }

  std::vector<std::wstring> workspaces;
  if (!workspace.empty()) {
    workspaces.push_back(workspace);
  }

  const std::wstring module_path = GetModulePath(g_state.module);
  const std::wstring module_dir = ParentPath(module_path);
  const std::wstring build_dir = ParentPath(module_dir);
  const std::wstring repository_root = ParentPath(build_dir);
  if (!repository_root.empty() && (workspace.empty() || repository_root != workspace)) {
    workspaces.push_back(repository_root);
  }

  if (workspaces.empty()) {
    LogFormat(L"Diamond CD key seed skipped: no workspace candidates available");
    return;
  }

  std::wstring last_path;
  if (loaded_path.empty()) {
    for (const std::wstring& candidate : workspaces) {
      const std::wstring path = candidate + L"\\NWN Diamond\\nwncdkey.ini";
      last_path = path;
      if (ReadSmallTextFile(path, &contents)) {
        loaded_path = path;
        break;
      }
    }
  }

  if (loaded_path.empty()) {
    LogFormat(L"Diamond CD key seed skipped: could not read %s", last_path.c_str());
    return;
  }

  g_diamond_cd_keys = ParseCdKeyIniValues(contents);
  if (g_diamond_cd_keys.empty()) {
    LogFormat(L"Diamond CD key seed skipped: no [CDKEY] values found");
    return;
  }

  wchar_t lengths[128]{};
  size_t offset = 0;
  for (size_t index = 0; index < g_diamond_cd_keys.size() && index < 4; ++index) {
    const int written = swprintf_s(
        lengths + offset,
        (sizeof(lengths) / sizeof(lengths[0])) - offset,
        index == 0 ? L"%zu" : L"/%zu",
        g_diamond_cd_keys[index].size());
    if (written <= 0) {
      break;
    }
    offset += static_cast<size_t>(written);
  }
  LogFormat(L"Diamond CD key seed source loaded: count=%zu lengths=%s source=%s", g_diamond_cd_keys.size(), lengths, loaded_path.c_str());
}

void __fastcall HookedStartAddressTranslation(void* this_pointer, const CExoStringView* address) {
  const std::wstring text = CExoStringToWide(address);
  if (!g_auto_connect_host.empty() && text == g_auto_connect_host) {
    InterlockedExchange(&g_target_address_translation_seen, 1);
  }
  LogFormat(L"address translation requested: '%s' length=%u", text.c_str(), address != nullptr ? address->length : 0);
  g_start_address_translation_original(this_pointer, address);
}

bool InitCExoStringValue(CExoStringInitFromCharArrayFn init_string, CExoStringView* field, const std::string& value) {
  if (init_string == nullptr || field == nullptr || value.empty() || value.size() > 0xFFFFFFFFull) {
    return false;
  }

  __try {
    field->data = nullptr;
    field->length = 0;
    field->capacity = 0;
    init_string(field, value.c_str(), static_cast<uint32_t>(value.size()));
    return static_cast<size_t>(field->length) == value.size();
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool ReadDirectConnectState(void* state, CExoStringView* address, unsigned short* port, uint32_t* phase = nullptr) {
  if (state == nullptr || address == nullptr || port == nullptr) {
    return false;
  }

  __try {
    if (phase != nullptr) {
      *phase = *reinterpret_cast<uint32_t*>(state);
    }
    *address = *reinterpret_cast<CExoStringView*>(static_cast<uint8_t*>(state) + 0x08);
    *port = *reinterpret_cast<unsigned short*>(static_cast<uint8_t*>(state) + 0x18);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool ReadDirectConnectStatePassword(void* state, CExoStringView* password) {
  if (state == nullptr || password == nullptr) {
    return false;
  }

  __try {
    *password = *reinterpret_cast<CExoStringView*>(static_cast<uint8_t*>(state) + kDirectConnectStatePasswordOffset);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool WriteDirectConnectStatePort(void* state, unsigned short port) {
  if (state == nullptr) {
    return false;
  }

  __try {
    *reinterpret_cast<unsigned short*>(static_cast<uint8_t*>(state) + 0x18) = port;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SeedDirectConnectStatePassword(void* state) {
  if (state == nullptr || g_auto_connect_password.empty()) {
    return false;
  }

  CExoStringView password{};
  if (!ReadDirectConnectStatePassword(state, &password) || SafeCExoStringLength(&password) != 0) {
    return false;
  }

  auto* const init_string = ResolveMainExport<CExoStringInitFromCharArrayFn>("?InitFromCharArray@CExoString@@AEAAAEAV1@PEBDI@Z");
  auto* const field = reinterpret_cast<CExoStringView*>(static_cast<uint8_t*>(state) + kDirectConnectStatePasswordOffset);
  if (!InitCExoStringValue(init_string, field, g_auto_connect_password)) {
    return false;
  }

  const LONG observation = InterlockedIncrement(&g_direct_connect_password_observations);
  if (observation <= 10) {
    LogFormat(
        L"direct-connect password seeded #%ld: state=%p length=%zu",
        observation,
        state,
        g_auto_connect_password.size());
  }
  return true;
}

void RepairDirectConnectStatePort(void* state) {
  if (g_auto_connect_host.empty() || g_auto_connect_port == 0) {
    return;
  }

  CExoStringView address{};
  unsigned short current_port = 0;
  uint32_t phase = 0;
  if (!ReadDirectConnectState(state, &address, &current_port, &phase)) {
    return;
  }

  const std::wstring address_text = CExoStringToWide(&address);
  if (address_text == g_auto_connect_host) {
    const LONG observation = InterlockedIncrement(&g_direct_connect_state_observations);
    LogFormat(
        L"direct-connect state observed #%ld: state=%p phase=%u address='%s' length=%u port=%hu target=%s:%hu",
        observation,
        state,
        phase,
        address_text.c_str(),
        address.length,
        current_port,
        g_auto_connect_host.c_str(),
        g_auto_connect_port);
    SeedDirectConnectStatePassword(state);
  }

  if (address_text != g_auto_connect_host || current_port == g_auto_connect_port) {
    return;
  }

  if (WriteDirectConnectStatePort(state, g_auto_connect_port)) {
    LogFormat(
        L"direct-connect port repaired for '%s': %hu -> %hu",
        address_text.c_str(),
        current_port,
        g_auto_connect_port);
  } else {
    LogFormat(
        L"direct-connect port repair failed for '%s': current=%hu target=%hu",
        address_text.c_str(),
        current_port,
        g_auto_connect_port);
  }
}

void __fastcall HookedDirectConnectStateUpdate(void* state) {
  RepairDirectConnectStatePort(state);
  g_direct_connect_state_update_original(state);
}

bool ReadNetPeer(void* net_peer, NetPeerView* snapshot) {
  if (net_peer == nullptr || snapshot == nullptr) {
    return false;
  }

  __try {
    std::memcpy(snapshot, net_peer, sizeof(NetPeerView));
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

void* ReadPointerField(void* object, size_t offset) {
  if (object == nullptr) {
    return nullptr;
  }

  __try {
    return *reinterpret_cast<void**>(static_cast<uint8_t*>(object) + offset);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return nullptr;
  }
}

void* GetClientExoAppFromAppManager(void* app_manager) {
  return ReadPointerField(app_manager, 0x00);
}

void* GetClientExoAppInternalFromAppManager(void* app_manager) {
  void* const client_app = GetClientExoAppFromAppManager(app_manager);
  return ReadPointerField(client_app, 0x08);
}

void* GetConnectionLibFromAppManager(void* app_manager) {
  void* const client_app = GetClientExoAppFromAppManager(app_manager);
  if (client_app == nullptr) {
    return nullptr;
  }

  __try {
    void* const vtable = *reinterpret_cast<void**>(client_app);
    if (vtable == nullptr) {
      return nullptr;
    }

    using GetConnectionLibFn = void*(__fastcall*)(void*);
    auto* const get_connection_lib = *reinterpret_cast<GetConnectionLibFn*>(static_cast<uint8_t*>(vtable) + 0x60);
    return get_connection_lib != nullptr ? get_connection_lib(client_app) : nullptr;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return nullptr;
  }
}

bool TryGetPublicPartFromCdKey(
    GetPublicPartFromCdKeyFn get_public_part,
    void* connection_lib,
    CExoStringView* public_part,
    const CExoStringView* raw_key) {
  if (get_public_part == nullptr || connection_lib == nullptr || public_part == nullptr || raw_key == nullptr) {
    return false;
  }

  __try {
    get_public_part(connection_lib, public_part, raw_key);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

void TryDestroyCExoString(CExoStringDestructorFn destroy_string, CExoStringView* value) {
  if (destroy_string == nullptr || value == nullptr || value->data == nullptr) {
    return;
  }

  __try {
    destroy_string(value);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}

ClientSendChatTalkFn ResolveClientSendChatTalk() {
  if (g_client_send_chat_talk != nullptr) {
    return g_client_send_chat_talk;
  }

  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    const LONG observation = InterlockedIncrement(&g_hg_password_auto_speak_resolve_observations);
    if (observation <= 3) {
      LogFormat(L"HG password auto-speak unavailable: main module not found");
    }
    return nullptr;
  }

  auto* const target = base + kEeClientSendChatTalkRva;
  static constexpr uint8_t kExpectedPrologue[] = {
      0x48, 0x89, 0x54, 0x24, 0x10,
      0x53,
      0x56,
      0x57,
      0x48, 0x83, 0xEC, 0x50,
  };
  if (!std::equal(kExpectedPrologue, kExpectedPrologue + sizeof(kExpectedPrologue), target)) {
    const LONG observation = InterlockedIncrement(&g_hg_password_auto_speak_resolve_observations);
    if (observation <= 3) {
      LogFormat(
          L"HG password auto-speak unavailable: unexpected talk-send prologue at rva=0x%zX target=%p expected=[%s] actual=[%s]",
          kEeClientSendChatTalkRva,
          target,
          FormatBytes(kExpectedPrologue, sizeof(kExpectedPrologue)).c_str(),
          FormatBytes(target, sizeof(kExpectedPrologue)).c_str());
    }
    return nullptr;
  }

  g_client_send_chat_talk = reinterpret_cast<ClientSendChatTalkFn>(target);
  LogFormat(
      L"HG password auto-speak resolved EE talk-send wrapper at rva=0x%zX target=%p",
      kEeClientSendChatTalkRva,
      target);
  return g_client_send_chat_talk;
}

bool ComputeDiamondPublicCdKeys(void* app_manager);
void* GetCurrentAppManager();

bool IsLegacyMasterServerFillerPassword(const std::string& password) {
  return password.size() <= 1;
}

void AddHgPasswordCandidate(
    std::vector<std::pair<std::string, std::wstring>>* candidates,
    const std::string& password,
    const std::wstring& source_label) {
  if (candidates == nullptr || password.empty()) {
    return;
  }

  for (const auto& candidate : *candidates) {
    if (candidate.first == password) {
      return;
    }
  }
  candidates->push_back({password, source_label});
}

std::string StripCdKeySeparators(const std::string& key) {
  std::string stripped;
  stripped.reserve(key.size());
  for (char ch : key) {
    if (ch != '-' && ch != ' ' && ch != '\t') {
      stripped.push_back(ch);
    }
  }
  return stripped;
}

std::vector<std::pair<std::string, std::wstring>> BuildHgPasswordCandidates() {
  std::vector<std::pair<std::string, std::wstring>> candidates;

  const std::string source = LowerAscii(g_auto_connect_password_source);
  if (source == "command-line" && !g_auto_connect_password.empty()) {
    AddHgPasswordCandidate(&candidates, g_auto_connect_password, L"command-line");
    return candidates;
  }

  const bool password_from_diamond_profile = source == "diamond profile";
  if (!g_auto_connect_password.empty() &&
      !IsLegacyMasterServerFillerPassword(g_auto_connect_password)) {
    AddHgPasswordCandidate(
        &candidates,
        g_auto_connect_password,
        g_auto_connect_password_source.empty() ? L"launcher password" : NarrowToWide(g_auto_connect_password_source));
    if (!password_from_diamond_profile) {
      return candidates;
    }
  }

  const bool include_public_cdkey_candidates =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_CDKEY_PASSWORD_CANDIDATES") ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_RAW_CDKEY_PASSWORD_CANDIDATES");
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_RAW_CDKEY_PASSWORD_CANDIDATES")) {
    for (size_t offset = 0; offset < g_diamond_cd_keys.size(); ++offset) {
      const size_t index = g_diamond_cd_keys.size() - 1 - offset;
      AddHgPasswordCandidate(
          &candidates,
          StripCdKeySeparators(g_diamond_cd_keys[index]),
          L"Diamond raw CD key without separators #" + std::to_wstring(index + 1));
      AddHgPasswordCandidate(
          &candidates,
          g_diamond_cd_keys[index],
          L"Diamond raw CD key #" + std::to_wstring(index + 1));
    }
  }

  if (include_public_cdkey_candidates) {
    if (g_diamond_public_cd_keys.empty()) {
      ComputeDiamondPublicCdKeys(GetCurrentAppManager());
    }
    for (size_t offset = 0; offset < g_diamond_public_cd_keys.size(); ++offset) {
      const size_t index = g_diamond_public_cd_keys.size() - 1 - offset;
      AddHgPasswordCandidate(
          &candidates,
          g_diamond_public_cd_keys[index],
          L"Diamond public CD key #" + std::to_wstring(index + 1));
    }
  }

  return candidates;
}

std::string SelectHgPasswordToSpeak(size_t candidate_index, std::wstring* source_label, size_t* candidate_count) {
  if (source_label != nullptr) {
    *source_label = L"<none>";
  }
  if (candidate_count != nullptr) {
    *candidate_count = 0;
  }

  const std::vector<std::pair<std::string, std::wstring>> candidates = BuildHgPasswordCandidates();
  if (candidate_count != nullptr) {
    *candidate_count = candidates.size();
  }
  if (candidate_index >= candidates.size()) {
    return {};
  }

  if (source_label != nullptr) {
    *source_label = candidates[candidate_index].second;
  }
  return candidates[candidate_index].first;
}

int64_t CallClientSendChatTalkNoExcept(
    ClientSendChatTalkFn send_chat_talk,
    void* message,
    CExoStringView* text,
    bool* raised_exception) {
  int64_t result = 0;
  bool raised = false;
  __try {
    result = send_chat_talk != nullptr ? send_chat_talk(message, text) : 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    raised = true;
  }
  if (raised_exception != nullptr) {
    *raised_exception = raised;
  }
  return result;
}

bool TryAutoSpeakHgPassword(void* message, LONG dispatch_observation) {
  std::wstring password_source;
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_SPEAK_PASSWORD")) {
    LogFormat(L"HG password auto-speak skipped: HG_BRIDGE_DISABLE_AUTO_SPEAK_PASSWORD is set");
    return false;
  }
  if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_AUTO_SPEAK_PASSWORD")) {
    const LONG skip = InterlockedIncrement(&g_hg_password_auto_speak_skip_observations);
    if (skip <= 8) {
      LogFormat(
          L"HG password auto-speak skipped #%ld: dispatch=%ld not enabled; set HG_BRIDGE_ENABLE_AUTO_SPEAK_PASSWORD=1 only for characters that need an in-game spoken password",
          skip,
          dispatch_observation);
    }
    return false;
  }
  if (message == nullptr) {
    return false;
  }

  const LONG attempt = InterlockedIncrement(&g_hg_password_auto_speak_attempts);
  size_t candidate_count = 0;
  const size_t candidate_index = static_cast<size_t>(attempt > 0 ? attempt - 1 : 0);
  const std::string password_to_speak = SelectHgPasswordToSpeak(candidate_index, &password_source, &candidate_count);
  if (password_to_speak.empty()) {
    LogFormat(
        L"HG password auto-speak #%ld skipped: dispatch=%ld candidate=%zu/%zu no remaining candidate",
        attempt,
        dispatch_observation,
        candidate_index + 1,
        candidate_count);
    return false;
  }

  auto* const send_chat_talk = ResolveClientSendChatTalk();
  auto* const init_string =
      ResolveMainExport<CExoStringInitFromCharArrayFn>("?InitFromCharArray@CExoString@@AEAAAEAV1@PEBDI@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");
  if (send_chat_talk == nullptr || init_string == nullptr) {
    LogFormat(
        L"HG password auto-speak #%ld failed before send: dispatch=%ld message=%p send_chat_talk=%p init_string=%p",
        attempt,
        dispatch_observation,
        message,
        send_chat_talk,
        init_string);
    return false;
  }

  CExoStringView password_text{};
  if (!InitCExoStringValue(init_string, &password_text, password_to_speak)) {
    LogFormat(
        L"HG password auto-speak #%ld failed before send: dispatch=%ld message=%p could not construct CExoString source=%s length=%zu",
        attempt,
        dispatch_observation,
        message,
        password_source.c_str(),
        password_to_speak.size());
    return false;
  }

  int64_t result = 0;
  bool raised_exception = false;
  result = CallClientSendChatTalkNoExcept(send_chat_talk, message, &password_text, &raised_exception);

  // The EE talk-send wrapper frees and clears the CExoString it receives on
  // normal paths. Clean up only if an exception or early return left ownership
  // with us.
  if (password_text.data != nullptr) {
    TryDestroyCExoString(destroy_string, &password_text);
  }

  LogFormat(
      L"HG password auto-speak #%ld: dispatch=%ld message=%p candidate=%zu/%zu source=%s password_length=%zu result=0x%llX/%lld%s",
      attempt,
      dispatch_observation,
      message,
      candidate_index + 1,
      candidate_count,
      password_source.c_str(),
      password_to_speak.size(),
      static_cast<unsigned long long>(result),
      static_cast<long long>(result),
      raised_exception ? L" exception" : L"");
  return !raised_exception && result != 0;
}

void CancelDelayedHgPasswordRetry(const wchar_t* reason, LONG dispatch_observation) {
  const LONG was_scheduled = InterlockedExchange(&g_hg_password_auto_speak_retry_scheduled, 0);
  void* const retry_message = g_hg_password_auto_speak_retry_message;
  const LONG retry_dispatch = g_hg_password_auto_speak_retry_dispatch;
  const ULONGLONG due_tick = g_hg_password_auto_speak_retry_due_tick;
  g_hg_password_auto_speak_retry_message = nullptr;
  g_hg_password_auto_speak_retry_dispatch = 0;
  g_hg_password_auto_speak_retry_due_tick = 0;

  if (was_scheduled != 0 || retry_message != nullptr || due_tick != 0) {
    LogFormat(
        L"HG password auto-speak retry canceled: dispatch=%ld scheduled=%ld scheduled_dispatch=%ld message=%p reason=%s",
        dispatch_observation,
        was_scheduled,
        retry_dispatch,
        retry_message,
        reason != nullptr ? reason : L"<unspecified>");
  }
}

void ScheduleDelayedHgPasswordRetry(void* message, LONG dispatch_observation) {
  if (message == nullptr) {
    return;
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_SPEAK_PASSWORD") ||
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_AUTO_SPEAK_PASSWORD")) {
    const LONG skip = InterlockedIncrement(&g_hg_password_auto_speak_skip_observations);
    if (skip <= 8) {
      LogFormat(
          L"HG password auto-speak retry skipped #%ld: dispatch=%ld not enabled",
          skip,
          dispatch_observation);
    }
    return;
  }

  constexpr DWORD kRetryDelayMs = 11000;
  if (InterlockedCompareExchange(&g_hg_password_auto_speak_retry_scheduled, 1, 0) != 0) {
    LogFormat(
        L"HG password auto-speak retry already scheduled: dispatch=%ld pending_dispatch=%ld due_in_ms=%llu",
        dispatch_observation,
        g_hg_password_auto_speak_retry_dispatch,
        g_hg_password_auto_speak_retry_due_tick > GetTickCount64()
            ? static_cast<unsigned long long>(g_hg_password_auto_speak_retry_due_tick - GetTickCount64())
            : 0ull);
    return;
  }

  g_hg_password_auto_speak_retry_message = message;
  g_hg_password_auto_speak_retry_dispatch = dispatch_observation;
  g_hg_password_auto_speak_retry_due_tick = GetTickCount64() + kRetryDelayMs;
  LogFormat(
      L"HG password auto-speak retry scheduled: dispatch=%ld message=%p delay_ms=%lu",
      dispatch_observation,
      message,
      static_cast<unsigned long>(kRetryDelayMs));
}

void TryPendingDelayedHgPasswordRetry(LONG dispatch_observation) {
  if (InterlockedCompareExchange(&g_hg_password_auto_speak_retry_scheduled, 1, 1) == 0) {
    return;
  }

  const ULONGLONG now = GetTickCount64();
  if (now < g_hg_password_auto_speak_retry_due_tick) {
    return;
  }

  if (InterlockedCompareExchange(&g_hg_password_auto_speak_retry_scheduled, 0, 1) != 1) {
    return;
  }

  void* const retry_message = g_hg_password_auto_speak_retry_message;
  const LONG retry_dispatch = g_hg_password_auto_speak_retry_dispatch;
  g_hg_password_auto_speak_retry_message = nullptr;
  g_hg_password_auto_speak_retry_dispatch = 0;
  g_hg_password_auto_speak_retry_due_tick = 0;

  LogFormat(
      L"HG password auto-speak retry firing: current_dispatch=%ld scheduled_dispatch=%ld message=%p",
      dispatch_observation,
      retry_dispatch,
      retry_message);
  TryAutoSpeakHgPassword(retry_message, dispatch_observation);
}

bool ComputeDiamondPublicCdKeys(void* app_manager) {
  if (g_diamond_cd_keys.empty()) {
    return false;
  }

  if (!g_diamond_public_cd_keys.empty()) {
    return true;
  }

  void* const connection_lib = GetConnectionLibFromAppManager(app_manager);
  auto* const get_public_part =
      ResolveMainExport<GetPublicPartFromCdKeyFn>("?GetPublicPartFromCDKey@CConnectionLib@@QEAA?AVCExoString@@AEBV2@@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");
  if (connection_lib == nullptr || get_public_part == nullptr || destroy_string == nullptr) {
    LogFormat(
        L"Diamond CD key public diagnostics skipped: connection_lib=%p get_public=%p destroy=%p",
        connection_lib,
        get_public_part,
        destroy_string);
    return false;
  }

  std::vector<std::string> public_keys;
  std::wstring lengths;
  for (size_t index = 0; index < g_diamond_cd_keys.size(); ++index) {
    const std::string& raw_key = g_diamond_cd_keys[index];
    CExoStringView raw_view{const_cast<char*>(raw_key.data()), static_cast<uint32_t>(raw_key.size()), 0};
    CExoStringView public_view{};
    const bool call_ok = TryGetPublicPartFromCdKey(get_public_part, connection_lib, &public_view, &raw_view);

    std::string public_key;
    if (call_ok) {
      public_key = CExoStringToString(&public_view, 32);
    }
    if (index != 0) {
      lengths += L"/";
    }
    lengths += std::to_wstring(public_key.size());
    if (public_key.size() == 8) {
      public_keys.push_back(public_key);
    }

    TryDestroyCExoString(destroy_string, &public_view);
  }

  if (InterlockedCompareExchange(&g_diamond_cd_key_public_diagnostics_logged, 1, 0) == 0) {
    LogFormat(
        L"Diamond CD key public diagnostics: raw_count=%zu public_lengths=%s valid_public_count=%zu",
        g_diamond_cd_keys.size(),
        lengths.c_str(),
        public_keys.size());
  }

  if (public_keys.empty()) {
    return false;
  }

  g_diamond_public_cd_keys = std::move(public_keys);
  return true;
}

void* GetCurrentAppManager() {
  void** app_manager_global = ResolveMainExport<void**>("?g_pAppManager@@3PEAVCAppManager@@EA");
  return app_manager_global != nullptr ? *app_manager_global : nullptr;
}

bool TryEncryptString(CExoEncryptStringFn encrypt_string, const std::string& input, CExoStringView* encrypted) {
  if (encrypt_string == nullptr || encrypted == nullptr) {
    return false;
  }

  CExoStringView input_view{
      const_cast<char*>(input.data()),
      static_cast<uint32_t>(std::min<size_t>(input.size(), UINT32_MAX)),
      0};

  __try {
    encrypt_string(nullptr, encrypted, &input_view);
    return encrypted->data != nullptr && encrypted->length > 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool BuildLegacyCdKeyVerifier(
    CExoEncryptStringFn encrypt_string,
    CExoStringDestructorFn destroy_string,
    size_t key_index,
    const std::string& cd_key_challenge,
    std::string* verifier) {
  if (verifier == nullptr || key_index >= g_diamond_cd_keys.size() || key_index >= g_diamond_public_cd_keys.size()) {
    return false;
  }

  const std::string& public_key = g_diamond_public_cd_keys[key_index];
  const std::string& raw_key = g_diamond_cd_keys[key_index];
  if (public_key.size() != 8 || raw_key.empty() || cd_key_challenge.empty()) {
    return false;
  }

  CExoStringView encrypted{};
  const std::string input = raw_key + cd_key_challenge;
  const bool encrypted_ok = TryEncryptString(encrypt_string, input, &encrypted);
  std::string encrypted_text;
  if (encrypted_ok) {
    encrypted_text = CExoStringToString(&encrypted, 64);
  }
  TryDestroyCExoString(destroy_string, &encrypted);

  if (encrypted_text.size() != 32) {
    return false;
  }

  *verifier = public_key + encrypted_text;
  return verifier->size() == 40;
}

bool SeedCExoStringField(CExoStringInitFromCharArrayFn init_string, void* app_internal, size_t offset, const std::string& value) {
  if (init_string == nullptr || app_internal == nullptr || value.empty()) {
    return false;
  }

  __try {
    auto* const field = reinterpret_cast<CExoStringView*>(static_cast<uint8_t*>(app_internal) + offset);
    return InitCExoStringValue(init_string, field, value);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SeedDiamondCdKeysIntoAppManager(void* app_manager, bool force) {
  if (g_diamond_cd_keys.empty() || app_manager == nullptr) {
    return false;
  }

  if (!force && InterlockedCompareExchange(&g_diamond_cd_keys_seeded, 0, 0) != 0) {
    return true;
  }

  void* const client_app = GetClientExoAppFromAppManager(app_manager);
  auto* const app_internal = GetClientExoAppInternalFromAppManager(app_manager);
  if (app_internal == nullptr) {
    if (force) {
      LogFormat(L"Diamond CD key seed attempt failed: app_manager=%p client_app=%p app_internal=null", app_manager, client_app);
    }
    return false;
  }

  auto* const init_string = ResolveMainExport<CExoStringInitFromCharArrayFn>("?InitFromCharArray@CExoString@@AEAAAEAV1@PEBDI@Z");
  if (init_string == nullptr) {
    LogFormat(L"Diamond CD key seed skipped: CExoString::InitFromCharArray export not found");
    return false;
  }

  ComputeDiamondPublicCdKeys(app_manager);

  const std::string& primary_key = g_diamond_cd_keys[0];
  const std::string& secondary_key = g_diamond_cd_keys.size() > 1 ? g_diamond_cd_keys[1] : primary_key;
  bool seeded_any = false;
  seeded_any |= SeedCExoStringField(init_string, app_internal, 0x4F0, primary_key);
  seeded_any |= SeedCExoStringField(init_string, app_internal, 0x4E0, secondary_key);

  if (seeded_any) {
    const LONG previous = InterlockedExchange(&g_diamond_cd_keys_seeded, 1);
    if (previous == 0) {
      LogFormat(
          L"Diamond CD keys seeded into EE identity fields: key1_length=%zu key2_length=%zu",
          primary_key.size(),
          secondary_key.size());
    }
  } else if (force || InterlockedCompareExchange(&g_diamond_cd_keys_seeded, 0, 0) == 0) {
    LogFormat(L"Diamond CD key seed attempt failed: app_internal=%p", app_internal);
  }
  return seeded_any;
}

DWORD WINAPI DiamondIdentitySeedThread(LPVOID) {
  if (g_diamond_cd_keys.empty()) {
    return 0;
  }

  void** app_manager_global = ResolveMainExport<void**>("?g_pAppManager@@3PEAVCAppManager@@EA");
  if (app_manager_global == nullptr) {
    LogFormat(L"Diamond CD key seed worker skipped: app manager export not found");
    return 0;
  }

  for (int attempt = 1; attempt <= 80; ++attempt) {
    void* const app_manager = *app_manager_global;
    if (SeedDiamondCdKeysIntoAppManager(app_manager, false)) {
      return 1;
    }
    if (attempt == 1 || attempt % 20 == 0) {
      LogFormat(L"Diamond CD key seed worker waiting for app manager internals (attempt %d)", attempt);
    }
    Sleep(250);
  }

  LogFormat(L"Diamond CD key seed worker gave up before app manager internals became available");
  return 0;
}

bool StartDiamondIdentitySeedWorker() {
  if (g_diamond_cd_keys.empty()) {
    return false;
  }

  HANDLE thread = CreateThread(nullptr, 0, DiamondIdentitySeedThread, nullptr, 0, nullptr);
  if (thread == nullptr) {
    LogFormat(L"Diamond CD key seed worker failed to start: error=%lu", GetLastError());
    return false;
  }

  CloseHandle(thread);
  LogFormat(L"Diamond CD key seed worker started");
  return true;
}

std::wstring FormatIpv4(const uint8_t* bytes) {
  if (bytes == nullptr) {
    return L"<null>";
  }

  wchar_t buffer[32]{};
  swprintf_s(buffer, L"%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
  return buffer;
}

bool SafeReadUInt32(const uint32_t* value, uint32_t* snapshot) {
  if (value == nullptr || snapshot == nullptr) {
    return false;
  }

  __try {
    *snapshot = *value;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeReadFloat(const float* value, float* snapshot) {
  if (value == nullptr || snapshot == nullptr) {
    return false;
  }

  __try {
    *snapshot = *value;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeWriteUInt32(uint32_t* value, uint32_t replacement) {
  if (value == nullptr) {
    return false;
  }

  __try {
    *value = replacement;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeWriteUInt16(uint16_t* value, uint16_t replacement) {
  if (value == nullptr) {
    return false;
  }

  __try {
    *value = replacement;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeWriteUInt8(uint8_t* value, uint8_t replacement) {
  if (value == nullptr) {
    return false;
  }

  __try {
    *value = replacement;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeWriteFloat(float* value, float replacement) {
  if (value == nullptr) {
    return false;
  }

  __try {
    *value = replacement;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeZeroMemory(void* value, size_t bytes) {
  if (value == nullptr || bytes == 0) {
    return false;
  }

  __try {
    std::memset(value, 0, bytes);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeReadUInt16(const uint16_t* value, uint16_t* snapshot) {
  if (value == nullptr || snapshot == nullptr) {
    return false;
  }

  __try {
    *snapshot = *value;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool SafeReadUInt8(const uint8_t* value, uint8_t* snapshot) {
  if (value == nullptr || snapshot == nullptr) {
    return false;
  }

  __try {
    *snapshot = *value;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

struct CnwMessageReadState {
  bool readable = false;
  uint32_t read_buffer_size = 0;
  uint32_t read_buffer_ptr = 0;
  uint32_t read_fragments_size = 0;
  uint32_t read_fragments_ptr = 0;
  uint8_t read_fragments_bit_size = 0;
  uint8_t read_fragments_bit_ptr = 0;
};

CnwMessageReadState ReadCnwMessageReadState(void* message) {
  CnwMessageReadState state{};
  if (message == nullptr) {
    return state;
  }

  uint8_t* const base = static_cast<uint8_t*>(message);
  const bool ok =
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x40), &state.read_buffer_size) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x44), &state.read_buffer_ptr) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x50), &state.read_fragments_size) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x54), &state.read_fragments_ptr) &&
      SafeReadUInt8(base + 0x5D, &state.read_fragments_bit_size) &&
      SafeReadUInt8(base + 0x5C, &state.read_fragments_bit_ptr);
  state.readable = ok;
  return state;
}

bool CnwMessageStateOverflow(const CnwMessageReadState& state) {
  return state.readable &&
      (state.read_buffer_ptr > state.read_buffer_size || state.read_fragments_ptr > state.read_fragments_size);
}

bool CnwMessageStateUnderflow(const CnwMessageReadState& state) {
  if (!state.readable) {
    return false;
  }
  if (state.read_buffer_ptr < state.read_buffer_size) {
    return true;
  }
  if (state.read_fragments_size < state.read_fragments_ptr) {
    return false;
  }

  const uint32_t fragment_bytes_remaining = state.read_fragments_size - state.read_fragments_ptr;
  return fragment_bytes_remaining > 1 ||
      (fragment_bytes_remaining == 1 && state.read_fragments_bit_ptr < state.read_fragments_bit_size);
}

std::wstring FormatCnwMessageReadState(const CnwMessageReadState& state) {
  if (!state.readable) {
    return L"<unreadable>";
  }

  wchar_t buffer[192]{};
  const int64_t buffer_remaining =
      static_cast<int64_t>(state.read_buffer_size) - static_cast<int64_t>(state.read_buffer_ptr);
  const int64_t fragment_remaining =
      static_cast<int64_t>(state.read_fragments_size) - static_cast<int64_t>(state.read_fragments_ptr);
  swprintf_s(
      buffer,
      L"buffer=%u/%u rem=%lld fragments=%u/%u rem=%lld bits=%u/%u overflow=%d underflow=%d",
      state.read_buffer_ptr,
      state.read_buffer_size,
      buffer_remaining,
      state.read_fragments_ptr,
      state.read_fragments_size,
      fragment_remaining,
      static_cast<unsigned int>(state.read_fragments_bit_ptr),
      static_cast<unsigned int>(state.read_fragments_bit_size),
      CnwMessageStateOverflow(state) ? 1 : 0,
      CnwMessageStateUnderflow(state) ? 1 : 0);
  return buffer;
}

bool ReadPointerAtOffset(void* object, size_t offset, void** snapshot) {
  if (object == nullptr || snapshot == nullptr) {
    return false;
  }

  __try {
    *snapshot = *reinterpret_cast<void**>(static_cast<uint8_t*>(object) + offset);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

std::wstring FormatCnwMessageReadPointers(void* message) {
  if (message == nullptr) {
    return L"read_buffer=<null> fragments_buffer=<null>";
  }

  void* read_buffer = nullptr;
  void* fragments_buffer = nullptr;
  ReadPointerAtOffset(message, 0x38, &read_buffer);
  ReadPointerAtOffset(message, 0x48, &fragments_buffer);

  wchar_t buffer[128]{};
  swprintf_s(buffer, L"read_buffer=%p fragments_buffer=%p", read_buffer, fragments_buffer);
  return buffer;
}

std::wstring FormatCnwMessageBufferPreview(
    void* message,
    const CnwMessageReadState& state,
    uint32_t max_bytes = 32) {
  if (message == nullptr || !state.readable) {
    return L"<unavailable>";
  }

  void* read_buffer = nullptr;
  void* fragments_buffer = nullptr;
  ReadPointerAtOffset(message, 0x38, &read_buffer);
  ReadPointerAtOffset(message, 0x48, &fragments_buffer);

  std::wstring text;
  if (read_buffer != nullptr && state.read_buffer_ptr <= state.read_buffer_size) {
    const uint32_t remaining = state.read_buffer_size - state.read_buffer_ptr;
    const uint32_t logged = std::min<uint32_t>(remaining, max_bytes);
    std::vector<uint8_t> bytes(logged);
    if (logged > 0 && TryCopyBytes(
          reinterpret_cast<const char*>(static_cast<uint8_t*>(read_buffer) + state.read_buffer_ptr),
          logged,
          reinterpret_cast<char*>(bytes.data()))) {
      text += L"buffer_next=[";
      text += FormatBytes(bytes.data(), bytes.size());
      text += L"]";
    } else {
      text += L"buffer_next=[]";
    }
  } else {
    text += L"buffer_next=<unreadable>";
  }

  text += L" ";
  if (fragments_buffer != nullptr && state.read_fragments_ptr <= state.read_fragments_size) {
    const uint32_t remaining = state.read_fragments_size - state.read_fragments_ptr;
    const uint32_t logged = std::min<uint32_t>(remaining, max_bytes);
    std::vector<uint8_t> bytes(logged);
    if (logged > 0 && TryCopyBytes(
          reinterpret_cast<const char*>(static_cast<uint8_t*>(fragments_buffer) + state.read_fragments_ptr),
          logged,
          reinterpret_cast<char*>(bytes.data()))) {
      text += L"fragments_next=[";
      text += FormatBytes(bytes.data(), bytes.size());
      text += L"]";
    } else {
      text += L"fragments_next=[]";
    }
  } else {
    text += L"fragments_next=<unreadable>";
  }

  return text;
}

struct CnwMessageWriteState {
  bool readable = false;
  void* write_buffer = nullptr;
  void* write_fragments = nullptr;
  uint32_t write_buffer_capacity = 0;
  uint32_t write_buffer_bytes = 0;
  uint32_t write_fragment_bytes = 0;
  uint32_t write_fragment_bit_ptr = 0;
  uint8_t write_fragment_bit_count = 0;
  uint32_t write_mode = 0;
  uint32_t field_60 = 0;
  uint32_t field_64 = 0;
};

CnwMessageWriteState ReadCnwMessageWriteState(void* message) {
  CnwMessageWriteState state{};
  if (message == nullptr) {
    return state;
  }

  uint8_t* const base = static_cast<uint8_t*>(message);
  const bool ok =
      ReadPointerAtOffset(message, 0x08, &state.write_buffer) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x10), &state.write_buffer_capacity) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x14), &state.write_buffer_bytes) &&
      ReadPointerAtOffset(message, 0x18, &state.write_fragments) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x24), &state.write_fragment_bytes) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x28), &state.write_fragment_bit_ptr) &&
      SafeReadUInt8(base + 0x2C, &state.write_fragment_bit_count) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x30), &state.write_mode) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x60), &state.field_60) &&
      SafeReadUInt32(reinterpret_cast<const uint32_t*>(base + 0x64), &state.field_64);
  state.readable = ok;
  return state;
}

std::wstring FormatCnwMessageWriteState(const CnwMessageWriteState& state) {
  if (!state.readable) {
    return L"<unreadable>";
  }

  wchar_t buffer[320]{};
  swprintf_s(
      buffer,
      L"write_buffer=%p cap=%u bytes=%u fragments=%p frag_bytes=%u frag_bit=%u frag_bits=%u mode=%u field60=%u/0x%X field64=%u/0x%X",
      state.write_buffer,
      state.write_buffer_capacity,
      state.write_buffer_bytes,
      state.write_fragments,
      state.write_fragment_bytes,
      state.write_fragment_bit_ptr,
      static_cast<unsigned int>(state.write_fragment_bit_count),
      state.write_mode,
      state.field_60,
      state.field_60,
      state.field_64,
      state.field_64);
  return buffer;
}

std::wstring FormatCnwMessageWritePreview(void* message, const CnwMessageWriteState& state, uint32_t max_bytes = 96) {
  if (message == nullptr || !state.readable || state.write_buffer == nullptr) {
    return L"<unavailable>";
  }

  const uint32_t logged_buffer = std::min<uint32_t>(state.write_buffer_bytes, max_bytes);
  std::vector<uint8_t> buffer_bytes(logged_buffer);
  std::wstring text;
  if (logged_buffer > 0 &&
      TryCopyBytes(reinterpret_cast<const char*>(state.write_buffer), logged_buffer, reinterpret_cast<char*>(buffer_bytes.data()))) {
    text += L"write_buffer=[";
    text += FormatBytes(buffer_bytes.data(), buffer_bytes.size());
    text += state.write_buffer_bytes > logged_buffer ? L" ...]" : L"]";
  } else {
    text += L"write_buffer=[]";
  }

  text += L" ";
  if (state.write_fragments != nullptr && state.write_fragment_bytes > 0) {
    const uint32_t logged_fragments = std::min<uint32_t>(state.write_fragment_bytes + 1, max_bytes);
    std::vector<uint8_t> fragment_bytes(logged_fragments);
    if (TryCopyBytes(
            reinterpret_cast<const char*>(state.write_fragments),
            logged_fragments,
            reinterpret_cast<char*>(fragment_bytes.data()))) {
      text += L"fragments=[";
      text += FormatBytes(fragment_bytes.data(), fragment_bytes.size());
      text += state.write_fragment_bytes + 1 > logged_fragments ? L" ...]" : L"]";
    } else {
      text += L"fragments=<read failed>";
    }
  } else {
    text += L"fragments=[]";
  }
  return text;
}

bool SafeReadWriteMessageOutputs(unsigned char** out_message, uint32_t* out_size, unsigned char** message, uint32_t* size) {
  if (message == nullptr || size == nullptr) {
    return false;
  }

  __try {
    *message = out_message != nullptr ? *out_message : nullptr;
    *size = out_size != nullptr ? *out_size : 0;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    *message = nullptr;
    *size = 0;
    return false;
  }
}

bool WriteUInt32AtOffset(void* object, size_t offset, uint32_t value) {
  if (object == nullptr) {
    return false;
  }

  __try {
    *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(object) + offset) = value;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool WriteUInt8AtOffset(void* object, size_t offset, uint8_t value) {
  if (object == nullptr) {
    return false;
  }

  __try {
    *reinterpret_cast<uint8_t*>(static_cast<uint8_t*>(object) + offset) = value;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool PeekReadBufferBytes(void* message, const CnwMessageReadState& state, uint32_t offset, uint32_t length, std::vector<uint8_t>* bytes) {
  if (message == nullptr || bytes == nullptr || !state.readable || offset > state.read_buffer_size ||
      length > state.read_buffer_size - offset) {
    return false;
  }

  void* read_buffer = nullptr;
  if (!ReadPointerAtOffset(message, 0x38, &read_buffer) || read_buffer == nullptr) {
    return false;
  }

  bytes->assign(length, 0);
  return length == 0 ||
      TryCopyBytes(
          reinterpret_cast<const char*>(static_cast<uint8_t*>(read_buffer) + offset),
          length,
          reinterpret_cast<char*>(bytes->data()));
}

bool TryReadU32LeFromBytes(const std::vector<uint8_t>& bytes, size_t offset, uint32_t* value) {
  if (value == nullptr || offset > bytes.size() || bytes.size() - offset < 4) {
    return false;
  }

  *value =
      static_cast<uint32_t>(bytes[offset]) |
      (static_cast<uint32_t>(bytes[offset + 1]) << 8) |
      (static_cast<uint32_t>(bytes[offset + 2]) << 16) |
      (static_cast<uint32_t>(bytes[offset + 3]) << 24);
  return true;
}

bool TryReadU16LeFromBytes(const std::vector<uint8_t>& bytes, size_t offset, uint16_t* value) {
  if (value == nullptr || offset > bytes.size() || bytes.size() - offset < 2) {
    return false;
  }

  *value =
      static_cast<uint16_t>(bytes[offset]) |
      static_cast<uint16_t>(static_cast<uint16_t>(bytes[offset + 1]) << 8);
  return true;
}

uint16_t ReadU16BeRaw(const unsigned char* bytes) {
  return static_cast<uint16_t>(
      (static_cast<uint16_t>(bytes[0]) << 8) |
      static_cast<uint16_t>(bytes[1]));
}

bool TryReadFloatLeFromBytes(const std::vector<uint8_t>& bytes, size_t offset, float* value) {
  uint32_t raw = 0;
  if (value == nullptr || !TryReadU32LeFromBytes(bytes, offset, &raw)) {
    return false;
  }
  static_assert(sizeof(float) == sizeof(uint32_t), "float must be 32-bit");
  std::memcpy(value, &raw, sizeof(float));
  return true;
}

bool TryReadU32LeFromRaw(const unsigned char* bytes, uint32_t length, uint32_t offset, uint32_t* value) {
  if (bytes == nullptr || value == nullptr || offset > length || length - offset < 4) {
    return false;
  }
  *value =
      static_cast<uint32_t>(bytes[offset]) |
      (static_cast<uint32_t>(bytes[offset + 1]) << 8) |
      (static_cast<uint32_t>(bytes[offset + 2]) << 16) |
      (static_cast<uint32_t>(bytes[offset + 3]) << 24);
  return true;
}

bool TryReadFloatLeFromRaw(const unsigned char* bytes, uint32_t length, uint32_t offset, float* value) {
  uint32_t raw = 0;
  if (value == nullptr || !TryReadU32LeFromRaw(bytes, length, offset, &raw)) {
    return false;
  }
  static_assert(sizeof(float) == sizeof(uint32_t), "float must be 32-bit");
  std::memcpy(value, &raw, sizeof(float));
  return true;
}

bool WriteU32LeToRaw(unsigned char* bytes, size_t length, size_t offset, uint32_t value) {
  if (bytes == nullptr || offset > length || length - offset < 4) {
    return false;
  }
  bytes[offset + 0] = static_cast<unsigned char>(value & 0xFFu);
  bytes[offset + 1] = static_cast<unsigned char>((value >> 8) & 0xFFu);
  bytes[offset + 2] = static_cast<unsigned char>((value >> 16) & 0xFFu);
  bytes[offset + 3] = static_cast<unsigned char>((value >> 24) & 0xFFu);
  return true;
}

bool WriteFloatLeToRaw(unsigned char* bytes, size_t length, size_t offset, float value) {
  uint32_t raw = 0;
  static_assert(sizeof(float) == sizeof(uint32_t), "float must be 32-bit");
  std::memcpy(&raw, &value, sizeof(raw));
  return WriteU32LeToRaw(bytes, length, offset, raw);
}

const wchar_t* ObjectRegistryTypeName(uint8_t object_type) {
  switch (object_type) {
    case 5:
      return L"creature";
    case 7:
      return L"trigger";
    case 9:
      return L"placeable";
    case 10:
      return L"door";
    default:
      return L"object";
  }
}

bool IsValidObjectIdForDiagnostics(uint32_t object_id) {
  return object_id != 0 && object_id != 0x7F000000 && object_id != 0xFFFFFFFF;
}

bool IsPlausibleObjectCoordinate(float value) {
  return std::isfinite(value) && value >= -100000.0f && value <= 100000.0f;
}

bool IsPlausibleLegacyObjectScale(float value) {
  return std::isfinite(value) && value > 0.0f && value <= 100.0f;
}

bool IsPlausibleObjectPosition(float x, float y, float z) {
  return IsPlausibleObjectCoordinate(x) &&
      IsPlausibleObjectCoordinate(y) &&
      IsPlausibleObjectCoordinate(z);
}

bool IsNearOriginNoisePosition(float x, float y, float z) {
  return std::fabs(x) < 1.0f &&
      std::fabs(y) < 1.0f &&
      std::fabs(z) < 1.0f;
}

bool TryReadGobPosition(void* object, float* x, float* y, float* z) {
  if (object == nullptr || x == nullptr || y == nullptr || z == nullptr) {
    return false;
  }

  // EE's exported Gob::GetPosition copies Vector from offsets 0xA8..0xB0.
  float observed_x = 0.0f;
  float observed_y = 0.0f;
  float observed_z = 0.0f;
  if (!SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + 0xA8), &observed_x) ||
      !SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + 0xAC), &observed_y) ||
      !SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + 0xB0), &observed_z)) {
    return false;
  }
  if (!IsPlausibleObjectPosition(observed_x, observed_y, observed_z)) {
    return false;
  }
  if (IsNearOriginNoisePosition(observed_x, observed_y, observed_z)) {
    return false;
  }

  *x = observed_x;
  *y = observed_y;
  *z = observed_z;
  return true;
}

bool TryGetPlaceableInteractionObject(void* placeable, void** interaction_object) {
  if (interaction_object == nullptr) {
    return false;
  }
  *interaction_object = nullptr;
  if (placeable == nullptr) {
    return false;
  }

  void* vtable = nullptr;
  void* function_pointer = nullptr;
  if (!ReadPointerAtOffset(placeable, 0x00, &vtable) ||
      vtable == nullptr ||
      !ReadPointerAtOffset(vtable, 0xA0, &function_pointer) ||
      function_pointer == nullptr) {
    return false;
  }

  using PlaceableInteractionObjectFn = void*(__fastcall*)(void*);
  auto get_interaction_object = reinterpret_cast<PlaceableInteractionObjectFn>(function_pointer);
  __try {
    *interaction_object = get_interaction_object(placeable);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    *interaction_object = nullptr;
    return false;
  }
}

bool LiveObjectPointerMatchesObjectId(void* object, uint32_t object_id) {
  if (object == nullptr || !IsValidObjectIdForDiagnostics(object_id)) {
    return false;
  }

  uint32_t observed_object_id = 0;
  return SafeReadUInt32(reinterpret_cast<const uint32_t*>(static_cast<uint8_t*>(object) + 0x08), &observed_object_id) &&
      observed_object_id == object_id;
}

void RememberLiveObjectPointerForDiagnostics(uint32_t object_id, uint8_t object_type, void* object) {
  if (!IsValidObjectIdForDiagnostics(object_id) ||
      object == nullptr ||
      !LiveObjectPointerMatchesObjectId(object, object_id)) {
    return;
  }

  LiveObjectPointerRegistryEntry entry{};
  entry.object_id = object_id;
  entry.object_type = object_type;
  entry.object = object;
  entry.tick = GetTickCount();

  bool changed = false;
  AcquireSRWLockExclusive(&g_live_object_pointer_registry_lock);
  for (LiveObjectPointerRegistryEntry& existing : g_live_object_pointer_registry) {
    if (existing.object_id != object_id) {
      continue;
    }
    changed = existing.object_type != object_type || existing.object != object;
    existing = entry;
    ReleaseSRWLockExclusive(&g_live_object_pointer_registry_lock);
    if (changed) {
      const LONG observation =
          InterlockedIncrement(&g_live_object_pointer_registry_observations);
      if (observation <= 120) {
        LogFormat(
            L"live object pointer registry update #%ld: id=0x%08X type=%u/%s object=%p",
            observation,
            object_id,
            static_cast<unsigned int>(object_type),
            ObjectRegistryTypeName(object_type),
            object);
      }
    }
    return;
  }

  constexpr size_t kMaxLiveObjectPointerRegistryEntries = 4096;
  if (g_live_object_pointer_registry.size() >= kMaxLiveObjectPointerRegistryEntries) {
    g_live_object_pointer_registry.erase(g_live_object_pointer_registry.begin());
  }
  g_live_object_pointer_registry.push_back(entry);
  ReleaseSRWLockExclusive(&g_live_object_pointer_registry_lock);

  const LONG observation =
      InterlockedIncrement(&g_live_object_pointer_registry_observations);
  if (observation <= 120) {
    LogFormat(
        L"live object pointer registry insert #%ld: id=0x%08X type=%u/%s object=%p",
        observation,
        object_id,
        static_cast<unsigned int>(object_type),
        ObjectRegistryTypeName(object_type),
        object);
  }
}

bool TryGetLiveObjectPointerForDiagnostics(uint32_t object_id, void** object) {
  if (object != nullptr) {
    *object = nullptr;
  }
  if (!IsValidObjectIdForDiagnostics(object_id)) {
    return false;
  }

  constexpr DWORD kMaxPointerAgeMs = 10 * 60 * 1000;
  const DWORD now = GetTickCount();
  AcquireSRWLockShared(&g_live_object_pointer_registry_lock);
  for (auto it = g_live_object_pointer_registry.rbegin();
       it != g_live_object_pointer_registry.rend();
       ++it) {
    if (it->object_id != object_id ||
        it->object == nullptr ||
        (it->tick != 0 && now - it->tick > kMaxPointerAgeMs)) {
      continue;
    }
    void* const candidate = it->object;
    if (!LiveObjectPointerMatchesObjectId(candidate, object_id)) {
      continue;
    }
    if (object != nullptr) {
      *object = candidate;
    }
    ReleaseSRWLockShared(&g_live_object_pointer_registry_lock);
    return true;
  }
  ReleaseSRWLockShared(&g_live_object_pointer_registry_lock);
  return false;
}

bool TryResolveNwcObjectById(uint32_t object_id, void** game_object, void** nwc_object) {
  if (game_object != nullptr) {
    *game_object = nullptr;
  }
  if (nwc_object != nullptr) {
    *nwc_object = nullptr;
  }
  if (!IsValidObjectIdForDiagnostics(object_id)) {
    return false;
  }

  void* const app_manager = GetCurrentAppManager();
  auto* const get_game_object =
      ResolveMainRva<AppManagerGetObjectByIdFn>(kEeAppManagerGetGameObjectRva);
  if (app_manager == nullptr || get_game_object == nullptr) {
    return false;
  }

  void* resolved_game_object = nullptr;
  void* resolved_nwc_object = nullptr;
  __try {
    resolved_game_object = get_game_object(app_manager, object_id);
    if (resolved_game_object != nullptr) {
      void* vtable = nullptr;
      void* as_client_object = nullptr;
      if (ReadPointerAtOffset(resolved_game_object, 0x00, &vtable) &&
          vtable != nullptr &&
          ReadPointerAtOffset(vtable, 0x18, &as_client_object) &&
          as_client_object != nullptr) {
        resolved_nwc_object =
            reinterpret_cast<NwcObjectAsClientObjectFn>(as_client_object)(resolved_game_object);
      }
    }
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    resolved_game_object = nullptr;
    resolved_nwc_object = nullptr;
  }

  if (game_object != nullptr) {
    *game_object = resolved_game_object;
  }
  if (nwc_object != nullptr) {
    *nwc_object = resolved_nwc_object;
  }
  if (resolved_nwc_object != nullptr) {
    return true;
  }

  void* cached_nwc_object = nullptr;
  if (!TryGetLiveObjectPointerForDiagnostics(object_id, &cached_nwc_object)) {
    return false;
  }
  if (nwc_object != nullptr) {
    *nwc_object = cached_nwc_object;
  }
  return true;
}

bool TryReadGobOrientation(void* object, Vector3f* orientation) {
  if (object == nullptr || orientation == nullptr) {
    return false;
  }

  Vector3f value{};
  if (!SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + 0xB4), &value.x) ||
      !SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + 0xB8), &value.y) ||
      !SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + 0xBC), &value.z)) {
    return false;
  }
  if (!IsPlausibleObjectCoordinate(value.x) ||
      !IsPlausibleObjectCoordinate(value.y) ||
      !IsPlausibleObjectCoordinate(value.z)) {
    return false;
  }

  *orientation = value;
  return true;
}

bool TrySetGobOrientation(void* object, const Vector3f& orientation) {
  if (object == nullptr ||
      !IsPlausibleObjectCoordinate(orientation.x) ||
      !IsPlausibleObjectCoordinate(orientation.y) ||
      !IsPlausibleObjectCoordinate(orientation.z)) {
    return false;
  }

  void* vtable = nullptr;
  void* set_orientation = nullptr;
  if (!ReadPointerAtOffset(object, 0x00, &vtable) ||
      vtable == nullptr ||
      !ReadPointerAtOffset(vtable, 0x120, &set_orientation) ||
      set_orientation == nullptr) {
    return false;
  }

  __try {
    reinterpret_cast<NwcObjectSetVectorFn>(set_orientation)(object, &orientation);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool TryReadGobScale(void* object, float* scale) {
  if (object == nullptr || scale == nullptr) {
    return false;
  }

  float value = 0.0f;
  if (!SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + 0x60), &value) ||
      !IsPlausibleLegacyObjectScale(value)) {
    return false;
  }

  *scale = value;
  return true;
}

bool TrySetGobScale(void* object, float scale) {
  if (object == nullptr || !IsPlausibleLegacyObjectScale(scale)) {
    return false;
  }

  return SafeWriteFloat(reinterpret_cast<float*>(static_cast<uint8_t*>(object) + 0x60), scale);
}

bool TrySetGobAppearanceState(void* object, uint16_t state) {
  if (object == nullptr) {
    return false;
  }

  auto* const set_appearance_state =
      ResolveMainRva<NwcObjectSetAppearanceStateFn>(kEeNwcObjectSetAppearanceStateRva);
  if (set_appearance_state == nullptr) {
    return false;
  }

  __try {
    set_appearance_state(object, state);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

std::wstring FormatVector3f(const Vector3f& value) {
  wchar_t text[96]{};
  swprintf_s(
      text,
      L"(%.4f, %.4f, %.4f)",
      static_cast<double>(value.x),
      static_cast<double>(value.y),
      static_cast<double>(value.z));
  return text;
}

std::wstring FormatObjectRegistrySpatialSuffix(const ObjectNameRegistryEntry& entry) {
  std::wstring suffix;
  if (entry.has_position) {
    wchar_t text[160]{};
    swprintf_s(
        text,
        L" pos=(%.3f, %.3f, %.3f)",
        static_cast<double>(entry.x),
        static_cast<double>(entry.y),
        static_cast<double>(entry.z));
    suffix += text;
  }
  if (entry.has_bounds) {
    wchar_t text[240]{};
    swprintf_s(
        text,
        L" bounds=[x %.3f..%.3f y %.3f..%.3f z %.3f..%.3f]",
        static_cast<double>(entry.min_x),
        static_cast<double>(entry.max_x),
        static_cast<double>(entry.min_y),
        static_cast<double>(entry.max_y),
        static_cast<double>(entry.min_z),
        static_cast<double>(entry.max_z));
    suffix += text;
  }
  return suffix;
}

void RememberObjectNameForDiagnostics(
    uint32_t object_id,
    uint8_t object_type,
    const std::string& name,
    int useable = -1,
    uint32_t transition_target = 0x7F000000,
    bool has_position = false,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f,
    bool has_bounds = false,
    float min_x = 0.0f,
    float min_y = 0.0f,
    float min_z = 0.0f,
    float max_x = 0.0f,
    float max_y = 0.0f,
    float max_z = 0.0f) {
  if (!IsValidObjectIdForDiagnostics(object_id)) {
    return;
  }

  ObjectNameRegistryEntry entry{};
  entry.object_id = object_id;
  entry.object_type = object_type;
  entry.useable = useable;
  entry.transition_target = transition_target;
  entry.has_position = has_position && IsPlausibleObjectPosition(x, y, z);
  entry.x = entry.has_position ? x : 0.0f;
  entry.y = entry.has_position ? y : 0.0f;
  entry.z = entry.has_position ? z : 0.0f;
  entry.has_bounds = has_bounds &&
      IsPlausibleObjectPosition(min_x, min_y, min_z) &&
      IsPlausibleObjectPosition(max_x, max_y, max_z) &&
      min_x <= max_x &&
      min_y <= max_y &&
      min_z <= max_z;
  entry.min_x = entry.has_bounds ? min_x : 0.0f;
  entry.min_y = entry.has_bounds ? min_y : 0.0f;
  entry.min_z = entry.has_bounds ? min_z : 0.0f;
  entry.max_x = entry.has_bounds ? max_x : 0.0f;
  entry.max_y = entry.has_bounds ? max_y : 0.0f;
  entry.max_z = entry.has_bounds ? max_z : 0.0f;
  entry.name = name.substr(0, 192);
  entry.tick = GetTickCount();

  bool inserted = false;
  bool changed = false;
  AcquireSRWLockExclusive(&g_object_name_registry_lock);
  for (ObjectNameRegistryEntry& existing : g_object_name_registry) {
    if (existing.object_id != object_id) {
      continue;
    }
    ObjectNameRegistryEntry merged = entry;
    if (merged.useable < 0 && existing.useable >= 0) {
      merged.useable = existing.useable;
    }
    if (merged.transition_target == 0x7F000000 && existing.transition_target != 0x7F000000) {
      merged.transition_target = existing.transition_target;
    }
    if (!merged.has_position && existing.has_position) {
      merged.has_position = true;
      merged.x = existing.x;
      merged.y = existing.y;
      merged.z = existing.z;
    }
    if (!merged.has_bounds && existing.has_bounds) {
      merged.has_bounds = true;
      merged.min_x = existing.min_x;
      merged.min_y = existing.min_y;
      merged.min_z = existing.min_z;
      merged.max_x = existing.max_x;
      merged.max_y = existing.max_y;
      merged.max_z = existing.max_z;
    }
    if (merged.name.empty() && !existing.name.empty()) {
      merged.name = existing.name;
    }
    changed =
        existing.object_type != merged.object_type ||
        existing.useable != merged.useable ||
        existing.transition_target != merged.transition_target ||
        existing.has_position != merged.has_position ||
        existing.x != merged.x ||
        existing.y != merged.y ||
        existing.z != merged.z ||
        existing.has_bounds != merged.has_bounds ||
        existing.min_x != merged.min_x ||
        existing.min_y != merged.min_y ||
        existing.min_z != merged.min_z ||
        existing.max_x != merged.max_x ||
        existing.max_y != merged.max_y ||
        existing.max_z != merged.max_z ||
        existing.name != merged.name;
    existing = merged;
    entry = merged;
    ReleaseSRWLockExclusive(&g_object_name_registry_lock);
    if (changed) {
      const LONG observation = InterlockedIncrement(&g_object_name_registry_observations);
      const bool interesting = !entry.name.empty() && (useable >= 0 || transition_target != 0x7F000000);
      if (observation <= 120 || interesting) {
        LogFormat(
            L"object registry update #%ld: id=0x%08X type=%u/%s useable=%d transition-target=0x%08X name='%s'%s",
            observation,
            object_id,
            static_cast<unsigned int>(object_type),
            ObjectRegistryTypeName(object_type),
            entry.useable,
            transition_target,
            NarrowToWide(entry.name).c_str(),
            FormatObjectRegistrySpatialSuffix(entry).c_str());
      }
    }
    return;
  }

  constexpr size_t kMaxObjectNameRegistryEntries = 4096;
  if (g_object_name_registry.size() >= kMaxObjectNameRegistryEntries) {
    g_object_name_registry.erase(g_object_name_registry.begin());
  }
  g_object_name_registry.push_back(entry);
  inserted = true;
  ReleaseSRWLockExclusive(&g_object_name_registry_lock);

  if (inserted) {
    const LONG observation = InterlockedIncrement(&g_object_name_registry_observations);
    const bool interesting = !entry.name.empty() && (useable >= 0 || transition_target != 0x7F000000);
    if (observation <= 120 || interesting) {
      LogFormat(
          L"object registry insert #%ld: id=0x%08X type=%u/%s useable=%d transition-target=0x%08X name='%s'%s",
          observation,
          object_id,
          static_cast<unsigned int>(object_type),
          ObjectRegistryTypeName(object_type),
          entry.useable,
          transition_target,
          NarrowToWide(entry.name).c_str(),
          FormatObjectRegistrySpatialSuffix(entry).c_str());
    }
  }
}

void RememberObjectPositionForDiagnostics(
    uint32_t object_id,
    uint8_t object_type,
    float x,
    float y,
    float z,
    const wchar_t* source_label) {
  if (!IsValidObjectIdForDiagnostics(object_id) ||
      !IsPlausibleObjectPosition(x, y, z) ||
      IsNearOriginNoisePosition(x, y, z)) {
    return;
  }

  ObjectNameRegistryEntry entry{};
  bool inserted = false;
  bool changed = false;

  AcquireSRWLockExclusive(&g_object_name_registry_lock);
  for (ObjectNameRegistryEntry& existing : g_object_name_registry) {
    if (existing.object_id != object_id) {
      continue;
    }

    existing.object_type = object_type != 0 ? object_type : existing.object_type;
    changed =
        !existing.has_position ||
        existing.x != x ||
        existing.y != y ||
        existing.z != z;
    existing.has_position = true;
    existing.x = x;
    existing.y = y;
    existing.z = z;
    existing.tick = GetTickCount();
    entry = existing;
    ReleaseSRWLockExclusive(&g_object_name_registry_lock);

    if (changed) {
      const LONG observation = InterlockedIncrement(&g_object_name_registry_observations);
      const bool interesting =
          entry.object_type == 7 ||
          entry.object_type == 10 ||
          entry.useable >= 0 ||
          entry.transition_target != 0x7F000000 ||
          !entry.name.empty();
      if (observation <= 240 || interesting) {
        LogFormat(
            L"object registry position update #%ld: id=0x%08X type=%u/%s source=%s name='%s'%s",
            observation,
            object_id,
            static_cast<unsigned int>(entry.object_type),
            ObjectRegistryTypeName(entry.object_type),
            source_label != nullptr ? source_label : L"<unknown>",
            NarrowToWide(entry.name).c_str(),
            FormatObjectRegistrySpatialSuffix(entry).c_str());
      }
    }
    return;
  }

  entry.object_id = object_id;
  entry.object_type = object_type;
  entry.useable = -1;
  entry.transition_target = 0x7F000000;
  entry.has_position = true;
  entry.x = x;
  entry.y = y;
  entry.z = z;
  entry.tick = GetTickCount();

  constexpr size_t kMaxObjectNameRegistryEntries = 4096;
  if (g_object_name_registry.size() >= kMaxObjectNameRegistryEntries) {
    g_object_name_registry.erase(g_object_name_registry.begin());
  }
  g_object_name_registry.push_back(entry);
  inserted = true;
  ReleaseSRWLockExclusive(&g_object_name_registry_lock);

  if (inserted) {
    const LONG observation = InterlockedIncrement(&g_object_name_registry_observations);
    if (observation <= 240 || object_type == 7 || object_type == 10) {
      LogFormat(
          L"object registry position insert #%ld: id=0x%08X type=%u/%s source=%s%s",
          observation,
          object_id,
          static_cast<unsigned int>(object_type),
          ObjectRegistryTypeName(object_type),
          source_label != nullptr ? source_label : L"<unknown>",
          FormatObjectRegistrySpatialSuffix(entry).c_str());
    }
  }
}

std::wstring FormatObjectNameRegistryLookup(uint32_t object_id) {
  if (!IsValidObjectIdForDiagnostics(object_id)) {
    return L" object-registry=<invalid>";
  }

  ObjectNameRegistryEntry match{};
  bool found = false;
  AcquireSRWLockShared(&g_object_name_registry_lock);
  for (auto it = g_object_name_registry.rbegin(); it != g_object_name_registry.rend(); ++it) {
    if (it->object_id == object_id) {
      match = *it;
      found = true;
      break;
    }
  }
  ReleaseSRWLockShared(&g_object_name_registry_lock);

  if (!found) {
    return L" object-registry=<unknown>";
  }

  wchar_t buffer[512]{};
  swprintf_s(
      buffer,
      L" object-registry={type=%u/%s useable=%d transition-target=0x%08X age-ms=%lu name='%s'%s}",
      static_cast<unsigned int>(match.object_type),
      ObjectRegistryTypeName(match.object_type),
      match.useable,
      match.transition_target,
      GetTickCount() - match.tick,
      NarrowToWide(match.name).c_str(),
      FormatObjectRegistrySpatialSuffix(match).c_str());
  return buffer;
}

bool TryGetObjectRegistryEntry(uint32_t object_id, ObjectNameRegistryEntry* entry) {
  if (entry != nullptr) {
    *entry = ObjectNameRegistryEntry{};
  }
  if (entry == nullptr || !IsValidObjectIdForDiagnostics(object_id)) {
    return false;
  }

  AcquireSRWLockShared(&g_object_name_registry_lock);
  for (auto it = g_object_name_registry.rbegin(); it != g_object_name_registry.rend(); ++it) {
    if (it->object_id == object_id) {
      *entry = *it;
      ReleaseSRWLockShared(&g_object_name_registry_lock);
      return true;
    }
  }
  ReleaseSRWLockShared(&g_object_name_registry_lock);
  return false;
}

float DistanceSquaredToObjectRegistryEntry(const ObjectNameRegistryEntry& entry, float x, float y, float z) {
  if (entry.has_bounds) {
    const float clamped_x = std::min(std::max(x, entry.min_x), entry.max_x);
    const float clamped_y = std::min(std::max(y, entry.min_y), entry.max_y);
    const float clamped_z = std::min(std::max(z, entry.min_z), entry.max_z);
    const float dx = x - clamped_x;
    const float dy = y - clamped_y;
    const float dz = z - clamped_z;
    return dx * dx + dy * dy + dz * dz;
  }
  if (entry.has_position) {
    const float dx = x - entry.x;
    const float dy = y - entry.y;
    const float dz = z - entry.z;
    return dx * dx + dy * dy + dz * dz;
  }
  return 1.0e30f;
}

std::wstring FormatNearbyTransitionRegistry(float x, float y, float z) {
  if (!IsPlausibleObjectPosition(x, y, z)) {
    return L"";
  }

  struct NearbyEntry {
    float distance_squared = 0.0f;
    ObjectNameRegistryEntry entry;
  };

  std::vector<NearbyEntry> nearby;
  AcquireSRWLockShared(&g_object_name_registry_lock);
  for (const ObjectNameRegistryEntry& entry : g_object_name_registry) {
    if (entry.object_type != 7 && entry.object_type != 9 && entry.object_type != 10) {
      continue;
    }
    if (!entry.has_position && !entry.has_bounds) {
      continue;
    }
    const bool transition_like =
        entry.object_type == 7 ||
        entry.object_type == 10 ||
        entry.transition_target != 0x7F000000 ||
        entry.useable > 0 ||
        !entry.name.empty();
    if (!transition_like) {
      continue;
    }
    const float distance_squared = DistanceSquaredToObjectRegistryEntry(entry, x, y, z);
    if (distance_squared <= 900.0f) {
      nearby.push_back(NearbyEntry{distance_squared, entry});
    }
  }
  ReleaseSRWLockShared(&g_object_name_registry_lock);

  if (nearby.empty()) {
    return L" nearby-transitions=<none-with-position>";
  }

  std::sort(
      nearby.begin(),
      nearby.end(),
      [](const NearbyEntry& left, const NearbyEntry& right) {
        return left.distance_squared < right.distance_squared;
      });

  std::wstring entries;
  const size_t count = std::min<size_t>(nearby.size(), 8);
  for (size_t index = 0; index < count; ++index) {
    const NearbyEntry& candidate = nearby[index];
    wchar_t text[640]{};
    swprintf_s(
        text,
        L"%s0x%08X %u/%s d=%.2f useable=%d trans=0x%08X age-ms=%lu name='%s'%s",
        entries.empty() ? L"" : L"; ",
        candidate.entry.object_id,
        static_cast<unsigned int>(candidate.entry.object_type),
        ObjectRegistryTypeName(candidate.entry.object_type),
        static_cast<double>(std::sqrt(candidate.distance_squared)),
        candidate.entry.useable,
        candidate.entry.transition_target,
        GetTickCount() - candidate.entry.tick,
        NarrowToWide(candidate.entry.name).c_str(),
        FormatObjectRegistrySpatialSuffix(candidate.entry).c_str());
    entries += text;
  }

  std::wstring result = L" nearby-transitions=[";
  result += entries;
  result += L"]";
  return result;
}

std::wstring FormatClientInputPayloadDetail(
    unsigned char major,
    unsigned char minor,
    const unsigned char* payload,
    uint32_t payload_size) {
  if (payload == nullptr || payload_size == 0) {
    return L"";
  }

  if (major != 6 && major != 4 && major != 2) {
    return L"";
  }

  std::wstring detail;
  if (payload_size >= 7 && payload[0] == 0x70) {
    uint32_t declared_size = 0;
    TryReadU32LeFromRaw(payload, payload_size, 3, &declared_size);
    wchar_t header[128]{};
    swprintf_s(
        header,
        L" write-header=[70 %02X %02X] declared=%u",
        static_cast<unsigned int>(payload[1]),
        static_cast<unsigned int>(payload[2]),
        declared_size);
    detail += header;
  }

  if (major == 6 && minor == 0x0B) {
    const uint32_t data_offset = payload[0] == 0x70 && payload_size >= 11 ? 7 : 0;
    uint32_t target_id = 0;
    const bool target_ok = TryReadU32LeFromRaw(payload, payload_size, data_offset, &target_id);
    const bool action_byte_ok = data_offset + 4 < payload_size;
    if (target_ok) {
      wchar_t input[320]{};
      swprintf_s(
          input,
          L" use-object-target=0x%08X/%u action-byte=%s%02X data-offset=%u",
          target_id,
          target_id,
          action_byte_ok ? L"0x" : L"<none>/0x",
          action_byte_ok ? static_cast<unsigned int>(payload[data_offset + 4]) : 0,
          data_offset);
      detail += input;
      detail += FormatObjectNameRegistryLookup(target_id);
    }
    return detail;
  }

  if (major == 6 && payload_size >= 23) {
    const uint32_t data_offset = payload[0] == 0x70 && payload_size >= 11 ? 7 : 0;
    uint32_t target_id = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    const bool target_ok = TryReadU32LeFromRaw(payload, payload_size, data_offset, &target_id);
    const bool x_ok = TryReadFloatLeFromRaw(payload, payload_size, data_offset + 4, &x);
    const bool y_ok = TryReadFloatLeFromRaw(payload, payload_size, data_offset + 8, &y);
    const bool z_ok = TryReadFloatLeFromRaw(payload, payload_size, data_offset + 12, &z);
    if (target_ok || (x_ok && y_ok)) {
      uint32_t action_object_id = 0;
      const uint32_t action_object_offset = data_offset + 18;
      const bool action_object_ok =
          payload_size >= action_object_offset + 5 &&
          TryReadU32LeFromRaw(payload, payload_size, action_object_offset, &action_object_id) &&
          IsValidObjectIdForDiagnostics(action_object_id);
      const bool action_byte_ok = action_object_ok && action_object_offset + 4 < payload_size;
      wchar_t action_suffix[128]{};
      if (action_object_ok) {
        swprintf_s(
            action_suffix,
            L" action-object=0x%08X/%u action-byte=%s%02X",
            action_object_id,
            action_object_id,
            action_byte_ok ? L"0x" : L"<none>/0x",
            action_byte_ok ? static_cast<unsigned int>(payload[action_object_offset + 4]) : 0);
      }
      wchar_t input[384]{};
      swprintf_s(
          input,
          L" input-target=0x%08X/%u position=(%.3f, %.3f, %.3f) data-offset=%u%s",
          target_id,
          target_id,
          x_ok ? static_cast<double>(x) : 0.0,
          y_ok ? static_cast<double>(y) : 0.0,
          z_ok ? static_cast<double>(z) : 0.0,
          data_offset,
          action_suffix);
      detail += input;
      if (action_object_ok) {
        detail += FormatObjectNameRegistryLookup(action_object_id);
      }
      if (x_ok && y_ok && z_ok) {
        detail += FormatNearbyTransitionRegistry(x, y, z);
      }
    }
  }

  return detail;
}

std::string LegacyResRefFromBytes(const std::vector<uint8_t>& bytes, size_t offset) {
  constexpr size_t kResRefBytes = 16;
  if (offset > bytes.size() || bytes.size() - offset < kResRefBytes) {
    return {};
  }

  size_t length = 0;
  while (length < kResRefBytes && bytes[offset + length] != 0) {
    ++length;
  }
  return std::string(
      reinterpret_cast<const char*>(bytes.data() + offset),
      reinterpret_cast<const char*>(bytes.data() + offset + length));
}

std::wstring JoinAsciiList(const std::vector<std::string>& values) {
  std::wstring joined;
  for (size_t index = 0; index < values.size(); ++index) {
    if (index != 0) {
      joined += L", ";
    }
    joined += NarrowToWide(values[index]);
  }
  return joined;
}

void AddUniquePath(std::vector<std::wstring>* paths, const std::wstring& path) {
  if (paths == nullptr || path.empty()) {
    return;
  }
  const std::wstring lowered = LowerAscii(path);
  for (const std::wstring& existing : *paths) {
    if (LowerAscii(existing) == lowered) {
      return;
    }
  }
  paths->push_back(path);
}

std::vector<std::wstring> GetHgAssetRoots() {
  std::vector<std::wstring> roots;

  const std::wstring bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!bundle.empty()) {
    AddUniquePath(&roots, JoinPath(bundle, L"hg-override"));
    AddUniquePath(&roots, JoinPath(bundle, L"hg-gui"));
    AddUniquePath(&roots, JoinPath(bundle, L"hg"));
    AddUniquePath(&roots, JoinPath(bundle, L"hg-std"));
    AddUniquePath(&roots, JoinPath(bundle, L"hg-overlay"));

    const std::wstring configured = GetEnvironmentString(L"HG_BRIDGE_HG_ASSET_ROOT");
    AddUniquePath(&roots, configured);
    return roots;
  }

  const std::wstring configured = GetEnvironmentString(L"HG_BRIDGE_HG_ASSET_ROOT");
  AddUniquePath(&roots, configured);

  AddUniquePath(
      &roots,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-override");
  AddUniquePath(
      &roots,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-gui");
  AddUniquePath(
      &roots,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-std");
  AddUniquePath(
      &roots,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-overlay");

  AddUniquePath(&roots, L"C:\\NWN\\Setup\\hghak_override-beta18.en");
  AddUniquePath(&roots, L"C:\\NWN\\Setup\\02 - hghak_override-beta18.en");
  AddUniquePath(&roots, L"C:\\NWN\\Setup\\hghak_gui-beta28.en");
  AddUniquePath(&roots, L"C:\\NWN\\Setup\\01 - hghak_gui-beta28.en");

  const std::wstring user_profile = GetEnvironmentString(L"USERPROFILE");
  if (!user_profile.empty()) {
    AddUniquePath(&roots, JoinPath(JoinPath(user_profile, L"Downloads"), L"hghak_std-beta28.en"));
  }

  const std::wstring workspace = GetEnvironmentString(L"HG_BRIDGE_WORKSPACE");
  if (!workspace.empty()) {
    AddUniquePath(&roots, JoinPath(JoinPath(workspace, L"NWN Utils\\Setup"), L"hghak_override-beta18.en"));
    AddUniquePath(&roots, JoinPath(JoinPath(workspace, L"NWN Utils\\Setup"), L"02 - hghak_override-beta18.en"));
    AddUniquePath(&roots, JoinPath(JoinPath(workspace, L"NWN Utils\\Setup"), L"hghak_gui-beta28.en"));
    AddUniquePath(&roots, JoinPath(JoinPath(workspace, L"NWN Utils\\Setup"), L"01 - hghak_gui-beta28.en"));
    AddUniquePath(&roots, JoinPath(workspace, L"hghak_std-beta28.en"));
    AddUniquePath(&roots, JoinPath(workspace, L"HG Assets"));
  }

  return roots;
}

std::vector<std::wstring> GetHgHakDirectories() {
  std::vector<std::wstring> directories;
  for (const std::wstring& root : GetHgAssetRoots()) {
    AddUniquePath(&directories, JoinPath(root, L"hak"));
    AddUniquePath(&directories, root);
  }
  return directories;
}

std::string ResolveHgHakOverridePath(const std::string& hak) {
  if (hak.empty() || IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_HG_HAK_OVERRIDE")) {
    return {};
  }

  const std::wstring hak_name = NarrowToWide(hak) + L".hak";
  for (const std::wstring& root : GetHgAssetRoots()) {
    const std::wstring nested = JoinPath(JoinPath(root, L"hak"), hak_name);
    if (FileExistsWide(nested)) {
      return ToUtf8(nested);
    }

    const std::wstring direct = JoinPath(root, hak_name);
    if (FileExistsWide(direct)) {
      return ToUtf8(direct);
    }
  }

  return {};
}

std::string ResolveHakAliasPath(
    const char* alias_name,
    const std::vector<std::wstring>& hak_directories,
    const std::string& hak,
    const wchar_t* reason) {
  if (alias_name == nullptr || alias_name[0] == '\0' || hak.empty()) {
    return {};
  }

  const std::wstring hak_name = NarrowToWide(hak) + L".hak";
  for (const std::wstring& directory : hak_directories) {
    const std::wstring candidate = JoinPath(directory, hak_name);
    if (!FileExistsWide(candidate)) {
      continue;
    }
    if (!AddRuntimeAlias(alias_name, directory, reason)) {
      return {};
    }
    return std::string(alias_name) + ":" + hak;
  }

  return {};
}

std::vector<std::string> ResolveLegacyHakDirectPaths(const std::string& hak) {
  std::vector<std::string> paths;
  if (hak.empty() || IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DIRECT_HAK_PATHS")) {
    return paths;
  }

  std::vector<std::wstring> candidates;
  const std::wstring hak_name = NarrowToWide(hak) + L".hak";
  const std::wstring hak_stem = NarrowToWide(hak);
  auto add_candidate_variants = [&](const std::wstring& directory) {
    if (directory.empty()) {
      return;
    }
    AddUniquePath(&candidates, JoinPath(directory, hak_stem));
    AddUniquePath(&candidates, JoinPath(directory, hak_name));
  };

  for (const std::wstring& directory : GetHgHakDirectories()) {
    add_candidate_variants(directory);
  }

  const std::wstring bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!bundle.empty()) {
    add_candidate_variants(JoinPath(JoinPath(bundle, L"hg-gui"), L"hak"));
    add_candidate_variants(JoinPath(JoinPath(bundle, L"hg-std"), L"hak"));
    add_candidate_variants(JoinPath(JoinPath(bundle, L"diamond"), L"hak"));
    add_candidate_variants(JoinPath(JoinPath(bundle, L"cep23"), L"hak"));
  } else {
    for (const std::wstring& directory : GetDiamondHakDirectories()) {
      add_candidate_variants(directory);
    }

    const std::wstring user_profile = GetEnvironmentString(L"USERPROFILE");
    if (!user_profile.empty()) {
      add_candidate_variants(JoinPath(JoinPath(user_profile, L"Documents\\Neverwinter Nights"), L"hak"));
    }

    const std::wstring workspace = GetEnvironmentString(L"HG_BRIDGE_WORKSPACE");
    if (!workspace.empty()) {
      add_candidate_variants(JoinPath(JoinPath(workspace, L"NWN Diamond"), L"hak"));
      add_candidate_variants(JoinPath(JoinPath(workspace, L"Neverwinter Nights EE"), L"hak"));
    }

    add_candidate_variants(
        L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-std\\hak");
    add_candidate_variants(
        L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-gui\\hak");
    add_candidate_variants(
        L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\diamond\\hak");
    add_candidate_variants(L"C:\\NWN\\NWN Diamond\\hak");
    add_candidate_variants(
        L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\cep23\\hak");
    add_candidate_variants(L"C:\\NWN\\Setup\\cep_23_full_c\\hak");
  }

  for (const std::wstring& candidate : candidates) {
    if (FileExistsWide(candidate) || FileExistsWide(candidate + L".hak")) {
      paths.push_back(ToUtf8(candidate));
    }
  }
  return paths;
}

std::vector<std::wstring> GetDiamondRootCandidates() {
  std::vector<std::wstring> candidates;

  const std::wstring bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!bundle.empty()) {
    AddUniquePath(&candidates, JoinPath(bundle, L"diamond"));

    const std::wstring configured = GetEnvironmentString(L"HG_BRIDGE_DIAMOND_ROOT");
    AddUniquePath(&candidates, configured);
    return candidates;
  }
  AddUniquePath(
      &candidates,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\diamond");

  AddUniquePath(&candidates, GetEnvironmentString(L"HG_BRIDGE_DIAMOND_ROOT"));
  AddUniquePath(&candidates, L"C:\\NWN\\NWN Diamond");

  const std::wstring workspace = GetEnvironmentString(L"HG_BRIDGE_WORKSPACE");
  if (!workspace.empty()) {
    AddUniquePath(&candidates, JoinPath(workspace, L"NWN Diamond"));
  }

  const std::wstring module_path = GetModulePath(g_state.module);
  const std::wstring module_dir = ParentPath(module_path);
  const std::wstring build_dir = ParentPath(module_dir);
  const std::wstring repository_root = ParentPath(build_dir);
  if (!repository_root.empty()) {
    AddUniquePath(&candidates, JoinPath(repository_root, L"NWN Diamond"));
  }

  return candidates;
}

std::vector<std::wstring> GetDiamondHakDirectories() {
  std::vector<std::wstring> directories;
  const std::wstring bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!bundle.empty()) {
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-std"), L"hak"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-gui"), L"hak"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"cep23"), L"hak"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"diamond"), L"hak"));
    for (const std::wstring& root : GetDiamondRootCandidates()) {
      AddUniquePath(&directories, JoinPath(root, L"hak"));
    }
    return directories;
  }

  for (const std::wstring& root : GetDiamondRootCandidates()) {
    AddUniquePath(&directories, JoinPath(root, L"hak"));
  }

  AddUniquePath(&directories, L"C:\\NWN\\Setup\\cep_23_full_c\\hak");
  return directories;
}

std::vector<std::wstring> GetLegacyOverrideDirectories() {
  std::vector<std::wstring> directories;

  const std::wstring configured = GetEnvironmentString(L"HG_BRIDGE_OVERRIDE_DIRECTORY");
  AddUniquePath(&directories, configured);

  const std::wstring bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!bundle.empty()) {
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"combined-override"), L"override"));
    AddUniquePath(&directories, JoinPath(bundle, L"combined-override"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-override"), L"override"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-gui"), L"override"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-std"), L"override"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-overlay"), L"override"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"diamond"), L"override"));
    return directories;
  }

  for (const std::wstring& root : GetHgAssetRoots()) {
    AddUniquePath(&directories, JoinPath(root, L"override"));
  }
  for (const std::wstring& root : GetDiamondRootCandidates()) {
    AddUniquePath(&directories, JoinPath(root, L"override"));
  }

  const std::wstring user_profile = GetEnvironmentString(L"USERPROFILE");
  if (!user_profile.empty()) {
    AddUniquePath(&directories, JoinPath(JoinPath(user_profile, L"Documents\\Neverwinter Nights"), L"override"));
  }

  AddUniquePath(
      &directories,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-override\\override");
  AddUniquePath(
      &directories,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-gui\\override");
  AddUniquePath(
      &directories,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\hg-std\\override");
  AddUniquePath(
      &directories,
      L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Neverwinter Nights\\hg-bridge-assets\\diamond\\override");
  AddUniquePath(&directories, L"C:\\NWN\\override");

  return directories;
}

std::wstring ResolveDiamondRootForArchives() {
  const std::vector<std::wstring> candidates = GetDiamondRootCandidates();
  for (const std::wstring& candidate : candidates) {
    if (!candidate.empty() && DirectoryExists(candidate) && FileExistsWide(JoinPath(candidate, L"chitin.key"))) {
      return candidate;
    }
  }
  return L"";
}

int CallAddFixedKeyTableFile(
    void* resman,
    ResManAddFileFn add_file,
    CExoStringFromCharFn construct_string,
    CExoStringDestructorFn destroy_string,
    const char* path_text,
    uint32_t flags,
    bool* raised_exception) {
  return CallAddEncapsulatedResourceFile(
      resman,
      add_file,
      construct_string,
      destroy_string,
      path_text,
      flags,
      raised_exception);
}

void MountDiamondBaseArchivesIfNeeded(const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DIAMOND_ARCHIVE_MOUNT")) {
    LogFormat(
        L"Diamond archive mount skipped for %s: HG_BRIDGE_DISABLE_DIAMOND_ARCHIVE_MOUNT is set",
        reason != nullptr ? reason : L"<unspecified>");
    return;
  }
  if (InterlockedCompareExchange(&g_diamond_archive_mount_state, 1, 0) != 0) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_diamond_archive_mount_observations);
  const std::wstring diamond_root = ResolveDiamondRootForArchives();
  if (diamond_root.empty()) {
    std::wstring candidates_text;
    const std::vector<std::wstring> candidates = GetDiamondRootCandidates();
    for (const std::wstring& candidate : candidates) {
      if (!candidates_text.empty()) {
        candidates_text += L"; ";
      }
      candidates_text += candidate.empty() ? L"<empty>" : candidate;
    }
    LogFormat(
        L"Diamond archive mount #%ld skipped: no Diamond root with chitin.key found for %s candidates=[%s]",
        observation,
        reason != nullptr ? reason : L"<unspecified>",
        candidates_text.c_str());
    return;
  }

  void** const resman_global = ResolveMainExport<void**>("?g_pExoResMan@@3PEAVCExoResMan@@EA");
  void* resman = nullptr;
  SafeReadGlobalPointer(resman_global, &resman);
  auto* const add_fixed = g_resman_add_fixed_key_table_file_original != nullptr
      ? g_resman_add_fixed_key_table_file_original
      : ResolveMainExport<ResManAddFileFn>("?AddFixedKeyTableFile@CExoResMan@@QEAAHAEBVCExoString@@I@Z");
  auto* const add_encapsulated = g_resman_add_encapsulated_resource_file_original != nullptr
      ? g_resman_add_encapsulated_resource_file_original
      : ResolveMainExport<ResManAddFileFn>("?AddEncapsulatedResourceFile@CExoResMan@@QEAAHAEBVCExoString@@I@Z");
  auto* const construct_string = ResolveMainExport<CExoStringFromCharFn>("??0CExoString@@QEAA@PEBD@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");

  LogFormat(
      L"Diamond archive mount #%ld begin: reason=%s root=%s resman_global=%p resman=%p add_fixed=%p add_erf=%p construct=%p destroy=%p",
      observation,
      reason != nullptr ? reason : L"<unspecified>",
      diamond_root.c_str(),
      resman_global,
      resman,
      add_fixed,
      add_encapsulated,
      construct_string,
      destroy_string);

  if (resman == nullptr || construct_string == nullptr || (add_fixed == nullptr && add_encapsulated == nullptr)) {
    LogFormat(L"Diamond archive mount #%ld aborted: missing EE resource-manager API", observation);
    return;
  }

  size_t mounted_keys = 0;
  if (add_fixed != nullptr) {
    static constexpr const wchar_t* kKeyFiles[] = {
        L"chitin.key",
        L"xp1.key",
        L"xp2.key",
        L"xp2patch.key",
        L"xp3.key",
    };

    for (size_t index = 0; index < sizeof(kKeyFiles) / sizeof(kKeyFiles[0]); ++index) {
      const std::wstring path = JoinPath(diamond_root, kKeyFiles[index]);
      if (!FileExistsWide(path)) {
        LogFormat(
            L"Diamond archive mount #%ld fixed key missing: path=%s",
            observation,
            path.c_str());
        continue;
      }

      bool raised_exception = false;
      const uint32_t flags = kDiamondBaseKeyTableAddFlags + static_cast<uint32_t>(index) * 0x000186A0u;
      const int result = CallAddFixedKeyTableFile(
          resman,
          add_fixed,
          construct_string,
          destroy_string,
          ToUtf8(path).c_str(),
          flags,
          &raised_exception);
      LogFormat(
          L"Diamond archive mount #%ld fixed key: path=%s flags=0x%08X result=%d%s",
          observation,
          path.c_str(),
          flags,
          result,
          raised_exception ? L" exception" : L"");
      if (result != 0) {
        ++mounted_keys;
      }
    }
  }

  size_t mounted_erfs = 0;
  if (add_encapsulated != nullptr) {
    static constexpr DiamondTexturePackMount kTexturePacks[] = {
        {L"Tiles_Tpa.erf", "Tiles_Tpa", 0},
        {L"Textures_Tpa.erf", "Textures_Tpa", 1},
        {L"GUI_32bit.erf", "GUI_32bit", 2},
        {L"XP1_GUI.erf", "XP1_GUI", 5},
        {L"xp1_tex_tpa.erf", "xp1_tex_tpa", 4},
        {L"XP2_GUI.erf", "XP2_GUI", 8},
        {L"xp2_tex_tpa.erf", "xp2_tex_tpa", 7},
        {L"Tiles_Tpb.erf", "Tiles_Tpb", 0},
        {L"xp1_tex_tpb.erf", "xp1_tex_tpb", 4},
        {L"xp2_tex_tpb.erf", "xp2_tex_tpb", 7},
        {L"Tiles_Tpc.erf", "Tiles_Tpc", 0},
        {L"Textures_Tpc.erf", "Textures_Tpc", 1},
        {L"xp1_tex_tpc.erf", "xp1_tex_tpc", 4},
        {L"xp2_tex_tpc.erf", "xp2_tex_tpc", 7},
    };

    const std::wstring texturepacks_root = JoinPath(diamond_root, L"texturepacks");
    AddRuntimeAlias("TEXTUREPACKS", texturepacks_root, L"Diamond texturepack archive mount");
    for (const DiamondTexturePackMount& texture_pack : kTexturePacks) {
      const std::wstring path = JoinPath(texturepacks_root, texture_pack.file_name);
      if (!FileExistsWide(path)) {
        LogFormat(
            L"Diamond archive mount #%ld texturepack missing: path=%s priority=%u",
            observation,
            path.c_str(),
            static_cast<unsigned int>(texture_pack.priority));
        continue;
      }

      const std::wstring path_no_extension =
          LowerAscii(path.substr(path.size() >= 4 ? path.size() - 4 : path.size())) == L".erf"
              ? path.substr(0, path.size() - 4)
              : path;
      std::vector<std::pair<std::string, std::wstring>> attempts;
      attempts.emplace_back(ToUtf8(path_no_extension), L"direct without extension");
      attempts.emplace_back(ToUtf8(path), L"direct file");
      attempts.emplace_back(std::string("TEXTUREPACKS:") + texture_pack.alias_name, L"TEXTUREPACKS alias");
      attempts.emplace_back(std::string("TEXTUREPACKS:") + texture_pack.alias_name + ".erf", L"TEXTUREPACKS alias with extension");

      int result = 0;
      bool raised_exception = false;
      for (const auto& attempt : attempts) {
        raised_exception = false;
        result = CallAddEncapsulatedResourceFile(
            resman,
            add_encapsulated,
            construct_string,
            destroy_string,
            attempt.first.c_str(),
            texture_pack.priority,
            &raised_exception);
        LogFormat(
            L"Diamond archive mount #%ld texturepack %s: file=%s path='%s' priority=%u result=%d%s",
            observation,
            attempt.second.c_str(),
            texture_pack.file_name,
            NarrowToWide(attempt.first).c_str(),
            static_cast<unsigned int>(texture_pack.priority),
            result,
            raised_exception ? L" exception" : L"");
        if (result != 0) {
          break;
        }
      }

      LogFormat(
          L"Diamond archive mount #%ld texturepack final: file=%s priority=%u result=%d%s",
          observation,
          texture_pack.file_name,
          static_cast<unsigned int>(texture_pack.priority),
          result,
          raised_exception ? L" exception" : L"");
      if (result != 0) {
        ++mounted_erfs;
      }
    }
  }

  LogFormat(
      L"Diamond archive mount #%ld complete: fixed_keys=%zu texturepacks=%zu root=%s",
      observation,
      mounted_keys,
      mounted_erfs,
      diamond_root.c_str());
  DumpResManKeyTables(resman, L"Diamond archive mount complete");
  BeginRuntimeResourceDiagnostics(L"Diamond archive mount", 180);
}

bool SafeReadGlobalPointer(void** global, void** value) {
  if (global == nullptr || value == nullptr) {
    return false;
  }

  __try {
    *value = *global;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    *value = nullptr;
    return false;
  }
}

int CallAddEncapsulatedResourceFile(
    void* resman,
    ResManAddFileFn add_file,
    CExoStringFromCharFn construct_string,
    CExoStringDestructorFn destroy_string,
    const char* path_text,
    uint32_t flags,
    bool* raised_exception) {
  if (raised_exception != nullptr) {
    *raised_exception = false;
  }
  if (resman == nullptr || add_file == nullptr || construct_string == nullptr || path_text == nullptr || path_text[0] == '\0') {
    return 0;
  }

  CExoStringView path{};
  int result = 0;
  __try {
    construct_string(&path, path_text);
    result = add_file(resman, &path, flags);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    if (raised_exception != nullptr) {
      *raised_exception = true;
    }
  }

  if (path.data != nullptr && destroy_string != nullptr) {
    __try {
      destroy_string(&path);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      if (raised_exception != nullptr) {
        *raised_exception = true;
      }
    }
  }

  return result;
}

int CallAddResourceDirectory(
    void* resman,
    ResManAddResourceDirectoryFn add_directory,
    CExoStringFromCharFn construct_string,
    CExoStringDestructorFn destroy_string,
    const char* path_text,
    uint32_t flags,
    bool* raised_exception) {
  if (raised_exception != nullptr) {
    *raised_exception = false;
  }
  if (resman == nullptr || add_directory == nullptr || construct_string == nullptr ||
      path_text == nullptr || path_text[0] == '\0') {
    return 0;
  }

  CExoStringView path{};
  int result = 0;
  __try {
    construct_string(&path, path_text);
    result = add_directory(resman, &path, flags, 0, nullptr);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    if (raised_exception != nullptr) {
      *raised_exception = true;
    }
  }

  if (path.data != nullptr && destroy_string != nullptr) {
    __try {
      destroy_string(&path);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      if (raised_exception != nullptr) {
        *raised_exception = true;
      }
    }
  }

  return result;
}

bool CallAddAliasNoExcept(
    void* alias_list,
    ExoAliasListAddFn alias_add,
    CExoStringFromCharFn construct_string,
    CExoStringDestructorFn destroy_string,
    const char* alias_name,
    const char* target_directory,
    bool* raised_exception) {
  if (raised_exception != nullptr) {
    *raised_exception = false;
  }
  if (alias_list == nullptr || alias_add == nullptr || construct_string == nullptr ||
      alias_name == nullptr || alias_name[0] == '\0' ||
      target_directory == nullptr || target_directory[0] == '\0') {
    return false;
  }

  CExoStringView alias{};
  CExoStringView target{};
  bool alias_constructed = false;
  bool target_constructed = false;
  bool added = false;

  __try {
    construct_string(&alias, alias_name);
    alias_constructed = true;
    construct_string(&target, target_directory);
    target_constructed = true;
    alias_add(alias_list, &alias, &target);
    added = true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    if (raised_exception != nullptr) {
      *raised_exception = true;
    }
  }

  if (target_constructed && destroy_string != nullptr) {
    __try {
      destroy_string(&target);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      if (raised_exception != nullptr) {
        *raised_exception = true;
      }
    }
  }
  if (alias_constructed && destroy_string != nullptr) {
    __try {
      destroy_string(&alias);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      if (raised_exception != nullptr) {
        *raised_exception = true;
      }
    }
  }

  return added && (raised_exception == nullptr || !*raised_exception);
}

bool AddRuntimeAlias(const char* alias_name, const std::wstring& target_directory, const wchar_t* reason) {
  if (alias_name == nullptr || alias_name[0] == '\0' || target_directory.empty() ||
      !DirectoryExists(target_directory)) {
    LogFormat(
        L"runtime alias add skipped: reason=%s alias=%s target=%s",
        reason != nullptr ? reason : L"<unspecified>",
        alias_name != nullptr ? NarrowToWide(alias_name).c_str() : L"<null>",
        target_directory.empty() ? L"<empty>" : target_directory.c_str());
    return false;
  }

  void** const exo_base_global = ResolveMainExport<void**>("?g_pExoBase@@3PEAVCExoBase@@EA");
  void* exo_base = nullptr;
  SafeReadGlobalPointer(exo_base_global, &exo_base);
  void* const alias_list = exo_base != nullptr ? ReadPointerField(exo_base, 0x20) : nullptr;
  auto* const alias_add = ResolveMainExport<ExoAliasListAddFn>("?Add@CExoAliasList@@QEAAXVCExoString@@0@Z");
  auto* const construct_string = ResolveMainExport<CExoStringFromCharFn>("??0CExoString@@QEAA@PEBD@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");

  if (alias_list == nullptr || alias_add == nullptr || construct_string == nullptr) {
    LogFormat(
        L"runtime alias add failed: reason=%s alias=%s target=%s exo_base_global=%p exo_base=%p alias_list=%p add=%p construct=%p destroy=%p",
        reason != nullptr ? reason : L"<unspecified>",
        NarrowToWide(alias_name).c_str(),
        target_directory.c_str(),
        exo_base_global,
        exo_base,
        alias_list,
        alias_add,
        construct_string,
        destroy_string);
    return false;
  }

  bool raised_exception = false;
  const std::string target_utf8 = ToUtf8(target_directory);
  const bool added = CallAddAliasNoExcept(
      alias_list,
      alias_add,
      construct_string,
      destroy_string,
      alias_name,
      target_utf8.c_str(),
      &raised_exception);

  LogFormat(
      L"runtime alias add: reason=%s alias=%s target=%s exo_base=%p alias_list=%p result=%s%s",
      reason != nullptr ? reason : L"<unspecified>",
      NarrowToWide(alias_name).c_str(),
      target_directory.c_str(),
      exo_base,
      alias_list,
      added ? L"ok" : L"failed",
      raised_exception ? L" exception" : L"");
  return added && !raised_exception;
}

void ReloadRulesAfterLegacyHakMount(size_t mounted_count) {
  if (mounted_count == 0 || IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_MODULE_HAK_RULES_RELOAD")) {
    return;
  }

  void** const rules_global = ResolveMainExport<void**>("?g_pRules@@3PEAVCNWRules@@EA");
  auto* const reload_all = ResolveMainExport<RulesReloadAllFn>("?ReloadAll@CNWRules@@QEAAXXZ");
  void* rules = nullptr;
  if (!SafeReadGlobalPointer(rules_global, &rules) || rules == nullptr || reload_all == nullptr) {
    LogFormat(
        L"legacy module hak rules reload skipped: rules_global=%p rules=%p reload=%p mounted=%zu",
        rules_global,
        rules,
        reload_all,
        mounted_count);
    return;
  }

  bool raised_exception = false;
  __try {
    reload_all(rules);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    raised_exception = true;
  }

  LogFormat(
      L"legacy module hak rules reload: %s mounted=%zu rules=%p",
      raised_exception ? L"exception" : L"ok",
      mounted_count,
      rules);
}

void RememberLegacyModuleHakList(const std::vector<std::string>& haks, const std::string& module_resref) {
  AcquireSRWLockExclusive(&g_legacy_module_hak_lock);
  g_last_legacy_module_haks = haks;
  g_last_legacy_module_resref = module_resref;
  ReleaseSRWLockExclusive(&g_legacy_module_hak_lock);
}

bool GetRememberedLegacyModuleHakList(std::vector<std::string>* haks, std::string* module_resref) {
  if (haks == nullptr || module_resref == nullptr) {
    return false;
  }

  AcquireSRWLockShared(&g_legacy_module_hak_lock);
  *haks = g_last_legacy_module_haks;
  *module_resref = g_last_legacy_module_resref;
  ReleaseSRWLockShared(&g_legacy_module_hak_lock);
  return !haks->empty();
}

void MountLegacyOverrideDirectories(const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_OVERRIDE_DIRECTORY_MOUNT")) {
    LogFormat(
        L"legacy override directory mount skipped for %s: HG_BRIDGE_DISABLE_LEGACY_OVERRIDE_DIRECTORY_MOUNT is set",
        reason != nullptr ? reason : L"<unspecified>");
    return;
  }

  void** const resman_global = ResolveMainExport<void**>("?g_pExoResMan@@3PEAVCExoResMan@@EA");
  void* resman = nullptr;
  SafeReadGlobalPointer(resman_global, &resman);
  auto* const add_directory = ResolveMainExport<ResManAddResourceDirectoryFn>(
      "?AddResourceDirectory@CExoResMan@@QEAAHAEBVCExoString@@IHQ6A_NPEAVCExoKeyTable@@AEBVCResRef@@G@Z@Z");
  auto* const construct_string = ResolveMainExport<CExoStringFromCharFn>("??0CExoString@@QEAA@PEBD@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");

  const LONG observation = InterlockedIncrement(&g_legacy_override_mount_observations);
  const std::vector<std::wstring> directories = GetLegacyOverrideDirectories();
  LogFormat(
      L"legacy override directory mount #%ld begin: reason=%s count=%zu resman_global=%p resman=%p add=%p construct=%p destroy=%p",
      observation,
      reason != nullptr ? reason : L"<unspecified>",
      directories.size(),
      resman_global,
      resman,
      add_directory,
      construct_string,
      destroy_string);

  if (resman == nullptr || add_directory == nullptr || construct_string == nullptr) {
    LogFormat(L"legacy override directory mount #%ld aborted: missing EE resource-manager API", observation);
    return;
  }

  size_t mounted = 0;
  size_t existing = 0;
  for (const std::wstring& directory : directories) {
    if (directory.empty() || !DirectoryExists(directory)) {
      continue;
    }
    ++existing;

    bool raised_exception = false;
    const std::string path = ToUtf8(directory);
    const int result = CallAddResourceDirectory(
        resman,
        add_directory,
        construct_string,
        destroy_string,
        path.c_str(),
        kEeLegacyOverrideDirectoryAddFlags,
        &raised_exception);
    LogFormat(
        L"legacy override directory mount #%ld directory: path=%s flags=0x%08X result=%d%s",
        observation,
        directory.c_str(),
        kEeLegacyOverrideDirectoryAddFlags,
        result,
        raised_exception ? L" exception" : L"");
    if (result != 0) {
      ++mounted;
    }
  }

  LogFormat(
      L"legacy override directory mount #%ld complete: mounted=%zu existing=%zu considered=%zu",
      observation,
      mounted,
      existing,
      directories.size());
}

CResRefView MakeResRefView(const char* name) {
  CResRefView resref{};
  if (name == nullptr) {
    return resref;
  }
  strncpy_s(resref.data, sizeof(resref.data), name, kCResRefTextBytes);
  resref.data[kCResRefTextBytes] = '\0';
  return resref;
}

void ProbeLegacyMountedResources(void* resman, const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_RESOURCE_PROBES")) {
    return;
  }

  auto* const exists = g_resman_exists_original != nullptr
      ? g_resman_exists_original
      : ResolveMainExport<ResManExistsFn>("?Exists@CExoResMan@@QEAAHAEBVCResRef@@GPEAI@Z");
  if (resman == nullptr || exists == nullptr) {
    LogFormat(
        L"legacy resource probe skipped: reason=%s resman=%p exists=%p",
        reason != nullptr ? reason : L"<unspecified>",
        resman,
        exists);
    return;
  }

  struct Probe {
    const char* name;
    uint16_t type;
  };
  constexpr Probe kProbes[] = {
      {"CTL_BTN_DM", 3},
      {"CTL_BTN_DM", 2022},
      {"CTL_BTN_DM", 2033},
      {"CTL_BTN_DM", 2059},
      {"shinywater", 3},
      {"shinywater", 2022},
      {"shinywater", 2033},
      {"shinywater", 2059},
      {"ttr01_grassrim01", 3},
      {"ttr01_grassrim01", 2022},
      {"ttr01_grassrim01", 2033},
      {"ttr01_grassrim01", 2059},
      {"spells", 2017},
      {"baseitems", 2017},
      {"itempropdef", 2017},
      {"iprp_spells", 2017},
      {"iprp_onhitspell", 2017},
      {"is_freemove", 3},
      {"is_hg_enerimm_a", 3},
      {"is_hg_caustwpn", 3},
      {"is_hg_caststone", 3},
      {"is_hg_dsirocco", 3},
      {"is_hg_fontvision", 3},
      {"is_hg_fontpurify", 3},
      {"iit_neck_247", 3},
      {"pfh0_robe137", 6},
      {"pfh0_robe137", 2002},
      {"pmh0_robe161", 6},
      {"pmh0_robe161", 3},
      {"pmh0_robe161", 2022},
      {"pmh0_robe161", 2033},
      {"pmh0_robe161", 2059},
      {"gi_armor01", 2002},
      {"PFH0_HANDR003", 2002},
      {"PFH0_HANDL003", 2002},
      {"PFH0_HANDL003", 6},
      {"PFH0_HANDL003", 3},
      {"PFH0_HANDL003", 2033},
      {"it_potion_000", 2002},
      {"ttr_udoor_01", 2002},
      {"TTR_UDoor_01", 2002},
      {"ttr_udoor_01", 2052},
      {"ttr01_i02_01", 2002},
      {"ttr01_i02_01", 2016},
      {"ttr01_i02_02", 2002},
      {"ttr01_i02_02", 2016},
      {"ttr01_a06_01", 2002},
      {"ttr01_a06_01", 2016},
      {"ttr01_s06_01", 2002},
      {"ttr01_s06_01", 2016},
      {"WMgRd_M_032", 2002},
      {"WMgSt_B_064", 2002},
      {"plx_bench", 2002},
      {"plx_bench", 2053},
      {"plx_crate", 2002},
      {"plx_crate", 2053},
      {"zlc_58d", 2002},
      {"zlc_44d", 2002},
      {"zlc_45d", 2002},
      {"plc_i02", 2002},
      {"plc_i02", 2053},
      {"plc_rock03", 2002},
      {"plc_rock05", 2002},
      {"plc_s05", 2002},
      {"plc_s05", 2053},
      {"plc_s01", 2002},
      {"plc_j02", 2002},
      {"tnp_tree_t22", 2002},
      {"tnp_tree_t63", 2002},
  };

  for (const Probe& probe : kProbes) {
    CResRefView resref = MakeResRefView(probe.name);
    uint32_t size = 0;
    const int result = exists(resman, &resref, probe.type, &size);
    LogFormat(
        L"legacy resource probe: reason=%s name='%s' type=%s result=%d size=%u",
        reason != nullptr ? reason : L"<unspecified>",
        CResRefToWide(&resref).c_str(),
        FormatResType(probe.type).c_str(),
        result,
        size);
  }
}

void MountLegacyModuleHakList(const std::vector<std::string>& haks, const std::string& module_resref) {
  if (haks.empty() || IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_MODULE_HAK_MOUNT")) {
    if (!haks.empty()) {
      LogFormat(
          L"legacy module hak mount skipped: disabled haks=%zu module='%s'",
          haks.size(),
          NarrowToWide(module_resref).c_str());
    }
    return;
  }

  RememberLegacyModuleHakList(haks, module_resref);
  MountDiamondBaseArchivesIfNeeded(L"legacy module hak list");

  void** const resman_global = ResolveMainExport<void**>("?g_pExoResMan@@3PEAVCExoResMan@@EA");
  void* resman = nullptr;
  SafeReadGlobalPointer(resman_global, &resman);

  auto* const add_file = g_resman_add_encapsulated_resource_file_original != nullptr
      ? g_resman_add_encapsulated_resource_file_original
      : ResolveMainExport<ResManAddFileFn>("?AddEncapsulatedResourceFile@CExoResMan@@QEAAHAEBVCExoString@@I@Z");
  auto* const construct_string = ResolveMainExport<CExoStringFromCharFn>("??0CExoString@@QEAA@PEBD@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");

  const LONG observation = InterlockedIncrement(&g_legacy_hak_mount_observations);
  LogFormat(
      L"legacy module hak mount #%ld begin: module='%s' count=%zu haks=[%s] resman_global=%p resman=%p add=%p construct=%p destroy=%p",
      observation,
      NarrowToWide(module_resref).c_str(),
      haks.size(),
      JoinAsciiList(haks).c_str(),
      resman_global,
      resman,
      add_file,
      construct_string,
      destroy_string);

  if (resman == nullptr || add_file == nullptr || construct_string == nullptr) {
    LogFormat(L"legacy module hak mount #%ld aborted: missing EE resource-manager API", observation);
    return;
  }

  size_t mounted = 0;
  for (auto it = haks.rbegin(); it != haks.rend(); ++it) {
    const std::string& hak = *it;
    if (hak.empty()) {
      continue;
    }

    int result = 0;
    bool raised_exception = false;

    const std::string hg_alias_path = result == 0
        ? ResolveHakAliasPath("HGHAK", GetHgHakDirectories(), hak, L"HG module hak preferred")
        : "";
    if (!hg_alias_path.empty()) {
      raised_exception = false;
      result = CallAddEncapsulatedResourceFile(
          resman,
          add_file,
          construct_string,
          destroy_string,
          hg_alias_path.c_str(),
          kEeHakAliasAddFlags,
          &raised_exception);
      LogFormat(
          L"legacy module hak mount #%ld HG alias: hak='%s' path='%s' flags=0x%08X result=%d%s",
          observation,
          NarrowToWide(hak).c_str(),
          NarrowToWide(hg_alias_path).c_str(),
          kEeHakAliasAddFlags,
          result,
          raised_exception ? L" exception" : L"");
    }

    const std::string diamond_alias_path =
        result == 0 ? ResolveHakAliasPath("DIAMONDHAK", GetDiamondHakDirectories(), hak, L"Diamond module hak fallback") : "";
    if (!diamond_alias_path.empty()) {
      raised_exception = false;
      result = CallAddEncapsulatedResourceFile(
          resman,
          add_file,
          construct_string,
          destroy_string,
          diamond_alias_path.c_str(),
          kEeHakAliasAddFlags,
          &raised_exception);
      LogFormat(
          L"legacy module hak mount #%ld Diamond alias: hak='%s' path='%s' flags=0x%08X result=%d%s",
          observation,
          NarrowToWide(hak).c_str(),
          NarrowToWide(diamond_alias_path).c_str(),
          kEeHakAliasAddFlags,
          result,
          raised_exception ? L" exception" : L"");
    }

    if (result == 0 && !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DIRECT_HAK_PREFERENCE")) {
      const std::vector<std::string> direct_paths = ResolveLegacyHakDirectPaths(hak);
      for (const std::string& direct_path : direct_paths) {
        raised_exception = false;
        result = CallAddEncapsulatedResourceFile(
            resman,
            add_file,
            construct_string,
            destroy_string,
            direct_path.c_str(),
            kEeHakAliasAddFlags,
            &raised_exception);
        LogFormat(
            L"legacy module hak mount #%ld direct fallback: hak='%s' path='%s' flags=0x%08X result=%d%s",
            observation,
            NarrowToWide(hak).c_str(),
            NarrowToWide(direct_path).c_str(),
            kEeHakAliasAddFlags,
            result,
            raised_exception ? L" exception" : L"");
        if (result != 0) {
          break;
        }
      }
    }

    const std::string override_path = result == 0 ? ResolveHgHakOverridePath(hak) : "";
    if (!override_path.empty()) {
      result = CallAddEncapsulatedResourceFile(
          resman,
          add_file,
          construct_string,
          destroy_string,
          override_path.c_str(),
          kEeHakAliasAddFlags,
          &raised_exception);
      LogFormat(
          L"legacy module hak mount #%ld HG override: hak='%s' path='%s' flags=0x%08X result=%d%s",
          observation,
          NarrowToWide(hak).c_str(),
          NarrowToWide(override_path).c_str(),
          kEeHakAliasAddFlags,
          result,
          raised_exception ? L" exception" : L"");

      if (result == 0 && override_path.size() > 4 &&
          LowerAscii(NarrowToWide(override_path.substr(override_path.size() - 4))) == L".hak") {
        const std::string override_path_no_extension = override_path.substr(0, override_path.size() - 4);
        raised_exception = false;
        result = CallAddEncapsulatedResourceFile(
            resman,
            add_file,
            construct_string,
            destroy_string,
            override_path_no_extension.c_str(),
            kEeHakAliasAddFlags,
            &raised_exception);
        LogFormat(
            L"legacy module hak mount #%ld HG override without extension: hak='%s' path='%s' flags=0x%08X result=%d%s",
            observation,
            NarrowToWide(hak).c_str(),
            NarrowToWide(override_path_no_extension).c_str(),
            kEeHakAliasAddFlags,
            result,
            raised_exception ? L" exception" : L"");
      }
    }

    if (result == 0) {
      const std::vector<std::string> direct_paths = ResolveLegacyHakDirectPaths(hak);
      for (const std::string& direct_path : direct_paths) {
        raised_exception = false;
        result = CallAddEncapsulatedResourceFile(
            resman,
            add_file,
            construct_string,
            destroy_string,
            direct_path.c_str(),
            kEeHakAliasAddFlags,
            &raised_exception);
        LogFormat(
            L"legacy module hak mount #%ld direct file: hak='%s' path='%s' flags=0x%08X result=%d%s",
            observation,
            NarrowToWide(hak).c_str(),
            NarrowToWide(direct_path).c_str(),
            kEeHakAliasAddFlags,
            result,
            raised_exception ? L" exception" : L"");
        if (result != 0) {
          break;
        }
      }
    }

    if (result == 0) {
      const std::string hak_alias_path = "HAK:" + hak;
      raised_exception = false;
      result = CallAddEncapsulatedResourceFile(
          resman,
          add_file,
          construct_string,
          destroy_string,
          hak_alias_path.c_str(),
          kEeHakAliasAddFlags,
          &raised_exception);
      LogFormat(
          L"legacy module hak mount #%ld HAK alias: hak='%s' path='%s' flags=0x%08X result=%d%s",
          observation,
          NarrowToWide(hak).c_str(),
          NarrowToWide(hak_alias_path).c_str(),
          kEeHakAliasAddFlags,
          result,
          raised_exception ? L" exception" : L"");
    }

    if (result == 0) {
      const std::string hak_install_path = "HAKINSTALL:" + hak;
      raised_exception = false;
      result = CallAddEncapsulatedResourceFile(
          resman,
          add_file,
          construct_string,
          destroy_string,
          hak_install_path.c_str(),
          kEeHakInstallAliasAddFlags,
          &raised_exception);
      LogFormat(
          L"legacy module hak mount #%ld HAKINSTALL alias: hak='%s' path='%s' flags=0x%08X result=%d%s",
          observation,
          NarrowToWide(hak).c_str(),
          NarrowToWide(hak_install_path).c_str(),
          kEeHakInstallAliasAddFlags,
          result,
          raised_exception ? L" exception" : L"");
    }

    if (result != 0) {
      ++mounted;
    }
  }

  LogFormat(
      L"legacy module hak mount #%ld complete: mounted=%zu/%zu module='%s'",
      observation,
      mounted,
      haks.size(),
      NarrowToWide(module_resref).c_str());
  MountLegacyOverrideDirectories(L"legacy module hak mount");
  ProbeLegacyMountedResources(resman, L"legacy module hak mount");
  DumpResManKeyTables(resman, L"legacy module hak mount complete");
  BeginRuntimeResourceDiagnostics(L"legacy module hak mount", 120);
  ReloadRulesAfterLegacyHakMount(mounted);
}

void RemountLegacyModuleResourcesAfterLoad(const wchar_t* reason) {
  std::vector<std::string> haks;
  std::string module_resref;
  const bool have_haks = GetRememberedLegacyModuleHakList(&haks, &module_resref);
  LogFormat(
      L"legacy module resource remount after load: reason=%s have_haks=%d count=%zu module='%s'",
      reason != nullptr ? reason : L"<unspecified>",
      have_haks ? 1 : 0,
      haks.size(),
      NarrowToWide(module_resref).c_str());

  if (have_haks) {
    MountLegacyModuleHakList(haks, module_resref);
  } else {
    MountLegacyOverrideDirectories(reason != nullptr ? reason : L"module load completion");
  }
}

bool TrySkipLegacyModuleHakList(
    void* message,
    const CnwMessageReadState& before,
    uint32_t* hak_count,
    uint32_t* skipped_bytes,
    uint32_t* resource_count_peek,
    std::wstring* prefix_bytes,
    std::vector<std::string>* haks,
    std::string* module_resref) {
  if (hak_count == nullptr || skipped_bytes == nullptr || resource_count_peek == nullptr || prefix_bytes == nullptr ||
      haks == nullptr || module_resref == nullptr) {
    return false;
  }
  *hak_count = 0;
  *skipped_bytes = 0;
  *resource_count_peek = 0;
  prefix_bytes->clear();
  haks->clear();
  module_resref->clear();

  if (!before.readable || before.read_buffer_ptr >= before.read_buffer_size) {
    return false;
  }

  const uint32_t max_prefix = std::min<uint32_t>(before.read_buffer_size - before.read_buffer_ptr, 96);
  std::vector<uint8_t> prefix;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, max_prefix, &prefix) || prefix.empty()) {
    return false;
  }
  *prefix_bytes = FormatBytes(prefix.data(), prefix.size());

  const uint32_t count = prefix[0];
  constexpr uint32_t kLegacyHakResRefBytes = 16;
  constexpr uint32_t kLegacyModuleResRefBytes = 16;
  constexpr uint32_t kMaxReasonableHakCount = 64;
  if (count > kMaxReasonableHakCount) {
    return false;
  }

  const uint32_t skip = 1 + count * kLegacyHakResRefBytes + kLegacyModuleResRefBytes;
  if (skip > before.read_buffer_size - before.read_buffer_ptr) {
    return false;
  }

  std::vector<uint8_t> legacy_hak_block;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, skip, &legacy_hak_block) ||
      legacy_hak_block.size() != skip) {
    return false;
  }

  std::vector<uint8_t> resource_count_bytes;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr + skip, 4, &resource_count_bytes) ||
      !TryReadU32LeFromBytes(resource_count_bytes, 0, resource_count_peek)) {
    return false;
  }

  if (!WriteUInt32AtOffset(message, 0x44, before.read_buffer_ptr + skip)) {
    return false;
  }

  haks->reserve(count);
  for (uint32_t index = 0; index < count; ++index) {
    haks->push_back(LegacyResRefFromBytes(legacy_hak_block, 1 + index * kLegacyHakResRefBytes));
  }
  *module_resref = LegacyResRefFromBytes(legacy_hak_block, 1 + count * kLegacyHakResRefBytes);

  *hak_count = count;
  *skipped_bytes = skip;
  return true;
}

void LogCnwMessageUnreadBytes(const wchar_t* context, void* message, const CnwMessageReadState& state) {
  if (context == nullptr || message == nullptr || !state.readable) {
    return;
  }

  void* read_buffer = nullptr;
  void* fragments_buffer = nullptr;
  ReadPointerAtOffset(message, 0x38, &read_buffer);
  ReadPointerAtOffset(message, 0x48, &fragments_buffer);

  LogFormat(
      L"%s: CNWMessage raw pointers message=%p read_buffer=%p fragments_buffer=%p state=[%s]",
      context,
      message,
      read_buffer,
      fragments_buffer,
      FormatCnwMessageReadState(state).c_str());

  if (read_buffer != nullptr) {
    if (state.read_buffer_ptr <= state.read_buffer_size) {
      const uint32_t remaining = state.read_buffer_size - state.read_buffer_ptr;
      const uint32_t logged = std::min<uint32_t>(remaining, 256);
      std::vector<uint8_t> bytes(logged);
      if (logged > 0 && TryCopyBytes(
            reinterpret_cast<const char*>(static_cast<uint8_t*>(read_buffer) + state.read_buffer_ptr),
            logged,
            reinterpret_cast<char*>(bytes.data()))) {
        LogFormat(
            L"%s: read-buffer unread offset=%u length=%u/%u bytes=[%s]",
            context,
            state.read_buffer_ptr,
            logged,
            remaining,
            FormatBytes(bytes.data(), bytes.size()).c_str());
      }
    } else {
      LogFormat(
          L"%s: read-buffer cursor past end offset=%u size=%u",
          context,
          state.read_buffer_ptr,
          state.read_buffer_size);
    }

    const uint32_t prefix_logged = std::min<uint32_t>(state.read_buffer_size, 256);
    std::vector<uint8_t> prefix(prefix_logged);
    if (prefix_logged > 0 &&
        TryCopyBytes(reinterpret_cast<const char*>(read_buffer), prefix_logged, reinterpret_cast<char*>(prefix.data()))) {
      LogFormat(
          L"%s: read-buffer prefix length=%u/%u bytes=[%s]",
          context,
          prefix_logged,
          state.read_buffer_size,
          FormatBytes(prefix.data(), prefix.size()).c_str());
    }
  }

  if (fragments_buffer != nullptr) {
    if (state.read_fragments_ptr <= state.read_fragments_size) {
      const uint32_t remaining = state.read_fragments_size - state.read_fragments_ptr;
      const uint32_t logged = std::min<uint32_t>(remaining, 256);
      std::vector<uint8_t> bytes(logged);
      if (logged > 0 && TryCopyBytes(
            reinterpret_cast<const char*>(static_cast<uint8_t*>(fragments_buffer) + state.read_fragments_ptr),
            logged,
            reinterpret_cast<char*>(bytes.data()))) {
        LogFormat(
            L"%s: fragments unread offset=%u length=%u/%u bit=%u/%u bytes=[%s]",
            context,
            state.read_fragments_ptr,
            logged,
            remaining,
            static_cast<unsigned int>(state.read_fragments_bit_ptr),
            static_cast<unsigned int>(state.read_fragments_bit_size),
            FormatBytes(bytes.data(), bytes.size()).c_str());
      }
    } else {
      LogFormat(
          L"%s: fragments cursor past end offset=%u size=%u bit=%u/%u",
          context,
          state.read_fragments_ptr,
          state.read_fragments_size,
          static_cast<unsigned int>(state.read_fragments_bit_ptr),
          static_cast<unsigned int>(state.read_fragments_bit_size));
    }

    const uint32_t prefix_logged = std::min<uint32_t>(state.read_fragments_size, 256);
    std::vector<uint8_t> prefix(prefix_logged);
    if (prefix_logged > 0 &&
        TryCopyBytes(reinterpret_cast<const char*>(fragments_buffer), prefix_logged, reinterpret_cast<char*>(prefix.data()))) {
      LogFormat(
          L"%s: fragments prefix length=%u/%u bytes=[%s]",
          context,
          prefix_logged,
          state.read_fragments_size,
          FormatBytes(prefix.data(), prefix.size()).c_str());
    }
  }
}

uint32_t ReadUInt32Field(void* object, size_t offset, uint32_t fallback = 0) {
  uint32_t value = fallback;
  if (object == nullptr) {
    return value;
  }
  SafeReadUInt32(reinterpret_cast<const uint32_t*>(static_cast<uint8_t*>(object) + offset), &value);
  return value;
}

uint8_t ReadUInt8Field(void* object, size_t offset, uint8_t fallback = 0) {
  uint8_t value = fallback;
  if (object == nullptr) {
    return value;
  }
  SafeReadUInt8(reinterpret_cast<const uint8_t*>(static_cast<uint8_t*>(object) + offset), &value);
  return value;
}

float ReadFloatField(void* object, size_t offset, float fallback = 0.0f) {
  float value = fallback;
  if (object == nullptr) {
    return value;
  }
  SafeReadFloat(reinterpret_cast<const float*>(static_cast<uint8_t*>(object) + offset), &value);
  return value;
}

bool ReadCExoStringViewAt(const void* address, CExoStringView* snapshot) {
  if (address == nullptr || snapshot == nullptr) {
    return false;
  }
  return TryCopyBytes(reinterpret_cast<const char*>(address), sizeof(CExoStringView), reinterpret_cast<char*>(snapshot));
}

bool ReadCExoStringField(void* object, size_t offset, CExoStringView* snapshot) {
  if (object == nullptr || snapshot == nullptr) {
    return false;
  }
  return ReadCExoStringViewAt(static_cast<uint8_t*>(object) + offset, snapshot);
}

void DumpResManKeyTables(void* resman, const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_RESMAN_TABLE_DUMP")) {
    return;
  }
  if (resman == nullptr) {
    LogFormat(
        L"CExoResMan table dump skipped: reason=%s resman=<null>",
        reason != nullptr ? reason : L"<unspecified>");
    return;
  }

  void* const tables = ReadPointerField(resman, 0x18);
  const uint32_t count = ReadUInt32Field(resman, 0x20, 0);
  const uint32_t capacity = ReadUInt32Field(resman, 0x24, 0);
  if (tables == nullptr || count > 1024 || capacity > 4096 || count > capacity) {
    LogFormat(
        L"CExoResMan table dump invalid: reason=%s resman=%p tables=%p count=%u capacity=%u",
        reason != nullptr ? reason : L"<unspecified>",
        resman,
        tables,
        count,
        capacity);
    return;
  }

  LogFormat(
      L"CExoResMan table dump begin: reason=%s resman=%p tables=%p count=%u capacity=%u",
      reason != nullptr ? reason : L"<unspecified>",
      resman,
      tables,
      count,
      capacity);

  const uint32_t logged_limit = std::min<uint32_t>(count, 160);
  for (uint32_t index = 0; index < logged_limit; ++index) {
    void* const table = ReadPointerField(tables, static_cast<size_t>(index) * sizeof(void*));
    if (table == nullptr) {
      LogFormat(
          L"CExoResMan table[%u]: table=<null>",
          index);
      continue;
    }

    CExoStringView requested_path{};
    CExoStringView resolved_path{};
    const bool requested_ok = ReadCExoStringField(table, 0x28, &requested_path);
    const bool resolved_ok = ReadCExoStringField(table, 0x70, &resolved_path);
    const uint32_t entry_count = ReadUInt32Field(table, 0x08, 0xFFFFFFFFu);
    const uint32_t table_capacity = ReadUInt32Field(table, 0x18, 0xFFFFFFFFu);
    const uint32_t kind = ReadUInt32Field(table, 0x20, 0xFFFFFFFFu);
    const uint32_t priority = ReadUInt32Field(table, 0x48, 0xFFFFFFFFu);
    const uint32_t raw_1c = ReadUInt32Field(table, 0x1C, 0xFFFFFFFFu);
    const uint32_t raw_4c = ReadUInt32Field(table, 0x4C, 0xFFFFFFFFu);
    void* const entries = ReadPointerField(table, 0x00);
    void* const filter = ReadPointerField(table, 0x60);
    const std::wstring requested_text = requested_ok ? CExoStringToWide(&requested_path) : L"<unreadable>";
    const std::wstring resolved_text = resolved_ok ? CExoStringToWide(&resolved_path) : L"<unreadable>";
    const bool legacy_path = LooksLikeLegacyResourcePath(requested_text) || LooksLikeLegacyResourcePath(resolved_text);
    const bool log_row = count <= 80 || index < 24 || index + 16 >= count || legacy_path;
    if (!log_row) {
      continue;
    }

    LogFormat(
        L"CExoResMan table[%u]: table=%p entries=%p entry_count=%u table_capacity=%u kind=%s raw_kind=%u priority=%u raw_1c=0x%08X raw_4c=0x%08X filter=%p requested='%s' resolved='%s' legacy_path=%d",
        index,
        table,
        entries,
        entry_count,
        table_capacity,
        FormatResManKeyTableKind(kind),
        kind,
        priority,
        raw_1c,
        raw_4c,
        filter,
        requested_text.c_str(),
        resolved_text.c_str(),
        legacy_path ? 1 : 0);
  }

  if (logged_limit < count) {
    LogFormat(
        L"CExoResMan table dump truncated: reason=%s logged=%u count=%u",
        reason != nullptr ? reason : L"<unspecified>",
        logged_limit,
        count);
  }
}

std::string ReadFixedAsciiField(void* object, size_t offset, size_t length) {
  if (object == nullptr || length == 0 || length > 64) {
    return {};
  }

  char buffer[64]{};
  if (!TryCopyBytes(reinterpret_cast<const char*>(static_cast<uint8_t*>(object) + offset), static_cast<uint32_t>(length), buffer)) {
    return {};
  }

  size_t end = length;
  while (end > 0 && (buffer[end - 1] == '\0' || buffer[end - 1] == ' ')) {
    --end;
  }

  std::string text;
  text.reserve(end);
  for (size_t index = 0; index < end; ++index) {
    const unsigned char ch = static_cast<unsigned char>(buffer[index]);
    text.push_back(ch >= 0x20 && ch <= 0x7E ? static_cast<char>(ch) : '.');
  }
  return text;
}

std::string CResRefToAscii(const CResRefView* value) {
  if (value == nullptr) {
    return "<null>";
  }

  CResRefView snapshot{};
  if (!TryCopyBytes(reinterpret_cast<const char*>(value), sizeof(snapshot), reinterpret_cast<char*>(&snapshot))) {
    return "<unreadable>";
  }

  size_t end = kCResRefTextBytes;
  while (end > 0 && (snapshot.data[end - 1] == '\0' || snapshot.data[end - 1] == ' ')) {
    --end;
  }

  std::string text;
  text.reserve(end);
  for (size_t index = 0; index < end; ++index) {
    const unsigned char ch = static_cast<unsigned char>(snapshot.data[index]);
    text.push_back(ch >= 0x20 && ch <= 0x7E ? static_cast<char>(ch) : '.');
  }
  return text;
}

std::wstring CResRefToWide(const CResRefView* value) {
  return NarrowToWide(CResRefToAscii(value));
}

std::wstring CResRefRawBytes(const CResRefView* value) {
  if (value == nullptr) {
    return L"";
  }

  CResRefView snapshot{};
  if (!TryCopyBytes(reinterpret_cast<const char*>(value), sizeof(snapshot), reinterpret_cast<char*>(&snapshot))) {
    return L"<unreadable>";
  }
  return FormatBytes(reinterpret_cast<const uint8_t*>(snapshot.data), kCResRefStorageBytes);
}

std::wstring FormatResType(uint16_t type) {
  const wchar_t* name = L"";
  switch (type) {
    case 1: name = L"BMP"; break;
    case 3: name = L"TGA"; break;
    case 4: name = L"WAV"; break;
    case 6: name = L"PLT"; break;
    case 7: name = L"INI"; break;
    case 8: name = L"BMU"; break;
    case 9: name = L"MPG"; break;
    case 10: name = L"TXT"; break;
    case 2002: name = L"MDL"; break;
    case 2005: name = L"FNT"; break;
    case 2009: name = L"NSS"; break;
    case 2010: name = L"NCS"; break;
    case 2011: name = L"MOD"; break;
    case 2012: name = L"ARE"; break;
    case 2013: name = L"SET"; break;
    case 2014: name = L"IFO"; break;
    case 2015: name = L"BIC"; break;
    case 2016: name = L"WOK"; break;
    case 2017: name = L"2DA"; break;
    case 2018: name = L"TLK"; break;
    case 2022: name = L"TXI"; break;
    case 2023: name = L"GIT"; break;
    case 2025: name = L"UTI"; break;
    case 2026: name = L"RES2026"; break;
    case 2027: name = L"UTC"; break;
    case 2029: name = L"DLG"; break;
    case 2030: name = L"ITP"; break;
    case 2032: name = L"UTT"; break;
    case 2033: name = L"DDS"; break;
    case 2035: name = L"UTS"; break;
    case 2036: name = L"LTR"; break;
    case 2037: name = L"GFF"; break;
    case 2038: name = L"FAC"; break;
    case 2039: name = L"BTE"; break;
    case 2040: name = L"UTE"; break;
    case 2041: name = L"BTI"; break;
    case 2042: name = L"UTI"; break;
    case 2043: name = L"BTC"; break;
    case 2044: name = L"UTP"; break;
    case 2045: name = L"DFT"; break;
    case 2046: name = L"GIC"; break;
    case 2047: name = L"GUI"; break;
    case 2051: name = L"UTM"; break;
    case 2052: name = L"DWK"; break;
    case 2053: name = L"PWK"; break;
    case 2056: name = L"JRL"; break;
    case 2058: name = L"UTW"; break;
    case 2059: name = L"4PC"; break;
    case 2060: name = L"SSF"; break;
    case 2064: name = L"NDB"; break;
    case 2065: name = L"PTM"; break;
    case 2066: name = L"PTT"; break;
    case 9996: name = L"EE_TILESET"; break;
    default: break;
  }

  wchar_t buffer[64]{};
  if (name[0] != L'\0') {
    swprintf_s(buffer, L"%hu/%s", type, name);
  } else {
    swprintf_s(buffer, L"%hu", type);
  }
  return buffer;
}

bool ModuleResourceDiagnosticScopeActive() {
  return g_client_module_load_depth > 0 || g_client_module_resource_load_depth > 0;
}

bool RuntimeResourceDiagnosticScopeActive() {
  const ULONGLONG until_tick = g_resman_runtime_diagnostics_until_tick;
  return until_tick != 0 && GetTickCount64() <= until_tick;
}

bool IsSelectedRuntime2daMiss(const CResRefView* resref) {
  const std::string name = LowerAscii(CResRefToAscii(resref));
  return name == "spells" ||
      name == "baseitems" ||
      name == "itempropdef" ||
      name == "itemprops" ||
      name == "iprp_spells" ||
      name == "iprp_onhitspell" ||
      name == "iprp_feats" ||
      name == "feat" ||
      name == "classes" ||
      name == "cls_feat_eyeofg" ||
      name == "cls_skill_eyeofg" ||
      name == "cls_bfeat_eyeofg" ||
      name == "cls_feat_shoudi" ||
      name == "cls_skill_shoudi" ||
      name == "cls_bfeat_shoudi";
}

bool IsRuntimeImportantResourceType(uint16_t type) {
  switch (type) {
    case 3:     // TGA
    case 6:     // PLT
    case 2002:  // MDL
    case 2013:  // SET
    case 2016:  // WOK
    case 2017:  // 2DA
    case 2022:  // TXI
    case 2026:  // Legacy/unknown EE resource class seen during texture resolution.
    case 2033:  // DDS
    case 2059:  // 4PC texture cache lookup before DDS/TGA fallback.
    case 9996:  // EE tileset-side companion lookup.
      return true;
    default:
      return false;
  }
}

bool IsRuntimeInterestingResourceLookup(const CResRefView* resref, uint16_t type) {
  if (type == 2017) {
    return IsSelectedRuntime2daMiss(resref);
  }
  return IsRuntimeImportantResourceType(type);
}

bool IsOptionalRuntimeDemandMissType(uint16_t type) {
  switch (type) {
    case 2022:  // TXI material sidecars are often absent by design.
    case 2059:  // EE texture-cache companion lookup before DDS/TGA fallback.
    case 9996:  // EE tileset-side companion lookup.
      return true;
    default:
      return false;
  }
}

bool StartsWithAscii(const std::string& value, const char* prefix) {
  if (prefix == nullptr) {
    return false;
  }
  const size_t prefix_length = strlen(prefix);
  return value.size() >= prefix_length && value.compare(0, prefix_length, prefix) == 0;
}

bool IsTargetedHgRuntimeResourceName(const std::string& lower_name) {
  return StartsWithAscii(lower_name, "is_") ||
      StartsWithAscii(lower_name, "iit_") ||
      StartsWithAscii(lower_name, "gi_") ||
      StartsWithAscii(lower_name, "it_") ||
      StartsWithAscii(lower_name, "pfh") ||
      StartsWithAscii(lower_name, "pmh") ||
      StartsWithAscii(lower_name, "pfa") ||
      StartsWithAscii(lower_name, "pma") ||
      StartsWithAscii(lower_name, "cloak") ||
      StartsWithAscii(lower_name, "ctl_") ||
      StartsWithAscii(lower_name, "gui_") ||
      StartsWithAscii(lower_name, "ttr") ||
      StartsWithAscii(lower_name, "tdt") ||
      StartsWithAscii(lower_name, "tde") ||
      StartsWithAscii(lower_name, "tcn") ||
      StartsWithAscii(lower_name, "tnp") ||
      StartsWithAscii(lower_name, "plc") ||
      StartsWithAscii(lower_name, "plx") ||
      StartsWithAscii(lower_name, "zlc") ||
      StartsWithAscii(lower_name, "wmg") ||
      lower_name == "spells" ||
      lower_name == "baseitems" ||
      lower_name == "itempropdef" ||
      lower_name == "itemprops" ||
      lower_name == "iprp_spells" ||
      lower_name == "iprp_onhitspell" ||
      lower_name == "iprp_feats" ||
      lower_name == "doa_emerald" ||
      lower_name == "nulltex";
}

void RememberTrackedResObject(void* object, const CResRefView* resref, uint16_t type) {
  if (object == nullptr || resref == nullptr) {
    return;
  }

  CResRefView snapshot{};
  if (!TryCopyBytes(reinterpret_cast<const char*>(resref), sizeof(snapshot), reinterpret_cast<char*>(&snapshot))) {
    return;
  }

  const LONG next = InterlockedIncrement(&g_tracked_res_object_next);
  const size_t slot = static_cast<size_t>(next) % (sizeof(g_tracked_res_objects) / sizeof(g_tracked_res_objects[0]));

  AcquireSRWLockExclusive(&g_tracked_res_objects_lock);
  g_tracked_res_objects[slot].object = object;
  g_tracked_res_objects[slot].resref = snapshot;
  g_tracked_res_objects[slot].type = type;
  g_tracked_res_objects[slot].tick = GetTickCount();
  ReleaseSRWLockExclusive(&g_tracked_res_objects_lock);
}

bool LookupTrackedResObject(void* object, CResRefView* resref, uint16_t* type) {
  if (object == nullptr || resref == nullptr || type == nullptr) {
    return false;
  }

  bool found = false;
  DWORD best_age = MAXDWORD;
  CResRefView best_resref{};
  uint16_t best_type = 0;
  const DWORD now = GetTickCount();

  AcquireSRWLockShared(&g_tracked_res_objects_lock);
  for (const TrackedResObject& entry : g_tracked_res_objects) {
    if (entry.object != object) {
      continue;
    }
    const DWORD age = now - entry.tick;
    if (!found || age < best_age) {
      found = true;
      best_age = age;
      best_resref = entry.resref;
      best_type = entry.type;
    }
  }
  ReleaseSRWLockShared(&g_tracked_res_objects_lock);

  if (!found) {
    return false;
  }

  *resref = best_resref;
  *type = best_type;
  return true;
}

bool EnsureResourceManagerDiagnosticsInstalled(const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_RESMAN_DIAGNOSTICS")) {
    LogFormat(
        L"CExoResMan diagnostic hooks not installed for %s: HG_BRIDGE_DISABLE_RESMAN_DIAGNOSTICS is set",
        reason != nullptr ? reason : L"<unspecified>");
    return false;
  }

  if (InterlockedCompareExchange(&g_resman_diagnostics_install_state, 1, 0) != 0) {
    return true;
  }

  const bool installed = InstallResourceManagerDiagnostics();
  if (!installed) {
    InterlockedExchange(&g_resman_diagnostics_install_state, 0);
  }
  LogFormat(
      L"CExoResMan diagnostic hooks %s for %s",
      installed ? L"installed" : L"failed to install",
      reason != nullptr ? reason : L"<unspecified>");
  return installed;
}

bool EnsureWorldResourceDiagnosticsInstalled(const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_WORLD_RESOURCE_DIAGNOSTICS")) {
    LogFormat(
        L"world resource diagnostic hooks not installed for %s: HG_BRIDGE_DISABLE_WORLD_RESOURCE_DIAGNOSTICS is set",
        reason != nullptr ? reason : L"<unspecified>");
    return false;
  }

  if (InterlockedCompareExchange(&g_world_resource_diagnostics_install_state, 1, 0) != 0) {
    return true;
  }

  const bool installed = InstallWorldResourceDiagnostics();
  if (!installed) {
    InterlockedExchange(&g_world_resource_diagnostics_install_state, 0);
  }
  LogFormat(
      L"world resource diagnostic hooks %s for %s",
      installed ? L"installed" : L"failed to install",
      reason != nullptr ? reason : L"<unspecified>");
  return installed;
}

void BeginRuntimeResourceDiagnostics(const wchar_t* reason, DWORD seconds) {
  if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_RUNTIME_RESOURCE_DIAGNOSTICS")) {
    return;
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_RUNTIME_RESOURCE_DIAGNOSTICS")) {
    return;
  }
  if (!EnsureResourceManagerDiagnosticsInstalled(reason)) {
    return;
  }
  EnsureWorldResourceDiagnosticsInstalled(reason);

  InterlockedExchange(&g_resman_runtime_miss_observations, 0);
  InterlockedExchange(&g_resman_runtime_hit_observations, 0);
  InterlockedExchange(&g_resman_runtime_demand_observations, 0);
  InterlockedExchange(&g_resman_runtime_non_optional_demand_miss_observations, 0);
  InterlockedExchange(&g_resman_runtime_targeted_demand_observations, 0);
  g_resman_runtime_diagnostics_until_tick = GetTickCount64() + static_cast<ULONGLONG>(seconds) * 1000ULL;
  LogFormat(
      L"CExoResMan runtime resource diagnostics armed: reason=%s seconds=%lu",
      reason != nullptr ? reason : L"<unspecified>",
      seconds);
}

void LogCExoStringField(const wchar_t* context, const wchar_t* name, void* object, size_t offset) {
  CExoStringView value{};
  if (!ReadCExoStringField(object, offset, &value)) {
    LogFormat(L"%s: %s=<unreadable>", context, name);
    return;
  }

  LogFormat(
      L"%s: %s='%s' length=%u capacity=%u",
      context,
      name,
      CExoStringToWide(&value).c_str(),
      value.length,
      value.capacity);
}

void LogClientModuleSnapshot(const wchar_t* context, void* module, uint32_t result) {
  if (module == nullptr) {
    LogFormat(L"%s: module=<null> result=0x%08X", context, result);
    return;
  }

  const uint8_t byte_15c = ReadUInt8Field(module, 0x15C);
  const uint8_t byte_2e4 = ReadUInt8Field(module, 0x2E4);
  const uint32_t resource_count = ReadUInt32Field(module, 0x2C8);
  const uint32_t resource_name_count = ReadUInt32Field(module, 0x2D8);
  const uint32_t resources_loaded = ReadUInt32Field(module, 0x2E8);
  const std::string custom_tlk = ReadFixedAsciiField(module, 0xC0, 16);
  uint8_t custom_tlk_raw[16]{};
  const bool custom_tlk_raw_ok = TryCopyBytes(
      reinterpret_cast<const char*>(static_cast<uint8_t*>(module) + 0xC0),
      sizeof(custom_tlk_raw),
      reinterpret_cast<char*>(custom_tlk_raw));

  LogFormat(
      L"%s: module=%p result=0x%08X byte15C=%u byte2E4=%u resources=%u/%u resources_loaded=%u custom_tlk='%s' custom_tlk_raw=%s[%s]",
      context,
      module,
      result,
      static_cast<unsigned int>(byte_15c),
      static_cast<unsigned int>(byte_2e4),
      resource_count,
      resource_name_count,
      resources_loaded,
      NarrowToWide(custom_tlk).c_str(),
      custom_tlk_raw_ok ? L"" : L"<unreadable>",
      custom_tlk_raw_ok ? FormatBytes(custom_tlk_raw, sizeof(custom_tlk_raw)).c_str() : L"");
  LogCExoStringField(context, L"server/module field +0x18", module, 0x18);
  LogCExoStringField(context, L"field +0x38", module, 0x38);
  LogCExoStringField(context, L"field +0x48", module, 0x48);
  LogCExoStringField(context, L"module description field +0xD8", module, 0xD8);

  uint8_t module_header[0x120]{};
  if (TryCopyBytes(reinterpret_cast<const char*>(module), sizeof(module_header), reinterpret_cast<char*>(module_header))) {
    LogFormat(
        L"%s: module raw prefix length=%zu bytes=[%s]",
        context,
        sizeof(module_header),
        FormatBytes(module_header, sizeof(module_header)).c_str());
  }

  void* const resource_ids = ReadPointerField(module, 0x2C0);
  void* const resource_names = ReadPointerField(module, 0x2D0);
  if (resource_count == 0 || resource_ids == nullptr || resource_names == nullptr) {
    LogFormat(
        L"%s: resource arrays ids=%p names=%p count=%u",
        context,
        resource_ids,
        resource_names,
        resource_count);
    return;
  }

  constexpr uint32_t kMaxLoggedResources = 32;
  const uint32_t logged_count = std::min(resource_count, kMaxLoggedResources);
  for (uint32_t index = 0; index < logged_count; ++index) {
    uint32_t resource_id = 0;
    const bool id_ok = SafeReadUInt32(reinterpret_cast<const uint32_t*>(static_cast<uint8_t*>(resource_ids) + index * sizeof(uint32_t)), &resource_id);

    CExoStringView name{};
    const bool name_ok = ReadCExoStringViewAt(static_cast<uint8_t*>(resource_names) + index * sizeof(CExoStringView), &name);
    LogFormat(
        L"%s: resource[%u] id=%s0x%08X name='%s' length=%u",
        context,
        index,
        id_ok ? L"" : L"<unreadable> ",
        id_ok ? resource_id : 0,
        name_ok ? CExoStringToWide(&name).c_str() : L"<unreadable>",
        name_ok ? name.length : 0);
  }

  if (resource_count > logged_count) {
    LogFormat(L"%s: resource list truncated after %u of %u entries", context, logged_count, resource_count);
  }
}

TlkTableOpenFileFn ResolveTlkTableOpenFile() {
  if (g_tlk_table_open_file == nullptr) {
    g_tlk_table_open_file =
        ResolveMainExport<TlkTableOpenFileFn>("?OpenFile@CTlkTable@@QEAAHVCExoString@@@Z");
  }
  return g_tlk_table_open_file;
}

TlkTableOpenFileAlternateFn ResolveTlkTableOpenFileAlternate() {
  if (g_tlk_table_open_file_alternate == nullptr) {
    g_tlk_table_open_file_alternate = ResolveMainExport<TlkTableOpenFileAlternateFn>(
        "?OpenFileAlternate@CTlkTable@@QEAAHAEBVCExoString@@@Z");
  }
  return g_tlk_table_open_file_alternate;
}

std::wstring StripTlkExtension(std::wstring path) {
  if (path.size() >= 4 && LowerAscii(path.substr(path.size() - 4)) == L".tlk") {
    path.resize(path.size() - 4);
  }
  return path;
}

bool IsReasonableTlkResref(const std::string& value) {
  if (value.empty() || value.size() > kCResRefTextBytes) {
    return false;
  }
  for (char ch : value) {
    const unsigned char byte = static_cast<unsigned char>(ch);
    const bool ok = (byte >= 'a' && byte <= 'z') ||
        (byte >= 'A' && byte <= 'Z') ||
        (byte >= '0' && byte <= '9') ||
        byte == '_';
    if (!ok) {
      return false;
    }
  }
  return true;
}

std::string NormalizeTlkResref(std::string value) {
  while (!value.empty() && (value.back() == '\0' || value.back() == ' ')) {
    value.pop_back();
  }
  const std::string lowered = LowerAscii(value);
  if (lowered.size() > 4 && lowered.substr(lowered.size() - 4) == ".tlk") {
    value.resize(value.size() - 4);
  }
  return IsReasonableTlkResref(value) ? value : std::string();
}

std::vector<std::wstring> GetLegacyTlkDirectories() {
  std::vector<std::wstring> directories;

  const std::wstring bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  if (!bundle.empty()) {
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-gui"), L"tlk"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"hg-std"), L"tlk"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"diamond"), L"tlk"));
    AddUniquePath(&directories, JoinPath(JoinPath(bundle, L"cep23"), L"tlk"));
    return directories;
  }

  for (const std::wstring& root : GetHgAssetRoots()) {
    AddUniquePath(&directories, JoinPath(root, L"tlk"));
    AddUniquePath(&directories, root);
  }

  for (const std::wstring& root : GetDiamondRootCandidates()) {
    AddUniquePath(&directories, JoinPath(root, L"tlk"));
    AddUniquePath(&directories, root);
  }

  const std::wstring user_profile = GetEnvironmentString(L"USERPROFILE");
  if (!user_profile.empty()) {
    AddUniquePath(
        &directories,
        JoinPath(JoinPath(user_profile, L"Documents\\Neverwinter Nights"), L"tlk"));
  }

  return directories;
}

std::wstring ResolveLegacyTlkFile(const std::string& resref) {
  const std::string normalized = NormalizeTlkResref(resref);
  if (normalized.empty()) {
    return L"";
  }

  const std::wstring file_name = NarrowToWide(normalized) + L".tlk";
  for (const std::wstring& directory : GetLegacyTlkDirectories()) {
    if (!DirectoryExists(directory)) {
      continue;
    }
    const std::wstring candidate = JoinPath(directory, file_name);
    if (FileExistsWide(candidate)) {
      return candidate;
    }
  }
  return L"";
}

void AddLegacyTlkAliases(const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_TLK_ALIASES")) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_legacy_tlk_alias_observations);
  size_t added = 0;
  for (const std::wstring& directory : GetLegacyTlkDirectories()) {
    if (!DirectoryExists(directory)) {
      continue;
    }
    if (AddRuntimeAlias("HGTLK", directory, reason)) {
      ++added;
    }
    AddRuntimeAlias("DIAMONDTLK", directory, reason);
    AddRuntimeAlias("TLKINSTALL", directory, reason);
    AddRuntimeAlias("TLKLOCINSTALL", directory, reason);
  }
  if (observation <= 8) {
    LogFormat(
        L"legacy TLK aliases #%ld: reason=%s directories-added=%zu",
        observation,
        reason != nullptr ? reason : L"<unspecified>",
        added);
  }
}

void LogTlkTableSnapshot(const wchar_t* context, void* tlk_table) {
  if (tlk_table == nullptr) {
    LogFormat(L"%s: tlk_table=<null>", context != nullptr ? context : L"TLK snapshot");
    return;
  }

  CExoStringView alternate_name{};
  const bool alternate_name_ok = ReadCExoStringField(tlk_table, 0x60, &alternate_name);
  LogFormat(
      L"%s: tlk_table=%p main=[%p,%p] alternate=[%p,%p] mode=%u alternate_name=%s'%s'",
      context != nullptr ? context : L"TLK snapshot",
      tlk_table,
      ReadPointerField(tlk_table, 0x08),
      ReadPointerField(tlk_table, 0x10),
      ReadPointerField(tlk_table, 0x18),
      ReadPointerField(tlk_table, 0x20),
      static_cast<unsigned int>(ReadUInt8Field(tlk_table, 0x28)),
      alternate_name_ok ? L"" : L"<unreadable>",
      alternate_name_ok ? CExoStringToWide(&alternate_name).c_str() : L"");
}

std::wstring FormatTlkTableFetchState(void* tlk_table) {
  if (tlk_table == nullptr) {
    return L"tlk_table=<null>";
  }

  CExoStringView alternate_name{};
  const bool alternate_name_ok = ReadCExoStringField(tlk_table, 0x60, &alternate_name);
  wchar_t buffer[512]{};
  swprintf_s(
      buffer,
      L"tlk_table=%p main0=%p main1=%p alt0=%p alt1=%p mode=%u alternate_name=%s'%s'",
      tlk_table,
      ReadPointerField(tlk_table, 0x08),
      ReadPointerField(tlk_table, 0x10),
      ReadPointerField(tlk_table, 0x18),
      ReadPointerField(tlk_table, 0x20),
      static_cast<unsigned int>(ReadUInt8Field(tlk_table, 0x28)),
      alternate_name_ok ? L"" : L"<unreadable>",
      alternate_name_ok ? CExoStringToWide(&alternate_name).c_str() : L"");
  return buffer;
}

std::wstring TlkStrResTextPreview(void* str_res) {
  CExoStringView text{};
  if (str_res == nullptr || !ReadCExoStringViewAt(str_res, &text)) {
    return L"<unreadable>";
  }
  return CExoStringToWide(&text);
}

uint32_t TlkStrResTextLength(void* str_res) {
  CExoStringView text{};
  if (str_res == nullptr || !ReadCExoStringViewAt(str_res, &text)) {
    return 0;
  }
  return text.length;
}

bool LooksLikeBadStrRefText(const std::wstring& text) {
  std::wstring normalized;
  normalized.reserve(std::min<size_t>(text.size(), 96));
  for (wchar_t ch : text) {
    if (normalized.size() >= 96) {
      break;
    }
    if (ch == L' ' || ch == L'\t' || ch == L'_' || ch == L'-') {
      continue;
    }
    if (ch >= L'A' && ch <= L'Z') {
      ch = static_cast<wchar_t>(ch - L'A' + L'a');
    }
    normalized.push_back(ch);
  }
  return normalized.find(L"badstrref") != std::wstring::npos;
}

std::wstring TlkStrResResRefPreview(void* str_res) {
  if (str_res == nullptr) {
    return L"<null>";
  }

  CResRefView resref{};
  if (!TryCopyBytes(
          reinterpret_cast<const char*>(static_cast<uint8_t*>(str_res) + 0x10),
          static_cast<uint32_t>(sizeof(resref)),
          reinterpret_cast<char*>(&resref))) {
    return L"<unreadable>";
  }
  return CResRefToWide(&resref);
}

int __fastcall HookedTlkTableFetchInternal(
    void* tlk_table,
    uint32_t strref,
    void* str_res,
    int resolve_tokens) {
  void* const return_address = _ReturnAddress();
  const int result = g_tlk_table_fetch_internal_original != nullptr
      ? g_tlk_table_fetch_internal_original(tlk_table, strref, str_res, resolve_tokens)
      : 0;

  const uint32_t masked_strref = strref & 0x00FFFFFFu;
  const bool custom_tlk_ref = (strref & 0x01000000u) != 0;
  const std::wstring text = TlkStrResTextPreview(str_res);
  const bool bad_strref = LooksLikeBadStrRefText(text);
  const bool failed = result == 0;
  const bool verbose = IsTruthyEnvironmentFlag(L"HG_BRIDGE_VERBOSE_TLK_FETCH");
  const bool verbose_all = IsTruthyEnvironmentFlag(L"HG_BRIDGE_VERBOSE_ALL_TLK_FETCH");

  LONG observation = 0;
  bool should_log = false;
  const wchar_t* reason = L"";
  if (bad_strref) {
    observation = InterlockedIncrement(&g_tlk_badstrref_observations);
    should_log = observation <= 500 || verbose;
    reason = L"badstrref";
  } else if (failed) {
    observation = InterlockedIncrement(&g_tlk_fetch_failure_observations);
    should_log = observation <= 200 || verbose;
    reason = L"failed";
  } else if (verbose && custom_tlk_ref) {
    observation = InterlockedIncrement(&g_tlk_fetch_observations);
    should_log = observation <= 500;
    reason = L"custom-ok";
  } else if (verbose_all) {
    observation = InterlockedIncrement(&g_tlk_fetch_observations);
    should_log = observation <= 1000;
    reason = L"ok";
  }

  if (should_log) {
    LogFormat(
        L"TLK fetch %s #%ld: caller=%s strref=%u/0x%08X custom=%d masked=%u resolve=%d result=%d text-len=%u text='%s' sound='%s' state=[%s]",
        reason,
        observation,
        FormatMainExecutableAddress(return_address).c_str(),
        strref,
        strref,
        custom_tlk_ref ? 1 : 0,
        masked_strref,
        resolve_tokens,
        result,
        TlkStrResTextLength(str_res),
        text.c_str(),
        TlkStrResResRefPreview(str_res).c_str(),
        FormatTlkTableFetchState(tlk_table).c_str());
  }

  return result;
}

int CallOpenLegacyTlkFileNoExcept(
    void* tlk_table,
    TlkTableOpenFileFn open_main,
    TlkTableOpenFileAlternateFn open_alternate,
    CExoStringFromCharFn construct_string,
    CExoStringDestructorFn destroy_string,
    const char* attempt,
    bool alternate,
    bool* raised_exception) {
  if (raised_exception != nullptr) {
    *raised_exception = false;
  }
  if (tlk_table == nullptr || construct_string == nullptr || attempt == nullptr ||
      (!alternate && open_main == nullptr) ||
      (alternate && open_alternate == nullptr)) {
    return 0;
  }

  CExoStringView tlk_name{};
  int result = 0;
  __try {
    construct_string(&tlk_name, attempt);
    result = alternate
        ? open_alternate(tlk_table, &tlk_name)
        : open_main(tlk_table, &tlk_name);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    if (raised_exception != nullptr) {
      *raised_exception = true;
    }
  }
  TryDestroyCExoString(destroy_string, &tlk_name);
  return result;
}

bool OpenLegacyTlkFile(
    const std::string& resref,
    const std::wstring& resolved_path,
    bool alternate,
    const wchar_t* reason) {
  void** const tlk_table_global = ResolveMainExport<void**>("?g_pTlkTable@@3PEAVCTlkTable@@EA");
  void* tlk_table = nullptr;
  SafeReadGlobalPointer(tlk_table_global, &tlk_table);

  auto* const open_main = ResolveTlkTableOpenFile();
  auto* const open_alternate = ResolveTlkTableOpenFileAlternate();
  auto* const construct_string = ResolveMainExport<CExoStringFromCharFn>("??0CExoString@@QEAA@PEBD@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");

  if (tlk_table == nullptr || construct_string == nullptr ||
      (!alternate && open_main == nullptr) ||
      (alternate && open_alternate == nullptr)) {
    LogFormat(
        L"legacy TLK open skipped: reason=%s alternate=%d resref='%s' table_global=%p table=%p open_main=%p open_alt=%p construct=%p destroy=%p",
        reason != nullptr ? reason : L"<unspecified>",
        alternate ? 1 : 0,
        NarrowToWide(resref).c_str(),
        tlk_table_global,
        tlk_table,
        open_main,
        open_alternate,
        construct_string,
        destroy_string);
    return false;
  }

  std::vector<std::string> attempts;
  if (!resolved_path.empty()) {
    attempts.push_back(ToUtf8(resolved_path));
    attempts.push_back(ToUtf8(StripTlkExtension(resolved_path)));
  }
  attempts.push_back(std::string("HGTLK:") + resref);
  attempts.push_back(std::string("DIAMONDTLK:") + resref);
  attempts.push_back(resref);

  for (const std::string& attempt : attempts) {
    if (attempt.empty()) {
      continue;
    }

    int result = 0;
    bool raised_exception = false;
    result = CallOpenLegacyTlkFileNoExcept(
        tlk_table,
        open_main,
        open_alternate,
        construct_string,
        destroy_string,
        attempt.c_str(),
        alternate,
        &raised_exception);

    const LONG observation = InterlockedIncrement(&g_legacy_tlk_load_observations);
    LogFormat(
        L"legacy TLK open #%ld: reason=%s alternate=%d resref='%s' attempt='%s' resolved='%s' result=%d%s",
        observation,
        reason != nullptr ? reason : L"<unspecified>",
        alternate ? 1 : 0,
        NarrowToWide(resref).c_str(),
        NarrowToWide(attempt).c_str(),
        resolved_path.empty() ? L"<unresolved>" : resolved_path.c_str(),
        result,
        raised_exception ? L" exception" : L"");
    if (observation <= 16 || result != 0 || raised_exception) {
      LogTlkTableSnapshot(L"legacy TLK table after open attempt", tlk_table);
    }
    if (result != 0 && !raised_exception) {
      return true;
    }
  }

  return false;
}

bool EnsureLegacyDialogTlkLoaded(const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_DIALOG_TLK")) {
    return false;
  }

  const std::wstring dialog_path = ResolveLegacyTlkFile("dialog");
  const std::string dialog_path_utf8 = ToUtf8(dialog_path);
  if (!dialog_path_utf8.empty() && g_legacy_dialog_tlk_loaded_path == dialog_path_utf8) {
    return true;
  }

  if (OpenLegacyTlkFile("dialog", dialog_path, false, reason)) {
    g_legacy_dialog_tlk_loaded_path = dialog_path_utf8.empty() ? "dialog" : dialog_path_utf8;
    return true;
  }
  return false;
}

bool EnsureLegacyCustomTlkLoaded(const std::string& custom_tlk, const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_CUSTOM_TLK")) {
    return false;
  }

  const std::string normalized = NormalizeTlkResref(custom_tlk.empty() ? "cep23_v1" : custom_tlk);
  if (normalized.empty()) {
    LogFormat(
        L"legacy custom TLK load skipped: reason=%s invalid resref='%s'",
        reason != nullptr ? reason : L"<unspecified>",
        NarrowToWide(custom_tlk).c_str());
    return false;
  }

  const std::wstring custom_path = ResolveLegacyTlkFile(normalized);
  const std::string custom_path_utf8 = ToUtf8(custom_path);
  if (!custom_path_utf8.empty() && g_legacy_custom_tlk_loaded_path == custom_path_utf8) {
    return true;
  }

  if (OpenLegacyTlkFile(normalized, custom_path, true, reason)) {
    g_legacy_custom_tlk_loaded_path = custom_path_utf8.empty() ? normalized : custom_path_utf8;
    return true;
  }
  return false;
}

bool ReadLegacyModuleInfoRawString(
    const unsigned char* packet,
    size_t packet_size,
    size_t* cursor,
    std::string* value) {
  if (packet == nullptr || cursor == nullptr || value == nullptr || *cursor > packet_size ||
      packet_size - *cursor < 4) {
    return false;
  }

  const uint32_t length = ReadU32LeRaw(packet + *cursor);
  *cursor += 4;
  constexpr uint32_t kMaxReasonableModuleInfoString = 4096;
  if (length > kMaxReasonableModuleInfoString || length > packet_size - *cursor) {
    return false;
  }

  value->assign(reinterpret_cast<const char*>(packet + *cursor), reinterpret_cast<const char*>(packet + *cursor + length));
  *cursor += length;
  return true;
}

std::string ParseModuleInfoCustomTlkFromPacket(const unsigned char* packet, uint32_t packet_size) {
  if (packet == nullptr || packet_size < 16 || packet[0] != 'P' || packet[1] != 3 || packet[2] != 1) {
    return {};
  }

  const uint32_t declared = ReadU32LeRaw(packet + 3);
  if (declared < 4 || declared > packet_size) {
    return {};
  }

  size_t cursor = 7;
  std::string module_name;
  std::string module_description;
  std::string custom_tlk;
  if (!ReadLegacyModuleInfoRawString(packet, packet_size, &cursor, &module_name) ||
      !ReadLegacyModuleInfoRawString(packet, packet_size, &cursor, &module_description)) {
    return {};
  }

  if (cursor >= packet_size) {
    return {};
  }
  ++cursor;
  if (!ReadLegacyModuleInfoRawString(packet, packet_size, &cursor, &custom_tlk)) {
    return {};
  }

  return NormalizeTlkResref(custom_tlk);
}

void EnsureLegacyModuleTlkLoadedFromPacket(
    const unsigned char* packet,
    uint32_t packet_size,
    const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_TLK_LOAD")) {
    return;
  }

  const std::string custom_tlk = ParseModuleInfoCustomTlkFromPacket(packet, packet_size);
  AddLegacyTlkAliases(reason);
  const bool dialog_ok = EnsureLegacyDialogTlkLoaded(reason);
  const bool custom_ok = EnsureLegacyCustomTlkLoaded(custom_tlk, reason);
  LogFormat(
      L"legacy module TLK load complete: reason=%s custom='%s' dialog_ok=%d custom_ok=%d",
      reason != nullptr ? reason : L"<unspecified>",
      NarrowToWide(custom_tlk.empty() ? std::string("cep23_v1") : custom_tlk).c_str(),
      dialog_ok ? 1 : 0,
      custom_ok ? 1 : 0);
}

std::wstring FormatNetPeer(const NetPeerView& peer) {
  wchar_t buffer[96]{};
  swprintf_s(
      buffer,
      L"family=%hu address=%s port=%hu",
      peer.family,
      FormatIpv4(peer.address).c_str(),
      ByteSwap16(peer.port_network));
  return buffer;
}

std::wstring FormatSockaddr(const sockaddr* address, int address_length) {
  if (address == nullptr) {
    return L"<null>";
  }
  if (address_length < static_cast<int>(sizeof(sockaddr_in)) || address->sa_family != AF_INET) {
    wchar_t buffer[96]{};
    swprintf_s(buffer, L"family=%hu length=%d", static_cast<unsigned short>(address->sa_family), address_length);
    return buffer;
  }

  auto* const ipv4 = reinterpret_cast<const sockaddr_in*>(address);
  const auto* const bytes = reinterpret_cast<const uint8_t*>(&ipv4->sin_addr.S_un.S_addr);
  wchar_t buffer[128]{};
  swprintf_s(
      buffer,
      L"family=%hu address=%u.%u.%u.%u port=%hu",
      static_cast<unsigned short>(address->sa_family),
      bytes[0],
      bytes[1],
      bytes[2],
      bytes[3],
      ByteSwap16(ipv4->sin_port));
  return buffer;
}

bool SockaddrTargetsAutoConnect(const sockaddr* address, int address_length) {
  if (address == nullptr || address_length < static_cast<int>(sizeof(sockaddr_in)) || address->sa_family != AF_INET ||
      !g_auto_connect_ip_known) {
    return false;
  }

  const uint8_t* const address_bytes =
      reinterpret_cast<const uint8_t*>(&reinterpret_cast<const sockaddr_in*>(address)->sin_addr.S_un.S_addr);
  return std::memcmp(address_bytes, g_auto_connect_ip, sizeof(g_auto_connect_ip)) == 0;
}

std::wstring FormatExoNetConnectionSlot(void* exonet_internal, uint32_t connection_id) {
  void* const begin = ReadPointerField(exonet_internal, 0x20);
  void* const end = ReadPointerField(exonet_internal, 0x28);
  if (begin == nullptr || end == nullptr) {
    wchar_t buffer[160]{};
    swprintf_s(buffer, L"connection=%u slot=<unavailable> begin=%p end=%p", connection_id, begin, end);
    return buffer;
  }

  const uintptr_t begin_address = reinterpret_cast<uintptr_t>(begin);
  const uintptr_t end_address = reinterpret_cast<uintptr_t>(end);
  if (end_address < begin_address) {
    wchar_t buffer[160]{};
    swprintf_s(buffer, L"connection=%u slot=<invalid-vector> begin=%p end=%p", connection_id, begin, end);
    return buffer;
  }

  constexpr uintptr_t kConnectionSlotSize = 0x150;
  const uintptr_t byte_count = end_address - begin_address;
  const uintptr_t slot_count = byte_count / kConnectionSlotSize;
  if (connection_id >= slot_count) {
    wchar_t buffer[160]{};
    swprintf_s(buffer, L"connection=%u slot=<out-of-range> count=%zu", connection_id, static_cast<size_t>(slot_count));
    return buffer;
  }

  auto* const slot = reinterpret_cast<uint8_t*>(begin_address + (static_cast<uintptr_t>(connection_id) * kConnectionSlotSize));
  uint32_t in_use = 0xFFFFFFFFu;
  uint32_t relay_mode = 0xFFFFFFFFu;
  uint32_t relays_enabled = 0xFFFFFFFFu;
  uint32_t relay_peer_enabled = 0xFFFFFFFFu;
  uint32_t relay_address = 0;
  uint16_t relay_port = 0;
  SafeReadUInt32(reinterpret_cast<uint32_t*>(slot + 0x04), &in_use);
  SafeReadUInt32(reinterpret_cast<uint32_t*>(slot + 0x10), &relay_mode);
  SafeReadUInt32(reinterpret_cast<uint32_t*>(slot + 0x14), &relay_address);
  SafeReadUInt16(reinterpret_cast<uint16_t*>(slot + 0x18), &relay_port);
  SafeReadUInt32(reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(exonet_internal) + 0x1007AC), &relays_enabled);
  SafeReadUInt32(reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(exonet_internal) + 0x100784), &relay_peer_enabled);

  NetPeerView actual_peer{};
  const bool actual_ok = ReadNetPeer(slot + 0x20, &actual_peer);
  wchar_t buffer[320]{};
  swprintf_s(
      buffer,
      L"connection=%u slot=%p in_use=%u relay_mode=%u relay_peer=%u:%hu relays_enabled=%u relay_peer_enabled=%u actual=%s",
      connection_id,
      slot,
      in_use,
      relay_mode,
      relay_address,
      ByteSwap16(relay_port),
      relays_enabled,
      relay_peer_enabled,
      actual_ok ? FormatNetPeer(actual_peer).c_str() : L"<unreadable>");
  return buffer;
}

std::wstring FormatMessagePrefix(const unsigned char* message, uint32_t length) {
  if (message == nullptr || length == 0) {
    return L"<empty>";
  }

  unsigned char bytes[4]{};
  const uint32_t count = std::min<uint32_t>(length, sizeof(bytes));
  if (!TryCopyBytes(reinterpret_cast<const char*>(message), count, reinterpret_cast<char*>(bytes))) {
    return L"<read failed>";
  }

  wchar_t text[5]{};
  for (uint32_t index = 0; index < count; ++index) {
    const unsigned char ch = bytes[index];
    text[index] = ch >= 0x20 && ch <= 0x7E ? static_cast<wchar_t>(ch) : L'.';
  }
  for (uint32_t index = count; index < 4; ++index) {
    text[index] = L' ';
  }

  wchar_t buffer[96]{};
  swprintf_s(
      buffer,
      L"'%s' [%02X %02X %02X %02X]",
      text,
      static_cast<unsigned int>(bytes[0]),
      static_cast<unsigned int>(bytes[1]),
      static_cast<unsigned int>(bytes[2]),
      static_cast<unsigned int>(bytes[3]));
  return buffer;
}

void LogPacketDump(
    const wchar_t* direction,
    LONG observation,
    const wchar_t* stage,
    uint32_t connection_id,
    const unsigned char* message,
    size_t length,
    const std::wstring& peer = L"") {
  constexpr size_t kMaxDumpedPacketBytes = 384;
  if (message == nullptr || length == 0) {
    return;
  }

  LogFormat(
      L"packet dump %s #%ld stage=%s connection=%u length=%zu peer=%s prefix=%s bytes=[%s]",
      direction,
      observation,
      stage,
      connection_id,
      length,
      peer.empty() ? L"-" : peer.c_str(),
      FormatMessagePrefix(message, static_cast<uint32_t>(std::min<size_t>(length, 4))).c_str(),
      FormatLimitedBytes(reinterpret_cast<const uint8_t*>(message), length, kMaxDumpedPacketBytes).c_str());
}

bool MessageStartsWithByte(const unsigned char* message, uint32_t length, unsigned char prefix);

std::wstring LookupMajorMinorNameBestEffort(unsigned char major, unsigned char minor) {
  if (g_lookup_major_minor_name == nullptr) {
    return L"<lookup unavailable>";
  }
  const char* const name = g_lookup_major_minor_name(major, minor);
  if (name == nullptr || name[0] == '\0') {
    return L"<unknown>";
  }
  return NarrowToWide(name);
}

std::wstring FormatOutboundGamePacketDetail(const unsigned char* message, uint32_t length) {
  if (message == nullptr || length == 0) {
    return L"<empty>";
  }

  wchar_t detail[1536]{};
  if (length >= 15 && message[12] == 0x70) {
    const unsigned char major = message[13];
    const unsigned char minor = message[14];
    const uint32_t high_size = length - 12;
    const uint32_t body_size = length - 15;
    const std::wstring name = LookupMajorMinorNameBestEffort(major, minor);
    const std::wstring input_detail =
        FormatClientInputPayloadDetail(major, minor, message + 12, high_size);

    swprintf_s(
        detail,
        L"high=[70 %02X %02X] name='%s' high_size=%u body_size=%u%s body=[%s]",
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        name.c_str(),
        high_size,
        body_size,
        input_detail.c_str(),
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(message + 15), body_size, 160).c_str());
    return detail;
  }

  size_t embedded_header = SIZE_MAX;
  const uint32_t scan_end = length >= 3 ? std::min<uint32_t>(length - 2, 40) : 0;
  for (uint32_t index = 0; index < scan_end; ++index) {
    if (message[index] == 0x70) {
      embedded_header = index;
      break;
    }
  }
  if (embedded_header != SIZE_MAX && embedded_header + 2 < length) {
    const unsigned char major = message[embedded_header + 1];
    const unsigned char minor = message[embedded_header + 2];
    const std::wstring name = LookupMajorMinorNameBestEffort(major, minor);
    swprintf_s(
        detail,
        L"high=<not-at-expected-offset> embedded70=%zu high=[70 %02X %02X] name='%s' bytes=[%s]",
        embedded_header,
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        name.c_str(),
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(message), length, 160).c_str());
    return detail;
  }

  swprintf_s(
      detail,
      L"high=<none> bytes=[%s]",
      FormatLimitedBytes(reinterpret_cast<const uint8_t*>(message), length, 96).c_str());
  return detail;
}

void MaybeLogOutboundGamePacket(
    const wchar_t* stage,
    LONG stage_observation,
    uint32_t connection_id,
    const unsigned char* message,
    uint32_t length,
    uint32_t flags) {
  if (message == nullptr || length == 0 || !MessageStartsWithByte(message, length, 'M')) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_outbound_game_packet_detail_observations);
  const bool post_area =
      g_client_to_server_area_loaded_observations > 0 || g_synthetic_area_loaded_observations > 0;
  const bool has_expected_header = length >= 15 && message[12] == 0x70;
  const unsigned char major = has_expected_header ? message[13] : 0;
  const bool input_or_transition_related =
      has_expected_header &&
      (major == 4 || major == 5 || major == 6 || major == 14 || major == 18 ||
       major == 30 || major == 44 || major == 49 || major == 50);
  const bool verbose = IsTruthyEnvironmentFlag(L"HG_BRIDGE_VERBOSE_OUTBOUND_GAME_PACKETS");
  const bool short_keepalive = length <= 12;
  const bool should_log =
      verbose ||
      observation <= 80 ||
      input_or_transition_related ||
      (post_area && !short_keepalive && observation <= 1000) ||
      (post_area && short_keepalive && (observation % 250) == 0);

  if (!should_log) {
    return;
  }

  LogFormat(
      L"outbound game packet detail #%ld: stage=%s stage_observation=%ld connection=%u length=%u flags=%u post_area=%d prefix=%s %s",
      observation,
      stage != nullptr ? stage : L"<unknown>",
      stage_observation,
      connection_id,
      length,
      flags,
      post_area ? 1 : 0,
      FormatMessagePrefix(message, length).c_str(),
      FormatOutboundGamePacketDetail(message, length).c_str());
}

uint32_t LegacyBncsPrivateBuild() {
  uint32_t value = kNwn169PrivateBuild;
  const std::wstring override_value = GetEnvironmentString(L"HG_BRIDGE_LEGACY_BNCS_BUILD_NUMBER");
  if (!override_value.empty() && !ParseUnsigned32(override_value, &value)) {
    LogFormat(L"legacy BNCS build override ignored: invalid HG_BRIDGE_LEGACY_BNCS_BUILD_NUMBER='%s'", override_value.c_str());
    return kNwn169PrivateBuild;
  }
  return value;
}

uint16_t LegacyBncsBuildField() {
  uint32_t value = kDiamondBncsBuildField;
  const std::wstring override_value = GetEnvironmentString(L"HG_BRIDGE_LEGACY_BNCS_BUILD_FIELD");
  if (override_value.empty()) {
    return static_cast<uint16_t>(value);
  }
  if (!ParseUnsigned32Flexible(override_value, &value) || value > 0xFFFFu) {
    LogFormat(L"legacy BNCS build-field override ignored: invalid HG_BRIDGE_LEGACY_BNCS_BUILD_FIELD='%s'", override_value.c_str());
    return kDiamondBncsBuildField;
  }
  LogFormat(L"legacy BNCS build-field override active: native=0x%04X override=0x%04X", kDiamondBncsBuildField, value);
  return static_cast<uint16_t>(value);
}

uint16_t GetClientUdpPortForBncs(void* cnet_layer_internal) {
  auto* const get_udp_port = ResolveMainExport<GetUdpPortFn>("?GetUDPPort@CExoNet@@QEAAIXZ");
  void* const exonet = ReadPointerField(cnet_layer_internal, 0x58);
  if (get_udp_port == nullptr || exonet == nullptr) {
    return 0;
  }

  __try {
    return static_cast<uint16_t>(get_udp_port(exonet) & 0xFFFFu);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return 0;
  }
}

uint32_t LegacyBncsClientChallenge() {
  static const uint32_t value = GetTickCount();
  const std::wstring override_value = GetEnvironmentString(L"HG_BRIDGE_LEGACY_BNCS_CLIENT_CHALLENGE");
  if (override_value.empty()) {
    return value;
  }

  uint32_t override_number = 0;
  if (!ParseUnsigned32Flexible(override_value, &override_number)) {
    LogFormat(
        L"legacy BNCS client challenge override ignored: invalid HG_BRIDGE_LEGACY_BNCS_CLIENT_CHALLENGE='%s'",
        override_value.c_str());
    return value;
  }

  LogFormat(
      L"legacy BNCS client challenge override active: native=0x%08X override=0x%08X",
      value,
      override_number);
  return override_number;
}

std::string LegacyBncsString(const CExoStringView* primary, const CExoStringView* fallback, bool use_diamond_fallback) {
  std::string value = CExoStringToString(primary, 255);
  if (value.empty()) {
    value = CExoStringToString(fallback, 255);
  }
  if (value.empty() && use_diamond_fallback && !g_diamond_public_cd_keys.empty()) {
    value = g_diamond_public_cd_keys[0].substr(0, 255);
  }
  return value;
}

bool TryReadDiamondPublicKeyOverride(
    const wchar_t* env_name,
    const wchar_t* label,
    std::string* value,
    size_t* zero_based_index) {
  if (env_name == nullptr || value == nullptr || zero_based_index == nullptr) {
    return false;
  }

  const std::wstring override_text = GetEnvironmentString(env_name);
  if (override_text.empty()) {
    return false;
  }

  uint32_t one_based_index = 0;
  if (!ParseUnsigned32Flexible(override_text, &one_based_index) ||
      one_based_index == 0 ||
      one_based_index > g_diamond_public_cd_keys.size()) {
    LogFormat(
        L"legacy BNCS %s public-key override ignored: invalid %s='%s' valid_range=1..%zu",
        label != nullptr ? label : L"<unknown>",
        env_name,
        override_text.c_str(),
        g_diamond_public_cd_keys.size());
    return false;
  }

  *zero_based_index = one_based_index - 1;
  *value = g_diamond_public_cd_keys[*zero_based_index].substr(0, 255);
  return !value->empty();
}

void AppendU32Le(std::vector<unsigned char>* packet, uint32_t value) {
  packet->push_back(static_cast<unsigned char>(value & 0xFFu));
  packet->push_back(static_cast<unsigned char>((value >> 8) & 0xFFu));
  packet->push_back(static_cast<unsigned char>((value >> 16) & 0xFFu));
  packet->push_back(static_cast<unsigned char>((value >> 24) & 0xFFu));
}

void AppendU16Le(std::vector<unsigned char>* packet, uint16_t value) {
  packet->push_back(static_cast<unsigned char>(value & 0xFFu));
  packet->push_back(static_cast<unsigned char>((value >> 8) & 0xFFu));
}

void AppendLegacyCExoString(std::vector<unsigned char>* packet, const std::string& value) {
  const size_t length = std::min<size_t>(value.size(), 255);
  packet->push_back(static_cast<unsigned char>(length));
  packet->insert(packet->end(), value.data(), value.data() + length);
}

bool ReadLegacyLengthPrefixedSegment(
    const unsigned char* message,
    uint32_t length,
    uint32_t* cursor,
    std::string* value) {
  if (message == nullptr || cursor == nullptr || value == nullptr || *cursor >= length) {
    return false;
  }

  const uint32_t segment_length = message[*cursor];
  ++(*cursor);
  if (segment_length > length - *cursor) {
    return false;
  }

  value->assign(
      reinterpret_cast<const char*>(message + *cursor),
      reinterpret_cast<const char*>(message + *cursor + segment_length));
  *cursor += segment_length;
  return true;
}

bool ObserveLegacyBncrChallenges(uint32_t connection_id, const unsigned char* message, uint32_t length) {
  if (message == nullptr || length < 8 || std::memcmp(message, "BNCR", 4) != 0) {
    return false;
  }

  const unsigned char status = message[6];
  if (status != 'V' && status != 'P') {
    const LONG observation = InterlockedIncrement(&g_legacy_bncr_non_challenge_observations);
    if (observation <= 20 || status == 'R') {
      const wchar_t printable_status = status >= 0x20 && status <= 0x7E ? static_cast<wchar_t>(status) : L'.';
      const unsigned int detail = length > 7 ? static_cast<unsigned int>(message[7]) : 0;
      LogFormat(
          L"legacy BNCR non-challenge #%ld observed: connection=%u length=%u status='%c'/0x%02X detail=0x%02X bytes=[%s]",
          observation,
          connection_id,
          length,
          printable_status,
          static_cast<unsigned int>(status),
          detail,
          FormatBytes(reinterpret_cast<const uint8_t*>(message), std::min<uint32_t>(length, 32)).c_str());
    }
    return true;
  }

  uint32_t cursor = 7;
  std::string game_password_challenge;
  std::string cd_key_challenge;
  std::string mst_password_challenge;
  if (status == 'P' && !ReadLegacyLengthPrefixedSegment(message, length, &cursor, &game_password_challenge)) {
    return false;
  }
  if (!ReadLegacyLengthPrefixedSegment(message, length, &cursor, &cd_key_challenge) ||
      !ReadLegacyLengthPrefixedSegment(message, length, &cursor, &mst_password_challenge) ||
      cursor != length) {
    return false;
  }

  AcquireSRWLockExclusive(&g_legacy_bncr_lock);
  g_legacy_bncr_status = status;
  g_legacy_bncr_cd_key_challenge = cd_key_challenge;
  g_legacy_bncr_mst_password_challenge = mst_password_challenge;
  ReleaseSRWLockExclusive(&g_legacy_bncr_lock);

  const LONG observation = InterlockedIncrement(&g_legacy_bncr_challenge_observations);
  if (observation <= 20) {
    LogFormat(
        L"legacy BNCR challenge #%ld captured: status='%c' game_len=%zu cd_len=%zu mst_len=%zu",
        observation,
        static_cast<wchar_t>(status),
        game_password_challenge.size(),
        cd_key_challenge.size(),
        mst_password_challenge.size());
  }
  return false;
}

bool CopyLatestLegacyBncrChallenge(std::string* cd_key_challenge, unsigned char* status) {
  if (cd_key_challenge == nullptr) {
    return false;
  }

  AcquireSRWLockShared(&g_legacy_bncr_lock);
  if (status != nullptr) {
    *status = g_legacy_bncr_status;
  }
  *cd_key_challenge = g_legacy_bncr_cd_key_challenge;
  ReleaseSRWLockShared(&g_legacy_bncr_lock);
  return !cd_key_challenge->empty();
}

bool LegacyGamePacketReplayDisabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_M_REPLAY");
}

bool IsLegacyStartupGamePacket(const unsigned char* message, uint32_t length) {
  if (message == nullptr || length != 15) {
    return false;
  }

  unsigned char bytes[15]{};
  if (!TryCopyBytes(reinterpret_cast<const char*>(message), sizeof(bytes), reinterpret_cast<char*>(bytes))) {
    return false;
  }

  return bytes[0] == 'M' &&
      bytes[3] == 0x00 &&
      bytes[5] == 0xFF &&
      bytes[6] == 0xFF &&
      bytes[8] == 0x00 &&
      bytes[9] == 0x01 &&
      bytes[10] == 0x00 &&
      bytes[11] == 0x03 &&
      bytes[12] == 0x70;
}

void BeginLegacyGamePacketGate(uint32_t connection_id) {
  if (LegacyGamePacketReplayDisabled()) {
    return;
  }

  AcquireSRWLockExclusive(&g_legacy_game_packet_gate_lock);
  g_legacy_game_packet_gate_active = true;
  g_legacy_bnvr_accepted = false;
  g_legacy_game_packet_gate_rejected = false;
  g_legacy_game_packet_gate_connection_id = connection_id;
  g_legacy_pending_game_packets.clear();
  ReleaseSRWLockExclusive(&g_legacy_game_packet_gate_lock);

  const LONG observation = InterlockedIncrement(&g_legacy_game_packet_gate_begin_observations);
  if (observation <= 20) {
    LogFormat(
        L"legacy M replay gate #%ld armed: connection=%u awaiting BNVR accept",
        observation,
        connection_id);
  }
}

void ClearLegacyGamePacketGate(uint32_t connection_id, const wchar_t* reason) {
  if (LegacyGamePacketReplayDisabled()) {
    return;
  }

  size_t cleared = 0;
  bool was_active = false;
  AcquireSRWLockExclusive(&g_legacy_game_packet_gate_lock);
  if (g_legacy_game_packet_gate_active &&
      (connection_id == 0 || g_legacy_game_packet_gate_connection_id == connection_id)) {
    was_active = true;
    cleared = g_legacy_pending_game_packets.size();
    g_legacy_game_packet_gate_active = false;
    g_legacy_bnvr_accepted = false;
    g_legacy_game_packet_gate_rejected = false;
    g_legacy_game_packet_gate_connection_id = 0;
    g_legacy_pending_game_packets.clear();
  }
  ReleaseSRWLockExclusive(&g_legacy_game_packet_gate_lock);

  if (was_active) {
    LogFormat(
        L"legacy M replay gate cleared: connection=%u pending=%zu reason=%s",
        connection_id,
        cleared,
        reason != nullptr ? reason : L"<unknown>");
  }
}

void RejectLegacyGamePacketGate(uint32_t connection_id, const wchar_t* reason) {
  if (LegacyGamePacketReplayDisabled()) {
    return;
  }

  size_t cleared = 0;
  AcquireSRWLockExclusive(&g_legacy_game_packet_gate_lock);
  cleared = g_legacy_pending_game_packets.size();
  g_legacy_game_packet_gate_active = true;
  g_legacy_bnvr_accepted = false;
  g_legacy_game_packet_gate_rejected = true;
  g_legacy_game_packet_gate_connection_id = connection_id;
  g_legacy_pending_game_packets.clear();
  ReleaseSRWLockExclusive(&g_legacy_game_packet_gate_lock);

  LogFormat(
      L"legacy M replay gate rejected: connection=%u cleared=%zu reason=%s; startup M packets will be suppressed until the next BNCS",
      connection_id,
      cleared,
      reason != nullptr ? reason : L"<unknown>");
}

bool QueueLegacyGamePacketIfNeeded(
    uint32_t connection_id,
    const unsigned char* message,
    uint32_t length,
    uint32_t flags,
    LONG send_observation) {
  constexpr size_t kMaxPendingPackets = 3;
  constexpr uint32_t kMaxPendingPacketLength = 2048;

  if (LegacyGamePacketReplayDisabled() || message == nullptr || length == 0) {
    return false;
  }
  if (length > kMaxPendingPacketLength) {
    return false;
  }
  if (!IsLegacyStartupGamePacket(message, length)) {
    // Short keepalives and unrelated reliable packets need to keep flowing
    // while BNVR is outstanding. The only packets that have proven order-
    // sensitive are the three startup payloads replayed after BNVR accepts.
    return false;
  }

  PendingLegacyGamePacket pending;
  pending.connection_id = connection_id;
  pending.flags = flags;
  pending.bytes.resize(length);
  if (!TryCopyBytes(
          reinterpret_cast<const char*>(message),
          length,
          reinterpret_cast<char*>(pending.bytes.data()))) {
    return false;
  }

  size_t queued = 0;
  bool should_queue = false;
  bool should_suppress = false;
  AcquireSRWLockExclusive(&g_legacy_game_packet_gate_lock);
  if (g_legacy_game_packet_gate_active &&
      !g_legacy_bnvr_accepted &&
      g_legacy_game_packet_gate_connection_id == connection_id) {
    if (g_legacy_game_packet_gate_rejected) {
      queued = g_legacy_pending_game_packets.size();
      should_suppress = true;
    } else if (g_legacy_pending_game_packets.size() < kMaxPendingPackets) {
      g_legacy_pending_game_packets.push_back(std::move(pending));
      queued = g_legacy_pending_game_packets.size();
      should_queue = true;
    } else {
      queued = g_legacy_pending_game_packets.size();
      should_suppress = true;
    }
  }
  ReleaseSRWLockExclusive(&g_legacy_game_packet_gate_lock);

  if (should_queue) {
    const LONG observation = InterlockedIncrement(&g_legacy_game_packet_queue_observations);
    if (observation <= 40) {
      LogFormat(
          L"legacy M packet queued #%ld: connection=%u length=%u flags=%u queued=%zu source_send=%ld prefix=%s",
          observation,
          connection_id,
          length,
          flags,
          queued,
          send_observation,
          FormatMessagePrefix(message, length).c_str());
    }
  } else if (should_suppress) {
    const LONG observation = InterlockedIncrement(&g_legacy_game_packet_suppressed_observations);
    if (observation <= 40) {
      LogFormat(
          L"legacy M startup packet suppressed #%ld: connection=%u length=%u flags=%u queued=%zu source_send=%ld prefix=%s",
          observation,
          connection_id,
          length,
          flags,
          queued,
          send_observation,
          FormatMessagePrefix(message, length).c_str());
    }
  }
  return should_queue || should_suppress;
}

void ObserveLegacyBnvrResponse(uint32_t connection_id, const unsigned char* message, uint32_t length) {
  if (LegacyGamePacketReplayDisabled() ||
      message == nullptr ||
      length < 5 ||
      std::memcmp(message, "BNVR", 4) != 0) {
    return;
  }

  const unsigned char status = message[4];
  bool gate_matches = false;
  size_t pending = 0;
  AcquireSRWLockExclusive(&g_legacy_game_packet_gate_lock);
  gate_matches = g_legacy_game_packet_gate_active &&
      g_legacy_game_packet_gate_connection_id == connection_id;
  if (gate_matches) {
    pending = g_legacy_pending_game_packets.size();
    g_legacy_bnvr_accepted = status == 'A';
  }
  ReleaseSRWLockExclusive(&g_legacy_game_packet_gate_lock);

  const LONG observation = InterlockedIncrement(&g_legacy_bnvr_response_observations);
  if (observation <= 20 || gate_matches) {
    const wchar_t printable_status = status >= 0x20 && status <= 0x7E ? static_cast<wchar_t>(status) : L'.';
    LogFormat(
        L"legacy BNVR response #%ld observed: connection=%u status='%c'/0x%02X gate_matches=%d pending=%zu",
        observation,
        connection_id,
        printable_status,
        static_cast<unsigned int>(status),
        gate_matches ? 1 : 0,
        pending);
  }

  if (gate_matches && status != 'A') {
    ClearLegacyGamePacketGate(connection_id, L"BNVR was not accepted");
  }
}

void ReleaseLegacyQueuedGamePackets(
    void* cnet_layer_internal,
    uint32_t connection_id,
    int handler_result,
    bool require_bnvr_accept,
    const wchar_t* reason) {
  if (LegacyGamePacketReplayDisabled() || g_send_direct_message_original == nullptr || handler_result == 0) {
    return;
  }

  std::vector<PendingLegacyGamePacket> pending;
  AcquireSRWLockExclusive(&g_legacy_game_packet_gate_lock);
  if (g_legacy_game_packet_gate_active &&
      g_legacy_game_packet_gate_connection_id == connection_id &&
      (!require_bnvr_accept || g_legacy_bnvr_accepted)) {
    pending.swap(g_legacy_pending_game_packets);
    g_legacy_game_packet_gate_active = false;
    g_legacy_bnvr_accepted = false;
    g_legacy_game_packet_gate_connection_id = 0;
  }
  ReleaseSRWLockExclusive(&g_legacy_game_packet_gate_lock);

  if (pending.empty()) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_legacy_game_packet_replay_observations);
  LogFormat(
      L"legacy M replay #%ld: connection=%u packets=%zu after %s result=%d",
      observation,
      connection_id,
      pending.size(),
      reason != nullptr ? reason : L"<unknown>",
      handler_result);

  for (size_t index = 0; index < pending.size(); ++index) {
    PendingLegacyGamePacket& packet = pending[index];
    const int result = packet.bytes.empty()
        ? 0
        : g_send_direct_message_original(
              cnet_layer_internal,
              packet.connection_id,
              packet.bytes.data(),
              static_cast<uint32_t>(packet.bytes.size()),
              packet.flags);
    if (observation <= 20 || result == 0) {
      LogFormat(
          L"legacy M replay #%ld packet=%zu/%zu connection=%u length=%zu flags=%u prefix=%s result=%d",
          observation,
          index + 1,
          pending.size(),
          packet.connection_id,
          packet.bytes.size(),
          packet.flags,
          FormatMessagePrefix(packet.bytes.data(), static_cast<uint32_t>(packet.bytes.size())).c_str(),
          result);
    }
  }
}

void ReplayLegacyQueuedGamePackets(void* cnet_layer_internal, uint32_t connection_id, int bnvr_handle_result) {
  ReleaseLegacyQueuedGamePackets(
      cnet_layer_internal,
      connection_id,
      bnvr_handle_result,
      true,
      L"BNVR accept");
}

int SendLegacyBnvsMessage(
    void* cnet_layer_internal,
    const CExoStringView* cd_key_response,
    const CExoStringView* community_name_response,
    const CExoStringView* game_password_response,
    LONG observation) {
  if (g_send_direct_message_original == nullptr) {
    LogFormat(L"legacy BNVS #%ld skipped: SendDirectMessage original is not available", observation);
    return g_send_bnvs_message_original != nullptr
        ? g_send_bnvs_message_original(cnet_layer_internal, cd_key_response, community_name_response, game_password_response)
        : 0;
  }

  if (SafeCExoStringLength(game_password_response) != 0) {
    LogFormat(L"legacy BNVS #%ld skipped: game-password verifier layout is not implemented", observation);
    return g_send_bnvs_message_original != nullptr
        ? g_send_bnvs_message_original(cnet_layer_internal, cd_key_response, community_name_response, game_password_response)
        : 0;
  }

  unsigned char status = 0;
  std::string cd_key_challenge;
  if (!CopyLatestLegacyBncrChallenge(&cd_key_challenge, &status)) {
    LogFormat(L"legacy BNVS #%ld skipped: no captured BNCR CD key challenge is available", observation);
    return g_send_bnvs_message_original != nullptr
        ? g_send_bnvs_message_original(cnet_layer_internal, cd_key_response, community_name_response, game_password_response)
        : 0;
  }

  void* const app_manager = GetCurrentAppManager();
  ComputeDiamondPublicCdKeys(app_manager);

  auto* const encrypt_string =
      ResolveMainExport<CExoEncryptStringFn>("?EncryptString@CExoEncrypt@@QEAA?AVCExoString@@AEBV2@@Z");
  auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");
  if (encrypt_string == nullptr || destroy_string == nullptr || g_diamond_cd_keys.empty() || g_diamond_public_cd_keys.empty()) {
    LogFormat(
        L"legacy BNVS #%ld skipped: encrypt=%p destroy=%p raw_keys=%zu public_keys=%zu",
        observation,
        encrypt_string,
        destroy_string,
        g_diamond_cd_keys.size(),
        g_diamond_public_cd_keys.size());
    return g_send_bnvs_message_original != nullptr
        ? g_send_bnvs_message_original(cnet_layer_internal, cd_key_response, community_name_response, game_password_response)
        : 0;
  }

  std::vector<std::string> verifiers;
  const size_t verifier_count = std::min(g_diamond_cd_keys.size(), g_diamond_public_cd_keys.size());
  verifiers.reserve(verifier_count);
  for (size_t index = 0; index < verifier_count && index < 255; ++index) {
    std::string verifier;
    if (!BuildLegacyCdKeyVerifier(encrypt_string, destroy_string, index, cd_key_challenge, &verifier)) {
      LogFormat(L"legacy BNVS #%ld skipped: failed to build CD key verifier index=%zu", observation, index);
      return g_send_bnvs_message_original != nullptr
          ? g_send_bnvs_message_original(cnet_layer_internal, cd_key_response, community_name_response, game_password_response)
          : 0;
    }
    verifiers.push_back(std::move(verifier));
  }

  const std::string original_first_verifier = CExoStringToString(cd_key_response, 255);
  const std::string community_response = CExoStringToString(community_name_response, 255);
  if (verifiers.empty() || community_response.empty()) {
    LogFormat(
        L"legacy BNVS #%ld skipped: verifier_count=%zu community_len=%zu",
        observation,
        verifiers.size(),
        community_response.size());
    return g_send_bnvs_message_original != nullptr
        ? g_send_bnvs_message_original(cnet_layer_internal, cd_key_response, community_name_response, game_password_response)
        : 0;
  }

  uint32_t connection_id = 0;
  SafeReadUInt32(
      reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(cnet_layer_internal) + 0x28A56D0),
      &connection_id);

  std::vector<unsigned char> packet;
  packet.reserve(6 + verifiers.size() * 41 + 1 + community_response.size());
  packet.insert(packet.end(), {'B', 'N', 'V', 'S'});
  packet.push_back('V');
  packet.push_back(static_cast<unsigned char>(verifiers.size()));
  for (const std::string& verifier : verifiers) {
    AppendLegacyCExoString(&packet, verifier);
  }
  AppendLegacyCExoString(&packet, community_response);

  const int result = g_send_direct_message_original(
      cnet_layer_internal,
      connection_id,
      packet.data(),
      static_cast<uint32_t>(packet.size()),
      0);

  SafeWriteUInt32(reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(cnet_layer_internal) + 0x28A56DC), 2);

  const LONG legacy_observation = InterlockedIncrement(&g_legacy_bnvs_send_observations);
  const bool first_matches = !original_first_verifier.empty() && original_first_verifier == verifiers.front();
  if (legacy_observation <= 20 || result == 0) {
    LogFormat(
        L"legacy BNVS #%ld sent: connection=%u status='%c' verifier_count=%zu first_matches_ee=%d community_len=%zu total=%zu result=%d",
        observation,
        connection_id,
        static_cast<wchar_t>(status != 0 ? status : 'V'),
        verifiers.size(),
        first_matches ? 1 : 0,
        community_response.size(),
        packet.size(),
        result);
  }
  return result;
}

int SendLegacyBncsMessage(
    void* cnet_layer_internal,
    uint32_t connection_id,
    unsigned char connection_type,
    int auth_mode,
    const CExoStringView* player_name,
    const CExoStringView* public_cd_key,
    const CExoStringView* legacy_cd_key,
    LONG observation) {
  if (g_send_direct_message_original == nullptr) {
    LogFormat(L"legacy BNCS #%ld skipped: SendDirectMessage original is not available", observation);
    return g_send_bncs_message_original != nullptr
        ? g_send_bncs_message_original(cnet_layer_internal, connection_id, connection_type, auth_mode, player_name, public_cd_key, legacy_cd_key)
        : 0;
  }

  const std::string player = LegacyBncsString(player_name, nullptr, false);
  std::string public_key = LegacyBncsString(public_cd_key, legacy_cd_key, true);
  size_t override_key_index = 0;
  bool public_key_overridden = TryReadDiamondPublicKeyOverride(
      L"HG_BRIDGE_LEGACY_BNCS_PUBLIC_KEY_INDEX",
      L"primary",
      &public_key,
      &override_key_index);
  bool public_key_forced = false;
  if (!public_key_overridden &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_BNCS_PUBLIC_KEY_FORCE") &&
      !g_diamond_public_cd_keys.empty()) {
    override_key_index = 0;
    public_key = g_diamond_public_cd_keys[override_key_index].substr(0, 255);
    public_key_forced = true;
  }
  const uint16_t udp_port = GetClientUdpPortForBncs(cnet_layer_internal);
  const uint32_t private_build = LegacyBncsPrivateBuild();
  const uint16_t build_field = LegacyBncsBuildField();
  const uint32_t client_challenge = LegacyBncsClientChallenge();

  std::vector<unsigned char> packet;
  packet.reserve(0x13 + player.size() + 1 + public_key.size());
  packet.insert(packet.end(), {'B', 'N', 'C', 'S'});
  AppendU16Le(&packet, udp_port);
  packet.push_back(connection_type);
  AppendU32Le(&packet, private_build);
  AppendU16Le(&packet, build_field);
  packet.push_back(static_cast<unsigned char>(auth_mode & 0xFF));
  AppendU32Le(&packet, client_challenge);
  AppendLegacyCExoString(&packet, player);
  AppendLegacyCExoString(&packet, public_key);

  const int result = g_send_direct_message_original(
      cnet_layer_internal,
      connection_id,
      packet.data(),
      static_cast<uint32_t>(packet.size()),
      0);
  if (result != 0) {
    BeginLegacyGamePacketGate(connection_id);
  }

  const wchar_t printable_type = connection_type >= 0x20 && connection_type <= 0x7E ? static_cast<wchar_t>(connection_type) : L'.';
  LogFormat(
      L"legacy BNCS #%ld sent: connection=%u type='%c'/0x%02X auth_mode=%d udp_port=%hu build=%u field=0x%04X client_challenge=0x%08X lengths=%zu/%zu total=%zu result=%d",
      observation,
      connection_id,
      printable_type,
      static_cast<unsigned int>(connection_type),
      auth_mode,
      udp_port,
      private_build,
      build_field,
      client_challenge,
      player.size(),
      public_key.size(),
      packet.size(),
      result);
  if (public_key_overridden || public_key_forced) {
    LogFormat(
        L"legacy BNCS #%ld public key %s active: key_index=%zu public_len=%zu",
        observation,
        public_key_overridden ? L"override" : L"force",
        override_key_index + 1,
        public_key.size());
  }
  return result;
}

bool MessageHasPrefix(const unsigned char* message, uint32_t length, const char* prefix) {
  if (message == nullptr || prefix == nullptr || length < 4) {
    return false;
  }

  char bytes[4]{};
  if (!TryCopyBytes(reinterpret_cast<const char*>(message), sizeof(bytes), bytes)) {
    return false;
  }
  return std::memcmp(bytes, prefix, sizeof(bytes)) == 0;
}

bool MessageStartsWithByte(const unsigned char* message, uint32_t length, unsigned char prefix) {
  if (message == nullptr || length == 0) {
    return false;
  }

  unsigned char value = 0;
  if (!TryCopyBytes(reinterpret_cast<const char*>(message), sizeof(value), reinterpret_cast<char*>(&value))) {
    return false;
  }
  return value == prefix;
}

void RepairSessionPeerPort(const wchar_t* context, NetPeerView* peer) {
  if (peer == nullptr || !g_auto_connect_ip_known || g_auto_connect_port == 0) {
    return;
  }

  const bool is_ipv4 = peer->family == 2;
  const bool ip_matches = is_ipv4 && std::memcmp(peer->address, g_auto_connect_ip, sizeof(g_auto_connect_ip)) == 0;
  const unsigned short current_port = ByteSwap16(peer->port_network);
  const LONG observation = InterlockedIncrement(&g_session_peer_observations);
  if (observation <= 10 || ip_matches) {
    LogFormat(
        L"%s peer observed #%ld: family=%hu address=%s port=%hu target=%s:%hu",
        context,
        observation,
        peer->family,
        FormatIpv4(peer->address).c_str(),
        current_port,
        g_auto_connect_host.c_str(),
        g_auto_connect_port);
  }

  if (!ip_matches || current_port == g_auto_connect_port) {
    return;
  }

  peer->port_network = ByteSwap16(g_auto_connect_port);
  LogFormat(
      L"%s peer port repaired for %s: %hu -> %hu",
      context,
      FormatIpv4(peer->address).c_str(),
      current_port,
      g_auto_connect_port);
}

int ForwardStartEnumerateSessions(
    const wchar_t* context,
    void* cnet_layer,
    uint32_t* session_ids,
    int mode,
    void* net_peer,
    int arg5) {
  void* const internal = ReadPointerField(cnet_layer, 0x08);
  if (g_start_enumerate_sessions_internal == nullptr || internal == nullptr) {
    LogFormat(L"%s peer hook failed: missing internal net layer or internal export", context);
    return 0;
  }

  NetPeerView peer_copy{};
  void* forwarded_peer = net_peer;
  if (ReadNetPeer(net_peer, &peer_copy)) {
    RepairSessionPeerPort(context, &peer_copy);
    forwarded_peer = &peer_copy;
  } else {
    LogFormat(L"%s peer hook warning: failed to read CNetPeer at %p", context, net_peer);
  }

  return g_start_enumerate_sessions_internal(internal, session_ids, mode, forwarded_peer, arg5);
}

int __fastcall HookedStartEnumerateSessionsScalar(void* cnet_layer, unsigned int session_id, int mode, void* net_peer, int arg5) {
  uint32_t session_ids[5] = {session_id, session_id, session_id, session_id, session_id};
  return ForwardStartEnumerateSessions(L"StartEnumerateSessions(scalar)", cnet_layer, session_ids, mode, net_peer, arg5);
}

int __fastcall HookedStartEnumerateSessionsArray(void* cnet_layer, uint32_t* session_ids, int mode, void* net_peer, int arg5) {
  return ForwardStartEnumerateSessions(L"StartEnumerateSessions(array)", cnet_layer, session_ids, mode, net_peer, arg5);
}

int __fastcall HookedOpenStandardConnectionInternal(void* cnet_layer_internal, int slot, CExoStringView* address, int port) {
  const LONG observation = InterlockedIncrement(&g_open_standard_observations);
  const std::wstring address_text = CExoStringToWide(address);
  const int result = g_open_standard_connection_internal_original != nullptr
      ? g_open_standard_connection_internal_original(cnet_layer_internal, slot, address, port)
      : 0;
  if (observation <= 20 || address_text == g_auto_connect_host) {
    LogFormat(
        L"OpenStandardConnection #%ld: slot=%d address='%s' port=%d result=%d",
        observation,
        slot,
        address_text.c_str(),
        port,
        result);
  }
  return result;
}

int __fastcall HookedSendDirectMessage(
    void* cnet_layer_internal,
    uint32_t connection_id,
    unsigned char* message,
    uint32_t length,
    uint32_t flags) {
  const LONG observation = InterlockedIncrement(&g_send_direct_observations);
  const bool is_bncs = MessageHasPrefix(message, length, "BNCS");
  const bool is_game_packet = MessageStartsWithByte(message, length, 'M');
  if (PacketDumpEnabled() && (is_bncs || is_game_packet)) {
    const LONG dump_observation = InterlockedIncrement(&g_packet_dump_send_observations);
    if (dump_observation <= 120 || is_bncs) {
      LogPacketDump(L"out", dump_observation, L"SendDirect", connection_id, message, length);
    }
  }
  if (observation <= 40 || is_bncs || length == 0) {
    LogFormat(
        L"SendDirect #%ld begin: connection=%u length=%u flags=%u prefix=%s",
        observation,
        connection_id,
        length,
        flags,
        FormatMessagePrefix(message, length).c_str());
  }

  if (is_game_packet && QueueLegacyGamePacketIfNeeded(connection_id, message, length, flags, observation)) {
    if (observation <= 40) {
      LogFormat(
          L"SendDirect #%ld result: connection=%u length=%u flags=%u prefix=%s result=1 queued_until_bnvr=1",
          observation,
          connection_id,
          length,
          flags,
          FormatMessagePrefix(message, length).c_str());
    }
    return 1;
  }

  const int result = g_send_direct_message_original != nullptr
      ? g_send_direct_message_original(cnet_layer_internal, connection_id, message, length, flags)
      : 0;
  const bool should_log_failed_send = result == 0 && (connection_id != 0 || observation <= 80);
  if (observation <= 40 || should_log_failed_send || is_bncs) {
    LogFormat(
        L"SendDirect #%ld result: connection=%u length=%u flags=%u prefix=%s result=%d",
        observation,
        connection_id,
        length,
        flags,
        FormatMessagePrefix(message, length).c_str(),
        result);
  }
  return result;
}

int __fastcall HookedExoNetSendMessage(
    void* exonet_internal,
    uint32_t connection_id,
    unsigned char* message,
    uint32_t length,
    uint32_t flags) {
  const LONG observation = InterlockedIncrement(&g_exonet_send_observations);
  const bool is_bncs = MessageHasPrefix(message, length, "BNCS");
  const bool is_game_packet = MessageStartsWithByte(message, length, 'M');
  if (PacketDumpEnabled() && (is_bncs || is_game_packet)) {
    const LONG dump_observation = InterlockedIncrement(&g_packet_dump_send_observations);
    if (dump_observation <= 120 || is_bncs) {
      LogPacketDump(L"out", dump_observation, L"ExoNet::SendMessageA", connection_id, message, length);
    }
  }
  if (observation <= 40 || is_bncs || length == 0) {
    LogFormat(
        L"ExoNet::SendMessageA #%ld begin: connection=%u length=%u flags=%u prefix=%s",
        observation,
        connection_id,
        length,
        flags,
        FormatMessagePrefix(message, length).c_str());
  }
  MaybeLogOutboundGamePacket(
      L"ExoNet::SendMessageA",
      observation,
      connection_id,
      message,
      length,
      flags);

  const int result = g_exonet_send_message_original != nullptr
      ? g_exonet_send_message_original(exonet_internal, connection_id, message, length, flags)
      : 0;
  const bool should_log_failed_send = result == 0 && (connection_id != 0 || observation <= 80);
  if (observation <= 40 || should_log_failed_send || is_bncs) {
    LogFormat(
        L"ExoNet::SendMessageA #%ld result: connection=%u length=%u flags=%u prefix=%s result=%d",
        observation,
        connection_id,
        length,
        flags,
        FormatMessagePrefix(message, length).c_str(),
        result);
  }
  if (result == 0 && is_game_packet && connection_id != 0) {
    const LONG failed_observation = InterlockedIncrement(&g_exonet_failed_m_observations);
    if (failed_observation <= 20) {
      LogFormat(
          L"ExoNet::SendMessageA failed M-slot #%ld: %s",
          failed_observation,
          FormatExoNetConnectionSlot(exonet_internal, connection_id).c_str());
    }
  }
  return result;
}

uint32_t* __fastcall HookedSetNetworkAddressData(
    void* exonet,
    uint32_t* out_connection_id,
    const void* net_peer,
    int relay_mode,
    const char* relay_token) {
  const LONG observation = InterlockedIncrement(&g_set_network_address_observations);
  NetPeerView peer{};
  const bool peer_ok = ReadNetPeer(const_cast<void*>(net_peer), &peer);
  const bool peer_matches = peer_ok && g_auto_connect_ip_known &&
      std::memcmp(peer.address, g_auto_connect_ip, sizeof(g_auto_connect_ip)) == 0;

  uint32_t* const result_ptr = g_set_network_address_data_original != nullptr
      ? g_set_network_address_data_original(exonet, out_connection_id, net_peer, relay_mode, relay_token)
      : out_connection_id;

  uint32_t connection_id = 0xFFFFFFFFu;
  SafeReadUInt32(result_ptr, &connection_id);

  if (observation <= 20 || (peer_matches && observation <= 30) || connection_id == 0xFFFFFFFFu) {
    LogFormat(
        L"SetNetworkAddressData #%ld: peer=%s relay_mode=%d result_connection=%u token=%p",
        observation,
        peer_ok ? FormatNetPeer(peer).c_str() : L"<unreadable>",
        relay_mode,
        connection_id,
        relay_token);
  }
  return result_ptr;
}

int __fastcall HookedHandleBnerMessage(void* cnet_layer_internal, uint32_t connection_id, void* shared_stream) {
  const LONG observation = InterlockedIncrement(&g_bner_observations);
  if (observation <= 10) {
    LogFormat(L"HandleBNER #%ld begin: connection=%u", observation, connection_id);
  }

  const int result = g_handle_bner_message_original != nullptr
      ? g_handle_bner_message_original(cnet_layer_internal, connection_id, shared_stream)
      : 0;
  if (observation <= 10 || result == 0) {
    LogFormat(L"HandleBNER #%ld result: connection=%u result=%d", observation, connection_id, result);
  }
  return result;
}

int CallLoggedStreamHandler(
    const wchar_t* name,
    volatile LONG* counter,
    HandleBnerMessageFn original,
    void* cnet_layer_internal,
    uint32_t connection_id,
    void* shared_stream,
    LONG log_limit) {
  const LONG observation = InterlockedIncrement(counter);
  if (observation <= log_limit) {
    LogFormat(L"%s #%ld begin: connection=%u", name, observation, connection_id);
  }

  const int result = original != nullptr ? original(cnet_layer_internal, connection_id, shared_stream) : 0;
  if (observation <= log_limit || result == 0) {
    LogFormat(L"%s #%ld result: connection=%u result=%d", name, observation, connection_id, result);
  }
  return result;
}

int __fastcall HookedHandleBncrMessage(void* cnet_layer_internal, uint32_t connection_id, void* shared_stream) {
  return CallLoggedStreamHandler(
      L"HandleBNCR",
      &g_bncr_handle_observations,
      g_handle_bncr_message_original,
      cnet_layer_internal,
      connection_id,
      shared_stream,
      30);
}

int __fastcall HookedHandleBnvrMessage(void* cnet_layer_internal, uint32_t connection_id, void* shared_stream) {
  const LONG observation = InterlockedIncrement(&g_bnvr_handle_observations);
  if (observation <= 20) {
    LogFormat(L"HandleBNVR #%ld begin: connection=%u", observation, connection_id);
  }

  const int result = g_handle_bnvr_message_original != nullptr
      ? g_handle_bnvr_message_original(cnet_layer_internal, connection_id, shared_stream)
      : 0;
  if (observation <= 20 || result == 0) {
    LogFormat(L"HandleBNVR #%ld result: connection=%u result=%d", observation, connection_id, result);
  }
  ReplayLegacyQueuedGamePackets(cnet_layer_internal, connection_id, result);
  return result;
}

int __fastcall HookedHandleBnvsMessage(void* cnet_layer_internal, uint32_t connection_id, void* shared_stream) {
  return CallLoggedStreamHandler(
      L"HandleBNVS",
      &g_bnvs_handle_observations,
      g_handle_bnvs_message_original,
      cnet_layer_internal,
      connection_id,
      shared_stream,
      20);
}

int __fastcall HookedHandleBnxrMessage(void* cnet_layer_internal, uint32_t connection_id, void* shared_stream) {
  return CallLoggedStreamHandler(
      L"HandleBNXR",
      &g_bnxr_handle_observations,
      g_handle_bnxr_message_original,
      cnet_layer_internal,
      connection_id,
      shared_stream,
      30);
}

int __fastcall HookedNonWindowMessages(
    void* cnet_layer_internal,
    uint32_t connection_id,
    unsigned char* message,
    uint32_t length) {
  const LONG observation = InterlockedIncrement(&g_non_window_message_observations);
  bool reject_legacy_packets_after_bncr = false;
  if (MessageHasPrefix(message, length, "BNCR")) {
    reject_legacy_packets_after_bncr = ObserveLegacyBncrChallenges(connection_id, message, length);
  }
  if (MessageHasPrefix(message, length, "BNVR")) {
    ObserveLegacyBnvrResponse(connection_id, message, length);
  }

  const bool is_known_control =
      MessageHasPrefix(message, length, "BNCR") ||
      MessageHasPrefix(message, length, "BNER") ||
      MessageHasPrefix(message, length, "BNXR") ||
      MessageHasPrefix(message, length, "BNVR") ||
      MessageHasPrefix(message, length, "BNVS") ||
      MessageHasPrefix(message, length, "BNDM") ||
      MessageHasPrefix(message, length, "BNDS");
  if (observation <= 80 || is_known_control || connection_id != 0) {
    LogFormat(
        L"NonWindow #%ld begin: connection=%u length=%u prefix=%s",
        observation,
        connection_id,
        length,
        FormatMessagePrefix(message, length).c_str());
  }

  const int result = g_non_window_messages_original != nullptr
      ? g_non_window_messages_original(cnet_layer_internal, connection_id, message, length)
      : 0;
  if (observation <= 80 || is_known_control || result == 0 || connection_id != 0) {
    LogFormat(
        L"NonWindow #%ld result: connection=%u length=%u prefix=%s result=%d",
        observation,
        connection_id,
        length,
        FormatMessagePrefix(message, length).c_str(),
        result);
  }
  if (reject_legacy_packets_after_bncr) {
    RejectLegacyGamePacketGate(connection_id, L"BNCR non-verifier response");
  }
  TryPendingDelayedHgPasswordRetry(-observation);
  return result;
}

int WSAAPI HookedSendTo(
    SOCKET socket,
    const char* buffer,
    int length,
    int flags,
    const sockaddr* address,
    int address_length) {
  const LONG observation = InterlockedIncrement(&g_sendto_observations);
  const bool is_game_packet = length > 0 &&
      MessageStartsWithByte(reinterpret_cast<const unsigned char*>(buffer), static_cast<uint32_t>(length), 'M');
  const bool is_ee_identity_packet = length >= 5 &&
      MessageStartsWithByte(reinterpret_cast<const unsigned char*>(buffer), static_cast<uint32_t>(length), 'I');
  const bool is_bncs = length >= 4 &&
      MessageHasPrefix(reinterpret_cast<const unsigned char*>(buffer), static_cast<uint32_t>(length), "BNCS");
  const bool targets_hg = SockaddrTargetsAutoConnect(address, address_length);
  const bool should_log_driver_ee_packet =
      DriverOnlyWinsockDiagnosticsEnabled() && targets_hg && is_ee_identity_packet;
  const LONG hg_m_observation = is_game_packet && targets_hg
      ? InterlockedIncrement(&g_sendto_hg_m_observations)
      : 0;
  const bool is_short_game_keepalive = is_game_packet && length == 12;
  const bool should_log_hg_m =
      (hg_m_observation > 0 && hg_m_observation <= 40) ||
      (is_game_packet && targets_hg && !is_short_game_keepalive);
  if (PacketDumpEnabled() && targets_hg && (is_bncs || is_game_packet || is_ee_identity_packet)) {
    const LONG dump_observation = InterlockedIncrement(&g_packet_dump_send_observations);
    if (dump_observation <= 160 || is_bncs || is_ee_identity_packet) {
      LogPacketDump(
          L"out",
          dump_observation,
          L"sendto",
          0,
          reinterpret_cast<const unsigned char*>(buffer),
          length > 0 ? static_cast<size_t>(length) : 0,
          FormatSockaddr(address, address_length));
    }
  }

  if (observation <= 40 || is_bncs || should_log_hg_m || should_log_driver_ee_packet) {
    LogFormat(
        L"sendto #%ld begin: socket=%llu length=%d flags=%d to=%s prefix=%s",
        observation,
        static_cast<unsigned long long>(socket),
        length,
        flags,
        FormatSockaddr(address, address_length).c_str(),
        FormatMessagePrefix(reinterpret_cast<const unsigned char*>(buffer), length > 0 ? static_cast<uint32_t>(length) : 0).c_str());
  }

  const int result = g_sendto_original != nullptr
      ? g_sendto_original(socket, buffer, length, flags, address, address_length)
      : SOCKET_ERROR;
  const int wsa_error = result == SOCKET_ERROR && g_wsa_get_last_error != nullptr ? g_wsa_get_last_error() : 0;
  const bool failed = result != length;
  if (observation <= 40 || is_bncs || failed || should_log_hg_m || should_log_driver_ee_packet) {
    LogFormat(
        L"sendto #%ld result: socket=%llu length=%d flags=%d to=%s prefix=%s result=%d wsa_error=%d",
        observation,
        static_cast<unsigned long long>(socket),
        length,
        flags,
        FormatSockaddr(address, address_length).c_str(),
        FormatMessagePrefix(reinterpret_cast<const unsigned char*>(buffer), length > 0 ? static_cast<uint32_t>(length) : 0).c_str(),
        result,
        wsa_error);
  }
  if (failed && is_game_packet && targets_hg) {
    const LONG failed_observation = InterlockedIncrement(&g_sendto_failed_m_observations);
    if (failed_observation <= 20) {
      LogFormat(
          L"sendto failed M #%ld: socket=%llu length=%d to=%s result=%d wsa_error=%d",
          failed_observation,
          static_cast<unsigned long long>(socket),
          length,
          FormatSockaddr(address, address_length).c_str(),
          result,
          wsa_error);
    }
  }
  return result;
}

int WSAAPI HookedRecvFrom(
    SOCKET socket,
    char* buffer,
    int length,
    int flags,
    sockaddr* from,
    int* from_length) {
  const int result = g_recvfrom_original != nullptr
      ? g_recvfrom_original(socket, buffer, length, flags, from, from_length)
      : SOCKET_ERROR;
  const int wsa_error = result == SOCKET_ERROR && g_wsa_get_last_error != nullptr ? g_wsa_get_last_error() : 0;
  const int observed_from_length = from_length != nullptr ? *from_length : 0;
  const bool from_hg = result > 0 && SockaddrTargetsAutoConnect(from, observed_from_length);
  const LONG observation = InterlockedIncrement(&g_recvfrom_observations);
  const LONG hg_observation = from_hg ? InterlockedIncrement(&g_recvfrom_hg_observations) : 0;
  const bool should_log_hg = hg_observation > 0 && hg_observation <= 80;

  if (result > 0) {
    const auto* message = reinterpret_cast<const unsigned char*>(buffer);
    const uint32_t message_length = static_cast<uint32_t>(result);
    const bool is_short_bn_control = result <= 32 &&
        MessageStartsWithByte(message, message_length, 'B') &&
        message_length >= 2 &&
        message[1] == 'N';
    const bool is_bn_packet = message_length >= 2 && message[0] == 'B' && message[1] == 'N';
    const bool is_game_packet = MessageStartsWithByte(message, message_length, 'M');
    const bool is_ee_identity_packet = MessageStartsWithByte(message, message_length, 'I') && message_length >= 5;
    const bool is_short_game_keepalive = is_game_packet && result == 12;
    const bool should_log_game_packet = from_hg && is_game_packet && !is_short_game_keepalive;
    const bool should_log_driver_ee_packet =
        DriverOnlyWinsockDiagnosticsEnabled() && from_hg && is_ee_identity_packet;
    if (PacketDumpEnabled() && from_hg && (is_short_bn_control || is_game_packet || is_ee_identity_packet || !is_bn_packet)) {
      const LONG dump_observation = InterlockedIncrement(&g_packet_dump_recv_observations);
      if (dump_observation <= 160 || is_game_packet || is_ee_identity_packet) {
        LogPacketDump(
            L"in",
            dump_observation,
            L"recvfrom",
            0,
            message,
            static_cast<size_t>(result),
            FormatSockaddr(from, observed_from_length));
      }
    }
    if (is_short_bn_control) {
      LogFormat(
          L"recvfrom #%ld: socket=%llu result=%d flags=%d from=%s prefix=%s bytes=[%s]",
          observation,
          static_cast<unsigned long long>(socket),
          result,
          flags,
          FormatSockaddr(from, observed_from_length).c_str(),
          FormatMessagePrefix(message, message_length).c_str(),
          FormatBytes(reinterpret_cast<const uint8_t*>(buffer), static_cast<size_t>(result)).c_str());
    } else if (observation <= 60 || should_log_hg || should_log_game_packet || should_log_driver_ee_packet) {
      LogFormat(
          L"recvfrom #%ld: socket=%llu result=%d flags=%d from=%s prefix=%s",
          observation,
          static_cast<unsigned long long>(socket),
          result,
          flags,
          FormatSockaddr(from, observed_from_length).c_str(),
          FormatMessagePrefix(message, message_length).c_str());
    }
  } else if (result == SOCKET_ERROR && observation <= 20 && wsa_error != WSAEWOULDBLOCK) {
    LogFormat(
        L"recvfrom #%ld: socket=%llu result=%d flags=%d from=%s wsa_error=%d",
        observation,
        static_cast<unsigned long long>(socket),
        result,
        flags,
        FormatSockaddr(from, observed_from_length).c_str(),
        wsa_error);
  }
  if (from_hg) {
    TryPendingDelayedHgPasswordRetry(-observation);
  }
  return result;
}

int __fastcall HookedSendBncrMessage(void* cnet_layer_internal, uint32_t connection_id, unsigned char response_type, unsigned int session_index) {
  const LONG observation = InterlockedIncrement(&g_bncr_send_observations);
  const int result = g_send_bncr_message_original != nullptr
      ? g_send_bncr_message_original(cnet_layer_internal, connection_id, response_type, session_index)
      : 0;
  if (observation <= 30 || result == 0) {
    const wchar_t printable_type = response_type >= 0x20 && response_type <= 0x7E ? static_cast<wchar_t>(response_type) : L'.';
    LogFormat(
        L"SendBNCR #%ld: connection=%u response_type='%c'/0x%02X session=%u result=%d",
        observation,
        connection_id,
        printable_type,
        static_cast<unsigned int>(response_type),
        session_index,
        result);
  }
  return result;
}

int __fastcall HookedSendBncsMessage(
    void* cnet_layer_internal,
    uint32_t connection_id,
    unsigned char connection_type,
    int auth_mode,
    const CExoStringView* player_name,
    const CExoStringView* public_cd_key,
    const CExoStringView* legacy_cd_key) {
  const LONG observation = InterlockedIncrement(&g_bncs_send_observations);
  if (observation <= 20) {
    const wchar_t printable_type = connection_type >= 0x20 && connection_type <= 0x7E ? static_cast<wchar_t>(connection_type) : L'.';
    LogFormat(
        L"SendBNCS #%ld begin: connection=%u type='%c'/0x%02X auth_mode=%d lengths=%u/%u/%u",
        observation,
        connection_id,
        printable_type,
        static_cast<unsigned int>(connection_type),
        auth_mode,
        SafeCExoStringLength(player_name),
        SafeCExoStringLength(public_cd_key),
        SafeCExoStringLength(legacy_cd_key));
  }

  const bool use_legacy_bncs = !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_BNCS_PACKET");
  const int result = use_legacy_bncs
      ? SendLegacyBncsMessage(
            cnet_layer_internal,
            connection_id,
            connection_type,
            auth_mode,
            player_name,
            public_cd_key,
            legacy_cd_key,
            observation)
      : (g_send_bncs_message_original != nullptr
            ? g_send_bncs_message_original(
                  cnet_layer_internal,
                  connection_id,
                  connection_type,
                  auth_mode,
                  player_name,
                  public_cd_key,
                  legacy_cd_key)
            : 0);
  if (observation <= 20 || result == 0) {
    const wchar_t printable_type = connection_type >= 0x20 && connection_type <= 0x7E ? static_cast<wchar_t>(connection_type) : L'.';
    LogFormat(
        L"SendBNCS #%ld result: connection=%u type='%c'/0x%02X auth_mode=%d mode=%s lengths=%u/%u/%u result=%d",
        observation,
        connection_id,
        printable_type,
        static_cast<unsigned int>(connection_type),
        auth_mode,
        use_legacy_bncs ? L"legacy-diamond" : L"ee-original",
        SafeCExoStringLength(player_name),
        SafeCExoStringLength(public_cd_key),
        SafeCExoStringLength(legacy_cd_key),
        result);
  }
  return result;
}

int __fastcall HookedSendBnvsMessage(
    void* cnet_layer_internal,
    const CExoStringView* cd_key_response,
    const CExoStringView* community_name_response,
    const CExoStringView* game_password_response) {
  const LONG observation = InterlockedIncrement(&g_bnvs_send_observations);
  uint32_t before_active = 0;
  uint32_t after_active = 0;
  uint32_t* const active_flag = cnet_layer_internal != nullptr
      ? reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(cnet_layer_internal) + kConnectToSessionActiveOffset)
      : nullptr;
  const bool read_before = SafeReadUInt32(active_flag, &before_active);
  bool state_repaired = false;

  if (g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_BNVS_STATE_PATCH") &&
      (!read_before || before_active == 0)) {
    state_repaired = SafeWriteUInt32(active_flag, 1);
  }

  if (observation <= 20 || state_repaired || (read_before && before_active == 0)) {
    LogFormat(
        L"SendBNVS #%ld begin: active=%s%u state_repaired=%d lengths=%u/%u/%u",
        observation,
        read_before ? L"" : L"?",
        before_active,
        state_repaired ? 1 : 0,
        SafeCExoStringLength(cd_key_response),
        SafeCExoStringLength(community_name_response),
        SafeCExoStringLength(game_password_response));
  }

  const bool use_legacy_bnvs = g_auto_connect_ip_known && !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_BNVS_PACKET");
  const int result = use_legacy_bnvs
      ? SendLegacyBnvsMessage(
            cnet_layer_internal,
            cd_key_response,
            community_name_response,
            game_password_response,
            observation)
      : (g_send_bnvs_message_original != nullptr
            ? g_send_bnvs_message_original(
                  cnet_layer_internal,
                  cd_key_response,
                  community_name_response,
                  game_password_response)
            : 0);

  const bool read_after = SafeReadUInt32(active_flag, &after_active);
  if (observation <= 20 || result == 0 || state_repaired) {
    LogFormat(
        L"SendBNVS #%ld result: active=%s%u mode=%s result=%d",
        observation,
        read_after ? L"" : L"?",
        after_active,
        use_legacy_bnvs ? L"legacy-diamond" : L"ee-original",
        result);
  }
  return result;
}

int __fastcall HookedStartConnectToSessionInternal(
    void* cnet_layer_internal,
    uint32_t session_index,
    const CExoStringView* player_name,
    int auth_mode,
    const CExoStringView* password,
    uint32_t timeout_seconds,
    uint32_t connection_type,
    const CExoStringView* cd_key_public,
    const CExoStringView* cd_key_legacy,
    const CExoStringView* client_extension) {
  const LONG observation = InterlockedIncrement(&g_start_connect_observations);
  if (g_diamond_public_cd_keys.empty()) {
    void** app_manager_global = ResolveMainExport<void**>("?g_pAppManager@@3PEAVCAppManager@@EA");
    void* const app_manager = app_manager_global != nullptr ? *app_manager_global : nullptr;
    ComputeDiamondPublicCdKeys(app_manager);
  }

  CExoStringView diamond_public_view{};
  CExoStringView diamond_legacy_view{};
  CExoStringView default_password_view{};
  const CExoStringView* effective_password = password;
  const CExoStringView* effective_cd_key_public = cd_key_public;
  const CExoStringView* effective_cd_key_legacy = cd_key_legacy;
  bool replaced_password = false;
  bool replaced_public = false;
  bool replaced_legacy = false;
  if (SafeCExoStringLength(effective_password) == 0 && !g_auto_connect_password.empty() && !g_auto_connect_host.empty()) {
    default_password_view.data = const_cast<char*>(g_auto_connect_password.data());
    default_password_view.length = static_cast<uint32_t>(g_auto_connect_password.size());
    effective_password = &default_password_view;
    replaced_password = true;
  }
  if (SafeCExoStringLength(effective_cd_key_public) == 0 && !g_diamond_public_cd_keys.empty()) {
    const std::string& value = g_diamond_public_cd_keys[0];
    diamond_public_view.data = const_cast<char*>(value.data());
    diamond_public_view.length = static_cast<uint32_t>(value.size());
    effective_cd_key_public = &diamond_public_view;
    replaced_public = true;
  }
  if (SafeCExoStringLength(effective_cd_key_legacy) == 0 && !g_diamond_public_cd_keys.empty()) {
    const size_t index = g_diamond_public_cd_keys.size() > 1 ? 1 : 0;
    const std::string& value = g_diamond_public_cd_keys[index];
    diamond_legacy_view.data = const_cast<char*>(value.data());
    diamond_legacy_view.length = static_cast<uint32_t>(value.size());
    effective_cd_key_legacy = &diamond_legacy_view;
    replaced_legacy = true;
  }

  LogFormat(
      L"StartConnectToSession #%ld: session=%u auth_mode=%d timeout=%u connection_type=%u lengths=%u/%u/%u/%u/%u fallback=%d/%d/%d",
      observation,
      session_index,
      auth_mode,
      timeout_seconds,
      connection_type,
      SafeCExoStringLength(player_name),
      SafeCExoStringLength(effective_password),
      SafeCExoStringLength(effective_cd_key_public),
      SafeCExoStringLength(effective_cd_key_legacy),
      SafeCExoStringLength(client_extension),
      replaced_password ? 1 : 0,
      replaced_public ? 1 : 0,
      replaced_legacy ? 1 : 0);

  const int result = g_start_connect_to_session_internal_original != nullptr
      ? g_start_connect_to_session_internal_original(
            cnet_layer_internal,
            session_index,
            player_name,
            auth_mode,
            effective_password,
            timeout_seconds,
            connection_type,
            effective_cd_key_public,
            effective_cd_key_legacy,
            client_extension)
      : 0;
  LogFormat(L"StartConnectToSession #%ld result=%d", observation, result);
  return result;
}

int __fastcall HookedStartConnectToSessionWrapper(
    void* cnet_layer,
    uint32_t session_index,
    const CExoStringView* player_name,
    int auth_mode,
    const CExoStringView* password,
    uint32_t timeout_seconds,
    uint32_t connection_type,
    const CExoStringView* cd_key_public,
    const CExoStringView* cd_key_legacy,
    const CExoStringView* client_extension) {
  const LONG observation = InterlockedIncrement(&g_start_connect_wrapper_observations);
  void* const internal = ReadPointerField(cnet_layer, 0x08);
  if (observation <= 20 || internal == nullptr) {
    LogFormat(
        L"StartConnectToSession wrapper #%ld: cnet_layer=%p internal=%p session=%u",
        observation,
        cnet_layer,
        internal,
        session_index);
  }

  if (internal == nullptr || g_start_connect_to_session_internal_original == nullptr) {
    LogFormat(L"StartConnectToSession wrapper #%ld failed: missing internal net layer/export", observation);
    return 0;
  }

  return HookedStartConnectToSessionInternal(
      internal,
      session_index,
      player_name,
      auth_mode,
      password,
      timeout_seconds,
      connection_type,
      cd_key_public,
      cd_key_legacy,
      client_extension);
}

bool ReturnAddressMatchesMainRva(void* return_address, size_t rva) {
  if (return_address == nullptr) {
    return false;
  }

  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    return false;
  }

  return static_cast<uint8_t*>(return_address) == base + rva;
}

bool ReturnAddressInMainRvaRange(void* return_address, size_t begin_rva, size_t end_rva) {
  if (return_address == nullptr || begin_rva >= end_rva) {
    return false;
  }

  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    return false;
  }

  auto* const address = static_cast<uint8_t*>(return_address);
  return address >= base + begin_rva && address < base + end_rva;
}

std::wstring FormatMainExecutableAddress(void* address) {
  if (address == nullptr) {
    return L"<null>";
  }

  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    wchar_t text[64]{};
    swprintf_s(text, L"%p", address);
    return text;
  }

  auto* const pointer = static_cast<uint8_t*>(address);
  auto* const dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
  const size_t image_size =
      dos_header->e_magic == IMAGE_DOS_SIGNATURE
          ? reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos_header->e_lfanew)->OptionalHeader.SizeOfImage
          : 0;
  if (image_size > 0 && pointer >= base && pointer < base + image_size) {
    wchar_t text[96]{};
    swprintf_s(text, L"%p/rva=0x%zX", address, static_cast<size_t>(pointer - base));
    return text;
  }

  wchar_t text[64]{};
  swprintf_s(text, L"%p", address);
  return text;
}

std::wstring FormatCurrentStackTrace(unsigned long frames_to_skip, unsigned long max_frames) {
  void* frames[16]{};
  if (max_frames > 16) {
    max_frames = 16;
  }

  const USHORT captured = RtlCaptureStackBackTrace(frames_to_skip + 1, max_frames, frames, nullptr);
  std::wstring text;
  for (USHORT i = 0; i < captured; ++i) {
    if (!text.empty()) {
      text += L" <- ";
    }
    text += FormatMainExecutableAddress(frames[i]);
  }
  return text;
}

const char* TryCallLookupMajorMinorName(unsigned char major, unsigned char minor) {
  if (g_lookup_major_minor_name == nullptr) {
    return nullptr;
  }

  const char* name = nullptr;
  __try {
    name = g_lookup_major_minor_name(major, minor);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    name = nullptr;
  }
  return name;
}

bool TryCopyNullTerminatedAscii(const char* source, char* destination, size_t destination_size, size_t* copied_length) {
  if (destination == nullptr || destination_size == 0 || copied_length == nullptr) {
    return false;
  }

  *copied_length = 0;
  if (source == nullptr) {
    destination[0] = '\0';
    return false;
  }

  __try {
    while (*copied_length + 1 < destination_size && source[*copied_length] != '\0') {
      destination[*copied_length] = source[*copied_length];
      ++(*copied_length);
    }
    destination[*copied_length] = '\0';
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    destination[0] = '\0';
    *copied_length = 0;
    return false;
  }
}

std::wstring LookupMajorMinorNameWide(unsigned char major, unsigned char minor) {
  if (g_lookup_major_minor_name == nullptr) {
    g_lookup_major_minor_name =
        ResolveMainExport<CnwMessageLookupMajorMinorNameFn>("?LookupMajorMinorName@CNWMessage@@SAPEBDEE@Z");
  }
  if (g_lookup_major_minor_name == nullptr) {
    return L"<lookup unavailable>";
  }

  const char* const name = TryCallLookupMajorMinorName(major, minor);
  if (name == nullptr) {
    return L"<unnamed>";
  }

  constexpr uint32_t kMaxNameLength = 96;
  char buffer[kMaxNameLength]{};
  size_t length = 0;
  if (!TryCopyNullTerminatedAscii(name, buffer, sizeof(buffer), &length)) {
    return L"<name read failed>";
  }
  if (length == 0) {
    return L"<unnamed>";
  }
  return NarrowToWide(std::string(buffer, buffer + length));
}

bool ShouldForceLegacyServerFeatureGate(void* return_address, int build, int feature, int revision) {
  (void)revision;
  if (!g_auto_connect_ip_known || IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_SERVER_FEATURE_GATE")) {
    return false;
  }
  if (build != 0x2001) {
    return false;
  }

  // ServerSatisfiesBuildBridge gates optional EE-era network fields. HG's 1.69
  // server never sends those fields in legacy packets, so those parsers must
  // stay on the 1.69 layout. Other callers, including discovery and login, keep
  // the native EE result.
  return g_client_module_load_depth > 0 ||
      (g_server_to_player_message_depth > 0 &&
       g_live_game_object_update_depth > 0 &&
       g_current_server_to_player_major == 5 &&
       g_current_server_to_player_minor == 1) ||
      (g_server_to_player_message_depth > 0 &&
       g_live_game_object_update_depth == 0 &&
       g_current_server_to_player_major == 30 &&
       (g_current_server_to_player_minor == 1 || g_current_server_to_player_minor == 2) &&
       feature == 0x24) ||
      ReturnAddressInMainRvaRange(return_address, kEeClientAreaLoadBeginRva, kEeClientAreaLoadEndRva);
}

bool ShouldForceLiveVisualTransformFeatureGate(void* return_address, int build, int feature, int revision) {
  if (!g_auto_connect_ip_known ||
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_LIVE_VISUAL_TRANSFORM_FEATURE_GATE") ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LIVE_VISUAL_TRANSFORM_FEATURE_GATE")) {
    return false;
  }
  if (build != 0x2001 || feature != 0x23 || revision != 0) {
    return false;
  }
  if (g_server_to_player_message_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1) {
    return false;
  }

  // Experimental only: forcing this EE-era visual-transform gate has helped
  // identify legacy packet boundaries, but some HG 1.69 live object packets use
  // data at this position for normal fields. Keep it opt-in until the packet
  // discriminator is understood.
  return ReturnAddressMatchesMainRva(return_address, kEeClientVisualTransformLerpFeatureReturnRva);
}

bool ShouldSkipLegacyLiveVisualTransformRead(void* return_address) {
  (void)return_address;
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_VISUAL_TRANSFORM_SKIP") &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1;
}

bool ShouldSkipLegacyLiveMaterialShaderParamCount(void* return_address) {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_MATERIAL_SHADER_PARAM_SKIP") &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1 &&
      ReturnAddressMatchesMainRva(
          return_address,
          kEeClientLiveGameObjectUpdateMaterialShaderParamCountReturnRva);
}

bool IsLegacyLiveExtendedArmorTableByteRead(void* return_address) {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_EXTENDED_ARMOR_TABLE_SKIP") &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1 &&
      ReturnAddressMatchesMainRva(
          return_address,
          kEeClientLiveItemAppearanceExtendedArmorByteReturnRva);
}

bool ShouldSkipLegacyLiveCreatureUpdateTailByteAtEnd(
    void* return_address,
    const CnwMessageReadState& state) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_CREATURE_TAIL_BYTE_SKIP") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      !state.readable ||
      CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr != state.read_buffer_size) {
    return false;
  }

  return ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailByte0ReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailByte0PairReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailByte1SingleReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailByte1ReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailByte2ReturnRva);
}

bool ShouldSuppressLegacyLiveCreatureUpdateTailReadAtEnd(
    void* return_address,
    const CnwMessageReadState& state) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_CREATURE_TAIL_EXACT_END_SUPPRESS") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      !state.readable ||
      CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr != state.read_buffer_size) {
    return false;
  }

  return ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailDwordReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailWordReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailBool1ReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailBool2ReturnRva) ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveCreatureUpdateTailBool3ReturnRva);
}

bool LooksLikeLegacyLiveObjectIdAt(const std::vector<uint8_t>& bytes, size_t offset) {
  // HG/1.69 object ids seen in live packets are little-endian values such as
  // 0x80016A31. The high byte is the strongest cheap discriminator here.
  return offset <= bytes.size() && bytes.size() - offset >= 4 && bytes[offset + 3] == 0x80;
}

bool IsLegacyLiveObjectOpcodeByte(uint8_t value) {
  return value == 'A' || value == 'D' || value == 'U' ||
      value == 'P' || value == 'I' || value == 'G' || value == 'W';
}

bool IsEeLiveGuiSubOpcodeByte(uint8_t value) {
  switch (value) {
    case 'A':
    case 'B':
    case 'C':
    case 'I':
    case 'M':
    case 'Q':
    case 'R':
    case 'S':
    case 'c':
    case 'i':
    case 'r':
      return true;
    default:
      return false;
  }
}

bool LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(const std::vector<uint8_t>& bytes, size_t offset) {
  if (offset > bytes.size() || bytes.size() - offset < 2) {
    return false;
  }

  const uint8_t opcode = bytes[offset];
  const uint8_t marker = bytes[offset + 1];
  const bool typed_object_boundary =
      (marker == 0x05 || marker == 0x07 || marker == 0x09 || marker == 0x0A) &&
      LooksLikeLegacyLiveObjectIdAt(bytes, offset + 2);
  const bool legacy_type5_sentinel_boundary =
      marker == 0x05 &&
      bytes.size() - offset >= 6 &&
      bytes[offset + 2] == 0xFD &&
      bytes[offset + 3] == 0xFF &&
      bytes[offset + 4] == 0xFF &&
      bytes[offset + 5] == 0xFF;
  if ((opcode == 'A' || opcode == 'D' || opcode == 'U' || opcode == 'P') &&
      (typed_object_boundary || legacy_type5_sentinel_boundary)) {
    return true;
  }
  const bool legacy_item_sentinel =
      marker == 0xFD &&
      bytes.size() - offset >= 5 &&
      bytes[offset + 2] == 0xFF &&
      bytes[offset + 3] == 0xFF &&
      bytes[offset + 4] == 0xFF;
  if (opcode == 'I' &&
      (marker == 0x05 || marker == 0xC5 || legacy_item_sentinel ||
       LooksLikeLegacyLiveObjectIdAt(bytes, offset + 1))) {
    return true;
  }
  const bool gui_object_boundary =
      opcode == 'G' &&
      IsEeLiveGuiSubOpcodeByte(marker) &&
      bytes.size() - offset >= 9 &&
      LooksLikeLegacyLiveObjectIdAt(bytes, offset + 5);
  if (gui_object_boundary) {
    return true;
  }
  const bool world_status_boundary =
      opcode == 'W' &&
      bytes.size() - offset >= 3 &&
      marker <= 0x0F &&
      bytes[offset + 2] == 0x0E;
  if (world_status_boundary) {
    return true;
  }
  return false;
}

bool LooksLikeLegacyLiveObjectSubMessageBoundary(
    void* message,
    const CnwMessageReadState& state,
    std::wstring* preview) {
  if (preview != nullptr) {
    preview->clear();
  }
  if (message == nullptr || !state.readable || state.read_buffer_ptr >= state.read_buffer_size) {
    return false;
  }

  const uint32_t remaining = state.read_buffer_size - state.read_buffer_ptr;
  const uint32_t length = std::min<uint32_t>(remaining, 8);
  std::vector<uint8_t> bytes;
  if (length < 2 || !PeekReadBufferBytes(message, state, state.read_buffer_ptr, length, &bytes) || bytes.size() < 2) {
    return false;
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), bytes.size());
  }

  return LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, 0);
}

bool LooksLikeInlineCExoStringAt(
    const std::vector<uint8_t>& bytes,
    size_t offset,
    uint32_t* string_length,
    size_t* string_end,
    std::string* preview) {
  if (string_length != nullptr) {
    *string_length = 0;
  }
  if (string_end != nullptr) {
    *string_end = offset;
  }
  if (preview != nullptr) {
    preview->clear();
  }
  if (offset > bytes.size() || bytes.size() - offset < 4) {
    return false;
  }

  uint32_t length = 0;
  if (!TryReadU32LeFromBytes(bytes, offset, &length)) {
    return false;
  }

  constexpr uint32_t kMaxLiveObjectNameBytes = 128;
  if (length > kMaxLiveObjectNameBytes || bytes.size() - offset - 4 < length) {
    return false;
  }

  const size_t text_start = offset + 4;
  const size_t end = text_start + length;
  if (length > 0) {
    uint32_t printable = 0;
    for (size_t index = text_start; index < end; ++index) {
      const uint8_t value = bytes[index];
      if ((value >= 0x20 && value <= 0x7E) || value == '\t') {
        ++printable;
      }
    }
    if (printable != length) {
      return false;
    }
    if (preview != nullptr) {
      preview->assign(
          reinterpret_cast<const char*>(bytes.data() + text_start),
          reinterpret_cast<const char*>(bytes.data() + end));
    }
  }

  if (string_length != nullptr) {
    *string_length = length;
  }
  if (string_end != nullptr) {
    *string_end = end;
  }
  return true;
}

bool FindLegacyLiveObjectSubMessageBoundaryInBytes(
    const std::vector<uint8_t>& bytes,
    size_t start_offset,
    size_t* boundary_offset) {
  if (boundary_offset != nullptr) {
    *boundary_offset = 0;
  }
  if (bytes.size() < 2 || start_offset >= bytes.size()) {
    return false;
  }

  constexpr size_t kMaxShortAddScanBytes = 96;
  const size_t end = std::min(bytes.size() - 1, start_offset + kMaxShortAddScanBytes);
  for (size_t offset = start_offset; offset < end; ++offset) {
    if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, offset)) {
      continue;
    }
    if (boundary_offset != nullptr) {
      *boundary_offset = offset;
    }
    return true;
  }
  return false;
}

bool TryFindLegacyLiveObjectSubMessageBoundaryBefore(
    void* message,
    const CnwMessageReadState& state,
    uint32_t max_rewind,
    uint32_t* target_buffer_ptr,
    uint32_t* rewind_bytes,
    std::wstring* preview) {
  if (target_buffer_ptr != nullptr) {
    *target_buffer_ptr = 0;
  }
  if (rewind_bytes != nullptr) {
    *rewind_bytes = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }
  if (message == nullptr ||
      !state.readable ||
      CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr == 0 ||
      state.read_buffer_ptr > state.read_buffer_size ||
      max_rewind == 0) {
    return false;
  }

  const uint32_t rewind_window = std::min<uint32_t>(state.read_buffer_ptr, max_rewind);
  const uint32_t scan_start = state.read_buffer_ptr - rewind_window;
  const uint32_t scan_length =
      std::min<uint32_t>(state.read_buffer_size - scan_start, rewind_window + 16);
  std::vector<uint8_t> bytes;
  if (scan_length < 2 ||
      !PeekReadBufferBytes(message, state, scan_start, scan_length, &bytes) ||
      bytes.size() < 2) {
    return false;
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64));
  }

  const size_t current_offset = state.read_buffer_ptr - scan_start;
  if (current_offset >= bytes.size()) {
    return false;
  }
  if (LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, current_offset)) {
    return false;
  }

  for (size_t offset = current_offset; offset > 0; --offset) {
    const size_t candidate = offset - 1;
    if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, candidate)) {
      continue;
    }
    const uint32_t target = scan_start + static_cast<uint32_t>(candidate);
    if (target >= state.read_buffer_ptr) {
      continue;
    }
    if (target_buffer_ptr != nullptr) {
      *target_buffer_ptr = target;
    }
    if (rewind_bytes != nullptr) {
      *rewind_bytes = state.read_buffer_ptr - target;
    }
    return true;
  }

  return false;
}

bool LooksLikeLegacyLiveShortAddLocStringRecordBytes(
    const std::vector<uint8_t>& bytes,
    uint8_t object_type,
    size_t name_offset,
    size_t* boundary_offset) {
  if (boundary_offset != nullptr) {
    *boundary_offset = 0;
  }
  if (name_offset > bytes.size() || bytes.size() - name_offset < 4) {
    return false;
  }

  uint32_t inline_string_length = 0;
  size_t inline_string_end = 0;
  std::string inline_string_preview;
  if (LooksLikeInlineCExoStringAt(
          bytes,
          name_offset,
          &inline_string_length,
          &inline_string_end,
          &inline_string_preview)) {
    return false;
  }

  size_t expected_boundary = 0;
  if (object_type == 10) {
    // Door add: TLK strref, then generic/faction WORD before the next live submessage.
    expected_boundary = name_offset + 4 + 2;
  } else if (object_type == 9) {
    // Placeable add: TLK strref, appearance BYTE, portrait WORD, bodybag WORD.
    expected_boundary = name_offset + 4 + 1 + 2 + 2;
  } else {
    return false;
  }

  if (expected_boundary > bytes.size() || bytes.size() - expected_boundary < 2 ||
      !LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, expected_boundary)) {
    return false;
  }

  if (boundary_offset != nullptr) {
    *boundary_offset = expected_boundary;
  }
  return true;
}

const wchar_t* LegacyLiveObjectTypeName(uint8_t object_type) {
  switch (object_type) {
    case 5:
      return L"creature";
    case 7:
      return L"trigger";
    case 9:
      return L"placeable";
    case 10:
      return L"door";
    default:
      return L"object";
  }
}

std::wstring FormatSetBitIndices(uint32_t value) {
  if (value == 0) {
    return L"<none>";
  }

  std::wstring result;
  for (uint32_t bit = 0; bit < 32; ++bit) {
    if ((value & (1u << bit)) == 0) {
      continue;
    }
    wchar_t text[16]{};
    swprintf_s(text, L"%s%u", result.empty() ? L"" : L",", bit);
    result += text;
  }
  return result;
}

std::wstring FormatLegacyCreatureUpdateMaskHints(uint32_t raw_mask) {
  wchar_t text[384]{};
  swprintf_s(
      text,
      L" mask-bits=[%s] ee-assoc-hints{commandable13=%d faction-party14=%d visibility15=%d associate-state17=%d}",
      FormatSetBitIndices(raw_mask).c_str(),
      (raw_mask & (1u << 13)) != 0 ? 1 : 0,
      (raw_mask & (1u << 14)) != 0 ? 1 : 0,
      (raw_mask & (1u << 15)) != 0 ? 1 : 0,
      (raw_mask & (1u << 17)) != 0 ? 1 : 0);
  return text;
}

bool ContainsAsciiNoCase(const std::string& text, const char* needle) {
  if (needle == nullptr || needle[0] == '\0') {
    return false;
  }

  const size_t needle_length = std::strlen(needle);
  if (needle_length == 0 || needle_length > text.size()) {
    return false;
  }

  for (size_t offset = 0; offset + needle_length <= text.size(); ++offset) {
    bool matches = true;
    for (size_t index = 0; index < needle_length; ++index) {
      unsigned char hay = static_cast<unsigned char>(text[offset + index]);
      unsigned char want = static_cast<unsigned char>(needle[index]);
      if (hay >= 'A' && hay <= 'Z') {
        hay = static_cast<unsigned char>(hay - 'A' + 'a');
      }
      if (want >= 'A' && want <= 'Z') {
        want = static_cast<unsigned char>(want - 'A' + 'a');
      }
      if (hay != want) {
        matches = false;
        break;
      }
    }
    if (matches) {
      return true;
    }
  }
  return false;
}

bool LegacyLiveObjectNameLooksTransitionRelated(const std::string& name) {
  return ContainsAsciiNoCase(name, "door") ||
      ContainsAsciiNoCase(name, "portal") ||
      ContainsAsciiNoCase(name, "transition") ||
      ContainsAsciiNoCase(name, "inn") ||
      ContainsAsciiNoCase(name, "tavern") ||
      ContainsAsciiNoCase(name, "crow") ||
      ContainsAsciiNoCase(name, "moon");
}

size_t FindNextLegacyLiveObjectSubMessageBoundaryAfter(
    const std::vector<uint8_t>& bytes,
    size_t offset) {
  if (offset >= bytes.size()) {
    return bytes.size();
  }

  const size_t start = std::min(bytes.size(), offset + 2);
  for (size_t candidate = start; candidate + 1 < bytes.size(); ++candidate) {
    if (LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, candidate)) {
      return candidate;
    }
  }
  return bytes.size();
}

bool IsPlausibleLegacyLiveAreaTriplet(float x, float y, float z) {
  if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
    return false;
  }
  if (x < -2048.0f || x > 2048.0f ||
      y < -2048.0f || y > 2048.0f ||
      z < -256.0f || z > 1024.0f) {
    return false;
  }
  if (IsNearOriginNoisePosition(x, y, z)) {
    return false;
  }
  return std::fabs(x) > 1.0f || std::fabs(y) > 1.0f;
}

bool TryReadLegacyLiveUpdateFixedPointPosition(
    const std::vector<uint8_t>& bytes,
    size_t offset,
    float* x,
    float* y,
    float* z) {
  if (x != nullptr) {
    *x = 0.0f;
  }
  if (y != nullptr) {
    *y = 0.0f;
  }
  if (z != nullptr) {
    *z = 0.0f;
  }
  if (x == nullptr || y == nullptr || z == nullptr ||
      offset > bytes.size() ||
      bytes.size() - offset < 14 ||
      bytes[offset] != 'U') {
    return false;
  }

  uint32_t update_marker = 0;
  if (!TryReadU32LeFromBytes(bytes, offset + 6, &update_marker) ||
      (update_marker != 0xFFFFFFF7 && update_marker != 0xFFFFFFF3)) {
    return false;
  }

  uint16_t x_fixed = 0;
  uint16_t y_fixed = 0;
  if (!TryReadU16LeFromBytes(bytes, offset + 10, &x_fixed) ||
      !TryReadU16LeFromBytes(bytes, offset + 12, &y_fixed)) {
    return false;
  }

  const float observed_x = static_cast<float>(x_fixed) / 100.0f;
  const float observed_y = static_cast<float>(y_fixed) / 100.0f;
  constexpr float kObservedZ = 0.0f;
  if (!IsPlausibleLegacyLiveAreaTriplet(observed_x, observed_y, kObservedZ)) {
    return false;
  }

  *x = observed_x;
  *y = observed_y;
  *z = kObservedZ;
  return true;
}

std::wstring FormatLegacyLiveRecordDiagnosticSuffix(
    const std::vector<uint8_t>& bytes,
    size_t offset,
    size_t record_end) {
  if (offset >= bytes.size()) {
    return L"";
  }

  const size_t safe_end = std::min(record_end, bytes.size());
  if (safe_end <= offset) {
    return L"";
  }

  const size_t record_length = safe_end - offset;
  std::wstring suffix;

  std::wstring triplets;
  size_t triplet_count = 0;
  for (size_t relative = 0; relative + 12 <= record_length && triplet_count < 6; ++relative) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if (!TryReadFloatLeFromBytes(bytes, offset + relative, &x) ||
        !TryReadFloatLeFromBytes(bytes, offset + relative + 4, &y) ||
        !TryReadFloatLeFromBytes(bytes, offset + relative + 8, &z) ||
        !IsPlausibleLegacyLiveAreaTriplet(x, y, z)) {
      continue;
    }

    wchar_t triplet[160]{};
    swprintf_s(
        triplet,
        L"%s+%zu=(%.3f,%.3f,%.3f)",
        triplets.empty() ? L"" : L",",
        relative,
        static_cast<double>(x),
        static_cast<double>(y),
        static_cast<double>(z));
    triplets += triplet;
    ++triplet_count;
  }

  wchar_t header[96]{};
  swprintf_s(header, L" raw-len=%zu", record_length);
  suffix += header;
  if (!triplets.empty()) {
    suffix += L" float-triplets=[";
    suffix += triplets;
    suffix += L"]";
  }
  suffix += L" raw=[";
  suffix += FormatLimitedBytes(bytes.data() + offset, record_length, 96);
  suffix += L"]";
  return suffix;
}

DWORD GetAutoUseObjectDelayMs(LONG stage) {
  uint32_t seconds = 2;
  const wchar_t* const env_name = stage <= 0
      ? L"HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS"
      : L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS";
  const std::wstring configured = GetEnvironmentStringWithAlias(env_name);
  if (!configured.empty() && (!ParseUnsigned32Flexible(configured, &seconds) || seconds > 120)) {
    LogFormat(
        L"auto-use object delay override ignored: invalid %s='%s'",
        env_name,
        configured.c_str());
    seconds = 2;
  }
  return seconds * 1000u;
}

DWORD GetAutoDoorTransitionClickDelayMs(LONG stage) {
  uint32_t milliseconds = 1500;
  const wchar_t* const env_name = stage <= 0
      ? L"HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS"
      : L"HG_BRIDGE_AUTO_DOOR_TRANSITION_SECOND_DELAY_MS";
  std::wstring configured = GetEnvironmentStringWithAlias(env_name);
  if (configured.empty() && stage > 0) {
    configured = GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS");
  }
  if (!configured.empty() &&
      (!ParseUnsigned32Flexible(configured, &milliseconds) || milliseconds > 30000)) {
    LogFormat(
        L"auto door transition delay override ignored: invalid %s='%s'",
        env_name,
        configured.c_str());
    milliseconds = 1500;
  }
  return milliseconds;
}

bool TryMatchConfiguredAutoUseObjectTarget(
    uint32_t object_id,
    uint8_t object_type,
    const std::string& object_name,
    const wchar_t* id_env_name,
    const wchar_t* type_env_name,
    const wchar_t* name_env_name,
    const wchar_t* reason_suffix,
    std::wstring* reason) {
  const std::wstring configured_type = GetEnvironmentStringWithAlias(type_env_name);
  bool type_filter_present = false;
  if (!configured_type.empty()) {
    type_filter_present = true;
    uint32_t wanted_type = 0;
    bool type_ok = false;
    const std::string type_text = ToUtf8(configured_type);
    if (ContainsAsciiNoCase(type_text, "creature")) {
      wanted_type = 5;
      type_ok = true;
    } else if (ContainsAsciiNoCase(type_text, "trigger")) {
      wanted_type = 7;
      type_ok = true;
    } else if (ContainsAsciiNoCase(type_text, "placeable")) {
      wanted_type = 9;
      type_ok = true;
    } else if (ContainsAsciiNoCase(type_text, "door")) {
      wanted_type = 10;
      type_ok = true;
    } else {
      type_ok = ParseUnsigned32Flexible(configured_type, &wanted_type);
    }
    if (!type_ok || wanted_type > 0xFFu) {
      if (reason != nullptr) {
        *reason = std::wstring(L"invalid ") + type_env_name;
      }
      return false;
    }
    if (object_type != static_cast<uint8_t>(wanted_type)) {
      return false;
    }
  }

  const std::wstring configured_id = GetEnvironmentStringWithAlias(id_env_name);
  const std::wstring configured_name = GetEnvironmentStringWithAlias(name_env_name);
  if (configured_id.empty() && configured_name.empty() && type_filter_present) {
    if (reason != nullptr) {
      *reason = std::wstring(L"type") + reason_suffix;
    }
    return true;
  }

  if (!configured_id.empty()) {
    uint32_t wanted_id = 0;
    if (!ParseUnsigned32Flexible(configured_id, &wanted_id)) {
      if (reason != nullptr) {
        *reason = std::wstring(L"invalid ") + id_env_name;
      }
      return false;
    }
    if (wanted_id == object_id) {
      if (reason != nullptr) {
        *reason = std::wstring(L"id") + reason_suffix;
      }
      return true;
    }
  }

  if (!configured_name.empty()) {
    const std::string wanted_name = ToUtf8(configured_name);
    if (!wanted_name.empty() && ContainsAsciiNoCase(object_name, wanted_name.c_str())) {
      if (reason != nullptr) {
        *reason = std::wstring(L"name") + reason_suffix;
      }
      return true;
    }
  }

  return false;
}

bool ShouldScheduleAutoUseObjectForTesting(
    uint32_t object_id,
    uint8_t object_type,
    const std::string& object_name,
    int effective_useable,
    std::wstring* reason,
    LONG* matched_stage) {
  if (reason != nullptr) {
    reason->clear();
  }
  if (matched_stage != nullptr) {
    *matched_stage = g_auto_use_object_stage;
  }
  if (!IsValidObjectIdForDiagnostics(object_id)) {
    return false;
  }
  if (effective_useable == 0 &&
      !IsTruthyEnvironmentFlagWithAlias(L"HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE")) {
    return false;
  }

  const LONG stage = g_auto_use_object_stage;
  if (matched_stage != nullptr) {
    *matched_stage = stage;
  }
  if (stage <= 0) {
    return TryMatchConfiguredAutoUseObjectTarget(
        object_id,
        object_type,
        object_name,
        L"HG_BRIDGE_AUTO_USE_OBJECT_ID",
        L"HG_BRIDGE_AUTO_USE_OBJECT_TYPE",
        L"HG_BRIDGE_AUTO_USE_OBJECT_NAME",
        L"",
        reason);
  }
  if (stage == 1) {
    return TryMatchConfiguredAutoUseObjectTarget(
        object_id,
        object_type,
        object_name,
        L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID",
        L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE",
        L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME",
        L"2",
        reason);
  }

  return false;
}

void ScheduleAutoUseObjectForTesting(
    uint32_t object_id,
    uint8_t object_type,
    const std::string& object_name,
    int effective_useable) {
  std::wstring match_reason;
  LONG matched_stage = g_auto_use_object_stage;
  if (!ShouldScheduleAutoUseObjectForTesting(
          object_id, object_type, object_name, effective_useable, &match_reason, &matched_stage)) {
    return;
  }
  if (InterlockedCompareExchange(&g_auto_use_object_state, 1, 0) != 0) {
    return;
  }

  g_pending_auto_use_object_stage = matched_stage;
  g_pending_auto_use_object_id = object_id;
  g_pending_auto_use_object_type = object_type;
  g_pending_auto_use_object_name = object_name.substr(0, 192);
  g_pending_auto_use_due_tick = GetTickCount64() + GetAutoUseObjectDelayMs(matched_stage);
  InterlockedExchange(&g_pending_auto_use_door_open_phase, 0);
  const LONG observation = InterlockedIncrement(&g_auto_use_object_observations);
  LogFormat(
      L"auto-use object scheduled #%ld: stage=%ld match=%s id=0x%08X type=%u/%s useable=%d due-ms=%llu name='%s'",
      observation,
      matched_stage,
      match_reason.empty() ? L"<unknown>" : match_reason.c_str(),
      object_id,
      static_cast<unsigned int>(object_type),
      ObjectRegistryTypeName(object_type),
      effective_useable,
      static_cast<unsigned long long>(g_pending_auto_use_due_tick - GetTickCount64()),
      NarrowToWide(g_pending_auto_use_object_name).c_str());
}

std::wstring FormatAutoUseObjectTargetConfig(LONG stage) {
  const wchar_t* const id_env = stage <= 0
      ? L"HG_BRIDGE_AUTO_USE_OBJECT_ID"
      : L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID";
  const wchar_t* const type_env = stage <= 0
      ? L"HG_BRIDGE_AUTO_USE_OBJECT_TYPE"
      : L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE";
  const wchar_t* const name_env = stage <= 0
      ? L"HG_BRIDGE_AUTO_USE_OBJECT_NAME"
      : L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME";
  wchar_t buffer[512]{};
  swprintf_s(
      buffer,
      L"id='%s' type='%s' name='%s'",
      GetEnvironmentStringWithAlias(id_env).c_str(),
      GetEnvironmentStringWithAlias(type_env).c_str(),
      GetEnvironmentStringWithAlias(name_env).c_str());
  return buffer;
}

bool AutoUseObjectTargetConfigured(LONG stage) {
  const wchar_t* const id_env = stage <= 0
      ? L"HG_BRIDGE_AUTO_USE_OBJECT_ID"
      : L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID";
  const wchar_t* const type_env = stage <= 0
      ? L"HG_BRIDGE_AUTO_USE_OBJECT_TYPE"
      : L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE";
  const wchar_t* const name_env = stage <= 0
      ? L"HG_BRIDGE_AUTO_USE_OBJECT_NAME"
      : L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME";
  return !GetEnvironmentStringWithAlias(id_env).empty() ||
      !GetEnvironmentStringWithAlias(type_env).empty() ||
      !GetEnvironmentStringWithAlias(name_env).empty();
}

std::wstring FormatAutoUseObjectCandidateList(size_t max_candidates) {
  std::wstring entries;
  size_t count = 0;
  AcquireSRWLockShared(&g_object_name_registry_lock);
  for (auto it = g_object_name_registry.rbegin(); it != g_object_name_registry.rend(); ++it) {
    if (count >= max_candidates) {
      break;
    }
    if (it->object_type != 7 && it->object_type != 9 && it->object_type != 10) {
      continue;
    }
    if (it->name.empty() &&
        it->object_type != 10 &&
        it->transition_target == 0x7F000000 &&
        it->useable < 0) {
      continue;
    }
    wchar_t entry[512]{};
    swprintf_s(
        entry,
        L"%s0x%08X %u/%s useable=%d trans=0x%08X age-ms=%lu name='%s'%s",
        entries.empty() ? L"" : L"; ",
        it->object_id,
        static_cast<unsigned int>(it->object_type),
        ObjectRegistryTypeName(it->object_type),
        it->useable,
        it->transition_target,
        GetTickCount() - it->tick,
        NarrowToWide(it->name).c_str(),
        FormatObjectRegistrySpatialSuffix(*it).c_str());
    entries += entry;
    ++count;
  }
  ReleaseSRWLockShared(&g_object_name_registry_lock);
  if (entries.empty()) {
    return L"<none>";
  }
  return entries;
}

bool TryGetTransitionClickPoint(const ObjectNameRegistryEntry& entry, float* x, float* y, float* z) {
  if (x == nullptr || y == nullptr || z == nullptr) {
    return false;
  }
  if (entry.has_position) {
    *x = entry.x;
    *y = entry.y;
    *z = entry.z;
    return true;
  }
  if (!entry.has_bounds ||
      !std::isfinite(entry.min_x) ||
      !std::isfinite(entry.min_y) ||
      !std::isfinite(entry.min_z) ||
      !std::isfinite(entry.max_x) ||
      !std::isfinite(entry.max_y) ||
      !std::isfinite(entry.max_z) ||
      entry.min_x > entry.max_x ||
      entry.min_y > entry.max_y ||
      entry.min_z > entry.max_z) {
    return false;
  }

  *x = (entry.min_x + entry.max_x) * 0.5f;
  *y = (entry.min_y + entry.max_y) * 0.5f;
  *z = (entry.min_z + entry.max_z) * 0.5f;
  return true;
}

bool ShouldDispatchAutoTransitionClick(const ObjectNameRegistryEntry& entry) {
  float click_x = 0.0f;
  float click_y = 0.0f;
  float click_z = 0.0f;
  if (!TryGetTransitionClickPoint(entry, &click_x, &click_y, &click_z)) {
    return false;
  }
  if (IsTruthyEnvironmentFlagWithAlias(L"HG_BRIDGE_AUTO_USE_TRANSITION_CLICK")) {
    return true;
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_DOOR_TRANSITION_CLICK")) {
    return false;
  }
  return entry.object_type == 10 || entry.object_type == 7;
}

bool ShouldOpenDoorBeforeAutoTransitionClick(const ObjectNameRegistryEntry& entry) {
  if (entry.object_type != 10) {
    return false;
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_DOOR_OPEN_FIRST")) {
    return false;
  }
  return IsTruthyEnvironmentFlagWithAlias(L"HG_BRIDGE_AUTO_DOOR_OPEN_FIRST");
}

bool TryBuildAutoDoorStatePayload(
    const ObjectNameRegistryEntry& entry,
    unsigned char* payload,
    size_t payload_size) {
  if (payload == nullptr ||
      payload_size < 14 ||
      !IsValidObjectIdForDiagnostics(entry.object_id) ||
      entry.object_type != 10) {
    return false;
  }

  std::fill(payload, payload + payload_size, static_cast<unsigned char>(0));
  payload[0] = 0x70;
  payload[1] = 0x1E;
  payload[2] = 0x02;
  WriteU32LeToRaw(payload, payload_size, 3, 0x0Du);
  WriteU32LeToRaw(payload, payload_size, 7, entry.object_id);
  payload[11] = 0x15;
  payload[12] = 0x00;
  payload[13] = 0x76;
  return true;
}

bool TryBuildAutoTransitionClickPayload(
    const ObjectNameRegistryEntry& entry,
    unsigned char* payload,
    size_t payload_size,
    uint32_t* area_target_id) {
  float click_x = 0.0f;
  float click_y = 0.0f;
  float click_z = 0.0f;
  if (payload == nullptr ||
      payload_size < 30 ||
      !IsValidObjectIdForDiagnostics(entry.object_id) ||
      !TryGetTransitionClickPoint(entry, &click_x, &click_y, &click_z)) {
    return false;
  }

  uint32_t target_area_id = g_current_area_object_id;
  const std::wstring configured_area_id =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_AREA_ID");
  if (!configured_area_id.empty()) {
    uint32_t parsed_area_id = 0;
    if (!ParseUnsigned32Flexible(configured_area_id, &parsed_area_id) ||
        !IsValidObjectIdForDiagnostics(parsed_area_id)) {
      LogFormat(
          L"auto-transition click ignored invalid HG_BRIDGE_AUTO_USE_AREA_ID='%s'",
          configured_area_id.c_str());
      return false;
    }
    target_area_id = parsed_area_id;
  }
  if (!IsValidObjectIdForDiagnostics(target_area_id)) {
    LogFormat(
        L"auto-transition click unavailable: current area target id is 0x%08X for object=0x%08X",
        target_area_id,
        entry.object_id);
    return false;
  }

  std::fill(payload, payload + payload_size, static_cast<unsigned char>(0));
  payload[0] = 0x70;
  payload[1] = 0x06;
  payload[2] = 0x03;
  WriteU32LeToRaw(payload, payload_size, 3, 0x1Du);
  WriteU32LeToRaw(payload, payload_size, 7, target_area_id);
  WriteFloatLeToRaw(payload, payload_size, 11, click_x);
  WriteFloatLeToRaw(payload, payload_size, 15, click_y);
  WriteFloatLeToRaw(payload, payload_size, 19, click_z == 0.0f ? 0.002f : click_z);
  payload[23] = 0x00;
  payload[24] = 0x00;
  WriteU32LeToRaw(payload, payload_size, 25, entry.object_id);
  payload[29] = 0xB6;
  if (area_target_id != nullptr) {
    *area_target_id = target_area_id;
  }
  return true;
}

bool TryBuildAutoTriggerWalkProbePayload(
    const ObjectNameRegistryEntry& entry,
    unsigned char* payload,
    size_t payload_size,
    uint32_t* area_target_id) {
  float click_x = 0.0f;
  float click_y = 0.0f;
  float click_z = 0.0f;
  if (payload == nullptr ||
      payload_size < 30 ||
      !IsValidObjectIdForDiagnostics(entry.object_id) ||
      entry.object_type != 7 ||
      !TryGetTransitionClickPoint(entry, &click_x, &click_y, &click_z)) {
    return false;
  }

  uint32_t target_area_id = g_current_area_object_id;
  const std::wstring configured_area_id =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_AREA_ID");
  if (!configured_area_id.empty()) {
    uint32_t parsed_area_id = 0;
    if (!ParseUnsigned32Flexible(configured_area_id, &parsed_area_id) ||
        !IsValidObjectIdForDiagnostics(parsed_area_id)) {
      LogFormat(
          L"auto-trigger walk probe ignored invalid HG_BRIDGE_AUTO_USE_AREA_ID='%s'",
          configured_area_id.c_str());
      return false;
    }
    target_area_id = parsed_area_id;
  }
  if (!IsValidObjectIdForDiagnostics(target_area_id)) {
    LogFormat(
        L"auto-trigger walk probe unavailable: current area target id is 0x%08X for object=0x%08X",
        target_area_id,
        entry.object_id);
    return false;
  }

  std::fill(payload, payload + payload_size, static_cast<unsigned char>(0));
  payload[0] = 0x70;
  payload[1] = 0x06;
  payload[2] = 0x01;
  WriteU32LeToRaw(payload, payload_size, 3, 0x1Du);
  WriteU32LeToRaw(payload, payload_size, 7, target_area_id);
  WriteFloatLeToRaw(payload, payload_size, 11, click_x);
  WriteFloatLeToRaw(payload, payload_size, 15, click_y);
  WriteFloatLeToRaw(payload, payload_size, 19, click_z == 0.0f ? 0.002f : click_z);
  payload[23] = 0x00;
  payload[24] = 0x00;
  WriteU32LeToRaw(payload, payload_size, 25, 0x7F000000u);
  payload[29] = 0xBE;
  if (area_target_id != nullptr) {
    *area_target_id = target_area_id;
  }
  return true;
}

bool TryPrepareAutoWalkToWaypoint(
    const ObjectNameRegistryEntry& entry,
    bool trigger_probe,
    uint32_t* area_target_id,
    float* click_x_out,
    float* click_y_out,
    float* click_z_out,
    uint32_t* action_object_id,
    unsigned char* action_byte) {
  float click_x = 0.0f;
  float click_y = 0.0f;
  float click_z = 0.0f;
  if (!IsValidObjectIdForDiagnostics(entry.object_id) ||
      !TryGetTransitionClickPoint(entry, &click_x, &click_y, &click_z)) {
    return false;
  }

  uint32_t target_area_id = g_current_area_object_id;
  const std::wstring configured_area_id =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_AREA_ID");
  if (!configured_area_id.empty()) {
    uint32_t parsed_area_id = 0;
    if (!ParseUnsigned32Flexible(configured_area_id, &parsed_area_id) ||
        !IsValidObjectIdForDiagnostics(parsed_area_id)) {
      LogFormat(
          L"auto-walk-to-waypoint ignored invalid HG_BRIDGE_AUTO_USE_AREA_ID='%s'",
          configured_area_id.c_str());
      return false;
    }
    target_area_id = parsed_area_id;
  }
  if (!IsValidObjectIdForDiagnostics(target_area_id)) {
    LogFormat(
        L"auto-walk-to-waypoint unavailable: current area target id is 0x%08X for object=0x%08X",
        target_area_id,
        entry.object_id);
    return false;
  }

  if (area_target_id != nullptr) {
    *area_target_id = target_area_id;
  }
  if (click_x_out != nullptr) {
    *click_x_out = click_x;
  }
  if (click_y_out != nullptr) {
    *click_y_out = click_y;
  }
  if (click_z_out != nullptr) {
    *click_z_out = click_z == 0.0f ? 0.002f : click_z;
  }
  if (action_object_id != nullptr) {
    *action_object_id = trigger_probe ? 0x7F000000u : entry.object_id;
  }
  if (action_byte != nullptr) {
    *action_byte = trigger_probe ? 0xBE : 0xB6;
  }
  return true;
}

bool TryFindNearbyTriggerTransitionForClick(
    float x,
    float y,
    float z,
    ObjectNameRegistryEntry* match,
    float* distance_squared) {
  if (match != nullptr) {
    *match = ObjectNameRegistryEntry{};
  }
  if (distance_squared != nullptr) {
    *distance_squared = 1.0e30f;
  }
  if (!IsPlausibleObjectPosition(x, y, z)) {
    return false;
  }

  constexpr float kRewriteRadius = 0.75f;
  constexpr DWORD kMaxTriggerAgeMs = 5 * 60 * 1000;
  const DWORD now = GetTickCount();
  ObjectNameRegistryEntry best{};
  float best_distance_squared = kRewriteRadius * kRewriteRadius;
  bool found = false;

  AcquireSRWLockShared(&g_object_name_registry_lock);
  for (const ObjectNameRegistryEntry& entry : g_object_name_registry) {
    if (entry.object_type != 7 ||
        !IsValidObjectIdForDiagnostics(entry.object_id) ||
        (!entry.has_position && !entry.has_bounds)) {
      continue;
    }
    if (entry.tick != 0 && now - entry.tick > kMaxTriggerAgeMs) {
      continue;
    }
    const float candidate_distance_squared = DistanceSquaredToObjectRegistryEntry(entry, x, y, z);
    if (candidate_distance_squared <= best_distance_squared) {
      best = entry;
      best_distance_squared = candidate_distance_squared;
      found = true;
    }
  }
  ReleaseSRWLockShared(&g_object_name_registry_lock);

  if (!found) {
    return false;
  }
  if (match != nullptr) {
    *match = best;
  }
  if (distance_squared != nullptr) {
    *distance_squared = best_distance_squared;
  }
  return true;
}

bool TryRewriteTriggerTransitionClickPayload(
    unsigned char major,
    unsigned char minor,
    unsigned char* payload,
    uint32_t payload_size) {
  if (major != 6 ||
      minor != 1 ||
      payload == nullptr ||
      payload_size < 30 ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_TRIGGER_TRANSITION_CLICK_REWRITE")) {
    return false;
  }

  const uint32_t data_offset = payload[0] == 0x70 && payload_size >= 11 ? 7 : 0;
  const uint32_t action_object_offset = data_offset + 18;
  if (payload_size < action_object_offset + 5) {
    return false;
  }

  uint32_t target_id = 0;
  uint32_t action_object_id = 0;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  if (!TryReadU32LeFromRaw(payload, payload_size, data_offset, &target_id) ||
      !TryReadFloatLeFromRaw(payload, payload_size, data_offset + 4, &x) ||
      !TryReadFloatLeFromRaw(payload, payload_size, data_offset + 8, &y) ||
      !TryReadFloatLeFromRaw(payload, payload_size, data_offset + 12, &z) ||
      !TryReadU32LeFromRaw(payload, payload_size, action_object_offset, &action_object_id) ||
      IsValidObjectIdForDiagnostics(action_object_id)) {
    return false;
  }

  ObjectNameRegistryEntry trigger{};
  float distance_squared = 0.0f;
  if (!TryFindNearbyTriggerTransitionForClick(x, y, z, &trigger, &distance_squared)) {
    return false;
  }

  payload[data_offset + 16] = 0x00;
  payload[data_offset + 17] = 0x00;
  WriteU32LeToRaw(payload, payload_size, action_object_offset, trigger.object_id);
  payload[action_object_offset + 4] = 0xB6;

  const LONG observation = InterlockedIncrement(&g_trigger_transition_click_rewrite_observations);
  if (observation <= 200) {
    LogFormat(
        L"trigger-transition click rewrite #%ld: target=0x%08X click=(%.3f, %.3f, %.3f) old-action=0x%08X new-action=0x%08X d=%.3f trigger-name='%s'%s payload=[%s]",
        observation,
        target_id,
        static_cast<double>(x),
        static_cast<double>(y),
        static_cast<double>(z),
        action_object_id,
        trigger.object_id,
        static_cast<double>(std::sqrt(distance_squared)),
        NarrowToWide(trigger.name).c_str(),
        FormatObjectRegistrySpatialSuffix(trigger).c_str(),
        FormatBytes(payload, payload_size).c_str());
  }
  return true;
}

void TryScheduleAutoUseObjectFromRegistry(const wchar_t* reason) {
  if (g_auto_use_object_state != 0) {
    return;
  }

  ObjectNameRegistryEntry match{};
  bool found = false;
  AcquireSRWLockShared(&g_object_name_registry_lock);
  for (auto it = g_object_name_registry.rbegin(); it != g_object_name_registry.rend(); ++it) {
    LONG matched_stage = g_auto_use_object_stage;
    std::wstring match_reason;
    if (ShouldScheduleAutoUseObjectForTesting(
            it->object_id, it->object_type, it->name, it->useable, &match_reason, &matched_stage)) {
      match = *it;
      found = true;
      break;
    }
  }
  ReleaseSRWLockShared(&g_object_name_registry_lock);

  if (!found) {
    const LONG stage = g_auto_use_object_stage;
    if (AutoUseObjectTargetConfigured(stage)) {
      const LONG miss = InterlockedIncrement(&g_auto_use_object_replay_miss_observations);
      if (miss <= 16 || reason != nullptr) {
        LogFormat(
            L"auto-use object registry replay miss #%ld: reason=%s stage=%ld target={%s} candidates=[%s]",
            miss,
            reason != nullptr ? reason : L"<unknown>",
            stage,
            FormatAutoUseObjectTargetConfig(stage).c_str(),
            FormatAutoUseObjectCandidateList(16).c_str());
      }
    }
    return;
  }

  LogFormat(
      L"auto-use object registry replay: reason=%s stage=%ld id=0x%08X type=%u/%s useable=%d name='%s'",
      reason != nullptr ? reason : L"<unknown>",
      g_auto_use_object_stage,
      match.object_id,
      static_cast<unsigned int>(match.object_type),
      ObjectRegistryTypeName(match.object_type),
      match.useable,
      NarrowToWide(match.name).c_str());
  ScheduleAutoUseObjectForTesting(match.object_id, match.object_type, match.name, match.useable);
}

void TryDispatchPendingAutoUseObject(void* message, LONG server_dispatch) {
  if (message == nullptr || g_auto_use_object_state != 1 || g_pending_auto_use_object_id == 0) {
    return;
  }
  const ULONGLONG now = GetTickCount64();
  if (g_pending_auto_use_due_tick != 0 && now < g_pending_auto_use_due_tick) {
    return;
  }
  if (InterlockedCompareExchange(&g_auto_use_object_state, 2, 1) != 1) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_auto_use_object_observations);
  ObjectNameRegistryEntry entry{};
  const bool entry_ok = TryGetObjectRegistryEntry(g_pending_auto_use_object_id, &entry);
  if (!entry_ok) {
    entry.object_id = g_pending_auto_use_object_id;
    entry.object_type = g_pending_auto_use_object_type;
    entry.name = g_pending_auto_use_object_name;
  }

  int64_t result = 0;
  bool sent_transition_click = false;
  bool sent_door_open = false;
  bool sent_trigger_walk_probe = false;
  auto send_input_use_object = [&](uint32_t object_id) -> int64_t {
    ClientInputUseObjectFn send_use_object =
        ResolveMainRva<ClientInputUseObjectFn>(kEeClientInputUseObjectRva);
    if (send_use_object == nullptr) {
      LogFormat(
          L"auto-use object skipped: EE Input_UseObject sender rva=0x%zX unavailable",
          kEeClientInputUseObjectRva);
      return 0;
    }
    return send_use_object(message, object_id, 0, 0);
  };
  auto send_input_change_door_state = [&](uint32_t object_id, uint16_t state) -> int64_t {
    ClientInputChangeDoorStateFn send_change_door_state =
        ResolveMainRva<ClientInputChangeDoorStateFn>(kEeClientInputChangeDoorStateRva);
    if (send_change_door_state == nullptr) {
      LogFormat(
          L"auto-door-open skipped: EE Input_ChangeDoorState sender rva=0x%zX unavailable",
          kEeClientInputChangeDoorStateRva);
      return 0;
    }
    return send_change_door_state(message, object_id, state);
  };
  auto send_input_walk_to_waypoint =
      [&](uint32_t area_id,
          float x,
          float y,
          float z,
          unsigned char action,
          uint32_t action_object_id) -> int64_t {
    ClientInputWalkToWaypointFn send_walk_to_waypoint =
        ResolveMainRva<ClientInputWalkToWaypointFn>(kEeClientInputWalkToWaypointRva);
    if (send_walk_to_waypoint == nullptr) {
      LogFormat(
          L"auto-walk-to-waypoint skipped: EE Input_WalkToWayPoint sender rva=0x%zX unavailable",
          kEeClientInputWalkToWaypointRva);
      return 0;
    }
    return send_walk_to_waypoint(message, area_id, x, y, z, 0, 0, 0, action, action_object_id);
  };
  uint32_t area_target_id = 0x7F000000;
  float transition_x = 0.0f;
  float transition_y = 0.0f;
  float transition_z = 0.0f;
  uint32_t transition_action_object_id = 0x7F000000;
  unsigned char transition_action_byte = 0;
  float trigger_walk_x = 0.0f;
  float trigger_walk_y = 0.0f;
  float trigger_walk_z = 0.0f;
  uint32_t trigger_walk_action_object_id = 0x7F000000;
  unsigned char trigger_walk_action_byte = 0;
  const bool trigger_walk_probe_available =
      IsTruthyEnvironmentFlagWithAlias(L"HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE") &&
      TryPrepareAutoWalkToWaypoint(
          entry,
          true,
          &area_target_id,
          &trigger_walk_x,
          &trigger_walk_y,
          &trigger_walk_z,
          &trigger_walk_action_object_id,
          &trigger_walk_action_byte);
  const bool transition_click_available =
      ShouldDispatchAutoTransitionClick(entry) &&
      TryPrepareAutoWalkToWaypoint(
          entry,
          false,
          &area_target_id,
          &transition_x,
          &transition_y,
          &transition_z,
          &transition_action_object_id,
          &transition_action_byte);
  if (transition_click_available &&
      ShouldOpenDoorBeforeAutoTransitionClick(entry) &&
      InterlockedCompareExchange(&g_pending_auto_use_door_open_phase, 1, 0) == 0) {
    sent_door_open = true;
    const DWORD door_transition_delay_ms =
        GetAutoDoorTransitionClickDelayMs(g_pending_auto_use_object_stage);
    LogFormat(
        L"auto-door-open dispatch #%ld begin: stage=%ld server_dispatch=%ld message=%p id=0x%08X state=0x0015 type=%u/%s name='%s'%s follow-up-ms=%lu sender-rva=0x%zX",
        observation,
        g_pending_auto_use_object_stage,
        server_dispatch,
        message,
        entry.object_id,
        static_cast<unsigned int>(entry.object_type),
        ObjectRegistryTypeName(entry.object_type),
        NarrowToWide(entry.name).c_str(),
        FormatObjectRegistrySpatialSuffix(entry).c_str(),
        door_transition_delay_ms,
        kEeClientInputChangeDoorStateRva);
    result = send_input_change_door_state(entry.object_id, 0x15);
    LogFormat(
        L"auto-door-open dispatch #%ld end: stage=%ld server_dispatch=%ld message=%p result=0x%llX/%lld id=0x%08X type=%u/%s name='%s'",
        observation,
        g_pending_auto_use_object_stage,
        server_dispatch,
        message,
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        g_pending_auto_use_object_id,
        static_cast<unsigned int>(g_pending_auto_use_object_type),
        ObjectRegistryTypeName(g_pending_auto_use_object_type),
        NarrowToWide(g_pending_auto_use_object_name).c_str());
    g_pending_auto_use_due_tick = GetTickCount64() + door_transition_delay_ms;
    InterlockedExchange(&g_auto_use_object_state, 1);
    return;
  }

  if (trigger_walk_probe_available) {
    sent_trigger_walk_probe = true;
    LogFormat(
        L"auto-trigger walk probe dispatch #%ld begin: stage=%ld server_dispatch=%ld message=%p area-target=0x%08X walk=(%.3f, %.3f, %.3f) action-object=0x%08X action-byte=0x%02X id=0x%08X type=%u/%s name='%s'%s sender-rva=0x%zX",
        observation,
        g_pending_auto_use_object_stage,
        server_dispatch,
        message,
        area_target_id,
        static_cast<double>(trigger_walk_x),
        static_cast<double>(trigger_walk_y),
        static_cast<double>(trigger_walk_z),
        trigger_walk_action_object_id,
        static_cast<unsigned int>(trigger_walk_action_byte),
        entry.object_id,
        static_cast<unsigned int>(entry.object_type),
        ObjectRegistryTypeName(entry.object_type),
        NarrowToWide(entry.name).c_str(),
        FormatObjectRegistrySpatialSuffix(entry).c_str(),
        kEeClientInputWalkToWaypointRva);
    result = send_input_walk_to_waypoint(
        area_target_id,
        trigger_walk_x,
        trigger_walk_y,
        trigger_walk_z,
        trigger_walk_action_byte,
        trigger_walk_action_object_id);
  } else if (transition_click_available) {
    sent_transition_click = true;
    LogFormat(
        L"auto-transition click dispatch #%ld begin: stage=%ld server_dispatch=%ld message=%p area-target=0x%08X walk=(%.3f, %.3f, %.3f) action-object=0x%08X action-byte=0x%02X id=0x%08X type=%u/%s name='%s'%s sender-rva=0x%zX",
        observation,
        g_pending_auto_use_object_stage,
        server_dispatch,
        message,
        area_target_id,
        static_cast<double>(transition_x),
        static_cast<double>(transition_y),
        static_cast<double>(transition_z),
        transition_action_object_id,
        static_cast<unsigned int>(transition_action_byte),
        entry.object_id,
        static_cast<unsigned int>(entry.object_type),
        ObjectRegistryTypeName(entry.object_type),
        NarrowToWide(entry.name).c_str(),
        FormatObjectRegistrySpatialSuffix(entry).c_str(),
        kEeClientInputWalkToWaypointRva);
    result = send_input_walk_to_waypoint(
        area_target_id,
        transition_x,
        transition_y,
        transition_z,
        transition_action_byte,
        transition_action_object_id);
  } else {
    LogFormat(
        L"auto-use object dispatch #%ld begin: stage=%ld server_dispatch=%ld message=%p id=0x%08X type=%u/%s name='%s'%s sender-rva=0x%zX flags=(0,0)",
        observation,
        g_pending_auto_use_object_stage,
        server_dispatch,
        message,
        g_pending_auto_use_object_id,
        static_cast<unsigned int>(g_pending_auto_use_object_type),
        ObjectRegistryTypeName(g_pending_auto_use_object_type),
        NarrowToWide(g_pending_auto_use_object_name).c_str(),
        entry_ok ? FormatObjectRegistrySpatialSuffix(entry).c_str() : L"",
        kEeClientInputUseObjectRva);
    result = send_input_use_object(g_pending_auto_use_object_id);
  }
  LogFormat(
      L"%s dispatch #%ld end: stage=%ld server_dispatch=%ld message=%p result=0x%llX/%lld id=0x%08X type=%u/%s name='%s'",
      sent_trigger_walk_probe ? L"auto-trigger walk probe" :
          (sent_transition_click ? L"auto-transition click" :
              (sent_door_open ? L"auto-door-open" : L"auto-use object")),
      observation,
      g_pending_auto_use_object_stage,
      server_dispatch,
      message,
      static_cast<unsigned long long>(result),
      static_cast<long long>(result),
      g_pending_auto_use_object_id,
      static_cast<unsigned int>(g_pending_auto_use_object_type),
      ObjectRegistryTypeName(g_pending_auto_use_object_type),
      NarrowToWide(g_pending_auto_use_object_name).c_str());
  InterlockedCompareExchange(
      &g_auto_use_object_stage,
      g_pending_auto_use_object_stage + 1,
      g_pending_auto_use_object_stage);
  InterlockedExchange(&g_auto_use_object_state, 0);
  TryScheduleAutoUseObjectFromRegistry(L"after auto-use dispatch");
}

bool TryFormatLegacyPartyPacketDiagnostic(
    const unsigned char* packet,
    uint32_t packet_size,
    std::wstring* diagnostic,
    bool* suspicious) {
  if (diagnostic != nullptr) {
    diagnostic->clear();
  }
  if (suspicious != nullptr) {
    *suspicious = false;
  }
  if (packet == nullptr || packet_size < 3 || packet[0] != 'P' || packet[1] != 14) {
    return false;
  }

  const unsigned char minor = packet[2];
  uint32_t internal_size = 0;
  const bool internal_size_known = TryReadU32LeFromRaw(packet, packet_size, 3, &internal_size);
  const uint32_t expected_internal_size = packet_size > 0 ? packet_size - 1 : 0;
  const bool internal_size_matches =
      internal_size_known && internal_size == expected_internal_size;

  uint32_t count = 0;
  uint32_t player_index = 0;
  uint32_t object_id = 0;
  bool count_known = false;
  bool player_index_known = false;
  bool object_id_known = false;
  std::wstring ids;

  // Server-to-player packets carry a 4-byte internal size at raw offset 3.
  // Handler data starts at raw offset 7; the final raw byte is the bit-fragment
  // tail owned by CNWMessage, not normal byte-aligned payload.
  const uint32_t data_begin = 7;
  const uint32_t data_end = packet_size > 0 ? packet_size - 1 : 0;
  const uint32_t data_available = data_end >= data_begin ? data_end - data_begin : 0;
  bool shape_ok = internal_size_matches;
  const bool list_like = minor == 1 || minor == 3 || minor == 4;
  if (list_like) {
    count_known = TryReadU32LeFromRaw(packet, packet_size, data_begin, &count);
    const uint64_t needed = 4ull + static_cast<uint64_t>(count) * 4ull;
    const uint32_t id_begin = data_begin + 4;
    const uint32_t id_bytes_available = data_end >= id_begin ? data_end - id_begin : 0;
    const uint32_t ids_available = id_bytes_available / 4;
    shape_ok = shape_ok &&
        count_known &&
        count <= 256 &&
        needed <= data_available &&
        (id_bytes_available % 4u) == 0;

    const uint32_t ids_to_log = std::min<uint32_t>(count, 8);
    for (uint32_t index = 0; index < ids_to_log; ++index) {
      uint32_t id = 0;
      if (!TryReadU32LeFromRaw(packet, packet_size, id_begin + index * 4, &id)) {
        break;
      }
      wchar_t id_text[64]{};
      swprintf_s(
          id_text,
          L"%s0x%08X",
          ids.empty() ? L"" : L",",
          id);
      ids += id_text;
    }
    if (count > ids_to_log) {
      ids += L",...";
    }

    if (diagnostic != nullptr) {
      wchar_t text[1024]{};
      swprintf_s(
          text,
          L"party-list-shape minor=%u internal-size-known=%d internal-size=%u expected=%u match=%d count-known=%d count=%u data-bytes=%u id-bytes=%u ids-available=%u ids=[%s] shape-ok=%d",
          static_cast<unsigned int>(minor),
          internal_size_known ? 1 : 0,
          internal_size,
          expected_internal_size,
          internal_size_matches ? 1 : 0,
          count_known ? 1 : 0,
          count,
          data_available,
          id_bytes_available,
          ids_available,
          ids.empty() ? L"<none>" : ids.c_str(),
          shape_ok ? 1 : 0);
      *diagnostic = text;
    }
  } else if (minor == 9) {
    object_id_known = TryReadU32LeFromRaw(packet, packet_size, data_begin, &object_id);
    shape_ok = shape_ok && object_id_known && data_available >= 4;
    if (diagnostic != nullptr) {
      wchar_t text[512]{};
      swprintf_s(
          text,
          L"party-invite-shape minor=%u internal-size-known=%d internal-size=%u expected=%u match=%d object-known=%d object=0x%08X data-bytes=%u shape-ok=%d",
          static_cast<unsigned int>(minor),
          internal_size_known ? 1 : 0,
          internal_size,
          expected_internal_size,
          internal_size_matches ? 1 : 0,
          object_id_known ? 1 : 0,
          object_id,
          data_available,
          shape_ok ? 1 : 0);
      *diagnostic = text;
    }
  } else if (minor == 14) {
    player_index_known = TryReadU32LeFromRaw(packet, packet_size, data_begin, &player_index);
    object_id_known = TryReadU32LeFromRaw(packet, packet_size, data_begin + 4, &object_id);
    shape_ok = shape_ok && player_index_known && object_id_known && data_available >= 8;
    if (diagnostic != nullptr) {
      wchar_t text[512]{};
      swprintf_s(
          text,
          L"party-transfer-shape minor=%u internal-size-known=%d internal-size=%u expected=%u match=%d player-known=%d player=%u object-known=%d object=0x%08X data-bytes=%u shape-ok=%d",
          static_cast<unsigned int>(minor),
          internal_size_known ? 1 : 0,
          internal_size,
          expected_internal_size,
          internal_size_matches ? 1 : 0,
          player_index_known ? 1 : 0,
          player_index,
          object_id_known ? 1 : 0,
          object_id,
          data_available,
          shape_ok ? 1 : 0);
      *diagnostic = text;
    }
  } else {
    if (diagnostic != nullptr) {
      wchar_t text[384]{};
      swprintf_s(
          text,
          L"party-shape minor=%u internal-size-known=%d internal-size=%u expected=%u match=%d data-bytes=%u",
          static_cast<unsigned int>(minor),
          internal_size_known ? 1 : 0,
          internal_size,
          expected_internal_size,
          internal_size_matches ? 1 : 0,
          data_available);
      *diagnostic = text;
    }
  }

  if (suspicious != nullptr) {
    *suspicious = !shape_ok;
  }
  return diagnostic == nullptr || !diagnostic->empty();
}

bool TryFormatLegacyLiveObjectPacketSummary(
    const unsigned char* packet,
    uint32_t packet_size,
    std::wstring* summary,
    bool* transition_related) {
  if (summary != nullptr) {
    summary->clear();
  }
  if (transition_related != nullptr) {
    *transition_related = false;
  }
  if (packet == nullptr || packet_size <= 7 || packet[0] != 'P' || packet[1] != 5 || packet[2] != 1) {
    return false;
  }

  std::vector<uint8_t> bytes(packet + 7, packet + packet_size);
  if (bytes.size() < 6) {
    return false;
  }

  uint32_t boundary_count = 0;
  uint32_t door_count = 0;
  uint32_t trigger_count = 0;
  uint32_t placeable_count = 0;
  uint32_t creature_count = 0;
  uint32_t item_count = 0;
  uint32_t named_count = 0;
  bool local_transition_related = false;
  std::wstring entries;
  uint32_t logged_entry_count = 0;
  constexpr uint32_t kMaxEntries = 32;
  const bool verbose_record_raw = IsTruthyEnvironmentFlag(L"HG_BRIDGE_VERBOSE_LIVE_OBJECT_RAW");

  for (size_t offset = 0; offset + 1 < bytes.size(); ++offset) {
    if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, offset)) {
      continue;
    }

    ++boundary_count;
    const uint8_t opcode = bytes[offset];
    const uint8_t object_type = bytes[offset + 1];
    if (opcode == 'I') {
      ++item_count;
      continue;
    }

    if (offset + 6 > bytes.size()) {
      continue;
    }

    uint32_t object_id = 0;
    TryReadU32LeFromBytes(bytes, offset + 2, &object_id);
    uint32_t first_dword = 0;
    const bool first_known =
        offset + 10 <= bytes.size() && TryReadU32LeFromBytes(bytes, offset + 6, &first_dword);
    float fixed_position_x = 0.0f;
    float fixed_position_y = 0.0f;
    float fixed_position_z = 0.0f;
    const bool fixed_position_ok =
        (object_type == 7 || object_type == 9 || object_type == 10) &&
        TryReadLegacyLiveUpdateFixedPointPosition(
            bytes,
            offset,
            &fixed_position_x,
            &fixed_position_y,
            &fixed_position_z);
    if (fixed_position_ok) {
      RememberObjectPositionForDiagnostics(
          object_id,
          object_type,
          fixed_position_x,
          fixed_position_y,
          fixed_position_z,
          L"legacy-live-update-fixed");
    }

    if (object_type == 10) {
      ++door_count;
      local_transition_related = true;
    } else if (object_type == 7) {
      ++trigger_count;
      local_transition_related = true;
    } else if (object_type == 9) {
      ++placeable_count;
    } else if (object_type == 5) {
      ++creature_count;
    }

    std::string inline_name;
    uint32_t inline_name_length = 0;
    size_t inline_name_end = 0;
    bool inline_name_known = false;
    bool short_locstring_known = false;
    uint32_t strref = 0;
    size_t name_offset = 0;
    if (object_type == 10 && first_known) {
      name_offset = first_dword == 0 ? 12 : 8;
    } else if (object_type == 9 || object_type == 5) {
      name_offset = 4;
    }

    const size_t object_start = offset + 2;
    const size_t name_start = name_offset != 0 ? object_start + name_offset : bytes.size();
    if (name_start < bytes.size()) {
      inline_name_known =
          LooksLikeInlineCExoStringAt(bytes, name_start, &inline_name_length, &inline_name_end, &inline_name);
      if (!inline_name_known &&
          (object_type == 9 || object_type == 10) &&
          LooksLikeLegacyLiveShortAddLocStringRecordBytes(bytes, object_type, name_start, nullptr)) {
        short_locstring_known = TryReadU32LeFromBytes(bytes, name_start, &strref);
      }
    }

    const bool name_transition_related =
        inline_name_known && LegacyLiveObjectNameLooksTransitionRelated(inline_name);
    if (inline_name_known) {
      ++named_count;
      if (name_transition_related) {
        local_transition_related = true;
      }
    } else if (short_locstring_known && (object_type == 9 || object_type == 10)) {
      local_transition_related = true;
    }

    if (((object_type == 5 && inline_name_known) ||
         ((object_type == 9 || object_type == 10) &&
          (inline_name_known || short_locstring_known)))) {
      std::string registry_name = inline_name_known ? inline_name : std::string{};
      if (!inline_name_known && short_locstring_known) {
        char strref_name[64]{};
        std::snprintf(strref_name, sizeof(strref_name), "strref:%u/0x%08X", strref, strref);
        registry_name = strref_name;
      }
      RememberObjectNameForDiagnostics(object_id, object_type, registry_name);
      ScheduleAutoUseObjectForTesting(object_id, object_type, registry_name, -1);
    }

    const bool raw_interesting =
        object_type == 5 ||
        object_type == 7 ||
        object_type == 10 ||
        name_transition_related ||
        (short_locstring_known && object_type == 10);
    const bool entry_interesting =
        raw_interesting ||
        inline_name_known ||
        short_locstring_known ||
        object_type == 5 ||
        object_type == 9;
    if (entry_interesting && logged_entry_count < kMaxEntries) {
      ++logged_entry_count;
      wchar_t entry[1024]{};
      const wchar_t printable_opcode = opcode >= 0x20 && opcode <= 0x7E ? static_cast<wchar_t>(opcode) : L'.';
      if (inline_name_known) {
        swprintf_s(
            entry,
            L"%s+%zu %c type=%u/%s id=0x%08X first=0x%08X name='%s'",
            entries.empty() ? L"" : L"; ",
            offset,
            printable_opcode,
            static_cast<unsigned int>(object_type),
            LegacyLiveObjectTypeName(object_type),
            object_id,
            first_dword,
            NarrowToWide(inline_name.substr(0, 80)).c_str());
      } else if (short_locstring_known) {
        swprintf_s(
            entry,
            L"%s+%zu %c type=%u/%s id=0x%08X first=0x%08X strref=0x%08X/%u",
            entries.empty() ? L"" : L"; ",
            offset,
            printable_opcode,
            static_cast<unsigned int>(object_type),
            LegacyLiveObjectTypeName(object_type),
            object_id,
            first_dword,
            strref,
            strref);
      } else {
        swprintf_s(
            entry,
            L"%s+%zu %c type=%u/%s id=0x%08X first=0x%08X",
            entries.empty() ? L"" : L"; ",
            offset,
            printable_opcode,
            static_cast<unsigned int>(object_type),
            LegacyLiveObjectTypeName(object_type),
            object_id,
            first_dword);
      }
      entries += entry;
      if (fixed_position_ok) {
        wchar_t position_text[128]{};
        swprintf_s(
            position_text,
            L" fixed-pos=(%.2f,%.2f,%.2f)",
            static_cast<double>(fixed_position_x),
            static_cast<double>(fixed_position_y),
            static_cast<double>(fixed_position_z));
        entries += position_text;
      }
      if (object_type == 5 && first_known) {
        entries += FormatLegacyCreatureUpdateMaskHints(first_dword);
      }
      if (raw_interesting || verbose_record_raw) {
        const size_t record_end = FindNextLegacyLiveObjectSubMessageBoundaryAfter(bytes, offset);
        entries += FormatLegacyLiveRecordDiagnosticSuffix(bytes, offset, record_end);
      }
    }
  }

  if (boundary_count == 0) {
    return false;
  }

  if (summary != nullptr) {
    wchar_t header[320]{};
    swprintf_s(
        header,
        L"boundaries=%u doors=%u triggers=%u placeables=%u creatures=%u items=%u named=%u transition-related=%d",
        boundary_count,
        door_count,
        trigger_count,
        placeable_count,
        creature_count,
        item_count,
        named_count,
        local_transition_related ? 1 : 0);
    *summary = header;
    if (!entries.empty()) {
      *summary += L" entries=[";
      *summary += entries;
      *summary += L"]";
    }
  }
  if (transition_related != nullptr) {
    *transition_related = local_transition_related;
  }
  return true;
}

bool LooksLikeLegacyLiveNameLocStringAtCurrentRead(
    const CnwMessageReadState& state,
    void* message,
    uint8_t object_type,
    size_t* boundary_delta,
    std::wstring* preview) {
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }
  if (message == nullptr || !state.readable || CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr >= state.read_buffer_size ||
      state.read_buffer_size - state.read_buffer_ptr < 6) {
    return false;
  }

  const uint32_t remaining = state.read_buffer_size - state.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 64);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, preview_length, &bytes) ||
      bytes.size() < 6) {
    return false;
  }

  size_t expected_boundary = 0;
  if (!LooksLikeLegacyLiveShortAddLocStringRecordBytes(bytes, object_type, 0, &expected_boundary)) {
    return false;
  }

  if (boundary_delta != nullptr) {
    *boundary_delta = expected_boundary;
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 48));
  }
  return true;
}

bool TryIdentifyLegacyLiveNameLocStringFlag(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const wchar_t** label,
    uint8_t* object_type,
    size_t* boundary_delta,
    std::wstring* preview) {
  if (label != nullptr) {
    *label = L"object";
  }
  if (object_type != nullptr) {
    *object_type = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_NAME_LOCSTRING_FLAG") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1) {
    return false;
  }

  uint8_t candidate_type = 0;
  const wchar_t* candidate_label = L"object";
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLiveDoorNameLocStringFlagReturnRva)) {
    candidate_type = 10;
    candidate_label = L"door";
  } else if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceableNameLocStringFlagReturnRva)) {
    candidate_type = 9;
    candidate_label = L"placeable";
  } else {
    return false;
  }

  if (!LooksLikeLegacyLiveNameLocStringAtCurrentRead(
          before,
          message,
          candidate_type,
          boundary_delta,
          preview)) {
    return false;
  }

  if (label != nullptr) {
    *label = candidate_label;
  }
  if (object_type != nullptr) {
    *object_type = candidate_type;
  }
  return true;
}

bool TryPlanLegacyLiveBadInlineLocStringSkip(
    void* message,
    const CnwMessageReadState& state,
    uint32_t* declared_length,
    uint32_t* available_length,
    uint32_t* boundary_delta,
    std::wstring* preview) {
  if (declared_length != nullptr) {
    *declared_length = 0;
  }
  if (available_length != nullptr) {
    *available_length = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_BAD_INLINE_LOCSTRING_SKIP") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      message == nullptr ||
      !state.readable ||
      CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr > state.read_buffer_size) {
    return false;
  }

  const uint32_t remaining = state.read_buffer_size - state.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 128);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, preview_length, &bytes)) {
    return false;
  }

  if (remaining < 4) {
    if (remaining == 0) {
      return false;
    }
    if (declared_length != nullptr) {
      *declared_length = 0;
    }
    if (available_length != nullptr) {
      *available_length = remaining;
    }
    if (boundary_delta != nullptr) {
      *boundary_delta = remaining;
    }
    if (preview != nullptr) {
      *preview = bytes.empty() ? L"" : FormatBytes(bytes.data(), bytes.size());
    }
    return true;
  }

  if (bytes.size() < 4) {
    return false;
  }

  uint32_t length = 0;
  if (!TryReadU32LeFromBytes(bytes, 0, &length)) {
    return false;
  }

  const uint32_t available = remaining - 4;
  if (LooksLikeInlineCExoStringAt(bytes, 0, nullptr, nullptr, nullptr)) {
    return false;
  }

  // A sane but non-printable string is still a possible string. The hangy
  // legacy case is the EE parser treating following item bytes as a huge
  // CExoString length, for example 0x0000380A inside a sub-1KB packet.
  constexpr uint32_t kMaxToleratedLiveLocStringBytes = 1024;
  if (length <= available && length <= kMaxToleratedLiveLocStringBytes) {
    return false;
  }

  size_t boundary_offset = 0;
  if (!FindLegacyLiveObjectSubMessageBoundaryInBytes(bytes, 4, &boundary_offset) ||
      boundary_offset < 4 ||
      boundary_offset > remaining) {
    return false;
  }

  if (declared_length != nullptr) {
    *declared_length = length;
  }
  if (available_length != nullptr) {
    *available_length = available;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = static_cast<uint32_t>(boundary_offset);
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64));
  }
  return true;
}

void ClearPendingLegacyLiveBadInlineLocStringSkip() {
  g_pending_legacy_live_inline_locstring_skip = PendingLegacyLiveInlineLocStringSkip{};
}

void ClearPendingLegacyLiveParserBoundaryGuard() {
  g_pending_legacy_live_parser_boundary_guard = PendingLegacyLiveParserBoundaryGuard{};
}

bool IsPendingLegacyLiveParserBoundaryGuardMatch(
    void* message,
    const CnwMessageReadState& state) {
  if (!g_pending_legacy_live_parser_boundary_guard.active) {
    return false;
  }

  const PendingLegacyLiveParserBoundaryGuard& guard =
      g_pending_legacy_live_parser_boundary_guard;
  if (guard.message != message ||
      guard.dispatch != g_current_server_to_player_message_observation) {
    ClearPendingLegacyLiveParserBoundaryGuard();
    return false;
  }

  if (!state.readable ||
      CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr != guard.boundary_buffer_ptr) {
    if (state.readable && state.read_buffer_ptr > guard.boundary_buffer_ptr) {
      ClearPendingLegacyLiveParserBoundaryGuard();
    }
    return false;
  }

  return true;
}

bool ArmPendingLegacyLiveParserBoundaryGuard(
    void* message,
    const CnwMessageReadState& source_state,
    uint32_t boundary_buffer_ptr,
    uint32_t declared_length,
    uint32_t available_length) {
  if (message == nullptr ||
      !source_state.readable ||
      boundary_buffer_ptr <= source_state.read_buffer_ptr ||
      boundary_buffer_ptr > source_state.read_buffer_size ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1) {
    ClearPendingLegacyLiveParserBoundaryGuard();
    return false;
  }

  std::vector<uint8_t> boundary_bytes;
  const uint32_t preview_length = std::min<uint32_t>(
      source_state.read_buffer_size - boundary_buffer_ptr,
      32);
  if (preview_length < 2 ||
      !PeekReadBufferBytes(message, source_state, boundary_buffer_ptr, preview_length, &boundary_bytes) ||
      boundary_bytes.size() < 2 ||
      !LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(boundary_bytes, 0)) {
    ClearPendingLegacyLiveParserBoundaryGuard();
    return false;
  }

  g_pending_legacy_live_parser_boundary_guard.active = true;
  g_pending_legacy_live_parser_boundary_guard.message = message;
  g_pending_legacy_live_parser_boundary_guard.dispatch =
      g_current_server_to_player_message_observation;
  g_pending_legacy_live_parser_boundary_guard.source_buffer_ptr =
      source_state.read_buffer_ptr;
  g_pending_legacy_live_parser_boundary_guard.boundary_buffer_ptr =
      boundary_buffer_ptr;
  g_pending_legacy_live_parser_boundary_guard.declared_length =
      declared_length;
  g_pending_legacy_live_parser_boundary_guard.available_length =
      available_length;
  g_pending_legacy_live_parser_boundary_guard.suppressed_reads = 0;

  const LONG observation =
      InterlockedIncrement(&g_legacy_live_parser_boundary_guard_observations);
  if (observation <= 240) {
    LogFormat(
        L"legacy live parser boundary guard armed #%ld: message=%p source=%u boundary=%u declared=%u available=%u source-state=[%s] boundary=[%s]%s",
        observation,
        message,
        source_state.read_buffer_ptr,
        boundary_buffer_ptr,
        declared_length,
        available_length,
        FormatCnwMessageReadState(source_state).c_str(),
        FormatBytes(boundary_bytes.data(), boundary_bytes.size()).c_str(),
        FormatLiveDispatchContext().c_str());
  }

  return true;
}

bool ShouldSuppressLegacyLiveParserBoundaryRead(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    const wchar_t* operation,
    std::wstring* detail) {
  if (detail != nullptr) {
    detail->clear();
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PARSER_BOUNDARY_GUARD") ||
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveGameObjectUpdateSubOpcodeReturnRva)) {
    return false;
  }

  if (ShouldSuppressLegacyLiveCreatureUpdateTailReadAtEnd(return_address, before)) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_creature_tail_exact_end_suppressions);
    if (observation <= 160) {
      LogFormat(
          L"legacy live creature tail exact-end suppression #%ld: caller=%s operation=%s message=%p state=[%s] next=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          operation != nullptr ? operation : L"<read>",
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageBufferPreview(message, before, 48).c_str(),
          FormatLiveDispatchContext().c_str());
    }
    if (detail != nullptr) {
      *detail = L"held-at-1.69-live-creature-tail exact-end";
    }
    return true;
  }

  if (!IsPendingLegacyLiveParserBoundaryGuardMatch(message, before)) {
    return false;
  }

  PendingLegacyLiveParserBoundaryGuard& guard =
      g_pending_legacy_live_parser_boundary_guard;
  if (++guard.suppressed_reads > 64) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_parser_boundary_guard_suppressions);
    LogFormat(
        L"legacy live parser boundary guard release #%ld: caller=%s operation=%s message=%p source=%u boundary=%u suppressions=%u state=[%s] release=too-many-suppressions%s",
        observation,
        FormatMainExecutableAddress(return_address).c_str(),
        operation != nullptr ? operation : L"<read>",
        message,
        guard.source_buffer_ptr,
        guard.boundary_buffer_ptr,
        guard.suppressed_reads,
        FormatCnwMessageReadState(before).c_str(),
        FormatLiveDispatchContext().c_str());
    ClearPendingLegacyLiveParserBoundaryGuard();
    return false;
  }

  const LONG observation =
      InterlockedIncrement(&g_legacy_live_parser_boundary_guard_suppressions);
  if (observation <= 320) {
    LogFormat(
        L"legacy live parser boundary guard suppression #%ld: caller=%s operation=%s message=%p source=%u boundary=%u suppressions=%u state=[%s] next=[%s]%s",
        observation,
        FormatMainExecutableAddress(return_address).c_str(),
        operation != nullptr ? operation : L"<read>",
        message,
        guard.source_buffer_ptr,
        guard.boundary_buffer_ptr,
        guard.suppressed_reads,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageBufferPreview(message, before, 48).c_str(),
        FormatLiveDispatchContext().c_str());
  }

  if (detail != nullptr) {
    wchar_t text[256]{};
    swprintf_s(
        text,
        L"held-at-1.69-live-boundary source=%u boundary=%u suppression=%u",
        guard.source_buffer_ptr,
        guard.boundary_buffer_ptr,
        guard.suppressed_reads);
    *detail = text;
  }
  return true;
}

void ReleaseLegacyLiveParserBoundaryGuardForSubOpcode(
    void* message,
    const CnwMessageReadState& before) {
  if (!IsPendingLegacyLiveParserBoundaryGuardMatch(message, before)) {
    return;
  }

  const PendingLegacyLiveParserBoundaryGuard guard =
      g_pending_legacy_live_parser_boundary_guard;
  const LONG observation =
      InterlockedIncrement(&g_legacy_live_parser_boundary_guard_suppressions);
  if (observation <= 320) {
    LogFormat(
        L"legacy live parser boundary guard release #%ld: message=%p source=%u boundary=%u suppressions=%u state=[%s] release=subopcode-loop%s",
        observation,
        message,
        guard.source_buffer_ptr,
        guard.boundary_buffer_ptr,
        guard.suppressed_reads,
        FormatCnwMessageReadState(before).c_str(),
        FormatLiveDispatchContext().c_str());
  }
  ClearPendingLegacyLiveParserBoundaryGuard();
}

bool QueuePendingLegacyLiveBadInlineLocStringSkip(
    void* message,
    const CnwMessageReadState& state,
    uint32_t declared_length,
    uint32_t available_length,
    uint32_t boundary_delta) {
  if (message == nullptr || !state.readable || boundary_delta == 0) {
    ClearPendingLegacyLiveBadInlineLocStringSkip();
    return false;
  }

  g_pending_legacy_live_inline_locstring_skip.active = true;
  g_pending_legacy_live_inline_locstring_skip.message = message;
  g_pending_legacy_live_inline_locstring_skip.dispatch = g_current_server_to_player_message_observation;
  g_pending_legacy_live_inline_locstring_skip.buffer_ptr = state.read_buffer_ptr;
  g_pending_legacy_live_inline_locstring_skip.fragments_ptr = state.read_fragments_ptr;
  g_pending_legacy_live_inline_locstring_skip.fragment_bit_ptr = state.read_fragments_bit_ptr;
  g_pending_legacy_live_inline_locstring_skip.fragment_bit_size = state.read_fragments_bit_size;
  g_pending_legacy_live_inline_locstring_skip.declared_length = declared_length;
  g_pending_legacy_live_inline_locstring_skip.available_length = available_length;
  g_pending_legacy_live_inline_locstring_skip.boundary_delta = boundary_delta;
  return true;
}

bool TryConsumePendingLegacyLiveBadInlineLocStringSkip(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    CExoStringView* result_string,
    CnwMessageReadState* after,
    uint32_t* declared_length,
    uint32_t* available_length,
    uint32_t* boundary_delta,
    uint32_t* rewind_bytes,
    bool* rewound_to_boundary,
    std::wstring* preview) {
  if (after != nullptr) {
    *after = before;
  }
  if (declared_length != nullptr) {
    *declared_length = 0;
  }
  if (available_length != nullptr) {
    *available_length = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (rewind_bytes != nullptr) {
    *rewind_bytes = 0;
  }
  if (rewound_to_boundary != nullptr) {
    *rewound_to_boundary = false;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  if (!g_pending_legacy_live_inline_locstring_skip.active ||
      !ReturnAddressMatchesMainRva(return_address, kEeClientReadCExoLocStringClientStringReturnRva)) {
    return false;
  }

  const PendingLegacyLiveInlineLocStringSkip pending = g_pending_legacy_live_inline_locstring_skip;
  if (pending.message != message ||
      pending.dispatch != g_current_server_to_player_message_observation ||
      pending.buffer_ptr != before.read_buffer_ptr ||
      pending.fragments_ptr != before.read_fragments_ptr ||
      pending.fragment_bit_ptr != before.read_fragments_bit_ptr ||
      pending.fragment_bit_size != before.read_fragments_bit_size) {
    ClearPendingLegacyLiveBadInlineLocStringSkip();
    return false;
  }

  uint32_t rewind_target = 0;
  uint32_t rewind_amount = 0;
  std::wstring rewind_preview;
  const bool should_try_rewind =
      g_current_server_to_player_live_opcode_known &&
      g_current_server_to_player_live_opcode != 'I' &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_BAD_INLINE_LOCSTRING_REWIND");
  if (should_try_rewind &&
      TryFindLegacyLiveObjectSubMessageBoundaryBefore(
          message,
          before,
          16,
          &rewind_target,
          &rewind_amount,
          &rewind_preview) &&
      WriteUInt32AtOffset(message, 0x44, rewind_target)) {
    SetEmptyCExoString(result_string);
    if (after != nullptr) {
      *after = ReadCnwMessageReadState(message);
    }
    if (declared_length != nullptr) {
      *declared_length = pending.declared_length;
    }
    if (available_length != nullptr) {
      *available_length = pending.available_length;
    }
    if (boundary_delta != nullptr) {
      *boundary_delta = pending.boundary_delta;
    }
    if (rewind_bytes != nullptr) {
      *rewind_bytes = rewind_amount;
    }
    if (rewound_to_boundary != nullptr) {
      *rewound_to_boundary = true;
    }
    if (preview != nullptr) {
      *preview = rewind_preview;
    }
    ClearPendingLegacyLiveBadInlineLocStringSkip();
    return true;
  }

  uint32_t planned_declared = 0;
  uint32_t planned_available = 0;
  uint32_t planned_boundary = 0;
  std::wstring planned_preview;
  if (!TryPlanLegacyLiveBadInlineLocStringSkip(
          message,
          before,
          &planned_declared,
          &planned_available,
          &planned_boundary,
          &planned_preview)) {
    ClearPendingLegacyLiveBadInlineLocStringSkip();
    return false;
  }

  const uint32_t target = before.read_buffer_ptr + planned_boundary;
  if (!WriteUInt32AtOffset(message, 0x44, target)) {
    ClearPendingLegacyLiveBadInlineLocStringSkip();
    return false;
  }
  ArmPendingLegacyLiveParserBoundaryGuard(
      message,
      before,
      target,
      planned_declared,
      planned_available);

  SetEmptyCExoString(result_string);
  if (after != nullptr) {
    *after = ReadCnwMessageReadState(message);
  }
  if (declared_length != nullptr) {
    *declared_length = planned_declared;
  }
  if (available_length != nullptr) {
    *available_length = planned_available;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = planned_boundary;
  }
  if (preview != nullptr) {
    *preview = planned_preview;
  }
  ClearPendingLegacyLiveBadInlineLocStringSkip();
  return true;
}

const wchar_t* LegacyLiveShortAddLabel(uint8_t object_type) {
  if (object_type == 10) {
    return L"door";
  }
  if (object_type == 9) {
    return L"placeable";
  }
  return L"object";
}

void ClearPendingLegacyLiveShortAddParse() {
  g_pending_legacy_live_short_add_parse = PendingLegacyLiveShortAddParse{};
}

void ClearPendingLegacyLivePlaceableAddDiagnostics() {
  g_pending_legacy_live_placeable_add_diagnostics =
      PendingLegacyLivePlaceableAddDiagnostics{};
}

const wchar_t* LegacyLivePlaceableAddBoolIndexName(size_t index) {
  switch (index) {
    case 0:
      return L"add-legacy-flag0";
    case 1:
      return L"add-legacy-optional-target";
    case 2:
      return L"add-legacy-flag1";
    case 3:
      return L"add-legacy-flag2-ee-useable";
    case 4:
      return L"add-legacy-flag3";
    case 5:
      return L"add-legacy-flag4";
    case 6:
      return L"add-legacy-flag5";
    case 7:
      return L"add-legacy-flag6";
    case 8:
      return L"add-legacy-flag7";
    case 9:
      return L"add-ee-only-final-visual";
    default:
      return L"unknown";
  }
}

bool TryGetLegacyLivePlaceableAddBoolIndex(const wchar_t* field, size_t* index) {
  if (index != nullptr) {
    *index = 0;
  }
  if (field == nullptr || field[0] == L'\0') {
    return false;
  }

  struct FieldMap {
    const wchar_t* field;
    size_t index;
  };
  static constexpr FieldMap kFields[] = {
      {L"add-legacy-flag0", 0},
      {L"add-legacy-optional-target", 1},
      {L"add-ee-optional-target", 1},
      {L"optional-target", 1},
      {L"add-legacy-flag1", 2},
      {L"add-legacy-flag2", 3},
      {L"add-legacy-flag3", 4},
      {L"add-legacy-flag4", 5},
      {L"add-ee-only-state0", 5},
      {L"ee-only-state0", 5},
      {L"add-legacy-flag5", 6},
      {L"add-ee-only-state1", 6},
      {L"ee-only-state1", 6},
      {L"add-legacy-flag6", 7},
      {L"add-ee-only-state2", 7},
      {L"ee-only-state2", 7},
      {L"add-legacy-flag7", 8},
      {L"add-ee-only-state3", 8},
      {L"ee-only-state3", 8},
      {L"add-ee-only-final-visual", 9},
      {L"add-ee-only-state4", 9},
      {L"ee-only-state4", 9},
  };

  for (const FieldMap& entry : kFields) {
    if (wcscmp(field, entry.field) == 0) {
      if (index != nullptr) {
        *index = entry.index;
      }
      return true;
    }
  }
  return false;
}

std::wstring FormatPendingLegacyLivePlaceableAddBoolMap(
    const PendingLegacyLivePlaceableAddDiagnostics& pending) {
  std::wstring text;
  for (size_t index = 0; index < pending.bool_values.size(); ++index) {
    if (!text.empty()) {
      text += L" ";
    }
    wchar_t item[96]{};
    if (pending.bool_seen[index]) {
      swprintf_s(
          item,
          L"%s=%d%s",
          LegacyLivePlaceableAddBoolIndexName(index),
          pending.bool_values[index],
          pending.bool_consumed[index] ? L"" : L"(synthetic)");
    } else {
      swprintf_s(item, L"%s=?", LegacyLivePlaceableAddBoolIndexName(index));
    }
    text += item;
  }
  return text;
}

bool TryQueueLegacyLivePlaceableAddDiagnostics(void* message) {
  ClearPendingLegacyLivePlaceableAddDiagnostics();
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_ADD_DIAGNOSTICS") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      message == nullptr) {
    return false;
  }

  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  if (!before.readable ||
      CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr >= before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 8) {
    return false;
  }

  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 192);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, preview_length, &bytes) ||
      bytes.size() < 8 ||
      !LooksLikeLegacyLiveObjectIdAt(bytes, 0)) {
    return false;
  }

  uint32_t object_id = 0;
  if (!TryReadU32LeFromBytes(bytes, 0, &object_id)) {
    return false;
  }

  size_t name_end = 0;
  uint32_t name_length = 0;
  std::string name_preview;
  if (!LooksLikeInlineCExoStringAt(bytes, 4, &name_length, &name_end, &name_preview)) {
    size_t locstring_boundary_offset = 0;
    if (LooksLikeLegacyLiveShortAddLocStringRecordBytes(
            bytes,
            9,
            4,
            &locstring_boundary_offset)) {
      uint32_t strref = 0;
      TryReadU32LeFromBytes(bytes, 4, &strref);
      char text[64]{};
      std::snprintf(text, sizeof(text), "<tlk:%u>", strref);
      name_preview = text;
      name_length = 0;
      name_end = 8;
    } else {
      name_end = 4;
    }
  }

  size_t boundary_offset = 0;
  const bool found_boundary =
      FindLegacyLiveObjectSubMessageBoundaryInBytes(
          bytes,
          std::min(bytes.size(), std::max<size_t>(8, name_end)),
          &boundary_offset) &&
      boundary_offset > 0;
  if (!found_boundary) {
    boundary_offset = std::min<size_t>(bytes.size(), remaining);
  }

  PendingLegacyLivePlaceableAddDiagnostics pending{};
  pending.active = true;
  pending.message = message;
  pending.dispatch = g_current_server_to_player_message_observation;
  pending.object_id = object_id;
  pending.record_start_buffer_ptr = before.read_buffer_ptr;
  pending.boundary_buffer_ptr =
      before.read_buffer_ptr + static_cast<uint32_t>(std::min<size_t>(boundary_offset, remaining));
  pending.tail_offset = static_cast<uint32_t>(name_end);
  pending.name_length = name_length;
  pending.name_preview = name_preview.substr(0, 96);
  pending.bool_values.fill(-1);
  pending.bool_seen.fill(false);
  pending.bool_consumed.fill(false);

  if (name_end < bytes.size()) {
    pending.appearance_type = bytes[name_end];
  }
  if (name_end + 3 <= bytes.size()) {
    uint16_t value = 0;
    if (TryReadU16LeFromBytes(bytes, name_end + 1, &value)) {
      pending.appearance_id = value;
    }
  }
  if (name_end + 5 <= bytes.size()) {
    uint16_t value = 0;
    if (TryReadU16LeFromBytes(bytes, name_end + 3, &value)) {
      pending.bodybag = value;
    }
  }

  g_pending_legacy_live_placeable_add_diagnostics = pending;

  const LONG observation =
      InterlockedIncrement(&g_legacy_live_placeable_add_diagnostics_observations);
  if (observation <= 320 || RuntimeResourceDiagnosticScopeActive()) {
    LogFormat(
        L"legacy live placeable add diagnostics begin #%ld: object=0x%08X/%u message=%p record-start=%u boundary=%u boundary-found=%d name-len=%u name='%s' tail-offset=%u raw-tail[appearance-type=0x%02X/%u appearance-id=0x%04X/%u bodybag=0x%04X/%u] before=[%s] raw=[%s]%s",
        observation,
        object_id,
        object_id,
        message,
        before.read_buffer_ptr,
        pending.boundary_buffer_ptr,
        found_boundary ? 1 : 0,
        name_length,
        NarrowToWide(pending.name_preview).c_str(),
        pending.tail_offset,
        static_cast<unsigned int>(pending.appearance_type),
        static_cast<unsigned int>(pending.appearance_type),
        static_cast<unsigned int>(pending.appearance_id),
        static_cast<unsigned int>(pending.appearance_id),
        static_cast<unsigned int>(pending.bodybag),
        static_cast<unsigned int>(pending.bodybag),
        FormatCnwMessageReadState(before).c_str(),
        FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 96)).c_str(),
        FormatLiveDispatchContext().c_str());
  }

  return true;
}

void RecordLegacyLivePlaceableAddBoolDiagnostic(
    void* message,
    const wchar_t* field,
    int value,
    bool consumed,
    const CnwMessageReadState& before,
    const CnwMessageReadState& after) {
  size_t index = 0;
  PendingLegacyLivePlaceableAddDiagnostics& pending =
      g_pending_legacy_live_placeable_add_diagnostics;
  if (!pending.active ||
      pending.message != message ||
      pending.dispatch != g_current_server_to_player_message_observation ||
      !TryGetLegacyLivePlaceableAddBoolIndex(field, &index) ||
      index >= pending.bool_values.size()) {
    return;
  }

  pending.bool_values[index] = value != 0 ? 1 : 0;
  pending.bool_seen[index] = true;
  pending.bool_consumed[index] = consumed;

  const LONG observation =
      InterlockedIncrement(&g_legacy_live_placeable_add_bool_map_observations);
  if (observation <= 720 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    LogFormat(
        L"legacy live placeable add bool map #%ld: object=0x%08X/%u field=%s index=%zu result=%d consumed=%d message=%p before=[%s] after=[%s] map=[%s]%s",
        observation,
        pending.object_id,
        pending.object_id,
        field != nullptr ? field : L"",
        index,
        value != 0 ? 1 : 0,
        consumed ? 1 : 0,
        message,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatPendingLegacyLivePlaceableAddBoolMap(pending).c_str(),
        FormatLiveDispatchContext().c_str());
  }
}

bool QueuePendingLegacyLiveShortAddParse(
    void* message,
    const CnwMessageReadState& state,
    uint8_t object_type,
    uint32_t boundary_delta,
    uint32_t strref) {
  if (message == nullptr ||
      !state.readable ||
      boundary_delta < 4 ||
      state.read_buffer_ptr > state.read_buffer_size ||
      boundary_delta > state.read_buffer_size - state.read_buffer_ptr) {
    ClearPendingLegacyLiveShortAddParse();
    return false;
  }

  g_pending_legacy_live_short_add_parse.active = true;
  g_pending_legacy_live_short_add_parse.message = message;
  g_pending_legacy_live_short_add_parse.dispatch = g_current_server_to_player_message_observation;
  g_pending_legacy_live_short_add_parse.object_type = object_type;
  g_pending_legacy_live_short_add_parse.name_buffer_ptr = state.read_buffer_ptr;
  g_pending_legacy_live_short_add_parse.boundary_buffer_ptr = state.read_buffer_ptr + boundary_delta;
  g_pending_legacy_live_short_add_parse.boundary_delta = boundary_delta;
  g_pending_legacy_live_short_add_parse.fragments_ptr = state.read_fragments_ptr;
  g_pending_legacy_live_short_add_parse.fragment_bit_ptr = state.read_fragments_bit_ptr;
  g_pending_legacy_live_short_add_parse.fragment_bit_size = state.read_fragments_bit_size;
  g_pending_legacy_live_short_add_parse.strref = strref;
  g_pending_legacy_live_short_add_parse.locstring_flag_forced = false;
  g_pending_legacy_live_short_add_parse.name_consumed = false;
  return true;
}

bool PendingLegacyLiveShortAddMatches(
    void* message,
    const CnwMessageReadState& state,
    bool require_name_cursor) {
  const PendingLegacyLiveShortAddParse& pending = g_pending_legacy_live_short_add_parse;
  if (!pending.active ||
      pending.message != message ||
      pending.dispatch != g_current_server_to_player_message_observation ||
      !state.readable ||
      CnwMessageStateOverflow(state)) {
    return false;
  }
  if (require_name_cursor) {
    return state.read_buffer_ptr == pending.name_buffer_ptr &&
        state.read_fragments_ptr == pending.fragments_ptr &&
        state.read_fragments_bit_ptr == pending.fragment_bit_ptr &&
        state.read_fragments_bit_size == pending.fragment_bit_size;
  }
  return state.read_buffer_ptr >= pending.name_buffer_ptr &&
      state.read_buffer_ptr <= pending.boundary_buffer_ptr;
}

bool IsLegacyLiveShortAddTailBoolReturnAddress(void* return_address, uint8_t object_type) {
  if (object_type == 10) {
    return ReturnAddressMatchesMainRva(return_address, kEeClientLiveDoorPostNameAbsentBoolReturnRva);
  }
  if (object_type == 9) {
    return ReturnAddressMatchesMainRva(
        return_address,
        kEeClientLivePlaceablePostNameEeOnlyFinalFlagReturnRva);
  }
  return false;
}

bool TryForceLegacyLiveShortAddLocStringInnerFlag(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const wchar_t** label,
    uint32_t* strref,
    uint32_t* boundary_delta) {
  if (!ReturnAddressMatchesMainRva(return_address, kEeClientReadCExoLocStringClientTlkFlagReturnRva) ||
      !PendingLegacyLiveShortAddMatches(message, before, true) ||
      g_pending_legacy_live_short_add_parse.name_consumed) {
    return false;
  }

  g_pending_legacy_live_short_add_parse.locstring_flag_forced = true;
  if (label != nullptr) {
    *label = LegacyLiveShortAddLabel(g_pending_legacy_live_short_add_parse.object_type);
  }
  if (strref != nullptr) {
    *strref = g_pending_legacy_live_short_add_parse.strref;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = g_pending_legacy_live_short_add_parse.boundary_delta;
  }
  return true;
}

bool TryForceLegacyLiveShortAddTailBool(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const wchar_t** label,
    uint32_t* boundary_remaining) {
  const PendingLegacyLiveShortAddParse& pending = g_pending_legacy_live_short_add_parse;
  if (!pending.active ||
      !pending.name_consumed ||
      !PendingLegacyLiveShortAddMatches(message, before, false) ||
      !IsLegacyLiveShortAddTailBoolReturnAddress(return_address, pending.object_type)) {
    return false;
  }

  if (label != nullptr) {
    *label = LegacyLiveShortAddLabel(pending.object_type);
  }
  if (boundary_remaining != nullptr) {
    *boundary_remaining = before.read_buffer_ptr <= pending.boundary_buffer_ptr
        ? pending.boundary_buffer_ptr - before.read_buffer_ptr
        : 0;
  }
  return true;
}

bool TryConsumePendingLegacyLiveShortAddNameString(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    CExoStringView* result_string,
    CnwMessageReadState* after,
    uint32_t* strref,
    uint32_t* consumed_bytes,
    uint32_t* boundary_delta,
    std::wstring* preview) {
  if (after != nullptr) {
    *after = before;
  }
  if (strref != nullptr) {
    *strref = 0;
  }
  if (consumed_bytes != nullptr) {
    *consumed_bytes = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  const PendingLegacyLiveShortAddParse& pending = g_pending_legacy_live_short_add_parse;
  if (!ReturnAddressMatchesMainRva(return_address, kEeClientReadCExoLocStringClientStringReturnRva) ||
      !pending.active ||
      !pending.locstring_flag_forced ||
      pending.name_consumed ||
      !PendingLegacyLiveShortAddMatches(message, before, true) ||
      before.read_buffer_size - before.read_buffer_ptr < 4 ||
      before.read_buffer_ptr + 4 > pending.boundary_buffer_ptr) {
    return false;
  }

  std::vector<uint8_t> bytes;
  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 64);
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, preview_length, &bytes) ||
      bytes.size() < 4) {
    return false;
  }

  uint32_t observed_strref = 0;
  if (!TryReadU32LeFromBytes(bytes, 0, &observed_strref)) {
    return false;
  }

  if (!WriteUInt32AtOffset(message, 0x44, before.read_buffer_ptr + 4)) {
    return false;
  }

  SetEmptyCExoString(result_string);
  g_pending_legacy_live_short_add_parse.name_consumed = true;
  if (after != nullptr) {
    *after = ReadCnwMessageReadState(message);
  }
  if (strref != nullptr) {
    *strref = observed_strref;
  }
  if (consumed_bytes != nullptr) {
    *consumed_bytes = 4;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = pending.boundary_delta;
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64));
  }
  return true;
}

void ClearPendingLegacyLiveTriggerAddParse() {
  g_pending_legacy_live_trigger_add_parse = PendingLegacyLiveTriggerAddParse{};
}

bool LooksLikeLegacyLiveTriggerAddRecordBytes(
    const std::vector<uint8_t>& bytes,
    size_t* boundary_offset,
    uint32_t* object_id,
    uint32_t* strref,
    uint8_t* legacy_primary_flag,
    float* height,
    uint8_t* vertex_count) {
  if (boundary_offset != nullptr) {
    *boundary_offset = 0;
  }
  if (object_id != nullptr) {
    *object_id = 0;
  }
  if (strref != nullptr) {
    *strref = 0;
  }
  if (legacy_primary_flag != nullptr) {
    *legacy_primary_flag = 0;
  }
  if (height != nullptr) {
    *height = 0.0f;
  }
  if (vertex_count != nullptr) {
    *vertex_count = 0;
  }

  constexpr size_t kObjectIdOffset = 0;
  constexpr size_t kNameOffset = 4;
  constexpr size_t kLegacyFlagOffset = kNameOffset + 4;
  constexpr size_t kHeightOffset = kLegacyFlagOffset + 1;
  constexpr size_t kVertexCountOffset = kHeightOffset + 4;
  constexpr size_t kVertexDataOffset = kVertexCountOffset + 1;
  constexpr size_t kVertexBytes = 12;

  if (bytes.size() < kVertexDataOffset ||
      !LooksLikeLegacyLiveObjectIdAt(bytes, kObjectIdOffset) ||
      LooksLikeInlineCExoStringAt(bytes, kNameOffset, nullptr, nullptr, nullptr)) {
    return false;
  }

  uint32_t observed_object_id = 0;
  uint32_t observed_strref = 0;
  float observed_height = 0.0f;
  if (!TryReadU32LeFromBytes(bytes, kObjectIdOffset, &observed_object_id) ||
      !TryReadU32LeFromBytes(bytes, kNameOffset, &observed_strref) ||
      !TryReadFloatLeFromRaw(
          bytes.data(),
          static_cast<uint32_t>(bytes.size()),
          static_cast<uint32_t>(kHeightOffset),
          &observed_height)) {
    return false;
  }

  const uint8_t observed_legacy_flag = bytes[kLegacyFlagOffset];
  const uint8_t observed_vertex_count = bytes[kVertexCountOffset];
  if (observed_legacy_flag > 1 ||
      observed_vertex_count == 0 ||
      observed_vertex_count > 64 ||
      !(observed_height >= -1000.0f && observed_height <= 1000.0f)) {
    return false;
  }

  const size_t expected_boundary =
      kVertexDataOffset + static_cast<size_t>(observed_vertex_count) * kVertexBytes;
  if (expected_boundary > bytes.size()) {
    return false;
  }
  if (expected_boundary < bytes.size() &&
      (bytes.size() - expected_boundary < 2 ||
       !LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, expected_boundary))) {
    return false;
  }

  for (size_t index = 0; index < observed_vertex_count; ++index) {
    const uint32_t vertex_offset =
        static_cast<uint32_t>(kVertexDataOffset + index * kVertexBytes);
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if (!TryReadFloatLeFromRaw(bytes.data(), static_cast<uint32_t>(bytes.size()), vertex_offset, &x) ||
        !TryReadFloatLeFromRaw(bytes.data(), static_cast<uint32_t>(bytes.size()), vertex_offset + 4, &y) ||
        !TryReadFloatLeFromRaw(bytes.data(), static_cast<uint32_t>(bytes.size()), vertex_offset + 8, &z) ||
        !(x >= -100000.0f && x <= 100000.0f) ||
        !(y >= -100000.0f && y <= 100000.0f) ||
        !(z >= -100000.0f && z <= 100000.0f)) {
      return false;
    }
  }

  if (boundary_offset != nullptr) {
    *boundary_offset = expected_boundary;
  }
  if (object_id != nullptr) {
    *object_id = observed_object_id;
  }
  if (strref != nullptr) {
    *strref = observed_strref;
  }
  if (legacy_primary_flag != nullptr) {
    *legacy_primary_flag = observed_legacy_flag;
  }
  if (height != nullptr) {
    *height = observed_height;
  }
  if (vertex_count != nullptr) {
    *vertex_count = observed_vertex_count;
  }
  return true;
}

bool TryReadLegacyLiveTriggerBounds(
    const std::vector<uint8_t>& bytes,
    uint8_t vertex_count,
    float* min_x,
    float* min_y,
    float* min_z,
    float* max_x,
    float* max_y,
    float* max_z) {
  if (vertex_count == 0 ||
      min_x == nullptr ||
      min_y == nullptr ||
      min_z == nullptr ||
      max_x == nullptr ||
      max_y == nullptr ||
      max_z == nullptr) {
    return false;
  }

  constexpr size_t kVertexDataOffset = 4 + 4 + 1 + 4 + 1;
  constexpr size_t kVertexBytes = 12;
  if (bytes.size() < kVertexDataOffset + static_cast<size_t>(vertex_count) * kVertexBytes) {
    return false;
  }

  float observed_min_x = 0.0f;
  float observed_min_y = 0.0f;
  float observed_min_z = 0.0f;
  float observed_max_x = 0.0f;
  float observed_max_y = 0.0f;
  float observed_max_z = 0.0f;
  for (uint8_t index = 0; index < vertex_count; ++index) {
    const uint32_t vertex_offset =
        static_cast<uint32_t>(kVertexDataOffset + static_cast<size_t>(index) * kVertexBytes);
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if (!TryReadFloatLeFromRaw(bytes.data(), static_cast<uint32_t>(bytes.size()), vertex_offset, &x) ||
        !TryReadFloatLeFromRaw(bytes.data(), static_cast<uint32_t>(bytes.size()), vertex_offset + 4, &y) ||
        !TryReadFloatLeFromRaw(bytes.data(), static_cast<uint32_t>(bytes.size()), vertex_offset + 8, &z) ||
        !IsPlausibleObjectPosition(x, y, z)) {
      return false;
    }
    if (index == 0) {
      observed_min_x = observed_max_x = x;
      observed_min_y = observed_max_y = y;
      observed_min_z = observed_max_z = z;
    } else {
      observed_min_x = std::min(observed_min_x, x);
      observed_min_y = std::min(observed_min_y, y);
      observed_min_z = std::min(observed_min_z, z);
      observed_max_x = std::max(observed_max_x, x);
      observed_max_y = std::max(observed_max_y, y);
      observed_max_z = std::max(observed_max_z, z);
    }
  }

  *min_x = observed_min_x;
  *min_y = observed_min_y;
  *min_z = observed_min_z;
  *max_x = observed_max_x;
  *max_y = observed_max_y;
  *max_z = observed_max_z;
  return true;
}

bool PendingLegacyLiveTriggerAddMatches(
    void* message,
    const CnwMessageReadState& state,
    bool require_name_cursor) {
  const PendingLegacyLiveTriggerAddParse& pending = g_pending_legacy_live_trigger_add_parse;
  if (!pending.active ||
      pending.message != message ||
      pending.dispatch != g_current_server_to_player_message_observation ||
      !state.readable ||
      CnwMessageStateOverflow(state)) {
    return false;
  }
  if (require_name_cursor) {
    return state.read_buffer_ptr == pending.name_buffer_ptr &&
        state.read_fragments_ptr == pending.fragments_ptr &&
        state.read_fragments_bit_ptr == pending.fragment_bit_ptr &&
        state.read_fragments_bit_size == pending.fragment_bit_size;
  }
  return state.read_buffer_ptr >= pending.name_buffer_ptr &&
      state.read_buffer_ptr <= pending.boundary_buffer_ptr;
}

bool TryQueueLegacyLiveTriggerAddParse(void* message) {
  ClearPendingLegacyLiveTriggerAddParse();
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_TRIGGER_ADD_PARSE") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      message == nullptr) {
    return false;
  }

  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  if (!before.readable || CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr >= before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 14) {
    return false;
  }

  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 1024);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, preview_length, &bytes)) {
    return false;
  }

  size_t boundary_offset = 0;
  uint32_t object_id = 0;
  uint32_t strref = 0;
  uint8_t legacy_primary_flag = 0;
  uint8_t vertex_count = 0;
  float height = 0.0f;
  if (!LooksLikeLegacyLiveTriggerAddRecordBytes(
          bytes,
          &boundary_offset,
          &object_id,
          &strref,
          &legacy_primary_flag,
          &height,
          &vertex_count) ||
      boundary_offset <= 4 ||
      boundary_offset > remaining) {
    return false;
  }

  g_pending_legacy_live_trigger_add_parse.active = true;
  g_pending_legacy_live_trigger_add_parse.message = message;
  g_pending_legacy_live_trigger_add_parse.dispatch =
      g_current_server_to_player_message_observation;
  g_pending_legacy_live_trigger_add_parse.object_id = object_id;
  g_pending_legacy_live_trigger_add_parse.name_buffer_ptr = before.read_buffer_ptr + 4;
  g_pending_legacy_live_trigger_add_parse.boundary_buffer_ptr =
      before.read_buffer_ptr + static_cast<uint32_t>(boundary_offset);
  g_pending_legacy_live_trigger_add_parse.boundary_delta =
      static_cast<uint32_t>(boundary_offset);
  g_pending_legacy_live_trigger_add_parse.fragments_ptr = before.read_fragments_ptr;
  g_pending_legacy_live_trigger_add_parse.fragment_bit_ptr = before.read_fragments_bit_ptr;
  g_pending_legacy_live_trigger_add_parse.fragment_bit_size = before.read_fragments_bit_size;
  g_pending_legacy_live_trigger_add_parse.strref = strref;
  g_pending_legacy_live_trigger_add_parse.legacy_primary_flag = legacy_primary_flag;
  g_pending_legacy_live_trigger_add_parse.vertex_count = vertex_count;
  g_pending_legacy_live_trigger_add_parse.height = height;

  float min_x = 0.0f;
  float min_y = 0.0f;
  float min_z = 0.0f;
  float max_x = 0.0f;
  float max_y = 0.0f;
  float max_z = 0.0f;
  const bool bounds_ok = TryReadLegacyLiveTriggerBounds(
      bytes,
      vertex_count,
      &min_x,
      &min_y,
      &min_z,
      &max_x,
      &max_y,
      &max_z);
  if (bounds_ok) {
    char trigger_name[96]{};
    std::snprintf(trigger_name, sizeof(trigger_name), "trigger-strref:%u/0x%08X", strref, strref);
    RememberObjectNameForDiagnostics(
        object_id,
        7,
        trigger_name,
        -1,
        0x7F000000,
        false,
        0.0f,
        0.0f,
        0.0f,
        true,
        min_x,
        min_y,
        min_z,
        max_x,
        max_y,
        max_z);
    ScheduleAutoUseObjectForTesting(object_id, 7, trigger_name, -1);
  }

  const LONG observation = InterlockedIncrement(&g_legacy_live_trigger_add_parse_observations);
  if (observation <= 240) {
    const bool has_next_boundary =
        boundary_offset < bytes.size() &&
        LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, boundary_offset);
    const wchar_t boundary_opcode =
        has_next_boundary && bytes[boundary_offset] >= 0x20 && bytes[boundary_offset] <= 0x7E
            ? static_cast<wchar_t>(bytes[boundary_offset])
            : L'.';
    const unsigned int boundary_byte = has_next_boundary ? bytes[boundary_offset] : 0;
    LogFormat(
        L"legacy live trigger add parse queued #%ld: object=0x%08X/%u strref=0x%08X/%u legacy-primary=%u height=%f vertices=%u bounds=%s boundary-offset=%zu boundary-opcode='%c'/0x%02X before=[%s] preview=[%s]%s",
        observation,
        object_id,
        object_id,
        strref,
        strref,
        static_cast<unsigned int>(legacy_primary_flag),
        static_cast<double>(height),
        static_cast<unsigned int>(vertex_count),
        bounds_ok
            ? FormatObjectRegistrySpatialSuffix(ObjectNameRegistryEntry{
                  object_id,
                  7,
                  -1,
                  0x7F000000,
                  false,
                  0.0f,
                  0.0f,
                  0.0f,
                  true,
                  min_x,
                  min_y,
                  min_z,
                  max_x,
                  max_y,
                  max_z,
                  std::string{},
                  0}).c_str()
            : L"<unreadable>",
        boundary_offset,
        boundary_opcode,
        boundary_byte,
        FormatCnwMessageReadState(before).c_str(),
        FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 128)).c_str(),
        FormatLiveDispatchContext().c_str());
  }
  return true;
}

bool TryForceLegacyLiveTriggerAddLocStringInnerFlag(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    uint32_t* strref,
    uint32_t* boundary_delta) {
  if (!ReturnAddressMatchesMainRva(return_address, kEeClientReadCExoLocStringClientTlkFlagReturnRva) ||
      !PendingLegacyLiveTriggerAddMatches(message, before, true) ||
      g_pending_legacy_live_trigger_add_parse.name_consumed) {
    return false;
  }

  g_pending_legacy_live_trigger_add_parse.locstring_flag_forced = true;
  if (strref != nullptr) {
    *strref = g_pending_legacy_live_trigger_add_parse.strref;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = g_pending_legacy_live_trigger_add_parse.boundary_delta;
  }
  return true;
}

bool ShouldCheckLegacyLiveTriggerAddNameString(void* return_address) {
  return g_pending_legacy_live_trigger_add_parse.active &&
      ReturnAddressMatchesMainRva(return_address, kEeClientReadCExoLocStringClientStringReturnRva);
}

bool TryConsumePendingLegacyLiveTriggerAddNameString(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    CExoStringView* result_string,
    CnwMessageReadState* after,
    uint32_t* strref,
    uint32_t* consumed_bytes,
    uint32_t* boundary_delta,
    std::wstring* preview) {
  if (after != nullptr) {
    *after = before;
  }
  if (strref != nullptr) {
    *strref = 0;
  }
  if (consumed_bytes != nullptr) {
    *consumed_bytes = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  const PendingLegacyLiveTriggerAddParse& pending = g_pending_legacy_live_trigger_add_parse;
  if (!ReturnAddressMatchesMainRva(return_address, kEeClientReadCExoLocStringClientStringReturnRva) ||
      !pending.active ||
      !pending.locstring_flag_forced ||
      pending.name_consumed ||
      !PendingLegacyLiveTriggerAddMatches(message, before, true) ||
      before.read_buffer_size - before.read_buffer_ptr < 4 ||
      before.read_buffer_ptr + 4 > pending.boundary_buffer_ptr) {
    return false;
  }

  std::vector<uint8_t> bytes;
  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 64);
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, preview_length, &bytes) ||
      bytes.size() < 4) {
    return false;
  }

  uint32_t observed_strref = 0;
  if (!TryReadU32LeFromBytes(bytes, 0, &observed_strref)) {
    return false;
  }

  if (!WriteUInt32AtOffset(message, 0x44, before.read_buffer_ptr + 4)) {
    return false;
  }

  SetEmptyCExoString(result_string);
  g_pending_legacy_live_trigger_add_parse.name_consumed = true;
  if (after != nullptr) {
    *after = ReadCnwMessageReadState(message);
  }
  if (strref != nullptr) {
    *strref = observed_strref;
  }
  if (consumed_bytes != nullptr) {
    *consumed_bytes = 4;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = pending.boundary_delta;
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64));
  }
  return true;
}

bool TryConsumeLegacyLiveTriggerAddPrimaryBool(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    uint8_t* result,
    CnwMessageReadState* after,
    uint32_t* boundary_remaining,
    uint8_t* observed_flag) {
  if (result != nullptr) {
    *result = 0;
  }
  if (after != nullptr) {
    *after = before;
  }
  if (boundary_remaining != nullptr) {
    *boundary_remaining = 0;
  }
  if (observed_flag != nullptr) {
    *observed_flag = 0;
  }

  PendingLegacyLiveTriggerAddParse& pending = g_pending_legacy_live_trigger_add_parse;
  if (!ReturnAddressMatchesMainRva(return_address, kEeClientLiveTriggerPostNamePrimaryBoolReturnRva) ||
      !pending.active ||
      !pending.name_consumed ||
      pending.legacy_primary_flag_consumed ||
      !PendingLegacyLiveTriggerAddMatches(message, before, false) ||
      before.read_buffer_ptr != pending.name_buffer_ptr + 4 ||
      before.read_buffer_ptr + 1 > pending.boundary_buffer_ptr) {
    return false;
  }

  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, 1, &bytes) ||
      bytes.empty() ||
      bytes[0] > 1 ||
      !WriteUInt32AtOffset(message, 0x44, before.read_buffer_ptr + 1)) {
    return false;
  }

  pending.legacy_primary_flag_consumed = true;
  if (result != nullptr) {
    *result = bytes[0] != 0 ? 1 : 0;
  }
  if (after != nullptr) {
    *after = ReadCnwMessageReadState(message);
  }
  if (boundary_remaining != nullptr) {
    *boundary_remaining = before.read_buffer_ptr < pending.boundary_buffer_ptr
        ? pending.boundary_buffer_ptr - (before.read_buffer_ptr + 1)
        : 0;
  }
  if (observed_flag != nullptr) {
    *observed_flag = bytes[0];
  }
  return true;
}

bool TryForceLegacyLiveTriggerAddSyntheticBool(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const wchar_t** field_name,
    uint32_t* boundary_remaining) {
  if (field_name != nullptr) {
    *field_name = L"";
  }
  if (boundary_remaining != nullptr) {
    *boundary_remaining = 0;
  }

  const PendingLegacyLiveTriggerAddParse& pending = g_pending_legacy_live_trigger_add_parse;
  if (!pending.active ||
      !pending.name_consumed ||
      !PendingLegacyLiveTriggerAddMatches(message, before, false)) {
    return false;
  }

  const bool secondary_bool =
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveTriggerPostNameSecondaryBoolReturnRva);
  const bool optional_bool =
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveTriggerPostNameOptionalBoolReturnRva);
  if (!secondary_bool && !optional_bool) {
    return false;
  }

  if (field_name != nullptr) {
    *field_name = secondary_bool ? L"ee-secondary" : L"ee-optional";
  }
  if (boundary_remaining != nullptr) {
    *boundary_remaining = before.read_buffer_ptr <= pending.boundary_buffer_ptr
        ? pending.boundary_buffer_ptr - before.read_buffer_ptr
        : 0;
  }
  return true;
}

bool IsLegacyLiveTriggerAddSyntheticCursorByteReturn(void* return_address) {
  return ReturnAddressMatchesMainRva(return_address, kEeClientLiveTriggerPostNameCursorByteReturnRva);
}

bool TryForceLegacyLiveTriggerAddSyntheticCursorByte(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    uint8_t* result,
    uint32_t* boundary_remaining) {
  if (result != nullptr) {
    *result = 0;
  }
  if (boundary_remaining != nullptr) {
    *boundary_remaining = 0;
  }

  const PendingLegacyLiveTriggerAddParse& pending = g_pending_legacy_live_trigger_add_parse;
  if (!IsLegacyLiveTriggerAddSyntheticCursorByteReturn(return_address) ||
      !pending.active ||
      !pending.name_consumed ||
      !PendingLegacyLiveTriggerAddMatches(message, before, false)) {
    return false;
  }

  if (result != nullptr) {
    // Trigger cursors are binary in EE: 1 means clickable/transition cursor.
    *result = 1;
  }
  if (boundary_remaining != nullptr) {
    *boundary_remaining = before.read_buffer_ptr <= pending.boundary_buffer_ptr
        ? pending.boundary_buffer_ptr - before.read_buffer_ptr
        : 0;
  }
  return true;
}

bool ShouldCheckLegacyLiveUpdateNameString(void* return_address) {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_UPDATE_NAME_RECOVER") &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_DOOR_UPDATE_NAME_RECOVER") &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1 &&
      (ReturnAddressMatchesMainRva(return_address, kEeClientLiveDoorUpdateNameStringReturnRva) ||
       ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateNameStringReturnRva));
}

bool IsLegacyLivePacketReadContext() {
  return g_auto_connect_ip_known &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1;
}

bool LegacyLiveUpdateAbsentNameSkipDisabled(uint8_t object_type) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_UPDATE_ABSENT_NAME_SKIP")) {
    return true;
  }
  if (object_type == 10) {
    return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_DOOR_UPDATE_ABSENT_NAME_SKIP");
  }
  if (object_type == 9) {
    return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_ABSENT_NAME_SKIP");
  }
  return true;
}

struct LegacyLiveUpdateMaskTranslationPlan {
  uint8_t object_type = 0;
  uint32_t object_id = 0;
  uint32_t raw_mask = 0;
  uint32_t translated_mask = 0;
  uint32_t record_start = 0;
  std::wstring preview;
};

struct LegacyLiveUpdateMaskReadInfo {
  uint8_t object_type = 0;
  uint32_t object_id = 0;
  uint32_t raw_mask = 0;
  uint32_t record_start = 0;
  uint32_t rewind = 0;
  std::wstring preview;
};

bool TryDescribeLegacyLiveUpdateMaskRead(
    void* return_address,
    void* message,
    int bit_count,
    const CnwMessageReadState& before,
    LegacyLiveUpdateMaskReadInfo* info) {
  if (info != nullptr) {
    *info = LegacyLiveUpdateMaskReadInfo{};
  }
  if (!IsLegacyLivePacketReadContext() ||
      bit_count != 32 ||
      !ReturnAddressMatchesMainRva(return_address, kEeClientLiveObjectUpdateMaskReturnRva) ||
      message == nullptr ||
      !before.readable ||
      CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr > before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 4) {
    return false;
  }

  uint32_t record_start = 0;
  uint32_t rewind = 0;
  std::wstring boundary_preview;
  if (!TryFindLegacyLiveObjectSubMessageBoundaryBefore(
          message,
          before,
          16,
          &record_start,
          &rewind,
          &boundary_preview) ||
      before.read_buffer_ptr != record_start + 6 ||
      before.read_buffer_size - record_start < 10) {
    return false;
  }

  const uint32_t preview_length = std::min<uint32_t>(before.read_buffer_size - record_start, 96);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, record_start, preview_length, &bytes) ||
      bytes.size() < 10 ||
      bytes[0] != 'U' ||
      !LooksLikeLegacyLiveObjectIdAt(bytes, 2)) {
    return false;
  }

  uint32_t object_id = 0;
  uint32_t raw_mask = 0;
  if (!TryReadU32LeFromBytes(bytes, 2, &object_id) ||
      !TryReadU32LeFromBytes(bytes, 6, &raw_mask)) {
    return false;
  }

  if (info != nullptr) {
    info->object_type = bytes[1];
    info->object_id = object_id;
    info->raw_mask = raw_mask;
    info->record_start = record_start;
    info->rewind = rewind;
    wchar_t text[1024]{};
    swprintf_s(
        text,
        L"record-start=%u rewind=%u bytes=[%s]",
        record_start,
        rewind,
        FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 80)).c_str());
    info->preview = text;
  }
  return true;
}

bool TryPlanLegacyLiveUpdateMaskTranslation(
    void* return_address,
    void* message,
    int bit_count,
    const CnwMessageReadState& before,
    LegacyLiveUpdateMaskTranslationPlan* plan) {
  if (plan != nullptr) {
    *plan = LegacyLiveUpdateMaskTranslationPlan{};
  }
  if (!IsLegacyLivePacketReadContext() ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_UPDATE_MASK_TRANSLATE") ||
      bit_count != 32 ||
      !ReturnAddressMatchesMainRva(return_address, kEeClientLiveObjectUpdateMaskReturnRva) ||
      message == nullptr ||
      !before.readable ||
      CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr > before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 4) {
    return false;
  }

  LegacyLiveUpdateMaskReadInfo info;
  if (!TryDescribeLegacyLiveUpdateMaskRead(
          return_address,
          message,
          bit_count,
          before,
          &info)) {
    return false;
  }

  uint32_t translated_mask = info.raw_mask;
  const wchar_t* reason = L"";
  if (info.object_type == 9) {
    // 1.69 placeable U records carry fixed-point position, five state/action
    // BOOLs, then a legacy inline name/tail payload. The state/action block is
    // still layout-compatible with EE, but the all-bits legacy mask is not.
    // Legacy bit 0x4 leaves a scale/state scalar tail for placeables; the
    // subopcode resync path consumes and applies that tail after EE skips it.
    constexpr uint32_t kLegacyPlaceableUpdateBits =
        0x00000001u |  // generic position
        0x00000010u |  // placeable state/action flags
        0x00080000u;   // placeable name payload
    translated_mask = info.raw_mask & kLegacyPlaceableUpdateBits;
    reason = L"placeable-position-state-name";
  } else if (info.object_type == 10) {
    // Door U records also preserve the five legacy state/action BOOLs. EE adds
    // one extra door-state BOOL after that block; HookedMessageReadBool
    // synthesizes it so the following legacy name/tail payload remains aligned.
    // Legacy bit 0x4 tails are handled by the typed scalar-tail resync path.
    constexpr uint32_t kLegacyDoorUpdateBits =
        0x00000001u |  // generic position
        0x00000010u |  // door state/action flags
        0x00080000u;   // door name payload
    translated_mask = info.raw_mask & kLegacyDoorUpdateBits;
    reason = L"door-position-state-name";
  } else if (info.object_type == 7) {
    // Trigger updates from 1.69 carry a short legacy scalar tail after the
    // fixed-point position. Trigger update has no name field to anchor a skip,
    // so leave only the generic position and let the subopcode resync consume
    // the remaining tail bytes before the next live object record.
    constexpr uint32_t kLegacyTriggerUpdateBits =
        0x00000001u;   // generic position
    translated_mask = info.raw_mask & kLegacyTriggerUpdateBits;
    reason = L"trigger-position-only";
  } else {
    return false;
  }

  if (translated_mask == info.raw_mask) {
    return false;
  }

  if (plan != nullptr) {
    plan->object_type = info.object_type;
    plan->object_id = info.object_id;
    plan->raw_mask = info.raw_mask;
    plan->translated_mask = translated_mask;
    plan->record_start = info.record_start;
    wchar_t text[1024]{};
    swprintf_s(
        text,
        L"reason=%s %s",
        reason,
        info.preview.c_str());
    plan->preview = text;
  }
  return true;
}

bool IdentifyLegacyLiveUpdateNameModeFlagReturn(
    void* return_address,
    uint8_t* object_type,
    const wchar_t** label) {
  if (object_type != nullptr) {
    *object_type = 0;
  }
  if (label != nullptr) {
    *label = L"object";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLiveDoorUpdateNameModeFlagReturnRva)) {
    if (object_type != nullptr) {
      *object_type = 10;
    }
    if (label != nullptr) {
      *label = L"door";
    }
    return true;
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateNameModeFlagReturnRva)) {
    if (object_type != nullptr) {
      *object_type = 9;
    }
    if (label != nullptr) {
      *label = L"placeable";
    }
    return true;
  }
  return false;
}

bool IdentifyLegacyLiveUpdateNameStringReturn(
    void* return_address,
    uint8_t* object_type,
    const wchar_t** label) {
  if (object_type != nullptr) {
    *object_type = 0;
  }
  if (label != nullptr) {
    *label = L"object";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLiveDoorUpdateNameStringReturnRva)) {
    if (object_type != nullptr) {
      *object_type = 10;
    }
    if (label != nullptr) {
      *label = L"door";
    }
    return true;
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateNameStringReturnRva)) {
    if (object_type != nullptr) {
      *object_type = 9;
    }
    if (label != nullptr) {
      *label = L"placeable";
    }
    return true;
  }
  return false;
}

bool TryFindLegacyLiveUpdateRecordBounds(
    void* message,
    const CnwMessageReadState& state,
    uint8_t object_type,
    uint32_t* record_start_buffer_ptr,
    uint32_t* boundary_buffer_ptr,
    uint32_t* object_id,
    uint32_t* update_mask,
    std::wstring* preview) {
  if (record_start_buffer_ptr != nullptr) {
    *record_start_buffer_ptr = 0;
  }
  if (boundary_buffer_ptr != nullptr) {
    *boundary_buffer_ptr = 0;
  }
  if (object_id != nullptr) {
    *object_id = 0;
  }
  if (update_mask != nullptr) {
    *update_mask = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  if (message == nullptr ||
      !state.readable ||
      CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr > state.read_buffer_size ||
      object_type == 0) {
    return false;
  }

  uint32_t record_start = 0;
  uint32_t rewind = 0;
  std::wstring rewind_preview;
  if (!TryFindLegacyLiveObjectSubMessageBoundaryBefore(
          message,
          state,
          192,
          &record_start,
          &rewind,
          &rewind_preview)) {
    return false;
  }
  if (record_start > state.read_buffer_size || state.read_buffer_size - record_start < 10) {
    return false;
  }

  const uint32_t remaining = state.read_buffer_size - record_start;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 256);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, state, record_start, preview_length, &bytes) ||
      bytes.size() < 10 ||
      bytes[0] != 'U' ||
      bytes[1] != object_type ||
      !LooksLikeLegacyLiveObjectIdAt(bytes, 2)) {
    return false;
  }

  uint32_t parsed_object_id = 0;
  uint32_t parsed_update_mask = 0;
  if (!TryReadU32LeFromBytes(bytes, 2, &parsed_object_id) ||
      !TryReadU32LeFromBytes(bytes, 6, &parsed_update_mask)) {
    return false;
  }

  const size_t current_offset = state.read_buffer_ptr > record_start
      ? static_cast<size_t>(state.read_buffer_ptr - record_start)
      : 0;
  const size_t search_start = std::max<size_t>(10, current_offset + 1);
  size_t boundary_offset = 0;
  bool consumed_to_end = false;
  const bool found_boundary =
      FindLegacyLiveObjectSubMessageBoundaryInBytes(bytes, search_start, &boundary_offset) &&
      boundary_offset > current_offset &&
      boundary_offset < bytes.size();

  uint32_t boundary = 0;
  if (found_boundary) {
    boundary = record_start + static_cast<uint32_t>(boundary_offset);
  } else if (remaining <= preview_length) {
    consumed_to_end = true;
    boundary = state.read_buffer_size;
    boundary_offset = bytes.size();
  } else {
    return false;
  }

  if (boundary <= state.read_buffer_ptr || boundary > state.read_buffer_size) {
    return false;
  }

  if (record_start_buffer_ptr != nullptr) {
    *record_start_buffer_ptr = record_start;
  }
  if (boundary_buffer_ptr != nullptr) {
    *boundary_buffer_ptr = boundary;
  }
  if (object_id != nullptr) {
    *object_id = parsed_object_id;
  }
  if (update_mask != nullptr) {
    *update_mask = parsed_update_mask;
  }
  if (preview != nullptr) {
    wchar_t text[1024]{};
    const wchar_t boundary_opcode = found_boundary && boundary_offset < bytes.size() &&
            bytes[boundary_offset] >= 0x20 && bytes[boundary_offset] <= 0x7E
        ? static_cast<wchar_t>(bytes[boundary_offset])
        : L'.';
    const unsigned int boundary_byte =
        found_boundary && boundary_offset < bytes.size() ? bytes[boundary_offset] : 0;
    swprintf_s(
        text,
        L"record-start=%u rewind=%u object=0x%08X/%u update-mask=0x%08X boundary=%u boundary-delta=%u consumed-to-end=%u boundary-opcode='%c'/0x%02X bytes=[%s]",
        record_start,
        rewind,
        parsed_object_id,
        parsed_object_id,
        parsed_update_mask,
        boundary,
        boundary - state.read_buffer_ptr,
        consumed_to_end ? 1u : 0u,
        boundary_opcode,
        boundary_byte,
        FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 96)).c_str());
    *preview = text;
  }
  return true;
}

bool QueuePendingLegacyLiveUpdateAbsentNameParse(
    void* message,
    const CnwMessageReadState& before,
    uint8_t object_type,
    uint32_t object_id,
    uint32_t update_mask,
    uint32_t record_start_buffer_ptr,
    uint32_t boundary_buffer_ptr) {
  g_pending_legacy_live_update_absent_name_parse = PendingLegacyLiveUpdateAbsentNameParse{};
  if (message == nullptr || !before.readable || object_type == 0 ||
      record_start_buffer_ptr >= boundary_buffer_ptr ||
      boundary_buffer_ptr > before.read_buffer_size) {
    return false;
  }

  PendingLegacyLiveUpdateAbsentNameParse pending{};
  pending.active = true;
  pending.message = message;
  pending.dispatch = g_current_server_to_player_message_observation;
  pending.object_type = object_type;
  pending.object_id = object_id;
  pending.update_mask = update_mask;
  pending.record_start_buffer_ptr = record_start_buffer_ptr;
  pending.boundary_buffer_ptr = boundary_buffer_ptr;
  pending.fragments_ptr = before.read_fragments_ptr;
  pending.fragment_bit_ptr = before.read_fragments_bit_ptr;
  pending.fragment_bit_size = before.read_fragments_bit_size;
  g_pending_legacy_live_update_absent_name_parse = pending;
  return true;
}

bool TryForceLegacyLiveUpdateAbsentNameModeFlag(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    uint8_t* object_type,
    const wchar_t** label,
    uint32_t* object_id,
    uint32_t* update_mask,
    uint32_t* boundary_delta,
    std::wstring* preview) {
  if (object_type != nullptr) {
    *object_type = 0;
  }
  if (label != nullptr) {
    *label = L"object";
  }
  if (object_id != nullptr) {
    *object_id = 0;
  }
  if (update_mask != nullptr) {
    *update_mask = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  uint8_t expected_type = 0;
  const wchar_t* expected_label = L"object";
  if (!IdentifyLegacyLiveUpdateNameModeFlagReturn(return_address, &expected_type, &expected_label) ||
      !IsLegacyLivePacketReadContext() ||
      LegacyLiveUpdateAbsentNameSkipDisabled(expected_type)) {
    return false;
  }

  uint32_t record_start = 0;
  uint32_t boundary = 0;
  uint32_t parsed_object_id = 0;
  uint32_t parsed_update_mask = 0;
  std::wstring record_preview;
  if (!TryFindLegacyLiveUpdateRecordBounds(
          message,
          before,
          expected_type,
          &record_start,
          &boundary,
          &parsed_object_id,
          &parsed_update_mask,
          &record_preview)) {
    return false;
  }

  if (!QueuePendingLegacyLiveUpdateAbsentNameParse(
          message,
          before,
          expected_type,
          parsed_object_id,
          parsed_update_mask,
          record_start,
          boundary)) {
    return false;
  }

  if (object_type != nullptr) {
    *object_type = expected_type;
  }
  if (label != nullptr) {
    *label = expected_label;
  }
  if (object_id != nullptr) {
    *object_id = parsed_object_id;
  }
  if (update_mask != nullptr) {
    *update_mask = parsed_update_mask;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = boundary - before.read_buffer_ptr;
  }
  if (preview != nullptr) {
    *preview = record_preview;
  }
  return true;
}

bool PendingLegacyLiveUpdateAbsentNameParseMatches(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    uint8_t* object_type,
    const wchar_t** label) {
  if (object_type != nullptr) {
    *object_type = 0;
  }
  if (label != nullptr) {
    *label = L"object";
  }

  uint8_t expected_type = 0;
  const wchar_t* expected_label = L"object";
  const PendingLegacyLiveUpdateAbsentNameParse& pending =
      g_pending_legacy_live_update_absent_name_parse;
  if (!pending.active ||
      !IdentifyLegacyLiveUpdateNameStringReturn(return_address, &expected_type, &expected_label) ||
      LegacyLiveUpdateAbsentNameSkipDisabled(expected_type) ||
      pending.message != message ||
      pending.dispatch != g_current_server_to_player_message_observation ||
      pending.object_type != expected_type ||
      !before.readable ||
      before.read_buffer_ptr < pending.record_start_buffer_ptr ||
      before.read_buffer_ptr >= pending.boundary_buffer_ptr ||
      before.read_fragments_ptr != pending.fragments_ptr ||
      before.read_fragments_bit_ptr != pending.fragment_bit_ptr ||
      before.read_fragments_bit_size != pending.fragment_bit_size) {
    return false;
  }

  if (object_type != nullptr) {
    *object_type = expected_type;
  }
  if (label != nullptr) {
    *label = expected_label;
  }
  return true;
}

bool FindLegacyLiveInlineNameInRecord(
    const std::vector<uint8_t>& bytes,
    size_t search_begin,
    size_t search_end,
    const std::string& expected_name,
    size_t* name_offset,
    size_t* name_end,
    uint32_t* name_length,
    std::string* preview) {
  if (name_offset != nullptr) {
    *name_offset = SIZE_MAX;
  }
  if (name_end != nullptr) {
    *name_end = SIZE_MAX;
  }
  if (name_length != nullptr) {
    *name_length = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }
  if (search_begin > bytes.size()) {
    return false;
  }
  search_end = std::min(search_end, bytes.size());
  if (search_begin >= search_end || search_end - search_begin < 4) {
    return false;
  }

  bool found = false;
  size_t best_offset = SIZE_MAX;
  size_t best_end = SIZE_MAX;
  size_t best_post_bytes = SIZE_MAX;
  uint32_t best_length = 0;
  std::string best_preview;

  for (size_t offset = search_begin; offset + 4 <= search_end; ++offset) {
    uint32_t candidate_length = 0;
    size_t candidate_end = 0;
    std::string candidate_preview;
    if (!LooksLikeInlineCExoStringAt(
            bytes,
            offset,
            &candidate_length,
            &candidate_end,
            &candidate_preview) ||
        candidate_end > search_end) {
      continue;
    }
    if (!expected_name.empty() && candidate_preview != expected_name) {
      continue;
    }

    const size_t post_bytes = search_end - candidate_end;
    if (!found ||
        post_bytes < best_post_bytes ||
        (post_bytes == best_post_bytes && offset < best_offset)) {
      found = true;
      best_offset = offset;
      best_end = candidate_end;
      best_post_bytes = post_bytes;
      best_length = candidate_length;
      best_preview = candidate_preview;
    }
  }

  if (!found && !expected_name.empty()) {
    return FindLegacyLiveInlineNameInRecord(
        bytes,
        search_begin,
        search_end,
        std::string{},
        name_offset,
        name_end,
        name_length,
        preview);
  }
  if (!found) {
    return false;
  }

  if (name_offset != nullptr) {
    *name_offset = best_offset;
  }
  if (name_end != nullptr) {
    *name_end = best_end;
  }
  if (name_length != nullptr) {
    *name_length = best_length;
  }
  if (preview != nullptr) {
    *preview = best_preview;
  }
  return true;
}

Vector3f LegacyFacingToOrientation(uint16_t facing) {
  constexpr float kTwoPi = 6.28318530717958647692f;
  const float radians = static_cast<float>(facing) * (kTwoPi / 65536.0f);
  return Vector3f{std::cos(radians), std::sin(radians), 0.0f};
}

void LogLegacyLivePlaceableUpdateTail(
    void* message,
    const CnwMessageReadState& before,
    const PendingLegacyLiveUpdateAbsentNameParse& pending,
    const std::string& registry_name) {
  if (message == nullptr ||
      (pending.object_type != 9 && pending.object_type != 10) ||
      pending.record_start_buffer_ptr >= pending.boundary_buffer_ptr ||
      pending.boundary_buffer_ptr > before.read_buffer_size ||
      before.read_buffer_ptr < pending.record_start_buffer_ptr ||
      before.read_buffer_ptr >= pending.boundary_buffer_ptr) {
    return;
  }

  const uint32_t record_length =
      pending.boundary_buffer_ptr - pending.record_start_buffer_ptr;
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(
          message,
          before,
          pending.record_start_buffer_ptr,
          record_length,
          &bytes) ||
      bytes.size() < 10) {
    return;
  }

  const size_t tail_offset = before.read_buffer_ptr - pending.record_start_buffer_ptr;
  size_t inline_name_offset = SIZE_MAX;
  size_t inline_name_end = SIZE_MAX;
  uint32_t inline_name_length = 0;
  std::string inline_name;
  bool name_found = FindLegacyLiveInlineNameInRecord(
      bytes,
      tail_offset,
      bytes.size(),
      registry_name,
      &inline_name_offset,
      &inline_name_end,
      &inline_name_length,
      &inline_name);
  if (!name_found && pending.object_type == 10 && bytes.size() >= tail_offset + 13) {
    uint32_t strref = 0;
    if (TryReadU32LeFromBytes(bytes, tail_offset + 9, &strref)) {
      char strref_name[64]{};
      std::snprintf(strref_name, sizeof(strref_name), "strref:%u/0x%08X", strref, strref);
      inline_name_offset = tail_offset + 9;
      inline_name_end = tail_offset + 13;
      inline_name_length = 0;
      inline_name = strref_name;
      name_found = true;
    }
  }
  const size_t tail_end = name_found ? inline_name_offset : bytes.size();
  const size_t tail_length =
      tail_end > tail_offset ? tail_end - tail_offset : 0;
  const size_t post_name_bytes =
      name_found && inline_name_end <= bytes.size() ? bytes.size() - inline_name_end : 0;

  uint16_t legacy_facing = 0;
  const bool has_facing =
      tail_length >= 2 && TryReadU16LeFromBytes(bytes, tail_offset, &legacy_facing);
  uint8_t legacy_state = 0;
  const bool has_state = tail_length >= 3;
  if (has_state) {
    legacy_state = bytes[tail_offset + 2];
  }
  float legacy_scale = 0.0f;
  const bool has_scale =
      tail_length >= 7 && TryReadFloatLeFromBytes(bytes, tail_offset + 3, &legacy_scale);
  uint16_t legacy_tail_flags = 0;
  const bool has_tail_flags =
      tail_length >= 9 && TryReadU16LeFromBytes(bytes, tail_offset + 7, &legacy_tail_flags);
  const Vector3f inferred_orientation =
      has_facing ? LegacyFacingToOrientation(legacy_facing) : Vector3f{};
  const bool scale_plausible =
      has_scale && IsPlausibleLegacyObjectScale(legacy_scale);

  void* resolved_game_object = nullptr;
  void* resolved_nwc_object = nullptr;
  Vector3f orientation_before{};
  Vector3f orientation_after{};
  bool orientation_before_ok = false;
  bool orientation_after_ok = false;
  bool orientation_apply_attempted = false;
  bool orientation_apply_ok = false;
  float scale_before = 0.0f;
  float scale_after = 0.0f;
  bool scale_before_ok = false;
  bool scale_after_ok = false;
  bool scale_apply_attempted = false;
  bool scale_apply_ok = false;
  bool state_apply_attempted = false;
  bool state_apply_ok = false;

  const bool tail_apply_disabled =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_TAIL_APPLY");
  const bool orientation_apply_enabled =
      has_facing &&
      !tail_apply_disabled &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_ORIENTATION_APPLY");
  const bool scale_apply_enabled =
      scale_plausible &&
      !tail_apply_disabled &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_SCALE_APPLY");
  const bool state_apply_enabled =
      has_tail_flags &&
      !tail_apply_disabled &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_STATE_APPLY");
  const bool tail_apply_attempted =
      orientation_apply_enabled || scale_apply_enabled || state_apply_enabled;

  if (tail_apply_attempted &&
      TryResolveNwcObjectById(
          pending.object_id,
          &resolved_game_object,
          &resolved_nwc_object)) {
    if (orientation_apply_enabled) {
      orientation_apply_attempted = true;
      orientation_before_ok = TryReadGobOrientation(resolved_nwc_object, &orientation_before);
      orientation_apply_ok = TrySetGobOrientation(resolved_nwc_object, inferred_orientation);
      orientation_after_ok = TryReadGobOrientation(resolved_nwc_object, &orientation_after);
    }
    if (scale_apply_enabled) {
      scale_apply_attempted = true;
      scale_before_ok = TryReadGobScale(resolved_nwc_object, &scale_before);
      scale_apply_ok = TrySetGobScale(resolved_nwc_object, legacy_scale);
      scale_after_ok = TryReadGobScale(resolved_nwc_object, &scale_after);
    }
    if (state_apply_enabled) {
      state_apply_attempted = true;
      state_apply_ok = TrySetGobAppearanceState(resolved_nwc_object, legacy_tail_flags);
    }
  } else if (tail_apply_attempted) {
    orientation_apply_attempted = orientation_apply_enabled;
    scale_apply_attempted = scale_apply_enabled;
    state_apply_attempted = state_apply_enabled;
  }

  if (tail_apply_attempted) {
    const LONG apply_observation =
        InterlockedIncrement(&g_legacy_live_placeable_update_tail_apply_observations);
    const bool tail_apply_ok =
        (!orientation_apply_attempted || orientation_apply_ok) &&
        (!scale_apply_attempted || scale_apply_ok) &&
        (!state_apply_attempted || state_apply_ok);
    if (apply_observation <= 360 || !tail_apply_ok) {
      const std::wstring orientation_before_text =
          orientation_before_ok ? FormatVector3f(orientation_before) : L"<unreadable>";
      const std::wstring orientation_after_text =
          orientation_after_ok ? FormatVector3f(orientation_after) : L"<unreadable>";
      wchar_t scale_before_text[48]{};
      wchar_t scale_after_text[48]{};
      if (scale_before_ok) {
        swprintf_s(scale_before_text, L"%.4f", static_cast<double>(scale_before));
      } else {
        wcscpy_s(scale_before_text, L"<unreadable>");
      }
      if (scale_after_ok) {
        swprintf_s(scale_after_text, L"%.4f", static_cast<double>(scale_after));
      } else {
        wcscpy_s(scale_after_text, L"<unreadable>");
      }
      LogFormat(
          L"legacy live %s update tail apply #%ld: object=0x%08X/%u game-object=%p nwc-object=%p facing=0x%04X/%u state-byte=0x%02X/%u appearance-state=0x%04X/%u scale=%.4f scale-plausible=%d orientation=%s attempted[tail=%d orientation=%d scale=%d state=%d] ok[tail=%d orientation=%d scale=%d state=%d] orientation-before=%s orientation-after=%s scale-before=%s scale-after=%s%s",
          LegacyLiveObjectTypeName(pending.object_type),
          apply_observation,
          pending.object_id,
          pending.object_id,
          resolved_game_object,
          resolved_nwc_object,
          static_cast<unsigned int>(legacy_facing),
          static_cast<unsigned int>(legacy_facing),
          static_cast<unsigned int>(legacy_state),
          static_cast<unsigned int>(legacy_state),
          static_cast<unsigned int>(legacy_tail_flags),
          static_cast<unsigned int>(legacy_tail_flags),
          static_cast<double>(legacy_scale),
          scale_plausible ? 1 : 0,
          has_facing ? FormatVector3f(inferred_orientation).c_str() : L"<absent>",
          tail_apply_attempted ? 1 : 0,
          orientation_apply_attempted ? 1 : 0,
          scale_apply_attempted ? 1 : 0,
          state_apply_attempted ? 1 : 0,
          tail_apply_ok ? 1 : 0,
          orientation_apply_ok ? 1 : 0,
          scale_apply_ok ? 1 : 0,
          state_apply_ok ? 1 : 0,
          orientation_before_text.c_str(),
          orientation_after_text.c_str(),
          scale_before_text,
          scale_after_text,
          FormatLiveDispatchContext().c_str());
    }
  }

  const LONG observation =
      InterlockedIncrement(&g_legacy_live_placeable_update_tail_observations);
  const bool interesting =
      observation <= 360 ||
      pending.object_type == 10 ||
      tail_length != 9 ||
      post_name_bytes != 0 ||
      !name_found ||
      ContainsAsciiNoCase(registry_name, "chest") ||
      ContainsAsciiNoCase(registry_name, "portal") ||
      ContainsAsciiNoCase(registry_name, "waystone") ||
      ContainsAsciiNoCase(registry_name, "pedestal");
  if (!interesting) {
    return;
  }

  LogFormat(
      L"legacy live %s update tail #%ld: object=0x%08X/%u update-mask=0x%08X record-start=%u boundary=%u tail-offset=%zu tail-len=%zu name-found=%d name-offset=%zu name-end=%zu name-len=%u post-name-bytes=%zu registry-name='%s' inline-name='%s' facing=%s state-byte=%s scale=%s scale-plausible=%d appearance-state=%s inferred-orientation=(%.4f, %.4f, %.4f) apply-attempted=%d apply-ok=%d orientation-ok=%d scale-ok=%d state-ok=%d tail=[%s] post-name=[%s] record=[%s]%s",
      LegacyLiveObjectTypeName(pending.object_type),
      observation,
      pending.object_id,
      pending.object_id,
      pending.update_mask,
      pending.record_start_buffer_ptr,
      pending.boundary_buffer_ptr,
      tail_offset,
      tail_length,
      name_found ? 1 : 0,
      name_found ? inline_name_offset : SIZE_MAX,
      name_found ? inline_name_end : SIZE_MAX,
      inline_name_length,
      post_name_bytes,
      NarrowToWide(registry_name).c_str(),
      NarrowToWide(inline_name).c_str(),
      has_facing
          ? FormatBytes(bytes.data() + tail_offset, 2).c_str()
          : L"<absent>",
      has_state
          ? FormatBytes(bytes.data() + tail_offset + 2, 1).c_str()
          : L"<absent>",
      has_scale
          ? FormatBytes(bytes.data() + tail_offset + 3, 4).c_str()
          : L"<absent>",
      scale_plausible ? 1 : 0,
      has_tail_flags
          ? FormatBytes(bytes.data() + tail_offset + 7, 2).c_str()
          : L"<absent>",
      static_cast<double>(inferred_orientation.x),
      static_cast<double>(inferred_orientation.y),
      static_cast<double>(inferred_orientation.z),
      tail_apply_attempted ? 1 : 0,
      ((orientation_apply_attempted && !orientation_apply_ok) ||
       (scale_apply_attempted && !scale_apply_ok) ||
       (state_apply_attempted && !state_apply_ok)) ? 0 : (tail_apply_attempted ? 1 : 0),
      orientation_apply_ok ? 1 : 0,
      scale_apply_ok ? 1 : 0,
      state_apply_ok ? 1 : 0,
      tail_length > 0
          ? FormatBytes(bytes.data() + tail_offset, tail_length).c_str()
          : L"",
      name_found && post_name_bytes > 0
          ? FormatBytes(bytes.data() + inline_name_end, post_name_bytes).c_str()
          : L"",
      FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 112)).c_str(),
      FormatLiveDispatchContext().c_str());
}

bool TryConsumePendingLegacyLiveUpdateAbsentNameString(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    CExoStringView* result_string,
    CnwMessageReadState* after,
    uint8_t* object_type,
    const wchar_t** label,
    uint32_t* object_id,
    uint32_t* update_mask,
    uint32_t* skipped_bytes,
    std::string* synthetic_name,
    std::wstring* preview) {
  if (after != nullptr) {
    *after = before;
  }
  if (object_type != nullptr) {
    *object_type = 0;
  }
  if (label != nullptr) {
    *label = L"object";
  }
  if (object_id != nullptr) {
    *object_id = 0;
  }
  if (update_mask != nullptr) {
    *update_mask = 0;
  }
  if (skipped_bytes != nullptr) {
    *skipped_bytes = 0;
  }
  if (synthetic_name != nullptr) {
    synthetic_name->clear();
  }
  if (preview != nullptr) {
    preview->clear();
  }

  uint8_t expected_type = 0;
  const wchar_t* expected_label = L"object";
  if (message == nullptr ||
      result_string == nullptr ||
      !PendingLegacyLiveUpdateAbsentNameParseMatches(
          message,
          return_address,
          before,
          &expected_type,
          &expected_label)) {
    return false;
  }

  const PendingLegacyLiveUpdateAbsentNameParse pending =
      g_pending_legacy_live_update_absent_name_parse;
  if (pending.boundary_buffer_ptr <= before.read_buffer_ptr ||
      pending.boundary_buffer_ptr > before.read_buffer_size) {
    g_pending_legacy_live_update_absent_name_parse = PendingLegacyLiveUpdateAbsentNameParse{};
    return false;
  }

  ObjectNameRegistryEntry registry_entry{};
  const bool registry_found = TryGetObjectRegistryEntry(pending.object_id, &registry_entry);
  std::string name = registry_found ? registry_entry.name : std::string{};
  if (!name.empty()) {
    auto* const init_string =
        ResolveMainExport<CExoStringInitFromCharArrayFn>("?InitFromCharArray@CExoString@@AEAAAEAV1@PEBDI@Z");
    if (!InitCExoStringValue(init_string, result_string, name)) {
      SetEmptyCExoString(result_string);
      name.clear();
    }
  } else {
    SetEmptyCExoString(result_string);
  }

  LogLegacyLivePlaceableUpdateTail(message, before, pending, name);

  if (!WriteUInt32AtOffset(message, 0x44, pending.boundary_buffer_ptr)) {
    SetEmptyCExoString(result_string);
    g_pending_legacy_live_update_absent_name_parse = PendingLegacyLiveUpdateAbsentNameParse{};
    return false;
  }

  if (after != nullptr) {
    *after = ReadCnwMessageReadState(message);
  }
  if (object_type != nullptr) {
    *object_type = expected_type;
  }
  if (label != nullptr) {
    *label = expected_label;
  }
  if (object_id != nullptr) {
    *object_id = pending.object_id;
  }
  if (update_mask != nullptr) {
    *update_mask = pending.update_mask;
  }
  if (skipped_bytes != nullptr) {
    *skipped_bytes = pending.boundary_buffer_ptr - before.read_buffer_ptr;
  }
  if (synthetic_name != nullptr) {
    *synthetic_name = name;
  }
  if (preview != nullptr) {
    const uint32_t preview_length = std::min<uint32_t>(
        pending.boundary_buffer_ptr - pending.record_start_buffer_ptr,
        96);
    std::vector<uint8_t> bytes;
    if (preview_length > 0 &&
        PeekReadBufferBytes(
            message,
            before,
            pending.record_start_buffer_ptr,
            preview_length,
            &bytes)) {
      wchar_t text[1024]{};
      swprintf_s(
          text,
          L"record-start=%u boundary=%u skipped=%u bytes=[%s]",
          pending.record_start_buffer_ptr,
          pending.boundary_buffer_ptr,
          pending.boundary_buffer_ptr - before.read_buffer_ptr,
          FormatBytes(bytes.data(), bytes.size()).c_str());
      *preview = text;
    }
  }

  g_pending_legacy_live_update_absent_name_parse = PendingLegacyLiveUpdateAbsentNameParse{};
  return true;
}

const wchar_t* IdentifyLegacyLivePlaceableBoolField(void* return_address) {
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameAbsentBoolReturnRva)) {
    return L"add-legacy-flag0";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag1ReturnRva)) {
    return L"add-legacy-optional-target";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag2ReturnRva)) {
    return L"add-legacy-flag1";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag3ReturnRva)) {
    return L"add-legacy-flag2";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag4ReturnRva)) {
    return L"add-legacy-flag3";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag5ReturnRva)) {
    return L"add-legacy-flag4";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag6ReturnRva)) {
    return L"add-legacy-flag5";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag7ReturnRva)) {
    return L"add-legacy-flag6";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag8ReturnRva)) {
    return L"add-legacy-flag7";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameEeOnlyFinalFlagReturnRva)) {
    return L"add-ee-only-final-visual";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag1ReturnRva)) {
    return L"update-flag0";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag2ReturnRva)) {
    return L"update-flag1";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag3ReturnRva)) {
    return L"update-flag2";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag4ReturnRva)) {
    return L"update-flag3";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag5ReturnRva)) {
    return L"update-flag4";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateNameModeFlagReturnRva)) {
    return L"update-name-mode";
  }
  return L"";
}

bool ShouldTraceLegacyLivePlaceableBoolField(void* return_address, const wchar_t** field) {
  const wchar_t* identified_field = IdentifyLegacyLivePlaceableBoolField(return_address);
  if (field != nullptr) {
    *field = identified_field;
  }
  return !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_BOOL_TRACE") &&
      IsLegacyLivePacketReadContext() &&
      identified_field != nullptr &&
      identified_field[0] != L'\0';
}

bool TryRecoverLegacyLiveUpdateNameString(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    CExoStringView* result_string,
    CnwMessageReadState* after,
    uint32_t* declared_length,
    uint32_t* available_length,
    int32_t* boundary_delta,
    std::wstring* preview) {
  if (after != nullptr) {
    *after = before;
  }
  if (declared_length != nullptr) {
    *declared_length = 0;
  }
  if (available_length != nullptr) {
    *available_length = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  if (!ShouldCheckLegacyLiveUpdateNameString(return_address) ||
      message == nullptr ||
      !before.readable ||
      CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr > before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 4) {
    return false;
  }

  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t available = remaining - 4;
  const uint32_t rewind_window = std::min<uint32_t>(before.read_buffer_ptr, 4);
  const uint32_t scan_start = before.read_buffer_ptr - rewind_window;
  const uint32_t scan_length = std::min<uint32_t>(before.read_buffer_size - scan_start, 128);
  std::vector<uint8_t> bytes;
  if (scan_length < 4 ||
      !PeekReadBufferBytes(message, before, scan_start, scan_length, &bytes) ||
      bytes.size() < rewind_window + 4) {
    return false;
  }

  const size_t current_offset = before.read_buffer_ptr - scan_start;
  uint32_t length = 0;
  if (!TryReadU32LeFromBytes(bytes, current_offset, &length)) {
    return false;
  }

  if (LooksLikeInlineCExoStringAt(bytes, current_offset, nullptr, nullptr, nullptr)) {
    return false;
  }

  constexpr uint32_t kMaxToleratedLiveUpdateNameBytes = 1024;
  if (length <= available && length <= kMaxToleratedLiveUpdateNameBytes) {
    return false;
  }

  if (before.read_buffer_ptr + 4 <= before.read_buffer_size) {
    const uint32_t target_after_strref = before.read_buffer_ptr + 4;
    const size_t boundary_after_strref = current_offset + 4;
    const bool ends_at_strref = target_after_strref == before.read_buffer_size;
    const bool next_is_submessage =
        boundary_after_strref + 1 < bytes.size() &&
        LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, boundary_after_strref);
    if (ends_at_strref || next_is_submessage) {
      if (!WriteUInt32AtOffset(message, 0x44, target_after_strref)) {
        return false;
      }

      SetEmptyCExoString(result_string);
      if (after != nullptr) {
        *after = ReadCnwMessageReadState(message);
      }
      if (declared_length != nullptr) {
        *declared_length = length;
      }
      if (available_length != nullptr) {
        *available_length = available;
      }
      if (boundary_delta != nullptr) {
        *boundary_delta = 4;
      }
      if (preview != nullptr) {
        *preview = FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 80));
      }
      return true;
    }
  }

  size_t best_offset = SIZE_MAX;
  uint32_t best_distance = UINT32_MAX;
  for (size_t offset = 0; offset + 1 < bytes.size(); ++offset) {
    const uint32_t absolute_offset = scan_start + static_cast<uint32_t>(offset);
    if (absolute_offset > before.read_buffer_size) {
      continue;
    }
    if (absolute_offset + 1 > before.read_buffer_size) {
      continue;
    }
    const int32_t delta =
        static_cast<int32_t>(absolute_offset) - static_cast<int32_t>(before.read_buffer_ptr);
    if (delta < -static_cast<int32_t>(rewind_window) || delta > 96) {
      continue;
    }
    if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, offset)) {
      continue;
    }
    const uint32_t distance = static_cast<uint32_t>(delta < 0 ? -delta : delta);
    if (distance < best_distance) {
      best_offset = offset;
      best_distance = distance;
    }
  }

  if (best_offset == SIZE_MAX) {
    return false;
  }

  const uint32_t target = scan_start + static_cast<uint32_t>(best_offset);
  if (!WriteUInt32AtOffset(message, 0x44, target)) {
    return false;
  }

  SetEmptyCExoString(result_string);
  if (after != nullptr) {
    *after = ReadCnwMessageReadState(message);
  }
  if (declared_length != nullptr) {
    *declared_length = length;
  }
  if (available_length != nullptr) {
    *available_length = available;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = static_cast<int32_t>(target) - static_cast<int32_t>(before.read_buffer_ptr);
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 80));
  }
  return true;
}

bool TrySkipLegacyLiveShortAddRecord(
    void* message,
    uint8_t object_type,
    const wchar_t* label) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_SHORT_ADD_SKIP") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      message == nullptr) {
    return false;
  }

  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  if (!before.readable || CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr >= before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 8) {
    return false;
  }

  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 128);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, preview_length, &bytes) ||
      bytes.size() < 8 ||
      !LooksLikeLegacyLiveObjectIdAt(bytes, 0)) {
    return false;
  }

  uint32_t object_id = 0;
  uint32_t first_dword = 0;
  uint32_t name_length = 0;
  size_t name_offset = 4;
  size_t name_end = 0;
  std::string name_preview;
  if (!TryReadU32LeFromBytes(bytes, 0, &object_id) ||
      !TryReadU32LeFromBytes(bytes, 4, &first_dword)) {
    return false;
  }

  if (object_type == 9) {
    name_offset = 4;
  } else if (object_type == 10) {
    name_offset = first_dword == 0 ? 12 : 8;
    if (name_offset > bytes.size() || bytes.size() - name_offset < 4) {
      return false;
    }
  } else {
    return false;
  }

  if (LooksLikeInlineCExoStringAt(bytes, name_offset, &name_length, &name_end, &name_preview)) {
    // This is the normal named 1.69 layout that the existing EE field shims can
    // parse. The short transition/door stubs are the records without a sane
    // CExoString at the EE name position.
    return false;
  }

  size_t locstring_boundary_offset = 0;
  if (LooksLikeLegacyLiveShortAddLocStringRecordBytes(
          bytes,
          object_type,
          name_offset,
          &locstring_boundary_offset)) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_short_add_locstring_parse_observations);
    if (observation <= 240) {
      uint32_t strref = 0;
      TryReadU32LeFromBytes(bytes, name_offset, &strref);
      LogFormat(
          L"legacy live short %s add locstring passthrough #%ld: type=%u object=0x%08X/%u first=0x%08X/%u strref=0x%08X/%u name-offset=%zu boundary-offset=%zu boundary-opcode='%c'/0x%02X before=[%s] preview=[%s]%s",
          label != nullptr ? label : L"object",
          observation,
          static_cast<unsigned int>(object_type),
          object_id,
          object_id,
          first_dword,
          first_dword,
          strref,
          strref,
          name_offset,
          locstring_boundary_offset,
          bytes[locstring_boundary_offset] >= 0x20 && bytes[locstring_boundary_offset] <= 0x7E
              ? static_cast<wchar_t>(bytes[locstring_boundary_offset])
              : L'.',
          static_cast<unsigned int>(bytes[locstring_boundary_offset]),
          FormatCnwMessageReadState(before).c_str(),
          FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64)).c_str(),
          FormatLiveDispatchContext().c_str());
    }
    return false;
  }

  size_t boundary_offset = 0;
  const size_t boundary_scan_start = std::min(bytes.size(), std::max<size_t>(8, name_offset));
  if (!FindLegacyLiveObjectSubMessageBoundaryInBytes(bytes, boundary_scan_start, &boundary_offset) ||
      boundary_offset <= 4) {
    return false;
  }

  const uint32_t target = before.read_buffer_ptr + static_cast<uint32_t>(boundary_offset);
  if (!WriteUInt32AtOffset(message, 0x44, target)) {
    return false;
  }

  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_legacy_live_short_add_skip_observations);
  if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    uint32_t declared_at_name = 0;
    if (name_offset <= bytes.size() && bytes.size() - name_offset >= 4) {
      TryReadU32LeFromBytes(bytes, name_offset, &declared_at_name);
    }
    LogFormat(
        L"legacy live short %s add skip #%ld: type=%u object=0x%08X/%u first=0x%08X/%u declared-at-name=0x%08X/%u skipped-buffer-bytes=%zu boundary-opcode='%c'/0x%02X before=[%s] after=[%s] preview=[%s]%s",
        label != nullptr ? label : L"object",
        observation,
        static_cast<unsigned int>(object_type),
        object_id,
        object_id,
        first_dword,
        first_dword,
        declared_at_name,
        declared_at_name,
        boundary_offset,
        bytes[boundary_offset] >= 0x20 && bytes[boundary_offset] <= 0x7E
            ? static_cast<wchar_t>(bytes[boundary_offset])
            : L'.',
        static_cast<unsigned int>(bytes[boundary_offset]),
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64)).c_str(),
        FormatLiveDispatchContext().c_str());
  }

  return true;
}

bool TrySkipLegacyLiveTriggerAddRecord(void* message) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_TRIGGER_ADD_SKIP") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      message == nullptr) {
    return false;
  }

  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  if (!before.readable || CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr >= before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 12) {
    return false;
  }

  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 192);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, preview_length, &bytes) ||
      bytes.size() < 12 ||
      !LooksLikeLegacyLiveObjectIdAt(bytes, 0)) {
    return false;
  }

  uint32_t object_id = 0;
  uint32_t first_dword = 0;
  if (!TryReadU32LeFromBytes(bytes, 0, &object_id) ||
      !TryReadU32LeFromBytes(bytes, 4, &first_dword)) {
    return false;
  }

  uint32_t inline_name_length = 0;
  size_t inline_name_end = 0;
  std::string inline_name_preview;
  if (LooksLikeInlineCExoStringAt(
          bytes,
          4,
          &inline_name_length,
          &inline_name_end,
          &inline_name_preview)) {
    return false;
  }

  size_t boundary_offset = 0;
  bool consumed_to_end = false;
  const bool found_boundary =
      FindLegacyLiveObjectSubMessageBoundaryInBytes(bytes, 8, &boundary_offset) &&
      boundary_offset > 4 &&
      boundary_offset < bytes.size();
  uint32_t target = 0;
  if (found_boundary) {
    target = before.read_buffer_ptr + static_cast<uint32_t>(boundary_offset);
  } else if (remaining <= preview_length) {
    consumed_to_end = true;
    boundary_offset = bytes.size();
    target = before.read_buffer_size;
  } else {
    return false;
  }

  if (target <= before.read_buffer_ptr || target > before.read_buffer_size ||
      !WriteUInt32AtOffset(message, 0x44, target)) {
    return false;
  }

  ClearPendingLegacyLiveBadInlineLocStringSkip();
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_legacy_live_trigger_add_skip_observations);
  if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    const wchar_t boundary_opcode = found_boundary && boundary_offset < bytes.size() &&
            bytes[boundary_offset] >= 0x20 && bytes[boundary_offset] <= 0x7E
        ? static_cast<wchar_t>(bytes[boundary_offset])
        : L'.';
    const unsigned int boundary_byte =
        found_boundary && boundary_offset < bytes.size() ? bytes[boundary_offset] : 0;
    LogFormat(
        L"legacy live trigger add skip #%ld: object=0x%08X/%u first=0x%08X/%u skipped-buffer-bytes=%zu consumed-to-end=%u boundary-opcode='%c'/0x%02X before=[%s] after=[%s] preview=[%s]%s",
        observation,
        object_id,
        object_id,
        first_dword,
        first_dword,
        boundary_offset,
        consumed_to_end ? 1u : 0u,
        boundary_opcode,
        boundary_byte,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 96)).c_str(),
        FormatLiveDispatchContext().c_str());
  }

  return true;
}

bool TryConsumeLegacyLiveScalarTail(
    void* message,
    const CnwMessageReadState& before,
    uint32_t* skipped_bytes,
    std::wstring* preview) {
  if (skipped_bytes != nullptr) {
    *skipped_bytes = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }

  if (!IsLegacyLivePacketReadContext() ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_SCALAR_TAIL_RESYNC") ||
      message == nullptr ||
      !before.readable ||
      CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr >= before.read_buffer_size) {
    return false;
  }

  uint32_t record_start = 0;
  uint32_t rewind_bytes = 0;
  std::wstring rewind_preview;
  if (!TryFindLegacyLiveObjectSubMessageBoundaryBefore(
          message,
          before,
          32,
          &record_start,
          &rewind_bytes,
          &rewind_preview) ||
      before.read_buffer_ptr <= record_start ||
      record_start > before.read_buffer_size ||
      before.read_buffer_size - record_start < 10) {
    return false;
  }

  const uint32_t remaining_from_record = before.read_buffer_size - record_start;
  const uint32_t preview_length = std::min<uint32_t>(remaining_from_record, 160);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, record_start, preview_length, &bytes) ||
      bytes.size() < 10 ||
      bytes[0] != 'U' ||
      !LooksLikeLegacyLiveObjectIdAt(bytes, 2)) {
    return false;
  }

  const uint8_t object_type = bytes[1];
  if (object_type != 9 && object_type != 10) {
    return false;
  }

  uint32_t object_id = 0;
  uint32_t update_mask = 0;
  if (!TryReadU32LeFromBytes(bytes, 2, &object_id) ||
      !TryReadU32LeFromBytes(bytes, 6, &update_mask) ||
      (update_mask & 0x00000004u) == 0) {
    return false;
  }

  const size_t current_offset = before.read_buffer_ptr - record_start;
  if (current_offset < 10 || current_offset >= bytes.size()) {
    return false;
  }

  size_t boundary_offset = 0;
  const bool found_boundary =
      FindLegacyLiveObjectSubMessageBoundaryInBytes(
          bytes,
          current_offset + 1,
          &boundary_offset) &&
      boundary_offset > current_offset &&
      boundary_offset < bytes.size();

  uint32_t target = 0;
  bool consumed_to_end = false;
  if (found_boundary) {
    target = record_start + static_cast<uint32_t>(boundary_offset);
  } else if (remaining_from_record <= preview_length) {
    consumed_to_end = true;
    boundary_offset = bytes.size();
    target = before.read_buffer_size;
  } else {
    return false;
  }

  if (target <= before.read_buffer_ptr || target > before.read_buffer_size) {
    return false;
  }

  const uint32_t tail_length = target - before.read_buffer_ptr;
  if (tail_length != 6 || bytes.size() - current_offset < tail_length) {
    return false;
  }

  float legacy_scale = 0.0f;
  uint16_t legacy_state = 0;
  if (!TryReadFloatLeFromBytes(bytes, current_offset, &legacy_scale) ||
      !TryReadU16LeFromBytes(bytes, current_offset + 4, &legacy_state) ||
      !IsPlausibleLegacyObjectScale(legacy_scale)) {
    return false;
  }

  if (!WriteUInt32AtOffset(message, 0x44, target)) {
    return false;
  }

  ObjectNameRegistryEntry registry_entry{};
  const bool registry_found = TryGetObjectRegistryEntry(object_id, &registry_entry);
  const std::string registry_name = registry_found ? registry_entry.name : std::string{};

  void* resolved_game_object = nullptr;
  void* resolved_nwc_object = nullptr;
  float scale_before = 0.0f;
  float scale_after = 0.0f;
  bool scale_before_ok = false;
  bool scale_after_ok = false;
  bool scale_apply_attempted = false;
  bool scale_apply_ok = false;
  bool state_apply_attempted = false;
  bool state_apply_ok = false;

  const bool tail_apply_disabled =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_SCALAR_TAIL_APPLY") ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_TAIL_APPLY");
  const bool scale_apply_enabled =
      !tail_apply_disabled &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_SCALE_APPLY");
  const bool state_apply_enabled =
      !tail_apply_disabled &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_STATE_APPLY");
  const bool tail_apply_attempted = scale_apply_enabled || state_apply_enabled;

  if (tail_apply_attempted &&
      TryResolveNwcObjectById(object_id, &resolved_game_object, &resolved_nwc_object)) {
    if (scale_apply_enabled) {
      scale_apply_attempted = true;
      scale_before_ok = TryReadGobScale(resolved_nwc_object, &scale_before);
      scale_apply_ok = TrySetGobScale(resolved_nwc_object, legacy_scale);
      scale_after_ok = TryReadGobScale(resolved_nwc_object, &scale_after);
    }
    if (state_apply_enabled) {
      state_apply_attempted = true;
      state_apply_ok = TrySetGobAppearanceState(resolved_nwc_object, legacy_state);
    }
  } else if (tail_apply_attempted) {
    scale_apply_attempted = scale_apply_enabled;
    state_apply_attempted = state_apply_enabled;
  }

  if (tail_apply_attempted) {
    InterlockedIncrement(&g_legacy_live_scalar_tail_apply_observations);
  }

  const bool tail_apply_ok =
      (!scale_apply_attempted || scale_apply_ok) &&
      (!state_apply_attempted || state_apply_ok);
  const LONG observation =
      InterlockedIncrement(&g_legacy_live_scalar_tail_observations);
  const bool interesting =
      observation <= 360 ||
      !tail_apply_ok ||
      object_type == 10 ||
      ContainsAsciiNoCase(registry_name, "chest") ||
      ContainsAsciiNoCase(registry_name, "door") ||
      ContainsAsciiNoCase(registry_name, "portal") ||
      ContainsAsciiNoCase(registry_name, "waystone") ||
      ContainsAsciiNoCase(registry_name, "pedestal");
  if (interesting) {
    wchar_t scale_before_text[48]{};
    wchar_t scale_after_text[48]{};
    if (scale_before_ok) {
      swprintf_s(scale_before_text, L"%.4f", static_cast<double>(scale_before));
    } else {
      wcscpy_s(scale_before_text, L"<unreadable>");
    }
    if (scale_after_ok) {
      swprintf_s(scale_after_text, L"%.4f", static_cast<double>(scale_after));
    } else {
      wcscpy_s(scale_after_text, L"<unreadable>");
    }
    LogFormat(
        L"legacy live %s scalar tail #%ld: object=0x%08X/%u update-mask=0x%08X record-start=%u rewind=%u tail-offset=%zu tail-len=%u target=%u consumed-to-end=%u scale=%.4f appearance-state=0x%04X/%u registry-name='%s' applied[tail=%d scale=%d state=%d] ok[tail=%d scale=%d state=%d] game-object=%p nwc-object=%p scale-before=%s scale-after=%s tail=[%s] record=[%s]%s",
        LegacyLiveObjectTypeName(object_type),
        observation,
        object_id,
        object_id,
        update_mask,
        record_start,
        rewind_bytes,
        current_offset,
        tail_length,
        target,
        consumed_to_end ? 1u : 0u,
        static_cast<double>(legacy_scale),
        static_cast<unsigned int>(legacy_state),
        static_cast<unsigned int>(legacy_state),
        NarrowToWide(registry_name).c_str(),
        tail_apply_attempted ? 1 : 0,
        scale_apply_attempted ? 1 : 0,
        state_apply_attempted ? 1 : 0,
        tail_apply_ok ? 1 : 0,
        scale_apply_ok ? 1 : 0,
        state_apply_ok ? 1 : 0,
        resolved_game_object,
        resolved_nwc_object,
        scale_before_text,
        scale_after_text,
        FormatBytes(bytes.data() + current_offset, tail_length).c_str(),
        FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 96)).c_str(),
        FormatLiveDispatchContext().c_str());
  }

  if (skipped_bytes != nullptr) {
    *skipped_bytes = tail_length;
  }
  if (preview != nullptr) {
    wchar_t text[1024]{};
    swprintf_s(
        text,
        L"typed-%s-scalar-tail=%u target=%u object=0x%08X raw-mask=0x%08X scale=%.4f state=0x%04X tail=[%s]",
        LegacyLiveObjectTypeName(object_type),
        tail_length,
        target,
        object_id,
        update_mask,
        static_cast<double>(legacy_scale),
        static_cast<unsigned int>(legacy_state),
        FormatBytes(bytes.data() + current_offset, tail_length).c_str());
    *preview = text;
  }

  return true;
}

bool ShouldResyncLegacyLiveObjectSubMessageBoundary(void* return_address, const CnwMessageReadState& state) {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_SUBOPCODE_RESYNC") &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1 &&
      g_current_server_to_player_live_opcode_known &&
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveGameObjectUpdateSubOpcodeReturnRva) &&
      state.readable &&
      state.read_buffer_ptr < state.read_buffer_size &&
      !CnwMessageStateOverflow(state);
}

bool TryResyncLegacyLiveObjectSubMessageBoundary(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    uint32_t* skipped_bytes,
    std::wstring* preview) {
  if (skipped_bytes != nullptr) {
    *skipped_bytes = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }
  if (!ShouldResyncLegacyLiveObjectSubMessageBoundary(return_address, before) ||
      before.read_buffer_size - before.read_buffer_ptr < 3) {
    return false;
  }

  constexpr uint32_t kMaxLegacyPaddingScan = 8;
  constexpr uint32_t kMaxLegacyForwardBoundaryScan = 256;
  const uint32_t remaining = before.read_buffer_size - before.read_buffer_ptr;
  const uint32_t length =
      std::min<uint32_t>(remaining, std::max(kMaxLegacyPaddingScan, kMaxLegacyForwardBoundaryScan) + 8);
  std::vector<uint8_t> bytes;
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, length, &bytes) || bytes.size() < 3) {
    return false;
  }
  if (preview != nullptr) {
    *preview = FormatBytes(bytes.data(), bytes.size());
  }

  if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, 0)) {
    uint32_t scalar_tail_skipped = 0;
    std::wstring scalar_tail_preview;
    if (TryConsumeLegacyLiveScalarTail(
            message,
            before,
            &scalar_tail_skipped,
            &scalar_tail_preview)) {
      if (skipped_bytes != nullptr) {
        *skipped_bytes = scalar_tail_skipped;
      }
      if (preview != nullptr) {
        *preview = scalar_tail_preview;
      }
      return true;
    }

    uint32_t backward_target = 0;
    uint32_t backward_rewind = 0;
    std::wstring backward_preview;
    if (TryFindLegacyLiveObjectSubMessageBoundaryBefore(
            message,
            before,
            32,
            &backward_target,
            &backward_rewind,
            &backward_preview)) {
      const bool repeated =
          g_last_legacy_live_backward_resync_dispatch == g_current_server_to_player_message_observation &&
          g_last_legacy_live_backward_resync_from == before.read_buffer_ptr &&
          g_last_legacy_live_backward_resync_target == backward_target;
      if (!repeated && WriteUInt32AtOffset(message, 0x44, backward_target)) {
        g_last_legacy_live_backward_resync_dispatch = g_current_server_to_player_message_observation;
        g_last_legacy_live_backward_resync_from = before.read_buffer_ptr;
        g_last_legacy_live_backward_resync_target = backward_target;
        if (skipped_bytes != nullptr) {
          *skipped_bytes = backward_rewind;
        }
        if (preview != nullptr) {
          wchar_t text[1024]{};
          swprintf_s(
              text,
              L"backward-rewind=%u target=%u current-preview=[%s] rewind-window=[%s]",
              backward_rewind,
              backward_target,
              FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64)).c_str(),
              backward_preview.c_str());
          *preview = text;
        }
        return true;
      }
    }
  }

  if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, 0) &&
      !IsLegacyLiveObjectOpcodeByte(bytes[0]) &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_SCALAR_TAIL_RESYNC")) {
    const uint32_t scan_limit = std::min<uint32_t>(
        32,
        bytes.size() > 1 ? static_cast<uint32_t>(bytes.size() - 1) : 0);
    for (uint32_t offset = 1; offset <= scan_limit; ++offset) {
      if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, offset)) {
        continue;
      }

      const uint32_t target = before.read_buffer_ptr + offset;
      if (!WriteUInt32AtOffset(message, 0x44, target)) {
        return false;
      }
      if (skipped_bytes != nullptr) {
        *skipped_bytes = offset;
      }
      if (preview != nullptr) {
        wchar_t text[1024]{};
        swprintf_s(
            text,
            L"forward-scalar-tail=%u target=%u current-preview=[%s]",
            offset,
            target,
            FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 128)).c_str());
        *preview = text;
      }
      return true;
    }

    if (remaining <= 16 && before.read_buffer_ptr + remaining == before.read_buffer_size) {
      if (!WriteUInt32AtOffset(message, 0x44, before.read_buffer_size)) {
        return false;
      }
      if (skipped_bytes != nullptr) {
        *skipped_bytes = remaining;
      }
      if (preview != nullptr) {
        wchar_t text[1024]{};
        swprintf_s(
            text,
            L"consume-final-scalar-tail=%u target=%u current-preview=[%s]",
            remaining,
            before.read_buffer_size,
            FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 64)).c_str());
        *preview = text;
      }
      return true;
    }
  }

  // Some 1.69 live add/update tails carry a few legacy scalar bytes before the
  // next object submessage. When EE reaches that tail it asks for another
  // subopcode and sees zero; use the typed object boundary as the anchor.
  if (bytes[0] == 0) {
    const uint32_t scan_limit = std::min<uint32_t>(
        kMaxLegacyForwardBoundaryScan,
        bytes.size() > 1 ? static_cast<uint32_t>(bytes.size() - 1) : 0);
    for (uint32_t offset = 1; offset <= scan_limit; ++offset) {
      if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, offset)) {
        continue;
      }

      const uint32_t target = before.read_buffer_ptr + offset;
      if (!WriteUInt32AtOffset(message, 0x44, target)) {
        return false;
      }
      if (skipped_bytes != nullptr) {
        *skipped_bytes = offset;
      }
      if (preview != nullptr) {
        wchar_t text[1024]{};
        swprintf_s(
            text,
            L"forward-scan=%u target=%u current-preview=[%s]",
            offset,
            target,
            FormatBytes(bytes.data(), std::min<size_t>(bytes.size(), 128)).c_str());
        *preview = text;
      }
      return true;
    }
  }

  // The current 1.69 HG item trailer mismatch leaves zero padding immediately
  // before the next live submessage. Keep this conservative so a bad parse does
  // not silently jump over real payload bytes.
  if (bytes[0] != 0) {
    return false;
  }

  for (uint32_t offset = 1; offset <= kMaxLegacyPaddingScan && offset < bytes.size(); ++offset) {
    if (bytes[offset - 1] != 0) {
      return false;
    }
    if (!LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(bytes, offset)) {
      continue;
    }

    const uint32_t target = before.read_buffer_ptr + offset;
    if (!WriteUInt32AtOffset(message, 0x44, target)) {
      return false;
    }
    if (skipped_bytes != nullptr) {
      *skipped_bytes = offset;
    }
    return true;
  }

  return false;
}

bool TryRemapLegacyInventoryEquipInitialObjectId(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    uint32_t* remapped_object_id) {
  if (remapped_object_id != nullptr) {
    *remapped_object_id = 0;
  }
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_INVENTORY_EQUIP_OBJECT_REMAP") ||
      g_server_to_player_message_depth <= 0 ||
      g_current_server_to_player_major != 12 ||
      (g_current_server_to_player_minor != 1 && g_current_server_to_player_minor != 2) ||
      message == nullptr ||
      !before.readable ||
      before.read_buffer_ptr > before.read_buffer_size ||
      before.read_buffer_size - before.read_buffer_ptr < 12 ||
      CnwMessageStateOverflow(before)) {
    return false;
  }

  std::vector<uint8_t> bytes;
  const uint32_t preview_length = std::min<uint32_t>(before.read_buffer_size - before.read_buffer_ptr, 16);
  if (!PeekReadBufferBytes(message, before, before.read_buffer_ptr, preview_length, &bytes) || bytes.size() < 12) {
    return false;
  }

  uint32_t legacy_prefix = 0;
  uint32_t object_id = 0;
  uint32_t equip_slot = 0;
  if (!TryReadU32LeFromBytes(bytes, 0, &legacy_prefix) ||
      !TryReadU32LeFromBytes(bytes, 4, &object_id) ||
      !TryReadU32LeFromBytes(bytes, 8, &equip_slot)) {
    return false;
  }

  // 1.69 Inventory_Equip packets seen from HG carry one legacy leading DWORD
  // before the object id. EE 8193.37 expects the object id first, then the
  // normal bool fragment and equip slot DWORD.
  const bool looks_like_legacy_prefix = legacy_prefix <= 0xFFu;
  const bool looks_like_server_object_id = (object_id & 0xFF000000u) == 0x80000000u;
  const bool looks_like_equip_slot = equip_slot != 0 && (equip_slot & 0xFF000000u) == 0;
  if (!looks_like_legacy_prefix || !looks_like_server_object_id || !looks_like_equip_slot) {
    return false;
  }

  const uint32_t target = before.read_buffer_ptr + 8;
  if (!WriteUInt32AtOffset(message, 0x44, target)) {
    return false;
  }

  if (remapped_object_id != nullptr) {
    *remapped_object_id = object_id;
  }

  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_legacy_inventory_equip_object_remap_observations);
  if (observation <= 80 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    LogFormat(
        L"legacy inventory equip object-id remap #%ld: caller=%s dispatch=%ld major=%u/0x%02X minor=%u/0x%02X message=%p legacy-prefix=0x%08X/%u remapped-object=0x%08X/%u equip-slot=0x%08X/%u skipped-buffer-bytes=8 before=[%s] after=[%s] preview=[%s] next-after=[%s]",
        observation,
        FormatMainExecutableAddress(return_address).c_str(),
        g_current_server_to_player_message_observation,
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        message,
        legacy_prefix,
        legacy_prefix,
        object_id,
        object_id,
        equip_slot,
        equip_slot,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatBytes(bytes.data(), bytes.size()).c_str(),
        FormatCnwMessageBufferPreview(message, after, 48).c_str());
  }

  return true;
}

bool ShouldSkipLegacyLiveExtendedArmorTableByte(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    std::wstring* boundary_preview,
    const wchar_t** reason) {
  if (boundary_preview != nullptr) {
    boundary_preview->clear();
  }
  if (reason != nullptr) {
    *reason = L"";
  }
  if (!IsLegacyLiveExtendedArmorTableByteRead(return_address) || !before.readable) {
    return false;
  }

  if (before.read_buffer_ptr >= before.read_buffer_size) {
    if (reason != nullptr) {
      *reason = L"end-of-read-buffer";
    }
    return true;
  }

  if (LooksLikeLegacyLiveObjectSubMessageBoundary(message, before, boundary_preview)) {
    if (reason != nullptr) {
      *reason = L"live-submessage-boundary";
    }
    return true;
  }

  return false;
}

bool ShouldCheckLegacyLiveOversizedCExoString() {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_OVERSIZED_STRING_SKIP") &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1 &&
      g_current_server_to_player_live_opcode_known;
}

bool LooksLikeOversizedLegacyLiveCExoString(
    void* message,
    const CnwMessageReadState& state,
    uint32_t* declared_length,
    uint32_t* available_length,
    uint32_t* boundary_delta,
    std::wstring* header_preview) {
  if (declared_length != nullptr) {
    *declared_length = 0;
  }
  if (available_length != nullptr) {
    *available_length = 0;
  }
  if (boundary_delta != nullptr) {
    *boundary_delta = 0;
  }
  if (header_preview != nullptr) {
    header_preview->clear();
  }
  if (!ShouldCheckLegacyLiveOversizedCExoString() || message == nullptr || !state.readable ||
      state.read_buffer_ptr > state.read_buffer_size || state.read_buffer_size - state.read_buffer_ptr < 4) {
    return false;
  }

  const uint32_t remaining = state.read_buffer_size - state.read_buffer_ptr;
  const uint32_t preview_length = std::min<uint32_t>(remaining, 128);
  std::vector<uint8_t> header;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, preview_length, &header) || header.size() < 4) {
    return false;
  }

  uint32_t length = 0;
  if (!TryReadU32LeFromBytes(header, 0, &length)) {
    return false;
  }

  const uint32_t available = remaining - 4;
  if (length <= available) {
    return false;
  }

  if (declared_length != nullptr) {
    *declared_length = length;
  }
  if (available_length != nullptr) {
    *available_length = available;
  }
  size_t boundary_offset = 0;
  if (FindLegacyLiveObjectSubMessageBoundaryInBytes(header, 4, &boundary_offset) &&
      boundary_offset > 4 &&
      boundary_offset <= remaining) {
    if (boundary_delta != nullptr) {
      *boundary_delta = static_cast<uint32_t>(boundary_offset);
    }
  }
  if (header_preview != nullptr) {
    *header_preview = FormatBytes(header.data(), std::min<size_t>(header.size(), 64));
  }
  return true;
}

bool ShouldRecoverLegacyLiveMisalignedCExoString(void* return_address) {
  UNREFERENCED_PARAMETER(return_address);
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_MISALIGNED_STRING_RECOVER") &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1;
}

bool IsLikelyLegacyLiveAsciiString(const std::vector<uint8_t>& text_bytes) {
  if (text_bytes.empty()) {
    return false;
  }

  uint32_t printable = 0;
  uint32_t strong_text = 0;
  for (const uint8_t value : text_bytes) {
    if ((value >= 0x20 && value <= 0x7E) || value == '\t') {
      ++printable;
    }
    if ((value >= 'A' && value <= 'Z') ||
        (value >= 'a' && value <= 'z') ||
        (value >= '0' && value <= '9') ||
        value == ' ' ||
        value == '-' ||
        value == '_' ||
        value == '\'' ||
        value == '.') {
      ++strong_text;
    }
  }

  return printable == text_bytes.size() && strong_text * 4 >= text_bytes.size() * 3;
}

bool CurrentLegacyLiveCExoStringHeaderIsBad(
    void* message,
    const CnwMessageReadState& state,
    uint32_t* declared_length,
    uint32_t* available_length) {
  if (declared_length != nullptr) {
    *declared_length = 0;
  }
  if (available_length != nullptr) {
    *available_length = 0;
  }
  if (message == nullptr || !state.readable || state.read_buffer_ptr > state.read_buffer_size ||
      state.read_buffer_size - state.read_buffer_ptr < 4) {
    return false;
  }

  std::vector<uint8_t> header;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, 4, &header) || header.size() < 4) {
    return false;
  }

  uint32_t length = 0;
  if (!TryReadU32LeFromBytes(header, 0, &length)) {
    return false;
  }

  const uint32_t available = state.read_buffer_size - state.read_buffer_ptr - 4;
  if (declared_length != nullptr) {
    *declared_length = length;
  }
  if (available_length != nullptr) {
    *available_length = available;
  }

  constexpr uint32_t kMaxNormalLiveStringBytes = 1024;
  return length > available || length > kMaxNormalLiveStringBytes;
}

bool TryRecoverLegacyLiveMisalignedCExoString(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    CExoStringView* result_string,
    CnwMessageReadState* after,
    uint32_t* declared_length,
    uint32_t* available_length,
    uint32_t* recovered_start,
    uint32_t* recovered_end,
    std::string* recovered_text,
    std::wstring* recovered_preview) {
  if (after != nullptr) {
    *after = before;
  }
  if (declared_length != nullptr) {
    *declared_length = 0;
  }
  if (available_length != nullptr) {
    *available_length = 0;
  }
  if (recovered_start != nullptr) {
    *recovered_start = 0;
  }
  if (recovered_end != nullptr) {
    *recovered_end = 0;
  }
  if (recovered_text != nullptr) {
    recovered_text->clear();
  }
  if (recovered_preview != nullptr) {
    recovered_preview->clear();
  }
  if (!ShouldRecoverLegacyLiveMisalignedCExoString(return_address) ||
      message == nullptr ||
      result_string == nullptr ||
      !before.readable ||
      CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr >= before.read_buffer_size) {
    return false;
  }

  uint32_t current_declared = 0;
  uint32_t current_available = 0;
  if (!CurrentLegacyLiveCExoStringHeaderIsBad(
          message,
          before,
          &current_declared,
          &current_available)) {
    return false;
  }

  constexpr int kScanBack = 8;
  constexpr int kScanForward = 12;
  constexpr uint32_t kMaxRecoveredStringBytes = 128;
  int best_score = INT_MIN;
  uint32_t best_start = 0;
  uint32_t best_end = 0;
  std::string best_text;
  std::wstring best_preview;

  for (int delta = -kScanBack; delta <= kScanForward; ++delta) {
    if (delta < 0 && before.read_buffer_ptr < static_cast<uint32_t>(-delta)) {
      continue;
    }

    const uint32_t start = static_cast<uint32_t>(static_cast<int64_t>(before.read_buffer_ptr) + delta);
    if (start > before.read_buffer_size || before.read_buffer_size - start < 4) {
      continue;
    }

    std::vector<uint8_t> header;
    if (!PeekReadBufferBytes(message, before, start, 4, &header) || header.size() < 4) {
      continue;
    }

    uint32_t length = 0;
    if (!TryReadU32LeFromBytes(header, 0, &length) ||
        length == 0 ||
        length > kMaxRecoveredStringBytes ||
        before.read_buffer_size - start - 4 < length) {
      continue;
    }

    const uint32_t text_start = start + 4;
    const uint32_t end = text_start + length;
    if (end <= before.read_buffer_ptr) {
      continue;
    }

    std::vector<uint8_t> text_bytes;
    if (!PeekReadBufferBytes(message, before, text_start, length, &text_bytes) ||
        !IsLikelyLegacyLiveAsciiString(text_bytes)) {
      continue;
    }

    int score = 100 - std::abs(delta) * 5 + static_cast<int>(std::min<uint32_t>(length, 24));
    std::vector<uint8_t> boundary;
    if (end == before.read_buffer_size) {
      score += 20;
    } else {
      const uint32_t boundary_len = std::min<uint32_t>(before.read_buffer_size - end, 8);
      if (boundary_len > 0 &&
          PeekReadBufferBytes(message, before, end, boundary_len, &boundary) &&
          LooksLikeLegacyLiveObjectSubMessageBoundaryBytes(boundary, 0)) {
        score += 70;
      }
    }

    if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateNameStringReturnRva)) {
      score += 20;
    }

    if (score > best_score) {
      best_score = score;
      best_start = start;
      best_end = end;
      best_text.assign(
          reinterpret_cast<const char*>(text_bytes.data()),
          reinterpret_cast<const char*>(text_bytes.data()) + text_bytes.size());
      best_preview = boundary.empty() ? L"" : FormatBytes(boundary.data(), boundary.size());
    }
  }

  if (best_score == INT_MIN || best_text.empty()) {
    return false;
  }

  auto* const init_string =
      ResolveMainExport<CExoStringInitFromCharArrayFn>("?InitFromCharArray@CExoString@@AEAAAEAV1@PEBDI@Z");
  if (!InitCExoStringValue(init_string, result_string, best_text) ||
      !WriteUInt32AtOffset(message, 0x44, best_end)) {
    SetEmptyCExoString(result_string);
    return false;
  }

  if (after != nullptr) {
    *after = ReadCnwMessageReadState(message);
  }
  if (declared_length != nullptr) {
    *declared_length = current_declared;
  }
  if (available_length != nullptr) {
    *available_length = current_available;
  }
  if (recovered_start != nullptr) {
    *recovered_start = best_start;
  }
  if (recovered_end != nullptr) {
    *recovered_end = best_end;
  }
  if (recovered_text != nullptr) {
    *recovered_text = best_text;
  }
  if (recovered_preview != nullptr) {
    *recovered_preview = best_preview;
  }
  return true;
}

void* CreateEmptyVisualTransformMap(void* visual_transform) {
  if (visual_transform == nullptr) {
    return nullptr;
  }

  if (g_visual_transform_tree_buy_head_node == nullptr) {
    auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
    if (base != nullptr) {
      g_visual_transform_tree_buy_head_node =
          reinterpret_cast<VisualTransformTreeBuyHeadNodeFn>(base + kEeVisualTransformTreeBuyHeadNodeRva);
    }
  }
  if (g_visual_transform_tree_buy_head_node == nullptr) {
    return nullptr;
  }

  void* const head = g_visual_transform_tree_buy_head_node(visual_transform);
  if (head == nullptr) {
    return nullptr;
  }

  auto** const map_words = reinterpret_cast<void**>(visual_transform);
  map_words[0] = head;
  map_words[1] = nullptr;
  return visual_transform;
}

bool ShouldUseLegacyLiveItemAppearanceRead() {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_ITEM_APPEARANCE_READ")) {
    return false;
  }

  if (g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1) {
    return true;
  }

  return g_server_to_player_message_depth > 0 &&
      g_current_server_to_player_major == 30 &&
      (g_current_server_to_player_minor == 1 || g_current_server_to_player_minor == 2);
}

void* ResolveBaseItemForLegacyAppearance(uint32_t base_item_id) {
  if (g_get_base_item == nullptr) {
    g_get_base_item =
        ResolveMainExport<GetBaseItemFn>("?GetBaseItem@CNWBaseItemArray@@QEBAPEAVCNWBaseItem@@H@Z");
  }

  void** const rules_global = ResolveMainExport<void**>("?g_pRules@@3PEAVCNWRules@@EA");
  void* rules = nullptr;
  void* base_item_array = nullptr;
  if (g_get_base_item == nullptr ||
      !SafeReadGlobalPointer(rules_global, &rules) ||
      rules == nullptr ||
      !ReadPointerAtOffset(rules, 0x20, &base_item_array) ||
      base_item_array == nullptr) {
    return nullptr;
  }

  __try {
    return g_get_base_item(base_item_array, static_cast<int>(base_item_id));
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return nullptr;
  }
}

CnwMessageReadDwordFn ResolveMessageReadDwordForLegacyAppearance() {
  if (g_message_read_dword_original != nullptr) {
    return g_message_read_dword_original;
  }
  return ResolveMainExport<CnwMessageReadDwordFn>("?ReadDWORD@CNWMessage@@QEAAIH@Z");
}

CnwMessageReadByteFn ResolveMessageReadByteForLegacyAppearance() {
  if (g_message_read_byte_original != nullptr) {
    return g_message_read_byte_original;
  }
  return ResolveMainExport<CnwMessageReadByteFn>("?ReadBYTE@CNWMessage@@QEAAEHH@Z");
}

CnwMessageFlagFn ResolveMessageReadOverflowForLegacyAppearance() {
  if (g_message_read_overflow_original != nullptr) {
    return g_message_read_overflow_original;
  }
  return ResolveMainExport<CnwMessageFlagFn>("?MessageReadOverflow@CNWMessage@@QEAAHXZ");
}

bool MessageReadOverflowForLegacyAppearance(void* message) {
  auto* const overflow = ResolveMessageReadOverflowForLegacyAppearance();
  if (overflow == nullptr) {
    return CnwMessageStateOverflow(ReadCnwMessageReadState(message));
  }
  __try {
    return overflow(message) != 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return true;
  }
}

uint32_t ReadDwordForLegacyAppearance(void* message) {
  auto* const read_dword = ResolveMessageReadDwordForLegacyAppearance();
  if (read_dword == nullptr) {
    return 0;
  }
  __try {
    return read_dword(message, 32);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return 0;
  }
}

uint8_t ReadByteForLegacyAppearance(void* message) {
  auto* const read_byte = ResolveMessageReadByteForLegacyAppearance();
  if (read_byte == nullptr) {
    return 0;
  }
  __try {
    return read_byte(message, 8, 1);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return 0;
  }
}

int64_t __fastcall HookedItemAppearanceRead(
    void* message,
    uint32_t* base_item_out,
    void* model_parts_holder,
    void* colors_holder,
    void* extended_armor_holder,
    uint8_t* model_variation_out,
    void* visual_transform) {
  if (!ShouldUseLegacyLiveItemAppearanceRead()) {
    return g_item_appearance_read_original != nullptr
        ? g_item_appearance_read_original(
              message,
              base_item_out,
              model_parts_holder,
              colors_holder,
              extended_armor_holder,
              model_variation_out,
              visual_transform)
        : 0;
  }

  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  void* model_parts = nullptr;
  void* colors = nullptr;
  void* extended_armor = nullptr;
  ReadPointerAtOffset(model_parts_holder, 0, &model_parts);
  ReadPointerAtOffset(colors_holder, 0, &colors);
  ReadPointerAtOffset(extended_armor_holder, 0, &extended_armor);

  uint32_t base_item_id = ReadDwordForLegacyAppearance(message);
  SafeWriteUInt32(base_item_out, base_item_id);
  if (MessageReadOverflowForLegacyAppearance(message)) {
    const LONG observation = InterlockedIncrement(&g_legacy_live_item_appearance_read_observations);
    LogFormat(
        L"legacy live item-appearance read #%ld failed after base item: message=%p before=[%s] after=[%s] base_item=%u dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s",
        observation,
        message,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(ReadCnwMessageReadState(message)).c_str(),
        base_item_id,
        g_current_server_to_player_message_observation,
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        g_current_server_to_player_size,
        FormatLiveObjectOpcode(
            g_current_server_to_player_live_opcode_known,
            g_current_server_to_player_live_opcode).c_str());
    return 0;
  }

  void* const base_item = ResolveBaseItemForLegacyAppearance(base_item_id);
  uint8_t appearance_type = 0xFF;
  if (base_item == nullptr ||
      !SafeReadUInt8(static_cast<uint8_t*>(base_item) + 0x16, &appearance_type)) {
    const LONG observation = InterlockedIncrement(&g_legacy_live_item_appearance_read_observations);
    LogFormat(
        L"legacy live item-appearance read #%ld failed: missing base item base_item=%u ptr=%p message=%p before=[%s] after=[%s] dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s",
        observation,
        base_item_id,
        base_item,
        message,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(ReadCnwMessageReadState(message)).c_str(),
        g_current_server_to_player_message_observation,
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        g_current_server_to_player_size,
        FormatLiveObjectOpcode(
            g_current_server_to_player_live_opcode_known,
            g_current_server_to_player_live_opcode).c_str());
    return 0;
  }

  auto* const parts_words = static_cast<uint16_t*>(model_parts);
  auto* const color_bytes = static_cast<uint8_t*>(colors);

  bool ok = true;
  const auto read_part_word = [&](size_t index) {
    const uint8_t value = ReadByteForLegacyAppearance(message);
    ok = SafeWriteUInt16(parts_words != nullptr ? parts_words + index : nullptr, value) && ok;
    ok = !MessageReadOverflowForLegacyAppearance(message) && ok;
  };
  const auto read_color_byte = [&](size_t index) {
    const uint8_t value = ReadByteForLegacyAppearance(message);
    ok = SafeWriteUInt8(color_bytes != nullptr ? color_bytes + index : nullptr, value) && ok;
    ok = !MessageReadOverflowForLegacyAppearance(message) && ok;
  };

  if (appearance_type == 0) {
    read_part_word(0);
  } else if (appearance_type == 2) {
    read_part_word(0);
    read_part_word(1);
    read_part_word(2);
  }

  if (appearance_type == 1) {
    read_part_word(0);
    for (size_t index = 0; index < 6; ++index) {
      read_color_byte(index);
    }
  }

  if (appearance_type == 3) {
    for (size_t index = 0; index < 19; ++index) {
      read_part_word(index);
    }
    for (size_t index = 0; index < 6; ++index) {
      read_color_byte(index);
    }
  }

  SafeWriteUInt8(model_variation_out, 0);
  if (appearance_type == 2) {
    const uint8_t value = ReadByteForLegacyAppearance(message);
    ok = SafeWriteUInt8(model_variation_out, value) && ok;
    ok = !MessageReadOverflowForLegacyAppearance(message) && ok;
  }

  const bool zeroed_extended_armor = SafeZeroMemory(extended_armor, 0x72);
  void* const visual_result = CreateEmptyVisualTransformMap(visual_transform);
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_legacy_live_item_appearance_read_observations);
  if (observation <= 220 || !ok || CnwMessageStateOverflow(after)) {
    LogFormat(
        L"legacy live item-appearance read #%ld: result=%d base_item=%u appearance=%u message=%p parts=%p colors=%p extended=%p zeroed_extended=%d visual=%p visual_result=%p before=[%s] after=[%s] next=[%s] dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s live_depth=%d",
        observation,
        ok ? 1 : 0,
        base_item_id,
        static_cast<unsigned int>(appearance_type),
        message,
        model_parts,
        colors,
        extended_armor,
        zeroed_extended_armor ? 1 : 0,
        visual_transform,
        visual_result,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, after, 48).c_str(),
        g_current_server_to_player_message_observation,
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        g_current_server_to_player_size,
        FormatLiveObjectOpcode(
            g_current_server_to_player_live_opcode_known,
            g_current_server_to_player_live_opcode).c_str(),
        g_live_game_object_update_depth);
  }

  return ok ? 1 : 0;
}

void* __fastcall HookedVisualTransformRead(void* visual_transform, void* message) {
  void* const return_address = _ReturnAddress();
  if (ShouldSkipLegacyLiveVisualTransformRead(return_address)) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    void* const result = CreateEmptyVisualTransformMap(visual_transform);
    const LONG observation = InterlockedIncrement(&g_legacy_visual_transform_skip_observations);
    if (observation <= 80 || result == nullptr) {
      LogFormat(
          L"legacy live visual-transform skip #%ld: caller=%s out=%p result=%p message=%p before=[%s] dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s live_depth=%d",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          visual_transform,
          result,
          message,
          FormatCnwMessageReadState(before).c_str(),
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          FormatLiveObjectOpcode(
              g_current_server_to_player_live_opcode_known,
              g_current_server_to_player_live_opcode).c_str(),
          g_live_game_object_update_depth);
    }
    if (result != nullptr) {
      return result;
    }
  }

  return g_visual_transform_read_original != nullptr
      ? g_visual_transform_read_original(visual_transform, message)
      : visual_transform;
}

bool ShouldSkipLegacyModuleHakListBeforeResourceCount(void* return_address) {
  if (!g_auto_connect_ip_known || IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_MODULE_HAK_LIST_SKIP")) {
    return false;
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY")) {
    return false;
  }
  if (g_client_module_load_depth <= 0) {
    return false;
  }

  // EE no longer consumes the 1.69 hak-list block in CNWCModule::LoadModule,
  // but the following DWORD read is still the legacy resource count.
  return ReturnAddressMatchesMainRva(return_address, kEeClientModuleLoadResourceCountReturnRva);
}

int __fastcall HookedServerSatisfiesBuildBridge(void* context, int build, int feature, int revision) {
  void* const return_address = _ReturnAddress();
  const int original = g_server_satisfies_build_bridge_original != nullptr
      ? g_server_satisfies_build_bridge_original(context, build, feature, revision)
      : 0;
  const bool driver_only = IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY");
  const bool force_live_visual_transform =
      !driver_only && ShouldForceLiveVisualTransformFeatureGate(return_address, build, feature, revision);
  const bool force_legacy =
      !driver_only && ShouldForceLegacyServerFeatureGate(return_address, build, feature, revision);
  const int result = force_legacy ? 0 : (force_live_visual_transform ? 1 : original);

  const bool interesting = build == 0x2001 && (feature == 0x0E || (feature >= 0x20 && feature <= 0x25));
  if (force_live_visual_transform || force_legacy || interesting) {
    const LONG observation = InterlockedIncrement(&g_server_satisfies_build_bridge_observations);
    if (observation <= 60 || force_legacy || (force_live_visual_transform && observation <= 240)) {
      const wchar_t* suffix = force_live_visual_transform
          ? L" forced-live-visual-transform-0x23"
          : (force_legacy ? L" forced-for-1.69-packet-parser" : L"");
        LogFormat(
          L"ServerSatisfiesBuild bridge #%ld: context=%p build=0x%X feature=0x%X revision=%d original=%d result=%d caller=%s module_load_depth=%d server_dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s object_depth=%d live_depth=%d%s",
          observation,
          context,
          build,
          feature,
          revision,
          original,
          result,
          FormatMainExecutableAddress(return_address).c_str(),
          g_client_module_load_depth,
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          FormatLiveObjectOpcode(
              g_current_server_to_player_live_opcode_known,
              g_current_server_to_player_live_opcode).c_str(),
          g_game_object_update_depth,
          g_live_game_object_update_depth,
          suffix);
    }
  }

  return result;
}

void LogModuleMessageRead(
    const wchar_t* operation,
    void* message,
    const CnwMessageReadState& before,
    const CnwMessageReadState& after,
    const wchar_t* detail) {
  if (g_client_module_load_depth <= 0 || operation == nullptr) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_module_message_read_observations);
  const bool trace_all_module_reads = IsTruthyEnvironmentFlag(L"HG_BRIDGE_TRACE_ALL_MODULE_READS");
  const long observation_limit = trace_all_module_reads ? 1800 : 160;
  if (observation <= observation_limit ||
      CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before) ||
      CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    LogFormat(
        L"CNWMessage module read #%ld %s: message=%p %s before=[%s] after=[%s]",
        observation,
        operation,
        message,
        detail != nullptr ? detail : L"",
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str());
  }
}

bool ShouldTraceAreaMessageRead(void* return_address) {
  return g_area_read_diagnostics_enabled &&
      g_auto_connect_ip_known &&
      ReturnAddressInMainRvaRange(return_address, kEeClientAreaLoadBeginRva, kEeClientAreaLoadEndRva);
}

bool ShouldTraceLiveMessageRead(void* return_address) {
  if (!g_live_message_read_diagnostics_enabled || !g_auto_connect_ip_known) {
    return false;
  }

  return g_game_object_update_depth > 0 ||
      g_live_game_object_update_depth > 0 ||
      ReturnAddressInMainRvaRange(return_address, kEeClientServerToPlayerMessageBeginRva, kEeClientServerToPlayerMessageEndRva) ||
      ReturnAddressInMainRvaRange(return_address, kEeClientLiveGameObjectUpdateBeginRva, kEeClientLiveGameObjectUpdateEndRva) ||
      ReturnAddressInMainRvaRange(return_address, kEeClientGameObjectUpdateBeginRva, kEeClientGameObjectUpdateEndRva) ||
      ReturnAddressInMainRvaRange(return_address, kEeClientReadCExoLocStringClientBeginRva, kEeClientReadCExoLocStringClientEndRva) ||
      ReturnAddressInMainRvaRange(return_address, kEeClientRawReadDwordRva, kEeClientRawReadDwordEndRva);
}

bool LegacyGameObjectUpdateFieldShimDisabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_OBJECT_UPDATE_FIELDS");
}

bool ShouldForceLegacyGameObjectUpdateField(void* return_address, size_t field_return_rva) {
  return g_auto_connect_ip_known &&
      g_game_object_update_depth > 0 &&
      !LegacyGameObjectUpdateFieldShimDisabled() &&
      ReturnAddressMatchesMainRva(return_address, field_return_rva);
}

bool ShouldForceLegacyGameObjectUpdateBool(void* return_address) {
  return ShouldForceLegacyGameObjectUpdateField(
      return_address,
      kEeClientGameObjectUpdateEePlayerInfoFlagReturnRva);
}

bool ShouldForceLegacyLiveInlineLocStringFlag(void* return_address) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_INLINE_LOCSTRING") ||
      !ReturnAddressMatchesMainRva(
          return_address,
          kEeClientReadCExoLocStringClientTlkFlagReturnRva)) {
    return false;
  }

  if (g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1) {
    return true;
  }

  // HG's 1.69 quickbar packet embeds legacy item data using the same inline
  // CExoLocString payloads as live object updates.
  return g_server_to_player_message_depth > 0 &&
      g_current_server_to_player_major == 30 &&
      (g_current_server_to_player_minor == 1 || g_current_server_to_player_minor == 2);
}

bool ShouldSkipLegacyLiveItemEeOnlyBool(void* return_address) {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_ITEM_EXTRA_BOOL_SKIP") &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1 &&
      ReturnAddressMatchesMainRva(
          return_address,
          kEeClientLiveItemLegacyAbsentExtraBoolReturnRva);
}

bool ShouldSkipLegacyLivePlaceableAddAbsentBool(void* return_address, const wchar_t** skipped_field) {
  if (skipped_field != nullptr) {
    *skipped_field = L"";
  }
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_ABSENT_BOOL_SKIP") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1) {
    return false;
  }

  // EE added an optional-target object id after the first post-name flags and
  // one extra final visual/effect flag before the transform payload. The target
  // flag is handled separately by consuming the legacy bit but returning false;
  // synthesize only the final EE flag here so the legacy transform starts cleanly.
  if (ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameEeOnlyFinalFlagReturnRva)) {
    if (skipped_field != nullptr) {
      *skipped_field = L"ee-only-final-visual";
    }
    return true;
  }
  return false;
}

bool ShouldForceLegacyLivePlaceableAddOptionalTargetFalse(void* return_address) {
  return IsLegacyLivePacketReadContext() &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_ADD_OPTIONAL_TARGET_FORCE_FALSE") &&
      ReturnAddressMatchesMainRva(return_address, kEeClientLivePlaceablePostNameFlag1ReturnRva);
}

bool IdentifyLegacyLivePlaceableUpdateFlagReturn(void* return_address, const wchar_t** skipped_field) {
  if (skipped_field != nullptr) {
    *skipped_field = L"";
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag1ReturnRva)) {
    if (skipped_field != nullptr) {
      *skipped_field = L"update-flag0";
    }
    return true;
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag2ReturnRva)) {
    if (skipped_field != nullptr) {
      *skipped_field = L"update-flag1";
    }
    return true;
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag3ReturnRva)) {
    if (skipped_field != nullptr) {
      *skipped_field = L"update-flag2";
    }
    return true;
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag4ReturnRva)) {
    if (skipped_field != nullptr) {
      *skipped_field = L"update-flag3";
    }
    return true;
  }
  if (ReturnAddressMatchesMainRva(return_address, kEeClientPlaceableUpdateFlag5ReturnRva)) {
    if (skipped_field != nullptr) {
      *skipped_field = L"update-flag4";
    }
    return true;
  }
  return false;
}

bool ShouldSkipLegacyLiveDoorUpdateEeOnlyExtraBool(void* return_address) {
  return IsLegacyLivePacketReadContext() &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_DOOR_UPDATE_EXTRA_BOOL_SKIP") &&
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveDoorUpdateEeOnlyExtraFlagReturnRva);
}

bool ShouldSkipLegacyLivePlaceableUpdateAbsentBool(
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const wchar_t** skipped_field,
    std::wstring* preview) {
  if (preview != nullptr) {
    preview->clear();
  }
  const wchar_t* field = L"";
  if (!IdentifyLegacyLivePlaceableUpdateFlagReturn(return_address, &field)) {
    if (skipped_field != nullptr) {
      *skipped_field = field;
    }
    return false;
  }
  if (skipped_field != nullptr) {
    *skipped_field = field;
  }
  if (!IsLegacyLivePacketReadContext() ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_UPDATE_BOOL_SKIP") ||
      message == nullptr ||
      !before.readable ||
      CnwMessageStateOverflow(before) ||
      before.read_buffer_ptr > before.read_buffer_size) {
    return false;
  }

  uint32_t record_start = 0;
  uint32_t boundary = 0;
  uint32_t object_id = 0;
  uint32_t update_mask = 0;
  std::wstring record_preview;
  if (!TryFindLegacyLiveUpdateRecordBounds(
          message,
          before,
          9,
          &record_start,
          &boundary,
          &object_id,
          &update_mask,
          &record_preview) ||
      boundary <= record_start ||
      boundary > before.read_buffer_size) {
    return false;
  }

  const uint32_t record_length = boundary - record_start;
  std::vector<uint8_t> bytes;
  if (record_length < 10 ||
      !PeekReadBufferBytes(message, before, record_start, record_length, &bytes) ||
      bytes.size() < 10) {
    return false;
  }

  const size_t current_offset = before.read_buffer_ptr > record_start
      ? static_cast<size_t>(before.read_buffer_ptr - record_start)
      : 0;
  size_t name_offset = SIZE_MAX;
  size_t name_end = 0;
  uint32_t name_length = 0;
  std::string name_preview;

  const auto try_name_at = [&](size_t offset) -> bool {
    uint32_t candidate_length = 0;
    size_t candidate_end = 0;
    std::string candidate_preview;
    if (!LooksLikeInlineCExoStringAt(
            bytes,
            offset,
            &candidate_length,
            &candidate_end,
            &candidate_preview)) {
      return false;
    }
    if (candidate_length == 0 && candidate_end != bytes.size()) {
      return false;
    }
    const size_t tail_length = bytes.size() - candidate_end;
    if (tail_length > 32) {
      return false;
    }
    name_offset = offset;
    name_end = candidate_end;
    name_length = candidate_length;
    name_preview = candidate_preview;
    return true;
  };

  if (bytes.size() > 25) {
    try_name_at(25);
  }
  if (name_offset == SIZE_MAX) {
    for (size_t offset = 10; offset + 4 <= bytes.size(); ++offset) {
      if (try_name_at(offset)) {
        break;
      }
    }
  }
  if (name_offset == SIZE_MAX) {
    return false;
  }

  const bool cursor_is_name_or_tail =
      current_offset + 8 >= name_offset &&
      current_offset <= name_end + 8;
  if (!cursor_is_name_or_tail) {
    return false;
  }

  if (preview != nullptr) {
    wchar_t text[1024]{};
    swprintf_s(
        text,
        L"object=0x%08X/%u update-mask=0x%08X record-start=%u boundary=%u current-offset=%zu name-offset=%zu name-len=%u name='%s' record=[%s]",
        object_id,
        object_id,
        update_mask,
        record_start,
        boundary,
        current_offset,
        name_offset,
        name_length,
        NarrowToWide(name_preview.substr(0, 80)).c_str(),
        record_preview.c_str());
    *preview = text;
  }
  return true;
}

bool ShouldForceLegacyGameObjectUpdateByte(void* return_address) {
  return ShouldForceLegacyGameObjectUpdateField(
      return_address,
      kEeClientGameObjectUpdateEePlayerInfoByteReturnRva);
}

bool ShouldForceLegacyGameObjectUpdateString(void* return_address) {
  return ShouldForceLegacyGameObjectUpdateField(
      return_address,
      kEeClientGameObjectUpdateEePlayerInfoStringReturnRva);
}

bool ShouldForceLegacyGameObjectUpdateWord(void* return_address) {
  UNREFERENCED_PARAMETER(return_address);
  return false;
}

bool LegacyObjectUpdateExtraNameConsumeDisabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_OBJECT_UPDATE_EXTRA_NAME_CONSUME");
}

bool LegacyObjectUpdateTailConsumeDisabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_OBJECT_UPDATE_TAIL_CONSUME");
}

bool LegacyObjectUpdateSecondNameConsumeDisabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_OBJECT_UPDATE_SECOND_NAME_CONSUME") ||
      LegacyObjectUpdateTailConsumeDisabled();
}

CnwMessageReadCResRefFn ResolveMessageReadCResRefOriginal() {
  if (g_message_read_cresref_original == nullptr) {
    g_message_read_cresref_original =
        ResolveMainExport<CnwMessageReadCResRefFn>("?ReadCResRef@CNWMessage@@QEAA?AVCResRef@@H@Z");
  }
  return g_message_read_cresref_original;
}

CnwMessageSetReadMessageFn ResolveMessageSetReadMessage() {
  if (g_message_set_read_message == nullptr) {
    g_message_set_read_message =
        ResolveMainExport<CnwMessageSetReadMessageFn>("?SetReadMessage@CNWMessage@@QEAAHPEAEIIH@Z");
  }
  return g_message_set_read_message;
}

CnwMessageCreateWriteMessageFn ResolveMessageCreateWriteMessage() {
  if (g_message_create_write_message_original == nullptr) {
    g_message_create_write_message_original =
        ResolveMainExport<CnwMessageCreateWriteMessageFn>("?CreateWriteMessage@CNWMessage@@QEAAXIIH@Z");
  }
  return g_message_create_write_message_original;
}

CnwMessageGetWriteMessageFn ResolveMessageGetWriteMessage() {
  if (g_message_get_write_message_original == nullptr) {
    g_message_get_write_message_original =
        ResolveMainExport<CnwMessageGetWriteMessageFn>("?GetWriteMessage@CNWMessage@@QEAAHPEAPEAEPEAI@Z");
  }
  return g_message_get_write_message_original;
}

CnwMessageWriteByteFn ResolveMessageWriteByte() {
  if (g_message_write_byte_original == nullptr) {
    g_message_write_byte_original =
        ResolveMainExport<CnwMessageWriteByteFn>("?WriteBYTE@CNWMessage@@QEAAXEHH@Z");
  }
  return g_message_write_byte_original;
}

CnwMessageWriteBoolFn ResolveMessageWriteBool() {
  if (g_message_write_bool_original == nullptr) {
    g_message_write_bool_original =
        ResolveMainExport<CnwMessageWriteBoolFn>("?WriteBOOL@CNWMessage@@QEAAXH@Z");
  }
  return g_message_write_bool_original;
}

CnwMessageWriteDwordFn ResolveMessageWriteDword() {
  if (g_message_write_dword_original == nullptr) {
    g_message_write_dword_original =
        ResolveMainExport<CnwMessageWriteDwordFn>("?WriteDWORD@CNWMessage@@QEAAXIH@Z");
  }
  return g_message_write_dword_original;
}

CnwMessageWriteIntFn ResolveMessageWriteInt() {
  if (g_message_write_int_original == nullptr) {
    g_message_write_int_original =
        ResolveMainExport<CnwMessageWriteIntFn>("?WriteINT@CNWMessage@@QEAAXHH@Z");
  }
  return g_message_write_int_original;
}

CnwMessageWriteWordFn ResolveMessageWriteWord() {
  if (g_message_write_word_original == nullptr) {
    g_message_write_word_original =
        ResolveMainExport<CnwMessageWriteWordFn>("?WriteWORD@CNWMessage@@QEAAXGH@Z");
  }
  return g_message_write_word_original;
}

bool LooksLikeLegacyObjectUpdateExtraName(
    void* message,
    const CnwMessageReadState& state,
    uint32_t* legacy_raw,
    uint32_t* string_length,
    std::string* preview) {
  if (legacy_raw != nullptr) {
    *legacy_raw = 0;
  }
  if (string_length != nullptr) {
    *string_length = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }
  if (LegacyObjectUpdateExtraNameConsumeDisabled() || message == nullptr || !state.readable ||
      state.read_buffer_ptr > state.read_buffer_size || state.read_buffer_size - state.read_buffer_ptr < 8) {
    return false;
  }

  std::vector<uint8_t> header;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, 8, &header)) {
    return false;
  }

  uint32_t raw = 0;
  uint32_t length = 0;
  if (!TryReadU32LeFromBytes(header, 0, &raw) || !TryReadU32LeFromBytes(header, 4, &length)) {
    return false;
  }

  constexpr uint32_t kMaxLegacyExtraNameLength = 128;
  if (length > kMaxLegacyExtraNameLength ||
      state.read_buffer_ptr + 8 > state.read_buffer_size ||
      length > state.read_buffer_size - state.read_buffer_ptr - 8) {
    return false;
  }

  if (length > 0) {
    std::vector<uint8_t> text_bytes;
    if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr + 8, length, &text_bytes)) {
      return false;
    }

    uint32_t printable = 0;
    for (const uint8_t value : text_bytes) {
      if ((value >= 0x20 && value <= 0x7E) || value == '\t') {
        ++printable;
      }
    }
    if (printable * 4 < length * 3) {
      return false;
    }

    if (preview != nullptr) {
      preview->assign(
          reinterpret_cast<const char*>(text_bytes.data()),
          reinterpret_cast<const char*>(text_bytes.data()) + text_bytes.size());
    }
  }

  if (legacy_raw != nullptr) {
    *legacy_raw = raw;
  }
  if (string_length != nullptr) {
    *string_length = length;
  }
  return true;
}

bool LooksLikeLegacyObjectUpdateTail(
    void* message,
    const CnwMessageReadState& state,
    uint32_t* legacy_raw,
    uint16_t* portrait_id,
    CResRefView* portrait_resref) {
  if (legacy_raw != nullptr) {
    *legacy_raw = 0;
  }
  if (portrait_id != nullptr) {
    *portrait_id = 0;
  }
  if (portrait_resref != nullptr) {
    *portrait_resref = CResRefView{};
  }
  if (LegacyObjectUpdateTailConsumeDisabled() || message == nullptr || !state.readable ||
      state.read_buffer_ptr > state.read_buffer_size || state.read_buffer_size - state.read_buffer_ptr < 6) {
    return false;
  }

  std::vector<uint8_t> header;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, 6, &header)) {
    return false;
  }

  uint32_t raw = 0;
  uint16_t portrait = 0;
  if (!TryReadU32LeFromBytes(header, 0, &raw) || !TryReadU16LeFromBytes(header, 4, &portrait)) {
    return false;
  }

  if (portrait >= 0xFFFE) {
    constexpr uint32_t kLegacyPortraitTailBytes = 4 + 2 + kCResRefTextBytes;
    if (state.read_buffer_size - state.read_buffer_ptr < kLegacyPortraitTailBytes) {
      return false;
    }

    std::vector<uint8_t> resref_bytes;
    if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr + 6, kCResRefTextBytes, &resref_bytes)) {
      return false;
    }

    uint32_t printable_or_nul = 0;
    for (const uint8_t value : resref_bytes) {
      if (value == 0 || (value >= 0x20 && value <= 0x7E)) {
        ++printable_or_nul;
      }
    }
    if (printable_or_nul != resref_bytes.size()) {
      return false;
    }
    if (portrait_resref != nullptr) {
      std::memcpy(portrait_resref->data, resref_bytes.data(), kCResRefTextBytes);
      portrait_resref->data[kCResRefTextBytes] = '\0';
    }
  }

  if (legacy_raw != nullptr) {
    *legacy_raw = raw;
  }
  if (portrait_id != nullptr) {
    *portrait_id = portrait;
  }
  return true;
}

bool LooksLikeLegacyObjectUpdateSecondName(
    void* message,
    const CnwMessageReadState& state,
    uint32_t* string_length,
    std::string* preview) {
  if (string_length != nullptr) {
    *string_length = 0;
  }
  if (preview != nullptr) {
    preview->clear();
  }
  if (LegacyObjectUpdateSecondNameConsumeDisabled() || message == nullptr || !state.readable ||
      state.read_buffer_ptr > state.read_buffer_size || state.read_buffer_size - state.read_buffer_ptr < 4) {
    return false;
  }

  std::vector<uint8_t> header;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, 4, &header)) {
    return false;
  }

  uint32_t length = 0;
  if (!TryReadU32LeFromBytes(header, 0, &length)) {
    return false;
  }

  constexpr uint32_t kMaxLegacySecondNameLength = 128;
  if (length > kMaxLegacySecondNameLength ||
      state.read_buffer_ptr + 4 > state.read_buffer_size ||
      length > state.read_buffer_size - state.read_buffer_ptr - 4) {
    return false;
  }

  if (length > 0) {
    std::vector<uint8_t> text_bytes;
    if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr + 4, length, &text_bytes)) {
      return false;
    }

    uint32_t printable = 0;
    for (const uint8_t value : text_bytes) {
      if ((value >= 0x20 && value <= 0x7E) || value == '\t') {
        ++printable;
      }
    }
    if (printable * 4 < length * 3) {
      return false;
    }

    if (preview != nullptr) {
      preview->assign(
          reinterpret_cast<const char*>(text_bytes.data()),
          reinterpret_cast<const char*>(text_bytes.data()) + text_bytes.size());
    }
  }

  if (string_length != nullptr) {
    *string_length = length;
  }
  return true;
}

bool LooksLikeLegacyObjectUpdatePortraitTail(
    void* message,
    const CnwMessageReadState& state,
    uint16_t* portrait_id,
    CResRefView* portrait_resref) {
  if (portrait_id != nullptr) {
    *portrait_id = 0;
  }
  if (portrait_resref != nullptr) {
    *portrait_resref = CResRefView{};
  }
  if (LegacyObjectUpdateTailConsumeDisabled() || message == nullptr || !state.readable ||
      state.read_buffer_ptr > state.read_buffer_size || state.read_buffer_size - state.read_buffer_ptr < 2) {
    return false;
  }

  std::vector<uint8_t> header;
  if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr, 2, &header)) {
    return false;
  }

  uint16_t portrait = 0;
  if (!TryReadU16LeFromBytes(header, 0, &portrait)) {
    return false;
  }

  if (portrait >= 0xFFFE) {
    constexpr uint32_t kLegacyPortraitTailBytes = 2 + kCResRefTextBytes;
    if (state.read_buffer_size - state.read_buffer_ptr < kLegacyPortraitTailBytes) {
      return false;
    }

    std::vector<uint8_t> resref_bytes;
    if (!PeekReadBufferBytes(message, state, state.read_buffer_ptr + 2, kCResRefTextBytes, &resref_bytes)) {
      return false;
    }

    uint32_t printable_or_nul = 0;
    for (const uint8_t value : resref_bytes) {
      if (value == 0 || (value >= 0x20 && value <= 0x7E)) {
        ++printable_or_nul;
      }
    }
    if (printable_or_nul != resref_bytes.size()) {
      return false;
    }
    if (portrait_resref != nullptr) {
      std::memcpy(portrait_resref->data, resref_bytes.data(), kCResRefTextBytes);
      portrait_resref->data[kCResRefTextBytes] = '\0';
    }
  }

  if (portrait_id != nullptr) {
    *portrait_id = portrait;
  }
  return true;
}

bool TryConsumeLegacyObjectUpdateExtraName(
    void* message,
    const CnwMessageReadState& before,
    uint32_t* legacy_raw,
    CExoStringView* legacy_name) {
  if (legacy_raw != nullptr) {
    *legacy_raw = 0;
  }
  if (legacy_name != nullptr) {
    SetEmptyCExoString(legacy_name);
  }
  if (message == nullptr || legacy_name == nullptr || g_raw_read_dword_original == nullptr ||
      g_message_read_cexo_string_original == nullptr) {
    return false;
  }

  uint32_t expected_raw = 0;
  uint32_t expected_length = 0;
  if (!LooksLikeLegacyObjectUpdateExtraName(message, before, &expected_raw, &expected_length, nullptr)) {
    return false;
  }

  __try {
    const uint32_t raw = g_raw_read_dword_original(message);
    CExoStringView* const result = g_message_read_cexo_string_original(message, legacy_name, 32);
    if (legacy_raw != nullptr) {
      *legacy_raw = raw;
    }
    return raw == expected_raw && result != nullptr && SafeCExoStringLength(result) == expected_length;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    SetEmptyCExoString(legacy_name);
    return false;
  }
}

bool TryConsumeLegacyObjectUpdateSecondName(
    void* message,
    const CnwMessageReadState& before,
    CExoStringView* legacy_name) {
  if (legacy_name != nullptr) {
    SetEmptyCExoString(legacy_name);
  }
  if (message == nullptr || legacy_name == nullptr || g_message_read_cexo_string_original == nullptr) {
    return false;
  }

  uint32_t expected_length = 0;
  if (!LooksLikeLegacyObjectUpdateSecondName(message, before, &expected_length, nullptr)) {
    return false;
  }

  __try {
    CExoStringView* const result = g_message_read_cexo_string_original(message, legacy_name, 32);
    return result != nullptr && SafeCExoStringLength(result) == expected_length;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    SetEmptyCExoString(legacy_name);
    return false;
  }
}

bool TryConsumeLegacyObjectUpdatePortraitTail(
    void* message,
    const CnwMessageReadState& before,
    uint16_t* portrait_id,
    CResRefView* portrait_resref) {
  if (portrait_id != nullptr) {
    *portrait_id = 0;
  }
  if (portrait_resref != nullptr) {
    *portrait_resref = CResRefView{};
  }
  if (message == nullptr || g_message_read_word_original == nullptr) {
    return false;
  }

  uint16_t expected_portrait = 0;
  CResRefView expected_resref{};
  if (!LooksLikeLegacyObjectUpdatePortraitTail(message, before, &expected_portrait, &expected_resref)) {
    return false;
  }

  __try {
    const uint16_t portrait = g_message_read_word_original(message, 16);
    CResRefView read_resref{};
    if (portrait >= 0xFFFE) {
      CnwMessageReadCResRefFn read_cresref = ResolveMessageReadCResRefOriginal();
      if (read_cresref == nullptr) {
        return false;
      }
      CResRefView* const result = read_cresref(message, &read_resref, 16);
      if (result == nullptr || std::memcmp(result, &expected_resref, sizeof(expected_resref)) != 0) {
        return false;
      }
      if (portrait_resref != nullptr) {
        *portrait_resref = *result;
      }
    }
    if (portrait_id != nullptr) {
      *portrait_id = portrait;
    }
    return portrait == expected_portrait;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool TryConsumeLegacyObjectUpdateTail(
    void* message,
    const CnwMessageReadState& before,
    uint32_t* legacy_raw,
    uint16_t* portrait_id,
    CResRefView* portrait_resref) {
  if (legacy_raw != nullptr) {
    *legacy_raw = 0;
  }
  if (portrait_id != nullptr) {
    *portrait_id = 0;
  }
  if (portrait_resref != nullptr) {
    *portrait_resref = CResRefView{};
  }
  if (message == nullptr || g_raw_read_dword_original == nullptr || g_message_read_word_original == nullptr) {
    return false;
  }

  uint32_t expected_raw = 0;
  uint16_t expected_portrait = 0;
  CResRefView expected_resref{};
  if (!LooksLikeLegacyObjectUpdateTail(message, before, &expected_raw, &expected_portrait, &expected_resref)) {
    return false;
  }

  __try {
    const uint32_t raw = g_raw_read_dword_original(message);
    const uint16_t portrait = g_message_read_word_original(message, 16);
    CResRefView read_resref{};
    if (portrait >= 0xFFFE) {
      CnwMessageReadCResRefFn read_cresref = ResolveMessageReadCResRefOriginal();
      if (read_cresref == nullptr) {
        return false;
      }
      CResRefView* const result = read_cresref(message, &read_resref, 16);
      if (result == nullptr || std::memcmp(result, &expected_resref, sizeof(expected_resref)) != 0) {
        return false;
      }
      if (portrait_resref != nullptr) {
        *portrait_resref = *result;
      }
    }
    if (legacy_raw != nullptr) {
      *legacy_raw = raw;
    }
    if (portrait_id != nullptr) {
      *portrait_id = portrait;
    }
    return raw == expected_raw && portrait == expected_portrait;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool ShouldForceLegacyAreaNameStringFlag(void* return_address) {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY") &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_AREA_NAME_STRING_FLAG") &&
      ReturnAddressMatchesMainRva(return_address, kEeClientAreaLoadNameStringFlagReturnRva);
}

bool IsLegacyAreaStreamExhaustedForEeTail(const CnwMessageReadState& state) {
  if (!state.readable || state.read_buffer_ptr < state.read_buffer_size ||
      state.read_fragments_size < state.read_fragments_ptr) {
    return false;
  }

  const uint32_t fragment_bytes_remaining = state.read_fragments_size - state.read_fragments_ptr;
  return fragment_bytes_remaining == 0 ||
      (fragment_bytes_remaining == 1 && state.read_fragments_bit_ptr >= state.read_fragments_bit_size);
}

bool ShouldForceLegacyAreaPostStaticCount(void* return_address, const CnwMessageReadState& before) {
  return g_auto_connect_ip_known &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY") &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_AREA_POST_STATIC_COUNTS") &&
      IsLegacyAreaStreamExhaustedForEeTail(before) &&
      (ReturnAddressMatchesMainRva(return_address, kEeClientAreaPostStaticSoundCountReturnRva) ||
       ReturnAddressMatchesMainRva(return_address, kEeClientAreaPostStaticCreatureCountReturnRva));
}

void LogAreaMessageRead(
    const wchar_t* operation,
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const CnwMessageReadState& after,
    const wchar_t* detail) {
  if (operation == nullptr) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_area_message_read_observations);
  size_t log_limit = 260;
  const std::wstring configured_limit = GetEnvironmentString(L"HG_BRIDGE_AREA_READ_LOG_LIMIT");
  if (!configured_limit.empty()) {
    wchar_t* end = nullptr;
    const unsigned long parsed = wcstoul(configured_limit.c_str(), &end, 10);
    if (end != configured_limit.c_str() && parsed > 0) {
      log_limit = static_cast<size_t>(parsed);
    }
  }
  const bool trace_all_area_reads = IsTruthyEnvironmentFlag(L"HG_BRIDGE_TRACE_ALL_AREA_READS");
  const bool bounds_transition =
      (!CnwMessageStateOverflow(before) && CnwMessageStateOverflow(after)) ||
      (!CnwMessageStateUnderflow(before) && CnwMessageStateUnderflow(after));
  if (observation <= static_cast<LONG>(log_limit) || trace_all_area_reads || bounds_transition) {
    LogFormat(
        L"CNWMessage area read #%ld %s: caller=%s message=%p %s pointers=[%s] before=[%s] after=[%s] next-before=[%s]",
        observation,
        operation,
        FormatMainExecutableAddress(return_address).c_str(),
        message,
        detail != nullptr ? detail : L"",
        FormatCnwMessageReadPointers(message).c_str(),
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, before).c_str());
  }
}

void LogLiveMessageRead(
    const wchar_t* operation,
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const CnwMessageReadState& after,
    const wchar_t* detail) {
  if (operation == nullptr) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_live_message_read_observations);
  const bool focused_live_object_trace =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_TRACE_FOCUSED_LIVE_READS") &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1 &&
      g_current_server_to_player_size > 0 &&
      g_current_server_to_player_size <= 900;
  const bool trace_all_live_reads = IsTruthyEnvironmentFlag(L"HG_BRIDGE_TRACE_ALL_LIVE_READS");
  const bool bounds_transition =
      (!CnwMessageStateOverflow(before) && CnwMessageStateOverflow(after)) ||
      (!CnwMessageStateUnderflow(before) && CnwMessageStateUnderflow(after));
  if (observation <= 420 || focused_live_object_trace || trace_all_live_reads ||
      bounds_transition) {
    const std::wstring opcode =
        FormatLiveObjectOpcode(g_current_server_to_player_live_opcode_known, g_current_server_to_player_live_opcode);
    LogFormat(
        L"CNWMessage live read #%ld %s: caller=%s player_depth=%d live_depth=%d dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s message=%p %s pointers=[%s] before=[%s] after=[%s] next-before=[%s]",
        observation,
        operation,
        FormatMainExecutableAddress(return_address).c_str(),
        g_game_object_update_depth,
        g_live_game_object_update_depth,
        g_current_server_to_player_message_observation,
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_major),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        static_cast<unsigned int>(g_current_server_to_player_minor),
        g_current_server_to_player_size,
        opcode.c_str(),
        message,
        detail != nullptr ? detail : L"",
        FormatCnwMessageReadPointers(message).c_str(),
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, before).c_str());
  }
}

void LogLegacyObjectUpdateFieldShim(
    const wchar_t* operation,
    void* return_address,
    void* message,
    const CnwMessageReadState& before,
    const CnwMessageReadState& after,
    const wchar_t* detail) {
  const LONG observation = InterlockedIncrement(&g_legacy_object_update_field_observations);
  if (observation <= 160) {
    LogFormat(
        L"legacy object-update field shim #%ld %s: caller=%s message=%p %s before=[%s] after=[%s] next-before=[%s]",
        observation,
        operation != nullptr ? operation : L"<unknown>",
        FormatMainExecutableAddress(return_address).c_str(),
        message,
        detail != nullptr ? detail : L"",
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, before).c_str());
  }
}

std::wstring FormatLiveDispatchContext() {
  if (g_server_to_player_message_depth <= 0) {
    return L"";
  }

  const std::wstring opcode =
      FormatLiveObjectOpcode(g_current_server_to_player_live_opcode_known, g_current_server_to_player_live_opcode);
  wchar_t text[224]{};
  swprintf_s(
      text,
      L" dispatch=#%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s",
      g_current_server_to_player_message_observation,
      static_cast<unsigned int>(g_current_server_to_player_major),
      static_cast<unsigned int>(g_current_server_to_player_major),
      static_cast<unsigned int>(g_current_server_to_player_minor),
      static_cast<unsigned int>(g_current_server_to_player_minor),
      g_current_server_to_player_size,
      opcode.c_str());
  return text;
}

bool ShouldConsumeLegacyLiveObjectUpdateTail(
    void* return_address,
    const CnwMessageReadState& state) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_OBJECT_UPDATE_TAIL_CONSUME")) {
    return false;
  }
  if (g_server_to_player_message_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1) {
    return false;
  }
  if (!ReturnAddressMatchesMainRva(return_address, kEeClientServerToPlayerUnderflowCheckReturnRva)) {
    return false;
  }

  const bool exact_legacy_tail =
      state.readable &&
      g_current_server_to_player_size == 218 &&
      state.read_buffer_size == 212 &&
      state.read_buffer_ptr == 77 &&
      state.read_fragments_size == 3 &&
      state.read_fragments_ptr == 0 &&
      state.read_fragments_bit_size == 3 &&
      state.read_fragments_bit_ptr == 3;
  if (exact_legacy_tail) {
    return true;
  }

  if (state.readable &&
      state.read_buffer_ptr == state.read_buffer_size &&
      state.read_fragments_ptr < state.read_fragments_size &&
      CnwMessageStateUnderflow(state) &&
      !CnwMessageStateOverflow(state) &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_STRICT_LEGACY_LIVE_OBJECT_UPDATE_TAIL_CONSUME")) {
    return true;
  }

  if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_BROAD_LEGACY_LIVE_OBJECT_UPDATE_TAIL_CONSUME") ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_STRICT_LEGACY_LIVE_OBJECT_UPDATE_TAIL_CONSUME")) {
    return false;
  }

  // Opt-in only. This is useful for experiments, but it can skip hundreds of
  // unread bytes in valid HG live packets and corrupt the object stream.
  return state.readable &&
      CnwMessageStateUnderflow(state) &&
      !CnwMessageStateOverflow(state);
}

bool ShouldConsumeLegacyObjectUpdateFragmentTail(
    void* return_address,
    const CnwMessageReadState& state) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_OBJECT_UPDATE_FRAGMENT_TAIL_CONSUME")) {
    return false;
  }
  if (g_server_to_player_message_depth <= 0 ||
      g_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 10 ||
      g_current_server_to_player_minor != 1) {
    return false;
  }
  if (!ReturnAddressMatchesMainRva(return_address, kEeClientGameObjectUpdateUnderflowCheckReturnRva)) {
    return false;
  }
  return state.readable &&
      state.read_buffer_ptr == state.read_buffer_size &&
      state.read_fragments_ptr < state.read_fragments_size &&
      CnwMessageStateUnderflow(state) &&
      !CnwMessageStateOverflow(state);
}

bool TryConsumeLegacyObjectUpdateFragmentTail(
    void* message,
    void* return_address,
    const CnwMessageReadState& before) {
  const std::wstring tail_preview = FormatCnwMessageBufferPreview(message, before, 48);
  const bool written =
      WriteUInt32AtOffset(message, 0x44, before.read_buffer_size) &&
      WriteUInt32AtOffset(message, 0x54, before.read_fragments_size) &&
      WriteUInt8AtOffset(message, 0x5C, before.read_fragments_bit_size);
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_legacy_object_update_fragment_tail_observations);
  LogFormat(
      L"legacy object-update fragment tail shim #%ld: caller=%s message=%p %s before=[%s] after=[%s] tail=[%s]",
      observation,
      FormatMainExecutableAddress(return_address).c_str(),
      message,
      written ? L"consumed" : L"failed",
      FormatCnwMessageReadState(before).c_str(),
      FormatCnwMessageReadState(after).c_str(),
      tail_preview.c_str());
  return written && !CnwMessageStateUnderflow(after) && !CnwMessageStateOverflow(after);
}

bool TryConsumeLegacyLiveObjectUpdateTail(
    void* message,
    void* return_address,
    const CnwMessageReadState& before) {
  const std::wstring tail_preview = FormatCnwMessageBufferPreview(message, before, 48);
  const std::wstring dispatch_context = FormatLiveDispatchContext();
  const bool written =
      WriteUInt32AtOffset(message, 0x44, before.read_buffer_size) &&
      WriteUInt32AtOffset(message, 0x54, before.read_fragments_size) &&
      WriteUInt8AtOffset(message, 0x5C, before.read_fragments_bit_size);
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_legacy_live_object_update_tail_observations);
  LogFormat(
      L"legacy live object-update tail shim #%ld: caller=%s message=%p %s before=[%s] after=[%s] tail=[%s]%s",
      observation,
      FormatMainExecutableAddress(return_address).c_str(),
      message,
      written ? L"consumed" : L"failed",
      FormatCnwMessageReadState(before).c_str(),
      FormatCnwMessageReadState(after).c_str(),
      tail_preview.c_str(),
      dispatch_context.c_str());
  return written && !CnwMessageStateUnderflow(after) && !CnwMessageStateOverflow(after);
}

bool ShouldSkipLegacyLiveGenericObjectUpdateTrailingWord(
    void* return_address,
    int bit_count,
    const CnwMessageReadState& state) {
  if (!g_auto_connect_ip_known ||
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_TRAILING_WORD_SKIP") ||
      g_server_to_player_message_depth <= 0 ||
      g_live_game_object_update_depth <= 0 ||
      g_current_server_to_player_major != 5 ||
      g_current_server_to_player_minor != 1 ||
      bit_count != 16 ||
      !ReturnAddressMatchesMainRva(
          return_address,
          kEeClientLiveGenericObjectUpdateTrailingWordReturnRva) ||
      !state.readable ||
      CnwMessageStateOverflow(state) ||
      state.read_buffer_ptr > state.read_buffer_size) {
    return false;
  }

  const uint32_t remaining = state.read_buffer_size - state.read_buffer_ptr;
  return remaining < 2;
}

bool TrySkipLegacyLiveGenericObjectUpdateTrailingWord(
    void* message,
    void* return_address,
    const CnwMessageReadState& before,
    CnwMessageReadState* after) {
  if (after != nullptr) {
    *after = before;
  }

  const std::wstring tail_preview = FormatCnwMessageBufferPreview(message, before, 48);
  const std::wstring dispatch_context = FormatLiveDispatchContext();
  const bool written =
      WriteUInt32AtOffset(message, 0x44, before.read_buffer_size) &&
      WriteUInt32AtOffset(message, 0x54, before.read_fragments_size) &&
      WriteUInt8AtOffset(message, 0x5C, before.read_fragments_bit_size);
  const CnwMessageReadState updated = ReadCnwMessageReadState(message);
  if (after != nullptr) {
    *after = updated;
  }

  const LONG observation =
      InterlockedIncrement(&g_legacy_live_generic_update_trailing_word_skip_observations);
  LogFormat(
      L"legacy live generic-update trailing word skip #%ld: caller=%s message=%p %s before=[%s] after=[%s] tail=[%s]%s",
      observation,
      FormatMainExecutableAddress(return_address).c_str(),
      message,
      written ? L"consumed-to-end result=0" : L"failed result=0",
      FormatCnwMessageReadState(before).c_str(),
      FormatCnwMessageReadState(updated).c_str(),
      tail_preview.c_str(),
      dispatch_context.c_str());
  return written && !CnwMessageStateUnderflow(updated) && !CnwMessageStateOverflow(updated);
}

int __fastcall HookedMessageReadOverflow(void* message) {
  void* const return_address = _ReturnAddress();
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState guard_before = ReadCnwMessageReadState(message);
  std::wstring guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          guard_before,
          L"MessageReadOverflow",
          &guard_detail)) {
    return 1;
  }
  const int result = g_message_read_overflow_original != nullptr
      ? g_message_read_overflow_original(message)
      : (CnwMessageStateOverflow(ReadCnwMessageReadState(message)) ? 1 : 0);
  if (g_client_module_load_depth > 0 || trace_area || trace_live || result != 0) {
    const LONG observation = InterlockedIncrement(&g_module_message_bounds_observations);
    const bool trace_all_live_reads = IsTruthyEnvironmentFlag(L"HG_BRIDGE_TRACE_ALL_LIVE_READS");
    if (observation <= 120 || trace_area || trace_all_live_reads || (result != 0 && observation <= 400)) {
      const CnwMessageReadState state = ReadCnwMessageReadState(message);
      const std::wstring dispatch_context = FormatLiveDispatchContext();
      LogFormat(
          L"CNWMessage bounds #%ld MessageReadOverflow: caller=%s message=%p result=%d state=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          result,
          FormatCnwMessageReadState(state).c_str(),
          dispatch_context.c_str());
      if (result != 0 && (g_client_module_load_depth > 0 || trace_live)) {
        LogCnwMessageUnreadBytes(L"CNWMessage overflow detail", message, state);
      }
    }
    if (trace_live) {
      const LONG live_observation = InterlockedIncrement(&g_live_message_bounds_observations);
      if (live_observation <= 180 || trace_all_live_reads || (result != 0 && live_observation <= 400)) {
        const CnwMessageReadState state = ReadCnwMessageReadState(message);
        const std::wstring dispatch_context = FormatLiveDispatchContext();
        LogFormat(
            L"CNWMessage live bounds #%ld MessageReadOverflow: caller=%s player_depth=%d live_depth=%d message=%p result=%d state=[%s]%s",
            live_observation,
            FormatMainExecutableAddress(return_address).c_str(),
            g_game_object_update_depth,
            g_live_game_object_update_depth,
            message,
            result,
            FormatCnwMessageReadState(state).c_str(),
            dispatch_context.c_str());
      }
    }
  }
  return result;
}

int __fastcall HookedMessageReadUnderflow(void* message) {
  void* const return_address = _ReturnAddress();
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  std::wstring guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          before,
          L"MessageReadUnderflow",
          &guard_detail)) {
    return 1;
  }
  if (ShouldConsumeLegacyObjectUpdateFragmentTail(return_address, before) &&
      TryConsumeLegacyObjectUpdateFragmentTail(message, return_address, before)) {
    return 0;
  }
  if (ShouldConsumeLegacyLiveObjectUpdateTail(return_address, before) &&
      TryConsumeLegacyLiveObjectUpdateTail(message, return_address, before)) {
    return 0;
  }
  const int result = g_message_read_underflow_original != nullptr
      ? g_message_read_underflow_original(message)
      : (CnwMessageStateUnderflow(before) ? 1 : 0);
  if (g_client_module_load_depth > 0 || trace_area || trace_live || result != 0) {
    const LONG observation = InterlockedIncrement(&g_module_message_bounds_observations);
    const bool trace_all_live_reads = IsTruthyEnvironmentFlag(L"HG_BRIDGE_TRACE_ALL_LIVE_READS");
    if (observation <= 120 || trace_area || trace_all_live_reads || (result != 0 && observation <= 400)) {
      const CnwMessageReadState state = ReadCnwMessageReadState(message);
      const std::wstring dispatch_context = FormatLiveDispatchContext();
      LogFormat(
          L"CNWMessage bounds #%ld MessageReadUnderflow: caller=%s message=%p result=%d state=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          result,
          FormatCnwMessageReadState(state).c_str(),
          dispatch_context.c_str());
      if (result != 0 && (g_client_module_load_depth > 0 || trace_live)) {
        LogCnwMessageUnreadBytes(L"CNWMessage underflow detail", message, state);
      }
    }
    if (trace_live) {
      const LONG live_observation = InterlockedIncrement(&g_live_message_bounds_observations);
      if (live_observation <= 180 || trace_all_live_reads || (result != 0 && live_observation <= 400)) {
        const CnwMessageReadState state = ReadCnwMessageReadState(message);
        const std::wstring dispatch_context = FormatLiveDispatchContext();
        LogFormat(
            L"CNWMessage live bounds #%ld MessageReadUnderflow: caller=%s player_depth=%d live_depth=%d message=%p result=%d state=[%s]%s",
            live_observation,
            FormatMainExecutableAddress(return_address).c_str(),
            g_game_object_update_depth,
            g_live_game_object_update_depth,
            message,
            result,
            FormatCnwMessageReadState(state).c_str(),
            dispatch_context.c_str());
      }
    }
  }
  return result;
}

int __fastcall HookedMessageReadBool(void* message) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const bool force_area_name_string = ShouldForceLegacyAreaNameStringFlag(return_address);
  const bool force_legacy_object_update_field = ShouldForceLegacyGameObjectUpdateBool(return_address);
  const bool skip_legacy_live_item_ee_only_bool = ShouldSkipLegacyLiveItemEeOnlyBool(return_address);
  const bool skip_legacy_live_door_update_extra_bool =
      ShouldSkipLegacyLiveDoorUpdateEeOnlyExtraBool(return_address);
  const bool force_legacy_live_placeable_add_optional_target_false =
      ShouldForceLegacyLivePlaceableAddOptionalTargetFalse(return_address);
  const wchar_t* legacy_live_placeable_absent_bool_field = L"";
  const bool skip_legacy_live_placeable_absent_bool =
      ShouldSkipLegacyLivePlaceableAddAbsentBool(
          return_address,
          &legacy_live_placeable_absent_bool_field);
  const wchar_t* legacy_live_placeable_bool_field = L"";
  const bool trace_legacy_live_placeable_bool_field =
      ShouldTraceLegacyLivePlaceableBoolField(return_address, &legacy_live_placeable_bool_field);
  const bool force_legacy_live_inline_locstring =
      ShouldForceLegacyLiveInlineLocStringFlag(return_address);
  const bool trace_locstring_client_tlk_flag =
      g_auto_connect_ip_known &&
      ReturnAddressMatchesMainRva(return_address, kEeClientReadCExoLocStringClientTlkFlagReturnRva);

  CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          boundary_guard_before,
          L"ReadBOOL",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"result=0x00000000/0 %s",
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadBOOL",
          return_address,
          message,
          boundary_guard_before,
          boundary_guard_before,
          detail);
    }
    return 0;
  }

  if (force_area_name_string) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const int original_result = g_message_read_bool_original != nullptr ? g_message_read_bool_original(message) : 0;
    const CnwMessageReadState after = ReadCnwMessageReadState(message);
    wchar_t detail[128]{};
    swprintf_s(
        detail,
        L"result=0x00000001/1 forced-legacy-area-name-string consumed-original=0x%08X/%u",
        original_result,
        original_result);
    if (trace_module) {
      LogModuleMessageRead(L"ReadBOOL", message, before, after, detail);
    }
    LogAreaMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    if (trace_live) {
      LogLiveMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    }
    return 1;
  }

  {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const wchar_t* legacy_name_label = L"object";
    uint8_t legacy_name_object_type = 0;
    size_t legacy_name_boundary_delta = 0;
    std::wstring legacy_name_preview;
    if (TryIdentifyLegacyLiveNameLocStringFlag(
            return_address,
            message,
            before,
            &legacy_name_label,
            &legacy_name_object_type,
            &legacy_name_boundary_delta,
            &legacy_name_preview)) {
      const int original_result =
          g_message_read_bool_original != nullptr ? g_message_read_bool_original(message) : 0;
      const CnwMessageReadState after = ReadCnwMessageReadState(message);
      uint32_t legacy_name_strref = 0;
      std::vector<uint8_t> legacy_name_bytes;
      if (after.readable &&
          after.read_buffer_ptr <= after.read_buffer_size &&
          after.read_buffer_size - after.read_buffer_ptr >= 4 &&
          PeekReadBufferBytes(message, after, after.read_buffer_ptr, 4, &legacy_name_bytes)) {
        TryReadU32LeFromBytes(legacy_name_bytes, 0, &legacy_name_strref);
      }
      const bool queued_short_add_parse =
          QueuePendingLegacyLiveShortAddParse(
              message,
              after,
              legacy_name_object_type,
              static_cast<uint32_t>(legacy_name_boundary_delta),
              legacy_name_strref);
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_name_locstring_flag_observations);
      if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
        LogFormat(
            L"legacy live %s name locstring flag force #%ld: caller=%s type=%u original=0x%08X/%u result=1 strref=0x%08X/%u queued=%d boundary-delta=%zu before=[%s] after=[%s] preview=[%s]%s",
            legacy_name_label != nullptr ? legacy_name_label : L"object",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            static_cast<unsigned int>(legacy_name_object_type),
            original_result,
            original_result,
            legacy_name_strref,
            legacy_name_strref,
            queued_short_add_parse ? 1 : 0,
            legacy_name_boundary_delta,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageReadState(after).c_str(),
            legacy_name_preview.c_str(),
            FormatLiveDispatchContext().c_str());
      }
      wchar_t detail[256]{};
      swprintf_s(
          detail,
          L"result=0x00000001/1 forced-1.69-live-%s-name-locstring consumed-original=0x%08X/%u boundary-delta=%zu",
          legacy_name_label != nullptr ? legacy_name_label : L"object",
          original_result,
          original_result,
          legacy_name_boundary_delta);
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, after, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
      }
      return 1;
    }
  }

  {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const wchar_t* short_add_label = L"object";
    uint32_t short_add_strref = 0;
    uint32_t short_add_boundary_delta = 0;
    if (TryForceLegacyLiveShortAddLocStringInnerFlag(
            return_address,
            message,
            before,
            &short_add_label,
            &short_add_strref,
            &short_add_boundary_delta)) {
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_short_add_locstring_flag_observations);
      if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
        LogFormat(
            L"legacy live short %s add locstring inner flag #%ld: caller=%s result=0 no-consume strref=0x%08X/%u boundary-delta=%u before=[%s] next=[%s]%s",
            short_add_label != nullptr ? short_add_label : L"object",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            short_add_strref,
            short_add_strref,
            short_add_boundary_delta,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageBufferPreview(message, before, 48).c_str(),
            FormatLiveDispatchContext().c_str());
      }
      const wchar_t* detail = L"result=0x00000000/0 forced-1.69-short-add-locstring-mode no-consume";
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      return 0;
    }

    uint32_t short_add_boundary_remaining = 0;
    if (TryForceLegacyLiveShortAddTailBool(
            return_address,
            message,
            before,
            &short_add_label,
            &short_add_boundary_remaining)) {
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_short_add_tail_bool_observations);
      if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
        LogFormat(
            L"legacy live short %s add tail bool #%ld: caller=%s result=0 no-consume boundary-remaining=%u before=[%s] next=[%s]%s",
            short_add_label != nullptr ? short_add_label : L"object",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            short_add_boundary_remaining,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageBufferPreview(message, before, 48).c_str(),
            FormatLiveDispatchContext().c_str());
      }
      const wchar_t* detail = L"result=0x00000000/0 forced-1.69-short-add-tail-bool no-consume";
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      return 0;
    }
  }

  {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    uint32_t trigger_add_strref = 0;
    uint32_t trigger_add_boundary_delta = 0;
    if (TryForceLegacyLiveTriggerAddLocStringInnerFlag(
            return_address,
            message,
            before,
            &trigger_add_strref,
            &trigger_add_boundary_delta)) {
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_trigger_add_locstring_flag_observations);
      if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
        LogFormat(
            L"legacy live trigger add locstring inner flag #%ld: caller=%s result=0 no-consume strref=0x%08X/%u boundary-delta=%u before=[%s] next=[%s]%s",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            trigger_add_strref,
            trigger_add_strref,
            trigger_add_boundary_delta,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageBufferPreview(message, before, 48).c_str(),
            FormatLiveDispatchContext().c_str());
      }
      const wchar_t* detail = L"result=0x00000000/0 forced-1.69-trigger-add-locstring-mode no-consume";
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      return 0;
    }

    uint8_t trigger_add_primary_result = 0;
    uint8_t trigger_add_primary_flag = 0;
    uint32_t trigger_add_primary_boundary_remaining = 0;
    CnwMessageReadState trigger_add_primary_after = before;
    if (TryConsumeLegacyLiveTriggerAddPrimaryBool(
            return_address,
            message,
            before,
            &trigger_add_primary_result,
            &trigger_add_primary_after,
            &trigger_add_primary_boundary_remaining,
            &trigger_add_primary_flag)) {
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_trigger_add_primary_bool_observations);
      if (observation <= 240 || CnwMessageStateOverflow(trigger_add_primary_after) ||
          CnwMessageStateUnderflow(trigger_add_primary_after)) {
        LogFormat(
            L"legacy live trigger add primary bool #%ld: caller=%s result=0x%02X/%u consumed-legacy-byte=0x%02X boundary-remaining=%u before=[%s] after=[%s] next=[%s]%s",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            static_cast<unsigned int>(trigger_add_primary_result),
            static_cast<unsigned int>(trigger_add_primary_result),
            static_cast<unsigned int>(trigger_add_primary_flag),
            trigger_add_primary_boundary_remaining,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageReadState(trigger_add_primary_after).c_str(),
            FormatCnwMessageBufferPreview(message, trigger_add_primary_after, 48).c_str(),
            FormatLiveDispatchContext().c_str());
      }
      wchar_t detail[160]{};
      swprintf_s(
          detail,
          L"result=0x%08X/%u consumed-1.69-trigger-primary-byte",
          static_cast<unsigned int>(trigger_add_primary_result),
          static_cast<unsigned int>(trigger_add_primary_result));
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, trigger_add_primary_after, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, trigger_add_primary_after, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, trigger_add_primary_after, detail);
      }
      return trigger_add_primary_result != 0 ? 1 : 0;
    }

    const wchar_t* trigger_add_field = L"";
    uint32_t trigger_add_boundary_remaining = 0;
    if (TryForceLegacyLiveTriggerAddSyntheticBool(
            return_address,
            message,
            before,
            &trigger_add_field,
            &trigger_add_boundary_remaining)) {
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_trigger_add_synthetic_bool_observations);
      if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
        LogFormat(
            L"legacy live trigger add synthetic bool #%ld: field=%s caller=%s result=0 no-consume boundary-remaining=%u before=[%s] next=[%s]%s",
            observation,
            trigger_add_field != nullptr ? trigger_add_field : L"",
            FormatMainExecutableAddress(return_address).c_str(),
            trigger_add_boundary_remaining,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageBufferPreview(message, before, 48).c_str(),
            FormatLiveDispatchContext().c_str());
      }
      const wchar_t* detail = L"result=0x00000000/0 synthetic-1.69-trigger-add-bool no-consume";
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      return 0;
    }
  }

  {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    uint8_t update_object_type = 0;
    const wchar_t* update_label = L"object";
    uint32_t update_object_id = 0;
    uint32_t update_mask = 0;
    uint32_t boundary_delta = 0;
    std::wstring update_preview;
    if (TryForceLegacyLiveUpdateAbsentNameModeFlag(
            return_address,
            message,
            before,
            &update_object_type,
            &update_label,
            &update_object_id,
            &update_mask,
            &boundary_delta,
            &update_preview)) {
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_update_absent_name_bool_skip_observations);
      if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
        LogFormat(
            L"legacy live %s update absent-name mode bool skip #%ld: caller=%s object=0x%08X/%u type=%u update-mask=0x%08X result=0 no-consume boundary-delta=%u before=[%s] next=[%s] record=[%s]%s",
            update_label != nullptr ? update_label : L"object",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            update_object_id,
            update_object_id,
            static_cast<unsigned int>(update_object_type),
            update_mask,
            boundary_delta,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageBufferPreview(message, before, 48).c_str(),
            update_preview.c_str(),
            FormatLiveDispatchContext().c_str());
      }
      const wchar_t* detail =
          L"result=0x00000000/0 skipped-1.69-live-update-absent-name-mode no-consume";
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      return 0;
    }
  }

  if (skip_legacy_live_door_update_extra_bool) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const wchar_t* detail =
        L"result=0x00000000/0 skipped-1.69-door-update-ee-only-bool no-consume";
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_door_update_extra_bool_skip_observations);
    if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
      LogFormat(
          L"legacy live door update EE-only extra bool skip #%ld: caller=%s message=%p result=0 no-consume before=[%s] next=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageBufferPreview(message, before, 48).c_str(),
          FormatLiveDispatchContext().c_str());
    }
    if (trace_module) {
      LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
    }
    if (trace_area) {
      LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
    }
    if (trace_live) {
      LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
    }
    return 0;
  }

  {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const wchar_t* update_bool_field = L"";
    std::wstring update_bool_preview;
    if (ShouldSkipLegacyLivePlaceableUpdateAbsentBool(
            return_address,
            message,
            before,
            &update_bool_field,
            &update_bool_preview)) {
      const wchar_t* detail =
          L"result=0x00000000/0 skipped-1.69-placeable-update-absent-bool no-consume";
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_placeable_update_absent_bool_skip_observations);
      if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
        LogFormat(
            L"legacy live placeable update absent bool skip #%ld: field=%s caller=%s message=%p result=0 no-consume before=[%s] next=[%s] %s%s",
            observation,
            update_bool_field != nullptr ? update_bool_field : L"",
            FormatMainExecutableAddress(return_address).c_str(),
            message,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageBufferPreview(message, before, 48).c_str(),
            update_bool_preview.c_str(),
            FormatLiveDispatchContext().c_str());
      }
      if (trace_module) {
        LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
      }
      if (trace_area) {
        LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      if (trace_live) {
        LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
      }
      return 0;
    }
  }

  if (force_legacy_live_placeable_add_optional_target_false) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const bool can_consume = g_message_read_bool_original != nullptr;
    const int original_result = can_consume ? g_message_read_bool_original(message) : 0;
    const CnwMessageReadState after = ReadCnwMessageReadState(message);
    RecordLegacyLivePlaceableAddBoolDiagnostic(
        message,
        L"add-legacy-optional-target",
        original_result,
        can_consume,
        before,
        after);
    wchar_t detail[192]{};
    swprintf_s(
        detail,
        L"result=0x00000000/0 forced-1.69-placeable-add-no-optional-target consumed-original=0x%08X/%u",
        original_result,
        original_result);
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_placeable_add_optional_target_bool_observations);
    if (observation <= 320 || original_result != 0 ||
        CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      LogFormat(
          L"legacy live placeable add optional-target bool force #%ld: caller=%s message=%p original=0x%08X/%u result=0 consumed=%d before=[%s] after=[%s] next=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          original_result,
          original_result,
          can_consume ? 1 : 0,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          FormatCnwMessageBufferPreview(message, after, 48).c_str(),
          FormatLiveDispatchContext().c_str());
    }
    if (trace_module) {
      LogModuleMessageRead(L"ReadBOOL", message, before, after, detail);
    }
    if (trace_area) {
      LogAreaMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    }
    if (trace_live) {
      LogLiveMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    }
    return 0;
  }

  if (skip_legacy_live_placeable_absent_bool) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    RecordLegacyLivePlaceableAddBoolDiagnostic(
        message,
        legacy_live_placeable_absent_bool_field,
        0,
        false,
        before,
        before);
    const wchar_t* detail =
        L"result=0x00000000/0 skipped-1.69-placeable-absent-bool no-consume";
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_placeable_add_absent_bool_skip_observations);
    if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
      LogFormat(
          L"legacy live placeable add absent bool skip #%ld: field=%s caller=%s message=%p result=0 no-consume before=[%s] next=[%s]%s",
          observation,
          legacy_live_placeable_absent_bool_field != nullptr
              ? legacy_live_placeable_absent_bool_field
              : L"",
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageBufferPreview(message, before, 48).c_str(),
          FormatLiveDispatchContext().c_str());
    }
    if (trace_module) {
      LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
    }
    if (trace_area) {
      LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
    }
    if (trace_live) {
      LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
    }
    return 0;
  }

  if (force_legacy_object_update_field) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    CnwMessageReadState after = before;
    uint32_t legacy_raw = 0;
    CExoStringView legacy_name{};
    const bool consumed_legacy_extra =
        TryConsumeLegacyObjectUpdateExtraName(message, before, &legacy_raw, &legacy_name);
    if (consumed_legacy_extra) {
      after = ReadCnwMessageReadState(message);
    }
    const std::wstring legacy_name_text = consumed_legacy_extra ? CExoStringToWide(&legacy_name) : L"";
    wchar_t detail[512]{};
    if (consumed_legacy_extra) {
      swprintf_s(
          detail,
          L"result=0x00000000/0 forced-1.69-object-update-ee-field consumed-legacy-extra raw=0x%08X name-len=%u name='%s'",
          legacy_raw,
          SafeCExoStringLength(&legacy_name),
          legacy_name_text.c_str());
    } else {
      swprintf_s(detail, L"result=0x00000000/0 forced-1.69-object-update-ee-field no-consume");
    }
    LogLegacyObjectUpdateFieldShim(L"ReadBOOL", return_address, message, before, after, detail);
    if (trace_module) {
      LogModuleMessageRead(L"ReadBOOL", message, before, after, detail);
    }
    if (trace_area) {
      LogAreaMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    }
    if (trace_live) {
      LogLiveMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    }
    auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");
    TryDestroyCExoString(destroy_string, &legacy_name);
    return 0;
  }

  if (skip_legacy_live_item_ee_only_bool) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const wchar_t* detail = L"result=0x00000000/0 skipped-ee-only-live-item-bool no-consume";
    const LONG observation = InterlockedIncrement(&g_legacy_live_item_extra_bool_skip_observations);
    if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
      LogFormat(
          L"legacy live item EE-only bool skip #%ld: caller=%s message=%p before=[%s] next=[%s] %s dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s live_depth=%d",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageBufferPreview(message, before, 32).c_str(),
          detail,
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          FormatLiveObjectOpcode(
              g_current_server_to_player_live_opcode_known,
              g_current_server_to_player_live_opcode).c_str(),
          g_live_game_object_update_depth);
    }
    if (trace_module) {
      LogModuleMessageRead(L"ReadBOOL", message, before, before, detail);
    }
    if (trace_area) {
      LogAreaMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
    }
    if (trace_live) {
      LogLiveMessageRead(L"ReadBOOL", return_address, message, before, before, detail);
    }
    return 0;
  }

  if (force_legacy_live_inline_locstring) {
    const CnwMessageReadState before = ReadCnwMessageReadState(message);
    const bool quickbar_context =
        g_server_to_player_message_depth > 0 &&
        g_current_server_to_player_major == 30 &&
        (g_current_server_to_player_minor == 1 || g_current_server_to_player_minor == 2) &&
        g_live_game_object_update_depth == 0;
    const int original_result = !quickbar_context && g_message_read_bool_original != nullptr
        ? g_message_read_bool_original(message)
        : 0;
    const CnwMessageReadState after = ReadCnwMessageReadState(message);
    if (!quickbar_context) {
      ClearPendingLegacyLiveBadInlineLocStringSkip();
    }
    uint32_t bad_declared_length = 0;
    uint32_t bad_available_length = 0;
    uint32_t bad_boundary_delta = 0;
    std::wstring bad_preview;
    const bool queued_bad_inline_string_skip =
        !quickbar_context &&
        TryPlanLegacyLiveBadInlineLocStringSkip(
            message,
            after,
            &bad_declared_length,
            &bad_available_length,
            &bad_boundary_delta,
            &bad_preview) &&
        QueuePendingLegacyLiveBadInlineLocStringSkip(
            message,
            after,
            bad_declared_length,
            bad_available_length,
            bad_boundary_delta);
    wchar_t detail[320]{};
    if (quickbar_context) {
      swprintf_s(detail, L"result=0x00000000/0 forced-1.69-inline-locstring no-consume-quickbar");
    } else if (queued_bad_inline_string_skip) {
      swprintf_s(
          detail,
          L"result=0x00000000/0 forced-1.69-inline-locstring consumed-original=0x%08X/%u bad-string-skip-pending declared=%u available=%u boundary-delta=%u",
          original_result,
          original_result,
          bad_declared_length,
          bad_available_length,
          bad_boundary_delta);
    } else {
      swprintf_s(
          detail,
          L"result=0x00000000/0 forced-1.69-inline-locstring consumed-original=0x%08X/%u",
          original_result,
          original_result);
    }
    const LONG observation = InterlockedIncrement(&g_legacy_live_inline_locstring_observations);
    if (observation <= 160 || original_result != 0) {
      LogFormat(
          L"legacy live inline-locstring #%ld: caller=%s message=%p before=[%s] after=[%s] %s dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s live_depth=%d",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          detail,
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          FormatLiveObjectOpcode(
              g_current_server_to_player_live_opcode_known,
              g_current_server_to_player_live_opcode).c_str(),
          g_live_game_object_update_depth);
    }
    if (queued_bad_inline_string_skip) {
      const LONG bad_observation =
          InterlockedIncrement(&g_legacy_live_bad_inline_locstring_skip_observations);
      if (bad_observation <= 240) {
        LogFormat(
            L"legacy live bad inline-locstring pending #%ld: caller=%s message=%p before=[%s] after=[%s] declared=%u available=%u boundary-delta=%u preview=[%s]%s",
            bad_observation,
            FormatMainExecutableAddress(return_address).c_str(),
            message,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageReadState(after).c_str(),
            bad_declared_length,
            bad_available_length,
            bad_boundary_delta,
            bad_preview.c_str(),
            FormatLiveDispatchContext().c_str());
      }
    }
    if (trace_module) {
      LogModuleMessageRead(L"ReadBOOL", message, before, after, detail);
    }
    if (trace_area) {
      LogAreaMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    }
    if (trace_live) {
      LogLiveMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
    }
    return 0;
  }

  if (!trace_module && !trace_area && !trace_live && !trace_locstring_client_tlk_flag &&
      !trace_legacy_live_placeable_bool_field) {
    return g_message_read_bool_original != nullptr ? g_message_read_bool_original(message) : 0;
  }

  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  const int result = g_message_read_bool_original != nullptr ? g_message_read_bool_original(message) : 0;
  const CnwMessageReadState after = ReadCnwMessageReadState(message);

  wchar_t detail[64]{};
  swprintf_s(detail, L"result=0x%08X/%u", result, result);
  if (trace_module) {
    LogModuleMessageRead(L"ReadBOOL", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadBOOL", return_address, message, before, after, detail);
  }
  if (trace_legacy_live_placeable_bool_field) {
    RecordLegacyLivePlaceableAddBoolDiagnostic(
        message,
        legacy_live_placeable_bool_field,
        result,
        true,
        before,
        after);
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_placeable_bool_field_observations);
    if (observation <= 600 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      LogFormat(
          L"legacy live placeable bool field #%ld: field=%s caller=%s message=%p result=0x%08X/%u before=[%s] after=[%s] next=[%s]%s",
          observation,
          legacy_live_placeable_bool_field != nullptr ? legacy_live_placeable_bool_field : L"",
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          result,
          result,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          FormatCnwMessageBufferPreview(message, after, 48).c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (trace_locstring_client_tlk_flag) {
    const LONG observation = InterlockedIncrement(&g_locstring_client_flag_observations);
    if (observation <= 240 || result != 0) {
      LogFormat(
          L"locstring client-tlk flag #%ld: caller=%s message=%p result=0x%08X/%u before=[%s] after=[%s] dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s object_depth=%d live_depth=%d module_depth=%d",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          result,
          result,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          FormatLiveObjectOpcode(
              g_current_server_to_player_live_opcode_known,
              g_current_server_to_player_live_opcode).c_str(),
          g_game_object_update_depth,
          g_live_game_object_update_depth,
          g_client_module_load_depth);
    }
  }
  return result;
}

char __fastcall HookedMessageReadChar(void* message, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const bool live_subopcode =
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveGameObjectUpdateSubOpcodeReturnRva);
  const bool should_handle_live_subopcode =
      live_subopcode &&
      g_auto_connect_ip_known &&
      g_server_to_player_message_depth > 0 &&
      g_live_game_object_update_depth > 0 &&
      g_current_server_to_player_major == 5 &&
      g_current_server_to_player_minor == 1;
  const CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  if (live_subopcode) {
    ReleaseLegacyLiveParserBoundaryGuardForSubOpcode(message, boundary_guard_before);
  } else {
    std::wstring boundary_guard_detail;
    if (ShouldSuppressLegacyLiveParserBoundaryRead(
            message,
            return_address,
            boundary_guard_before,
            L"ReadCHAR",
            &boundary_guard_detail)) {
      if (trace_live) {
        wchar_t detail[320]{};
        swprintf_s(
            detail,
            L"bits=%d result='.'/0x00/0 %s",
            bit_count,
            boundary_guard_detail.c_str());
        LogLiveMessageRead(
            L"ReadCHAR",
            return_address,
            message,
            boundary_guard_before,
            boundary_guard_before,
            detail);
      }
      return 0;
    }
  }

  if (!should_handle_live_subopcode && !trace_module && !trace_area && !trace_live) {
    return g_message_read_char_original != nullptr ? g_message_read_char_original(message, bit_count) : 0;
  }

  const CnwMessageReadState original_before = boundary_guard_before;
  CnwMessageReadState before = original_before;
  uint32_t legacy_live_resync_skipped = 0;
  std::wstring legacy_live_resync_preview;
  const bool legacy_live_resynced =
      TryResyncLegacyLiveObjectSubMessageBoundary(
          message,
          return_address,
          original_before,
          &legacy_live_resync_skipped,
          &legacy_live_resync_preview);
  if (legacy_live_resynced) {
    before = ReadCnwMessageReadState(message);
  }
  const bool skipped_legacy_live_tail_subopcode =
      should_handle_live_subopcode &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_TAIL_SUBOPCODE_SKIP") &&
      before.readable &&
      !CnwMessageStateOverflow(before) &&
      before.read_buffer_ptr == before.read_buffer_size;
  const char result = !skipped_legacy_live_tail_subopcode && g_message_read_char_original != nullptr
      ? g_message_read_char_original(message, bit_count)
      : 0;
  const CnwMessageReadState after = skipped_legacy_live_tail_subopcode ? before : ReadCnwMessageReadState(message);
  const unsigned char value = static_cast<unsigned char>(result);
  const wchar_t printable = value >= 0x20 && value <= 0x7E ? static_cast<wchar_t>(value) : L'.';

  wchar_t detail[192]{};
  swprintf_s(
      detail,
      L"bits=%d result='%c'/0x%02X/%d%s",
      bit_count,
      printable,
      static_cast<unsigned int>(value),
      static_cast<int>(result),
      live_subopcode ? L" live-object-subopcode" : L"");
  if (legacy_live_resynced) {
    swprintf_s(
        detail,
        L"bits=%d result='%c'/0x%02X/%d%s resynced-1.69-live-padding skipped=%u from=[%s]",
        bit_count,
        printable,
        static_cast<unsigned int>(value),
        static_cast<int>(result),
        live_subopcode ? L" live-object-subopcode" : L"",
        legacy_live_resync_skipped,
        FormatCnwMessageReadState(original_before).c_str());
  }
  if (skipped_legacy_live_tail_subopcode) {
    swprintf_s(
        detail,
        L"bits=%d result='.'/0x00/0%s forced-1.69-live-tail-subopcode no-consume",
        bit_count,
        live_subopcode ? L" live-object-subopcode" : L"");
  }

  if (legacy_live_resynced) {
    const LONG resync_observation = InterlockedIncrement(&g_legacy_live_subopcode_resync_observations);
    if (resync_observation <= 160) {
      LogFormat(
          L"legacy live sub-opcode resync #%ld: caller=%s message=%p skipped=%u before=[%s] aligned=[%s] after-read=[%s] preview=[%s]%s",
          resync_observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          legacy_live_resync_skipped,
          FormatCnwMessageReadState(original_before).c_str(),
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          legacy_live_resync_preview.c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }

  if (trace_module) {
    LogModuleMessageRead(L"ReadCHAR", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadCHAR", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadCHAR", return_address, message, before, after, detail);
  }
  if (live_subopcode) {
    const LONG observation = InterlockedIncrement(&g_live_object_subopcode_observations);
    if (observation <= 320 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      LogFormat(
          L"live object sub-opcode #%ld: caller=%s dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u opcode='%c'/0x%02X message=%p before=[%s] after=[%s] next-after=[%s]",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          printable,
          static_cast<unsigned int>(value),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          FormatCnwMessageBufferPreview(message, after).c_str());
    }
  }
  return result;
}

unsigned char __fastcall HookedMessageReadByte(void* message, int bit_count, int consume_type) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const bool force_legacy_object_update_field = ShouldForceLegacyGameObjectUpdateByte(return_address);
  const bool legacy_live_extended_armor_table_candidate =
      IsLegacyLiveExtendedArmorTableByteRead(return_address);
  const bool legacy_live_trigger_cursor_candidate =
      IsLegacyLiveTriggerAddSyntheticCursorByteReturn(return_address);
  const CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  const bool force_legacy_live_creature_tail_byte =
      ShouldSkipLegacyLiveCreatureUpdateTailByteAtEnd(return_address, boundary_guard_before);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          boundary_guard_before,
          L"ReadBYTE",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d type=%d result=0x00/0 %s",
          bit_count,
          consume_type,
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadBYTE",
          return_address,
          message,
          boundary_guard_before,
          boundary_guard_before,
          detail);
    }
    return 0;
  }

  if (!force_legacy_object_update_field &&
      !legacy_live_extended_armor_table_candidate &&
      !legacy_live_trigger_cursor_candidate &&
      !force_legacy_live_creature_tail_byte &&
      !trace_module &&
      !trace_area &&
      !trace_live) {
    return g_message_read_byte_original != nullptr
        ? g_message_read_byte_original(message, bit_count, consume_type)
        : 0;
  }

  const CnwMessageReadState before = boundary_guard_before;
  std::wstring extended_armor_boundary_preview;
  const wchar_t* extended_armor_reason = L"";
  const bool force_legacy_live_extended_armor_table =
      ShouldSkipLegacyLiveExtendedArmorTableByte(
          return_address,
          message,
          before,
          &extended_armor_boundary_preview,
          &extended_armor_reason);
  uint8_t legacy_live_trigger_cursor_result = 0;
  uint32_t legacy_live_trigger_cursor_boundary_remaining = 0;
  const bool force_legacy_live_trigger_cursor =
      TryForceLegacyLiveTriggerAddSyntheticCursorByte(
          return_address,
          message,
          before,
          &legacy_live_trigger_cursor_result,
          &legacy_live_trigger_cursor_boundary_remaining);
  unsigned char result = 0;
  CnwMessageReadState after = before;
  if (!force_legacy_object_update_field &&
      !force_legacy_live_extended_armor_table &&
      !force_legacy_live_trigger_cursor &&
      !force_legacy_live_creature_tail_byte) {
    result = g_message_read_byte_original != nullptr
        ? g_message_read_byte_original(message, bit_count, consume_type)
        : 0;
    after = ReadCnwMessageReadState(message);
  } else if (force_legacy_live_trigger_cursor) {
    result = legacy_live_trigger_cursor_result;
  }
  const wchar_t* forced_suffix = L"";
  if (force_legacy_object_update_field) {
    forced_suffix = L" forced-1.69-object-update-ee-field no-consume";
  } else if (force_legacy_live_extended_armor_table) {
    forced_suffix = L" forced-empty-1.69-live-extended-armor-table no-consume";
  } else if (force_legacy_live_trigger_cursor) {
    forced_suffix = L" synthetic-1.69-trigger-cursor no-consume";
  } else if (force_legacy_live_creature_tail_byte) {
    forced_suffix = L" forced-empty-1.69-live-creature-tail-byte no-consume";
  }
  wchar_t detail[192]{};
  swprintf_s(
      detail,
      L"bits=%d type=%d result=0x%02X/%u%s",
      bit_count,
      consume_type,
      result,
      result,
      forced_suffix);
  if (force_legacy_object_update_field) {
    LogLegacyObjectUpdateFieldShim(L"ReadBYTE", return_address, message, before, after, detail);
  }
  if (force_legacy_live_extended_armor_table) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_extended_armor_table_skip_observations);
    if (observation <= 160 || before.read_buffer_ptr >= before.read_buffer_size) {
      LogFormat(
          L"legacy live extended-armor table skip #%ld: caller=%s reason=%s message=%p before=[%s] boundary=[%s] next=[%s] dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s live_depth=%d",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          extended_armor_reason != nullptr ? extended_armor_reason : L"",
          message,
          FormatCnwMessageReadState(before).c_str(),
          extended_armor_boundary_preview.c_str(),
          FormatCnwMessageBufferPreview(message, before, 48).c_str(),
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          FormatLiveObjectOpcode(
              g_current_server_to_player_live_opcode_known,
              g_current_server_to_player_live_opcode).c_str(),
          g_live_game_object_update_depth);
    }
  }
  if (force_legacy_live_creature_tail_byte) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_creature_tail_byte_skip_observations);
    if (observation <= 120) {
      LogFormat(
          L"legacy live creature tail byte skip #%ld: caller=%s message=%p before=[%s] next=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageBufferPreview(message, before, 48).c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (force_legacy_live_trigger_cursor) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_trigger_add_synthetic_cursor_observations);
    if (observation <= 240 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
      LogFormat(
          L"legacy live trigger add synthetic cursor #%ld: caller=%s result=0x%02X/%u no-consume boundary-remaining=%u before=[%s] next=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          static_cast<unsigned int>(result),
          static_cast<unsigned int>(result),
          legacy_live_trigger_cursor_boundary_remaining,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageBufferPreview(message, before, 48).c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (trace_module) {
    LogModuleMessageRead(L"ReadBYTE", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadBYTE", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadBYTE", return_address, message, before, after, detail);
  }
  return result;
}

short __fastcall HookedMessageReadShort(void* message, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          before,
          L"ReadSHORT",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d result=0x0000/0 %s",
          bit_count,
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadSHORT",
          return_address,
          message,
          before,
          before,
          detail);
    }
    return 0;
  }

  if (!trace_module && !trace_area && !trace_live) {
    return g_message_read_short_original != nullptr ? g_message_read_short_original(message, bit_count) : 0;
  }

  const short result =
      g_message_read_short_original != nullptr ? g_message_read_short_original(message, bit_count) : 0;
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  wchar_t detail[192]{};
  swprintf_s(
      detail,
      L"bits=%d result=0x%04X/%d",
      bit_count,
      static_cast<unsigned int>(static_cast<uint16_t>(result)),
      static_cast<int>(result));
  if (trace_module) {
    LogModuleMessageRead(L"ReadSHORT", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadSHORT", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadSHORT", return_address, message, before, after, detail);
  }
  return result;
}

unsigned short __fastcall HookedMessageReadWord(void* message, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          before,
          L"ReadWORD",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d result=0x0000/0 %s",
          bit_count,
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadWORD",
          return_address,
          message,
          before,
          before,
          detail);
    }
    return 0;
  }
  const bool force_legacy_area_empty_tail = ShouldForceLegacyAreaPostStaticCount(return_address, before);
  const bool force_legacy_object_update_field = ShouldForceLegacyGameObjectUpdateWord(return_address);
  const bool force_legacy_live_material_shader_params =
      ShouldSkipLegacyLiveMaterialShaderParamCount(return_address);
  const bool skip_legacy_live_trailing_word =
      ShouldSkipLegacyLiveGenericObjectUpdateTrailingWord(return_address, bit_count, before);

  if (!force_legacy_area_empty_tail &&
      !force_legacy_object_update_field &&
      !force_legacy_live_material_shader_params &&
      !skip_legacy_live_trailing_word &&
      !trace_module &&
      !trace_area &&
      !trace_live) {
    return g_message_read_word_original != nullptr ? g_message_read_word_original(message, bit_count) : 0;
  }

  unsigned short result = 0;
  CnwMessageReadState after = before;
  if (!force_legacy_area_empty_tail &&
      !force_legacy_object_update_field &&
      !force_legacy_live_material_shader_params &&
      !skip_legacy_live_trailing_word) {
    result = g_message_read_word_original != nullptr ? g_message_read_word_original(message, bit_count) : 0;
    after = ReadCnwMessageReadState(message);
  } else if (skip_legacy_live_trailing_word) {
    TrySkipLegacyLiveGenericObjectUpdateTrailingWord(message, return_address, before, &after);
  }

  const wchar_t* forced_suffix = L"";
  if (force_legacy_area_empty_tail) {
    forced_suffix = L" forced-empty-1.69-area-tail no-consume";
  } else if (force_legacy_object_update_field) {
    forced_suffix = L" forced-1.69-object-update-ee-field no-consume";
  } else if (force_legacy_live_material_shader_params) {
    forced_suffix = L" forced-empty-1.69-live-material-shader-params no-consume";
  } else if (skip_legacy_live_trailing_word) {
    forced_suffix = L" skipped-1.69-live-trailing-word consumed-to-end";
  }

  wchar_t detail[192]{};
  swprintf_s(
      detail,
      L"bits=%d result=0x%04X/%u%s",
      bit_count,
      static_cast<unsigned int>(result),
      static_cast<unsigned int>(result),
      forced_suffix);
  if (force_legacy_object_update_field) {
    LogLegacyObjectUpdateFieldShim(L"ReadWORD", return_address, message, before, after, detail);
  }
  if (force_legacy_live_material_shader_params) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_material_shader_param_skip_observations);
    if (observation <= 80) {
      LogFormat(
          L"legacy live material-shader-param skip #%ld: caller=%s message=%p before=[%s] after=[%s] dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u %s live_depth=%d",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          g_current_server_to_player_size,
          FormatLiveObjectOpcode(
              g_current_server_to_player_live_opcode_known,
              g_current_server_to_player_live_opcode).c_str(),
          g_live_game_object_update_depth);
    }
  }
  if (trace_module) {
    LogModuleMessageRead(L"ReadWORD", message, before, after, detail);
  }
  if (trace_area || force_legacy_area_empty_tail) {
    LogAreaMessageRead(L"ReadWORD", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadWORD", return_address, message, before, after, detail);
  }
  return result;
}

uint32_t __fastcall HookedMessageReadDword(void* message, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool skip_legacy_hak_list = ShouldSkipLegacyModuleHakListBeforeResourceCount(return_address);
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const bool legacy_live_update_mask_candidate =
      IsLegacyLivePacketReadContext() &&
      bit_count == 32 &&
      ReturnAddressMatchesMainRva(return_address, kEeClientLiveObjectUpdateMaskReturnRva);
  const CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          boundary_guard_before,
          L"ReadDWORD",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d result=0x00000000/0 %s",
          bit_count,
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadDWORD",
          return_address,
          message,
          boundary_guard_before,
          boundary_guard_before,
          detail);
    }
    return 0;
  }
  if (!skip_legacy_hak_list &&
      !legacy_live_update_mask_candidate &&
      !trace_module &&
      !trace_area &&
      !trace_live) {
    return g_message_read_dword_original != nullptr ? g_message_read_dword_original(message, bit_count) : 0;
  }

  const CnwMessageReadState before = boundary_guard_before;
  LegacyLiveUpdateMaskReadInfo legacy_live_update_mask_info;
  const bool observe_legacy_live_update_mask =
      legacy_live_update_mask_candidate &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_UPDATE_MASK_OBSERVE") &&
      TryDescribeLegacyLiveUpdateMaskRead(
          return_address,
          message,
          bit_count,
          before,
          &legacy_live_update_mask_info);
  LegacyLiveUpdateMaskTranslationPlan legacy_live_update_mask_plan;
  const bool translate_legacy_live_update_mask =
      TryPlanLegacyLiveUpdateMaskTranslation(
          return_address,
          message,
          bit_count,
          before,
          &legacy_live_update_mask_plan);
  uint32_t hak_count = 0;
  uint32_t skipped_bytes = 0;
  uint32_t resource_count_peek = 0;
  std::wstring prefix_bytes;
  std::vector<std::string> haks;
  std::string module_resref;
  const bool skipped = skip_legacy_hak_list &&
      TrySkipLegacyModuleHakList(
          message,
          before,
          &hak_count,
          &skipped_bytes,
          &resource_count_peek,
          &prefix_bytes,
          &haks,
          &module_resref);
  if (skipped) {
    MountLegacyModuleHakList(haks, module_resref);
  }
  const uint32_t raw_result =
      g_message_read_dword_original != nullptr ? g_message_read_dword_original(message, bit_count) : 0;
  const uint32_t result =
      translate_legacy_live_update_mask ? legacy_live_update_mask_plan.translated_mask : raw_result;
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  wchar_t detail[1024]{};
  const std::wstring caller = FormatMainExecutableAddress(return_address);
  swprintf_s(
      detail,
      L"bits=%d result=0x%08X/%u caller=%s%s%s%s",
      bit_count,
      result,
      result,
      caller.c_str(),
      skipped ? L" skipped-1.69-hak-list" : L"",
      skipped ? L"" : (skip_legacy_hak_list ? L" hak-list-skip-failed" : L""),
      translate_legacy_live_update_mask ? L" translated-1.69-live-update-mask" : L"");
  if (trace_module || skip_legacy_hak_list) {
    LogModuleMessageRead(L"ReadDWORD", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadDWORD", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadDWORD", return_address, message, before, after, detail);
  }
  if (skip_legacy_hak_list) {
    LogFormat(
        L"legacy module-load hak-list skip: %s hak_count=%u skipped=%u resource_count_peek=%u before=[%s] after=[%s] prefix=[%s]",
        skipped ? L"applied" : L"failed",
        hak_count,
        skipped_bytes,
        resource_count_peek,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        prefix_bytes.c_str());
    if (skipped) {
      LogFormat(
          L"legacy module-load hak-list parsed: module_resref='%s' haks=[%s]",
          NarrowToWide(module_resref).c_str(),
          JoinAsciiList(haks).c_str());
    }
  }
  if (translate_legacy_live_update_mask) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_update_mask_translate_observations);
    if (observation <= 240 || raw_result != legacy_live_update_mask_plan.raw_mask) {
      LogFormat(
          L"legacy live update mask translate #%ld: caller=%s message=%p type=%u/%s object=0x%08X/%u raw=0x%08X/%u read=0x%08X/%u translated=0x%08X/%u before=[%s] after=[%s] %s%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          static_cast<unsigned int>(legacy_live_update_mask_plan.object_type),
          LegacyLiveObjectTypeName(legacy_live_update_mask_plan.object_type),
          legacy_live_update_mask_plan.object_id,
          legacy_live_update_mask_plan.object_id,
          legacy_live_update_mask_plan.raw_mask,
          legacy_live_update_mask_plan.raw_mask,
          raw_result,
          raw_result,
          legacy_live_update_mask_plan.translated_mask,
          legacy_live_update_mask_plan.translated_mask,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          legacy_live_update_mask_plan.preview.c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (observe_legacy_live_update_mask && !translate_legacy_live_update_mask) {
    const bool legacy_full_mask =
        legacy_live_update_mask_info.raw_mask == 0xFFFFFFF7u ||
        legacy_live_update_mask_info.raw_mask == 0xFFFFFFFFu ||
        legacy_live_update_mask_info.raw_mask == 0xFFFFFFF3u;
    const bool interesting_type =
        legacy_live_update_mask_info.object_type != 9 ||
        raw_result != legacy_live_update_mask_info.raw_mask ||
        legacy_full_mask;
    if (interesting_type) {
      const LONG observation =
          InterlockedIncrement(&g_legacy_live_update_mask_observe_observations);
      if (observation <= 320 || legacy_full_mask || raw_result != legacy_live_update_mask_info.raw_mask) {
        LogFormat(
            L"legacy live update mask observe #%ld: caller=%s message=%p type=%u/%s object=0x%08X/%u raw=0x%08X/%u read=0x%08X/%u action=pass-through before=[%s] after=[%s] %s%s",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            message,
            static_cast<unsigned int>(legacy_live_update_mask_info.object_type),
            LegacyLiveObjectTypeName(legacy_live_update_mask_info.object_type),
            legacy_live_update_mask_info.object_id,
            legacy_live_update_mask_info.object_id,
            legacy_live_update_mask_info.raw_mask,
            legacy_live_update_mask_info.raw_mask,
            raw_result,
            raw_result,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageReadState(after).c_str(),
            legacy_live_update_mask_info.preview.c_str(),
            FormatLiveDispatchContext().c_str());
      }
    }
  }
  return result;
}

int __fastcall HookedMessageReadInt(void* message, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          boundary_guard_before,
          L"ReadINT",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d result=0x00000000/0 %s",
          bit_count,
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadINT",
          return_address,
          message,
          boundary_guard_before,
          boundary_guard_before,
          detail);
    }
    return 0;
  }

  if (!trace_module && !trace_area && !trace_live) {
    return g_message_read_int_original != nullptr ? g_message_read_int_original(message, bit_count) : 0;
  }

  const CnwMessageReadState before = boundary_guard_before;
  const int result = g_message_read_int_original != nullptr ? g_message_read_int_original(message, bit_count) : 0;
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  wchar_t detail[128]{};
  swprintf_s(detail, L"bits=%d result=0x%08X/%d", bit_count, static_cast<uint32_t>(result), result);
  if (trace_module) {
    LogModuleMessageRead(L"ReadINT", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadINT", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadINT", return_address, message, before, after, detail);
  }
  return result;
}

float __fastcall HookedMessageReadFloat(void* message, float default_value, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          boundary_guard_before,
          L"ReadFLOAT",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d default=%f result=0.000000 %s",
          bit_count,
          static_cast<double>(default_value),
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadFLOAT",
          return_address,
          message,
          boundary_guard_before,
          boundary_guard_before,
          detail);
    }
    return 0.0f;
  }

  if (!trace_module && !trace_area && !trace_live) {
    return g_message_read_float_original != nullptr
        ? g_message_read_float_original(message, default_value, bit_count)
        : 0.0f;
  }

  const CnwMessageReadState before = boundary_guard_before;
  const float result = g_message_read_float_original != nullptr
      ? g_message_read_float_original(message, default_value, bit_count)
      : 0.0f;
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  wchar_t detail[160]{};
  swprintf_s(
      detail,
      L"bits=%d default=%f result=%f",
      bit_count,
      static_cast<double>(default_value),
      static_cast<double>(result));
  if (trace_module) {
    LogModuleMessageRead(L"ReadFLOAT", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadFLOAT", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadFLOAT", return_address, message, before, after, detail);
  }
  return result;
}

CExoStringView* __fastcall HookedMessageReadCExoString(void* message, CExoStringView* result_string, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const bool force_legacy_object_update_field = ShouldForceLegacyGameObjectUpdateString(return_address);
  const bool check_legacy_live_oversized_string = ShouldCheckLegacyLiveOversizedCExoString();
  const bool check_legacy_live_update_name =
      ShouldCheckLegacyLiveUpdateNameString(return_address);
  const bool check_legacy_live_trigger_add_name =
      ShouldCheckLegacyLiveTriggerAddNameString(return_address);
  uint8_t legacy_live_update_absent_name_expected_type = 0;
  const wchar_t* legacy_live_update_absent_name_expected_label = L"object";
  const bool check_legacy_live_update_absent_name_string =
      IsLegacyLivePacketReadContext() &&
      IdentifyLegacyLiveUpdateNameStringReturn(
          return_address,
          &legacy_live_update_absent_name_expected_type,
          &legacy_live_update_absent_name_expected_label) &&
      !LegacyLiveUpdateAbsentNameSkipDisabled(legacy_live_update_absent_name_expected_type);
  const CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          boundary_guard_before,
          L"ReadCExoString",
          &boundary_guard_detail)) {
    SetEmptyCExoString(result_string);
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d length=0 text='' %s",
          bit_count,
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadCExoString",
          return_address,
          message,
          boundary_guard_before,
          boundary_guard_before,
          detail);
    }
    return result_string;
  }

  if (!force_legacy_object_update_field &&
      !check_legacy_live_oversized_string &&
      !check_legacy_live_update_name &&
      !check_legacy_live_update_absent_name_string &&
      !check_legacy_live_trigger_add_name &&
      !trace_module && !trace_area && !trace_live) {
    return g_message_read_cexo_string_original != nullptr
        ? g_message_read_cexo_string_original(message, result_string, bit_count)
        : result_string;
  }

  const CnwMessageReadState before = boundary_guard_before;
  CExoStringView* result = result_string;
  CnwMessageReadState after = before;
  CExoStringView legacy_second_name{};
  uint16_t legacy_portrait = 0;
  CResRefView legacy_portrait_resref{};
  bool consumed_legacy_second_name = false;
  bool consumed_legacy_portrait_tail = false;
  bool consumed_legacy_live_short_add_name = false;
  bool consumed_legacy_live_trigger_add_name = false;
  bool skipped_legacy_live_oversized_string = false;
  bool skipped_legacy_live_bad_inline_locstring = false;
  bool rewound_legacy_live_bad_inline_locstring = false;
  bool skipped_legacy_live_update_absent_name = false;
  bool recovered_legacy_live_misaligned_string = false;
  bool recovered_legacy_live_update_name = false;
  uint32_t legacy_live_short_add_strref = 0;
  uint32_t legacy_live_short_add_consumed_bytes = 0;
  uint32_t legacy_live_short_add_boundary_delta = 0;
  uint32_t legacy_live_trigger_add_strref = 0;
  uint32_t legacy_live_trigger_add_consumed_bytes = 0;
  uint32_t legacy_live_trigger_add_boundary_delta = 0;
  uint8_t legacy_live_update_absent_name_object_type = 0;
  const wchar_t* legacy_live_update_absent_name_label = L"object";
  uint32_t legacy_live_update_absent_name_object_id = 0;
  uint32_t legacy_live_update_absent_name_mask = 0;
  uint32_t legacy_live_update_absent_name_skipped_bytes = 0;
  std::string legacy_live_update_absent_name_synthetic_name;
  uint32_t legacy_live_declared_length = 0;
  uint32_t legacy_live_available_length = 0;
  uint32_t legacy_live_oversized_boundary_delta = 0;
  uint32_t legacy_live_bad_inline_boundary_delta = 0;
  uint32_t legacy_live_bad_inline_rewind_bytes = 0;
  int32_t legacy_live_update_boundary_delta = 0;
  uint32_t recovered_legacy_live_string_start = 0;
  uint32_t recovered_legacy_live_string_end = 0;
  std::string recovered_legacy_live_string_text;
  std::wstring legacy_live_header_preview;
  if (force_legacy_object_update_field) {
    SetEmptyCExoString(result_string);
    consumed_legacy_second_name =
        TryConsumeLegacyObjectUpdateSecondName(message, before, &legacy_second_name);
    if (consumed_legacy_second_name) {
      const CnwMessageReadState after_second_name = ReadCnwMessageReadState(message);
      consumed_legacy_portrait_tail =
          TryConsumeLegacyObjectUpdatePortraitTail(
              message,
              after_second_name,
              &legacy_portrait,
              &legacy_portrait_resref);
      after = ReadCnwMessageReadState(message);
    }
  } else if (TryConsumePendingLegacyLiveTriggerAddNameString(
                 message,
                 return_address,
                 before,
                 result_string,
                 &after,
                 &legacy_live_trigger_add_strref,
                 &legacy_live_trigger_add_consumed_bytes,
                 &legacy_live_trigger_add_boundary_delta,
                 &legacy_live_header_preview)) {
    result = result_string;
    consumed_legacy_live_trigger_add_name = true;
  } else if (TryConsumePendingLegacyLiveShortAddNameString(
                 message,
                 return_address,
                 before,
                 result_string,
                 &after,
                 &legacy_live_short_add_strref,
                 &legacy_live_short_add_consumed_bytes,
                 &legacy_live_short_add_boundary_delta,
                 &legacy_live_header_preview)) {
    result = result_string;
    consumed_legacy_live_short_add_name = true;
  } else if (TryConsumePendingLegacyLiveBadInlineLocStringSkip(
                  message,
                  return_address,
                  before,
                 result_string,
                 &after,
                 &legacy_live_declared_length,
                 &legacy_live_available_length,
                 &legacy_live_bad_inline_boundary_delta,
                 &legacy_live_bad_inline_rewind_bytes,
                 &rewound_legacy_live_bad_inline_locstring,
                 &legacy_live_header_preview)) {
    result = result_string;
    skipped_legacy_live_bad_inline_locstring = true;
  } else if (TryConsumePendingLegacyLiveUpdateAbsentNameString(
                 message,
                 return_address,
                 before,
                 result_string,
                 &after,
                 &legacy_live_update_absent_name_object_type,
                 &legacy_live_update_absent_name_label,
                 &legacy_live_update_absent_name_object_id,
                 &legacy_live_update_absent_name_mask,
                 &legacy_live_update_absent_name_skipped_bytes,
                 &legacy_live_update_absent_name_synthetic_name,
                 &legacy_live_header_preview)) {
    result = result_string;
    skipped_legacy_live_update_absent_name = true;
  } else if (TryRecoverLegacyLiveMisalignedCExoString(
                 message,
                 return_address,
                 before,
                 result_string,
                 &after,
                 &legacy_live_declared_length,
                 &legacy_live_available_length,
                 &recovered_legacy_live_string_start,
                 &recovered_legacy_live_string_end,
                 &recovered_legacy_live_string_text,
                 &legacy_live_header_preview)) {
    result = result_string;
    recovered_legacy_live_misaligned_string = true;
  } else if (TryRecoverLegacyLiveUpdateNameString(
                 message,
                 return_address,
                 before,
                 result_string,
                 &after,
                 &legacy_live_declared_length,
                 &legacy_live_available_length,
                 &legacy_live_update_boundary_delta,
                 &legacy_live_header_preview)) {
    result = result_string;
    recovered_legacy_live_update_name = true;
  } else if (LooksLikeOversizedLegacyLiveCExoString(
                 message,
                  before,
                  &legacy_live_declared_length,
                  &legacy_live_available_length,
                  &legacy_live_oversized_boundary_delta,
                  &legacy_live_header_preview)) {
    SetEmptyCExoString(result_string);
    result = result_string;
    if (legacy_live_oversized_boundary_delta > 0 &&
        legacy_live_oversized_boundary_delta <= before.read_buffer_size - before.read_buffer_ptr) {
      const uint32_t target = before.read_buffer_ptr + legacy_live_oversized_boundary_delta;
      if (WriteUInt32AtOffset(message, 0x44, target)) {
        ArmPendingLegacyLiveParserBoundaryGuard(
            message,
            before,
            target,
            legacy_live_declared_length,
            legacy_live_available_length);
      }
    }
    after = ReadCnwMessageReadState(message);
    skipped_legacy_live_oversized_string = true;
  } else {
    result = g_message_read_cexo_string_original != nullptr
        ? g_message_read_cexo_string_original(message, result_string, bit_count)
        : result_string;
    after = ReadCnwMessageReadState(message);
  }
  wchar_t detail[768]{};
  const uint32_t length = result != nullptr ? SafeCExoStringLength(result) : 0;
  swprintf_s(
      detail,
      L"bits=%d length=%u text='%s'%s",
      bit_count,
      length,
      result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
      force_legacy_object_update_field ? L" forced-1.69-object-update-ee-field no-consume" : L"");
  if (skipped_legacy_live_oversized_string) {
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' skipped-1.69-live-oversized-string%s declared=%u available=%u boundary-delta=%u header=[%s]",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        legacy_live_oversized_boundary_delta > 0 ? L" to-boundary" : L" no-consume",
        legacy_live_declared_length,
        legacy_live_available_length,
        legacy_live_oversized_boundary_delta,
        legacy_live_header_preview.c_str());
  }
  if (consumed_legacy_live_short_add_name) {
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' consumed-1.69-live-short-add-name strref=0x%08X/%u consumed=%u boundary-delta=%u header=[%s]",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        legacy_live_short_add_strref,
        legacy_live_short_add_strref,
        legacy_live_short_add_consumed_bytes,
        legacy_live_short_add_boundary_delta,
        legacy_live_header_preview.c_str());
  }
  if (consumed_legacy_live_trigger_add_name) {
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' consumed-1.69-live-trigger-add-name strref=0x%08X/%u consumed=%u boundary-delta=%u header=[%s]",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        legacy_live_trigger_add_strref,
        legacy_live_trigger_add_strref,
        legacy_live_trigger_add_consumed_bytes,
        legacy_live_trigger_add_boundary_delta,
        legacy_live_header_preview.c_str());
  }
  if (skipped_legacy_live_bad_inline_locstring) {
    if (rewound_legacy_live_bad_inline_locstring) {
      swprintf_s(
          detail,
          L"bits=%d length=%u text='%s' rewound-1.69-live-bad-inline-locstring declared=%u available=%u boundary-delta=%u rewind=%u header=[%s]",
          bit_count,
          length,
          result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
          legacy_live_declared_length,
          legacy_live_available_length,
          legacy_live_bad_inline_boundary_delta,
          legacy_live_bad_inline_rewind_bytes,
          legacy_live_header_preview.c_str());
    } else {
      swprintf_s(
          detail,
          L"bits=%d length=%u text='%s' skipped-1.69-live-bad-inline-locstring declared=%u available=%u boundary-delta=%u header=[%s]",
          bit_count,
          length,
          result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
          legacy_live_declared_length,
          legacy_live_available_length,
          legacy_live_bad_inline_boundary_delta,
          legacy_live_header_preview.c_str());
    }
  }
  if (recovered_legacy_live_update_name) {
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' recovered-1.69-live-update-name declared=%u available=%u boundary-delta=%d header=[%s]",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        legacy_live_declared_length,
        legacy_live_available_length,
        legacy_live_update_boundary_delta,
        legacy_live_header_preview.c_str());
  }
  if (skipped_legacy_live_update_absent_name) {
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' skipped-1.69-live-%s-update-absent-name object=0x%08X/%u type=%u update-mask=0x%08X skipped=%u synthetic-name='%s' header=[%s]",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        legacy_live_update_absent_name_label != nullptr
            ? legacy_live_update_absent_name_label
            : L"object",
        legacy_live_update_absent_name_object_id,
        legacy_live_update_absent_name_object_id,
        static_cast<unsigned int>(legacy_live_update_absent_name_object_type),
        legacy_live_update_absent_name_mask,
        legacy_live_update_absent_name_skipped_bytes,
        NarrowToWide(legacy_live_update_absent_name_synthetic_name).c_str(),
        legacy_live_header_preview.c_str());
  }
  if (recovered_legacy_live_misaligned_string) {
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' recovered-1.69-live-misaligned-string declared=%u available=%u start=%u end=%u boundary=[%s]",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        legacy_live_declared_length,
        legacy_live_available_length,
        recovered_legacy_live_string_start,
        recovered_legacy_live_string_end,
        legacy_live_header_preview.c_str());
  }
  if (consumed_legacy_second_name) {
    const std::wstring legacy_second_name_text = CExoStringToWide(&legacy_second_name);
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' forced-1.69-object-update-ee-field no-consume consumed-legacy-second-name name-len=%u name='%s'%s",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        SafeCExoStringLength(&legacy_second_name),
        legacy_second_name_text.c_str(),
        consumed_legacy_portrait_tail ? L" consumed-legacy-portrait-tail" : L" portrait-tail-not-consumed");
  }
  if (consumed_legacy_portrait_tail) {
    const std::wstring legacy_second_name_text = CExoStringToWide(&legacy_second_name);
    swprintf_s(
        detail,
        L"bits=%d length=%u text='%s' forced-1.69-object-update-ee-field no-consume consumed-legacy-second-name name-len=%u name='%s' consumed-legacy-portrait-tail portrait=0x%04X resref='%s'",
        bit_count,
        length,
        result != nullptr ? CExoStringToWide(result).c_str() : L"<null>",
        SafeCExoStringLength(&legacy_second_name),
        legacy_second_name_text.c_str(),
        static_cast<unsigned int>(legacy_portrait),
        CResRefToWide(&legacy_portrait_resref).c_str());
  }
  if (skipped_legacy_live_oversized_string) {
    const LONG observation = InterlockedIncrement(&g_legacy_live_oversized_cexo_string_skip_observations);
    if (observation <= 160) {
      LogFormat(
          L"legacy live oversized CExoString skip #%ld: caller=%s message=%p declared=%u available=%u boundary-delta=%u header=[%s] before=[%s] after=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          legacy_live_declared_length,
          legacy_live_available_length,
          legacy_live_oversized_boundary_delta,
          legacy_live_header_preview.c_str(),
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (consumed_legacy_live_short_add_name) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_short_add_name_consume_observations);
    if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      LogFormat(
          L"legacy live short add name consume #%ld: caller=%s message=%p strref=0x%08X/%u consumed=%u boundary-delta=%u before=[%s] after=[%s] header=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          legacy_live_short_add_strref,
          legacy_live_short_add_strref,
          legacy_live_short_add_consumed_bytes,
          legacy_live_short_add_boundary_delta,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          legacy_live_header_preview.c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (consumed_legacy_live_trigger_add_name) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_trigger_add_name_consume_observations);
    if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      LogFormat(
          L"legacy live trigger add name consume #%ld: caller=%s message=%p strref=0x%08X/%u consumed=%u boundary-delta=%u before=[%s] after=[%s] header=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          legacy_live_trigger_add_strref,
          legacy_live_trigger_add_strref,
          legacy_live_trigger_add_consumed_bytes,
          legacy_live_trigger_add_boundary_delta,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          legacy_live_header_preview.c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (skipped_legacy_live_bad_inline_locstring) {
    const LONG observation = InterlockedIncrement(
        rewound_legacy_live_bad_inline_locstring
            ? &g_legacy_live_bad_inline_locstring_rewind_observations
            : &g_legacy_live_bad_inline_locstring_skip_observations);
    if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      if (rewound_legacy_live_bad_inline_locstring) {
        LogFormat(
            L"legacy live bad inline-locstring rewind #%ld: caller=%s message=%p declared=%u available=%u forward-boundary-delta=%u rewind=%u before=[%s] after=[%s] header=[%s]%s",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            message,
            legacy_live_declared_length,
            legacy_live_available_length,
            legacy_live_bad_inline_boundary_delta,
            legacy_live_bad_inline_rewind_bytes,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageReadState(after).c_str(),
            legacy_live_header_preview.c_str(),
            FormatLiveDispatchContext().c_str());
      } else {
        LogFormat(
            L"legacy live bad inline-locstring string skip #%ld: caller=%s message=%p declared=%u available=%u boundary-delta=%u before=[%s] after=[%s] header=[%s]%s",
            observation,
            FormatMainExecutableAddress(return_address).c_str(),
            message,
            legacy_live_declared_length,
            legacy_live_available_length,
            legacy_live_bad_inline_boundary_delta,
            FormatCnwMessageReadState(before).c_str(),
            FormatCnwMessageReadState(after).c_str(),
            legacy_live_header_preview.c_str(),
            FormatLiveDispatchContext().c_str());
      }
    }
  }
  if (recovered_legacy_live_update_name) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_update_name_recover_observations);
    if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      LogFormat(
          L"legacy live update name recover #%ld: caller=%s message=%p declared=%u available=%u boundary-delta=%d before=[%s] after=[%s] header=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          legacy_live_declared_length,
          legacy_live_available_length,
          legacy_live_update_boundary_delta,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          legacy_live_header_preview.c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (skipped_legacy_live_update_absent_name) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_update_absent_name_string_skip_observations);
    if (observation <= 240 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
      LogFormat(
          L"legacy live %s update absent-name string skip #%ld: caller=%s message=%p object=0x%08X/%u type=%u update-mask=0x%08X skipped=%u synthetic-name='%s' before=[%s] after=[%s] record=[%s]%s",
          legacy_live_update_absent_name_label != nullptr
              ? legacy_live_update_absent_name_label
              : L"object",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          legacy_live_update_absent_name_object_id,
          legacy_live_update_absent_name_object_id,
          static_cast<unsigned int>(legacy_live_update_absent_name_object_type),
          legacy_live_update_absent_name_mask,
          legacy_live_update_absent_name_skipped_bytes,
          NarrowToWide(legacy_live_update_absent_name_synthetic_name).c_str(),
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          legacy_live_header_preview.c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (recovered_legacy_live_misaligned_string) {
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_misaligned_cexo_string_recover_observations);
    if (observation <= 160) {
      LogFormat(
          L"legacy live misaligned CExoString recover #%ld: caller=%s message=%p declared=%u available=%u start=%u end=%u text='%s' boundary=[%s] before=[%s] after=[%s]%s",
          observation,
          FormatMainExecutableAddress(return_address).c_str(),
          message,
          legacy_live_declared_length,
          legacy_live_available_length,
          recovered_legacy_live_string_start,
          recovered_legacy_live_string_end,
          NarrowToWide(recovered_legacy_live_string_text).c_str(),
          legacy_live_header_preview.c_str(),
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  if (force_legacy_object_update_field) {
    LogLegacyObjectUpdateFieldShim(L"ReadCExoString", return_address, message, before, after, detail);
  }
  if (trace_module) {
    LogModuleMessageRead(L"ReadCExoString", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadCExoString", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadCExoString", return_address, message, before, after, detail);
  }
  if (consumed_legacy_second_name) {
    auto* const destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");
    TryDestroyCExoString(destroy_string, &legacy_second_name);
  }
  return result;
}

CResRefView* __fastcall HookedMessageReadCResRef(void* message, CResRefView* result_ref, int bit_count) {
  void* const return_address = _ReturnAddress();
  const bool trace_module = g_module_read_diagnostics_enabled && g_client_module_load_depth > 0;
  const bool trace_area = ShouldTraceAreaMessageRead(return_address);
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState boundary_guard_before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          boundary_guard_before,
          L"ReadCResRef",
          &boundary_guard_detail)) {
    if (result_ref != nullptr) {
      std::memset(result_ref->data, 0, sizeof(result_ref->data));
    }
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"bits=%d resref='' %s",
          bit_count,
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadCResRef",
          return_address,
          message,
          boundary_guard_before,
          boundary_guard_before,
          detail);
    }
    return result_ref;
  }

  if (!trace_module && !trace_area && !trace_live) {
    return g_message_read_cresref_original != nullptr
        ? g_message_read_cresref_original(message, result_ref, bit_count)
        : result_ref;
  }

  const CnwMessageReadState before = boundary_guard_before;
  CResRefView* const result = g_message_read_cresref_original != nullptr
      ? g_message_read_cresref_original(message, result_ref, bit_count)
      : result_ref;
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  wchar_t detail[512]{};
  swprintf_s(
      detail,
      L"bits=%d resref='%s' raw=[%s]",
      bit_count,
      CResRefToWide(result).c_str(),
      CResRefRawBytes(result).c_str());
  if (trace_module) {
    LogModuleMessageRead(L"ReadCResRef", message, before, after, detail);
  }
  if (trace_area) {
    LogAreaMessageRead(L"ReadCResRef", return_address, message, before, after, detail);
  }
  if (trace_live) {
    LogLiveMessageRead(L"ReadCResRef", return_address, message, before, after, detail);
  }
  return result;
}

void __fastcall HookedMessageCreateWriteMessage(
    void* message,
    uint32_t reserve_bytes,
    uint32_t arg3,
    int arg4) {
  const LONG observation = InterlockedIncrement(&g_message_create_write_observations);
  void* const return_address = _ReturnAddress();
  const CnwMessageWriteState before = ReadCnwMessageWriteState(message);

  if (observation <= 160) {
    LogFormat(
        L"CNWMessage::CreateWriteMessage #%ld begin: message=%p reserve=%u arg3=%u/0x%X arg4=%d caller=%s before=[%s]",
        observation,
        message,
        reserve_bytes,
        arg3,
        arg3,
        arg4,
        FormatMainExecutableAddress(return_address).c_str(),
        FormatCnwMessageWriteState(before).c_str());
  }

  if (g_message_create_write_message_original != nullptr) {
    g_message_create_write_message_original(message, reserve_bytes, arg3, arg4);
  }

  if (observation <= 160) {
    const CnwMessageWriteState after = ReadCnwMessageWriteState(message);
    LogFormat(
        L"CNWMessage::CreateWriteMessage #%ld end: message=%p after=[%s] preview=[%s]",
        observation,
        message,
        FormatCnwMessageWriteState(after).c_str(),
        FormatCnwMessageWritePreview(message, after, 64).c_str());
  }
}

int __fastcall HookedMessageGetWriteMessage(void* message, unsigned char** out_message, uint32_t* out_size) {
  const LONG observation = InterlockedIncrement(&g_message_get_write_observations);
  void* const return_address = _ReturnAddress();
  const CnwMessageWriteState before = ReadCnwMessageWriteState(message);
  const int result = g_message_get_write_message_original != nullptr
      ? g_message_get_write_message_original(message, out_message, out_size)
      : 0;

  unsigned char* output_message = nullptr;
  uint32_t output_size = 0;
  const bool output_ok = SafeReadWriteMessageOutputs(out_message, out_size, &output_message, &output_size);
  const CnwMessageWriteState after = ReadCnwMessageWriteState(message);
  const bool interesting = observation <= 180 || result == 0 || output_size <= 8;
  if (interesting) {
    LogFormat(
        L"CNWMessage::GetWriteMessage #%ld: message=%p result=%d output_ok=%d output=%p size=%u caller=%s before=[%s] after=[%s] preview=[%s] bytes=[%s]",
        observation,
        message,
        result,
        output_ok ? 1 : 0,
        output_message,
        output_size,
        FormatMainExecutableAddress(return_address).c_str(),
        FormatCnwMessageWriteState(before).c_str(),
        FormatCnwMessageWriteState(after).c_str(),
        FormatCnwMessageWritePreview(message, after, 64).c_str(),
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(output_message), output_size, 128).c_str());
  }
  return result;
}

int64_t __fastcall HookedClientSendPlayerToServerMessage(
    void* message,
    unsigned char major,
    unsigned char minor,
    unsigned char* payload,
    uint32_t payload_size) {
  const LONG observation = InterlockedIncrement(&g_client_to_server_message_observations);
  const bool is_area_loaded = major == 4 && minor == 3;
  const bool is_synthetic_area_loaded =
      is_area_loaded && g_synthetic_area_loaded_send_depth > 0;
  const LONG area_dispatches_before_send = g_area_load_dispatch_observations;
  const LONG area_loaded_before_send = g_client_to_server_area_loaded_observations;
  const bool area_loaded_has_outstanding_dispatch =
      area_dispatches_before_send > area_loaded_before_send;
  const bool suppress_unexpected_area_loaded =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_SUPPRESS_UNEXPECTED_AREA_LOADED");
  LONG area_loaded_observation = 0;

  const std::wstring name = LookupMajorMinorNameWide(major, minor);
  const bool is_input = major == 6;
  const bool is_load_or_area_related = major == 2 || major == 3 || major == 4 || major == 44;
  const bool trigger_transition_click_rewritten =
      TryRewriteTriggerTransitionClickPayload(major, minor, payload, payload_size);
  const std::wstring payload_detail =
      FormatClientInputPayloadDetail(major, minor, payload, payload_size);
  const bool interesting =
      observation <= 260 || is_area_loaded || is_input || is_load_or_area_related ||
      major == 18 || major == 49 || major == 50;
  if (interesting) {
    LogFormat(
        L"client-to-server message #%ld begin: message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X payload=%p payload_size=%u header=[70 %02X %02X] payload=[%s]%s caller=%s stack=[%s]%s%s",
        observation,
        message,
        name.c_str(),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        static_cast<unsigned int>(minor),
        payload,
        payload_size,
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(payload), payload_size, 160).c_str(),
        payload_detail.c_str(),
        FormatMainExecutableAddress(_ReturnAddress()).c_str(),
        (is_area_loaded || is_input) ? FormatCurrentStackTrace(1, 8).c_str() : L"-",
        is_area_loaded ? L" AREA_LOADED" : L"",
        trigger_transition_click_rewritten ? L" TRIGGER_TRANSITION_REWRITE" : L"");
  }

  if (is_area_loaded && !is_synthetic_area_loaded && !area_loaded_has_outstanding_dispatch &&
      suppress_unexpected_area_loaded) {
    const LONG suppressed_observation =
        InterlockedIncrement(&g_client_to_server_area_loaded_suppressed_observations);
    LogFormat(
        L"suppressed unexpected Area_AreaLoaded #%ld: client_message=%ld message=%p area_dispatches=%ld forwarded_area_loaded=%ld synthetic_area_loaded=%ld synthetic_send_depth=%ld payload_size=%u payload=[%s] caller=%s stack=[%s]",
        suppressed_observation,
        observation,
        message,
        area_dispatches_before_send,
        area_loaded_before_send,
        g_synthetic_area_loaded_observations,
        g_synthetic_area_loaded_send_depth,
        payload_size,
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(payload), payload_size, 160).c_str(),
        FormatMainExecutableAddress(_ReturnAddress()).c_str(),
        FormatCurrentStackTrace(1, 8).c_str());
    return 1;
  }

  if (is_area_loaded) {
    area_loaded_observation = InterlockedIncrement(&g_client_to_server_area_loaded_observations);
  }

  const int64_t result = g_client_send_player_to_server_message_original != nullptr
      ? g_client_send_player_to_server_message_original(message, major, minor, payload, payload_size)
      : 0;

  if (interesting) {
    LogFormat(
        L"client-to-server message #%ld end: message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X result=0x%llX/%lld forwarded_area_loaded_count=%ld suppressed_area_loaded_count=%ld%s%s",
        observation,
        message,
        name.c_str(),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        static_cast<unsigned int>(minor),
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        g_client_to_server_area_loaded_observations,
        g_client_to_server_area_loaded_suppressed_observations,
        is_area_loaded ? L" AREA_LOADED" : L"",
        is_synthetic_area_loaded ? L" SYNTHETIC" : L"");
  }

  if (is_area_loaded && area_loaded_observation <= 3) {
    LogFormat(
        L"client Area_AreaLoaded marker #%ld: send_result=0x%llX/%lld total_client_messages=%ld total_server_dispatches=%ld area_dispatches=%ld suppressed_area_loaded=%ld live_dispatches=%ld object_dispatches=%ld%s",
        area_loaded_observation,
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        g_client_to_server_message_observations,
        g_server_to_player_message_observations,
        g_area_load_dispatch_observations,
        g_client_to_server_area_loaded_suppressed_observations,
        g_live_object_dispatch_observations,
        g_object_update_dispatch_observations,
        is_synthetic_area_loaded ? L" SYNTHETIC" : L"");
  }

  return result;
}

std::string NormalizeCharacterResref(std::wstring value) {
  std::string text = LowerAscii(TrimAsciiWhitespace(ToUtf8(value)));
  if (text.size() >= 4 && text.substr(text.size() - 4) == ".bic") {
    text.resize(text.size() - 4);
  }
  if (text.empty() || text.size() > kCResRefTextBytes) {
    return {};
  }

  for (char ch : text) {
    const unsigned char byte = static_cast<unsigned char>(ch);
    const bool ok = (byte >= 'a' && byte <= 'z') ||
        (byte >= '0' && byte <= '9') ||
        byte == '_' ||
        byte == '-';
    if (!ok) {
      return {};
    }
  }
  return text;
}

void CopyCharacterResrefToPayload(std::vector<unsigned char>* payload, size_t offset) {
  if (payload == nullptr || offset + kCResRefTextBytes > payload->size()) {
    return;
  }

  std::fill(payload->begin() + static_cast<ptrdiff_t>(offset),
            payload->begin() + static_cast<ptrdiff_t>(offset + kCResRefTextBytes),
            static_cast<unsigned char>(0));
  const size_t copy_size = std::min(g_auto_character_resref.size(), kCResRefTextBytes);
  std::memcpy(payload->data() + offset, g_auto_character_resref.data(), copy_size);
}

std::vector<unsigned char> BuildAutoCharacterUpdatePayload() {
  std::vector<unsigned char> payload(25, 0);
  payload[3] = 0x18;
  payload[7] = 0x05;
  CopyCharacterResrefToPayload(&payload, 8);
  payload[24] = 0x60;
  return payload;
}

std::vector<unsigned char> BuildAutoCharacterLoginPayload() {
  std::vector<unsigned char> payload(24, 0);
  payload[3] = 0x17;
  CopyCharacterResrefToPayload(&payload, 7);
  payload[23] = 0x60;
  return payload;
}

int64_t InvokeClientSendPlayerToServerMessageOriginal(
    void* message,
    unsigned char major,
    unsigned char minor,
    unsigned char* payload,
    uint32_t payload_size) {
  return g_client_send_player_to_server_message_original != nullptr
      ? g_client_send_player_to_server_message_original(message, major, minor, payload, payload_size)
      : 0;
}

int64_t SendAutoCharacterPayload(
    void* message,
    unsigned char major,
    unsigned char minor,
    std::vector<unsigned char>* payload,
    const wchar_t* label,
    LONG server_dispatch) {
  if (g_client_send_player_to_server_message_original == nullptr) {
    LogFormat(
        L"auto-character %s skipped: server_dispatch=%ld missing SendPlayerToServerMessage original",
        label != nullptr ? label : L"<unknown>",
        server_dispatch);
    return 0;
  }

  unsigned char* const payload_data = payload != nullptr && !payload->empty() ? payload->data() : nullptr;
  const uint32_t payload_size = payload != nullptr ? static_cast<uint32_t>(payload->size()) : 0;
  LogFormat(
      L"auto-character %s send begin: server_dispatch=%ld target='%s' major=%u minor=%u payload_size=%u payload=[%s]",
      label != nullptr ? label : L"<unknown>",
      server_dispatch,
      NarrowToWide(g_auto_character_resref).c_str(),
      static_cast<unsigned int>(major),
      static_cast<unsigned int>(minor),
      payload_size,
      FormatLimitedBytes(payload_data, payload_size, 64).c_str());
  const int64_t result = IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY")
      ? InvokeClientSendPlayerToServerMessageOriginal(message, major, minor, payload_data, payload_size)
      : HookedClientSendPlayerToServerMessage(message, major, minor, payload_data, payload_size);
  LogFormat(
      L"auto-character %s send end: server_dispatch=%ld target='%s' result=0x%llX/%lld",
      label != nullptr ? label : L"<unknown>",
      server_dispatch,
      NarrowToWide(g_auto_character_resref).c_str(),
      static_cast<unsigned long long>(result),
      static_cast<long long>(result));
  return result;
}

bool DriverAutoOpenInventoryEnabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_AUTO_OPEN_INVENTORY");
}

DWORD GetDriverAutoOpenInventoryDelayMs() {
  uint32_t milliseconds = 2000;
  const std::wstring configured =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS");
  if (!configured.empty() &&
      (!ParseUnsigned32Flexible(configured, &milliseconds) || milliseconds > 60000)) {
    LogFormat(
        L"driver-only auto-inventory delay override ignored: invalid HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS='%s'",
        configured.c_str());
    milliseconds = 2000;
  }
  return milliseconds;
}

bool LooksLikeDriverSelfPlayerObjectId(uint32_t object_id) {
  return object_id != 0 &&
      object_id != 0xFFFFFFFFu &&
      (object_id & 0xFF000000u) == 0xFF000000u;
}

uint32_t GetDriverSelfPlayerObjectId() {
  return static_cast<uint32_t>(
      InterlockedCompareExchange(&g_driver_player_self_object_id_bits, 0, 0));
}

void RememberDriverSelfPlayerObjectId(
    uint32_t object_id,
    const wchar_t* source,
    const unsigned char* packet,
    uint32_t packet_size) {
  if (!LooksLikeDriverSelfPlayerObjectId(object_id)) {
    return;
  }

  const LONG desired = static_cast<LONG>(object_id);
  const LONG previous =
      InterlockedCompareExchange(&g_driver_player_self_object_id_bits, desired, 0);
  const LONG current =
      previous == 0 ? desired : InterlockedCompareExchange(&g_driver_player_self_object_id_bits, 0, 0);
  const LONG observation = InterlockedIncrement(&g_driver_player_self_candidate_observations);
  if (previous != 0 && previous != desired) {
    if (observation <= 12) {
      LogFormat(
          L"driver-only self player object id candidate ignored #%ld: existing=0x%08X candidate=0x%08X source=%s packet=[%s]",
          observation,
          static_cast<unsigned int>(static_cast<uint32_t>(previous)),
          object_id,
          source != nullptr ? source : L"<unknown>",
          FormatLimitedBytes(reinterpret_cast<const uint8_t*>(packet), packet_size, 96).c_str());
    }
    return;
  }

  if (observation <= 12 || previous == 0) {
    LogFormat(
        L"driver-only self player object id candidate #%ld: id=0x%08X source=%s stored=%d packet=[%s]",
        observation,
        static_cast<unsigned int>(static_cast<uint32_t>(current)),
        source != nullptr ? source : L"<unknown>",
        previous == 0 ? 1 : 0,
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(packet), packet_size, 96).c_str());
  }
}

void TryRememberDriverSelfPlayerObjectIdFromPlayerList(
    const unsigned char* buffer,
    uint32_t size) {
  if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY") ||
      buffer == nullptr ||
      size < 15 ||
      buffer[0] != 'P' ||
      buffer[1] != 10 ||
      (buffer[2] != 1 && buffer[2] != 2)) {
    return;
  }

  const uint32_t minor = buffer[2];
  const uint32_t entry_offset = minor == 1 ? 8u : 7u;
  if (minor == 1 && (size <= 7 || buffer[7] == 0)) {
    return;
  }

  uint32_t candidate = 0;
  if (TryReadU32LeFromRaw(buffer, size, entry_offset + 4u, &candidate) &&
      LooksLikeDriverSelfPlayerObjectId(candidate)) {
    RememberDriverSelfPlayerObjectId(
        candidate,
        minor == 1 ? L"PlayerList_All player_object" : L"PlayerList_Add player_object",
        buffer,
        size);
  }
}

bool TryOpenDriverLocalSelfInventory(const wchar_t* reason, LONG trigger_observation) {
  const LONG observation =
      InterlockedIncrement(&g_driver_auto_inventory_local_open_observations);
  const uint32_t self_object_id = GetDriverSelfPlayerObjectId();
  const bool self_known = LooksLikeDriverSelfPlayerObjectId(self_object_id);
  static constexpr uint32_t kCurrentPlayerOwnerId = 0x7F000000u;
  void* const app_manager = GetCurrentAppManager();
  void* const client_app = GetClientExoAppFromAppManager(app_manager);
  void* const client_internal = GetClientExoAppInternalFromAppManager(app_manager);
  void* const panel_owner_root = ReadPointerField(client_internal, 0x90);
  void* const fallback_panel = ReadPointerField(panel_owner_root, 0x48);
  auto* const get_inventory_panel =
      ResolveMainRva<ClientAppGetInventoryPanelFn>(kEeClientGetInventoryPanelRva);
  auto* const open_inventory_panel =
      ResolveMainRva<ClientInventoryPanelOpenFn>(kEeClientOpenInventoryPanelRva);

  void* panel = nullptr;
  bool panel_exception_seen = false;
  if (get_inventory_panel != nullptr && client_app != nullptr) {
    __try {
      panel = get_inventory_panel(client_app);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      panel_exception_seen = true;
      panel = nullptr;
    }
  }
  if (panel == nullptr) {
    panel = fallback_panel;
  }

  const uint32_t before_state = ReadUInt32Field(panel, 0xB0, 0xFFFFFFFFu);
  const uint32_t before_owner = ReadUInt32Field(panel, 0x2F68, 0xFFFFFFFFu);
  bool opened = false;
  bool open_exception_seen = false;
  if (open_inventory_panel != nullptr && panel != nullptr) {
    __try {
      open_inventory_panel(panel, 1, 1, kCurrentPlayerOwnerId);
      opened = true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      open_exception_seen = true;
      opened = false;
    }
  }
  const uint32_t after_state = ReadUInt32Field(panel, 0xB0, 0xFFFFFFFFu);
  const uint32_t after_owner = ReadUInt32Field(panel, 0x2F68, 0xFFFFFFFFu);

  LogFormat(
      L"driver-only local inventory open #%ld: reason=%s trigger=%ld self-known=%d self=0x%08X app=%p client=%p internal=%p owner-root=%p panel-rva=%p panel=%p fallback-panel=%p open-rva=%p owner-request=0x%08X state=%u->%u owner=0x%08X->0x%08X opened=%d panel-exception=%d open-exception=%d",
      observation,
      reason != nullptr ? reason : L"<unknown>",
      trigger_observation,
      self_known ? 1 : 0,
      self_object_id,
      app_manager,
      client_app,
      client_internal,
      panel_owner_root,
      get_inventory_panel,
      panel,
      fallback_panel,
      open_inventory_panel,
      kCurrentPlayerOwnerId,
      before_state,
      after_state,
      before_owner,
      after_owner,
      opened ? 1 : 0,
      panel_exception_seen ? 1 : 0,
      open_exception_seen ? 1 : 0);
  return opened;
}

std::vector<unsigned char> BuildDriverAutoInventoryStatusPayload() {
  // EE's GuiInventory_Status self-open payload observed from the real client:
  // this SendPlayerToServerMessage call receives a three-byte prefix that is
  // skipped by the serializer, then BOOL open=true, OBJECTID self=0xFFFFFFFD,
  // plus the CNW fragment tail byte.
  return std::vector<unsigned char>{
      0x00, 0x00, 0x00,
      0x0B, 0x00, 0x00, 0x00,
      0xFD, 0xFF, 0xFF, 0xFF,
      0x90};
}

int64_t SendDriverAutoInventoryStatusPayload(
    void* message,
    const wchar_t* reason,
    LONG trigger_observation) {
  if (g_client_send_player_to_server_message_original == nullptr || message == nullptr) {
    LogFormat(
        L"driver-only auto-inventory skipped: reason=%s trigger=%ld message=%p original=%p",
        reason != nullptr ? reason : L"<unknown>",
        trigger_observation,
        message,
        g_client_send_player_to_server_message_original);
    return 0;
  }

  std::vector<unsigned char> payload = BuildDriverAutoInventoryStatusPayload();
  LogFormat(
      L"driver-only auto-inventory send begin: reason=%s trigger=%ld message=%p major=13 minor=1 payload=[%s]",
      reason != nullptr ? reason : L"<unknown>",
      trigger_observation,
      message,
      FormatLimitedBytes(payload.data(), static_cast<uint32_t>(payload.size()), 64).c_str());
  const int64_t result = InvokeClientSendPlayerToServerMessageOriginal(
      message,
      13,
      1,
      payload.data(),
      static_cast<uint32_t>(payload.size()));
  const bool local_opened =
      TryOpenDriverLocalSelfInventory(reason, trigger_observation);
  LogFormat(
      L"driver-only auto-inventory send end: reason=%s trigger=%ld result=0x%llX/%lld local-opened=%d",
      reason != nullptr ? reason : L"<unknown>",
      trigger_observation,
      static_cast<unsigned long long>(result),
      static_cast<long long>(result),
      local_opened ? 1 : 0);
  return result;
}

struct DriverAutoInventoryOpenRequest {
  void* message = nullptr;
  LONG trigger_observation = 0;
  DWORD delay_ms = 0;
  wchar_t reason[96]{};
};

DWORD WINAPI DriverAutoInventoryOpenWorker(LPVOID context) {
  auto* request = static_cast<DriverAutoInventoryOpenRequest*>(context);
  const DriverAutoInventoryOpenRequest local =
      request != nullptr ? *request : DriverAutoInventoryOpenRequest{};
  if (request != nullptr) {
    HeapFree(GetProcessHeap(), 0, request);
  }

  if (local.delay_ms != 0) {
    Sleep(local.delay_ms);
  }
  InterlockedExchange(&g_driver_auto_inventory_open_scheduled, 0);
  SendDriverAutoInventoryStatusPayload(
      local.message,
      local.reason[0] != L'\0' ? local.reason : L"<unknown>",
      local.trigger_observation);
  return 0;
}

void ScheduleDriverAutoOpenInventory(
    void* message,
    const wchar_t* reason,
    LONG trigger_observation) {
  if (!DriverAutoOpenInventoryEnabled()) {
    return;
  }
  if (message == nullptr) {
    LogFormat(
        L"driver-only auto-inventory not scheduled: reason=%s trigger=%ld missing message context",
        reason != nullptr ? reason : L"<unknown>",
        trigger_observation);
    return;
  }
  if (InterlockedCompareExchange(&g_driver_auto_inventory_open_scheduled, 1, 0) != 0) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_driver_auto_inventory_open_observations);
  const DWORD delay_ms = GetDriverAutoOpenInventoryDelayMs();
  g_driver_auto_inventory_open_due_tick = GetTickCount64() + delay_ms;
  g_driver_auto_inventory_open_trigger_observation = trigger_observation;
  wcsncpy_s(
      g_driver_auto_inventory_open_reason,
      reason != nullptr ? reason : L"<unknown>",
      _TRUNCATE);

  LogFormat(
      L"driver-only auto-inventory scheduled #%ld: reason=%s trigger=%ld delay-ms=%lu due-tick=%llu seed-message=%p dispatch-thread=1",
      observation,
      reason != nullptr ? reason : L"<unknown>",
      trigger_observation,
      delay_ms,
      g_driver_auto_inventory_open_due_tick,
      message);
}

void TryDispatchPendingDriverAutoOpenInventory(void* message, LONG server_dispatch) {
  if (!DriverAutoOpenInventoryEnabled()) {
    return;
  }
  if (InterlockedCompareExchange(&g_driver_auto_inventory_open_scheduled, 1, 1) != 1) {
    return;
  }
  const ULONGLONG now = GetTickCount64();
  if (g_driver_auto_inventory_open_due_tick != 0 && now < g_driver_auto_inventory_open_due_tick) {
    return;
  }
  if (InterlockedCompareExchange(&g_driver_auto_inventory_open_scheduled, 2, 1) != 1) {
    return;
  }

  LogFormat(
      L"driver-only auto-inventory dispatching on server message: server_dispatch=%ld trigger=%ld due-tick=%llu now=%llu reason=%s message=%p",
      server_dispatch,
      g_driver_auto_inventory_open_trigger_observation,
      g_driver_auto_inventory_open_due_tick,
      now,
      g_driver_auto_inventory_open_reason[0] != L'\0' ? g_driver_auto_inventory_open_reason : L"<unknown>",
      message);
  SendDriverAutoInventoryStatusPayload(
      message,
      g_driver_auto_inventory_open_reason[0] != L'\0' ? g_driver_auto_inventory_open_reason : L"<unknown>",
      g_driver_auto_inventory_open_trigger_observation);
}

void* GetPregameScreenFromAppManager(void* app_manager, void** direct_internal, void** client_internal) {
  if (direct_internal != nullptr) {
    *direct_internal = nullptr;
  }
  if (client_internal != nullptr) {
    *client_internal = nullptr;
  }
  if (app_manager == nullptr) {
    return nullptr;
  }

  void* const direct = ReadPointerField(app_manager, 0x08);
  if (direct_internal != nullptr) {
    *direct_internal = direct;
  }
  void* const direct_pregame = ReadPointerField(direct, 0xB0);
  if (direct_pregame != nullptr) {
    return direct_pregame;
  }

  void* const client = GetClientExoAppFromAppManager(app_manager);
  void* const internal = ReadPointerField(client, 0x08);
  if (client_internal != nullptr) {
    *client_internal = internal;
  }
  return ReadPointerField(internal, 0xB0);
}

bool TryInvokePregameCharacterListModalCallback(
    PregameCharacterListModalCallbackFn callback,
    void* modal,
    int button_id,
    int64_t* result) {
  if (result != nullptr) {
    *result = 0;
  }
  if (callback == nullptr || modal == nullptr) {
    return false;
  }

  __try {
    const int64_t callback_result = callback(modal, button_id);
    if (result != nullptr) {
      *result = callback_result;
    }
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool TryDeactivatePregameCharacterListModal(GuiModalPanelDeactivateFn deactivate, void* modal) {
  if (deactivate == nullptr || modal == nullptr) {
    return false;
  }

  __try {
    deactivate(modal, 0);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool TryInvokePregameLeaveScreen(void* pregame, void** leave_function) {
  if (leave_function != nullptr) {
    *leave_function = nullptr;
  }
  if (pregame == nullptr) {
    return false;
  }

  void* const vtable = ReadPointerField(pregame, 0x00);
  void* const target = ReadPointerField(vtable, 0x148);
  if (leave_function != nullptr) {
    *leave_function = target;
  }
  if (target == nullptr) {
    return false;
  }

  __try {
    reinterpret_cast<PregameLeaveScreenFn>(target)(pregame);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

bool TryInvokeGuiPanelNoArg(GuiPanelNoArgFn function, void* panel) {
  if (function == nullptr || panel == nullptr) {
    return false;
  }

  __try {
    function(panel);
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

void TryCloseAutoCharacterPregameUi(LONG server_dispatch) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_CHARACTER_UI_CLOSE")) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_auto_character_ui_close_observations);
  void* const app_manager = GetCurrentAppManager();
  void* direct_internal = nullptr;
  void* client_internal = nullptr;
  void* const pregame = GetPregameScreenFromAppManager(app_manager, &direct_internal, &client_internal);
  void* const character_modal = ReadPointerField(pregame, 0x2C0);
  void* const character_info_panel = ReadPointerField(pregame, 0x1250);
  void* const character_list = ReadPointerField(pregame, 0x1258);
  const uint32_t selected_index = ReadUInt32Field(pregame, 0x1260, 0xFFFFFFFFu);
  const uint32_t pregame_state = ReadUInt32Field(pregame, 0x1C68, 0xFFFFFFFFu);
  void* const pregame_vtable = ReadPointerField(pregame, 0x00);
  void* const modal_vtable = ReadPointerField(character_modal, 0x00);
  void* const modal_owner = ReadPointerField(character_modal, 0x80);
  void* const modal_callback = ReadPointerField(character_modal, 0x88);
  void* leave_function = nullptr;

  HMODULE const main_module = GetModuleHandleW(nullptr);
  auto* const expected_callback = main_module != nullptr
      ? reinterpret_cast<uint8_t*>(main_module) + kEePregameCharacterListModalCallbackRva
      : nullptr;
  const bool callback_matches = modal_callback != nullptr && modal_callback == expected_callback;

  LogFormat(
      L"auto-character UI close #%ld begin: server_dispatch=%ld app_manager=%p direct_internal=%p client_internal=%p pregame=%p pregame_vtable=%p state=%u/0x%08X selected=%u/0x%08X modal=%p modal_vtable=%p owner=%p callback=%p expected=%p matches=%d info_panel=%p list=%p target='%s'",
      observation,
      server_dispatch,
      app_manager,
      direct_internal,
      client_internal,
      pregame,
      pregame_vtable,
      pregame_state,
      pregame_state,
      selected_index,
      selected_index,
      character_modal,
      modal_vtable,
      modal_owner,
      modal_callback,
      expected_callback,
      callback_matches ? 1 : 0,
      character_info_panel,
      character_list,
      NarrowToWide(g_auto_character_resref).c_str());

  bool pregame_left = false;
  if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_CHARACTER_PREGAME_LEAVE")) {
    pregame_left = TryInvokePregameLeaveScreen(pregame, &leave_function);
  }

  bool invoked = false;
  int64_t callback_result = 0;
  if (!pregame_left && character_modal != nullptr && callback_matches) {
    auto* const callback = reinterpret_cast<PregameCharacterListModalCallbackFn>(expected_callback);
    invoked = TryInvokePregameCharacterListModalCallback(callback, character_modal, 3, &callback_result);
  }

  bool deactivated = false;
  if (!pregame_left && !invoked && character_modal != nullptr) {
    auto* const deactivate =
        ResolveMainExport<GuiModalPanelDeactivateFn>("?Deactivate@CGuiModalPanel@@QEAAXH@Z");
    deactivated = TryDeactivatePregameCharacterListModal(deactivate, character_modal);
  }

  bool hid_info_panel = false;
  bool hid_pregame = false;
  if (!pregame_left && !invoked) {
    auto* const hide_panel = ResolveMainExport<GuiPanelNoArgFn>("?Hide@CGuiPanel@@QEAAXXZ");
    hid_info_panel = TryInvokeGuiPanelNoArg(hide_panel, character_info_panel);
    hid_pregame = TryInvokeGuiPanelNoArg(hide_panel, pregame);
  }

  LogFormat(
      L"auto-character UI close #%ld end: server_dispatch=%ld pregame_leave=%d leave_function=%p invoked_callback=%d callback_result=0x%llX/%lld fallback_deactivate=%d hid_info_panel=%d hid_pregame=%d pregame_state_after=%u/0x%08X modal_after=%p",
      observation,
      server_dispatch,
      pregame_left ? 1 : 0,
      leave_function,
      invoked ? 1 : 0,
      static_cast<unsigned long long>(callback_result),
      static_cast<long long>(callback_result),
      deactivated ? 1 : 0,
      hid_info_panel ? 1 : 0,
      hid_pregame ? 1 : 0,
      ReadUInt32Field(pregame, 0x1C68, 0xFFFFFFFFu),
      ReadUInt32Field(pregame, 0x1C68, 0xFFFFFFFFu),
      ReadPointerField(pregame, 0x2C0));
}

std::string ReadPregameCharacterEntryResref(void* entry) {
  CResRefView resref{};
  if (entry == nullptr ||
      !TryCopyBytes(
          reinterpret_cast<const char*>(static_cast<uint8_t*>(entry) + 0x10),
          sizeof(resref),
          reinterpret_cast<char*>(&resref))) {
    return {};
  }

  return LowerAscii(CResRefToAscii(&resref));
}

bool TrySelectAutoCharacterInPregameList(LONG server_dispatch, const wchar_t* reason) {
  if (g_auto_character_resref.empty()) {
    return false;
  }

  void* const app_manager = GetCurrentAppManager();
  void* direct_internal = nullptr;
  void* client_internal = nullptr;
  void* const pregame = GetPregameScreenFromAppManager(app_manager, &direct_internal, &client_internal);
  void* const character_list = ReadPointerField(pregame, 0x1258);
  void* const entries = ReadPointerField(character_list, 0x00);
  const uint32_t count = ReadUInt32Field(character_list, 0x08, 0);
  const uint32_t capacity_or_selected_high = ReadUInt32Field(character_list, 0x0C, 0);
  const uint32_t selected_before = ReadUInt32Field(pregame, 0x1260, 0xFFFFFFFFu);
  const uint32_t logged_limit = std::min<uint32_t>(count, 12);
  const uint32_t scan_limit = std::min<uint32_t>(count, 512);
  const std::string target = LowerAscii(g_auto_character_resref);

  if (pregame == nullptr || character_list == nullptr || entries == nullptr || count == 0 || count > 4096) {
    LogFormat(
        L"auto-character pregame selection unavailable: server_dispatch=%ld reason=%s app_manager=%p direct_internal=%p client_internal=%p pregame=%p list=%p entries=%p count=%u raw0c=%u selected=%u/0x%08X target='%s'",
        server_dispatch,
        reason != nullptr ? reason : L"<unknown>",
        app_manager,
        direct_internal,
        client_internal,
        pregame,
        character_list,
        entries,
        count,
        capacity_or_selected_high,
        selected_before,
        selected_before,
        NarrowToWide(target).c_str());
    return false;
  }

  uint32_t matched_index = 0xFFFFFFFFu;
  std::wstring logged_entries;
  for (uint32_t index = 0; index < scan_limit; ++index) {
    void* const entry = ReadPointerField(entries, static_cast<size_t>(index) * sizeof(void*));
    const std::string resref = ReadPregameCharacterEntryResref(entry);
    if (index < logged_limit) {
      if (!logged_entries.empty()) {
        logged_entries += L", ";
      }
      wchar_t entry_text[160]{};
      swprintf_s(
          entry_text,
          L"%u:%p:'%s'",
          index,
          entry,
          NarrowToWide(resref).c_str());
      logged_entries += entry_text;
    }
    if (resref == target) {
      matched_index = index;
      break;
    }
  }

  if (matched_index == 0xFFFFFFFFu) {
    LogFormat(
        L"auto-character pregame selection target not found: server_dispatch=%ld reason=%s pregame=%p list=%p entries=%p count=%u raw0c=%u selected=%u/0x%08X target='%s' first_entries=[%s]%s",
        server_dispatch,
        reason != nullptr ? reason : L"<unknown>",
        pregame,
        character_list,
        entries,
        count,
        capacity_or_selected_high,
        selected_before,
        selected_before,
        NarrowToWide(target).c_str(),
        logged_entries.c_str(),
        count > logged_limit ? L" ..." : L"");
    return false;
  }

  const bool selected = selected_before == matched_index ||
      WriteUInt32AtOffset(pregame, 0x1260, matched_index);
  const uint32_t selected_after = ReadUInt32Field(pregame, 0x1260, 0xFFFFFFFFu);
  LogFormat(
      L"auto-character pregame selection %s: server_dispatch=%ld reason=%s pregame=%p list=%p entries=%p count=%u raw0c=%u selected=%u/0x%08X -> %u/0x%08X target='%s' matched_index=%u first_entries=[%s]%s",
      selected ? L"ready" : L"write failed",
      server_dispatch,
      reason != nullptr ? reason : L"<unknown>",
      pregame,
      character_list,
      entries,
      count,
      capacity_or_selected_high,
      selected_before,
      selected_before,
      selected_after,
      selected_after,
      NarrowToWide(target).c_str(),
      matched_index,
      logged_entries.c_str(),
      count > logged_limit ? L" ..." : L"");
  return selected && selected_after == matched_index;
}

void TryAutoRequestCharacterFromList(void* message, LONG server_dispatch) {
  if (g_auto_character_resref.empty()) {
    return;
  }
  if (InterlockedCompareExchange(&g_auto_character_state, 1, 0) != 0) {
    return;
  }
  InterlockedExchange(&g_auto_character_update_responses, 0);
  InterlockedExchange(&g_auto_character_fallback_play_scheduled, 0);

  const LONG observation = InterlockedIncrement(&g_auto_character_observations);
  std::vector<unsigned char> payload = BuildAutoCharacterUpdatePayload();
  if (!TrySelectAutoCharacterInPregameList(server_dispatch, L"before CharList_RequestUpdateChar") &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_AUTO_CHARACTER_ALLOW_UNSELECTED_REQUEST")) {
    LogFormat(
        L"auto-character #%ld request skipped: server_dispatch=%ld target='%s' no matching selected pregame character yet",
        observation,
        server_dispatch,
        NarrowToWide(g_auto_character_resref).c_str());
    InterlockedExchange(&g_auto_character_state, 0);
    return;
  }

  LogFormat(
      L"auto-character #%ld requesting vault character: server_dispatch=%ld target='%s'",
      observation,
      server_dispatch,
      NarrowToWide(g_auto_character_resref).c_str());
  SendAutoCharacterPayload(message, 17, 3, &payload, L"CharList_RequestUpdateChar", server_dispatch);
}

struct AutoCharacterInitialRequest {
  LONG observation = 0;
  wchar_t reason[96]{};
};

DWORD WINAPI AutoCharacterInitialRequestWorker(LPVOID context) {
  auto* request = static_cast<AutoCharacterInitialRequest*>(context);
  const AutoCharacterInitialRequest local = request != nullptr ? *request : AutoCharacterInitialRequest{};
  if (request != nullptr) {
    HeapFree(GetProcessHeap(), 0, request);
  }

  Sleep(1200);
  InterlockedExchange(&g_auto_character_initial_request_scheduled, 0);

  const LONG state = InterlockedCompareExchange(&g_auto_character_state, 0, 0);
  if (state != 0) {
    LogFormat(
        L"auto-character initial request fallback skipped #%ld: reason=%s state=%ld",
        local.observation,
        local.reason,
        state);
    return 0;
  }

  void* const message = InterlockedCompareExchangePointer(
      &g_auto_character_last_client_message_context,
      nullptr,
      nullptr);
  if (message == nullptr) {
    LogFormat(
        L"auto-character initial request fallback skipped #%ld: reason=%s missing client message context",
        local.observation,
        local.reason);
    return 0;
  }

  LogFormat(
      L"auto-character initial request fallback firing #%ld: reason=%s message=%p",
      local.observation,
      local.reason,
      message);
  TryAutoRequestCharacterFromList(message, 0);
  return 0;
}

void ScheduleAutoCharacterInitialRequestFromLastContext(const wchar_t* reason) {
  if (g_auto_character_resref.empty()) {
    return;
  }
  if (InterlockedCompareExchange(&g_auto_character_state, 0, 0) != 0) {
    return;
  }
  if (InterlockedCompareExchangePointer(
          &g_auto_character_last_client_message_context,
          nullptr,
          nullptr) == nullptr) {
    LogFormat(
        L"auto-character initial request fallback not scheduled: reason=%s missing client message context",
        reason != nullptr ? reason : L"<unknown>");
    return;
  }
  if (InterlockedCompareExchange(&g_auto_character_initial_request_scheduled, 1, 0) != 0) {
    return;
  }

  auto* request = static_cast<AutoCharacterInitialRequest*>(
      HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AutoCharacterInitialRequest)));
  if (request == nullptr) {
    InterlockedExchange(&g_auto_character_initial_request_scheduled, 0);
    LogFormat(
        L"auto-character initial request fallback allocation failed; firing immediately: reason=%s",
        reason != nullptr ? reason : L"<unknown>");
    TryAutoRequestCharacterFromList(
        InterlockedCompareExchangePointer(
            &g_auto_character_last_client_message_context,
            nullptr,
            nullptr),
        0);
    return;
  }

  const LONG observation = InterlockedIncrement(&g_auto_character_observations);
  request->observation = observation;
  wcsncpy_s(
      request->reason,
      reason != nullptr ? reason : L"<unknown>",
      _TRUNCATE);
  wchar_t logged_reason[96]{};
  wcsncpy_s(logged_reason, request->reason, _TRUNCATE);
  HANDLE thread = CreateThread(nullptr, 0, &AutoCharacterInitialRequestWorker, request, 0, nullptr);
  if (thread == nullptr) {
    const DWORD error = GetLastError();
    HeapFree(GetProcessHeap(), 0, request);
    InterlockedExchange(&g_auto_character_initial_request_scheduled, 0);
    LogFormat(
        L"auto-character initial request fallback worker creation failed: error=%lu reason=%s; firing immediately",
        error,
        reason != nullptr ? reason : L"<unknown>");
    TryAutoRequestCharacterFromList(
        InterlockedCompareExchangePointer(
            &g_auto_character_last_client_message_context,
            nullptr,
            nullptr),
        0);
    return;
  }

  LogFormat(
      L"auto-character initial request fallback scheduled #%ld: reason=%s",
      observation,
      logged_reason);
  CloseHandle(thread);
}

void TryAutoPlayCharacterUpdate(void* message, LONG server_dispatch, const wchar_t* reason) {
  if (g_auto_character_resref.empty()) {
    return;
  }
  if (InterlockedCompareExchange(&g_auto_character_state, 2, 1) != 1) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_auto_character_observations);
  LogFormat(
      L"auto-character #%ld %s; sending Play/Login: server_dispatch=%ld target='%s'",
      observation,
      reason != nullptr ? reason : L"received character update",
      server_dispatch,
      NarrowToWide(g_auto_character_resref).c_str());
  SendAutoCharacterPayload(message, 49, 2, nullptr, L"PlayModuleCharacterList_Stop", server_dispatch);
  std::vector<unsigned char> payload = BuildAutoCharacterLoginPayload();
  SendAutoCharacterPayload(message, 2, 17, &payload, L"Login_ServerSubDirectoryCharacter", server_dispatch);
  TryCloseAutoCharacterPregameUi(server_dispatch);
}

void TryAutoPlayConfirmedCharacterUpdate(void* message, LONG server_dispatch) {
  TryAutoPlayCharacterUpdate(message, server_dispatch, L"received target BIC update");
}

struct AutoCharacterFallbackPlayRequest {
  void* message = nullptr;
  LONG server_dispatch = 0;
  LONG update_observation = 0;
  uint32_t packet_size = 0;
};

DWORD WINAPI AutoCharacterFallbackPlayWorker(LPVOID context) {
  auto* request = static_cast<AutoCharacterFallbackPlayRequest*>(context);
  const AutoCharacterFallbackPlayRequest local = request != nullptr ? *request : AutoCharacterFallbackPlayRequest{};
  if (request != nullptr) {
    HeapFree(GetProcessHeap(), 0, request);
  }

  Sleep(650);
  InterlockedExchange(&g_auto_character_fallback_play_scheduled, 0);

  const LONG state = InterlockedCompareExchange(&g_auto_character_state, 1, 1);
  if (state != 1) {
    LogFormat(
        L"auto-character fallback skipped: update_observation=%ld server_dispatch=%ld state=%ld",
        local.update_observation,
        local.server_dispatch,
        state);
    return 0;
  }

  LogFormat(
      L"auto-character fallback firing after post-request BIC update quiet period: update_observation=%ld server_dispatch=%ld size=%u",
      local.update_observation,
      local.server_dispatch,
      local.packet_size);
  TryAutoPlayCharacterUpdate(
      local.message,
      local.server_dispatch,
      L"fallback after post-request BIC update quiet period");
  return 0;
}

void ScheduleAutoCharacterFallbackPlay(void* message, const unsigned char* buffer, uint32_t size, LONG server_dispatch) {
  const LONG state = InterlockedCompareExchange(&g_auto_character_state, 1, 1);
  if (state != 1) {
    return;
  }

  const LONG update_observation = InterlockedIncrement(&g_auto_character_update_responses);
  const unsigned char* const preview_buffer = size > 3 && buffer != nullptr ? buffer + 3 : buffer;
  const uint32_t preview_size = size > 3 ? size - 3 : size;
  const std::string preview = ExtractAsciiPacketText(preview_buffer, preview_size, 220);
  LogFormat(
      L"auto-character post-request BIC update #%ld did not expose target resref; scheduling fallback: server_dispatch=%ld size=%u target='%s' preview='%s'",
      update_observation,
      server_dispatch,
      size,
      NarrowToWide(g_auto_character_resref).c_str(),
      NarrowToWide(preview).c_str());

  if (InterlockedCompareExchange(&g_auto_character_fallback_play_scheduled, 1, 0) != 0) {
    LogFormat(
        L"auto-character fallback already scheduled: update_observation=%ld server_dispatch=%ld",
        update_observation,
        server_dispatch);
    return;
  }

  auto* request = static_cast<AutoCharacterFallbackPlayRequest*>(
      HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AutoCharacterFallbackPlayRequest)));
  if (request == nullptr) {
    InterlockedExchange(&g_auto_character_fallback_play_scheduled, 0);
    LogFormat(
        L"auto-character fallback allocation failed; firing immediately: update_observation=%ld server_dispatch=%ld",
        update_observation,
        server_dispatch);
    TryAutoPlayCharacterUpdate(
        message,
        server_dispatch,
        L"fallback after post-request BIC update (allocation unavailable)");
    return;
  }

  request->message = message;
  request->server_dispatch = server_dispatch;
  request->update_observation = update_observation;
  request->packet_size = size;

  HANDLE thread = CreateThread(nullptr, 0, &AutoCharacterFallbackPlayWorker, request, 0, nullptr);
  if (thread == nullptr) {
    const DWORD error = GetLastError();
    HeapFree(GetProcessHeap(), 0, request);
    InterlockedExchange(&g_auto_character_fallback_play_scheduled, 0);
    LogFormat(
        L"auto-character fallback worker creation failed: error=%lu update_observation=%ld server_dispatch=%ld; firing immediately",
        error,
        update_observation,
        server_dispatch);
    TryAutoPlayCharacterUpdate(
        message,
        server_dispatch,
        L"fallback after post-request BIC update (worker unavailable)");
    return;
  }

  CloseHandle(thread);
}

void TryAutoPlayCharacterAfterUpdate(
    void* message,
    const unsigned char* buffer,
    uint32_t size,
    LONG server_dispatch) {
  if (g_auto_character_resref.empty() || buffer == nullptr || size == 0) {
    return;
  }
  if (InterlockedCompareExchange(&g_auto_character_state, 1, 1) != 1) {
    return;
  }
  if (!PacketContainsAsciiNoCase(buffer, size, g_auto_character_resref.c_str())) {
    ScheduleAutoCharacterFallbackPlay(message, buffer, size, server_dispatch);
    return;
  }

  TryAutoPlayConfirmedCharacterUpdate(message, server_dispatch);
}

int64_t __fastcall HookedClientSendPlayerToServerMessageDriverOnly(
    void* message,
    unsigned char major,
    unsigned char minor,
    unsigned char* payload,
    uint32_t payload_size) {
  InterlockedExchangePointer(&g_auto_character_last_client_message_context, message);
  const LONG observation = InterlockedIncrement(&g_client_to_server_message_observations);
  const std::wstring name = LookupMajorMinorNameWide(major, minor);
  const bool is_area_loaded = major == 4 && minor == 3;
  const bool is_input = major == 6;
  const bool is_load_or_area_related = major == 2 || major == 3 || major == 4 || major == 44;
  const bool is_gui_inventory = major == 13;
  const std::wstring payload_detail =
      FormatClientInputPayloadDetail(major, minor, payload, payload_size);
  const bool interesting =
      observation <= 260 || is_input || is_load_or_area_related || is_gui_inventory ||
      major == 18 || major == 49 || major == 50;
  if (interesting) {
    LogFormat(
        L"driver-only client-to-server message #%ld begin: message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X payload=%p payload_size=%u header=[70 %02X %02X] payload=[%s]%s caller=%s stack=[%s]",
        observation,
        message,
        name.c_str(),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        static_cast<unsigned int>(minor),
        payload,
        payload_size,
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(payload), payload_size, 160).c_str(),
        payload_detail.c_str(),
        FormatMainExecutableAddress(_ReturnAddress()).c_str(),
        is_input ? FormatCurrentStackTrace(1, 8).c_str() : L"-");
  }
  if (major == 17 &&
      minor == 3 &&
      !g_auto_character_resref.empty() &&
      !PacketContainsAsciiNoCase(payload, payload_size, g_auto_character_resref.c_str())) {
    const LONG auto_state = InterlockedCompareExchange(&g_auto_character_state, 0, 0);
    const bool target_selectable =
        auto_state == 1 ||
        TrySelectAutoCharacterInPregameList(
            observation,
            L"driver-only blocking native non-target CharList_RequestUpdateChar");
    if (target_selectable) {
      LogFormat(
          L"driver-only auto-character suppressed native non-target CharList_RequestUpdateChar #%ld: state=%ld target='%s' payload=[%s]",
          observation,
          auto_state,
          NarrowToWide(g_auto_character_resref).c_str(),
          FormatLimitedBytes(reinterpret_cast<const uint8_t*>(payload), payload_size, 160).c_str());
      if (auto_state == 0) {
        ScheduleAutoCharacterInitialRequestFromLastContext(
            L"driver-only native non-target CharList_RequestUpdateChar suppressed");
      }
      return 1;
    }
  }
  const int64_t result =
      InvokeClientSendPlayerToServerMessageOriginal(message, major, minor, payload, payload_size);
  if ((major == 17 && minor == 1) || (major == 49 && minor == 1)) {
    ScheduleAutoCharacterInitialRequestFromLastContext(
        major == 17 ? L"driver-only CharList_Request sent"
                    : L"driver-only PlayModuleCharacterList_Start sent");
  }
  if (interesting) {
    LogFormat(
        L"driver-only client-to-server message #%ld end: message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X result=0x%llX/%lld",
        observation,
        message,
        name.c_str(),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        static_cast<unsigned int>(minor),
      static_cast<unsigned long long>(result),
      static_cast<long long>(result));
  }
  if (is_area_loaded) {
    ScheduleDriverAutoOpenInventory(message, L"Area_AreaLoaded", observation);
  }
  return result;
}

uint32_t GetDriverServerDispatchLogLimit() {
  uint32_t limit = 160;
  const std::wstring configured = GetEnvironmentString(L"HG_BRIDGE_DRIVER_DISPATCH_LOG_LIMIT");
  if (!configured.empty()) {
    uint32_t parsed = 0;
    if (ParseUnsigned32(configured, &parsed)) {
      limit = parsed;
    }
  }
  return limit;
}

uint32_t GetDriverNetLayerLogLimit() {
  uint32_t limit = 800;
  const std::wstring configured = GetEnvironmentString(L"HG_BRIDGE_DRIVER_NETLAYER_LOG_LIMIT");
  if (!configured.empty()) {
    uint32_t parsed = 0;
    if (ParseUnsigned32(configured, &parsed)) {
      limit = parsed;
    }
  }
  return limit;
}

bool ShouldLogDriverNetLayerObservation(LONG observation) {
  const uint32_t limit = GetDriverNetLayerLogLimit();
  return limit == 0 || static_cast<uint32_t>(observation) <= limit || PacketDumpEnabled();
}

std::wstring FormatDriverReliableFrameSummary(const unsigned char* buffer, uint32_t size) {
  if (buffer == nullptr || size == 0) {
    return L"<empty>";
  }
  if (size < 12 || buffer[0] != 'M') {
    wchar_t text[512]{};
    swprintf_s(
        text,
        L"m=%d size=%u prefix=[%s]",
        buffer[0] == 'M' ? 1 : 0,
        size,
        FormatLimitedBytes(reinterpret_cast<const uint8_t*>(buffer), size, 96).c_str());
    return text;
  }

  const uint16_t crc = ReadU16BeRaw(buffer + 1);
  const uint16_t sequence = ReadU16BeRaw(buffer + 3);
  const uint16_t ack_sequence = ReadU16BeRaw(buffer + 5);
  const uint8_t flags = buffer[7];
  const uint16_t packetized_sequence = ReadU16BeRaw(buffer + 8);
  const uint16_t packetized_length = ReadU16BeRaw(buffer + 10);
  const uint32_t payload_size = size - 12;
  std::wstring high_detail = L"high=<none>";
  if (size >= 15 && (buffer[12] == 0x50 || buffer[12] == 0x70)) {
    const unsigned char major = buffer[13];
    const unsigned char minor = buffer[14];
    const std::wstring name = LookupMajorMinorNameWide(major, minor);
    wchar_t high_text[256]{};
    swprintf_s(
        high_text,
        L"high=[%02X %02X %02X] name='%s'",
        static_cast<unsigned int>(buffer[12]),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        name.c_str());
    high_detail = high_text;
  } else {
    const uint32_t scan_end = size >= 3 ? std::min<uint32_t>(size - 2, 80) : 0;
    for (uint32_t index = 12; index < scan_end; ++index) {
      if (buffer[index] != 0x50 && buffer[index] != 0x70) {
        continue;
      }
      const unsigned char major = buffer[index + 1];
      const unsigned char minor = buffer[index + 2];
      const std::wstring name = LookupMajorMinorNameWide(major, minor);
      wchar_t high_text[256]{};
      swprintf_s(
          high_text,
          L"high=<embedded@%u [%02X %02X %02X] name='%s'>",
          index,
          static_cast<unsigned int>(buffer[index]),
          static_cast<unsigned int>(major),
          static_cast<unsigned int>(minor),
          name.c_str());
      high_detail = high_text;
      break;
    }
  }

  wchar_t text[1024]{};
  swprintf_s(
      text,
      L"seq=%u ack=%u crc=0x%04X flags=0x%02X pkt_seq=%u pkt_len=%u payload=%u %s prefix=[%s]",
      static_cast<unsigned int>(sequence),
      static_cast<unsigned int>(ack_sequence),
      static_cast<unsigned int>(crc),
      static_cast<unsigned int>(flags),
      static_cast<unsigned int>(packetized_sequence),
      static_cast<unsigned int>(packetized_length),
      payload_size,
      high_detail.c_str(),
      FormatLimitedBytes(reinterpret_cast<const uint8_t*>(buffer), size, 128).c_str());
  return text;
}

int64_t __fastcall HookedDriverNetLayerFrameReceive(void* window, unsigned char* frame, uint32_t size) {
  const LONG observation = InterlockedIncrement(&g_driver_frame_receive_observations);
  const bool should_log = ShouldLogDriverNetLayerObservation(observation);
  if (should_log) {
    LogFormat(
        L"driver-only net FrameReceive #%ld begin: window=%p size=%u %s",
        observation,
        window,
        size,
        FormatDriverReliableFrameSummary(frame, size).c_str());
  }

  const int64_t result = g_netlayer_window_frame_receive_original != nullptr
      ? g_netlayer_window_frame_receive_original(window, frame, size)
      : 0;
  if (should_log) {
    LogFormat(
        L"driver-only net FrameReceive #%ld end: result=0x%llX/%lld",
        observation,
        static_cast<unsigned long long>(result),
        static_cast<long long>(result));
  }
  return result;
}

int __fastcall HookedDriverNetLayerUnpacketizeFullMessages(void* window, int side) {
  const LONG observation = InterlockedIncrement(&g_driver_unpacketize_observations);
  const bool should_log = ShouldLogDriverNetLayerObservation(observation);
  if (should_log) {
    LogFormat(
        L"driver-only net UnpacketizeFullMessages #%ld begin: window=%p side=%d",
        observation,
        window,
        side);
  }

  const int result = g_netlayer_window_unpacketize_full_messages_original != nullptr
      ? g_netlayer_window_unpacketize_full_messages_original(window, side)
      : 0;
  if (should_log) {
    LogFormat(
        L"driver-only net UnpacketizeFullMessages #%ld end: result=0x%08X/%d",
        observation,
        static_cast<unsigned int>(result),
        result);
  }
  return result;
}

int __fastcall HookedDriverNetLayerUncompressMessage(
    void* netlayer_internal,
    uint32_t player_id,
    unsigned char* buffer,
    uint32_t size) {
  const LONG observation = InterlockedIncrement(&g_driver_uncompress_observations);
  const bool should_log = ShouldLogDriverNetLayerObservation(observation);
  if (should_log) {
    LogFormat(
        L"driver-only net UncompressMessage #%ld begin: internal=%p player=%u size=%u %s",
        observation,
        netlayer_internal,
        player_id,
        size,
        FormatDriverReliableFrameSummary(buffer, size).c_str());
  }

  const int result = g_netlayer_internal_uncompress_message_original != nullptr
      ? g_netlayer_internal_uncompress_message_original(netlayer_internal, player_id, buffer, size)
      : 0;
  if (should_log) {
    LogFormat(
        L"driver-only net UncompressMessage #%ld end: result=0x%08X/%d",
        observation,
        static_cast<unsigned int>(result),
        result);
  }
  return result;
}

bool DriverAutoQuitConfirmEnabled() {
  return IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY") &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DRIVER_AUTO_QUIT_CONFIRM");
}

bool TryCallDriverQuitExport(QuitFn quit, bool* exception_seen) {
  if (quit == nullptr) {
    return false;
  }

  __try {
    quit();
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    if (exception_seen != nullptr) {
      *exception_seen = true;
    }
    return false;
  }

  return true;
}

bool TryCallDriverQuitConfirmationYes(DriverQuitConfirmationYesFn confirm_yes, void* screen) {
  if (confirm_yes == nullptr || screen == nullptr) {
    return false;
  }

  __try {
    confirm_yes(screen);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }

  return true;
}

void SignalDriverAutoQuitConfirm(const wchar_t* reason, int disconnect_result) {
  if (!DriverAutoQuitConfirmEnabled()) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_driver_auto_quit_confirm_observations);
  if (InterlockedCompareExchange(&g_driver_auto_quit_confirm_triggered, 1, 0) != 0) {
    if (observation <= 5) {
      LogFormat(
          L"driver-only auto-quit confirm already triggered #%ld: reason=%s disconnect_result=%d",
          observation,
          reason != nullptr ? reason : L"<null>",
          disconnect_result);
    }
    return;
  }

  // This runs from quit-prompt or disconnect callbacks. Calling the exported
  // global Quit() or posting WM_QUIT re-entrantly from here can crash EE after
  // the real DisconnectFromSession path has already completed, so let the
  // native accepted-close path own shutdown.
  LogFormat(
      L"driver-only auto-quit confirm #%ld: reason=%s disconnect_result=%d quit_export=skipped post_quit=0 thread=%lu",
      observation,
      reason != nullptr ? reason : L"<null>",
      disconnect_result,
      GetCurrentThreadId());
}

bool TryInvokeDriverQuitConfirmationYes(void* screen, const wchar_t* reason) {
  if (screen == nullptr) {
    LogFormat(
        L"driver-only quit confirmation auto-accept skipped: screen=<null> reason=%s",
        reason != nullptr ? reason : L"<null>");
    return false;
  }

  auto* const confirm_yes = ResolveMainRva<DriverQuitConfirmationYesFn>(kEeDriverQuitConfirmationYesRva);
  if (confirm_yes == nullptr) {
    LogFormat(
        L"driver-only quit confirmation auto-accept skipped: callback rva=0x%zX unavailable reason=%s",
        kEeDriverQuitConfirmationYesRva,
        reason != nullptr ? reason : L"<null>");
    return false;
  }

  if (!TryCallDriverQuitConfirmationYes(confirm_yes, screen)) {
    LogFormat(
        L"driver-only quit confirmation auto-accept exception: callback_rva=0x%zX screen=%p reason=%s",
        kEeDriverQuitConfirmationYesRva,
        screen,
        reason != nullptr ? reason : L"<null>");
    return false;
  }

  return true;
}

void __fastcall HookedDriverQuitConfirmationPrompt(void* screen) {
  const LONG observation = InterlockedIncrement(&g_driver_quit_confirmation_prompt_observations);
  if (!DriverAutoQuitConfirmEnabled()) {
    if (observation <= 5) {
      LogFormat(
          L"driver-only quit confirmation prompt #%ld pass-through: screen=%p auto-confirm disabled",
          observation,
          screen);
    }
    if (g_driver_quit_confirmation_prompt_original != nullptr) {
      g_driver_quit_confirmation_prompt_original(screen);
    }
    return;
  }

  if (observation <= 20) {
    LogFormat(
        L"driver-only quit confirmation prompt #%ld auto-accept begin: screen=%p",
        observation,
        screen);
  }

  if (TryInvokeDriverQuitConfirmationYes(screen, L"quit confirmation prompt")) {
    SignalDriverAutoQuitConfirm(L"quit confirmation prompt accepted", 1);
    return;
  }

  LogFormat(
      L"driver-only quit confirmation prompt #%ld auto-accept fallback: screen=%p invoking original prompt",
      observation,
      screen);
  if (g_driver_quit_confirmation_prompt_original != nullptr) {
    g_driver_quit_confirmation_prompt_original(screen);
  }
  SignalDriverAutoQuitConfirm(L"quit confirmation prompt fallback", 0);
}

int __fastcall HookedDriverDisconnectFromSessionInternal(void* netlayer_internal) {
  const LONG observation = InterlockedIncrement(&g_driver_disconnect_from_session_observations);
  if (observation <= 20) {
    LogFormat(
        L"driver-only DisconnectFromSession #%ld begin: internal=%p",
        observation,
        netlayer_internal);
  }

  const int result = g_netlayer_internal_disconnect_from_session_original != nullptr
      ? g_netlayer_internal_disconnect_from_session_original(netlayer_internal)
      : 0;
  if (observation <= 20 || result == 0) {
    LogFormat(
        L"driver-only DisconnectFromSession #%ld end: result=0x%08X/%d",
        observation,
        static_cast<unsigned int>(result),
        result);
  }

  SignalDriverAutoQuitConfirm(L"CNetLayerInternal::DisconnectFromSession", result);
  return result;
}

void LogDriverOnlyServerDispatchIfEnabled(const unsigned char* buffer, uint32_t size) {
  if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_DRIVER_DISPATCH_LOG")) {
    return;
  }

  const LONG observation = InterlockedIncrement(&g_driver_server_dispatch_log_observations);
  const uint32_t limit = GetDriverServerDispatchLogLimit();
  if (limit != 0 && static_cast<uint32_t>(observation) > limit && !PacketDumpEnabled()) {
    return;
  }

  const bool is_p_message = buffer != nullptr && size >= 3 && buffer[0] == 'P';
  const uint8_t major = is_p_message ? buffer[1] : 0;
  const uint8_t minor = is_p_message ? buffer[2] : 0;
  const uint32_t body_size = size >= 3 ? size - 3 : 0;
  uint32_t declared = 0;
  const bool declared_known = is_p_message && size >= 7 &&
      TryReadU32LeFromRaw(buffer, size, 3, &declared);
  const bool declared_underflow = declared_known && declared < 3;
  const uint64_t read_size = declared_known && declared >= 3
      ? static_cast<uint64_t>(declared - 3)
      : 0;
  const bool cnw_split_ok = declared_known && !declared_underflow &&
      read_size <= static_cast<uint64_t>(body_size);
  const uint32_t fragment_bytes = cnw_split_ok
      ? static_cast<uint32_t>(static_cast<uint64_t>(body_size) - read_size)
      : 0;
  const bool first_opcode_known = is_p_message && cnw_split_ok && read_size > 4 && size > 7;
  const uint8_t first_opcode = first_opcode_known ? buffer[7] : 0;

  LogFormat(
      L"driver-only server dispatch raw #%ld: p=%d major=%u minor=%u size=%u body=%u cnw_declared=%u declared_known=%d cnw_read=%llu cnw_fragments=%u cnw_ok=%d first=%s prefix=[%s]",
      observation,
      is_p_message ? 1 : 0,
      static_cast<unsigned int>(major),
      static_cast<unsigned int>(minor),
      size,
      body_size,
      declared,
      declared_known ? 1 : 0,
      static_cast<unsigned long long>(read_size),
      fragment_bytes,
      cnw_split_ok ? 1 : 0,
      FormatLiveObjectOpcode(first_opcode_known, first_opcode).c_str(),
      FormatLimitedBytes(reinterpret_cast<const uint8_t*>(buffer), size, 128).c_str());

  if (is_p_message && major == 5 && minor == 1) {
    std::wstring packet_summary;
    bool transition_related = false;
    if (TryFormatLegacyLiveObjectPacketSummary(buffer, size, &packet_summary, &transition_related)) {
      const LONG summary_observation =
          InterlockedIncrement(&g_live_object_packet_summary_observations);
      LogFormat(
          L"driver-only live-object dispatch summary #%ld/%ld: transition=%d %s",
          summary_observation,
          observation,
          transition_related ? 1 : 0,
          packet_summary.c_str());
    } else {
      LogFormat(
          L"driver-only live-object dispatch summary unavailable #%ld: size=%u cnw_declared=%u prefix=[%s]",
          observation,
          size,
          declared,
          FormatLimitedBytes(reinterpret_cast<const uint8_t*>(buffer), size, 128).c_str());
    }
  }
}

int64_t __fastcall HookedServerToPlayerMessageDriverOnly(void* message, unsigned char* buffer, uint32_t size) {
  const bool driver_dispatch_log_enabled =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_DRIVER_DISPATCH_LOG");
  LogDriverOnlyServerDispatchIfEnabled(buffer, size);

  const bool is_p_message = buffer != nullptr && size >= 3 && buffer[0] == 'P';
  const unsigned char major = is_p_message ? buffer[1] : 0;
  const unsigned char minor = is_p_message ? buffer[2] : 0;
  const bool auto_use_active =
      AutoUseObjectTargetConfigured(g_auto_use_object_stage) || g_auto_use_object_state != 0;
  if (is_p_message) {
    TryRememberDriverSelfPlayerObjectIdFromPlayerList(buffer, size);
  }
  if (auto_use_active &&
      !driver_dispatch_log_enabled &&
      is_p_message &&
      major == 5 &&
      minor == 1) {
    static volatile LONG s_driver_auto_use_scan_observations = 0;
    const LONG auto_use_scan_observation =
        InterlockedIncrement(&s_driver_auto_use_scan_observations);
    std::wstring packet_summary;
    bool transition_related = false;
    const bool parsed =
        TryFormatLegacyLiveObjectPacketSummary(buffer, size, &packet_summary, &transition_related);
    if (auto_use_scan_observation <= 24 || !parsed || transition_related) {
      LogFormat(
          L"driver-only auto-use live-object scan #%ld: parsed=%d transition=%d stage=%ld state=%ld target={%s} %s",
          auto_use_scan_observation,
          parsed ? 1 : 0,
          transition_related ? 1 : 0,
          g_auto_use_object_stage,
          g_auto_use_object_state,
          FormatAutoUseObjectTargetConfig(g_auto_use_object_stage).c_str(),
          parsed ? packet_summary.c_str() : L"<unparsed>");
    }
  }
  const bool target_update_before_original =
      major == 17 && minor == 4 &&
      !g_auto_character_resref.empty() &&
      InterlockedCompareExchange(&g_auto_character_state, 1, 1) == 1 &&
      PacketContainsAsciiNoCase(buffer, size, g_auto_character_resref.c_str());
  const int64_t result = g_server_to_player_message_original != nullptr
      ? g_server_to_player_message_original(message, buffer, size)
      : 0;
  LONG observation = 0;
  if (is_p_message) {
    observation = InterlockedIncrement(&g_server_to_player_message_observations);
    if (major == 17 && minor == 2) {
      TryAutoRequestCharacterFromList(message, observation);
    } else if (major == 17 && minor == 4) {
      if (target_update_before_original) {
        TryAutoPlayConfirmedCharacterUpdate(message, observation);
      } else {
        TryAutoPlayCharacterAfterUpdate(message, buffer, size, observation);
      }
    }
  }
  if (auto_use_active || g_auto_use_object_state != 0) {
    TryDispatchPendingAutoUseObject(message, observation);
  }
  TryDispatchPendingDriverAutoOpenInventory(message, observation);
  return result;
}

constexpr size_t kLegacyQuickbarButtonCount = 36;
constexpr size_t kLegacyQuickbarReadCursorStart = 4;

struct LegacyQuickbarPacketReader {
  const uint8_t* read_buffer = nullptr;
  size_t read_size = 0;
  const uint8_t* fragments = nullptr;
  size_t fragment_size = 0;
  size_t cursor = kLegacyQuickbarReadCursorStart;
  size_t fragment_cursor = 0;
  uint8_t fragment_bit = 0;
  uint8_t final_fragment_bits = 0;
};

struct LegacyQuickbarLocStringField {
  bool custom_tlk = false;
  bool language_selector = false;
  uint32_t string_ref = 0xFFFFFFFFu;
  std::string text;
};

struct LegacyQuickbarActivePropertyEntry {
  uint16_t property = 0;
  uint16_t subtype = 0;
  uint16_t cost_table_value = 0;
  uint8_t param = 0;
};

struct LegacyQuickbarActiveItemProperties {
  bool has_armor_word = false;
  uint16_t armor_word = 0;
  bool name_is_locstring = false;
  LegacyQuickbarLocStringField locstring_name;
  std::string string_name;
  bool post_name_bool1 = false;
  uint32_t cost = 0;
  uint32_t stack_or_charges = 0;
  bool post_name_bool2 = false;
  bool post_name_bool3 = false;
  bool post_name_bool4 = false;
  std::vector<LegacyQuickbarActivePropertyEntry> properties;
  uint8_t state_mask = 0;
  uint8_t value_mask = 0;
  std::vector<uint8_t> value_mask_bytes;
};

struct LegacyQuickbarItemObject {
  bool present = false;
  uint32_t object_id = 0x7F000000u;
  int32_t int_param = -1;
  uint32_t base_item = 0xFFFFFFFFu;
  uint8_t appearance_type = 0xFF;
  bool active_props_skipped = false;
  LegacyQuickbarActiveItemProperties active_props;
  std::vector<uint8_t> appearance_bytes;
  std::string name_preview;
};

struct LegacyQuickbarButton {
  uint8_t source_type = 0;
  bool parsed = false;
  bool spell = false;
  bool no_payload = false;
  bool item_payload = false;
  bool int_payload = false;
  uint32_t int_payload_value = 0;
  bool byte_payload = false;
  uint8_t byte_payload_value = 0;
  bool resref_payload_present = false;
  std::string resref_payload;
  bool string_payload_present = false;
  std::string string_payload;
  bool string_payload2_present = false;
  std::string string_payload2;
  LegacyQuickbarItemObject primary_item;
  LegacyQuickbarItemObject secondary_item;
  uint8_t spell_class = 0;
  uint32_t spell_id = 0;
  uint8_t metamagic = 0;
  uint8_t domain = 0;
  size_t source_offset = 0;
  size_t next_offset = 0;
  size_t source_fragment_offset = 0;
  uint8_t source_fragment_bit = 0;
  size_t next_fragment_offset = 0;
  uint8_t next_fragment_bit = 0;
};

struct LegacyQuickbarParseStats {
  size_t read_size = 0;
  size_t fragment_size = 0;
  size_t final_cursor = 0;
  int score = 0;
  int spells = 0;
  int no_payload_buttons = 0;
  int general_buttons_preserved = 0;
  int item_buttons_translated = 0;
  int item_buttons_blanked = 0;
  int unsupported_buttons_blanked = 0;
};

uint32_t ReadU32LeRaw(const uint8_t* bytes) {
  return static_cast<uint32_t>(bytes[0]) |
      (static_cast<uint32_t>(bytes[1]) << 8) |
      (static_cast<uint32_t>(bytes[2]) << 16) |
      (static_cast<uint32_t>(bytes[3]) << 24);
}

uint16_t ReadU16LeRaw(const uint8_t* bytes) {
  return static_cast<uint16_t>(
      static_cast<uint16_t>(bytes[0]) |
      (static_cast<uint16_t>(bytes[1]) << 8));
}

bool ReadLegacyQuickbarBit(LegacyQuickbarPacketReader* reader, uint32_t* bit) {
  if (reader == nullptr || bit == nullptr || reader->fragments == nullptr ||
      reader->fragment_cursor >= reader->fragment_size || reader->fragment_bit >= 8) {
    return false;
  }

  *bit = (reader->fragments[reader->fragment_cursor] >> (7 - reader->fragment_bit)) & 1u;
  ++reader->fragment_bit;
  if (reader->fragment_bit >= 8) {
    reader->fragment_bit = 0;
    ++reader->fragment_cursor;
  }
  return true;
}

bool ReadLegacyQuickbarBits(
    LegacyQuickbarPacketReader* reader,
    uint8_t bit_count,
    uint32_t* value) {
  if (value != nullptr) {
    *value = 0;
  }
  if (reader == nullptr || value == nullptr || bit_count > 32) {
    return false;
  }

  uint32_t result = 0;
  for (uint8_t bit_index = 0; bit_index < bit_count; ++bit_index) {
    uint32_t bit = 0;
    if (!ReadLegacyQuickbarBit(reader, &bit)) {
      return false;
    }
    result = (result << 1) | (bit & 1u);
  }
  *value = result;
  return true;
}

bool ReadLegacyQuickbarBool(LegacyQuickbarPacketReader* reader, bool* value) {
  uint32_t bit = 0;
  if (!ReadLegacyQuickbarBit(reader, &bit)) {
    return false;
  }
  if (value != nullptr) {
    *value = bit != 0;
  }
  return true;
}

bool ReadLegacyQuickbarByte(LegacyQuickbarPacketReader* reader, uint8_t* value) {
  if (reader == nullptr || value == nullptr || reader->read_buffer == nullptr ||
      reader->cursor >= reader->read_size) {
    return false;
  }
  *value = reader->read_buffer[reader->cursor++];
  return true;
}

bool ReadLegacyQuickbarFixedCResRef(
    LegacyQuickbarPacketReader* reader,
    std::string* preview) {
  if (preview != nullptr) {
    preview->clear();
  }
  if (reader == nullptr || reader->read_buffer == nullptr ||
      reader->cursor > reader->read_size ||
      reader->read_size - reader->cursor < kCResRefTextBytes) {
    return false;
  }

  if (preview != nullptr) {
    size_t length = 0;
    while (length < kCResRefTextBytes && reader->read_buffer[reader->cursor + length] != 0) {
      ++length;
    }
    preview->assign(
        reinterpret_cast<const char*>(reader->read_buffer + reader->cursor),
        reinterpret_cast<const char*>(reader->read_buffer + reader->cursor + length));
  }
  reader->cursor += kCResRefTextBytes;
  return true;
}

bool ReadLegacyQuickbarWord(LegacyQuickbarPacketReader* reader, uint16_t* value) {
  if (reader == nullptr || value == nullptr || reader->read_buffer == nullptr ||
      reader->cursor > reader->read_size || reader->read_size - reader->cursor < 2) {
    return false;
  }
  *value = ReadU16LeRaw(reader->read_buffer + reader->cursor);
  reader->cursor += 2;
  return true;
}

bool ReadLegacyQuickbarDword(LegacyQuickbarPacketReader* reader, uint32_t* value) {
  if (reader == nullptr || value == nullptr || reader->read_buffer == nullptr ||
      reader->cursor > reader->read_size || reader->read_size - reader->cursor < 4) {
    return false;
  }
  *value = ReadU32LeRaw(reader->read_buffer + reader->cursor);
  reader->cursor += 4;
  return true;
}

bool SkipLegacyQuickbarBytes(LegacyQuickbarPacketReader* reader, size_t byte_count) {
  if (reader == nullptr || reader->cursor > reader->read_size ||
      byte_count > reader->read_size - reader->cursor) {
    return false;
  }
  reader->cursor += byte_count;
  return true;
}

std::wstring FormatLegacyQuickbarReaderWindow(const LegacyQuickbarPacketReader* reader) {
  if (reader == nullptr || reader->read_buffer == nullptr) {
    return L"<no-reader>";
  }

  const size_t start = reader->cursor > 16 ? reader->cursor - 16 : 0;
  const size_t end = std::min(reader->read_size, reader->cursor + 64);
  const size_t length = end >= start ? end - start : 0;
  wchar_t prefix[192]{};
  swprintf_s(
      prefix,
      L"cursor=%zu/%zu frag=%zu/%zu:%u finalbits=%u window@%zu=[",
      reader->cursor,
      reader->read_size,
      reader->fragment_cursor,
      reader->fragment_size,
      static_cast<unsigned int>(reader->fragment_bit),
      static_cast<unsigned int>(reader->final_fragment_bits),
      start);
  std::wstring result = prefix;
  result += FormatLimitedBytes(reader->read_buffer + start, length, 96);
  result += L"]";
  return result;
}

void SetLegacyQuickbarFailure(
    std::wstring* failure_reason,
    const LegacyQuickbarPacketReader* reader,
    const std::wstring& detail) {
  if (failure_reason == nullptr) {
    return;
  }
  *failure_reason = detail;
  *failure_reason += L" ";
  *failure_reason += FormatLegacyQuickbarReaderWindow(reader);
}

bool IsLegacyQuickbarPlausibleType(uint8_t type) {
  return type <= 48;
}

bool LegacyQuickbarTypeHasNoPayload(uint8_t type) {
  switch (type) {
    case 0:
    case 5:
    case 6:
    case 7:
    case 9:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 35:
    case 36:
    case 38:
    case 40:
    case 41:
      return true;
    default:
      return false;
  }
}

bool LegacyQuickbarTypeHasIntPayload(uint8_t type) {
  switch (type) {
    case 3:
    case 4:
    case 8:
    case 10:
    case 27:
    case 28:
    case 31:
    case 32:
    case 33:
    case 34:
    case 37:
    case 42:
    case 43:
    case 45:
    case 46:
    case 47:
    case 48:
      return true;
    default:
      return false;
  }
}

bool SkipLegacyQuickbarString(const uint8_t* read_buffer, size_t read_size, size_t* cursor) {
  if (read_buffer == nullptr || cursor == nullptr || *cursor > read_size || read_size - *cursor < 4) {
    return false;
  }

  const uint32_t length = ReadU32LeRaw(read_buffer + *cursor);
  constexpr uint32_t kMaxReasonableQuickbarStringBytes = 1024;
  if (length > kMaxReasonableQuickbarStringBytes || length > read_size - *cursor - 4) {
    return false;
  }

  *cursor += 4 + length;
  return true;
}

bool ReadLegacyQuickbarString(
    LegacyQuickbarPacketReader* reader,
    std::string* preview,
    uint32_t max_length = 4096) {
  if (preview != nullptr) {
    preview->clear();
  }
  uint32_t length = 0;
  if (!ReadLegacyQuickbarDword(reader, &length) ||
      reader == nullptr ||
      reader->read_buffer == nullptr ||
      reader->cursor > reader->read_size ||
      length > max_length ||
      length > reader->read_size - reader->cursor) {
    return false;
  }

  if (preview != nullptr) {
    const uint32_t preview_length = std::min<uint32_t>(length, 80);
    preview->assign(
        reinterpret_cast<const char*>(reader->read_buffer + reader->cursor),
        reinterpret_cast<const char*>(reader->read_buffer + reader->cursor + preview_length));
  }
  reader->cursor += length;
  return true;
}

bool ReadLegacyQuickbarLocStringField(
    LegacyQuickbarPacketReader* reader,
    LegacyQuickbarLocStringField* field,
    std::string* preview) {
  if (field != nullptr) {
    *field = LegacyQuickbarLocStringField{};
  }
  bool custom_tlk = false;
  if (!ReadLegacyQuickbarBool(reader, &custom_tlk)) {
    return false;
  }
  if (field != nullptr) {
    field->custom_tlk = custom_tlk;
  }

  if (custom_tlk) {
    uint32_t language_selector = 0;
    uint32_t string_ref = 0;
    if (!ReadLegacyQuickbarBits(reader, 1, &language_selector) ||
        !ReadLegacyQuickbarDword(reader, &string_ref)) {
      return false;
    }
    if (field != nullptr) {
      field->language_selector = language_selector != 0;
      field->string_ref = string_ref;
    }
    if (preview != nullptr) {
      char text[64]{};
      sprintf_s(text, "strref:%u%s", string_ref, language_selector != 0 ? ":alt" : "");
      *preview = text;
    }
    return true;
  }

  std::string text;
  if (!ReadLegacyQuickbarString(reader, &text)) {
    return false;
  }
  if (field != nullptr) {
    field->text = text;
  }
  if (preview != nullptr) {
    *preview = text;
  }
  return true;
}

bool SkipLegacyQuickbarLocString(LegacyQuickbarPacketReader* reader, std::string* preview) {
  return ReadLegacyQuickbarLocStringField(reader, nullptr, preview);
}

bool LegacyQuickbarBaseItemRequiresActivePropertyWord(uint32_t base_item_id) {
  return base_item_id == 0x10;
}

bool ParseLegacyQuickbarItemAppearance(
    LegacyQuickbarPacketReader* reader,
    LegacyQuickbarItemObject* item,
    std::wstring* failure_reason) {
  if (reader == nullptr || item == nullptr || reader->read_buffer == nullptr) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"item appearance missing reader/item");
    return false;
  }

  const size_t start = reader->cursor;
  uint32_t base_item_id = 0;
  if (!ReadLegacyQuickbarDword(reader, &base_item_id)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"item appearance missing base item id");
    return false;
  }
  item->base_item = base_item_id;

  void* const base_item = ResolveBaseItemForLegacyAppearance(base_item_id);
  uint8_t appearance_type = 0xFF;
  if (base_item == nullptr ||
      !SafeReadUInt8(static_cast<uint8_t*>(base_item) + 0x16, &appearance_type)) {
    wchar_t detail[192]{};
    swprintf_s(
        detail,
        L"item appearance base lookup failed base=%u/0x%08X base_ptr=%p",
        base_item_id,
        base_item_id,
        base_item);
    SetLegacyQuickbarFailure(failure_reason, reader, detail);
    return false;
  }
  item->appearance_type = appearance_type;

  size_t extra_bytes = 0;
  switch (appearance_type) {
    case 0:
      extra_bytes = 1;
      break;
    case 1:
      extra_bytes = 1 + 6;
      break;
    case 2:
      extra_bytes = 3 + 1;
      break;
    case 3:
      extra_bytes = 19 + 6;
      break;
    default:
      {
        wchar_t detail[192]{};
        swprintf_s(
            detail,
            L"item appearance unsupported appearance type=%u base=%u/0x%08X",
            static_cast<unsigned int>(appearance_type),
            base_item_id,
            base_item_id);
        SetLegacyQuickbarFailure(failure_reason, reader, detail);
      }
      return false;
  }

  if (!SkipLegacyQuickbarBytes(reader, extra_bytes)) {
    wchar_t detail[192]{};
    swprintf_s(
        detail,
        L"item appearance truncated base=%u/0x%08X appearance_type=%u extra=%zu",
        base_item_id,
        base_item_id,
        static_cast<unsigned int>(appearance_type),
        extra_bytes);
    SetLegacyQuickbarFailure(failure_reason, reader, detail);
    return false;
  }

  const size_t end = reader->cursor;
  item->appearance_bytes.assign(reader->read_buffer + start, reader->read_buffer + end);
  return true;
}

bool ParseLegacyQuickbarActiveItemProperties(
    LegacyQuickbarPacketReader* reader,
    uint32_t base_item_id,
    LegacyQuickbarActiveItemProperties* properties,
    std::string* name_preview,
    std::wstring* failure_reason) {
  if (name_preview != nullptr) {
    name_preview->clear();
  }
  if (properties != nullptr) {
    *properties = LegacyQuickbarActiveItemProperties{};
  }

  if (LegacyQuickbarBaseItemRequiresActivePropertyWord(base_item_id)) {
    uint16_t armor_word = 0;
    if (!ReadLegacyQuickbarWord(reader, &armor_word)) {
      SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing armor word");
      return false;
    }
    if (properties != nullptr) {
      properties->has_armor_word = true;
      properties->armor_word = armor_word;
    }
  }

  bool name_is_locstring = false;
  if (!ReadLegacyQuickbarBool(reader, &name_is_locstring)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing name kind bit");
    return false;
  }
  if (properties != nullptr) {
    properties->name_is_locstring = name_is_locstring;
  }
  if (name_is_locstring) {
    if (!ReadLegacyQuickbarLocStringField(
            reader,
            properties != nullptr ? &properties->locstring_name : nullptr,
            name_preview)) {
      SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties failed locstring name");
      return false;
    }
  } else {
    std::string name;
    if (!ReadLegacyQuickbarString(reader, &name)) {
      SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties failed string name");
      return false;
    }
    if (properties != nullptr) {
      properties->string_name = name;
    }
    if (name_preview != nullptr) {
      *name_preview = name;
    }
  }

  bool unused_bool = false;
  uint32_t unused_dword = 0;
  uint8_t property_count = 0;
  if (!ReadLegacyQuickbarBool(reader, &unused_bool)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing post-name bool 1");
    return false;
  }
  if (properties != nullptr) {
    properties->post_name_bool1 = unused_bool;
  }
  if (!ReadLegacyQuickbarDword(reader, &unused_dword)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing cost dword");
    return false;
  }
  if (properties != nullptr) {
    properties->cost = unused_dword;
  }
  if (!ReadLegacyQuickbarDword(reader, &unused_dword)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing stack/charges dword");
    return false;
  }
  if (properties != nullptr) {
    properties->stack_or_charges = unused_dword;
  }
  if (!ReadLegacyQuickbarBool(reader, &unused_bool)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing post-name bool 2");
    return false;
  }
  if (properties != nullptr) {
    properties->post_name_bool2 = unused_bool;
  }
  if (!ReadLegacyQuickbarBool(reader, &unused_bool)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing post-name bool 3");
    return false;
  }
  if (properties != nullptr) {
    properties->post_name_bool3 = unused_bool;
  }
  if (!ReadLegacyQuickbarBool(reader, &unused_bool)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing post-name bool 4");
    return false;
  }
  if (properties != nullptr) {
    properties->post_name_bool4 = unused_bool;
  }
  if (!ReadLegacyQuickbarByte(reader, &property_count)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing property count");
    return false;
  }

  constexpr uint8_t kMaxReasonableQuickbarItemProperties = 128;
  if (property_count > kMaxReasonableQuickbarItemProperties) {
    wchar_t detail[192]{};
    swprintf_s(
        detail,
        L"active item properties unreasonable property count=%u base=%u/0x%08X name='%S'",
        static_cast<unsigned int>(property_count),
        base_item_id,
        base_item_id,
        name_preview != nullptr ? name_preview->c_str() : "");
    SetLegacyQuickbarFailure(failure_reason, reader, detail);
    return false;
  }
  if (properties != nullptr) {
    properties->properties.reserve(property_count);
  }

  for (uint8_t index = 0; index < property_count; ++index) {
    LegacyQuickbarActivePropertyEntry entry{};
    if (!ReadLegacyQuickbarWord(reader, &entry.property) ||
        !ReadLegacyQuickbarWord(reader, &entry.subtype) ||
        !ReadLegacyQuickbarWord(reader, &entry.cost_table_value) ||
        !ReadLegacyQuickbarByte(reader, &entry.param)) {
      wchar_t detail[160]{};
      swprintf_s(
          detail,
          L"active item properties truncated property index=%u/%u",
          static_cast<unsigned int>(index),
          static_cast<unsigned int>(property_count));
      SetLegacyQuickbarFailure(failure_reason, reader, detail);
      return false;
    }
    if (properties != nullptr) {
      properties->properties.push_back(entry);
    }
  }

  uint8_t state_mask = 0;
  uint8_t value_mask = 0;
  if (!ReadLegacyQuickbarByte(reader, &state_mask) ||
      !ReadLegacyQuickbarByte(reader, &value_mask)) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"active item properties missing state/value masks");
    return false;
  }
  if (properties != nullptr) {
    properties->state_mask = state_mask;
    properties->value_mask = value_mask;
  }

  for (uint8_t bit = 0; bit < 8; ++bit) {
    if ((value_mask & (1u << bit)) != 0) {
      uint8_t value = 0;
      if (!ReadLegacyQuickbarByte(reader, &value)) {
        wchar_t detail[160]{};
        swprintf_s(
            detail,
            L"active item properties missing value mask byte bit=%u value_mask=0x%02X",
            static_cast<unsigned int>(bit),
            static_cast<unsigned int>(value_mask));
        SetLegacyQuickbarFailure(failure_reason, reader, detail);
        return false;
      }
      if (properties != nullptr) {
        properties->value_mask_bytes.push_back(value);
      }
    }
  }

  return true;
}

bool SkipLegacyQuickbarActiveItemProperties(
    LegacyQuickbarPacketReader* reader,
    uint32_t base_item_id,
    std::string* name_preview,
    std::wstring* failure_reason) {
  return ParseLegacyQuickbarActiveItemProperties(
      reader,
      base_item_id,
      nullptr,
      name_preview,
      failure_reason);
}

bool ParseLegacyQuickbarItemObject(
    LegacyQuickbarPacketReader* reader,
    bool has_int_param,
    LegacyQuickbarItemObject* item,
    const wchar_t* label,
    std::wstring* failure_reason) {
  if (item == nullptr) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"item object missing output");
    return false;
  }
  *item = LegacyQuickbarItemObject{};

  bool present = false;
  if (!ReadLegacyQuickbarBool(reader, &present)) {
    std::wstring detail = L"item object missing present bit";
    if (label != nullptr) {
      detail += L" label=";
      detail += label;
    }
    SetLegacyQuickbarFailure(failure_reason, reader, detail);
    return false;
  }
  item->present = present;
  if (!present) {
    return true;
  }

  uint32_t object_id = 0;
  uint32_t raw_int_param = 0;
  if (!ReadLegacyQuickbarDword(reader, &object_id)) {
    std::wstring detail = L"item object missing object id";
    if (label != nullptr) {
      detail += L" label=";
      detail += label;
    }
    SetLegacyQuickbarFailure(failure_reason, reader, detail);
    return false;
  }
  item->object_id = object_id;
  if (has_int_param) {
    if (!ReadLegacyQuickbarDword(reader, &raw_int_param)) {
      std::wstring detail = L"item object missing int param";
      if (label != nullptr) {
        detail += L" label=";
        detail += label;
      }
      SetLegacyQuickbarFailure(failure_reason, reader, detail);
      return false;
    }
    item->int_param = static_cast<int32_t>(raw_int_param);
  }

  if (!ParseLegacyQuickbarItemAppearance(reader, item, failure_reason) ||
      !ParseLegacyQuickbarActiveItemProperties(
          reader,
          item->base_item,
          &item->active_props,
          &item->name_preview,
          failure_reason)) {
    if (failure_reason != nullptr && !failure_reason->empty() && label != nullptr) {
      *failure_reason = std::wstring(label) + L" " + *failure_reason;
    }
    return false;
  }

  item->active_props_skipped = true;
  return true;
}

bool ParseLegacyQuickbarItemPayload(
    LegacyQuickbarPacketReader* reader,
    LegacyQuickbarButton* button,
    std::wstring* failure_reason) {
  if (reader == nullptr || button == nullptr) {
    SetLegacyQuickbarFailure(failure_reason, reader, L"item payload missing reader/button");
    return false;
  }

  if (!ParseLegacyQuickbarItemObject(reader, true, &button->primary_item, L"primary", failure_reason) ||
      !ParseLegacyQuickbarItemObject(reader, false, &button->secondary_item, L"secondary", failure_reason)) {
    return false;
  }

  button->parsed = true;
  button->item_payload = true;
  button->next_offset = reader->cursor;
  button->next_fragment_offset = reader->fragment_cursor;
  button->next_fragment_bit = reader->fragment_bit;
  return true;
}

bool ParseLegacyQuickbarNonItemFromReader(
    LegacyQuickbarPacketReader* reader,
    uint8_t source_type,
    LegacyQuickbarButton* button) {
  if (reader == nullptr || button == nullptr) {
    return false;
  }

  LegacyQuickbarButton parsed = *button;
  parsed.source_type = source_type;

  if (!IsLegacyQuickbarPlausibleType(source_type) || source_type == 1) {
    return false;
  }

  if (LegacyQuickbarTypeHasNoPayload(source_type)) {
    parsed.parsed = true;
    parsed.no_payload = true;
    parsed.next_offset = reader->cursor;
    parsed.next_fragment_offset = reader->fragment_cursor;
    parsed.next_fragment_bit = reader->fragment_bit;
    *button = parsed;
    return true;
  }

  if (source_type == 2) {
    uint8_t spell_class = 0;
    uint32_t spell_id = 0;
    uint8_t metamagic = 0;
    uint8_t domain = 0;
    if (!ReadLegacyQuickbarByte(reader, &spell_class) ||
        !ReadLegacyQuickbarDword(reader, &spell_id) ||
        !ReadLegacyQuickbarByte(reader, &metamagic) ||
        !ReadLegacyQuickbarByte(reader, &domain)) {
      return false;
    }
    parsed.parsed = true;
    parsed.spell_class = spell_class;
    parsed.spell_id = spell_id;
    parsed.metamagic = metamagic;
    parsed.domain = domain;
    constexpr uint32_t kMaxReasonableLegacyQuickbarSpellId = 10000;
    parsed.spell = spell_id <= kMaxReasonableLegacyQuickbarSpellId;
    parsed.next_offset = reader->cursor;
    parsed.next_fragment_offset = reader->fragment_cursor;
    parsed.next_fragment_bit = reader->fragment_bit;
    *button = parsed;
    return true;
  }

  if (LegacyQuickbarTypeHasIntPayload(source_type)) {
    uint32_t value = 0;
    if (!ReadLegacyQuickbarDword(reader, &value)) {
      return false;
    }
    parsed.parsed = true;
    parsed.int_payload = true;
    parsed.int_payload_value = value;
    parsed.next_offset = reader->cursor;
    parsed.next_fragment_offset = reader->fragment_cursor;
    parsed.next_fragment_bit = reader->fragment_bit;
    *button = parsed;
    return true;
  }

  if (source_type == 44) {
    uint32_t value = 0;
    uint8_t byte_value = 0;
    if (!ReadLegacyQuickbarDword(reader, &value) ||
        !ReadLegacyQuickbarByte(reader, &byte_value)) {
      return false;
    }
    parsed.parsed = true;
    parsed.int_payload = true;
    parsed.int_payload_value = value;
    parsed.byte_payload = true;
    parsed.byte_payload_value = byte_value;
    parsed.next_offset = reader->cursor;
    parsed.next_fragment_offset = reader->fragment_cursor;
    parsed.next_fragment_bit = reader->fragment_bit;
    *button = parsed;
    return true;
  }

  if (source_type >= 11 && source_type <= 17) {
    std::string resref;
    std::string text;
    if (!ReadLegacyQuickbarFixedCResRef(reader, &resref) ||
        !ReadLegacyQuickbarString(reader, &text)) {
      return false;
    }
    parsed.parsed = true;
    parsed.resref_payload_present = true;
    parsed.resref_payload = resref;
    parsed.string_payload_present = true;
    parsed.string_payload = text;
    parsed.next_offset = reader->cursor;
    parsed.next_fragment_offset = reader->fragment_cursor;
    parsed.next_fragment_bit = reader->fragment_bit;
    *button = parsed;
    return true;
  }

  if (source_type == 18) {
    std::string text1;
    std::string text2;
    if (!ReadLegacyQuickbarString(reader, &text1) ||
        !ReadLegacyQuickbarString(reader, &text2)) {
      return false;
    }
    parsed.parsed = true;
    parsed.string_payload_present = true;
    parsed.string_payload = text1;
    parsed.string_payload2_present = true;
    parsed.string_payload2 = text2;
    parsed.next_offset = reader->cursor;
    parsed.next_fragment_offset = reader->fragment_cursor;
    parsed.next_fragment_bit = reader->fragment_bit;
    *button = parsed;
    return true;
  }

  if (source_type == 29 || source_type == 30) {
    std::string resref;
    if (!ReadLegacyQuickbarFixedCResRef(reader, &resref)) {
      return false;
    }
    parsed.parsed = true;
    parsed.resref_payload_present = true;
    parsed.resref_payload = resref;
    parsed.next_offset = reader->cursor;
    parsed.next_fragment_offset = reader->fragment_cursor;
    parsed.next_fragment_bit = reader->fragment_bit;
    *button = parsed;
    return true;
  }

  return false;
}

bool AdvanceLegacyQuickbarNonItem(
    const uint8_t* read_buffer,
    size_t read_size,
    size_t cursor,
    LegacyQuickbarButton* button,
    size_t* next_cursor) {
  if (read_buffer == nullptr || button == nullptr || next_cursor == nullptr || cursor >= read_size) {
    return false;
  }

  LegacyQuickbarButton parsed{};
  parsed.source_offset = cursor;
  parsed.source_type = read_buffer[cursor++];

  if (!IsLegacyQuickbarPlausibleType(parsed.source_type) ||
      parsed.source_type == 1) {
    return false;
  }

  if (LegacyQuickbarTypeHasNoPayload(parsed.source_type)) {
    parsed.parsed = true;
    parsed.no_payload = true;
    parsed.next_offset = cursor;
    *button = parsed;
    *next_cursor = cursor;
    return true;
  }

  if (parsed.source_type == 2) {
    if (read_size - cursor < 7) {
      return false;
    }
    parsed.parsed = true;
    parsed.spell_class = read_buffer[cursor++];
    parsed.spell_id = ReadU32LeRaw(read_buffer + cursor);
    cursor += 4;
    parsed.metamagic = read_buffer[cursor++];
    parsed.domain = read_buffer[cursor++];
    constexpr uint32_t kMaxReasonableLegacyQuickbarSpellId = 10000;
    parsed.spell = parsed.spell_id <= kMaxReasonableLegacyQuickbarSpellId;
    parsed.next_offset = cursor;
    *button = parsed;
    *next_cursor = cursor;
    return true;
  }

  if (LegacyQuickbarTypeHasIntPayload(parsed.source_type)) {
    if (read_size - cursor < 4) {
      return false;
    }
    parsed.parsed = true;
    parsed.int_payload = true;
    parsed.int_payload_value = ReadU32LeRaw(read_buffer + cursor);
    parsed.next_offset = cursor + 4;
    *button = parsed;
    *next_cursor = parsed.next_offset;
    return true;
  }

  if (parsed.source_type == 44) {
    if (read_size - cursor < 5) {
      return false;
    }
    parsed.parsed = true;
    parsed.int_payload = true;
    parsed.int_payload_value = ReadU32LeRaw(read_buffer + cursor);
    parsed.byte_payload = true;
    parsed.byte_payload_value = read_buffer[cursor + 4];
    parsed.next_offset = cursor + 5;
    *button = parsed;
    *next_cursor = parsed.next_offset;
    return true;
  }

  if (parsed.source_type >= 11 && parsed.source_type <= 17) {
    if (read_size - cursor < kCResRefTextBytes) {
      return false;
    }
    cursor += kCResRefTextBytes;
    if (!SkipLegacyQuickbarString(read_buffer, read_size, &cursor)) {
      return false;
    }
    parsed.parsed = true;
    parsed.next_offset = cursor;
    *button = parsed;
    *next_cursor = cursor;
    return true;
  }

  if (parsed.source_type == 18) {
    if (!SkipLegacyQuickbarString(read_buffer, read_size, &cursor) ||
        !SkipLegacyQuickbarString(read_buffer, read_size, &cursor)) {
      return false;
    }
    parsed.parsed = true;
    parsed.next_offset = cursor;
    *button = parsed;
    *next_cursor = cursor;
    return true;
  }

  if (parsed.source_type == 29 || parsed.source_type == 30) {
    if (read_size - cursor < kCResRefTextBytes) {
      return false;
    }
    parsed.parsed = true;
    parsed.next_offset = cursor + kCResRefTextBytes;
    *button = parsed;
    *next_cursor = parsed.next_offset;
    return true;
  }

  return false;
}

bool LegacyQuickbarButtonHasPreservableGeneralPayload(const LegacyQuickbarButton& button) {
  return button.parsed &&
      !button.spell &&
      !button.item_payload &&
      (button.no_payload ||
       button.int_payload ||
       button.byte_payload ||
       button.resref_payload_present ||
       button.string_payload_present ||
       button.string_payload2_present);
}

int ScoreLegacyQuickbarParseFrom(
    const uint8_t* read_buffer,
    size_t read_size,
    size_t slot,
    size_t cursor,
    std::vector<int>* memo) {
  constexpr int kBadScore = -1000000000;
  constexpr int kUnknownScore = INT_MIN;

  if (read_buffer == nullptr || memo == nullptr || cursor > read_size) {
    return kBadScore;
  }

  if (slot == kLegacyQuickbarButtonCount) {
    const size_t unread = cursor <= read_size ? read_size - cursor : cursor - read_size;
    return 100000 - static_cast<int>(std::min<size_t>(unread, 10000)) * 25;
  }

  if (cursor >= read_size) {
    return kBadScore;
  }

  const size_t memo_width = read_size + 1;
  const size_t memo_index = slot * memo_width + cursor;
  if (memo_index < memo->size() && (*memo)[memo_index] != kUnknownScore) {
    return (*memo)[memo_index];
  }

  int best_score = kBadScore;
  const uint8_t type = read_buffer[cursor];
  const size_t remaining_slots_after_this = kLegacyQuickbarButtonCount - slot - 1;

  if (type == 1 || type == 9) {
    const size_t item_payload_start = cursor + 1;
    if (item_payload_start <= read_size) {
      const size_t min_candidate = std::min(read_size, item_payload_start + 8);
      const size_t max_candidate = std::min(read_size, item_payload_start + 420);
      for (size_t candidate = min_candidate; candidate <= max_candidate; ++candidate) {
        if (candidate + remaining_slots_after_this > read_size) {
          break;
        }
        if (remaining_slots_after_this > 0 &&
            (candidate >= read_size || !IsLegacyQuickbarPlausibleType(read_buffer[candidate]))) {
          continue;
        }

        int score = ScoreLegacyQuickbarParseFrom(read_buffer, read_size, slot + 1, candidate, memo);
        if (score <= kBadScore / 2) {
          continue;
        }
        const size_t skipped = candidate > item_payload_start ? candidate - item_payload_start : 0;
        score += 12 - static_cast<int>(std::min<size_t>(skipped / 16, 120));
        if (score > best_score) {
          best_score = score;
        }
      }
    }
  } else {
    LegacyQuickbarButton parsed{};
    size_t next_cursor = cursor;
    if (AdvanceLegacyQuickbarNonItem(read_buffer, read_size, cursor, &parsed, &next_cursor)) {
      int score = ScoreLegacyQuickbarParseFrom(read_buffer, read_size, slot + 1, next_cursor, memo);
      if (score > kBadScore / 2) {
        if (parsed.spell) {
          score += 60;
        } else if (parsed.no_payload) {
          score += parsed.source_type == 0 ? 8 : 20;
        } else {
          score += 4;
        }
        best_score = score;
      }
    }
  }

  if (memo_index < memo->size()) {
    (*memo)[memo_index] = best_score;
  }
  return best_score;
}

int ScoreLegacyQuickbarCandidateWindow(
    const uint8_t* read_buffer,
    size_t read_size,
    size_t cursor,
    size_t remaining_slots) {
  if (read_buffer == nullptr || cursor >= read_size || remaining_slots == 0) {
    return -1000000;
  }

  int score = 0;
  const size_t slots_to_probe = std::min<size_t>(remaining_slots, 8);
  for (size_t probe = 0; probe < slots_to_probe; ++probe) {
    if (cursor >= read_size) {
      return probe == 0 ? -1000000 : score - 20;
    }

    const uint8_t type = read_buffer[cursor];
    if (!IsLegacyQuickbarPlausibleType(type)) {
      return -1000000;
    }
    if (type == 1 || type == 9) {
      return score + 12;
    }

    LegacyQuickbarButton parsed{};
    size_t next_cursor = cursor;
    if (!AdvanceLegacyQuickbarNonItem(read_buffer, read_size, cursor, &parsed, &next_cursor) ||
        next_cursor <= cursor) {
      return -1000000;
    }

    if (parsed.spell) {
      score += 80;
    } else if (parsed.no_payload) {
      score += parsed.source_type == 0 ? 8 : 24;
    } else {
      score += 4;
    }
    cursor = next_cursor;
  }

  return score;
}

bool ChooseLegacyQuickbarItemEnd(
    const uint8_t* read_buffer,
    size_t read_size,
    size_t slot,
    size_t cursor,
    std::vector<int>* memo,
    size_t* next_cursor) {
  UNREFERENCED_PARAMETER(memo);
  if (read_buffer == nullptr || memo == nullptr || next_cursor == nullptr || cursor >= read_size) {
    return false;
  }

  const size_t remaining_slots_after_this = kLegacyQuickbarButtonCount - slot - 1;
  const size_t item_payload_start = cursor + 1;
  const size_t min_candidate = std::min(read_size, item_payload_start + 8);
  const size_t max_candidate = std::min(read_size, item_payload_start + 512);
  int best_score = -1000000;
  size_t best_candidate = 0;

  for (size_t candidate = min_candidate; candidate <= max_candidate; ++candidate) {
    if (candidate + remaining_slots_after_this > read_size) {
      break;
    }
    if (remaining_slots_after_this > 0 &&
            (candidate >= read_size || !IsLegacyQuickbarPlausibleType(read_buffer[candidate]))) {
      continue;
    }

    int score = remaining_slots_after_this == 0
        ? 100
        : ScoreLegacyQuickbarCandidateWindow(read_buffer, read_size, candidate, remaining_slots_after_this);
    if (score < 0) {
      continue;
    }
    if (remaining_slots_after_this > 0) {
      const uint8_t next_type = read_buffer[candidate];
      if (next_type == 2) {
        score += 35;
      } else if (next_type == 0) {
        score += 2;
      } else if (LegacyQuickbarTypeHasNoPayload(next_type)) {
        score += 8;
      }
    }
    const size_t skipped = candidate > item_payload_start ? candidate - item_payload_start : 0;
    score -= static_cast<int>(std::min<size_t>(skipped / 32, 80));
    if (score > best_score) {
      best_score = score;
      best_candidate = candidate;
    }
  }

  if (best_score < 0) {
    return false;
  }

  *next_cursor = best_candidate;
  return true;
}

std::wstring FormatLegacyQuickbarSummary(
    const std::array<LegacyQuickbarButton, kLegacyQuickbarButtonCount>& buttons) {
  std::wstring summary;
  for (size_t slot = 0; slot < buttons.size(); ++slot) {
    const LegacyQuickbarButton& button = buttons[slot];
    if (!button.spell && !button.item_payload && !button.no_payload && button.source_type == 0) {
      continue;
    }
    wchar_t entry[384]{};
    if (button.spell) {
      swprintf_s(
          entry,
          L"%zu:spell(c=%u,id=%u,meta=%u,dom=%u)",
          slot,
          static_cast<unsigned int>(button.spell_class),
          static_cast<unsigned int>(button.spell_id),
          static_cast<unsigned int>(button.metamagic),
          static_cast<unsigned int>(button.domain));
    } else if (button.item_payload) {
      const std::wstring primary_name = NarrowToWide(button.primary_item.name_preview);
      const std::wstring secondary_name = NarrowToWide(button.secondary_item.name_preview);
      swprintf_s(
          entry,
          L"%zu:item(type=%u,off=%zu..%zu,frag=%zu:%u..%zu:%u,p=%d/0x%08X base=%u,s=%d/0x%08X base=%u,name='%s'%s%s%s)",
          slot,
          static_cast<unsigned int>(button.source_type),
          button.source_offset,
          button.next_offset,
          button.source_fragment_offset,
          static_cast<unsigned int>(button.source_fragment_bit),
          button.next_fragment_offset,
          static_cast<unsigned int>(button.next_fragment_bit),
          button.primary_item.present ? 1 : 0,
          button.primary_item.object_id,
          button.primary_item.base_item,
          button.secondary_item.present ? 1 : 0,
          button.secondary_item.object_id,
          button.secondary_item.base_item,
          primary_name.c_str(),
          secondary_name.empty() ? L"" : L",sname='",
          secondary_name.c_str(),
          secondary_name.empty() ? L"" : L"'");
    } else {
      if (button.int_payload || button.byte_payload) {
        if (button.byte_payload) {
          swprintf_s(
              entry,
              L"%zu:type%u(int=%u/0x%08X,byte=%u)",
              slot,
              static_cast<unsigned int>(button.source_type),
              button.int_payload_value,
              button.int_payload_value,
              static_cast<unsigned int>(button.byte_payload_value));
        } else {
          swprintf_s(
              entry,
              L"%zu:type%u(int=%u/0x%08X)",
              slot,
              static_cast<unsigned int>(button.source_type),
              button.int_payload_value,
              button.int_payload_value);
        }
      } else if (button.resref_payload_present || button.string_payload_present ||
                 button.string_payload2_present) {
        const std::wstring resref = NarrowToWide(button.resref_payload);
        const std::wstring text1 = NarrowToWide(button.string_payload);
        const std::wstring text2 = NarrowToWide(button.string_payload2);
        swprintf_s(
            entry,
            L"%zu:type%u(resref='%s',text='%s'%s%s%s)",
            slot,
            static_cast<unsigned int>(button.source_type),
            resref.c_str(),
            text1.c_str(),
            text2.empty() ? L"" : L",text2='",
            text2.c_str(),
            text2.empty() ? L"" : L"'");
      } else {
        swprintf_s(
            entry,
            L"%zu:type%u",
            slot,
            static_cast<unsigned int>(button.source_type));
      }
    }
    if (!summary.empty()) {
      summary += L"; ";
    }
    summary += entry;
    if (summary.size() > 5000) {
      summary += L"; ...";
      break;
    }
  }
  if (summary.empty()) {
    summary = L"<all-empty>";
  }
  return summary;
}

bool ParseLegacyQuickbarSetAllButtons(
    const unsigned char* packet,
    uint32_t packet_size,
    std::array<LegacyQuickbarButton, kLegacyQuickbarButtonCount>* buttons,
    LegacyQuickbarParseStats* stats,
    std::wstring* failure_reason) {
  if (failure_reason != nullptr) {
    failure_reason->clear();
  }
  if (buttons == nullptr || stats == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing output pointers";
    }
    return false;
  }
  buttons->fill(LegacyQuickbarButton{});
  *stats = LegacyQuickbarParseStats{};

  if (packet == nullptr || packet_size < 7 || packet[0] != 'P' || packet[1] != 30 || packet[2] != 1) {
    if (failure_reason != nullptr) {
      *failure_reason = L"not a GuiQuickbar_SetAllButtons packet";
    }
    return false;
  }

  const uint8_t* const payload = packet + 3;
  const size_t payload_size = packet_size - 3;
  const uint32_t declared = ReadU32LeRaw(payload);
  if (declared < 3) {
    if (failure_reason != nullptr) {
      *failure_reason = L"invalid CNWMessage declared size";
    }
    return false;
  }
  const size_t read_size = static_cast<size_t>(declared - 3);
  if (read_size < kLegacyQuickbarReadCursorStart || read_size > payload_size) {
    if (failure_reason != nullptr) {
      wchar_t reason[160]{};
      swprintf_s(reason, L"read buffer size %zu outside payload size %zu", read_size, payload_size);
      *failure_reason = reason;
    }
    return false;
  }

  LegacyQuickbarPacketReader reader{};
  reader.read_buffer = payload;
  reader.read_size = read_size;
  reader.fragments = payload + read_size;
  reader.fragment_size = payload_size - read_size;
  reader.cursor = kLegacyQuickbarReadCursorStart;

  stats->read_size = read_size;
  stats->fragment_size = payload_size - read_size;
  if (reader.fragment_size == 0) {
    if (failure_reason != nullptr) {
      *failure_reason = L"quickbar packet has no fragment stream";
    }
    return false;
  }

  uint32_t final_fragment_bits = 0;
  if (!ReadLegacyQuickbarBits(&reader, 3, &final_fragment_bits)) {
    if (failure_reason != nullptr) {
      *failure_reason = L"could not read quickbar fragment bit-size header";
    }
    return false;
  }
  reader.final_fragment_bits = static_cast<uint8_t>(final_fragment_bits);
  stats->score = 0;

  for (size_t slot = 0; slot < kLegacyQuickbarButtonCount; ++slot) {
    if (reader.cursor >= reader.read_size) {
      if (slot > 0 || stats->spells > 0 || stats->item_buttons_translated > 0) {
        stats->unsupported_buttons_blanked += static_cast<int>(kLegacyQuickbarButtonCount - slot);
        stats->final_cursor = reader.cursor;
        return true;
      }
      if (failure_reason != nullptr) {
        wchar_t reason[160]{};
        swprintf_s(reason, L"cursor reached end at slot %zu read_size=%zu", slot, reader.read_size);
        *failure_reason = reason;
      }
      return false;
    }

    LegacyQuickbarButton parsed{};
    parsed.source_offset = reader.cursor;
    parsed.source_fragment_offset = reader.fragment_cursor;
    parsed.source_fragment_bit = reader.fragment_bit;

    uint8_t type = 0;
    if (!ReadLegacyQuickbarByte(&reader, &type)) {
      if (slot > 0 || stats->spells > 0 || stats->item_buttons_translated > 0) {
        stats->unsupported_buttons_blanked += static_cast<int>(kLegacyQuickbarButtonCount - slot);
        stats->final_cursor = reader.cursor;
        return true;
      }
      if (failure_reason != nullptr) {
        wchar_t reason[160]{};
        swprintf_s(reason, L"could not read quickbar type at slot %zu cursor=%zu", slot, reader.cursor);
        *failure_reason = reason;
      }
      return false;
    }
    parsed.source_type = type;

    if (type == 1) {
      std::wstring item_failure;
      if (!ParseLegacyQuickbarItemPayload(&reader, &parsed, &item_failure)) {
        LogFormat(
            L"legacy quickbar item payload rejected: slot=%zu source-offset=%zu type=%u reason=%s",
            slot,
            parsed.source_offset,
            static_cast<unsigned int>(type),
            item_failure.empty() ? L"<none>" : item_failure.c_str());
        if (slot > 0 || stats->spells > 0 || stats->item_buttons_translated > 0) {
          stats->unsupported_buttons_blanked += static_cast<int>(kLegacyQuickbarButtonCount - slot);
          stats->final_cursor = parsed.source_offset;
          return true;
        }
        if (failure_reason != nullptr) {
          wchar_t reason[192]{};
          swprintf_s(
              reason,
              L"could not parse item payload at slot %zu cursor=%zu fragment=%zu:%u",
              slot,
              reader.cursor,
              reader.fragment_cursor,
              static_cast<unsigned int>(reader.fragment_bit));
          *failure_reason = std::wstring(reason) + L" reason=" + item_failure;
        }
        return false;
      }

      (*buttons)[slot] = parsed;
      ++stats->item_buttons_translated;
      stats->score += 100;
      continue;
    }

    if (!ParseLegacyQuickbarNonItemFromReader(&reader, type, &parsed)) {
      if (slot > 0 || stats->spells > 0 || stats->item_buttons_translated > 0) {
        stats->unsupported_buttons_blanked += static_cast<int>(kLegacyQuickbarButtonCount - slot);
        stats->final_cursor = parsed.source_offset;
        return true;
      }
      if (failure_reason != nullptr) {
        wchar_t reason[192]{};
        swprintf_s(
            reason,
            L"unsupported quickbar type %u at slot %zu cursor=%zu fragment=%zu:%u",
            static_cast<unsigned int>(type),
            slot,
            parsed.source_offset,
            parsed.source_fragment_offset,
            static_cast<unsigned int>(parsed.source_fragment_bit));
        *failure_reason = reason;
      }
      return false;
    }

    (*buttons)[slot] = parsed;
    if (parsed.spell) {
      ++stats->spells;
      stats->score += 80;
    } else if (parsed.no_payload) {
      ++stats->no_payload_buttons;
      stats->score += parsed.source_type == 0 ? 8 : 24;
    } else if (LegacyQuickbarButtonHasPreservableGeneralPayload(parsed)) {
      ++stats->general_buttons_preserved;
      stats->score += 32;
    } else {
      ++stats->unsupported_buttons_blanked;
      stats->score += 4;
    }
  }

  stats->final_cursor = reader.cursor;
  return true;
}

bool WriteLegacyQuickbarAppearanceBytes(
    void* message,
    const LegacyQuickbarItemObject& item,
    std::wstring* failure_reason) {
  auto* const write_byte = ResolveMessageWriteByte();
  auto* const write_dword = ResolveMessageWriteDword();
  if (write_byte == nullptr || write_dword == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage appearance write exports";
    }
    return false;
  }
  if (item.appearance_bytes.size() < 4) {
    if (failure_reason != nullptr) {
      wchar_t reason[160]{};
      swprintf_s(reason, L"item 0x%08X has no captured appearance bytes", item.object_id);
      *failure_reason = reason;
    }
    return false;
  }

  write_dword(message, ReadU32LeRaw(item.appearance_bytes.data()), 32);
  for (size_t index = 4; index < item.appearance_bytes.size(); ++index) {
    write_byte(message, item.appearance_bytes[index], 8, 1);
  }
  return true;
}

bool WriteQuickbarStringBytes(
    void* message,
    const std::string& value,
    std::wstring* failure_reason) {
  auto* const write_byte = ResolveMessageWriteByte();
  auto* const write_dword = ResolveMessageWriteDword();
  if (write_byte == nullptr || write_dword == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage string write exports";
    }
    return false;
  }
  if (value.size() > UINT32_MAX) {
    if (failure_reason != nullptr) {
      *failure_reason = L"quickbar string is too large";
    }
    return false;
  }

  write_dword(message, static_cast<uint32_t>(value.size()), 32);
  for (unsigned char ch : value) {
    write_byte(message, ch, 8, 1);
  }
  return true;
}

bool WriteQuickbarFixedCResRefBytes(
    void* message,
    const std::string& value,
    std::wstring* failure_reason) {
  auto* const write_byte = ResolveMessageWriteByte();
  if (write_byte == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage resref write export";
    }
    return false;
  }
  if (value.size() > kCResRefTextBytes) {
    if (failure_reason != nullptr) {
      *failure_reason = L"quickbar resref is too large";
    }
    return false;
  }

  for (size_t index = 0; index < kCResRefTextBytes; ++index) {
    const uint8_t value_byte = index < value.size()
        ? static_cast<uint8_t>(static_cast<unsigned char>(value[index]))
        : 0;
    write_byte(message, value_byte, 8, 1);
  }
  return true;
}

bool WriteQuickbarLocStringField(
    void* message,
    const LegacyQuickbarLocStringField& field,
    std::wstring* failure_reason) {
  auto* const write_bool = ResolveMessageWriteBool();
  auto* const write_byte = ResolveMessageWriteByte();
  auto* const write_dword = ResolveMessageWriteDword();
  if (write_bool == nullptr || write_byte == nullptr || write_dword == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage locstring write exports";
    }
    return false;
  }

  write_bool(message, field.custom_tlk ? 1 : 0);
  if (field.custom_tlk) {
    write_byte(message, field.language_selector ? 1 : 0, 1, 1);
    write_dword(message, field.string_ref, 32);
    return true;
  }

  return WriteQuickbarStringBytes(message, field.text, failure_reason);
}

std::wstring FormatQuickbarActiveItemName(const LegacyQuickbarActiveItemProperties& properties) {
  if (!properties.name_is_locstring) {
    return NarrowToWide(properties.string_name);
  }

  if (properties.locstring_name.custom_tlk) {
    wchar_t text[96]{};
    swprintf_s(
        text,
        L"strref=0x%08X custom=%d lang=%d",
        properties.locstring_name.string_ref,
        properties.locstring_name.custom_tlk ? 1 : 0,
        properties.locstring_name.language_selector ? 1 : 0);
    return text;
  }

  return NarrowToWide(properties.locstring_name.text);
}

bool ShouldFlattenQuickbarInlineLocStringName(
    const LegacyQuickbarActiveItemProperties& properties) {
  return properties.name_is_locstring &&
      !properties.locstring_name.custom_tlk &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_QUICKBAR_LOCSTRING_FLATTEN");
}

std::string QuickbarActiveItemFlattenedName(
    const LegacyQuickbarActiveItemProperties& properties) {
  if (!properties.name_is_locstring) {
    return properties.string_name;
  }
  return properties.locstring_name.custom_tlk
      ? std::string()
      : properties.locstring_name.text;
}

bool ExpandLegacyQuickbarActiveValueMaskBytes(
    const LegacyQuickbarActiveItemProperties& properties,
    std::array<uint8_t, 8>* expanded,
    std::wstring* failure_reason) {
  if (expanded == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing quickbar active property value-byte output";
    }
    return false;
  }

  expanded->fill(0);
  size_t value_index = 0;
  for (uint8_t bit = 0; bit < expanded->size(); ++bit) {
    if ((properties.value_mask & (1u << bit)) == 0) {
      continue;
    }
    if (value_index >= properties.value_mask_bytes.size()) {
      if (failure_reason != nullptr) {
        wchar_t reason[160]{};
        swprintf_s(
            reason,
            L"quickbar item active property value mask missing byte bit=%u mask=0x%02X count=%zu",
            static_cast<unsigned int>(bit),
            static_cast<unsigned int>(properties.value_mask),
            properties.value_mask_bytes.size());
        *failure_reason = reason;
      }
      return false;
    }
    (*expanded)[bit] = properties.value_mask_bytes[value_index++];
  }

  return true;
}

bool WriteQuickbarActiveItemProperties(
    void* message,
    const LegacyQuickbarActiveItemProperties& properties,
    uint32_t base_item_id,
    std::wstring* failure_reason) {
  auto* const write_bool = ResolveMessageWriteBool();
  auto* const write_byte = ResolveMessageWriteByte();
  auto* const write_dword = ResolveMessageWriteDword();
  auto* const write_int = ResolveMessageWriteInt();
  auto* const write_word = ResolveMessageWriteWord();
  if (write_bool == nullptr || write_byte == nullptr || write_dword == nullptr ||
      write_int == nullptr || write_word == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage active-item-property write exports";
    }
    return false;
  }
  if (properties.properties.size() > 255) {
    if (failure_reason != nullptr) {
      *failure_reason = L"quickbar item has too many active properties";
    }
    return false;
  }

  std::array<uint8_t, 8> expanded_value_bytes{};
  if (!ExpandLegacyQuickbarActiveValueMaskBytes(properties, &expanded_value_bytes, failure_reason)) {
    return false;
  }

  const bool flatten_inline_locstring_name =
      ShouldFlattenQuickbarInlineLocStringName(properties);
  const bool write_name_as_locstring =
      properties.name_is_locstring && !flatten_inline_locstring_name;
  const bool write_extra_active_bool =
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_QUICKBAR_EXTRA_ACTIVE_BOOL") &&
      (!write_name_as_locstring ||
       IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_QUICKBAR_EXTRA_ACTIVE_BOOL"));
  const bool write_item_build_extras =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_QUICKBAR_ITEM_BUILD_EXTRAS") &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_QUICKBAR_ITEM_BUILD_EXTRAS");
  const LONG observation = InterlockedIncrement(&g_legacy_quickbar_active_prop_write_observations);
  if (observation <= 80) {
    const std::wstring name = FormatQuickbarActiveItemName(properties);
    LogFormat(
        L"legacy quickbar active props write #%ld: base=%u/0x%08X armor=%d/0x%04X name-kind=%s name='%s' bools=%d,%d,%d,%d extra-active-bool=%d ee-build-extra=%d cost=%u stack=%u props=%zu state=0x%02X value-in=0x%02X value-out=0xFF compact=[%s] expanded=[%s]",
        observation,
        base_item_id,
        base_item_id,
        properties.has_armor_word ? 1 : 0,
        static_cast<unsigned int>(properties.armor_word),
        write_name_as_locstring
            ? L"locstring"
            : (properties.name_is_locstring ? L"locstring->string" : L"string"),
        name.c_str(),
        properties.post_name_bool1 ? 1 : 0,
        properties.post_name_bool2 ? 1 : 0,
        properties.post_name_bool3 ? 1 : 0,
        properties.post_name_bool4 ? 1 : 0,
        write_extra_active_bool ? 1 : 0,
        write_item_build_extras ? 1 : 0,
        properties.cost,
        properties.stack_or_charges,
        properties.properties.size(),
        static_cast<unsigned int>(properties.state_mask),
        static_cast<unsigned int>(properties.value_mask),
        FormatBytes(properties.value_mask_bytes.data(), properties.value_mask_bytes.size()).c_str(),
        FormatBytes(expanded_value_bytes.data(), expanded_value_bytes.size()).c_str());
  }

  if (LegacyQuickbarBaseItemRequiresActivePropertyWord(base_item_id)) {
    write_word(message, properties.has_armor_word ? properties.armor_word : 0, 16);
  }

  write_bool(message, write_name_as_locstring ? 1 : 0);
  if (write_name_as_locstring) {
    if (!WriteQuickbarLocStringField(message, properties.locstring_name, failure_reason)) {
      return false;
    }
  } else if (!WriteQuickbarStringBytes(
                 message,
                 QuickbarActiveItemFlattenedName(properties),
                 failure_reason)) {
    return false;
  }

  write_bool(message, properties.post_name_bool1 ? 1 : 0);
  write_dword(message, properties.cost, 32);
  write_dword(message, properties.stack_or_charges, 32);
  write_bool(message, properties.post_name_bool2 ? 1 : 0);
  write_bool(message, properties.post_name_bool3 ? 1 : 0);
  write_bool(message, properties.post_name_bool4 ? 1 : 0);
  if (write_extra_active_bool) {
    // EE v8193 reads one additional active-item bit for string-name payloads.
    // Without it, the next quickbar slot's present bit is consumed as the
    // secondary item flag.
    write_bool(message, 0);
  }
  if (write_item_build_extras) {
    // Current EE builds gate these behind CNWSPlayer::SatisfiesBuild on the
    // server side, but this client always expects them in EE-formatted item
    // payloads. Legacy HG does not send equivalents, so emit neutral values.
    write_int(message, 0, 32);
    if (!WriteQuickbarStringBytes(message, std::string(), failure_reason)) {
      return false;
    }
  }
  write_byte(message, static_cast<uint8_t>(properties.properties.size()), 8, 1);
  for (const LegacyQuickbarActivePropertyEntry& entry : properties.properties) {
    write_word(message, entry.property, 16);
    write_word(message, entry.subtype, 16);
    write_word(message, entry.cost_table_value, 16);
    write_byte(message, entry.param, 8, 1);
  }

  write_byte(message, properties.state_mask, 8, 1);
  // EE's own quickbar item writer always emits all eight active-value bytes
  // under a 0xFF mask, even when only a subset has meaningful values.
  write_byte(message, 0xFF, 8, 1);
  for (uint8_t bit = 0; bit < 8; ++bit) {
    write_byte(message, expanded_value_bytes[bit], 8, 1);
  }

  return true;
}

bool WriteEmptyQuickbarActiveItemProperties(
    void* message,
    uint32_t base_item_id,
    std::wstring* failure_reason) {
  auto* const write_bool = ResolveMessageWriteBool();
  auto* const write_byte = ResolveMessageWriteByte();
  auto* const write_dword = ResolveMessageWriteDword();
  auto* const write_int = ResolveMessageWriteInt();
  auto* const write_word = ResolveMessageWriteWord();
  if (write_bool == nullptr || write_byte == nullptr || write_dword == nullptr ||
      write_int == nullptr || write_word == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage active-item-property write exports";
    }
    return false;
  }

  if (LegacyQuickbarBaseItemRequiresActivePropertyWord(base_item_id)) {
    write_word(message, 0, 16);
  }

  const bool write_extra_active_bool =
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_QUICKBAR_EXTRA_ACTIVE_BOOL");
  const bool write_item_build_extras =
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_QUICKBAR_ITEM_BUILD_EXTRAS") &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_QUICKBAR_ITEM_BUILD_EXTRAS");
  write_bool(message, 0);       // Item name follows as CExoString, not CExoLocString.
  write_dword(message, 0, 32);  // Empty item name.
  write_bool(message, 0);
  write_dword(message, 0, 32);
  write_dword(message, 0, 32);
  write_bool(message, 0);
  write_bool(message, 0);
  write_bool(message, 0);
  if (write_extra_active_bool) {
    write_bool(message, 0);
  }
  if (write_item_build_extras) {
    write_int(message, 0, 32);
    if (!WriteQuickbarStringBytes(message, std::string(), failure_reason)) {
      return false;
    }
  }
  write_byte(message, 0, 8, 1);  // Active property count.
  write_byte(message, 0, 8, 1);  // Active property state mask.
  write_byte(message, 0, 8, 1);  // Active property value mask.
  return true;
}

bool WriteQuickbarItemObject(
    void* message,
    const LegacyQuickbarItemObject& item,
    bool include_int_param,
    size_t slot,
    const wchar_t* label,
    std::wstring* failure_reason) {
  auto* const write_bool = ResolveMessageWriteBool();
  auto* const write_dword = ResolveMessageWriteDword();
  auto* const write_int = ResolveMessageWriteInt();
  if (write_bool == nullptr || write_dword == nullptr || write_int == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage item write exports";
    }
    return false;
  }

  write_bool(message, item.present ? 1 : 0);
  if (!item.present) {
    return true;
  }

  write_dword(message, item.object_id, 32);
  if (include_int_param) {
    write_int(message, item.int_param, 32);
  }
  const bool preserve_active_props =
      item.active_props_skipped &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_QUICKBAR_ACTIVE_PROPS");
  const bool wrote =
      WriteLegacyQuickbarAppearanceBytes(message, item, failure_reason) &&
      (preserve_active_props
           ? WriteQuickbarActiveItemProperties(
                 message,
                 item.active_props,
                 item.base_item,
                 failure_reason)
           : WriteEmptyQuickbarActiveItemProperties(message, item.base_item, failure_reason));

  const LONG observation = InterlockedIncrement(&g_legacy_quickbar_item_write_observations);
  if (item.present && (observation <= 80 || !wrote)) {
    const CnwMessageWriteState state = ReadCnwMessageWriteState(message);
    LogFormat(
        L"legacy quickbar item write #%ld: slot=%zu part=%s object=0x%08X base=%u/0x%08X int=%d include-int=%d appearance-type=%u appearance=[%s] preserve-active=%d active-props=%d prop-count=%zu name='%s' wrote=%d state=[%s]",
        observation,
        slot,
        label != nullptr ? label : L"?",
        item.object_id,
        item.base_item,
        item.base_item,
        item.int_param,
        include_int_param ? 1 : 0,
        static_cast<unsigned int>(item.appearance_type),
        FormatBytes(item.appearance_bytes.data(), item.appearance_bytes.size()).c_str(),
        preserve_active_props ? 1 : 0,
        item.active_props_skipped ? 1 : 0,
        item.active_props.properties.size(),
        NarrowToWide(item.name_preview).c_str(),
        wrote ? 1 : 0,
        FormatCnwMessageWriteState(state).c_str());
  }

  return wrote;
}

bool ShouldPreserveLegacyQuickbarGeneralButton(const LegacyQuickbarButton& button) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_QUICKBAR_GENERAL_PRESERVE")) {
    return false;
  }
  return LegacyQuickbarButtonHasPreservableGeneralPayload(button);
}

bool WriteQuickbarGeneralButton(
    void* message,
    const LegacyQuickbarButton& button,
    size_t slot,
    std::wstring* failure_reason) {
  auto* const write_byte = ResolveMessageWriteByte();
  auto* const write_int = ResolveMessageWriteInt();
  if (write_byte == nullptr || write_int == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage general quickbar write exports";
    }
    return false;
  }

  write_byte(message, button.source_type, 8, 1);
  if (button.int_payload) {
    write_int(message, static_cast<int32_t>(button.int_payload_value), 32);
  }
  if (button.byte_payload) {
    write_byte(message, button.byte_payload_value, 8, 1);
  }
  if (button.source_type >= 11 && button.source_type <= 17) {
    if (!WriteQuickbarFixedCResRefBytes(message, button.resref_payload, failure_reason) ||
        !WriteQuickbarStringBytes(message, button.string_payload, failure_reason)) {
      return false;
    }
  } else if (button.source_type == 18) {
    if (!WriteQuickbarStringBytes(message, button.string_payload, failure_reason) ||
        !WriteQuickbarStringBytes(message, button.string_payload2, failure_reason)) {
      return false;
    }
  } else if (button.source_type == 29 || button.source_type == 30) {
    if (!WriteQuickbarFixedCResRefBytes(message, button.resref_payload, failure_reason)) {
      return false;
    }
  }

  const LONG observation = InterlockedIncrement(&g_legacy_quickbar_general_write_observations);
  if (observation <= 80 || button.source_type != 0) {
    const CnwMessageWriteState state = ReadCnwMessageWriteState(message);
    LogFormat(
        L"legacy quickbar general write #%ld: slot=%zu type=%u no-payload=%d int=%d value=%u/0x%08X byte=%d byte-value=%u resref='%s' text='%s' text2='%s' state=[%s]",
        observation,
        slot,
        static_cast<unsigned int>(button.source_type),
        button.no_payload ? 1 : 0,
        button.int_payload ? 1 : 0,
        button.int_payload_value,
        button.int_payload_value,
        button.byte_payload ? 1 : 0,
        static_cast<unsigned int>(button.byte_payload_value),
        NarrowToWide(button.resref_payload).c_str(),
        NarrowToWide(button.string_payload).c_str(),
        NarrowToWide(button.string_payload2).c_str(),
        FormatCnwMessageWriteState(state).c_str());
  }
  return true;
}

bool BuildEeQuickbarSetAllButtonsPacket(
    void* message,
    const std::array<LegacyQuickbarButton, kLegacyQuickbarButtonCount>& buttons,
    std::vector<unsigned char>* translated,
    std::wstring* failure_reason) {
  if (failure_reason != nullptr) {
    failure_reason->clear();
  }
  if (message == nullptr || translated == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing message or translated output";
    }
    return false;
  }

  auto* const create_write = ResolveMessageCreateWriteMessage();
  auto* const get_write = ResolveMessageGetWriteMessage();
  auto* const write_bool = ResolveMessageWriteBool();
  auto* const write_byte = ResolveMessageWriteByte();
  auto* const write_dword = ResolveMessageWriteDword();
  auto* const write_int = ResolveMessageWriteInt();
  auto* const write_word = ResolveMessageWriteWord();
  if (create_write == nullptr || get_write == nullptr || write_bool == nullptr ||
      write_byte == nullptr || write_dword == nullptr || write_int == nullptr || write_word == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage write exports";
    }
    return false;
  }

  create_write(message, 4096, 0xFFFFFFFFu, 1);
  for (const LegacyQuickbarButton& button : buttons) {
    if (button.spell) {
      write_byte(message, 2, 8, 1);
      write_byte(message, button.spell_class, 8, 1);
      write_dword(message, button.spell_id, 32);
      write_byte(message, button.metamagic, 8, 1);
      write_byte(message, button.domain, 8, 1);
    } else if (button.item_payload && (button.primary_item.present || button.secondary_item.present)) {
      write_byte(message, 1, 8, 1);
      const size_t slot = static_cast<size_t>(&button - buttons.data());
      if (!WriteQuickbarItemObject(message, button.primary_item, true, slot, L"primary", failure_reason) ||
          !WriteQuickbarItemObject(message, button.secondary_item, false, slot, L"secondary", failure_reason)) {
        return false;
      }
    } else if (ShouldPreserveLegacyQuickbarGeneralButton(button)) {
      const size_t slot = static_cast<size_t>(&button - buttons.data());
      if (!WriteQuickbarGeneralButton(message, button, slot, failure_reason)) {
        return false;
      }
    } else {
      write_byte(message, 0, 8, 1);
    }
  }

  unsigned char* output_message = nullptr;
  uint32_t output_size = 0;
  const int get_result = get_write(message, &output_message, &output_size);
  if (get_result == 0 || output_message == nullptr || output_size <= 3) {
    if (failure_reason != nullptr) {
      wchar_t reason[192]{};
      swprintf_s(
          reason,
          L"GetWriteMessage failed result=%d output=%p size=%u",
          get_result,
          output_message,
          output_size);
      *failure_reason = reason;
    }
    return false;
  }

  const uint32_t network_payload_size = output_size - 3;
  const LONG packet_observation = InterlockedIncrement(&g_legacy_quickbar_write_packet_observations);
  if (packet_observation <= 40) {
    const uint32_t declared_size =
        output_size >= 7 ? ReadU32LeRaw(output_message + 3) : 0;
    const uint32_t read_size =
        declared_size >= 3 ? declared_size - 3 : 0;
    const uint32_t fragment_size =
        output_size >= 3 + read_size ? output_size - 3 - read_size : 0;
    const unsigned char* const fragment_bytes =
        fragment_size > 0 ? output_message + 3 + read_size : nullptr;
    LogFormat(
        L"legacy quickbar write packet #%ld: output-size=%u network-payload=%u declared=%u read-size=%u fragments=%u fragment-bytes=[%s] header=[%s]",
        packet_observation,
        output_size,
        network_payload_size,
        declared_size,
        read_size,
        fragment_size,
        fragment_bytes != nullptr ? FormatLimitedBytes(fragment_bytes, fragment_size, 96).c_str() : L"",
        FormatLimitedBytes(output_message, output_size, 64).c_str());
  }
  translated->assign(3 + network_payload_size, 0);
  (*translated)[0] = 'P';
  (*translated)[1] = 30;
  (*translated)[2] = 1;
  memcpy(translated->data() + 3, output_message + 3, network_payload_size);
  return true;
}

bool BuildEeLoadBarEndPacket(
    void* message,
    uint32_t stall_event_id,
    uint32_t result_code,
    std::vector<unsigned char>* translated,
    std::wstring* failure_reason) {
  if (failure_reason != nullptr) {
    failure_reason->clear();
  }
  if (message == nullptr || translated == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing message or translated output";
    }
    return false;
  }

  auto* const create_write = ResolveMessageCreateWriteMessage();
  auto* const get_write = ResolveMessageGetWriteMessage();
  auto* const write_dword = ResolveMessageWriteDword();
  if (create_write == nullptr || get_write == nullptr || write_dword == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage write exports";
    }
    return false;
  }

  create_write(message, 4, 0xFFFFFFFFu, 1);
  write_dword(message, stall_event_id, 32);
  write_dword(message, result_code & 0x0Fu, 4);

  unsigned char* output_message = nullptr;
  uint32_t output_size = 0;
  const int get_result = get_write(message, &output_message, &output_size);
  if (get_result == 0 || output_message == nullptr || output_size <= 3) {
    if (failure_reason != nullptr) {
      wchar_t reason[192]{};
      swprintf_s(
          reason,
          L"GetWriteMessage failed result=%d output=%p size=%u",
          get_result,
          output_message,
          output_size);
      *failure_reason = reason;
    }
    return false;
  }

  const uint32_t network_payload_size = output_size - 3;
  if (network_payload_size < 5 || network_payload_size > 32) {
    if (failure_reason != nullptr) {
      wchar_t reason[192]{};
      swprintf_s(
          reason,
          L"LoadBar_End payload size is implausible: output=%u payload=%u",
          output_size,
          network_payload_size);
      *failure_reason = reason;
    }
    return false;
  }

  translated->assign(3 + network_payload_size, 0);
  (*translated)[0] = 'P';
  (*translated)[1] = 44;
  (*translated)[2] = 3;
  memcpy(translated->data() + 3, output_message + 3, network_payload_size);
  return true;
}

bool BuildEeLoadBarStartPacket(
    void* message,
    uint32_t stall_event_id,
    std::vector<unsigned char>* translated,
    std::wstring* failure_reason) {
  if (failure_reason != nullptr) {
    failure_reason->clear();
  }
  if (message == nullptr || translated == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing message or translated output";
    }
    return false;
  }

  auto* const create_write = ResolveMessageCreateWriteMessage();
  auto* const get_write = ResolveMessageGetWriteMessage();
  auto* const write_dword = ResolveMessageWriteDword();
  if (create_write == nullptr || get_write == nullptr || write_dword == nullptr) {
    if (failure_reason != nullptr) {
      *failure_reason = L"missing CNWMessage write exports";
    }
    return false;
  }

  create_write(message, 4, 0xFFFFFFFFu, 1);
  write_dword(message, stall_event_id, 32);

  unsigned char* output_message = nullptr;
  uint32_t output_size = 0;
  const int get_result = get_write(message, &output_message, &output_size);
  if (get_result == 0 || output_message == nullptr || output_size <= 3) {
    if (failure_reason != nullptr) {
      wchar_t reason[192]{};
      swprintf_s(
          reason,
          L"GetWriteMessage failed result=%d output=%p size=%u",
          get_result,
          output_message,
          output_size);
      *failure_reason = reason;
    }
    return false;
  }

  const uint32_t network_payload_size = output_size - 3;
  if (network_payload_size < 8 || network_payload_size > 24) {
    if (failure_reason != nullptr) {
      wchar_t reason[192]{};
      swprintf_s(
          reason,
          L"LoadBar_Start payload size is implausible: output=%u payload=%u",
          output_size,
          network_payload_size);
      *failure_reason = reason;
    }
    return false;
  }

  translated->assign(3 + network_payload_size, 0);
  (*translated)[0] = 'P';
  (*translated)[1] = 44;
  (*translated)[2] = 1;
  memcpy(translated->data() + 3, output_message + 3, network_payload_size);
  return true;
}

bool TryExtractLoadBarStallEventId(const unsigned char* packet, uint32_t packet_size, uint32_t* stall_event_id) {
  if (stall_event_id == nullptr) {
    return false;
  }
  *stall_event_id = 0;
  if (packet == nullptr || packet_size < 11 || packet[0] != 'P' || packet[1] != 44) {
    return false;
  }
  *stall_event_id = ReadU32LeRaw(packet + 7);
  return true;
}

int64_t TryDispatchSyntheticEeLoadBarEnd(
    void* message,
    LONG server_dispatch,
    const CnwMessageReadState& after_area_state,
    const wchar_t* reason) {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_SYNTHETIC_LOADBAR_END")) {
    return 0;
  }
  if (g_server_to_player_message_original == nullptr) {
    LogFormat(
        L"synthetic LoadBar_End unavailable: server_dispatch=%ld message=%p missing original server-to-player dispatcher",
        server_dispatch,
        message);
    return 0;
  }

  constexpr uint32_t kDefaultAreaStallEventId = 2;
  constexpr uint32_t kSuccessResultCode = 0;
  const bool close_real_loadbar = g_loadbar_outstanding != 0 && g_last_loadbar_stall_event_id > 0;
  const uint32_t stall_event_id = close_real_loadbar
      ? static_cast<uint32_t>(g_last_loadbar_stall_event_id)
      : kDefaultAreaStallEventId;
  std::vector<unsigned char> start_packet;
  std::vector<unsigned char> end_packet;
  std::wstring failure;
  if (!close_real_loadbar &&
      !BuildEeLoadBarStartPacket(message, stall_event_id, &start_packet, &failure)) {
    LogFormat(
        L"synthetic LoadBar_Start unavailable: server_dispatch=%ld message=%p reason=%s",
        server_dispatch,
        message,
        failure.empty() ? L"<unknown>" : failure.c_str());
    return 0;
  }
  if (!BuildEeLoadBarEndPacket(message, stall_event_id, kSuccessResultCode, &end_packet, &failure)) {
    LogFormat(
        L"synthetic LoadBar_End unavailable: server_dispatch=%ld message=%p reason=%s",
        server_dispatch,
        message,
        failure.empty() ? L"<unknown>" : failure.c_str());
    return 0;
  }

  auto dispatch_loadbar_packet = [&](unsigned char minor, const std::vector<unsigned char>& packet) -> int64_t {
    const int previous_depth = g_server_to_player_message_depth;
    const LONG previous_observation = g_current_server_to_player_message_observation;
    const unsigned char previous_major = g_current_server_to_player_major;
    const unsigned char previous_minor = g_current_server_to_player_minor;
    const uint32_t previous_size = g_current_server_to_player_size;
    const bool previous_live_opcode_known = g_current_server_to_player_live_opcode_known;
    const unsigned char previous_live_opcode = g_current_server_to_player_live_opcode;

    ++g_server_to_player_message_depth;
    g_current_server_to_player_message_observation = server_dispatch;
    g_current_server_to_player_major = 44;
    g_current_server_to_player_minor = minor;
    g_current_server_to_player_size = static_cast<uint32_t>(packet.size());
    g_current_server_to_player_live_opcode_known = false;
    g_current_server_to_player_live_opcode = 0;

    const int64_t dispatch_result = g_server_to_player_message_original(
        message,
        const_cast<unsigned char*>(packet.data()),
        static_cast<uint32_t>(packet.size()));

    g_server_to_player_message_depth = previous_depth;
    g_current_server_to_player_message_observation = previous_observation;
    g_current_server_to_player_major = previous_major;
    g_current_server_to_player_minor = previous_minor;
    g_current_server_to_player_size = previous_size;
    g_current_server_to_player_live_opcode_known = previous_live_opcode_known;
    g_current_server_to_player_live_opcode = previous_live_opcode;
    return dispatch_result;
  };

  CnwMessageReadState after_start = after_area_state;
  if (!close_real_loadbar) {
    const LONG start_observation = InterlockedIncrement(&g_synthetic_loadbar_start_observations);
    LogFormat(
        L"synthetic LoadBar_Start #%ld begin: server_dispatch=%ld message=%p stall=%u reason=%s area_state=[%s] size=%zu raw=[%s]",
        start_observation,
        server_dispatch,
        message,
        stall_event_id,
        reason != nullptr ? reason : L"<unknown>",
        FormatCnwMessageReadState(after_area_state).c_str(),
        start_packet.size(),
        FormatLimitedBytes(start_packet.data(), start_packet.size(), 96).c_str());

    const int64_t start_result = dispatch_loadbar_packet(1, start_packet);
    after_start = ReadCnwMessageReadState(message);
    LogFormat(
        L"synthetic LoadBar_Start #%ld end: server_dispatch=%ld message=%p result=0x%llX/%lld synthetic_count=%ld after=[%s] next-after=[%s]",
        start_observation,
        server_dispatch,
        message,
        static_cast<unsigned long long>(start_result),
        static_cast<long long>(start_result),
        g_synthetic_loadbar_start_observations,
        FormatCnwMessageReadState(after_start).c_str(),
        FormatCnwMessageBufferPreview(message, after_start).c_str());
  } else {
    LogFormat(
        L"synthetic LoadBar_Start skipped: server_dispatch=%ld message=%p closing real outstanding stall=%u real_start_dispatch=%ld reason=%s area_state=[%s]",
        server_dispatch,
        message,
        stall_event_id,
        g_last_loadbar_start_dispatch,
        reason != nullptr ? reason : L"<unknown>",
        FormatCnwMessageReadState(after_area_state).c_str());
  }

  const LONG synthetic_observation = InterlockedIncrement(&g_synthetic_loadbar_end_observations);
  LogFormat(
      L"synthetic LoadBar_End #%ld begin: server_dispatch=%ld message=%p stall=%u result-code=%u close-real=%d reason=%s area_state=[%s] size=%zu raw=[%s]",
      synthetic_observation,
      server_dispatch,
      message,
      stall_event_id,
      kSuccessResultCode,
      close_real_loadbar ? 1 : 0,
      reason != nullptr ? reason : L"<unknown>",
      FormatCnwMessageReadState(after_start).c_str(),
      end_packet.size(),
      FormatLimitedBytes(end_packet.data(), end_packet.size(), 96).c_str());

  const int64_t result = dispatch_loadbar_packet(3, end_packet);
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  LogFormat(
      L"synthetic LoadBar_End #%ld end: server_dispatch=%ld message=%p result=0x%llX/%lld synthetic_count=%ld after=[%s] next-after=[%s]",
      synthetic_observation,
      server_dispatch,
      message,
      static_cast<unsigned long long>(result),
      static_cast<long long>(result),
      g_synthetic_loadbar_end_observations,
      FormatCnwMessageReadState(after).c_str(),
      FormatCnwMessageBufferPreview(message, after).c_str());
  if (close_real_loadbar) {
    InterlockedExchange(&g_loadbar_outstanding, 0);
  }
  return result;
}

bool TryTranslateLegacyQuickbarSetAllButtons(
    void* message,
    const unsigned char* packet,
    uint32_t packet_size,
    std::vector<unsigned char>* translated,
    LegacyQuickbarParseStats* stats,
    std::wstring* summary,
    std::wstring* failure_reason) {
  if (translated != nullptr) {
    translated->clear();
  }
  if (summary != nullptr) {
    summary->clear();
  }
  if (failure_reason != nullptr) {
    failure_reason->clear();
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_QUICKBAR_TRANSLATE")) {
    if (failure_reason != nullptr) {
      *failure_reason = L"disabled by HG_BRIDGE_DISABLE_LEGACY_QUICKBAR_TRANSLATE";
    }
    return false;
  }

  try {
    LogFormat(
        L"legacy quickbar translation probe begin: message=%p packet-size=%u",
        message,
        packet_size);
    std::array<LegacyQuickbarButton, kLegacyQuickbarButtonCount> buttons{};
    LegacyQuickbarParseStats local_stats{};
    std::wstring parse_failure;
    if (!ParseLegacyQuickbarSetAllButtons(packet, packet_size, &buttons, &local_stats, &parse_failure)) {
      if (failure_reason != nullptr) {
        *failure_reason = parse_failure;
      }
      return false;
    }
    LogFormat(
        L"legacy quickbar translation parse ok: message=%p read-size=%zu fragments=%zu final-cursor=%zu score=%d spells=%d no-payload=%d general-preserved=%d items-translated=%d unsupported-blanked=%d",
        message,
        local_stats.read_size,
        local_stats.fragment_size,
        local_stats.final_cursor,
        local_stats.score,
        local_stats.spells,
        local_stats.no_payload_buttons,
        local_stats.general_buttons_preserved,
        local_stats.item_buttons_translated,
        local_stats.unsupported_buttons_blanked);

    std::wstring build_failure;
    LogFormat(L"legacy quickbar translation build begin: message=%p", message);
    if (!BuildEeQuickbarSetAllButtonsPacket(message, buttons, translated, &build_failure)) {
      if (failure_reason != nullptr) {
        *failure_reason = build_failure;
      }
      return false;
    }
    LogFormat(
        L"legacy quickbar translation build ok: message=%p translated-size=%zu",
        message,
        translated != nullptr ? translated->size() : 0);

    if (translated == nullptr || translated->size() < 8 || translated->size() > 16384) {
      if (failure_reason != nullptr) {
        wchar_t reason[192]{};
        swprintf_s(
            reason,
            L"translated quickbar size is implausible: %zu",
            translated != nullptr ? translated->size() : 0);
        *failure_reason = reason;
      }
      if (translated != nullptr) {
        translated->clear();
      }
      return false;
    }

    if (stats != nullptr) {
      *stats = local_stats;
    }
    if (summary != nullptr) {
      *summary = FormatLegacyQuickbarSummary(buttons);
    }
    return true;
  } catch (const std::exception& ex) {
    if (failure_reason != nullptr) {
      *failure_reason = L"exception during quickbar translation: " + NarrowToWide(ex.what());
    }
    if (translated != nullptr) {
      translated->clear();
    }
    return false;
  } catch (...) {
    if (failure_reason != nullptr) {
      *failure_reason = L"unknown exception during quickbar translation";
    }
    if (translated != nullptr) {
      translated->clear();
    }
    return false;
  }
}

int64_t __fastcall HookedServerToPlayerMessage(void* message, unsigned char* buffer, uint32_t size) {
  const LONG observation = InterlockedIncrement(&g_server_to_player_message_observations);
  const DWORD thread_id = GetCurrentThreadId();
  const ULONGLONG dispatch_start_tick = GetTickCount64();
  const unsigned char major = size > 1 && buffer != nullptr ? buffer[1] : 0;
  const unsigned char minor = size > 2 && buffer != nullptr ? buffer[2] : 0;
  const bool live_object_opcode_known =
      buffer != nullptr && size > 7 && buffer[0] == 'P' && major == 5 && minor == 1;
  const unsigned char live_object_opcode = live_object_opcode_known ? buffer[7] : 0;
  const std::wstring live_object_opcode_text =
      FormatLiveObjectOpcode(live_object_opcode_known, live_object_opcode);
  const std::wstring name = LookupMajorMinorNameWide(major, minor);
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  const bool is_server_chat = buffer != nullptr && size >= 3 && buffer[0] == 'P' && major == 18 && minor == 11;
  const bool has_password_gate_prompt = is_server_chat &&
      (PacketContainsAsciiNoCase(buffer, size, "speak your password") ||
       PacketContainsAsciiNoCase(buffer, size, "password before you can continue"));
  const bool has_hg_welcome_text = is_server_chat &&
      PacketContainsAsciiNoCase(buffer, size, "Welcome to Higher Ground");
  const bool has_hg_password_incorrect = buffer != nullptr && size >= 3 && buffer[0] == 'P' &&
      PacketContainsAsciiNoCase(buffer, size, "password is incorrect");

  const bool is_packet = buffer != nullptr && size >= 3 && buffer[0] == 'P';
  const bool is_area_packet = is_packet && major == 4;
  const bool is_login_or_legacy_load_packet = is_packet && major == 2;
  const bool is_loadscreen_packet = is_packet && major == 44;

  if (is_packet) {
    if (major == 4 && minor == 1) {
      const LONG area_observation = InterlockedIncrement(&g_area_load_dispatch_observations);
      if (area_observation <= 20) {
        LogFormat(
            L"area-load dispatch observed #%ld: server_dispatch=%ld message=%p name='%s' size=%u raw=[%s]",
            area_observation,
            observation,
            message,
            name.c_str(),
            size,
            FormatLimitedBytes(buffer, size, 96).c_str());
      }
      if (area_observation <= 3) {
        BeginRuntimeResourceDiagnostics(L"area-load dispatch", 180);
      }
    } else if (major == 4) {
      const LONG area_observation = InterlockedIncrement(&g_area_misc_dispatch_observations);
      if (area_observation <= 80 || RuntimeResourceDiagnosticScopeActive()) {
        LogFormat(
            L"area dispatch observed #%ld: server_dispatch=%ld message=%p name='%s' minor=%u size=%u raw=[%s] before=[%s]",
            area_observation,
            observation,
            message,
            name.c_str(),
            static_cast<unsigned int>(minor),
            size,
            FormatLimitedBytes(buffer, size, 96).c_str(),
            FormatCnwMessageReadState(before).c_str());
      }
    } else if (major == 2) {
      const LONG load_observation = InterlockedIncrement(&g_loadscreen_dispatch_observations);
      if (load_observation <= 160 || RuntimeResourceDiagnosticScopeActive()) {
        LogFormat(
            L"login dispatch observed #%ld: server_dispatch=%ld message=%p name='%s' minor=%u size=%u raw=[%s] before=[%s]",
            load_observation,
            observation,
            message,
            name.c_str(),
            static_cast<unsigned int>(minor),
            size,
            FormatLimitedBytes(buffer, size, 96).c_str(),
            FormatCnwMessageReadState(before).c_str());
      }
    } else if (major == 44) {
      const LONG load_observation = InterlockedIncrement(&g_loadscreen_dispatch_observations);
      uint32_t stall_event_id = 0;
      const bool stall_event_id_known = TryExtractLoadBarStallEventId(buffer, size, &stall_event_id);
      if (minor == 1 && stall_event_id_known) {
        const LONG start_observation = InterlockedIncrement(&g_real_loadbar_start_observations);
        InterlockedExchange(&g_loadbar_outstanding, 1);
        InterlockedExchange(&g_last_loadbar_stall_event_id, static_cast<LONG>(stall_event_id));
        InterlockedExchange(&g_last_loadbar_start_dispatch, observation);
        LogFormat(
            L"real LoadBar_Start #%ld: server_dispatch=%ld message=%p stall=%u size=%u raw=[%s] before=[%s]",
            start_observation,
            observation,
            message,
            stall_event_id,
            size,
            FormatLimitedBytes(buffer, size, 96).c_str(),
            FormatCnwMessageReadState(before).c_str());
      } else if (minor == 3 && stall_event_id_known) {
        const LONG end_observation = InterlockedIncrement(&g_real_loadbar_end_observations);
        const LONG previous_outstanding = InterlockedExchange(&g_loadbar_outstanding, 0);
        LogFormat(
            L"real LoadBar_End #%ld: server_dispatch=%ld message=%p stall=%u previous_outstanding=%ld last_start_dispatch=%ld size=%u raw=[%s] before=[%s]",
            end_observation,
            observation,
            message,
            stall_event_id,
            previous_outstanding,
            g_last_loadbar_start_dispatch,
            size,
            FormatLimitedBytes(buffer, size, 96).c_str(),
            FormatCnwMessageReadState(before).c_str());
      }
      if (load_observation <= 80 || RuntimeResourceDiagnosticScopeActive()) {
        LogFormat(
            L"loadbar dispatch observed #%ld: server_dispatch=%ld message=%p name='%s' minor=%u stall-known=%d stall=%u outstanding=%ld size=%u raw=[%s] before=[%s]",
            load_observation,
            observation,
            message,
            name.c_str(),
            static_cast<unsigned int>(minor),
            stall_event_id_known ? 1 : 0,
            stall_event_id,
            g_loadbar_outstanding,
            size,
            FormatLimitedBytes(buffer, size, 96).c_str(),
            FormatCnwMessageReadState(before).c_str());
      }
    } else if (major == 14) {
      std::wstring party_diagnostic;
      bool party_suspicious = false;
      if (TryFormatLegacyPartyPacketDiagnostic(
              buffer,
              size,
              &party_diagnostic,
              &party_suspicious)) {
        const LONG party_observation =
            InterlockedIncrement(&g_party_packet_diagnostic_observations);
        if (party_observation <= 120 || party_suspicious || RuntimeResourceDiagnosticScopeActive()) {
          LogFormat(
              L"party packet diagnostic #%ld: server_dispatch=%ld message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X size=%u suspicious=%d %s raw=[%s] before=[%s]",
              party_observation,
              observation,
              message,
              name.c_str(),
              static_cast<unsigned int>(major),
              static_cast<unsigned int>(major),
              static_cast<unsigned int>(minor),
              static_cast<unsigned int>(minor),
              size,
              party_suspicious ? 1 : 0,
              party_diagnostic.c_str(),
              FormatLimitedBytes(buffer, size, 128).c_str(),
              FormatCnwMessageReadState(before).c_str());
        }
      }
    } else if (major == 10) {
      const LONG object_observation = InterlockedIncrement(&g_object_update_dispatch_observations);
      if (object_observation <= 60) {
        LogFormat(
            L"object-update dispatch observed #%ld: server_dispatch=%ld message=%p name='%s' minor=%u size=%u raw=[%s]",
            object_observation,
            observation,
            message,
            name.c_str(),
            static_cast<unsigned int>(minor),
            size,
            FormatLimitedBytes(buffer, size, 96).c_str());
      }
    } else if (major == 5) {
      const LONG live_observation = InterlockedIncrement(&g_live_object_dispatch_observations);
      if (live_observation <= 80) {
        LogFormat(
            L"live-object dispatch observed #%ld: server_dispatch=%ld message=%p name='%s' minor=%u size=%u %s raw=[%s]",
            live_observation,
            observation,
            message,
            name.c_str(),
            static_cast<unsigned int>(minor),
            size,
            live_object_opcode_text.c_str(),
            FormatLimitedBytes(buffer, size, 96).c_str());
      }
      if (minor == 8) {
        uint32_t object_id = 0;
        const bool object_id_known = TryReadU32LeFromRaw(buffer, size, 3, &object_id);
        const bool usable_known = size > 7;
        const unsigned int usable_raw = usable_known ? static_cast<unsigned int>(buffer[7]) : 0;
        const LONG useable_observation =
            InterlockedIncrement(&g_object_useable_dispatch_observations);
        if (useable_observation <= 160 || RuntimeResourceDiagnosticScopeActive()) {
          LogFormat(
              L"object-useable dispatch observed #%ld: server_dispatch=%ld message=%p name='%s' size=%u object_id_known=%d object_id=0x%08X usable_raw_known=%d usable_raw=0x%02X before=[%s] raw=[%s]",
              useable_observation,
              observation,
              message,
              name.c_str(),
              size,
              object_id_known ? 1 : 0,
              object_id,
              usable_known ? 1 : 0,
              usable_raw,
              FormatCnwMessageReadState(before).c_str(),
              FormatLimitedBytes(buffer, size, 96).c_str());
        }
      }
      std::wstring packet_summary;
      bool transition_related = false;
      if (TryFormatLegacyLiveObjectPacketSummary(buffer, size, &packet_summary, &transition_related)) {
        const LONG summary_observation =
            InterlockedIncrement(&g_live_object_packet_summary_observations);
        if (summary_observation <= 180 || transition_related || RuntimeResourceDiagnosticScopeActive()) {
          LogFormat(
              L"live-object packet summary #%ld: server_dispatch=%ld message=%p name='%s' minor=%u size=%u %s %s",
              summary_observation,
              observation,
              message,
              name.c_str(),
              static_cast<unsigned int>(minor),
              size,
              live_object_opcode_text.c_str(),
              packet_summary.c_str());
        }
      }
    }
  }

  if (is_server_chat || has_hg_password_incorrect) {
    const LONG chat_observation = InterlockedIncrement(&g_server_chat_message_observations);
    const std::string text_preview = ExtractAsciiPacketText(buffer + 3, size - 3, 768);
    if (chat_observation <= 80 || has_password_gate_prompt || has_hg_welcome_text || has_hg_password_incorrect) {
      LogFormat(
          L"server chat dispatch #%ld: server_dispatch=%ld message=%p name='%s' size=%u major=%u minor=%u password_prompt=%d password_incorrect=%d hg_welcome=%d text-preview='%s'",
          chat_observation,
          observation,
          message,
          name.c_str(),
          size,
          static_cast<unsigned int>(major),
          static_cast<unsigned int>(minor),
          has_password_gate_prompt ? 1 : 0,
          has_hg_password_incorrect ? 1 : 0,
          has_hg_welcome_text ? 1 : 0,
          NarrowToWide(text_preview).c_str());
    }
    if (has_password_gate_prompt || has_hg_welcome_text || has_hg_password_incorrect) {
      const LONG gate_observation = InterlockedIncrement(&g_hg_password_gate_observations);
      LogFormat(
          L"HG password-gate marker #%ld: server_dispatch=%ld message=%p prompt=%d incorrect=%d welcome=%d area_dispatches=%ld object_dispatches=%ld live_dispatches=%ld live_updates=%ld legacy_object_shims=%ld legacy_live_tail_shims=%ld hg_m_sendto=%ld recv_hg=%ld",
          gate_observation,
          observation,
          message,
          has_password_gate_prompt ? 1 : 0,
          has_hg_password_incorrect ? 1 : 0,
          has_hg_welcome_text ? 1 : 0,
          g_area_load_dispatch_observations,
          g_object_update_dispatch_observations,
          g_live_object_dispatch_observations,
          g_live_message_update_observations,
          g_legacy_object_update_field_observations,
          g_legacy_live_object_update_tail_observations,
          g_sendto_hg_m_observations,
          g_recvfrom_hg_observations);
    }
  }

  if (observation <= 260) {
    LogFormat(
        L"server-to-player dispatch #%ld begin: thread=%lu message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X size=%u %s raw=[%s] before=[%s]",
        observation,
        static_cast<unsigned long>(thread_id),
        message,
        name.c_str(),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        static_cast<unsigned int>(minor),
        size,
        live_object_opcode_text.c_str(),
        FormatLimitedBytes(buffer, size, 96).c_str(),
        FormatCnwMessageReadState(before).c_str());
  }

  if (g_auto_connect_ip_known && buffer != nullptr && size >= 3 && buffer[0] == 'P' && major == 3 && minor == 1) {
    EnsureLegacyModuleTlkLoadedFromPacket(buffer, size, L"module-info dispatch");
  }

  if (g_auto_connect_ip_known && size == 3 && buffer != nullptr && buffer[0] == 'P' && major == 2 && minor == 5 &&
      IsTruthyEnvironmentFlag(L"HG_BRIDGE_SKIP_LEGACY_LOADBAR_START")) {
    auto* const set_read_message = ResolveMessageSetReadMessage();
    if (set_read_message == nullptr) {
      LogFormat(
          L"server-to-player dispatch #%ld legacy loadbar-start shim unavailable: thread=%lu message=%p missing CNWMessage::SetReadMessage; falling back to original handler",
          observation,
          static_cast<unsigned long>(thread_id),
          message);
    } else {
      const int set_result = set_read_message(message, buffer + 3, size - 3, 0xFFFFFFFFu, 1);
      const CnwMessageReadState after = ReadCnwMessageReadState(message);
      LogFormat(
          L"server-to-player dispatch #%ld consumed legacy loadbar start: thread=%lu message=%p set-read-result=%d before=[%s] after=[%s]",
          observation,
          static_cast<unsigned long>(thread_id),
          message,
          set_result,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageReadState(after).c_str());
      return 0;
    }
  }

  if (g_auto_connect_ip_known && buffer != nullptr && size >= 3 && buffer[0] == 'P' && major == 30 && minor == 1) {
    std::vector<unsigned char> translated_quickbar;
    LegacyQuickbarParseStats quickbar_stats{};
    std::wstring quickbar_summary;
    std::wstring quickbar_failure;
    if (TryTranslateLegacyQuickbarSetAllButtons(
            message,
            buffer,
            size,
            &translated_quickbar,
            &quickbar_stats,
            &quickbar_summary,
            &quickbar_failure)) {
      const LONG quickbar_observation = InterlockedIncrement(&g_legacy_quickbar_translate_observations);
      LogFormat(
          L"server-to-player dispatch #%ld translated legacy quickbar SetAllButtons #%ld: thread=%lu message=%p old-size=%u new-size=%zu read-size=%zu fragments=%zu final-cursor=%zu score=%d spells=%d no-payload=%d general-preserved=%d items-translated=%d items-blanked=%d unsupported-blanked=%d before=[%s] summary=[%s] old-raw=[%s] new-raw=[%s]",
          observation,
          quickbar_observation,
          static_cast<unsigned long>(thread_id),
          message,
          size,
          translated_quickbar.size(),
          quickbar_stats.read_size,
          quickbar_stats.fragment_size,
          quickbar_stats.final_cursor,
          quickbar_stats.score,
          quickbar_stats.spells,
          quickbar_stats.no_payload_buttons,
          quickbar_stats.general_buttons_preserved,
          quickbar_stats.item_buttons_translated,
          quickbar_stats.item_buttons_blanked,
          quickbar_stats.unsupported_buttons_blanked,
          FormatCnwMessageReadState(before).c_str(),
          quickbar_summary.c_str(),
          FormatLimitedBytes(buffer, size, 160).c_str(),
          FormatLimitedBytes(translated_quickbar.data(), translated_quickbar.size(), 160).c_str());

      const int previous_depth = g_server_to_player_message_depth;
      const LONG previous_observation = g_current_server_to_player_message_observation;
      const unsigned char previous_major = g_current_server_to_player_major;
      const unsigned char previous_minor = g_current_server_to_player_minor;
      const uint32_t previous_size = g_current_server_to_player_size;
      const bool previous_live_opcode_known = g_current_server_to_player_live_opcode_known;
      const unsigned char previous_live_opcode = g_current_server_to_player_live_opcode;

      ++g_server_to_player_message_depth;
      g_current_server_to_player_message_observation = observation;
      g_current_server_to_player_major = major;
      g_current_server_to_player_minor = minor;
      g_current_server_to_player_size = static_cast<uint32_t>(translated_quickbar.size());
      g_current_server_to_player_live_opcode_known = false;
      g_current_server_to_player_live_opcode = 0;

      const int64_t result = g_server_to_player_message_original != nullptr
          ? g_server_to_player_message_original(
                message,
                translated_quickbar.data(),
                static_cast<uint32_t>(translated_quickbar.size()))
          : 0;

      g_server_to_player_message_depth = previous_depth;
      g_current_server_to_player_message_observation = previous_observation;
      g_current_server_to_player_major = previous_major;
      g_current_server_to_player_minor = previous_minor;
      g_current_server_to_player_size = previous_size;
      g_current_server_to_player_live_opcode_known = previous_live_opcode_known;
      g_current_server_to_player_live_opcode = previous_live_opcode;

      const CnwMessageReadState after = ReadCnwMessageReadState(message);
      LogFormat(
          L"server-to-player dispatch #%ld legacy quickbar translation end: thread=%lu message=%p result=0x%llX/%lld after=[%s]",
          observation,
          static_cast<unsigned long>(thread_id),
          message,
          static_cast<unsigned long long>(result),
          static_cast<long long>(result),
          FormatCnwMessageReadState(after).c_str());
      return result;
    }

    const LONG quickbar_failure_observation = g_legacy_quickbar_translate_observations + 1;
    if (observation <= 260 || quickbar_failure_observation <= 20) {
      LogFormat(
          L"server-to-player dispatch #%ld legacy quickbar translation unavailable: thread=%lu message=%p reason=%s size=%u before=[%s] raw=[%s]",
          observation,
          static_cast<unsigned long>(thread_id),
          message,
          quickbar_failure.empty() ? L"<unknown>" : quickbar_failure.c_str(),
          size,
          FormatCnwMessageReadState(before).c_str(),
          FormatLimitedBytes(buffer, size, 160).c_str());
    }
  }

  if (g_auto_connect_ip_known && buffer != nullptr && size >= 3 && buffer[0] == 'P' && major == 30 && minor == 1 &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_QUICKBAR_SKIP")) {
    const LONG quickbar_observation = InterlockedIncrement(&g_legacy_quickbar_skip_observations);
    auto* const set_read_message = ResolveMessageSetReadMessage();
    int set_result = -1;
    CnwMessageReadState after_skip = before;
    if (set_read_message != nullptr) {
      set_result = set_read_message(message, buffer + 3, 0, 0xFFFFFFFFu, 1);
      after_skip = ReadCnwMessageReadState(message);
    }
    LogFormat(
        L"server-to-player dispatch #%ld consumed legacy quickbar SetAllButtons #%ld: thread=%lu message=%p size=%u set-read-result=%d before=[%s] after=[%s] raw=[%s]",
        observation,
        quickbar_observation,
        static_cast<unsigned long>(thread_id),
        message,
        size,
        set_result,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after_skip).c_str(),
        FormatLimitedBytes(buffer, size, 160).c_str());
    return 0;
  }

  const int previous_depth = g_server_to_player_message_depth;
  const LONG previous_observation = g_current_server_to_player_message_observation;
  const unsigned char previous_major = g_current_server_to_player_major;
  const unsigned char previous_minor = g_current_server_to_player_minor;
  const uint32_t previous_size = g_current_server_to_player_size;
  const bool previous_live_opcode_known = g_current_server_to_player_live_opcode_known;
  const unsigned char previous_live_opcode = g_current_server_to_player_live_opcode;
  const LONG client_area_loaded_before_dispatch = g_client_to_server_area_loaded_observations;

  ++g_server_to_player_message_depth;
  g_current_server_to_player_message_observation = observation;
  g_current_server_to_player_major = major;
  g_current_server_to_player_minor = minor;
  g_current_server_to_player_size = size;
  g_current_server_to_player_live_opcode_known = live_object_opcode_known;
  g_current_server_to_player_live_opcode = live_object_opcode;

  const int64_t result = g_server_to_player_message_original != nullptr
      ? g_server_to_player_message_original(message, buffer, size)
      : 0;

  g_server_to_player_message_depth = previous_depth;
  g_current_server_to_player_message_observation = previous_observation;
  g_current_server_to_player_major = previous_major;
  g_current_server_to_player_minor = previous_minor;
  g_current_server_to_player_size = previous_size;
  g_current_server_to_player_live_opcode_known = previous_live_opcode_known;
  g_current_server_to_player_live_opcode = previous_live_opcode;

  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  if (observation <= 260 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    LogFormat(
        L"server-to-player dispatch #%ld end: thread=%lu message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X size=%u %s result=0x%llX/%lld before=[%s] after=[%s] next-after=[%s]",
        observation,
        static_cast<unsigned long>(thread_id),
        message,
        name.c_str(),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        static_cast<unsigned int>(minor),
        size,
        live_object_opcode_text.c_str(),
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, after).c_str());
  }

  if ((is_loadscreen_packet || is_area_packet || is_login_or_legacy_load_packet) && observation > 260) {
    LogFormat(
        L"server-to-player flow marker #%ld: thread=%lu message=%p name='%s' major=%u/0x%02X minor=%u/0x%02X size=%u result=0x%llX/%lld elapsed_ms=%llu before=[%s] after=[%s] next-after=[%s]",
        observation,
        static_cast<unsigned long>(thread_id),
        message,
        name.c_str(),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(major),
        static_cast<unsigned int>(minor),
        static_cast<unsigned int>(minor),
        size,
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        static_cast<unsigned long long>(GetTickCount64() - dispatch_start_tick),
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, after).c_str());
  }

  if (g_auto_connect_ip_known && buffer != nullptr && size >= 3 && buffer[0] == 'P') {
    if (major == 17 && minor == 2) {
      TryAutoRequestCharacterFromList(message, observation);
    } else if (major == 17 && minor == 4) {
      TryAutoPlayCharacterAfterUpdate(message, buffer, size, observation);
    }
  }

  if (g_auto_connect_ip_known && buffer != nullptr && size >= 3 && buffer[0] == 'P' && major == 4 && minor == 1 &&
      result == 0 && g_client_to_server_area_loaded_observations == client_area_loaded_before_dispatch &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_SYNTHETIC_AREA_LOADED")) {
    const LONG synthetic_observation = InterlockedIncrement(&g_synthetic_area_loaded_observations);
    LogFormat(
        L"synthetic Area_AreaLoaded #%ld begin: server_dispatch=%ld message=%p area_dispatches=%ld client_area_loaded_before=%ld after_area_state=[%s] reason=Area_ClientArea",
        synthetic_observation,
        observation,
        message,
        g_area_load_dispatch_observations,
        client_area_loaded_before_dispatch,
        FormatCnwMessageReadState(after).c_str());
    InterlockedIncrement(&g_synthetic_area_loaded_send_depth);
    const int64_t synthetic_result = HookedClientSendPlayerToServerMessage(message, 4, 3, nullptr, 0);
    InterlockedDecrement(&g_synthetic_area_loaded_send_depth);
    LogFormat(
        L"synthetic Area_AreaLoaded #%ld end: server_dispatch=%ld message=%p result=0x%llX/%lld client_area_loaded_after=%ld",
        synthetic_observation,
        observation,
        message,
        static_cast<unsigned long long>(synthetic_result),
        static_cast<long long>(synthetic_result),
        g_client_to_server_area_loaded_observations);
  }

  if (g_auto_connect_ip_known && buffer != nullptr && size >= 3 && buffer[0] == 'P' && major == 4 && minor == 1 &&
      result == 0) {
    TryDispatchSyntheticEeLoadBarEnd(message, observation, after, L"Area_ClientArea completed");
  }

  TryDispatchPendingAutoUseObject(message, observation);

  if (has_password_gate_prompt) {
    InterlockedExchange(&g_hg_password_auto_speak_attempts, 0);
    CancelDelayedHgPasswordRetry(L"new prompt", observation);
    TryAutoSpeakHgPassword(message, observation);
  } else if (has_hg_welcome_text) {
    CancelDelayedHgPasswordRetry(L"HG welcome text", observation);
  } else if (has_hg_password_incorrect) {
    ScheduleDelayedHgPasswordRetry(message, observation);
  } else {
    TryPendingDelayedHgPasswordRetry(observation);
  }
  return result;
}

int64_t __fastcall HookedLiveDoorAdd(void* message) {
  if (TrySkipLegacyLiveShortAddRecord(message, 10, L"door")) {
    ClearPendingLegacyLiveShortAddParse();
    return 1;
  }
  const int64_t result = g_live_door_add_original != nullptr ? g_live_door_add_original(message) : 0;
  ClearPendingLegacyLiveShortAddParse();
  return result;
}

int64_t __fastcall HookedLiveTriggerAdd(void* message) {
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_live_trigger_add_observations);
  if (observation <= 240 || RuntimeResourceDiagnosticScopeActive()) {
    uint32_t object_id = 0;
    std::vector<uint8_t> bytes;
    if (before.readable &&
        before.read_buffer_ptr <= before.read_buffer_size &&
        before.read_buffer_size - before.read_buffer_ptr >= 4 &&
        PeekReadBufferBytes(message, before, before.read_buffer_ptr, 4, &bytes) &&
        bytes.size() >= 4) {
      TryReadU32LeFromBytes(bytes, 0, &object_id);
    }
    LogFormat(
        L"legacy live trigger add begin #%ld: message=%p object=0x%08X/%u before=[%s] next=[%s]%s",
        observation,
        message,
        object_id,
        object_id,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageBufferPreview(message, before, 96).c_str(),
        FormatLiveDispatchContext().c_str());
  }

  const bool queued_legacy_trigger_parse = TryQueueLegacyLiveTriggerAddParse(message);
  if (!queued_legacy_trigger_parse && TrySkipLegacyLiveTriggerAddRecord(message)) {
    ClearPendingLegacyLiveTriggerAddParse();
    ClearPendingLegacyLiveShortAddParse();
    ClearPendingLegacyLiveBadInlineLocStringSkip();
    return 1;
  }

  const int64_t result = g_live_trigger_add_original != nullptr ? g_live_trigger_add_original(message) : 0;
  ClearPendingLegacyLiveTriggerAddParse();
  ClearPendingLegacyLiveShortAddParse();

  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  if (observation <= 240 ||
      result == 0 ||
      CnwMessageStateOverflow(after) ||
      CnwMessageStateUnderflow(after) ||
      RuntimeResourceDiagnosticScopeActive()) {
    LogFormat(
        L"legacy live trigger add end #%ld: message=%p result=0x%llX/%lld before=[%s] after=[%s] next=[%s]%s",
        observation,
        message,
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, after, 96).c_str(),
        FormatLiveDispatchContext().c_str());
  }
  ClearPendingLegacyLiveTriggerAddParse();
  return result;
}

int64_t __fastcall HookedLivePlaceableAdd(void* message) {
  ClearPendingLegacyLivePlaceableAddDiagnostics();
  if (TrySkipLegacyLiveShortAddRecord(message, 9, L"placeable")) {
    ClearPendingLegacyLiveShortAddParse();
    ClearPendingLegacyLivePlaceableAddDiagnostics();
    return 1;
  }
  const bool queued_placeable_add_diagnostics =
      TryQueueLegacyLivePlaceableAddDiagnostics(message);
  const int64_t result = g_live_placeable_add_original != nullptr ? g_live_placeable_add_original(message) : 0;
  if (queued_placeable_add_diagnostics &&
      g_pending_legacy_live_placeable_add_diagnostics.active) {
    const PendingLegacyLivePlaceableAddDiagnostics& pending =
        g_pending_legacy_live_placeable_add_diagnostics;
    const LONG observation =
        InterlockedIncrement(&g_legacy_live_placeable_add_diagnostics_observations);
    if (observation <= 320 || result == 0) {
      LogFormat(
          L"legacy live placeable add diagnostics end #%ld: object=0x%08X/%u result=0x%llX/%lld no-SetUseable-match message=%p map=[%s]%s",
          observation,
          pending.object_id,
          pending.object_id,
          static_cast<unsigned long long>(result),
          static_cast<long long>(result),
          message,
          FormatPendingLegacyLivePlaceableAddBoolMap(pending).c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  ClearPendingLegacyLiveShortAddParse();
  ClearPendingLegacyLivePlaceableAddDiagnostics();
  return result;
}

bool ShouldForcePlaceableUseableForTesting(const std::string& name, int original_useable) {
  if (original_useable != 0) {
    return false;
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_FORCE_ALL_PLACEABLES_USEABLE")) {
    return true;
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_TRANSITION_PLACEABLE_USEABLE_FORCE")) {
    return false;
  }

  static constexpr const char* kTransitionNameHints[] = {
      "ascension",
      "bank",
      "branch",
      "crow",
      "door",
      "inn",
      "lathander",
      "lighthouse",
      "moon",
      "portal",
      "sooty",
      "temple",
      "transition",
      "tyr",
      "tavern",
      "waukeen",
      "west",
  };
  for (const char* hint : kTransitionNameHints) {
    if (ContainsAsciiNoCase(name, hint)) {
      return true;
    }
  }
  return false;
}

void __fastcall HookedPlaceableSetUseable(void* placeable, int useable) {
  CExoStringView name_view{};
  const bool name_ok = ReadCExoStringField(placeable, 0x170, &name_view);
  const std::string name_text = name_ok ? CExoStringToString(&name_view, 192) : std::string{};
  const std::wstring name_wide = name_ok ? CExoStringToWide(&name_view) : L"<unreadable>";
  const uint32_t object_id = ReadUInt32Field(placeable, 0x08, 0x7F000000);
  RememberLiveObjectPointerForDiagnostics(object_id, 9, placeable);
  void* interaction_object = nullptr;
  const bool interaction_object_ok = TryGetPlaceableInteractionObject(placeable, &interaction_object);
  const uint32_t transition_target = interaction_object != nullptr
      ? ReadUInt32Field(interaction_object, 0x1B8, 0x7F000000)
      : ReadUInt32Field(placeable, 0x1B8, 0x7F000000);
  const uint32_t current_stored_useable = ReadUInt32Field(placeable, 0x11C, 0xFFFFFFFFu);
  const uint32_t parsed_flag_194 = ReadUInt32Field(placeable, 0x194, 0xFFFFFFFFu);
  const uint32_t parsed_flag_198 = ReadUInt32Field(placeable, 0x198, 0xFFFFFFFFu);
  const uint32_t parsed_flag_19c = ReadUInt32Field(placeable, 0x19C, 0xFFFFFFFFu);
  const uint32_t parsed_flag_1a4 = ReadUInt32Field(placeable, 0x1A4, 0xFFFFFFFFu);
  const uint32_t parsed_flag_1a8 = ReadUInt32Field(placeable, 0x1A8, 0xFFFFFFFFu);
  const uint32_t parsed_flag_1ac = ReadUInt32Field(placeable, 0x1AC, 0xFFFFFFFFu);
  const uint32_t parsed_flag_1b0 = ReadUInt32Field(placeable, 0x1B0, 0xFFFFFFFFu);
  const uint32_t parsed_flag_1bc = ReadUInt32Field(placeable, 0x1BC, 0xFFFFFFFFu);
  float position_x = 0.0f;
  float position_y = 0.0f;
  float position_z = 0.0f;
  const bool position_ok =
      TryReadGobPosition(interaction_object != nullptr ? interaction_object : placeable, &position_x, &position_y, &position_z);
  const bool force_useable = ShouldForcePlaceableUseableForTesting(name_text, useable);
  PendingLegacyLivePlaceableAddDiagnostics& pending_add =
      g_pending_legacy_live_placeable_add_diagnostics;
  const bool pending_add_matches =
      pending_add.active &&
      pending_add.dispatch == g_current_server_to_player_message_observation &&
      pending_add.object_id == object_id;
  const int legacy_add_useable =
      pending_add_matches && pending_add.bool_seen[3] ? pending_add.bool_values[3] : -1;
  const bool legacy_add_useable_remap =
      !force_useable &&
      pending_add_matches &&
      legacy_add_useable >= 0 &&
      useable != legacy_add_useable &&
      !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_PLACEABLE_ADD_USEABLE_REMAP");
  const int effective_useable = force_useable
      ? 1
      : (legacy_add_useable_remap ? legacy_add_useable : useable);
  if (legacy_add_useable_remap) {
    const LONG remap_observation =
        InterlockedIncrement(&g_legacy_live_placeable_add_useable_remap_observations);
    if (remap_observation <= 240) {
      LogFormat(
          L"legacy live placeable add useable remap #%ld: object=0x%08X/%u original=%d remapped=%d legacy-useable=%d name='%s' bool-map=[%s]%s",
          remap_observation,
          object_id,
          object_id,
          useable,
          effective_useable,
          legacy_add_useable,
          name_wide.c_str(),
          FormatPendingLegacyLivePlaceableAddBoolMap(pending_add).c_str(),
          FormatLiveDispatchContext().c_str());
    }
  }
  RememberObjectNameForDiagnostics(
      object_id,
      9,
      name_text,
      effective_useable,
      transition_target,
      position_ok,
      position_x,
      position_y,
      position_z);
  ScheduleAutoUseObjectForTesting(object_id, 9, name_text, effective_useable);

  const LONG observation = InterlockedIncrement(&g_placeable_set_useable_observations);
  const bool interesting_name =
      ContainsAsciiNoCase(name_text, "sooty") ||
      ContainsAsciiNoCase(name_text, "inn") ||
      ContainsAsciiNoCase(name_text, "temple") ||
      ContainsAsciiNoCase(name_text, "bank") ||
      ContainsAsciiNoCase(name_text, "ascension") ||
      ContainsAsciiNoCase(name_text, "chest") ||
      ContainsAsciiNoCase(name_text, "container") ||
      ContainsAsciiNoCase(name_text, "storage") ||
      ContainsAsciiNoCase(name_text, "waystone") ||
      ContainsAsciiNoCase(name_text, "pedestal");
  if (observation <= 260 || interesting_name || force_useable || RuntimeResourceDiagnosticScopeActive()) {
    LogFormat(
        L"placeable SetUseable observed #%ld: placeable=%p interaction=%p interaction-ok=%d object_id=0x%08X original=%d effective=%d forced=%d legacy-remapped=%d stored-before=0x%08X transition-target=0x%08X position=%s registry=%s flags[194=%u 198=%u 19C=%u 1A4=%u 1A8=%u 1AC=%u 1B0=%u 1BC=%u] name-ok=%d name-len=%u name='%s'%s",
        observation,
        placeable,
        interaction_object,
        interaction_object_ok ? 1 : 0,
        object_id,
        useable,
        effective_useable,
        force_useable ? 1 : 0,
        legacy_add_useable_remap ? 1 : 0,
        current_stored_useable,
        transition_target,
        position_ok
            ? FormatObjectRegistrySpatialSuffix(ObjectNameRegistryEntry{
                  object_id,
                  9,
                  effective_useable,
                  transition_target,
                  true,
                  position_x,
                  position_y,
                  position_z,
                  false,
                  0.0f,
                  0.0f,
                  0.0f,
                  0.0f,
                  0.0f,
                  0.0f,
                  std::string{},
                  0}).c_str()
            : L"<unreadable>",
        FormatObjectNameRegistryLookup(object_id).c_str(),
        parsed_flag_194,
        parsed_flag_198,
        parsed_flag_19c,
        parsed_flag_1a4,
        parsed_flag_1a8,
        parsed_flag_1ac,
        parsed_flag_1b0,
        parsed_flag_1bc,
        name_ok ? 1 : 0,
        name_ok ? SafeCExoStringLength(&name_view) : 0,
        name_wide.c_str(),
        FormatLiveDispatchContext().c_str());
  }

  if (pending_add_matches) {
    const LONG add_observation =
        InterlockedIncrement(&g_legacy_live_placeable_add_diagnostics_observations);
    if (add_observation <= 320 || force_useable || useable != legacy_add_useable) {
      LogFormat(
          L"legacy live placeable add diagnostics final #%ld: object=0x%08X/%u original-useable=%d effective-useable=%d forced=%d legacy-remapped=%d name='%s' raw-tail[appearance-type=0x%02X/%u appearance-id=0x%04X/%u bodybag=0x%04X/%u] bool-map=[%s] useable-candidate[legacy-useable=%d] record-start=%u boundary=%u registry=%s%s",
          add_observation,
          object_id,
          object_id,
          useable,
          effective_useable,
          force_useable ? 1 : 0,
          legacy_add_useable_remap ? 1 : 0,
          name_wide.c_str(),
          static_cast<unsigned int>(pending_add.appearance_type),
          static_cast<unsigned int>(pending_add.appearance_type),
          static_cast<unsigned int>(pending_add.appearance_id),
          static_cast<unsigned int>(pending_add.appearance_id),
          static_cast<unsigned int>(pending_add.bodybag),
          static_cast<unsigned int>(pending_add.bodybag),
          FormatPendingLegacyLivePlaceableAddBoolMap(pending_add).c_str(),
          legacy_add_useable,
          pending_add.record_start_buffer_ptr,
          pending_add.boundary_buffer_ptr,
          FormatObjectNameRegistryLookup(object_id).c_str(),
          FormatLiveDispatchContext().c_str());
    }
    ClearPendingLegacyLivePlaceableAddDiagnostics();
  }

  if (g_placeable_set_useable_original != nullptr) {
    g_placeable_set_useable_original(placeable, effective_useable);
  } else {
    WriteUInt32AtOffset(placeable, 0x11C, static_cast<uint32_t>(effective_useable));
  }
}

int64_t __fastcall HookedLiveGameObjectUpdate(void* message, unsigned char minor) {
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_live_game_object_update_observations);
  const std::wstring dispatch_context = FormatLiveDispatchContext();
  ++g_live_game_object_update_depth;

  if (observation <= 180) {
    LogFormat(
        L"live HandleServerToPlayerGameObjectUpdate #%ld begin: message=%p minor=%u live_depth=%d state=[%s] pointers=[%s] next=[%s]%s",
        observation,
        message,
        static_cast<unsigned int>(minor),
        g_live_game_object_update_depth,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadPointers(message).c_str(),
        FormatCnwMessageBufferPreview(message, before).c_str(),
        dispatch_context.c_str());
  }

  const int64_t result = g_live_game_object_update_original != nullptr
      ? g_live_game_object_update_original(message, minor)
      : 0;

  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  if (observation <= 180 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after) || result == 0) {
    LogFormat(
        L"live HandleServerToPlayerGameObjectUpdate #%ld end: message=%p minor=%u result=0x%llX/%lld live_depth=%d before=[%s] after=[%s] next-after=[%s]%s",
        observation,
        message,
        static_cast<unsigned int>(minor),
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        g_live_game_object_update_depth,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, after).c_str(),
        dispatch_context.c_str());
  }

  --g_live_game_object_update_depth;
  return result;
}

int64_t __fastcall HookedGameObjectUpdate(void* message, unsigned char minor) {
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  ++g_game_object_update_depth;

  const LONG observation = InterlockedIncrement(&g_live_message_update_observations);
  if (observation <= 120) {
    LogFormat(
        L"CNWMessage player-list/object update #%ld begin: message=%p minor=%u depth=%d state=[%s] pointers=[%s] next=[%s]",
        observation,
        message,
        static_cast<unsigned int>(minor),
        g_game_object_update_depth,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadPointers(message).c_str(),
        FormatCnwMessageBufferPreview(message, before).c_str());
  }

  const int64_t result = g_game_object_update_original != nullptr
      ? g_game_object_update_original(message, minor)
      : 0;

  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  if (observation <= 120 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    LogFormat(
        L"CNWMessage player-list/object update #%ld end: message=%p minor=%u result=0x%llX/%lld depth=%d before=[%s] after=[%s] next-after=[%s]",
        observation,
        message,
        static_cast<unsigned int>(minor),
        static_cast<unsigned long long>(result),
        static_cast<long long>(result),
        g_game_object_update_depth,
        FormatCnwMessageReadState(before).c_str(),
        FormatCnwMessageReadState(after).c_str(),
        FormatCnwMessageBufferPreview(message, after).c_str());
  }

  --g_game_object_update_depth;
  return result;
}

uint32_t __fastcall HookedRawReadDword(void* message) {
  void* const return_address = _ReturnAddress();
  const bool trace_live = ShouldTraceLiveMessageRead(return_address);
  const CnwMessageReadState before = ReadCnwMessageReadState(message);
  std::wstring boundary_guard_detail;
  if (ShouldSuppressLegacyLiveParserBoundaryRead(
          message,
          return_address,
          before,
          L"ReadRawDWORD",
          &boundary_guard_detail)) {
    if (trace_live) {
      wchar_t detail[320]{};
      swprintf_s(
          detail,
          L"result=0x00000000/0 raw-helper %s",
          boundary_guard_detail.c_str());
      LogLiveMessageRead(
          L"ReadRawDWORD",
          return_address,
          message,
          before,
          before,
          detail);
    }
    return 0;
  }
  const bool inventory_equip_context =
      g_auto_connect_ip_known &&
      g_server_to_player_message_depth > 0 &&
      g_current_server_to_player_major == 12 &&
      (g_current_server_to_player_minor == 1 || g_current_server_to_player_minor == 2);
  if (inventory_equip_context) {
    const LONG probe_observation = InterlockedIncrement(&g_legacy_inventory_equip_raw_read_probe_observations);
    if (probe_observation <= 40 || CnwMessageStateOverflow(before) || CnwMessageStateUnderflow(before)) {
      LogFormat(
          L"legacy inventory equip raw DWORD probe #%ld: caller=%s dispatch=%ld major=%u/0x%02X minor=%u/0x%02X message=%p before=[%s] next=[%s]",
          probe_observation,
          FormatMainExecutableAddress(return_address).c_str(),
          g_current_server_to_player_message_observation,
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_major),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          static_cast<unsigned int>(g_current_server_to_player_minor),
          message,
          FormatCnwMessageReadState(before).c_str(),
          FormatCnwMessageBufferPreview(message, before, 48).c_str());
    }
  }
  uint32_t legacy_inventory_object_id = 0;
  if (TryRemapLegacyInventoryEquipInitialObjectId(
          message,
          return_address,
          before,
          &legacy_inventory_object_id)) {
    return legacy_inventory_object_id;
  }

  if (!trace_live) {
    return g_raw_read_dword_original != nullptr ? g_raw_read_dword_original(message) : 0;
  }

  const uint32_t result = g_raw_read_dword_original != nullptr ? g_raw_read_dword_original(message) : 0;
  const CnwMessageReadState after = ReadCnwMessageReadState(message);
  const LONG observation = InterlockedIncrement(&g_live_message_raw_read_observations);
  if (observation <= 220 || CnwMessageStateOverflow(after) || CnwMessageStateUnderflow(after)) {
    wchar_t detail[128]{};
    swprintf_s(detail, L"result=0x%08X/%u raw-helper", result, result);
    LogLiveMessageRead(L"ReadRawDWORD", return_address, message, before, after, detail);
  }
  return result;
}

void LogResManLookup(
    const wchar_t* operation,
    void* return_address,
    void* resman,
    const CResRefView* resref,
    uint16_t type,
    const wchar_t* result_text,
    const wchar_t* extra_text,
    bool missing,
    bool definitive_result) {
  if (operation == nullptr || resref == nullptr || result_text == nullptr) {
    return;
  }

  const bool active = ModuleResourceDiagnosticScopeActive();
  const bool runtime_important =
      RuntimeResourceDiagnosticScopeActive() && IsRuntimeInterestingResourceLookup(resref, type);
  const bool runtime_missing =
      definitive_result && missing && runtime_important;
  const bool runtime_hit =
      definitive_result && !missing && runtime_important;
  const LONG observation = InterlockedIncrement(&g_resman_lookup_observations);
  const LONG runtime_miss_observation = runtime_missing
      ? InterlockedIncrement(&g_resman_runtime_miss_observations)
      : 0;
  const LONG runtime_hit_observation = runtime_hit
      ? InterlockedIncrement(&g_resman_runtime_hit_observations)
      : 0;
  if (active || observation <= 40 ||
      (runtime_missing && runtime_miss_observation <= 2000) ||
      (runtime_hit && runtime_hit_observation <= 1200)) {
    LogFormat(
        L"CExoResMan lookup #%ld %s: caller=%s resman=%p resref=%p name='%s' raw=[%s] type=%s result=%s%s%s%s%s%s%s load_depth=%d resource_depth=%d",
        observation,
        operation,
        return_address != nullptr ? FormatMainExecutableAddress(return_address).c_str() : L"<unknown>",
        resman,
        resref,
        CResRefToWide(resref).c_str(),
        CResRefRawBytes(resref).c_str(),
        FormatResType(type).c_str(),
        result_text,
        extra_text != nullptr && extra_text[0] != L'\0' ? L" " : L"",
        extra_text != nullptr ? extra_text : L"",
        runtime_missing ? L" runtime-miss#" : L"",
        runtime_missing ? std::to_wstring(runtime_miss_observation).c_str() : L"",
        runtime_hit ? L" runtime-hit#" : L"",
        runtime_hit ? std::to_wstring(runtime_hit_observation).c_str() : L"",
        g_client_module_load_depth,
        g_client_module_resource_load_depth);
  }
}

int __fastcall HookedResManExists(void* resman, const CResRefView* resref, uint16_t type, uint32_t* size_out) {
  void* const return_address = _ReturnAddress();
  if (g_resman_hook_depth > 0) {
    return g_resman_exists_original != nullptr ? g_resman_exists_original(resman, resref, type, size_out) : 0;
  }

  ++g_resman_hook_depth;
  const int result = g_resman_exists_original != nullptr ? g_resman_exists_original(resman, resref, type, size_out) : 0;
  uint32_t size_value = 0;
  const bool size_ok = size_out != nullptr && SafeReadUInt32(size_out, &size_value);
  wchar_t result_text[64]{};
  swprintf_s(result_text, L"%d", result);
  wchar_t extra_text[96]{};
  swprintf_s(
      extra_text,
      L"size_out=%p%s%u",
      size_out,
      size_ok ? L" value=" : L"",
      size_ok ? size_value : 0);
  LogResManLookup(
      L"Exists",
      return_address,
      resman,
      resref,
      type,
      result_text,
      extra_text,
      result == 0,
      true);
  --g_resman_hook_depth;
  return result;
}

void* __fastcall HookedResManGetResObject(void* resman, const CResRefView* resref, uint16_t type) {
  void* const return_address = _ReturnAddress();
  if (g_resman_hook_depth > 0) {
    return g_resman_get_res_object_original != nullptr
        ? g_resman_get_res_object_original(resman, resref, type)
        : nullptr;
  }

  ++g_resman_hook_depth;
  void* const result = g_resman_get_res_object_original != nullptr
      ? g_resman_get_res_object_original(resman, resref, type)
      : nullptr;
  wchar_t result_text[64]{};
  swprintf_s(result_text, L"%p", result);
  if (result != nullptr) {
    RememberTrackedResObject(result, resref, type);
  }
  LogResManLookup(
      L"GetResObject",
      return_address,
      resman,
      resref,
      type,
      result_text,
      result == nullptr ? L"cache-miss" : L"",
      result == nullptr,
      result != nullptr);
  --g_resman_hook_depth;
  return result;
}

void __fastcall HookedResManSetResObject(void* resman, const CResRefView* resref, uint16_t type, void* resource) {
  void* const return_address = _ReturnAddress();
  if (resource == nullptr) {
    return;
  }

  if (g_resman_hook_depth > 0) {
    if (g_resman_set_res_object_original != nullptr) {
      g_resman_set_res_object_original(resman, resref, type, resource);
    }
    return;
  }

  ++g_resman_hook_depth;
  RememberTrackedResObject(resource, resref, type);
  if (g_resman_set_res_object_original != nullptr) {
    g_resman_set_res_object_original(resman, resref, type, resource);
  }

  wchar_t result_text[64]{};
  swprintf_s(result_text, L"%p", resource);
  LogResManLookup(
      L"SetResObject",
      return_address,
      resman,
      resref,
      type,
      result_text,
      L"tracked",
      false,
      true);
  --g_resman_hook_depth;
}

void* __fastcall HookedResManDemand(void* resman, void* resource) {
  if (g_resman_demand_hook_depth > 0) {
    return g_resman_demand_original != nullptr ? g_resman_demand_original(resman, resource) : nullptr;
  }

  ++g_resman_demand_hook_depth;
  void* const result = g_resman_demand_original != nullptr
      ? g_resman_demand_original(resman, resource)
      : nullptr;
  CResRefView resref{};
  uint16_t type = 0;
  const bool known = LookupTrackedResObject(resource, &resref, &type);
  const bool active = RuntimeResourceDiagnosticScopeActive();
  const bool important = known && IsRuntimeImportantResourceType(type);
  const bool runtime_important = active && important;
  const bool missing_important = runtime_important && result == nullptr;
  const bool hit_important = runtime_important && result != nullptr;
  const std::string lower_name = known ? LowerAscii(CResRefToAscii(&resref)) : std::string();
  const bool targeted_name = known && IsTargetedHgRuntimeResourceName(lower_name);
  const bool optional_missing = missing_important && IsOptionalRuntimeDemandMissType(type);
  const bool non_optional_missing = missing_important && !optional_missing;
  const bool targeted_demand = runtime_important && targeted_name;
  const LONG observation = InterlockedIncrement(&g_resman_demand_observations);
  const LONG runtime_observation = missing_important
      ? InterlockedIncrement(&g_resman_runtime_demand_observations)
      : 0;
  const LONG non_optional_miss_observation = non_optional_missing
      ? InterlockedIncrement(&g_resman_runtime_non_optional_demand_miss_observations)
      : 0;
  const LONG runtime_hit_observation = hit_important
      ? InterlockedIncrement(&g_resman_runtime_hit_observations)
      : 0;
  const LONG targeted_demand_observation = targeted_demand
      ? InterlockedIncrement(&g_resman_runtime_targeted_demand_observations)
      : 0;

  if ((optional_missing && runtime_observation <= 120) ||
      (non_optional_missing && non_optional_miss_observation <= 2000) ||
      (hit_important && runtime_hit_observation <= 1200) ||
      (targeted_demand && targeted_demand_observation <= 1500) ||
      (ModuleResourceDiagnosticScopeActive() && known)) {
    LogFormat(
        L"CExoResMan demand #%ld: resman=%p resource=%p tracked=%d name='%s' raw=[%s] type=%s result=%p%s%s%s%s%s%s%s%s load_depth=%d resource_depth=%d",
        observation,
        resman,
        resource,
        known ? 1 : 0,
        known ? CResRefToWide(&resref).c_str() : L"<unknown>",
        known ? CResRefRawBytes(&resref).c_str() : L"",
        known ? FormatResType(type).c_str() : L"<unknown>",
        result,
        missing_important ? L" runtime-demand-miss#" : L"",
        missing_important ? std::to_wstring(runtime_observation).c_str() : L"",
        non_optional_missing ? L" non-optional-miss#" : L"",
        non_optional_missing ? std::to_wstring(non_optional_miss_observation).c_str() : L"",
        hit_important ? L" runtime-demand-hit#" : L"",
        hit_important ? std::to_wstring(runtime_hit_observation).c_str() : L"",
        targeted_demand ? L" targeted-demand#" : L"",
        targeted_demand ? std::to_wstring(targeted_demand_observation).c_str() : L"",
        g_client_module_load_depth,
        g_client_module_resource_load_depth);
  }

  --g_resman_demand_hook_depth;
  return result;
}

void* __fastcall HookedTileSetManagerGetTileSet(void* manager, const CResRefView* tileset) {
  void* const return_address = _ReturnAddress();
  if (g_tileset_hook_depth > 0) {
    return g_tileset_manager_get_tileset_original != nullptr
        ? g_tileset_manager_get_tileset_original(manager, tileset)
        : nullptr;
  }

  ++g_tileset_hook_depth;
  void* const result = g_tileset_manager_get_tileset_original != nullptr
      ? g_tileset_manager_get_tileset_original(manager, tileset)
      : nullptr;
  const LONG observation = InterlockedIncrement(&g_tileset_manager_get_observations);
  if (RuntimeResourceDiagnosticScopeActive() || observation <= 20) {
    const bool missing_or_empty = result == nullptr || tileset == nullptr || CResRefToWide(tileset).empty();
    const std::wstring stack_text = missing_or_empty ? FormatCurrentStackTrace(1, 8) : L"";
    LogFormat(
        L"CNWTileSetManager::GetTileSet #%ld: manager=%p tileset='%s' raw=[%s] result=%p caller=%s%s%s%s",
        observation,
        manager,
        CResRefToWide(tileset).c_str(),
        CResRefRawBytes(tileset).c_str(),
        result,
        FormatMainExecutableAddress(return_address).c_str(),
        missing_or_empty ? L" stack=[" : L"",
        stack_text.c_str(),
        missing_or_empty ? L"]" : L"");
  }
  --g_tileset_hook_depth;
  return result;
}

int __fastcall HookedNwTileSetLoadTileSet(void* tileset) {
  if (g_tileset_hook_depth > 0) {
    return g_nw_tileset_load_tileset_original != nullptr ? g_nw_tileset_load_tileset_original(tileset) : 0;
  }

  ++g_tileset_hook_depth;
  const int result = g_nw_tileset_load_tileset_original != nullptr ? g_nw_tileset_load_tileset_original(tileset) : 0;
  const LONG observation = InterlockedIncrement(&g_tileset_load_observations);
  if (RuntimeResourceDiagnosticScopeActive() || result == 0 || observation <= 20) {
    LogFormat(
        L"CNWTileSet::LoadTileSet #%ld: tileset=%p result=%d",
        observation,
        tileset,
        result);
  }
  --g_tileset_hook_depth;
  return result;
}

int __fastcall HookedNwsAreaSetTileset(void* area, const CResRefView* tileset) {
  if (g_tileset_hook_depth > 0) {
    return g_nws_area_set_tileset_original != nullptr ? g_nws_area_set_tileset_original(area, tileset) : 0;
  }

  ++g_tileset_hook_depth;
  const int result = g_nws_area_set_tileset_original != nullptr ? g_nws_area_set_tileset_original(area, tileset) : 0;
  const LONG observation = InterlockedIncrement(&g_area_set_tileset_observations);
  if (RuntimeResourceDiagnosticScopeActive() || result == 0 || observation <= 20) {
    LogFormat(
        L"CNWSArea::SetTileset #%ld: area=%p tileset='%s' raw=[%s] result=%d",
        observation,
        area,
        CResRefToWide(tileset).c_str(),
        CResRefRawBytes(tileset).c_str(),
        result);
  }
  --g_tileset_hook_depth;
  return result;
}

void LogResManAddFile(
    const wchar_t* operation,
    void* resman,
    const CExoStringView* path,
    uint32_t flags,
    int result) {
  const LONG observation = InterlockedIncrement(&g_resman_add_file_observations);
  if (observation <= 120 || ModuleResourceDiagnosticScopeActive()) {
    LogFormat(
        L"CExoResMan add #%ld %s: resman=%p path=%p text='%s' length=%u flags=0x%08X result=%d load_depth=%d resource_depth=%d",
        observation,
        operation != nullptr ? operation : L"<unknown>",
        resman,
        path,
        path != nullptr ? CExoStringToWide(path).c_str() : L"<null>",
        path != nullptr ? SafeCExoStringLength(path) : 0,
        flags,
        result,
        g_client_module_load_depth,
        g_client_module_resource_load_depth);
  }
}

const wchar_t* FormatResManKeyTableKind(uint32_t kind) {
  switch (kind) {
    case 1: return L"1/fixed";
    case 2: return L"2/directory";
    case 3: return L"3/encapsulated";
    default: return L"<unknown>";
  }
}

bool LooksLikeLegacyResourcePath(const std::wstring& path) {
  const std::wstring lowered = LowerAscii(path);
  return lowered.find(L"hghak:") != std::wstring::npos ||
      lowered.find(L"diamondhak:") != std::wstring::npos ||
      lowered.find(L"hg-bridge-assets") != std::wstring::npos ||
      lowered.find(L"\\hg-gui") != std::wstring::npos ||
      lowered.find(L"\\hg-std") != std::wstring::npos ||
      lowered.find(L"\\hg-overlay") != std::wstring::npos ||
      lowered.find(L"\\combined-override") != std::wstring::npos ||
      lowered.find(L"cep2_") != std::wstring::npos ||
      lowered.find(L"cep23") != std::wstring::npos;
}

int __fastcall HookedResManAddEncapsulatedResourceFile(void* resman, const CExoStringView* path, uint32_t flags) {
  const int result = g_resman_add_encapsulated_resource_file_original != nullptr
      ? g_resman_add_encapsulated_resource_file_original(resman, path, flags)
      : 0;
  LogResManAddFile(L"AddEncapsulatedResourceFile", resman, path, flags, result);
  return result;
}

int __fastcall HookedResManAddFixedKeyTableFile(void* resman, const CExoStringView* path, uint32_t flags) {
  const int result = g_resman_add_fixed_key_table_file_original != nullptr
      ? g_resman_add_fixed_key_table_file_original(resman, path, flags)
      : 0;
  LogResManAddFile(L"AddFixedKeyTableFile", resman, path, flags, result);
  return result;
}

int __fastcall HookedResManAddResourceDirectory(
    void* resman,
    const CExoStringView* path,
    uint32_t flags,
    int table_arg,
    void* filter) {
  const int result = g_resman_add_resource_directory_original != nullptr
      ? g_resman_add_resource_directory_original(resman, path, flags, table_arg, filter)
      : 0;

  const LONG observation = InterlockedIncrement(&g_resman_add_directory_observations);
  const std::wstring path_text = CExoStringToWide(path);
  const bool legacy_path = LooksLikeLegacyResourcePath(path_text);
  if (observation <= 160 || legacy_path || ModuleResourceDiagnosticScopeActive()) {
    LogFormat(
        L"CExoResMan add-directory #%ld: caller=%s resman=%p path=%p text='%s' length=%u flags=0x%08X arg=%d filter=%p result=%d legacy_path=%d load_depth=%d resource_depth=%d",
        observation,
        FormatMainExecutableAddress(_ReturnAddress()).c_str(),
        resman,
        path,
        path_text.c_str(),
        SafeCExoStringLength(path),
        flags,
        table_arg,
        filter,
        result,
        legacy_path ? 1 : 0,
        g_client_module_load_depth,
        g_client_module_resource_load_depth);
  }
  return result;
}

int __fastcall HookedResManRemoveKeyTable(
    void* resman,
    const CExoStringView* path,
    uint32_t key_table_type,
    int remove_arg) {
  const void* const return_address = _ReturnAddress();
  const std::wstring path_text = CExoStringToWide(path);
  const uint32_t path_length = SafeCExoStringLength(path);
  const bool legacy_path = LooksLikeLegacyResourcePath(path_text);

  const int result = g_resman_remove_key_table_original != nullptr
      ? g_resman_remove_key_table_original(resman, path, key_table_type, remove_arg)
      : 0;

  const LONG observation = InterlockedIncrement(&g_resman_remove_key_table_observations);
  const bool interesting = legacy_path || ModuleResourceDiagnosticScopeActive() || RuntimeResourceDiagnosticScopeActive();
  if (observation <= 220 || interesting || result != 0) {
    const bool include_stack = observation <= 80 || interesting || result != 0;
    const std::wstring stack_text = include_stack ? FormatCurrentStackTrace(1, 8) : L"";
    LogFormat(
        L"CExoResMan remove-keytable #%ld: caller=%s resman=%p path=%p text='%s' length=%u kind=%s raw_kind=%u arg=%d result=%d legacy_path=%d load_depth=%d resource_depth=%d%s%s%s",
        observation,
        FormatMainExecutableAddress(const_cast<void*>(return_address)).c_str(),
        resman,
        path,
        path_text.c_str(),
        path_length,
        FormatResManKeyTableKind(key_table_type),
        key_table_type,
        remove_arg,
        result,
        legacy_path ? 1 : 0,
        g_client_module_load_depth,
        g_client_module_resource_load_depth,
        include_stack ? L" stack=[" : L"",
        stack_text.c_str(),
        include_stack ? L"]" : L"");
  }
  return result;
}

void __fastcall HookedResManClearOverrides(void* resman) {
  const void* const return_address = _ReturnAddress();
  const LONG observation = InterlockedIncrement(&g_resman_clear_overrides_observations);
  const bool include_stack = observation <= 40 || ModuleResourceDiagnosticScopeActive() || RuntimeResourceDiagnosticScopeActive();
  const std::wstring before_stack = include_stack ? FormatCurrentStackTrace(1, 8) : L"";
  LogFormat(
      L"CExoResMan clear-overrides #%ld begin: caller=%s resman=%p load_depth=%d resource_depth=%d%s%s%s",
      observation,
      FormatMainExecutableAddress(const_cast<void*>(return_address)).c_str(),
      resman,
      g_client_module_load_depth,
      g_client_module_resource_load_depth,
      include_stack ? L" stack=[" : L"",
      before_stack.c_str(),
      include_stack ? L"]" : L"");

  if (g_resman_clear_overrides_original != nullptr) {
    g_resman_clear_overrides_original(resman);
  }

  LogFormat(
      L"CExoResMan clear-overrides #%ld end: resman=%p load_depth=%d resource_depth=%d",
      observation,
      resman,
      g_client_module_load_depth,
      g_client_module_resource_load_depth);
}

uint32_t __fastcall HookedClientModuleLoad(void* module) {
  const LONG observation = InterlockedIncrement(&g_client_module_load_observations);
  if (observation <= 10) {
    LogFormat(L"CNWCModule::LoadModule #%ld begin: module=%p", observation, module);
  }

  ++g_client_module_load_depth;
  const uint32_t result = g_client_module_load_original != nullptr
      ? g_client_module_load_original(module)
      : 0xFFFFFFFFu;
  --g_client_module_load_depth;

  if (result == 0 && !IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY")) {
    RemountLegacyModuleResourcesAfterLoad(L"CNWCModule::LoadModule completion");
  }

  if (observation <= 10 || result != 0) {
    LogFormat(L"CNWCModule::LoadModule #%ld result=0x%08X", observation, result);
    LogClientModuleSnapshot(L"CNWCModule::LoadModule snapshot", module, result);
  }
  return result;
}

uint32_t __fastcall HookedClientModuleLoadResources(void* module) {
  const LONG observation = InterlockedIncrement(&g_client_module_load_resources_observations);
  if (observation <= 10) {
    LogFormat(L"CNWCModule::LoadModuleResources #%ld begin: module=%p", observation, module);
  }

  ++g_client_module_resource_load_depth;
  const uint32_t result = g_client_module_load_resources_original != nullptr
      ? g_client_module_load_resources_original(module)
      : 0xFFFFFFFFu;
  --g_client_module_resource_load_depth;

  if (result == 0 && IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY")) {
    ScheduleAutoCharacterInitialRequestFromLastContext(L"CNWCModule::LoadModuleResources completed");
  }

  if (observation <= 10 || result != 0) {
    LogFormat(L"CNWCModule::LoadModuleResources #%ld result=0x%08X", observation, result);
    LogClientModuleSnapshot(L"CNWCModule::LoadModuleResources snapshot", module, result);
  }
  return result;
}

std::wstring FormatClientAreaSnapshot(void* area) {
  if (area == nullptr) {
    return L"area=<null>";
  }

  CResRefView resref{};
  const bool resref_ok = TryCopyBytes(
      reinterpret_cast<const char*>(static_cast<uint8_t*>(area) + 0x128),
      sizeof(resref),
      reinterpret_cast<char*>(&resref));
  CExoStringView name{};
  const bool name_ok = ReadCExoStringField(area, 0x238, &name);
  const uint32_t field_118 = ReadUInt32Field(area, 0x118, 0xFFFFFFFFu);
  const uint32_t field_310 = ReadUInt32Field(area, 0x310, 0xFFFFFFFFu);
  const uint32_t field_320 = ReadUInt32Field(area, 0x320, 0xFFFFFFFFu);
  void* const field_248 = ReadPointerField(area, 0x248);
  void* const field_290 = ReadPointerField(area, 0x290);
  void* const field_298 = ReadPointerField(area, 0x298);
  void* const field_318 = ReadPointerField(area, 0x318);

  wchar_t buffer[1024]{};
  swprintf_s(
      buffer,
      L"area=%p resref='%s' resref-raw=[%s] name='%s' field_118=0x%08X field_248=%p field_290=%p field_298=%p field_310=0x%08X field_318=%p field_320=%u/0x%08X",
      area,
      resref_ok ? CResRefToWide(&resref).c_str() : L"<unreadable>",
      resref_ok ? CResRefRawBytes(&resref).c_str() : L"<unreadable>",
      name_ok ? CExoStringToWide(&name).c_str() : L"<unreadable>",
      field_118,
      field_248,
      field_290,
      field_298,
      field_310,
      field_318,
      field_320,
      field_320);
  return buffer;
}

uint32_t __fastcall HookedClientAreaLoad(void* area, void* camera, void* loading_panel) {
  const LONG observation = InterlockedIncrement(&g_client_area_load_observations);
  const DWORD thread_id = GetCurrentThreadId();
  const ULONGLONG start_tick = GetTickCount64();
  const bool include_stack = observation <= 4;
  const std::wstring stack = include_stack ? FormatCurrentStackTrace(1, 8) : L"";

  LogFormat(
      L"CNWCArea::LoadArea #%ld begin: thread=%lu area=%p camera=%p loading-panel=%p current_dispatch=%ld major=%u/0x%02X minor=%u/0x%02X size=%u area_dispatches=%ld client_area_loaded=%ld synthetic_area_loaded=%ld snapshot=[%s]%s%s%s",
      observation,
      static_cast<unsigned long>(thread_id),
      area,
      camera,
      loading_panel,
      g_current_server_to_player_message_observation,
      static_cast<unsigned int>(g_current_server_to_player_major),
      static_cast<unsigned int>(g_current_server_to_player_major),
      static_cast<unsigned int>(g_current_server_to_player_minor),
      static_cast<unsigned int>(g_current_server_to_player_minor),
      g_current_server_to_player_size,
      g_area_load_dispatch_observations,
      g_client_to_server_area_loaded_observations,
      g_synthetic_area_loaded_observations,
      FormatClientAreaSnapshot(area).c_str(),
      include_stack ? L" stack=[" : L"",
      stack.c_str(),
      include_stack ? L"]" : L"");

  const uint32_t result = g_client_area_load_original != nullptr
      ? g_client_area_load_original(area, camera, loading_panel)
      : 0;

  const uint32_t current_area_object_id = ReadUInt32Field(area, 0x118, 0x7F000000u);
  if (IsValidObjectIdForDiagnostics(current_area_object_id)) {
    g_current_area_object_id = current_area_object_id;
  }

  const ULONGLONG elapsed_ms = GetTickCount64() - start_tick;
  LogFormat(
      L"CNWCArea::LoadArea #%ld end: thread=%lu result=0x%08X/%u elapsed_ms=%llu area=%p camera=%p loading-panel=%p current_dispatch=%ld area_dispatches=%ld client_area_loaded=%ld synthetic_area_loaded=%ld snapshot=[%s]",
      observation,
      static_cast<unsigned long>(thread_id),
      result,
      result,
      static_cast<unsigned long long>(elapsed_ms),
      area,
      camera,
      loading_panel,
      g_current_server_to_player_message_observation,
      g_area_load_dispatch_observations,
      g_client_to_server_area_loaded_observations,
      g_synthetic_area_loaded_observations,
      FormatClientAreaSnapshot(area).c_str());
  return result;
}

bool WriteAbsoluteJump(uint8_t* target, void* destination, size_t patch_size) {
  if (target == nullptr || destination == nullptr || patch_size < kAbsoluteJumpSize) {
    return false;
  }

  DWORD old_protect = 0;
  if (!VirtualProtect(target, patch_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"hook patch failed: VirtualProtect(%p) error=%lu", target, GetLastError());
    return false;
  }

  target[0] = 0x48;
  target[1] = 0xB8;
  *reinterpret_cast<uint64_t*>(target + 2) = reinterpret_cast<uint64_t>(destination);
  target[10] = 0xFF;
  target[11] = 0xE0;
  if (patch_size > kAbsoluteJumpSize) {
    std::memset(target + kAbsoluteJumpSize, 0x90, patch_size - kAbsoluteJumpSize);
  }

  DWORD ignored_protect = 0;
  VirtualProtect(target, patch_size, old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), target, patch_size);
  return true;
}

bool CreateTrampoline(uint8_t* target, size_t stolen_size, void** trampoline) {
  const size_t trampoline_size = stolen_size + kTrampolineReturnJumpSize;
  auto* const code = static_cast<uint8_t*>(VirtualAlloc(nullptr, trampoline_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
  if (code == nullptr) {
    LogFormat(L"hook trampoline allocation failed: error=%lu", GetLastError());
    return false;
  }

  std::memcpy(code, target, stolen_size);
  // Use r11 for the trampoline return so copied prologues that seed rax
  // (for example, mov rax, rsp) keep the register value the original body expects.
  code[stolen_size + 0] = 0x49;
  code[stolen_size + 1] = 0xBB;
  *reinterpret_cast<uint64_t*>(code + stolen_size + 2) = reinterpret_cast<uint64_t>(target + stolen_size);
  code[stolen_size + 10] = 0x41;
  code[stolen_size + 11] = 0xFF;
  code[stolen_size + 12] = 0xE3;
  FlushInstructionCache(GetCurrentProcess(), code, trampoline_size);

  *trampoline = code;
  return true;
}

bool InstallExportHook(
    const char* symbol,
    const uint8_t* expected_prologue,
    size_t prologue_size,
    void* hook,
    void** original,
    const wchar_t* log_name) {
  HMODULE main_module = GetModuleHandleW(nullptr);
  auto* const target = reinterpret_cast<uint8_t*>(GetProcAddress(main_module, symbol));
  if (target == nullptr) {
    LogFormat(L"%s hook skipped: export not found", log_name);
    return false;
  }

  if (!std::equal(expected_prologue, expected_prologue + prologue_size, target)) {
    const size_t logged_size = std::min<size_t>(prologue_size, 32);
    LogFormat(
        L"%s hook skipped: unexpected prologue at %p expected=[%s] actual=[%s]",
        log_name,
        target,
        FormatBytes(expected_prologue, logged_size).c_str(),
        FormatBytes(target, logged_size).c_str());
    return false;
  }

  void* trampoline = nullptr;
  if (!CreateTrampoline(target, prologue_size, &trampoline)) {
    LogFormat(L"%s hook skipped: trampoline creation failed", log_name);
    return false;
  }

  if (!WriteAbsoluteJump(target, hook, prologue_size)) {
    LogFormat(L"%s hook skipped: patch failed", log_name);
    return false;
  }

  *original = trampoline;
  LogFormat(L"%s hook installed at %p", log_name, target);
  return true;
}

bool InstallMainExecutableRvaHook(
    size_t rva,
    const uint8_t* expected_prologue,
    size_t prologue_size,
    void* hook,
    void** original,
    const wchar_t* log_name) {
  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    LogFormat(L"%s hook skipped: main module not found", log_name);
    return false;
  }

  auto* const target = base + rva;
  if (!std::equal(expected_prologue, expected_prologue + prologue_size, target)) {
    const size_t logged_size = std::min<size_t>(prologue_size, 32);
    LogFormat(
        L"%s hook skipped: unexpected prologue at rva=0x%zX target=%p expected=[%s] actual=[%s]",
        log_name,
        rva,
        target,
        FormatBytes(expected_prologue, logged_size).c_str(),
        FormatBytes(target, logged_size).c_str());
    return false;
  }

  void* trampoline = nullptr;
  if (!CreateTrampoline(target, prologue_size, &trampoline)) {
    LogFormat(L"%s hook skipped: trampoline creation failed", log_name);
    return false;
  }

  if (!WriteAbsoluteJump(target, hook, prologue_size)) {
    LogFormat(L"%s hook skipped: patch failed", log_name);
    return false;
  }

  *original = trampoline;
  LogFormat(L"%s hook installed at rva=0x%zX target=%p", log_name, rva, target);
  return true;
}

bool InstallIatHook(
    const char* dll_name,
    const char* import_name,
    void* hook,
    void** original,
    const wchar_t* log_name) {
  if (dll_name == nullptr || import_name == nullptr || hook == nullptr || original == nullptr) {
    return false;
  }

  const bool match_ordinal = import_name[0] == '#';
  unsigned long wanted_ordinal = 0;
  if (match_ordinal) {
    char* end = nullptr;
    wanted_ordinal = std::strtoul(import_name + 1, &end, 10);
    if (end == import_name + 1 || *end != '\0' || wanted_ordinal == 0) {
      LogFormat(L"%s IAT hook skipped: invalid ordinal import specifier %hs", log_name, import_name);
      return false;
    }
  }

  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    LogFormat(L"%s IAT hook skipped: main module not found", log_name);
    return false;
  }

  auto* const dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
  if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
    LogFormat(L"%s IAT hook skipped: invalid DOS header", log_name);
    return false;
  }

  auto* const nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    LogFormat(L"%s IAT hook skipped: invalid PE header", log_name);
    return false;
  }

  const IMAGE_DATA_DIRECTORY& imports = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
  if (imports.VirtualAddress == 0) {
    LogFormat(L"%s IAT hook skipped: import directory not found", log_name);
    return false;
  }

  auto* descriptor = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + imports.VirtualAddress);
  for (; descriptor->Name != 0; ++descriptor) {
    const char* const imported_dll = reinterpret_cast<const char*>(base + descriptor->Name);
    if (_stricmp(imported_dll, dll_name) != 0) {
      continue;
    }

    auto* lookup_thunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(
        base + (descriptor->OriginalFirstThunk != 0 ? descriptor->OriginalFirstThunk : descriptor->FirstThunk));
    auto* iat_thunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(base + descriptor->FirstThunk);

    for (; lookup_thunk->u1.AddressOfData != 0; ++lookup_thunk, ++iat_thunk) {
      if ((lookup_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) != 0) {
        if (!match_ordinal || IMAGE_ORDINAL64(lookup_thunk->u1.Ordinal) != wanted_ordinal) {
          continue;
        }
      } else {
        if (match_ordinal) {
          continue;
        }
        auto* const by_name = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(base + lookup_thunk->u1.AddressOfData);
        if (std::strcmp(reinterpret_cast<const char*>(by_name->Name), import_name) != 0) {
          continue;
        }
      }

      DWORD old_protect = 0;
      if (!VirtualProtect(&iat_thunk->u1.Function, sizeof(iat_thunk->u1.Function), PAGE_READWRITE, &old_protect)) {
        LogFormat(L"%s IAT hook failed: VirtualProtect(%p) error=%lu", log_name, &iat_thunk->u1.Function, GetLastError());
        return false;
      }

      *original = reinterpret_cast<void*>(iat_thunk->u1.Function);
      iat_thunk->u1.Function = reinterpret_cast<ULONGLONG>(hook);

      DWORD ignored_protect = 0;
      VirtualProtect(&iat_thunk->u1.Function, sizeof(iat_thunk->u1.Function), old_protect, &ignored_protect);
      LogFormat(L"%s IAT hook installed for %hs!%hs at %p original=%p", log_name, dll_name, import_name, &iat_thunk->u1.Function, *original);
      return true;
    }
  }

  LogFormat(L"%s IAT hook skipped: %hs!%hs not found", log_name, dll_name, import_name);
  return false;
}

bool InstallWinsockSendToHook() {
  HMODULE winsock = GetModuleHandleW(L"ws2_32.dll");
  if (winsock == nullptr) {
    winsock = LoadLibraryW(L"ws2_32.dll");
  }
  if (winsock != nullptr) {
    g_wsa_get_last_error = reinterpret_cast<WsaGetLastErrorFn>(GetProcAddress(winsock, "WSAGetLastError"));
  }

  bool installed = InstallIatHook(
      "WS2_32.dll",
      "#20",
      reinterpret_cast<void*>(&HookedSendTo),
      reinterpret_cast<void**>(&g_sendto_original),
      L"sendto diagnostic");
  installed |= InstallIatHook(
      "WS2_32.dll",
      "#17",
      reinterpret_cast<void*>(&HookedRecvFrom),
      reinterpret_cast<void**>(&g_recvfrom_original),
      L"recvfrom diagnostic");
  return installed;
}

bool InstallTlkDiagnosticHooks() {
  if (g_tlk_table_fetch_internal_original != nullptr) {
    LogFormat(L"CTlkTable::FetchInternal diagnostic hook already installed");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x40,
      0x55,
      0x53,
      0x56,
      0x57,
      0x41, 0x54,
      0x41, 0x55,
      0x41, 0x56,
      0x41, 0x57,
      0x48, 0x8D, 0x6C, 0x24, 0xE1,
      0x48, 0x81, 0xEC, 0xC8, 0x00, 0x00, 0x00,
  };
  return InstallExportHook(
      "?FetchInternal@CTlkTable@@IEAAHIAEAUSTR_RES@@H@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedTlkTableFetchInternal),
      reinterpret_cast<void**>(&g_tlk_table_fetch_internal_original),
      L"CTlkTable::FetchInternal diagnostic");
}

bool InstallMessageReadBoolHook(const wchar_t* log_name) {
  if (g_message_read_bool_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadBOOL");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x40,
      0x53,
      0x48, 0x83, 0xEC, 0x20,
      0xBA, 0x02, 0x00, 0x00, 0x00,
      0x48, 0x8B, 0xD9,
  };
  return InstallExportHook(
      "?ReadBOOL@CNWMessage@@QEAAHXZ",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadBool),
      reinterpret_cast<void**>(&g_message_read_bool_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadBOOL");
}

bool InstallMessageReadByteHook(const wchar_t* log_name) {
  if (g_message_read_byte_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadBYTE");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x57,
      0x48, 0x83, 0xEC, 0x20,
      0x8B, 0xFA,
  };
  return InstallExportHook(
      "?ReadBYTE@CNWMessage@@QEAAEHH@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadByte),
      reinterpret_cast<void**>(&g_message_read_byte_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadBYTE");
}

bool InstallMessageReadCharHook(const wchar_t* log_name) {
  if (g_message_read_char_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadCHAR");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x57,
      0x48, 0x83, 0xEC, 0x20,
      0x8B, 0xFA,
      0x48, 0x8B, 0xD9,
      0xBA, 0x04, 0x00, 0x00, 0x00,
  };
  return InstallExportHook(
      "?ReadCHAR@CNWMessage@@QEAADH@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadChar),
      reinterpret_cast<void**>(&g_message_read_char_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadCHAR");
}

bool InstallMessageReadShortHook(const wchar_t* log_name) {
  if (g_message_read_short_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadSHORT");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x57,
      0x48, 0x83, 0xEC, 0x20,
      0x8B, 0xFA,
  };
  return InstallExportHook(
      "?ReadSHORT@CNWMessage@@QEAAFH@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadShort),
      reinterpret_cast<void**>(&g_message_read_short_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadSHORT");
}

bool InstallMessageReadWordHook(const wchar_t* log_name) {
  if (g_message_read_word_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadWORD");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x57,
      0x48, 0x83, 0xEC, 0x20,
      0x8B, 0xFA,
  };
  return InstallExportHook(
      "?ReadWORD@CNWMessage@@QEAAGH@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadWord),
      reinterpret_cast<void**>(&g_message_read_word_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadWORD");
}

bool InstallMessageReadDwordHook(const wchar_t* log_name) {
  if (g_message_read_dword_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadDWORD");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x57,
      0x48, 0x83, 0xEC, 0x20,
      0x8B, 0xFA,
  };
  return InstallExportHook(
      "?ReadDWORD@CNWMessage@@QEAAIH@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadDword),
      reinterpret_cast<void**>(&g_message_read_dword_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadDWORD");
}

bool InstallMessageReadIntHook(const wchar_t* log_name) {
  if (g_message_read_int_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadINT");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x57,
      0x48, 0x83, 0xEC, 0x20,
      0x8B, 0xFA,
  };
  return InstallExportHook(
      "?ReadINT@CNWMessage@@QEAAHH@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadInt),
      reinterpret_cast<void**>(&g_message_read_int_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadINT");
}

bool InstallMessageReadFloatHook(const wchar_t* log_name) {
  if (g_message_read_float_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadFLOAT");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x57,
      0x48, 0x83, 0xEC, 0x30,
      0x0F, 0x29, 0x74, 0x24, 0x20,
  };
  return InstallExportHook(
      "?ReadFLOAT@CNWMessage@@QEAAMMH@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadFloat),
      reinterpret_cast<void**>(&g_message_read_float_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadFLOAT");
}

bool InstallMessageReadCExoStringHook(const wchar_t* log_name) {
  if (g_message_read_cexo_string_original != nullptr) {
    LogFormat(L"%s hook already installed", log_name != nullptr ? log_name : L"CNWMessage::ReadCExoString");
    return true;
  }

  static constexpr uint8_t kExpected[] = {
      0x48, 0x89, 0x5C, 0x24, 0x08,
      0x48, 0x89, 0x6C, 0x24, 0x10,
      0x48, 0x89, 0x74, 0x24, 0x18,
  };
  return InstallExportHook(
      "?ReadCExoString@CNWMessage@@QEAA?AVCExoString@@H@Z",
      kExpected,
      sizeof(kExpected),
      reinterpret_cast<void*>(&HookedMessageReadCExoString),
      reinterpret_cast<void**>(&g_message_read_cexo_string_original),
      log_name != nullptr ? log_name : L"CNWMessage::ReadCExoString");
}

bool PatchCallInstructionToReturnZero(uint8_t* call_site, const wchar_t* log_name) {
  if (call_site == nullptr || log_name == nullptr) {
    return false;
  }
  if (call_site[0] != 0xE8) {
    LogFormat(L"%s patch skipped: expected call at %p actual=[%s]", log_name, call_site, FormatBytes(call_site, 5).c_str());
    return false;
  }

  static constexpr uint8_t kReturnZeroPatch[] = {
      0x31, 0xC0,  // xor eax,eax
      0x90, 0x90, 0x90,
  };

  DWORD old_protect = 0;
  if (!VirtualProtect(call_site, sizeof(kReturnZeroPatch), PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"%s patch failed: VirtualProtect(%p) error=%lu", log_name, call_site, GetLastError());
    return false;
  }

  std::memcpy(call_site, kReturnZeroPatch, sizeof(kReturnZeroPatch));

  DWORD ignored_protect = 0;
  VirtualProtect(call_site, sizeof(kReturnZeroPatch), old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), call_site, sizeof(kReturnZeroPatch));
  return true;
}

void VerifyLegacyModuleLoadTailFields() {
  // Diamond 1.69 and EE 8193.37 both consume BYTE + BOOL + BOOL after the
  // resource list. EE then may read two additional BOOLs behind feature gates;
  // those EE-only gates are handled separately by HookedServerSatisfiesBuildBridge.
  auto* const base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (base == nullptr) {
    LogFormat(L"legacy module-load tail verification skipped: main module not found");
    return;
  }

  auto* const tail = base + kEeClientModuleLoadLegacyTailRva;
  const bool expected =
      tail[0x00] == 0xBA && tail[0x01] == 0x08 && tail[0x02] == 0x00 && tail[0x03] == 0x00 &&
      tail[0x04] == 0x00 && tail[0x05] == 0x44 && tail[0x06] == 0x8D && tail[0x07] == 0x42 &&
      tail[0x08] == 0xF9 && tail[0x09] == 0x49 && tail[0x0A] == 0x8B && tail[0x0B] == 0xCC &&
      tail[0x0C] == 0xE8 && tail[0x11] == 0x88 && tail[0x12] == 0x86 && tail[0x13] == 0xE4 &&
      tail[0x14] == 0x02 && tail[0x15] == 0x00 && tail[0x16] == 0x00 && tail[0x17] == 0x49 &&
      tail[0x18] == 0x8B && tail[0x19] == 0xCC && tail[0x1A] == 0xE8 && tail[0x1F] == 0x8B &&
      tail[0x20] == 0xD0 && tail[0x2E] == 0x49 && tail[0x2F] == 0x8B && tail[0x30] == 0xCC &&
      tail[0x31] == 0xE8 && tail[0x36] == 0x8B && tail[0x37] == 0xD0;
  if (!expected) {
    LogFormat(
        L"legacy module-load tail verification mismatch: unexpected bytes at rva=0x%zX target=%p actual=[%s]",
        kEeClientModuleLoadLegacyTailRva,
        tail,
        FormatBytes(tail, 0x38).c_str());
    return;
  }

  LogFormat(
      L"legacy module-load tail verified at rva=0x%zX: native post-resource BYTE+BOOL+BOOL consumption matches 1.69; EE-only feature BOOLs remain gated off",
      kEeClientModuleLoadLegacyTailRva);
}

bool InstallModuleMessageReadDiagnostics() {
  bool installed = false;
  g_module_read_diagnostics_enabled = true;

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x83, 0xEC, 0x58,
        0x8B, 0x41, 0x44,
        0x8B, 0x51, 0x40,
        0x3B, 0xC2,
    };
    installed |= InstallExportHook(
        "?MessageReadOverflow@CNWMessage@@QEAAHXZ",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedMessageReadOverflow),
        reinterpret_cast<void**>(&g_message_read_overflow_original),
        L"CNWMessage::MessageReadOverflow diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x83, 0xEC, 0x58,
        0x8B, 0x41, 0x44,
        0x8B, 0x51, 0x40,
        0x3B, 0xC2,
    };
    installed |= InstallExportHook(
        "?MessageReadUnderflow@CNWMessage@@QEAAHXZ",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedMessageReadUnderflow),
        reinterpret_cast<void**>(&g_message_read_underflow_original),
        L"CNWMessage::MessageReadUnderflow diagnostic");
  }

  {
    installed |= InstallMessageReadBoolHook(L"CNWMessage::ReadBOOL diagnostic");
  }

  {
    installed |= InstallMessageReadCharHook(L"CNWMessage::ReadCHAR diagnostic");
  }

  {
    installed |= InstallMessageReadByteHook(L"CNWMessage::ReadBYTE diagnostic");
  }

  {
    installed |= InstallMessageReadShortHook(L"CNWMessage::ReadSHORT diagnostic");
  }

  {
    installed |= InstallMessageReadWordHook(L"CNWMessage::ReadWORD diagnostic");
  }

  {
    installed |= InstallMessageReadDwordHook(L"CNWMessage::ReadDWORD diagnostic");
  }

  {
    installed |= InstallMessageReadIntHook(L"CNWMessage::ReadINT diagnostic");
  }

  {
    installed |= InstallMessageReadFloatHook(L"CNWMessage::ReadFLOAT diagnostic");
  }

  {
    installed |= InstallMessageReadCExoStringHook(L"CNWMessage::ReadCExoString diagnostic");
  }

  // CNWMessage::ReadCResRef would be useful here, but its first 12+ bytes
  // include a RIP-relative security-cookie load. The simple trampoline used by
  // this bridge does not relocate RIP-relative instructions, so leave that
  // function unhooked and rely on module snapshots plus resource-manager logs.

  return installed;
}

bool InstallAreaMessageReadDiagnostics() {
  bool installed = false;
  g_area_read_diagnostics_enabled = true;

  if (g_message_read_overflow_original == nullptr) {
    static constexpr uint8_t kOverflowExpected[] = {
        0x48, 0x83, 0xEC, 0x58,
        0x8B, 0x41, 0x44,
        0x8B, 0x51, 0x40,
        0x3B, 0xC2,
    };
    installed |= InstallExportHook(
        "?MessageReadOverflow@CNWMessage@@QEAAHXZ",
        kOverflowExpected,
        sizeof(kOverflowExpected),
        reinterpret_cast<void*>(&HookedMessageReadOverflow),
        reinterpret_cast<void**>(&g_message_read_overflow_original),
        L"CNWMessage::MessageReadOverflow area diagnostic");
  } else {
    LogFormat(L"CNWMessage::MessageReadOverflow area diagnostic hook already installed");
    installed = true;
  }

  if (g_message_read_underflow_original == nullptr) {
    static constexpr uint8_t kUnderflowExpected[] = {
        0x48, 0x83, 0xEC, 0x58,
        0x8B, 0x41, 0x44,
        0x8B, 0x51, 0x40,
        0x3B, 0xC2,
    };
    installed |= InstallExportHook(
        "?MessageReadUnderflow@CNWMessage@@QEAAHXZ",
        kUnderflowExpected,
        sizeof(kUnderflowExpected),
        reinterpret_cast<void*>(&HookedMessageReadUnderflow),
        reinterpret_cast<void**>(&g_message_read_underflow_original),
        L"CNWMessage::MessageReadUnderflow area diagnostic");
  } else {
    LogFormat(L"CNWMessage::MessageReadUnderflow area diagnostic hook already installed");
    installed = true;
  }

  installed |= InstallMessageReadBoolHook(L"CNWMessage::ReadBOOL area diagnostic");
  installed |= InstallMessageReadCharHook(L"CNWMessage::ReadCHAR area diagnostic");
  installed |= InstallMessageReadByteHook(L"CNWMessage::ReadBYTE area diagnostic");
  installed |= InstallMessageReadShortHook(L"CNWMessage::ReadSHORT area diagnostic");
  installed |= InstallMessageReadWordHook(L"CNWMessage::ReadWORD area diagnostic");
  installed |= InstallMessageReadDwordHook(L"CNWMessage::ReadDWORD area diagnostic");
  installed |= InstallMessageReadIntHook(L"CNWMessage::ReadINT area diagnostic");
  installed |= InstallMessageReadFloatHook(L"CNWMessage::ReadFLOAT area diagnostic");
  installed |= InstallMessageReadCExoStringHook(L"CNWMessage::ReadCExoString area diagnostic");
  LogFormat(
      L"CNWMessage area-read diagnostics enabled for CNWCArea::LoadArea rva=0x%zX..0x%zX",
      kEeClientAreaLoadBeginRva,
      kEeClientAreaLoadEndRva);
  return installed;
}

bool InstallClientAreaLoadDiagnostics() {
  if (g_client_area_load_original != nullptr) {
    LogFormat(L"CNWCArea::LoadArea diagnostic hook already installed");
    return true;
  }

  static constexpr uint8_t kAreaLoadPrologue[] = {
      0x48, 0x8B, 0xC4,
      0x55,
      0x56,
      0x57,
      0x41, 0x54,
      0x41, 0x55,
      0x41, 0x56,
      0x41, 0x57,
      0x48, 0x8D, 0xA8, 0x28, 0xFA, 0xFF, 0xFF,
      0x48, 0x81, 0xEC, 0xA0, 0x06, 0x00, 0x00,
  };
  return InstallMainExecutableRvaHook(
      kEeClientAreaLoadBeginRva,
      kAreaLoadPrologue,
      sizeof(kAreaLoadPrologue),
      reinterpret_cast<void*>(&HookedClientAreaLoad),
      reinterpret_cast<void**>(&g_client_area_load_original),
      L"CNWCArea::LoadArea diagnostic");
}

bool InstallClientMessageWriteDiagnostics() {
  bool installed = false;

  if (g_client_send_player_to_server_message_original == nullptr) {
    static constexpr uint8_t kClientSendExpected[] = {
        0x48, 0x8B, 0xC4,
        0x44, 0x88, 0x40, 0x18,
        0x88, 0x50, 0x10,
        0x48, 0x89, 0x48, 0x08,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientSendPlayerToServerMessageRva,
        kClientSendExpected,
        sizeof(kClientSendExpected),
        reinterpret_cast<void*>(&HookedClientSendPlayerToServerMessage),
        reinterpret_cast<void**>(&g_client_send_player_to_server_message_original),
        L"SendPlayerToServerMessage diagnostic");
  } else {
    LogFormat(L"SendPlayerToServerMessage diagnostic hook already installed");
    installed = true;
  }

  if (g_message_create_write_message_original == nullptr) {
    static constexpr uint8_t kCreateWriteExpected[] = {
        0x40,
        0x53,
        0x48, 0x83, 0xEC, 0x20,
        0x83, 0xC2, 0x07,
        0x44, 0x89, 0x49, 0x60,
        0x48, 0x8B, 0xD9,
        0x44, 0x89, 0x41, 0x64,
        0xC7, 0x41, 0x30, 0x01, 0x00, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        "?CreateWriteMessage@CNWMessage@@QEAAXIIH@Z",
        kCreateWriteExpected,
        sizeof(kCreateWriteExpected),
        reinterpret_cast<void*>(&HookedMessageCreateWriteMessage),
        reinterpret_cast<void**>(&g_message_create_write_message_original),
        L"CNWMessage::CreateWriteMessage diagnostic");
  } else {
    LogFormat(L"CNWMessage::CreateWriteMessage diagnostic hook already installed");
    installed = true;
  }

  if (g_message_get_write_message_original == nullptr) {
    static constexpr uint8_t kGetWriteExpected[] = {
        0x40,
        0x53,
        0x57,
        0x48, 0x83, 0xEC, 0x28,
        0x8B, 0x41, 0x28,
        0x48, 0x8B, 0xFA,
    };
    installed |= InstallExportHook(
        "?GetWriteMessage@CNWMessage@@QEAAHPEAPEAEPEAI@Z",
        kGetWriteExpected,
        sizeof(kGetWriteExpected),
        reinterpret_cast<void*>(&HookedMessageGetWriteMessage),
        reinterpret_cast<void**>(&g_message_get_write_message_original),
        L"CNWMessage::GetWriteMessage diagnostic");
  } else {
    LogFormat(L"CNWMessage::GetWriteMessage diagnostic hook already installed");
    installed = true;
  }

  LogFormat(
      L"CNWMessage client-write diagnostics enabled for SendPlayerToServerMessage rva=0x%zX",
      kEeClientSendPlayerToServerMessageRva);
  return installed;
}

bool InstallDriverOnlyAutoCharacterHooks() {
  bool installed = false;

  if (g_client_send_player_to_server_message_original == nullptr) {
    static constexpr uint8_t kClientSendExpected[] = {
        0x48, 0x8B, 0xC4,
        0x44, 0x88, 0x40, 0x18,
        0x88, 0x50, 0x10,
        0x48, 0x89, 0x48, 0x08,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientSendPlayerToServerMessageRva,
        kClientSendExpected,
        sizeof(kClientSendExpected),
        reinterpret_cast<void*>(&HookedClientSendPlayerToServerMessageDriverOnly),
        reinterpret_cast<void**>(&g_client_send_player_to_server_message_original),
        L"driver-only SendPlayerToServerMessage pass-through");
  } else {
    LogFormat(L"driver-only SendPlayerToServerMessage pass-through hook already installed");
    installed = true;
  }

  if (g_server_to_player_message_original == nullptr) {
    static constexpr uint8_t kServerToPlayerMessageExpected[] = {
        0x44, 0x89, 0x44, 0x24, 0x18,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0x6C, 0x24, 0xD9,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientServerToPlayerMessageBeginRva,
        kServerToPlayerMessageExpected,
        sizeof(kServerToPlayerMessageExpected),
        reinterpret_cast<void*>(&HookedServerToPlayerMessageDriverOnly),
        reinterpret_cast<void**>(&g_server_to_player_message_original),
        L"driver-only HandleServerToPlayerMessage auto-character observer");
  } else {
    LogFormat(L"driver-only HandleServerToPlayerMessage observer hook already installed");
    installed = true;
  }

  LogFormat(L"driver-only auto-character hooks installed=%d; no protocol/resource compatibility rewrites are active", installed ? 1 : 0);
  return installed;
}

bool InstallLiveMessageReadDiagnostics() {
  bool installed = false;
  g_live_message_read_diagnostics_enabled = true;

  installed |= InstallClientMessageWriteDiagnostics();

  if (g_message_read_overflow_original == nullptr) {
    static constexpr uint8_t kOverflowExpected[] = {
        0x48, 0x83, 0xEC, 0x58,
        0x8B, 0x41, 0x44,
        0x8B, 0x51, 0x40,
        0x3B, 0xC2,
    };
    installed |= InstallExportHook(
        "?MessageReadOverflow@CNWMessage@@QEAAHXZ",
        kOverflowExpected,
        sizeof(kOverflowExpected),
        reinterpret_cast<void*>(&HookedMessageReadOverflow),
        reinterpret_cast<void**>(&g_message_read_overflow_original),
        L"CNWMessage::MessageReadOverflow live diagnostic");
  } else {
    LogFormat(L"CNWMessage::MessageReadOverflow live diagnostic hook already installed");
    installed = true;
  }

  if (g_message_read_underflow_original == nullptr) {
    static constexpr uint8_t kUnderflowExpected[] = {
        0x48, 0x83, 0xEC, 0x58,
        0x8B, 0x41, 0x44,
        0x8B, 0x51, 0x40,
        0x3B, 0xC2,
    };
    installed |= InstallExportHook(
        "?MessageReadUnderflow@CNWMessage@@QEAAHXZ",
        kUnderflowExpected,
        sizeof(kUnderflowExpected),
        reinterpret_cast<void*>(&HookedMessageReadUnderflow),
        reinterpret_cast<void**>(&g_message_read_underflow_original),
        L"CNWMessage::MessageReadUnderflow live diagnostic");
  } else {
    LogFormat(L"CNWMessage::MessageReadUnderflow live diagnostic hook already installed");
    installed = true;
  }

  installed |= InstallMessageReadBoolHook(L"CNWMessage::ReadBOOL live diagnostic");
  installed |= InstallMessageReadCharHook(L"CNWMessage::ReadCHAR live diagnostic");
  installed |= InstallMessageReadByteHook(L"CNWMessage::ReadBYTE live diagnostic");
  installed |= InstallMessageReadShortHook(L"CNWMessage::ReadSHORT live diagnostic");
  installed |= InstallMessageReadWordHook(L"CNWMessage::ReadWORD live diagnostic");
  installed |= InstallMessageReadDwordHook(L"CNWMessage::ReadDWORD live diagnostic");
  installed |= InstallMessageReadIntHook(L"CNWMessage::ReadINT live diagnostic");
  installed |= InstallMessageReadFloatHook(L"CNWMessage::ReadFLOAT live diagnostic");
  installed |= InstallMessageReadCExoStringHook(L"CNWMessage::ReadCExoString live diagnostic");

  if (g_raw_read_dword_original == nullptr) {
    static constexpr uint8_t kRawReadDwordExpected[] = {
        0x44, 0x8B, 0x49, 0x44,
        0x8B, 0x51, 0x40,
        0x49, 0x8D, 0x41, 0x04,
        0x45, 0x8D, 0x41, 0x04,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientRawReadDwordRva,
        kRawReadDwordExpected,
        sizeof(kRawReadDwordExpected),
        reinterpret_cast<void*>(&HookedRawReadDword),
        reinterpret_cast<void**>(&g_raw_read_dword_original),
        L"CNWMessage raw DWORD live diagnostic");
  } else {
    LogFormat(L"CNWMessage raw DWORD live diagnostic hook already installed");
    installed = true;
  }

  if (g_server_to_player_message_original == nullptr) {
    static constexpr uint8_t kServerToPlayerMessageExpected[] = {
        0x44, 0x89, 0x44, 0x24, 0x18,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0x6C, 0x24, 0xD9,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientServerToPlayerMessageBeginRva,
        kServerToPlayerMessageExpected,
        sizeof(kServerToPlayerMessageExpected),
        reinterpret_cast<void*>(&HookedServerToPlayerMessage),
        reinterpret_cast<void**>(&g_server_to_player_message_original),
        L"HandleServerToPlayerMessage live diagnostic");
  } else {
    LogFormat(L"HandleServerToPlayerMessage live diagnostic hook already installed");
    installed = true;
  }

  if (g_live_game_object_update_original == nullptr) {
    static constexpr uint8_t kLiveGameObjectUpdateExpected[] = {
        0x48, 0x8B, 0xC4,
        0x57,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x83, 0xEC, 0x50,
        0x48, 0xC7, 0x40, 0xC8, 0xFE, 0xFF, 0xFF, 0xFF,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientLiveGameObjectUpdateBeginRva,
        kLiveGameObjectUpdateExpected,
        sizeof(kLiveGameObjectUpdateExpected),
        reinterpret_cast<void*>(&HookedLiveGameObjectUpdate),
        reinterpret_cast<void**>(&g_live_game_object_update_original),
        L"HandleServerToPlayerGameObjectUpdate actual live diagnostic");
  } else {
    LogFormat(L"HandleServerToPlayerGameObjectUpdate actual live diagnostic hook already installed");
    installed = true;
  }

  if (g_game_object_update_original == nullptr) {
    static constexpr uint8_t kGameObjectUpdateExpected[] = {
        0x48, 0x8B, 0xC4,
        0x55,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0xA8, 0x58, 0xFB, 0xFF, 0xFF,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientGameObjectUpdateBeginRva,
        kGameObjectUpdateExpected,
        sizeof(kGameObjectUpdateExpected),
        reinterpret_cast<void*>(&HookedGameObjectUpdate),
        reinterpret_cast<void**>(&g_game_object_update_original),
        L"HandleServerToPlayerGameObjectUpdate player-list/major10 diagnostic");
  } else {
    LogFormat(L"HandleServerToPlayerGameObjectUpdate player-list/major10 diagnostic hook already installed");
    installed = true;
  }

  LogFormat(
      L"CNWMessage live-read diagnostics enabled for live game-object updates rva=0x%zX..0x%zX, player-list/major10 updates rva=0x%zX..0x%zX, and dispatcher rva=0x%zX..0x%zX",
      kEeClientLiveGameObjectUpdateBeginRva,
      kEeClientLiveGameObjectUpdateEndRva,
      kEeClientGameObjectUpdateBeginRva,
      kEeClientGameObjectUpdateEndRva,
      kEeClientServerToPlayerMessageBeginRva,
      kEeClientServerToPlayerMessageEndRva);
  return installed;
}

bool InstallResourceManagerDiagnostics() {
  bool installed = false;

  {
    static constexpr uint8_t kExpected[] = {
        0x40, 0x53,
        0x55,
        0x56,
        0x57,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x83, 0xEC, 0x48,
    };
    installed |= InstallExportHook(
        "?Exists@CExoResMan@@QEAAHAEBVCResRef@@GPEAI@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManExists),
        reinterpret_cast<void**>(&g_resman_exists_original),
        L"CExoResMan::Exists diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x83, 0xEC, 0x48,
        0x33, 0xC0,
        0xC6, 0x44, 0x24, 0x28, 0x01,
        0x48, 0x89, 0x44, 0x24, 0x30,
    };
    installed |= InstallExportHook(
        "?GetResObject@CExoResMan@@QEAAPEAVCRes@@AEBVCResRef@@G@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManGetResObject),
        reinterpret_cast<void**>(&g_resman_get_res_object_original),
        L"CExoResMan::GetResObject diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x4D, 0x85, 0xC9,
        0x0F, 0x84, 0x4B, 0x02, 0x00, 0x00,
        0x48, 0x8B, 0xC4,
    };
    installed |= InstallExportHook(
        "?SetResObject@CExoResMan@@QEAAXAEBVCResRef@@GPEAVCRes@@@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManSetResObject),
        reinterpret_cast<void**>(&g_resman_set_res_object_original),
        L"CExoResMan::SetResObject diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x40,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x83, 0xEC, 0x60,
    };
    installed |= InstallExportHook(
        "?Demand@CExoResMan@@QEAAPEAXPEAVCRes@@@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManDemand),
        reinterpret_cast<void**>(&g_resman_demand_original),
        L"CExoResMan::Demand diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x83, 0xEC, 0x38,
        0x41, 0x8B, 0xC0,
        0x45, 0x33, 0xC0,
        0x4C, 0x89, 0x44, 0x24, 0x28,
    };
    installed |= InstallExportHook(
        "?AddEncapsulatedResourceFile@CExoResMan@@QEAAHAEBVCExoString@@I@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManAddEncapsulatedResourceFile),
        reinterpret_cast<void**>(&g_resman_add_encapsulated_resource_file_original),
        L"CExoResMan::AddEncapsulatedResourceFile diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x83, 0xEC, 0x38,
        0x41, 0x8B, 0xC0,
        0x45, 0x33, 0xC0,
        0x4C, 0x89, 0x44, 0x24, 0x28,
    };
    installed |= InstallExportHook(
        "?AddFixedKeyTableFile@CExoResMan@@QEAAHAEBVCExoString@@I@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManAddFixedKeyTableFile),
        reinterpret_cast<void**>(&g_resman_add_fixed_key_table_file_original),
        L"CExoResMan::AddFixedKeyTableFile diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x83, 0xEC, 0x38,
        0x48, 0x8B, 0x44, 0x24, 0x60,
        0x45, 0x8B, 0xD0,
    };
    installed |= InstallExportHook(
        "?AddResourceDirectory@CExoResMan@@QEAAHAEBVCExoString@@IHQ6A_NPEAVCExoKeyTable@@AEBVCResRef@@G@Z@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManAddResourceDirectory),
        reinterpret_cast<void**>(&g_resman_add_resource_directory_original),
        L"CExoResMan::AddResourceDirectory diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x44, 0x89, 0x4C, 0x24, 0x20,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x83, 0xEC, 0x60,
    };
    installed |= InstallExportHook(
        "?RemoveKeyTable@CExoResMan@@QEAAHAEBVCExoString@@IH@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManRemoveKeyTable),
        reinterpret_cast<void**>(&g_resman_remove_key_table_original),
        L"CExoResMan::RemoveKeyTable diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x40,
        0x53,
        0x48, 0x83, 0xEC, 0x30,
        0x48, 0xC7, 0x44, 0x24, 0x20, 0xFE, 0xFF, 0xFF, 0xFF,
    };
    installed |= InstallExportHook(
        "?ClearOverrides@CExoResMan@@QEAAXXZ",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedResManClearOverrides),
        reinterpret_cast<void**>(&g_resman_clear_overrides_original),
        L"CExoResMan::ClearOverrides diagnostic");
  }

  return installed;
}

bool InstallWorldResourceDiagnostics() {
  bool installed = false;

  {
    static constexpr uint8_t kExpected[] = {
        0x40,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0x6C, 0x24, 0xD0,
        0x48, 0x81, 0xEC, 0x30, 0x01, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        "?GetTileSet@CNWTileSetManager@@QEAAPEAVCNWTileSet@@AEBVCResRef@@@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedTileSetManagerGetTileSet),
        reinterpret_cast<void**>(&g_tileset_manager_get_tileset_original),
        L"CNWTileSetManager::GetTileSet diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x8B, 0xC4,
        0x55,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0xA8, 0x08, 0xFE, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0xD0, 0x02, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        "?LoadTileSet@CNWTileSet@@QEAAHXZ",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedNwTileSetLoadTileSet),
        reinterpret_cast<void**>(&g_nw_tileset_load_tileset_original),
        L"CNWTileSet::LoadTileSet diagnostic");
  }

  {
    static constexpr uint8_t kExpected[] = {
        0x48, 0x89, 0x5C, 0x24, 0x18,
        0x48, 0x89, 0x74, 0x24, 0x20,
        0x57,
        0x48, 0x83, 0xEC, 0x40,
    };
    installed |= InstallExportHook(
        "?SetTileset@CNWSArea@@QEAAHAEBVCResRef@@@Z",
        kExpected,
        sizeof(kExpected),
        reinterpret_cast<void*>(&HookedNwsAreaSetTileset),
        reinterpret_cast<void**>(&g_nws_area_set_tileset_original),
        L"CNWSArea::SetTileset diagnostic");
  }

  return installed;
}

bool InstallLegacyLiveVisualTransformSkipHook() {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_VISUAL_TRANSFORM_SKIP")) {
    LogFormat(
        L"legacy live visual-transform skip hook skipped: HG_BRIDGE_DISABLE_LEGACY_LIVE_VISUAL_TRANSFORM_SKIP is set");
    return false;
  }

  static constexpr uint8_t kExpectedPrologue[] = {
      0x40,
      0x55,
      0x56,
      0x57,
      0x41, 0x56,
      0x41, 0x57,
      0x48, 0x8B, 0xEC,
      0x48, 0x83, 0xEC, 0x70,
  };
  return InstallMainExecutableRvaHook(
      kEeClientVisualTransformReadRva,
      kExpectedPrologue,
      sizeof(kExpectedPrologue),
      reinterpret_cast<void*>(&HookedVisualTransformRead),
      reinterpret_cast<void**>(&g_visual_transform_read_original),
      L"legacy live visual-transform skip");
}

bool InstallLegacyLiveItemAppearanceReadHook() {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_ITEM_APPEARANCE_READ")) {
    LogFormat(
        L"legacy live item-appearance read hook skipped: HG_BRIDGE_DISABLE_LEGACY_LIVE_ITEM_APPEARANCE_READ is set");
    return false;
  }
  if (g_item_appearance_read_original != nullptr) {
    LogFormat(L"legacy live item-appearance read hook already installed");
    return true;
  }

  static constexpr uint8_t kExpectedPrologue[] = {
      0x40, 0x57,
      0x41, 0x56,
      0x41, 0x57,
      0x48, 0x83, 0xEC, 0x40,
      0x48, 0xC7, 0x44, 0x24, 0x20, 0xFE, 0xFF, 0xFF, 0xFF,
  };
  return InstallMainExecutableRvaHook(
      kEeClientItemAppearanceReadRva,
      kExpectedPrologue,
      sizeof(kExpectedPrologue),
      reinterpret_cast<void*>(&HookedItemAppearanceRead),
      reinterpret_cast<void**>(&g_item_appearance_read_original),
      L"legacy live item-appearance read");
}

bool InstallPlaceableUseableDiagnosticsHook() {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_PLACEABLE_USEABLE_DIAGNOSTIC")) {
    LogFormat(
        L"placeable SetUseable diagnostic hook skipped: HG_BRIDGE_DISABLE_PLACEABLE_USEABLE_DIAGNOSTIC is set");
    return false;
  }
  if (g_placeable_set_useable_original != nullptr) {
    LogFormat(L"placeable SetUseable diagnostic hook already installed");
    return true;
  }

  static constexpr uint8_t kExpectedPrologue[] = {
      0x40,
      0x53,
      0x48, 0x83, 0xEC, 0x20,
      0x89, 0x91, 0x1C, 0x01, 0x00, 0x00,
      0x48, 0x8B, 0xD9,
  };
  return InstallMainExecutableRvaHook(
      kEeClientPlaceableSetUseableRva,
      kExpectedPrologue,
      sizeof(kExpectedPrologue),
      reinterpret_cast<void*>(&HookedPlaceableSetUseable),
      reinterpret_cast<void**>(&g_placeable_set_useable_original),
      L"placeable SetUseable diagnostic");
}

bool InstallLegacyLiveShortAddSkipHooks() {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_LIVE_SHORT_ADD_SKIP")) {
    LogFormat(
        L"legacy live short add skip hooks skipped: HG_BRIDGE_DISABLE_LEGACY_LIVE_SHORT_ADD_SKIP is set");
    return false;
  }

  bool installed = false;
  if (g_live_door_add_original == nullptr) {
    static constexpr uint8_t kDoorExpectedPrologue[] = {
        0x40,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0x6C, 0x24, 0xA0,
        0x48, 0x81, 0xEC, 0x60, 0x01, 0x00, 0x00,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientLiveDoorAddRva,
        kDoorExpectedPrologue,
        sizeof(kDoorExpectedPrologue),
        reinterpret_cast<void*>(&HookedLiveDoorAdd),
        reinterpret_cast<void**>(&g_live_door_add_original),
        L"legacy live short door add skip");
  } else {
    LogFormat(L"legacy live short door add skip hook already installed");
    installed = true;
  }

  if (g_live_trigger_add_original == nullptr) {
    static constexpr uint8_t kTriggerExpectedPrologue[] = {
        0x48, 0x8B, 0xC4,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0x68, 0xA1,
        0x48, 0x81, 0xEC, 0xF0, 0x00, 0x00, 0x00,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientLiveTriggerAddRva,
        kTriggerExpectedPrologue,
        sizeof(kTriggerExpectedPrologue),
        reinterpret_cast<void*>(&HookedLiveTriggerAdd),
        reinterpret_cast<void**>(&g_live_trigger_add_original),
        L"legacy live trigger add diagnostic");
  } else {
    LogFormat(L"legacy live trigger add diagnostic hook already installed");
    installed = true;
  }

  if (g_live_placeable_add_original == nullptr) {
    static constexpr uint8_t kPlaceableExpectedPrologue[] = {
        0x40,
        0x55,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0xAC, 0x24, 0x60, 0xFF, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0xA0, 0x01, 0x00, 0x00,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientLivePlaceableAddRva,
        kPlaceableExpectedPrologue,
        sizeof(kPlaceableExpectedPrologue),
        reinterpret_cast<void*>(&HookedLivePlaceableAdd),
        reinterpret_cast<void**>(&g_live_placeable_add_original),
        L"legacy live short placeable add skip");
  } else {
    LogFormat(L"legacy live short placeable add skip hook already installed");
    installed = true;
  }

  return installed;
}

bool InstallDriverOnlyNetLayerDiagnostics() {
  if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_DRIVER_NETLAYER_DIAGNOSTICS")) {
    return false;
  }

  bool installed = false;
  {
    static constexpr char kSymbol[] = "?FrameReceive@CNetLayerWindow@@QEAAHPEAEI@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x44, 0x89, 0x44, 0x24, 0x18,
        0x55,
        0x53,
        0x56,
        0x57,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedDriverNetLayerFrameReceive),
        reinterpret_cast<void**>(&g_netlayer_window_frame_receive_original),
        L"driver-only CNetLayerWindow::FrameReceive diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?UnpacketizeFullMessages@CNetLayerWindow@@QEAAHH@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x48, 0x8B, 0xC4,
        0x55,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0xA8, 0x98, 0xFE, 0xFF, 0xFF,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedDriverNetLayerUnpacketizeFullMessages),
        reinterpret_cast<void**>(&g_netlayer_window_unpacketize_full_messages_original),
        L"driver-only CNetLayerWindow::UnpacketizeFullMessages diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?UncompressMessage@CNetLayerInternal@@QEAAHIPEAEI@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x48, 0x8B, 0xC4,
        0x55,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0xA8, 0x08, 0xFF, 0xFF, 0xFF,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedDriverNetLayerUncompressMessage),
        reinterpret_cast<void**>(&g_netlayer_internal_uncompress_message_original),
        L"driver-only CNetLayerInternal::UncompressMessage diagnostic");
  }

  if (installed) {
    LogFormat(L"driver-only netlayer diagnostics enabled; proxy still owns all packet compatibility");
  }
  return installed;
}

bool InstallDriverOnlyAutoQuitConfirmHook() {
  if (!DriverAutoQuitConfirmEnabled()) {
    if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DRIVER_AUTO_QUIT_CONFIRM")) {
      LogFormat(L"driver-only auto-quit confirm hook skipped: HG_BRIDGE_DISABLE_DRIVER_AUTO_QUIT_CONFIRM is set");
    }
    return false;
  }

  bool installed = false;
  {
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x53,
        0x48, 0x83, 0xEC, 0x30,
        0x48, 0xC7, 0x44, 0x24, 0x20, 0xFE, 0xFF, 0xFF, 0xFF,
        0x48, 0x8B, 0xD9,
    };
    if (g_driver_quit_confirmation_prompt_original == nullptr) {
      installed |= InstallMainExecutableRvaHook(
          kEeDriverQuitConfirmationPromptRva,
          kExpectedPrologue,
          sizeof(kExpectedPrologue),
          reinterpret_cast<void*>(&HookedDriverQuitConfirmationPrompt),
          reinterpret_cast<void**>(&g_driver_quit_confirmation_prompt_original),
          L"driver-only quit confirmation auto-accept");
    } else {
      installed = true;
      LogFormat(L"driver-only quit confirmation auto-accept hook already installed");
    }
  }

  {
    static constexpr char kSymbol[] = "?DisconnectFromSession@CNetLayerInternal@@QEAAHXZ";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x53,
        0x48, 0x83, 0xEC, 0x20,
        0x83, 0xB9, 0xD8, 0x56, 0x8A, 0x02, 0xFF,
        0x48, 0x8B, 0xD9,
    };
    if (g_netlayer_internal_disconnect_from_session_original == nullptr) {
      installed |= InstallExportHook(
          kSymbol,
          kExpectedPrologue,
          sizeof(kExpectedPrologue),
          reinterpret_cast<void*>(&HookedDriverDisconnectFromSessionInternal),
          reinterpret_cast<void**>(&g_netlayer_internal_disconnect_from_session_original),
          L"driver-only CNetLayerInternal::DisconnectFromSession auto-quit confirm");
    } else {
      installed = true;
      LogFormat(L"driver-only CNetLayerInternal::DisconnectFromSession auto-quit confirm hook already installed");
    }
  }

  if (installed) {
    LogFormat(
        L"driver-only auto-quit confirm enabled; set HG_BRIDGE_DISABLE_DRIVER_AUTO_QUIT_CONFIRM=1 to keep quit prompts visible");
  }
  return installed;
}

bool InstallEeModuleCompatibilityHooks() {
  bool installed = false;

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_MODULE_TAIL_VERIFY")) {
    LogFormat(L"legacy module-load tail verification skipped: HG_BRIDGE_DISABLE_LEGACY_MODULE_TAIL_VERIFY is set");
  } else {
    VerifyLegacyModuleLoadTailFields();
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_MODULE_HAK_LIST_SKIP")) {
    LogFormat(L"legacy module-load hak-list skip hook skipped: HG_BRIDGE_DISABLE_LEGACY_MODULE_HAK_LIST_SKIP is set");
  } else {
    installed |= InstallMessageReadDwordHook(L"legacy module-load hak-list skip");
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_MODULE_READ_DIAGNOSTICS")) {
    LogFormat(L"CNWMessage module-read diagnostic hooks skipped: HG_BRIDGE_DISABLE_MODULE_READ_DIAGNOSTICS is set");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_MODULE_READ_DIAGNOSTICS")) {
    installed |= InstallModuleMessageReadDiagnostics();
  } else {
    LogFormat(L"CNWMessage module-read diagnostic hooks skipped: HG_BRIDGE_ENABLE_MODULE_READ_DIAGNOSTICS is not set");
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AREA_READ_DIAGNOSTICS")) {
    LogFormat(L"CNWMessage area-read diagnostic hooks skipped: HG_BRIDGE_DISABLE_AREA_READ_DIAGNOSTICS is set");
  } else {
    installed |= InstallAreaMessageReadDiagnostics();
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AREA_LOAD_DIAGNOSTICS")) {
    LogFormat(L"CNWCArea::LoadArea diagnostic hook skipped: HG_BRIDGE_DISABLE_AREA_LOAD_DIAGNOSTICS is set");
  } else {
    installed |= InstallClientAreaLoadDiagnostics();
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LIVE_MESSAGE_READ_DIAGNOSTICS")) {
    LogFormat(L"CNWMessage live-read diagnostic hooks skipped: HG_BRIDGE_DISABLE_LIVE_MESSAGE_READ_DIAGNOSTICS is set");
  } else {
    installed |= InstallLiveMessageReadDiagnostics();
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_RESMAN_DIAGNOSTICS")) {
    LogFormat(L"CExoResMan diagnostic hooks skipped: HG_BRIDGE_DISABLE_RESMAN_DIAGNOSTICS is set");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_RESMAN_DIAGNOSTICS")) {
    installed |= EnsureResourceManagerDiagnosticsInstalled(L"startup env opt-in");
  } else {
    LogFormat(L"CExoResMan diagnostic hooks deferred until runtime module-resource diagnostics");
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_MODULE_FEATURE_GATE")) {
    LogFormat(L"legacy server feature gate hook skipped: HG_BRIDGE_DISABLE_LEGACY_MODULE_FEATURE_GATE is set");
  } else {
    static constexpr uint8_t kFeatureGatePrologue[] = {
        0x48, 0x89, 0x5C, 0x24, 0x08,
        0x48, 0x89, 0x74, 0x24, 0x10,
        0x57,
        0x48, 0x83, 0xEC, 0x20,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeServerSatisfiesBuildBridgeRva,
        kFeatureGatePrologue,
        sizeof(kFeatureGatePrologue),
        reinterpret_cast<void*>(&HookedServerSatisfiesBuildBridge),
        reinterpret_cast<void**>(&g_server_satisfies_build_bridge_original),
        L"legacy server feature gate");
  }

  installed |= InstallLegacyLiveVisualTransformSkipHook();
  installed |= InstallLegacyLiveItemAppearanceReadHook();
  installed |= InstallLegacyLiveShortAddSkipHooks();
  installed |= InstallPlaceableUseableDiagnosticsHook();

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_MODULE_LOAD_DIAGNOSTICS")) {
    LogFormat(L"client module-load diagnostic hooks skipped: HG_BRIDGE_DISABLE_MODULE_LOAD_DIAGNOSTICS is set");
    return installed;
  }

  {
    static constexpr uint8_t kLoadModulePrologue[] = {
        0x48, 0x8B, 0xC4,
        0x55,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0x68, 0xA1,
        0x48, 0x81, 0xEC, 0xC0, 0x00, 0x00, 0x00,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientModuleLoadRva,
        kLoadModulePrologue,
        sizeof(kLoadModulePrologue),
        reinterpret_cast<void*>(&HookedClientModuleLoad),
        reinterpret_cast<void**>(&g_client_module_load_original),
        L"CNWCModule::LoadModule diagnostic");
  }

  {
    static constexpr uint8_t kLoadResourcesPrologue[] = {
        0x48, 0x8B, 0xC4,
        0x55,
        0x41, 0x54,
        0x41, 0x55,
        0x41, 0x56,
        0x41, 0x57,
        0x48, 0x8D, 0xA8, 0x68, 0xFE, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0x70, 0x02, 0x00, 0x00,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeClientModuleLoadResourcesRva,
        kLoadResourcesPrologue,
        sizeof(kLoadResourcesPrologue),
        reinterpret_cast<void*>(&HookedClientModuleLoadResources),
        reinterpret_cast<void**>(&g_client_module_load_resources_original),
        L"CNWCModule::LoadModuleResources diagnostic");
  }

  return installed;
}

bool InstallDriverOnlyModuleResourceDiagnostics() {
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DRIVER_MODULE_RESOURCE_DIAGNOSTICS")) {
    LogFormat(L"driver-only CNWCModule module diagnostic hooks skipped: HG_BRIDGE_DISABLE_DRIVER_MODULE_RESOURCE_DIAGNOSTICS is set");
    return false;
  }

  bool installed = false;
  static constexpr uint8_t kLoadModulePrologue[] = {
      0x48, 0x8B, 0xC4,
      0x55,
      0x41, 0x54,
      0x41, 0x55,
      0x41, 0x56,
      0x41, 0x57,
      0x48, 0x8D, 0x68, 0xA1,
      0x48, 0x81, 0xEC, 0xC0, 0x00, 0x00, 0x00,
  };
  installed |= InstallMainExecutableRvaHook(
      kEeClientModuleLoadRva,
      kLoadModulePrologue,
      sizeof(kLoadModulePrologue),
      reinterpret_cast<void*>(&HookedClientModuleLoad),
      reinterpret_cast<void**>(&g_client_module_load_original),
      L"driver-only CNWCModule::LoadModule diagnostic");

  static constexpr uint8_t kLoadResourcesPrologue[] = {
      0x48, 0x8B, 0xC4,
      0x55,
      0x41, 0x54,
      0x41, 0x55,
      0x41, 0x56,
      0x41, 0x57,
      0x48, 0x8D, 0xA8, 0x68, 0xFE, 0xFF, 0xFF,
      0x48, 0x81, 0xEC, 0x70, 0x02, 0x00, 0x00,
  };
  installed |= InstallMainExecutableRvaHook(
      kEeClientModuleLoadResourcesRva,
      kLoadResourcesPrologue,
      sizeof(kLoadResourcesPrologue),
      reinterpret_cast<void*>(&HookedClientModuleLoadResources),
      reinterpret_cast<void**>(&g_client_module_load_resources_original),
      L"driver-only CNWCModule::LoadModuleResources diagnostic");
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_MODULE_READ_DIAGNOSTICS")) {
    installed |= InstallModuleMessageReadDiagnostics();
    LogFormat(
        L"driver-only CNWMessage module-read diagnostics enabled; legacy module/resource compatibility remains disabled");
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_DRIVER_AREA_DIAGNOSTICS")) {
    installed |= InstallAreaMessageReadDiagnostics();
    installed |= InstallClientAreaLoadDiagnostics();
    installed |= EnsureWorldResourceDiagnosticsInstalled(L"driver-only area diagnostics");
    LogFormat(
        L"driver-only area/tileset diagnostics enabled; legacy area/module/resource compatibility remains disabled");
    if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_RESMAN_DIAGNOSTICS") ||
        IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_RUNTIME_RESOURCE_DIAGNOSTICS")) {
      installed |= EnsureResourceManagerDiagnosticsInstalled(L"driver-only area diagnostics");
    }
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_DRIVER_FEATURE_DIAGNOSTICS")) {
    static constexpr uint8_t kFeatureGatePrologue[] = {
        0x48, 0x89, 0x5C, 0x24, 0x08,
        0x48, 0x89, 0x74, 0x24, 0x10,
        0x57,
        0x48, 0x83, 0xEC, 0x20,
    };
    installed |= InstallMainExecutableRvaHook(
        kEeServerSatisfiesBuildBridgeRva,
        kFeatureGatePrologue,
        sizeof(kFeatureGatePrologue),
        reinterpret_cast<void*>(&HookedServerSatisfiesBuildBridge),
        reinterpret_cast<void**>(&g_server_satisfies_build_bridge_original),
        L"driver-only ServerSatisfiesBuild diagnostic");
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_DRIVER_LIVE_DIAGNOSTICS")) {
    LogFormat(
        L"driver-only live-read diagnostics not installed: existing CNWMessage live hooks include legacy compatibility shims, and driver-only must leave live-object compatibility to the proxy");
  }
  if (installed) {
    LogFormat(L"driver-only module diagnostics installed; proxy still owns all protocol/resource compatibility");
  }
  return installed;
}

bool InstallStartAddressTranslationLogHook() {
  static constexpr char kSymbol[] = "?StartAddressTranslation@CExoNetInternal@@QEAAXAEBVCExoString@@@Z";
  static constexpr uint8_t kExpectedPrologue[] = {
      0x40,
      0x55,
      0x56,
      0x57,
      0x41,
      0x54,
      0x41,
      0x55,
      0x41,
      0x56,
      0x41,
      0x57,
      0x48,
      0x8D,
      0x6C,
      0x24,
      0x90,
      0x48,
      0x81,
      0xEC,
      0x70,
      0x01,
      0x00,
      0x00,
  };

  HMODULE main_module = GetModuleHandleW(nullptr);
  auto* const target = reinterpret_cast<uint8_t*>(GetProcAddress(main_module, kSymbol));
  if (target == nullptr) {
    LogFormat(L"address translation hook skipped: export not found");
    return false;
  }

  if (!std::equal(kExpectedPrologue, kExpectedPrologue + sizeof(kExpectedPrologue), target)) {
    LogFormat(L"address translation hook skipped: unexpected prologue at %p", target);
    return false;
  }

  void* trampoline = nullptr;
  if (!CreateTrampoline(target, sizeof(kExpectedPrologue), &trampoline)) {
    return false;
  }

  if (!WriteAbsoluteJump(target, reinterpret_cast<void*>(&HookedStartAddressTranslation), sizeof(kExpectedPrologue))) {
    return false;
  }

  g_start_address_translation_original = reinterpret_cast<StartAddressTranslationFn>(trampoline);
  InterlockedExchange(&g_address_translation_hook_active, 1);
  LogFormat(L"address translation diagnostic hook installed at %p", target);
  return true;
}

struct EeConnectApi {
  CExoStringFromCharFn construct_string = nullptr;
  CExoStringDestructorFn destroy_string = nullptr;
  CAppManagerConnectToServerFn connect_to_server = nullptr;
  void** app_manager_global = nullptr;
};

bool ResolveEeConnectApi(EeConnectApi* api) {
  if (api == nullptr) {
    return false;
  }

  api->construct_string = ResolveMainExport<CExoStringFromCharFn>("??0CExoString@@QEAA@PEBD@Z");
  api->destroy_string = ResolveMainExport<CExoStringDestructorFn>("??1CExoString@@QEAA@XZ");
  api->connect_to_server = ResolveMainExport<CAppManagerConnectToServerFn>("?ConnectToServer@CAppManager@@QEAAXVCExoString@@H@Z");
  api->app_manager_global = ResolveMainExport<void**>("?g_pAppManager@@3PEAVCAppManager@@EA");

  if (api->construct_string == nullptr || api->destroy_string == nullptr || api->connect_to_server == nullptr ||
      api->app_manager_global == nullptr) {
    LogFormat(
        L"auto-connect skipped: missing exports construct=%p destroy=%p connect=%p app_global=%p",
        api->construct_string,
        api->destroy_string,
        api->connect_to_server,
        api->app_manager_global);
    return false;
  }
  return true;
}

bool IsEeClientReady(void* app_manager) {
  if (app_manager == nullptr) {
    return false;
  }

  return GetClientExoAppInternalFromAppManager(app_manager) != nullptr;
}

enum class AutoConnectAttemptResult {
  kNotReady,
  kInvoked,
  kFailed,
};

bool InvokeEeConnectRaw(const EeConnectApi& api, void* app_manager, const char* host, unsigned short port) {
  CExoStringView address{};
  bool invoked = false;
  bool raised_exception = false;

  __try {
    api.construct_string(&address, host);
    api.connect_to_server(app_manager, &address, static_cast<int>(port));
    invoked = true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    raised_exception = true;
  }

  if (address.data != nullptr) {
    __try {
      api.destroy_string(&address);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      raised_exception = true;
    }
  }

  if (raised_exception) {
    LogFormat(L"auto-connect failed: exception while invoking or cleaning up CAppManager::ConnectToServer");
  }
  return invoked;
}

AutoConnectAttemptResult TryAutoConnect(const EeConnectApi& api, const std::wstring& host, unsigned short port, int attempt) {
  void* const app_manager = api.app_manager_global != nullptr ? *api.app_manager_global : nullptr;
  if (!IsEeClientReady(app_manager)) {
    if (attempt == 1 || attempt % 5 == 0) {
      LogFormat(L"auto-connect waiting for EE client app readiness (attempt %d)", attempt);
    }
    return AutoConnectAttemptResult::kNotReady;
  }

  const std::string host_utf8 = ToUtf8(host);
  if (host_utf8.empty()) {
    LogFormat(L"auto-connect failed: target host is empty after UTF-8 conversion");
    return AutoConnectAttemptResult::kFailed;
  }

  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY")) {
    LogFormat(L"driver-only: skipped Diamond CD key seeding before auto-connect");
  } else {
    SeedDiamondCdKeysIntoAppManager(app_manager, true);
  }

  LogFormat(L"auto-connect invoking CAppManager::ConnectToServer('%s', %hu) attempt=%d", host.c_str(), port, attempt);
  const bool invoked = InvokeEeConnectRaw(api, app_manager, host_utf8.c_str(), port);

  return invoked ? AutoConnectAttemptResult::kInvoked : AutoConnectAttemptResult::kFailed;
}

DWORD WINAPI AutoConnectThread(LPVOID) {
  const std::wstring host = g_auto_connect_host;
  const unsigned short port = g_auto_connect_port;
  const bool driver_only = IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY");
  if (host.empty() || port == 0) {
    LogFormat(L"auto-connect skipped: incomplete target host='%s' port=%hu", host.c_str(), port);
    return 0;
  }

  EeConnectApi api{};
  if (!ResolveEeConnectApi(&api)) {
    return 0;
  }

  Sleep(driver_only ? 10000 : 6000);
  constexpr int kMaxAttempts = 20;
  for (int attempt = 1; attempt <= kMaxAttempts; ++attempt) {
    if (InterlockedCompareExchange(&g_target_address_translation_seen, 0, 0) != 0) {
      LogFormat(L"auto-connect complete: target address translation was observed");
      return 1;
    }

    const AutoConnectAttemptResult result = TryAutoConnect(api, host, port, attempt);
    if (result == AutoConnectAttemptResult::kInvoked) {
      if (InterlockedCompareExchange(&g_address_translation_hook_active, 0, 0) == 0) {
        if (!driver_only) {
          LogFormat(L"auto-connect invoked once; address hook is disabled, so completion cannot be observed");
          return 1;
        }

        for (int wait = 0; wait < 16; ++wait) {
          Sleep(500);
          if (InterlockedCompareExchange(&g_server_to_player_message_observations, 0, 0) != 0 ||
              InterlockedCompareExchange(&g_auto_character_state, 0, 0) != 0) {
            LogFormat(L"auto-connect complete: driver-only server message observation was seen");
            return 1;
          }
        }
        LogFormat(L"auto-connect driver-only attempt %d did not observe server messages yet", attempt);
      } else {
        for (int wait = 0; wait < 12; ++wait) {
          Sleep(250);
          if (InterlockedCompareExchange(&g_target_address_translation_seen, 0, 0) != 0) {
            LogFormat(L"auto-connect complete: target address translation was observed");
            return 1;
          }
        }
        LogFormat(L"auto-connect attempt %d did not reach target address translation yet", attempt);
      }
    } else if (result == AutoConnectAttemptResult::kFailed) {
      return 0;
    }

    Sleep(1000);
  }

  LogFormat(L"auto-connect gave up after %d attempts without target address translation", kMaxAttempts);
  return 0;
}

bool StartAutoConnectWorker() {
  HANDLE thread = CreateThread(nullptr, 0, AutoConnectThread, nullptr, 0, nullptr);
  if (thread == nullptr) {
    LogFormat(L"auto-connect worker failed to start: error=%lu", GetLastError());
    return false;
  }

  CloseHandle(thread);
  LogFormat(L"auto-connect worker started for %s:%hu", g_auto_connect_host.c_str(), g_auto_connect_port);
  return true;
}

std::vector<uint8_t*> FindPatternInRange(uint8_t* begin, size_t size, const uint8_t* pattern, size_t pattern_size) {
  std::vector<uint8_t*> matches;
  if (begin == nullptr || pattern == nullptr || pattern_size == 0 || size < pattern_size) {
    return matches;
  }

  for (size_t offset = 0; offset <= size - pattern_size; ++offset) {
    if (std::equal(pattern, pattern + pattern_size, begin + offset)) {
      matches.push_back(begin + offset);
    }
  }
  return matches;
}

std::vector<uint8_t*> FindPatternInRange(uint8_t* begin, size_t size, const int* pattern, size_t pattern_size) {
  std::vector<uint8_t*> matches;
  if (begin == nullptr || pattern == nullptr || pattern_size == 0 || size < pattern_size) {
    return matches;
  }

  for (size_t offset = 0; offset <= size - pattern_size; ++offset) {
    bool matched = true;
    for (size_t index = 0; index < pattern_size; ++index) {
      if (pattern[index] >= 0 && begin[offset + index] != static_cast<uint8_t>(pattern[index])) {
        matched = false;
        break;
      }
    }

    if (matched) {
      matches.push_back(begin + offset);
    }
  }
  return matches;
}

std::vector<uint8_t*> FindPatternInMainExecutableSections(const uint8_t* pattern, size_t pattern_size) {
  std::vector<uint8_t*> matches;
  auto* const image_base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (image_base == nullptr) {
    LogFormat(L"pattern scan failed: GetModuleHandleW(nullptr) returned null");
    return matches;
  }

  const auto* const dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image_base);
  if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
    LogFormat(L"pattern scan failed: main module has invalid DOS signature");
    return matches;
  }

  const auto* const nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(image_base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    LogFormat(L"pattern scan failed: main module has invalid 64-bit PE header");
    return matches;
  }

  const DWORD image_size = nt->OptionalHeader.SizeOfImage;
  const auto* const sections = IMAGE_FIRST_SECTION(nt);
  for (WORD index = 0; index < nt->FileHeader.NumberOfSections; ++index) {
    const IMAGE_SECTION_HEADER& section = sections[index];
    if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) == 0) {
      continue;
    }

    const DWORD section_va = section.VirtualAddress;
    DWORD section_size = section.Misc.VirtualSize != 0 ? section.Misc.VirtualSize : section.SizeOfRawData;
    if (section_va >= image_size || section_size == 0) {
      continue;
    }

    section_size = std::min(section_size, image_size - section_va);
    std::vector<uint8_t*> section_matches = FindPatternInRange(image_base + section_va, section_size, pattern, pattern_size);
    matches.insert(matches.end(), section_matches.begin(), section_matches.end());
  }
  return matches;
}

std::vector<uint8_t*> FindPatternInMainExecutableSections(const int* pattern, size_t pattern_size) {
  std::vector<uint8_t*> matches;
  auto* const image_base = reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr));
  if (image_base == nullptr) {
    LogFormat(L"pattern scan failed: GetModuleHandleW(nullptr) returned null");
    return matches;
  }

  const auto* const dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image_base);
  if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
    LogFormat(L"pattern scan failed: main module has invalid DOS signature");
    return matches;
  }

  const auto* const nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(image_base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    LogFormat(L"pattern scan failed: main module has invalid 64-bit PE header");
    return matches;
  }

  const DWORD image_size = nt->OptionalHeader.SizeOfImage;
  const auto* const sections = IMAGE_FIRST_SECTION(nt);
  for (WORD index = 0; index < nt->FileHeader.NumberOfSections; ++index) {
    const IMAGE_SECTION_HEADER& section = sections[index];
    if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) == 0) {
      continue;
    }

    const DWORD section_va = section.VirtualAddress;
    DWORD section_size = section.Misc.VirtualSize != 0 ? section.Misc.VirtualSize : section.SizeOfRawData;
    if (section_va >= image_size || section_size == 0) {
      continue;
    }

    section_size = std::min(section_size, image_size - section_va);
    std::vector<uint8_t*> section_matches = FindPatternInRange(image_base + section_va, section_size, pattern, pattern_size);
    matches.insert(matches.end(), section_matches.begin(), section_matches.end());
  }
  return matches;
}

bool InstallDirectConnectPortRepairHook() {
  // EE 8193.37 direct-connect state update. The state object keeps the target
  // address as a CExoString at +0x08, the target port at +0x18, and the
  // password CExoString at +0x130.
  static constexpr uint8_t kPatchPrologue[] = {
      0x48,
      0x8B,
      0xC4,
      0x55,
      0x41,
      0x54,
      0x41,
      0x55,
      0x41,
      0x56,
      0x41,
      0x57,
      0x48,
      0x8D,
      0x6C,
      0x24,
      0x90,
      0x48,
      0x81,
      0xEC,
      0x70,
      0x01,
      0x00,
      0x00,
  };
  static constexpr int kSignature[] = {
      0x48, 0x8B, 0xC4, 0x55, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
      0x48, 0x8D, 0x6C, 0x24, 0x90, 0x48, 0x81, 0xEC, 0x70, 0x01, 0x00, 0x00,
      0x48, 0xC7, 0x45, 0x10, 0xFE, 0xFF, 0xFF, 0xFF, 0x48, 0x89, 0x58, 0x10,
      0x48, 0x89, 0x70, 0x18, 0x48, 0x89, 0x78, 0x20, 0x48, 0x8B, 0x05, -1,
      -1,   -1,   -1,   0x48, 0x33, 0xC4, 0x48, 0x89, 0x45, 0x68, 0x48, 0x8B,
      0xF1, 0x33, 0xDB, 0x38, 0x1D, -1,   -1,   -1,   -1,   0x0F, 0x84, 0x86,
      0x00, 0x00, 0x00,
  };

  std::vector<uint8_t*> matches =
      FindPatternInMainExecutableSections(kSignature, sizeof(kSignature) / sizeof(kSignature[0]));
  if (matches.size() != 1) {
    LogFormat(L"direct-connect port repair hook skipped: expected 1 signature match, found %zu", matches.size());
    return false;
  }

  uint8_t* const target = matches[0];
  if (!std::equal(kPatchPrologue, kPatchPrologue + sizeof(kPatchPrologue), target)) {
    LogFormat(L"direct-connect port repair hook skipped: unexpected prologue at %p", target);
    return false;
  }

  void* trampoline = nullptr;
  if (!CreateTrampoline(target, sizeof(kPatchPrologue), &trampoline)) {
    return false;
  }

  if (!WriteAbsoluteJump(target, reinterpret_cast<void*>(&HookedDirectConnectStateUpdate), sizeof(kPatchPrologue))) {
    return false;
  }

  g_direct_connect_state_update_original = reinterpret_cast<DirectConnectStateUpdateFn>(trampoline);
  LogFormat(L"direct-connect port repair hook installed at %p", target);
  return true;
}

bool HasStartEnumerateScalarPrologue(uint8_t* target) {
  return target != nullptr && target[0] == 0x48 && target[1] == 0x83 && target[2] == 0xEC && target[3] == 0x68 &&
         target[4] == 0x48 && target[5] == 0x8B && target[6] == 0x05 && target[11] == 0x48 && target[12] == 0x33 &&
         target[13] == 0xC4;
}

bool HasStartEnumerateArrayPrologue(uint8_t* target) {
  static constexpr uint8_t kExpected[] = {
      0x48, 0x83, 0xEC, 0x48, 0x41, 0x0F, 0x10, 0x01, 0x8B, 0x44, 0x24, 0x70,
  };
  return target != nullptr && std::equal(kExpected, kExpected + sizeof(kExpected), target);
}

bool InstallStartEnumerateSessionsPortRepairHooks() {
  static constexpr char kInternalSymbol[] = "?StartEnumerateSessions@CNetLayerInternal@@QEAAHQEAIHVCNetPeer@@H@Z";
  static constexpr char kScalarSymbol[] = "?StartEnumerateSessions@CNetLayer@@QEAAHIHVCNetPeer@@H@Z";
  static constexpr char kArraySymbol[] = "?StartEnumerateSessions@CNetLayer@@QEAAHQEAIHVCNetPeer@@H@Z";
  constexpr size_t kScalarPatchSize = 14;
  constexpr size_t kArrayPatchSize = 12;

  g_start_enumerate_sessions_internal = ResolveMainExport<StartEnumerateSessionsInternalFn>(kInternalSymbol);
  if (g_start_enumerate_sessions_internal == nullptr) {
    LogFormat(L"session peer port repair hooks skipped: internal StartEnumerateSessions export not found");
    return false;
  }

  auto* const scalar_target = ResolveMainExport<uint8_t*>(kScalarSymbol);
  auto* const array_target = ResolveMainExport<uint8_t*>(kArraySymbol);
  bool installed = false;

  if (scalar_target == nullptr) {
    LogFormat(L"session peer scalar hook skipped: export not found");
  } else if (!HasStartEnumerateScalarPrologue(scalar_target)) {
    LogFormat(L"session peer scalar hook skipped: unexpected prologue at %p", scalar_target);
  } else if (WriteAbsoluteJump(scalar_target, reinterpret_cast<void*>(&HookedStartEnumerateSessionsScalar), kScalarPatchSize)) {
    LogFormat(L"session peer scalar hook installed at %p", scalar_target);
    installed = true;
  }

  if (array_target == nullptr) {
    LogFormat(L"session peer array hook skipped: export not found");
  } else if (!HasStartEnumerateArrayPrologue(array_target)) {
    LogFormat(L"session peer array hook skipped: unexpected prologue at %p", array_target);
  } else if (WriteAbsoluteJump(array_target, reinterpret_cast<void*>(&HookedStartEnumerateSessionsArray), kArrayPatchSize)) {
    LogFormat(L"session peer array hook installed at %p", array_target);
    installed = true;
  }

  return installed;
}

bool PatchLegacyStartConnectHandshake() {
  // EE 8193.37 StartConnectToSession normally sends a BNK key-exchange packet
  // when the connection library is active. A 1.69 server expects the legacy BNCS
  // login packet directly, so force the second "connection library active" test
  // to take the BNCS branch while leaving session lookup intact.
  static constexpr int kPattern[] = {
      0xFF, 0x90, 0xC0, 0x00, 0x00, 0x00,  // call qword ptr [rax+0C0h]
      0x85, 0xC0,                          // test eax,eax
      0x0F, 0x84, -1, -1, -1, -1,          // jz legacy BNCS path
      0x83, 0xBE, 0xA8, 0x56, 0x8A, 0x02, 0x01,
  };
  constexpr size_t kJccOffset = 8;

  std::vector<uint8_t*> matches = FindPatternInMainExecutableSections(kPattern, sizeof(kPattern) / sizeof(kPattern[0]));
  if (matches.size() != 1) {
    LogFormat(L"legacy StartConnect BNCS patch skipped: expected 1 signature match, found %zu", matches.size());
    return false;
  }

  uint8_t* const jcc = matches[0] + kJccOffset;
  if (jcc[0] != 0x0F || jcc[1] != 0x84) {
    LogFormat(L"legacy StartConnect BNCS patch skipped: unexpected conditional jump at %p bytes=[%s]", jcc, FormatBytes(jcc, 6).c_str());
    return false;
  }

  const int32_t old_displacement = *reinterpret_cast<int32_t*>(jcc + 2);
  uint8_t* const target = jcc + 6 + old_displacement;
  const intptr_t new_displacement_wide = target - (jcc + 5);
  if (new_displacement_wide < INT32_MIN || new_displacement_wide > INT32_MAX) {
    LogFormat(L"legacy StartConnect BNCS patch skipped: branch target is out of range");
    return false;
  }

  DWORD old_protect = 0;
  if (!VirtualProtect(jcc, 6, PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"legacy StartConnect BNCS patch failed: VirtualProtect(%p) error=%lu", jcc, GetLastError());
    return false;
  }

  jcc[0] = 0xE9;
  *reinterpret_cast<int32_t*>(jcc + 1) = static_cast<int32_t>(new_displacement_wide);
  jcc[5] = 0x90;

  DWORD ignored_protect = 0;
  VirtualProtect(jcc, 6, old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), jcc, 6);

  LogFormat(L"legacy StartConnect BNCS patch installed at %p -> %p", jcc, target);
  return true;
}

bool PatchLegacyStartConnectConnectionState() {
  // In the same forced legacy StartConnect branch, EE still seeds the active
  // connection/sliding-window fields with SINGLEPLAYER. The BNCS packet itself
  // uses r15d, which is the real session connection id, so make the state match
  // that or the following game packets are emitted on connection 0.
  static constexpr int kPattern[] = {
      0x8B, 0x15, -1, -1, -1, -1,              // mov edx, CNetConnectionId::SINGLEPLAYER
      0x4C, 0x8D, 0x86, 0xD8, 0x56, 0x8A, 0x02,  // lea r8, [rsi+28A56D8h]
      0x48, 0x8B, 0xCE,                        // mov rcx, rsi
      0x89, 0x96, 0xD0, 0x56, 0x8A, 0x02,      // mov [rsi+28A56D0h], edx
      0xE8, -1, -1, -1, -1,                    // call SetSlidingWindow
      0x4C, 0x8B, 0x44, 0x24, -1,              // mov r8, [rsp+...]
      0x41, 0x8B, 0xD7,                        // mov edx, r15d
      0x8B, 0x86, 0xD8, 0x56, 0x8A, 0x02,      // mov eax, [rsi+28A56D8h]
  };

  std::vector<uint8_t*> matches = FindPatternInMainExecutableSections(kPattern, sizeof(kPattern) / sizeof(kPattern[0]));
  if (matches.size() != 1) {
    LogFormat(L"legacy StartConnect connection-state patch skipped: expected 1 signature match, found %zu", matches.size());
    return false;
  }

  uint8_t* const mov_singleplayer = matches[0];
  if (mov_singleplayer[0] != 0x8B || mov_singleplayer[1] != 0x15) {
    LogFormat(
        L"legacy StartConnect connection-state patch skipped: unexpected instruction at %p bytes=[%s]",
        mov_singleplayer,
        FormatBytes(mov_singleplayer, 6).c_str());
    return false;
  }

  DWORD old_protect = 0;
  if (!VirtualProtect(mov_singleplayer, 6, PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"legacy StartConnect connection-state patch failed: VirtualProtect(%p) error=%lu", mov_singleplayer, GetLastError());
    return false;
  }

  // mov edx, r15d; nop; nop; nop
  mov_singleplayer[0] = 0x41;
  mov_singleplayer[1] = 0x8B;
  mov_singleplayer[2] = 0xD7;
  mov_singleplayer[3] = 0x90;
  mov_singleplayer[4] = 0x90;
  mov_singleplayer[5] = 0x90;

  DWORD ignored_protect = 0;
  VirtualProtect(mov_singleplayer, 6, old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), mov_singleplayer, 6);

  LogFormat(L"legacy StartConnect connection-state patch installed at %p: sliding window now uses session connection id", mov_singleplayer);
  return true;
}

bool PatchLegacyBncsEncryptionClassifier() {
  // EE's UDP sender encrypts BNC*, BNV*, and the normal gameplay packet stream.
  // The forced legacy StartConnect path never completes EE's BNK crypto
  // exchange, so classify legacy BN* control packets and non-B* gameplay
  // frames as plain UDP.
  static constexpr int kPattern[] = {
      0x41, 0x83, 0xFC, 0x04,        // cmp r12d, 4
      0x72, -1,                      // jb encrypted
      0x80, 0x3F, 0x42,              // cmp byte ptr [rdi], 'B'
      0x75, -1,                      // jnz encrypted
      0x80, 0x7F, 0x01, 0x4E,        // cmp byte ptr [rdi+1], 'N'
      0x75, -1,                      // jnz encrypted
      0x0F, 0xB6, 0x47, 0x02,        // movzx eax, byte ptr [rdi+2]
      0x3C, 0x43,                    // cmp al, 'C'
      0x74, -1,                      // jz encrypted
      0x3C, 0x56,                    // cmp al, 'V'
      0x41, 0x8B, 0xC6,              // mov eax, r14d
      0x75, -1,                      // jnz plain
      0xB8, 0x01, 0x00, 0x00, 0x00,  // mov eax, 1
  };
  constexpr size_t kNonBJumpOffset = 9;
  constexpr size_t kBncJumpOffset = 23;
  constexpr size_t kBnvCompareImmediateOffset = 26;
  constexpr uint8_t kJumpToPlainFromNonB = 0x10;

  std::vector<uint8_t*> matches = FindPatternInMainExecutableSections(kPattern, sizeof(kPattern) / sizeof(kPattern[0]));
  if (matches.size() != 1) {
    LogFormat(L"legacy BNCS encryption classifier patch skipped: expected 1 signature match, found %zu", matches.size());
    return false;
  }

  uint8_t* const non_b_jump = matches[0] + kNonBJumpOffset;
  uint8_t* const jump = matches[0] + kBncJumpOffset;
  if (non_b_jump[0] != 0x75) {
    LogFormat(
        L"legacy BNCS encryption classifier patch skipped: unexpected non-B jump at %p bytes=[%s]",
        non_b_jump,
        FormatBytes(non_b_jump, 2).c_str());
    return false;
  }
  if (jump[0] != 0x74) {
    LogFormat(L"legacy BNCS encryption classifier patch skipped: unexpected jump at %p bytes=[%s]", jump, FormatBytes(jump, 2).c_str());
    return false;
  }

  DWORD old_protect = 0;
  if (!VirtualProtect(matches[0], sizeof(kPattern) / sizeof(kPattern[0]), PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"legacy BNCS encryption classifier patch failed: VirtualProtect(%p) error=%lu", matches[0], GetLastError());
    return false;
  }

  // Jump to the existing "mov eax, r14d" instruction, not directly to the
  // shared store. The old comparison flags are intentionally still live so
  // the following jnz reaches the plain-send block with eax=0.
  non_b_jump[1] = kJumpToPlainFromNonB;
  jump[0] = 0x90;
  jump[1] = 0x90;
  // Diamond sends BNVS/BNVR as plain legacy control packets. Make the EE
  // BNx classifier treat the V-family the same way after the BNC jump above.
  matches[0][kBnvCompareImmediateOffset] = 0x00;

  DWORD ignored_protect = 0;
  VirtualProtect(matches[0], sizeof(kPattern) / sizeof(kPattern[0]), old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), matches[0], sizeof(kPattern) / sizeof(kPattern[0]));

  LogFormat(L"legacy BNCS encryption classifier patch installed at %p: BN* and non-B* now use legacy plain UDP", matches[0]);
  return true;
}

bool PatchLegacyReceiveEncryptionClassifier() {
  // The receive side has its own small PacketRequiresEncryption helper. Without
  // this patch, EE tries to decrypt plain 1.69 BNCR responses and drops them
  // before CNetLayerInternal::NonWindowMessages can dispatch HandleBNCR.
  static constexpr char kSymbol[] = "?PacketRequiresEncryption@CExoNetInternal@@QEBAHPEBE_K@Z";
  static constexpr uint8_t kExpected[] = {
      0x49, 0x83, 0xF8, 0x04,              // cmp r8, 4
      0x72, 0x1A,                          // jb encrypted
      0x80, 0x3A, 0x42,                    // cmp byte ptr [rdx], 'B'
      0x75, 0x15,                          // jnz encrypted
      0x80, 0x7A, 0x01, 0x4E,              // cmp byte ptr [rdx+1], 'N'
      0x75, 0x0F,                          // jnz encrypted
      0x0F, 0xB6, 0x42, 0x02,              // movzx eax, byte ptr [rdx+2]
      0x3C, 0x43,                          // cmp al, 'C'
      0x74, 0x07,                          // jz encrypted
      0x3C, 0x56,                          // cmp al, 'V'
      0x74, 0x03,                          // jz encrypted
      0x33, 0xC0,                          // xor eax, eax
      0xC3,                                // retn
      0xB8, 0x01, 0x00, 0x00, 0x00,        // mov eax, 1
      0xC3,                                // retn
  };
  constexpr size_t kNonBJumpOffset = 9;
  constexpr size_t kNonBnJumpOffset = 15;
  constexpr size_t kBncJumpOffset = 23;
  constexpr size_t kBnvJumpOffset = 27;
  constexpr uint8_t kJumpToPlainFromNonB = 0x12;
  constexpr uint8_t kJumpToPlainFromNonBn = 0x0C;
  constexpr uint8_t kJumpToPlainFromBnc = 0x04;

  auto* const target = ResolveMainExport<uint8_t*>(kSymbol);
  if (target == nullptr) {
    LogFormat(L"legacy receive encryption classifier patch skipped: export not found");
    return false;
  }
  if (!std::equal(kExpected, kExpected + sizeof(kExpected), target)) {
    LogFormat(
        L"legacy receive encryption classifier patch skipped: unexpected helper bytes at %p actual=[%s]",
        target,
        FormatBytes(target, sizeof(kExpected)).c_str());
    return false;
  }

  DWORD old_protect = 0;
  if (!VirtualProtect(target, sizeof(kExpected), PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"legacy receive encryption classifier patch failed: VirtualProtect(%p) error=%lu", target, GetLastError());
    return false;
  }

  target[kNonBJumpOffset + 1] = kJumpToPlainFromNonB;
  target[kNonBnJumpOffset + 1] = kJumpToPlainFromNonBn;
  target[kBncJumpOffset + 1] = kJumpToPlainFromBnc;
  target[kBnvJumpOffset] = 0x90;
  target[kBnvJumpOffset + 1] = 0x90;

  DWORD ignored_protect = 0;
  VirtualProtect(target, sizeof(kExpected), old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), target, sizeof(kExpected));

  LogFormat(L"legacy receive encryption classifier patch installed at %p: BN* and non-BN* now arrive as plain UDP", target);
  return true;
}

bool PatchClientDevicePropertiesUpdate() {
  // EE advertises local renderer/UI settings to EE servers as player-device
  // properties (message 0x3601 / Device_AdvertiseProperty). Diamond has no
  // matching protocol, and a 1.69 server can see these before the character-list
  // login flow. Patch the producer instead of dropping packetized frames later,
  // so the reliable stream sequence never allocates these EE-only messages.
  static constexpr int kPattern[] = {
      0x48, 0x8B, 0xC4,                          // mov rax,rsp
      0x55,                                      // push rbp
      0x41, 0x54,                                // push r12
      0x41, 0x55,                                // push r13
      0x41, 0x56,                                // push r14
      0x41, 0x57,                                // push r15
      0x48, 0x8D, 0x68, 0xA1,                    // lea rbp,[rax-5Fh]
      0x48, 0x81, 0xEC, 0xE0, 0x00, 0x00, 0x00,  // sub rsp,0E0h
      0x48, 0xC7, 0x45, 0xAF, 0xFE, 0xFF, 0xFF, 0xFF,
      0x48, 0x89, 0x58, 0x10,                    // mov [rax+10h],rbx
      0x48, 0x89, 0x70, 0x18,                    // mov [rax+18h],rsi
      0x48, 0x89, 0x78, 0x20,                    // mov [rax+20h],rdi
      0x48, 0x8B, 0x05, -1, -1, -1, -1,          // mov rax, __security_cookie
      0x48, 0x33, 0xC4,                          // xor rax,rsp
      0x48, 0x89, 0x45, 0x27,                    // mov [rbp+27h],rax
      0x48, 0x8B, 0xD9,                          // mov rbx,rcx
      0x45, 0x33, 0xFF,                          // xor r15d,r15d
      0x44, 0x38, 0x3D, -1, -1, -1, -1,          // cmp g_bTracyEnabled,r15b
      0x0F, 0x84, 0x88, 0x00, 0x00, 0x00,        // jz after tracy scope
  };

  std::vector<uint8_t*> matches = FindPatternInMainExecutableSections(kPattern, sizeof(kPattern) / sizeof(kPattern[0]));
  if (matches.size() != 1) {
    LogFormat(L"client device-properties patch skipped: expected 1 signature match, found %zu", matches.size());
    return false;
  }

  uint8_t* const target = matches[0];
  if (target[0] != 0x48 || target[1] != 0x8B || target[2] != 0xC4) {
    LogFormat(
        L"client device-properties patch skipped: unexpected prologue at %p bytes=[%s]",
        target,
        FormatBytes(target, 16).c_str());
    return false;
  }

  DWORD old_protect = 0;
  if (!VirtualProtect(target, 1, PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"client device-properties patch failed: VirtualProtect(%p) error=%lu", target, GetLastError());
    return false;
  }

  target[0] = 0xC3;  // ret

  DWORD ignored_protect = 0;
  VirtualProtect(target, 1, old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), target, 1);

  LogFormat(L"client device-properties patch installed at %p: EE Device_AdvertiseProperty messages disabled", target);
  return true;
}

bool InstallStartConnectToSessionWrapperHook() {
  static constexpr char kWrapperSymbol[] = "?StartConnectToSession@CNetLayer@@QEAAHIAEBVCExoString@@H0II000@Z";
  static constexpr char kInternalSymbol[] = "?StartConnectToSession@CNetLayerInternal@@QEAAHIAEBVCExoString@@H0II000@Z";
  constexpr size_t kPatchSize = 12;

  g_start_connect_to_session_internal_original = ResolveMainExport<StartConnectToSessionInternalFn>(kInternalSymbol);
  if (g_start_connect_to_session_internal_original == nullptr) {
    LogFormat(L"StartConnectToSession wrapper hook skipped: internal export not found");
    return false;
  }

  auto* const target = ResolveMainExport<uint8_t*>(kWrapperSymbol);
  if (target == nullptr) {
    LogFormat(L"StartConnectToSession wrapper hook skipped: wrapper export not found");
    return false;
  }

  const bool has_expected_wrapper = target[0] == 0x48 && target[1] == 0x8B && target[2] == 0x49 && target[3] == 0x08 && target[4] == 0xE9;
  if (!has_expected_wrapper) {
    LogFormat(L"StartConnectToSession wrapper hook skipped: unexpected wrapper prologue at %p actual=[%s]", target, FormatBytes(target, kPatchSize).c_str());
    return false;
  }

  if (!WriteAbsoluteJump(target, reinterpret_cast<void*>(&HookedStartConnectToSessionWrapper), kPatchSize)) {
    LogFormat(L"StartConnectToSession wrapper hook skipped: patch failed");
    return false;
  }

  LogFormat(L"StartConnectToSession wrapper hook installed at %p", target);
  return true;
}

bool InstallNetFlowDiagnosticHooks() {
  bool installed = false;

  {
    static constexpr char kSymbol[] = "?OpenStandardConnection@CNetLayerInternal@@QEAAHHVCExoString@@H@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x4C, 0x89, 0x44, 0x24, 0x18, 0x56, 0x57, 0x41, 0x56, 0x48, 0x83, 0xEC, 0x40,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedOpenStandardConnectionInternal),
        reinterpret_cast<void**>(&g_open_standard_connection_internal_original),
        L"OpenStandardConnection diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?SendDirectMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@PEAEII@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x8D, 0xAC, 0x24, 0x10, 0xFF, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0xF0, 0x01, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedSendDirectMessage),
        reinterpret_cast<void**>(&g_send_direct_message_original),
        L"SendDirect diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?SendMessageA@CExoNetInternal@@QEAAHUCNetConnectionId@@PEAEII@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x8D, 0xAC, 0x24, 0x70, 0xFE, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0x90, 0x02, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedExoNetSendMessage),
        reinterpret_cast<void**>(&g_exonet_send_message_original),
        L"ExoNet SendMessageA diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?SetNetworkAddressData@CExoNet@@QEAA?AUCNetConnectionId@@AEBVCNetPeer@@W4RelayModeChange@@QEBD@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x53, 0x48, 0x83, 0xEC, 0x30, 0x48, 0x8B, 0x44, 0x24, 0x60,
        0x48, 0x8B, 0xDA, 0x48, 0x8B, 0x49, 0x08,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedSetNetworkAddressData),
        reinterpret_cast<void**>(&g_set_network_address_data_original),
        L"SetNetworkAddressData diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?NonWindowMessages@CNetLayerInternal@@QEAAHUCNetConnectionId@@PEAEI@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x48, 0x8B, 0xC4, 0x44, 0x89, 0x48, 0x20, 0x4C, 0x89, 0x40, 0x18,
        0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x81, 0xEC, 0x60, 0x0E, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedNonWindowMessages),
        reinterpret_cast<void**>(&g_non_window_messages_original),
        L"NonWindowMessages diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?HandleBNERMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@AEBV?$shared_ptr@VDataViewStream@@@std@@@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x8D, 0x6C, 0x24, 0xD9, 0x48, 0x81, 0xEC, 0xE0, 0x00, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedHandleBnerMessage),
        reinterpret_cast<void**>(&g_handle_bner_message_original),
        L"HandleBNER diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?HandleBNCRMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@AEBV?$shared_ptr@VDataViewStream@@@std@@@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x8D, 0xAC, 0x24, 0xC0, 0xFE, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0x40, 0x02, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedHandleBncrMessage),
        reinterpret_cast<void**>(&g_handle_bncr_message_original),
        L"HandleBNCR diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?HandleBNVRMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@AEBV?$shared_ptr@VDataViewStream@@@std@@@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x48, 0x89, 0x5C, 0x24, 0x08, 0x55, 0x56, 0x57, 0x41, 0x54,
        0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x83, 0xEC, 0x40,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedHandleBnvrMessage),
        reinterpret_cast<void**>(&g_handle_bnvr_message_original),
        L"HandleBNVR diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?HandleBNVSMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@AEBV?$shared_ptr@VDataViewStream@@@std@@@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x8D, 0xAC, 0x24, 0x10, 0xFE, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0xF0, 0x02, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedHandleBnvsMessage),
        reinterpret_cast<void**>(&g_handle_bnvs_message_original),
        L"HandleBNVS diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?HandleBNXRMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@AEBV?$shared_ptr@VDataViewStream@@@std@@@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x8D, 0xAC, 0x24, 0x30, 0xFE, 0xFF, 0xFF,
        0x48, 0x81, 0xEC, 0xD0, 0x02, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedHandleBnxrMessage),
        reinterpret_cast<void**>(&g_handle_bnxr_message_original),
        L"HandleBNXR diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?SendBNCRMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@EI@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x40, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x8D, 0x6C, 0x24, 0xC0, 0x48, 0x81, 0xEC, 0x40, 0x01, 0x00, 0x00,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedSendBncrMessage),
        reinterpret_cast<void**>(&g_send_bncr_message_original),
        L"SendBNCR diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?SendBNCSMessage@CNetLayerInternal@@QEAAHUCNetConnectionId@@EHAEBVCExoString@@11@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x48, 0x8B, 0xC4, 0x57, 0x41, 0x56, 0x41, 0x57, 0x48, 0x83, 0xEC, 0x50,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedSendBncsMessage),
        reinterpret_cast<void**>(&g_send_bncs_message_original),
        L"SendBNCS diagnostic");
  }

  {
    static constexpr char kSymbol[] = "?SendBNVSMessage@CNetLayerInternal@@QEAAHAEBVCExoString@@00@Z";
    static constexpr uint8_t kExpectedPrologue[] = {
        0x48, 0x89, 0x5C, 0x24, 0x20, 0x41, 0x54, 0x41,
        0x56, 0x41, 0x57, 0x48, 0x83, 0xEC, 0x40,
    };
    installed |= InstallExportHook(
        kSymbol,
        kExpectedPrologue,
        sizeof(kExpectedPrologue),
        reinterpret_cast<void*>(&HookedSendBnvsMessage),
        reinterpret_cast<void**>(&g_send_bnvs_message_original),
        L"SendBNVS diagnostic");
  }

  {
    const bool wrapper_installed = InstallStartConnectToSessionWrapperHook();
    installed |= wrapper_installed;
    if (!wrapper_installed) {
      static constexpr char kSymbol[] = "?StartConnectToSession@CNetLayerInternal@@QEAAHIAEBVCExoString@@H0II000@Z";
      static constexpr uint8_t kExpectedPrologue[] = {
          0x53, 0x56, 0x57, 0x41, 0x55, 0x41, 0x57, 0x48, 0x81, 0xEC, 0xA0, 0x00, 0x00, 0x00,
      };
      installed |= InstallExportHook(
          kSymbol,
          kExpectedPrologue,
          sizeof(kExpectedPrologue),
          reinterpret_cast<void*>(&HookedStartConnectToSessionInternal),
          reinterpret_cast<void**>(&g_start_connect_to_session_internal_original),
          L"StartConnectToSession diagnostic");
    }
  }

  return installed;
}

bool PatchBncsBuildField() {
  // EE 8193.37 CNetLayerInternal::SendBNCSMessage:
  //   66 41 C7 46 0B 03 00    mov word ptr [r14+0Bh], 3
  //   41 88 7E 0D             mov [r14+0Dh], dil
  static constexpr uint8_t kPattern[] = {
      0x66, 0x41, 0xC7, 0x46, 0x0B, 0x03, 0x00, 0x41, 0x88, 0x7E, 0x0D,
  };
  constexpr size_t kImmediateOffset = 5;

  std::vector<uint8_t*> matches = FindPatternInMainExecutableSections(kPattern, sizeof(kPattern));
  if (matches.size() != 1) {
    LogFormat(L"BNCS build-field patch failed: expected 1 signature match, found %zu", matches.size());
    return false;
  }

  uint8_t* const patch_site = matches[0] + kImmediateOffset;
  if (patch_site[0] != static_cast<uint8_t>(kEeBuildField & 0xFF) ||
      patch_site[1] != static_cast<uint8_t>((kEeBuildField >> 8) & 0xFF)) {
    LogFormat(L"BNCS build-field patch failed: unexpected current value at %p", patch_site);
    return false;
  }

  DWORD old_protect = 0;
  if (!VirtualProtect(patch_site, sizeof(kSinfarBncsBuildField), PAGE_EXECUTE_READWRITE, &old_protect)) {
    LogFormat(L"BNCS build-field patch failed: VirtualProtect error=%lu", GetLastError());
    return false;
  }

  patch_site[0] = static_cast<uint8_t>(kSinfarBncsBuildField & 0xFF);
  patch_site[1] = static_cast<uint8_t>((kSinfarBncsBuildField >> 8) & 0xFF);

  DWORD ignored_protect = 0;
  VirtualProtect(patch_site, sizeof(kSinfarBncsBuildField), old_protect, &ignored_protect);
  FlushInstructionCache(GetCurrentProcess(), patch_site, sizeof(kSinfarBncsBuildField));

  LogFormat(
      L"BNCS build-field patch installed at %p: 0x%04X -> 0x%04X",
      patch_site,
      kEeBuildField,
      kSinfarBncsBuildField);
  return true;
}

DWORD WINAPI BridgeMain(LPVOID) {
  if (InterlockedCompareExchange(&g_state.initialized, 1, 0) != 0) {
    return 1;
  }

  const DWORD pid = GetCurrentProcessId();
  const std::wstring module_path = GetModulePath(g_state.module);
  const std::wstring server_id = GetEnvironmentString(L"HG_BRIDGE_SERVER_ID");
  const std::wstring server_address = GetEnvironmentString(L"HG_BRIDGE_SERVER_ADDRESS");
  const std::wstring server_host = GetEnvironmentString(L"HG_BRIDGE_SERVER_HOST");
  const std::wstring server_port_text = GetEnvironmentString(L"HG_BRIDGE_SERVER_PORT");
  const std::wstring auto_password_text = GetEnvironmentString(L"HG_BRIDGE_AUTO_PASSWORD");
  const std::wstring auto_password_source = GetEnvironmentString(L"HG_BRIDGE_AUTO_PASSWORD_SOURCE");
  const std::wstring auto_character_text = GetEnvironmentString(L"HG_BRIDGE_AUTO_CHARACTER");
  const std::wstring auto_use_id = GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_ID");
  const std::wstring auto_use_type = GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_TYPE");
  const std::wstring auto_use_name = GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_NAME");
  const std::wstring auto_use_delay =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS");
  const std::wstring auto_use_second_id =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID");
  const std::wstring auto_use_second_type =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE");
  const std::wstring auto_use_second_name =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME");
  const std::wstring auto_use_second_delay =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS");
  const std::wstring auto_open_inventory_delay =
      GetEnvironmentStringWithAlias(L"HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS");
  const std::wstring workspace = GetEnvironmentString(L"HG_BRIDGE_WORKSPACE");
  const std::wstring asset_bundle = GetEnvironmentString(L"HG_BRIDGE_ASSET_BUNDLE");
  const std::wstring diamond_root = GetEnvironmentString(L"HG_BRIDGE_DIAMOND_ROOT");
  const std::wstring hg_asset_root = GetEnvironmentString(L"HG_BRIDGE_HG_ASSET_ROOT");
  const bool driver_only = IsTruthyEnvironmentFlag(L"HG_BRIDGE_DRIVER_ONLY");

  LogFormat(L"nwncx_hg v%s loaded in pid=%lu module=%s", kBridgeVersion, pid, module_path.c_str());
  LogFormat(
      L"launcher context: server_id=%s server_address=%s workspace=%s asset_bundle=%s diamond_root=%s hg_asset_root=%s",
      server_id.c_str(),
      server_address.c_str(),
      workspace.c_str(),
      asset_bundle.c_str(),
      diamond_root.c_str(),
      hg_asset_root.c_str());
  if (driver_only) {
    LogFormat(L"driver-only mode enabled: proxy owns compatibility; hooks are limited to launch/test driving");
    if (!auto_use_id.empty() || !auto_use_type.empty() || !auto_use_name.empty() ||
        !auto_use_second_id.empty() || !auto_use_second_type.empty() || !auto_use_second_name.empty()) {
      LogFormat(
          L"driver-only auto-use config: stage0={id='%s' type='%s' name='%s' delay='%s'} stage1={id='%s' type='%s' name='%s' delay='%s'} allow-unuseable=%d transition-click=%d trigger-walk-probe=%d",
          auto_use_id.c_str(),
          auto_use_type.c_str(),
          auto_use_name.c_str(),
          auto_use_delay.c_str(),
          auto_use_second_id.c_str(),
          auto_use_second_type.c_str(),
          auto_use_second_name.c_str(),
          auto_use_second_delay.c_str(),
          IsTruthyEnvironmentFlagWithAlias(L"HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE") ? 1 : 0,
          IsTruthyEnvironmentFlagWithAlias(L"HG_BRIDGE_AUTO_USE_TRANSITION_CLICK") ? 1 : 0,
          IsTruthyEnvironmentFlagWithAlias(L"HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE") ? 1 : 0);
    }
    if (DriverAutoOpenInventoryEnabled()) {
      LogFormat(
          L"driver-only auto-inventory enabled: delay-ms='%s'",
          auto_open_inventory_delay.empty() ? L"<default>" : auto_open_inventory_delay.c_str());
    }
  }

  const hgbridge::HgServer* server = hgbridge::FindServerByAddress(server_address);
  if (server != nullptr) {
    LogFormat(L"matched HG endpoint id=%s address=%s", std::wstring(server->id).c_str(), std::wstring(server->address).c_str());
    unsigned short parsed_port = 0;
    g_auto_connect_host = !server_host.empty() ? server_host : std::wstring(server->ip);
    g_auto_connect_port = ParseUnsignedShort(server_port_text, &parsed_port) ? parsed_port : server->port;
    g_auto_connect_ip_known = ParseIpv4Bytes(g_auto_connect_host, g_auto_connect_ip);
    if (!g_auto_connect_ip_known) {
      LogFormat(L"session peer port repair warning: target host is not an IPv4 literal: %s", g_auto_connect_host.c_str());
    }
    if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_PASSWORD")) {
      LogFormat(L"auto-connect password seed skipped: HG_BRIDGE_DISABLE_AUTO_PASSWORD is set");
    } else {
      g_auto_connect_password = ToUtf8(auto_password_text.empty() ? L"A" : auto_password_text);
      g_auto_connect_password_source = ToUtf8(auto_password_source.empty() ? L"default" : auto_password_source);
      LogFormat(
          L"auto-connect password seed enabled: source=%s length=%zu",
          NarrowToWide(g_auto_connect_password_source).c_str(),
          g_auto_connect_password.size());
    }

    g_auto_character_resref = NormalizeCharacterResref(auto_character_text);
    if (!auto_character_text.empty() && g_auto_character_resref.empty()) {
      LogFormat(
          L"auto-character skipped: invalid HG_BRIDGE_AUTO_CHARACTER='%s' (use the server vault filename/resref, max 16 chars)",
          auto_character_text.c_str());
    } else if (!g_auto_character_resref.empty()) {
      LogFormat(
          L"auto-character enabled: target='%s'",
          NarrowToWide(g_auto_character_resref).c_str());
    }
  } else {
    LogFormat(L"server address is not in the known HG endpoint table");
  }

  if (driver_only) {
    LogFormat(
        L"driver-only: skipped compatibility hooks: legacy BNCS, BNK/packet classifier, module/resource rewrites, TLK, device-property, live-object, Diamond CD key loading, asset mounting diagnostics, packet diagnostics, and identity seed worker");
    InstallDriverOnlyModuleResourceDiagnostics();
    InstallDriverOnlyAutoQuitConfirmHook();
    InstallDriverOnlyNetLayerDiagnostics();
    if (DriverOnlyWinsockDiagnosticsEnabled()) {
      if (InstallWinsockSendToHook()) {
        LogFormat(L"driver-only Winsock diagnostics enabled; raw socket logging only, no packet compatibility rewrites are active");
      } else {
        LogFormat(L"driver-only Winsock diagnostics requested but hook installation failed");
      }
    }
    if (!g_auto_character_resref.empty() || DriverAutoOpenInventoryEnabled()) {
      InstallDriverOnlyAutoCharacterHooks();
    }
    if (server == nullptr) {
      LogFormat(L"auto-connect skipped: launcher target is not a known HG endpoint");
    } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_CONNECT")) {
      LogFormat(L"auto-connect skipped: HG_BRIDGE_DISABLE_AUTO_CONNECT is set");
    } else {
      StartAutoConnectWorker();
    }
    return 1;
  }

  if (server != nullptr) {
    LoadDiamondCdKeys(workspace);
    LogAssetDiagnostics(workspace);
  }

  bool initialized = true;
  bool patch_installed = false;
  if (server == nullptr) {
    LogFormat(L"Sinfar BNCS build-field patch skipped: launcher target is not a known HG endpoint");
  } else if (!IsTruthyEnvironmentFlag(L"HG_BRIDGE_ENABLE_SINFAR_BNCS_FIELD_PATCH")) {
    LogFormat(L"Sinfar BNCS build-field patch skipped: HG_BRIDGE_ENABLE_SINFAR_BNCS_FIELD_PATCH is not set; legacy BNCS uses Diamond field 0x%04X", kDiamondBncsBuildField);
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_VERSION_PATCH")) {
    LogFormat(L"Sinfar BNCS build-field patch skipped: HG_BRIDGE_DISABLE_VERSION_PATCH is set");
  } else {
    patch_installed = PatchBncsBuildField();
    initialized = patch_installed;
  }

  if (!initialized) {
    LogFormat(L"bridge initialization failed");
    return 0;
  }

  if (patch_installed) {
    LogFormat(L"hook status: Sinfar BNCS version-field patch is active");
  } else {
    LogFormat(L"hook status: no Sinfar BNCS version-field patch installed for this launch context");
  }
  if (server == nullptr) {
    LogFormat(L"legacy StartConnect BNCS patch skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_CONNECT_PATCH")) {
    LogFormat(L"legacy StartConnect BNCS patch skipped: HG_BRIDGE_DISABLE_LEGACY_CONNECT_PATCH is set");
  } else {
    PatchLegacyStartConnectHandshake();
  }
  if (server == nullptr) {
    LogFormat(L"legacy StartConnect connection-state patch skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_LEGACY_CONNECT_STATE_PATCH")) {
    LogFormat(L"legacy StartConnect connection-state patch skipped: HG_BRIDGE_DISABLE_LEGACY_CONNECT_STATE_PATCH is set");
  } else {
    PatchLegacyStartConnectConnectionState();
  }
  if (server == nullptr) {
    LogFormat(L"legacy BNCS encryption classifier patch skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_BNCS_ENCRYPTION_PATCH")) {
    LogFormat(L"legacy BNCS encryption classifier patch skipped: HG_BRIDGE_DISABLE_BNCS_ENCRYPTION_PATCH is set");
  } else {
    PatchLegacyBncsEncryptionClassifier();
    PatchLegacyReceiveEncryptionClassifier();
  }
  if (server == nullptr) {
    LogFormat(L"client device-properties patch skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DEVICE_PROPERTIES_PATCH")) {
    LogFormat(L"client device-properties patch skipped: HG_BRIDGE_DISABLE_DEVICE_PROPERTIES_PATCH is set");
  } else {
    PatchClientDevicePropertiesUpdate();
  }
  if (server == nullptr) {
    LogFormat(L"client module-load compatibility hooks skipped: launcher target is not a known HG endpoint");
  } else {
    InstallEeModuleCompatibilityHooks();
  }
  if (server == nullptr) {
    LogFormat(L"CTlkTable diagnostic hook skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_TLK_DIAGNOSTICS")) {
    LogFormat(L"CTlkTable diagnostic hook skipped: HG_BRIDGE_DISABLE_TLK_DIAGNOSTICS is set");
  } else {
    InstallTlkDiagnosticHooks();
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_ADDRESS_LOG")) {
    LogFormat(L"address translation diagnostic hook skipped: HG_BRIDGE_DISABLE_ADDRESS_LOG is set");
  } else {
    InstallStartAddressTranslationLogHook();
  }
  if (server == nullptr) {
    LogFormat(L"direct-connect port repair hook skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_PORT_REPAIR")) {
    LogFormat(L"direct-connect port repair hook skipped: HG_BRIDGE_DISABLE_PORT_REPAIR is set");
  } else {
    InstallDirectConnectPortRepairHook();
  }
  if (server == nullptr) {
    LogFormat(L"session peer port repair hooks skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_SESSION_PEER_REPAIR")) {
    LogFormat(L"session peer port repair hooks skipped: HG_BRIDGE_DISABLE_SESSION_PEER_REPAIR is set");
  } else {
    InstallStartEnumerateSessionsPortRepairHooks();
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_NET_FLOW_LOG")) {
    LogFormat(L"net-flow diagnostic hooks skipped: HG_BRIDGE_DISABLE_NET_FLOW_LOG is set");
  } else {
    InstallNetFlowDiagnosticHooks();
  }
  if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_SENDTO_LOG")) {
    LogFormat(L"sendto diagnostic hook skipped: HG_BRIDGE_DISABLE_SENDTO_LOG is set");
  } else {
    InstallWinsockSendToHook();
  }
  if (server == nullptr) {
    LogFormat(L"Diamond CD key seed worker skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_DIAMOND_IDENTITY_SEED")) {
    LogFormat(L"Diamond CD key seed worker skipped: HG_BRIDGE_DISABLE_DIAMOND_IDENTITY_SEED is set");
  } else {
    StartDiamondIdentitySeedWorker();
  }
  if (server == nullptr) {
    LogFormat(L"auto-connect skipped: launcher target is not a known HG endpoint");
  } else if (IsTruthyEnvironmentFlag(L"HG_BRIDGE_DISABLE_AUTO_CONNECT")) {
    LogFormat(L"auto-connect skipped: HG_BRIDGE_DISABLE_AUTO_CONNECT is set");
  } else {
    StartAutoConnectWorker();
  }
  LogFormat(L"model/body compatibility hooks are deferred unless HG proves they are required");
  return 1;
}

}  // namespace

extern "C" __declspec(dllexport) DWORD WINAPI HgBridgeInit(LPVOID) {
  return BridgeMain(nullptr);
}

extern "C" __declspec(dllexport) const wchar_t* WINAPI HgBridgeVersion() {
  return kBridgeVersion;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    g_state.module = module;
    DisableThreadLibraryCalls(module);
  }
  return TRUE;
}
