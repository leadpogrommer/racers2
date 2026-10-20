#ifndef LR2_NETWORKAPPLAYER_H
#define LR2_NETWORKAPPLAYER_H

#include "core/application/applicationmessages.h"
#include "game/controller/gamecontroller.h"

typedef ApplicationCharacterQueue NetworkTextInputQueue;
DECOMP_SIZE_ASSERT(NetworkTextInputQueue, 0x8c);

struct NetworkChatState {
	LegoU16 m_input[24];
	LegoS32 m_inputLength;
	LegoU16 m_history[10][26];
	LegoU32 m_historyIndex;
	LegoU32 m_sender;
	LegoU32 m_changed;
	LegoU8 m_unk0x248[4];
	NetworkTextInputQueue m_queue;
};
DECOMP_SIZE_ASSERT(NetworkChatState, 0x2d8);
DECOMP_STATIC_ASSERT(offsetof(NetworkChatState, m_queue) == 0x24c);

struct NetworkPlayerRecord {
	LegoS32 m_key;
	GameControllerName m_name;
	undefined m_unk0x18[0x30 - 0x18];
	LegoU32 m_state[4];
	undefined m_unk0x40[0x64 - 0x40];
	GameControllerItem m_items[36];
};
DECOMP_SIZE_ASSERT(NetworkPlayerRecord, 0x334);

struct NetworkPayloadTracker {
	LegoU32 m_keys[128];
	LegoU8 m_active[128];
};
DECOMP_SIZE_ASSERT(NetworkPayloadTracker, 0x280);

struct NetworkAppLayer : GameObject {
	NetworkAppLayer(GameObjectTemplate* p_template);
	LegoU32 m_state; // 0x58
	LegoU32 m_menuState;
	LegoU32 m_sessionState;
	LegoU32 m_discoveryState;
	LegoFloat m_messageTime;
	undefined m_unk0x6c[0xb58 - 0x6c];
	LegoS32 m_discoveredSessionCount;
	CharBuffer24 m_serverName;
	NetworkChatState m_chat;
	LegoFloat m_discoveryTime;        // 0xe4c
	NetworkPlayerRecord m_players[8]; // 0xe50
	LegoS32 m_unk0x27f0[3];
	undefined m_unk0x27fc[8];
	NetworkPayloadTracker m_payloads;
	LegoU32 m_unk0x2a84;
	undefined m_unk0x2a88[0x2b0c - 0x2a88];
};
DECOMP_SIZE_ASSERT(NetworkAppLayer, 0x2b0c);
DECOMP_STATIC_ASSERT(offsetof(NetworkAppLayer, m_serverName) == 0xb5c);
DECOMP_STATIC_ASSERT(offsetof(NetworkAppLayer, m_chat) == 0xb74);
DECOMP_STATIC_ASSERT(offsetof(NetworkAppLayer, m_discoveryTime) == 0xe4c);
DECOMP_STATIC_ASSERT(offsetof(NetworkAppLayer, m_players) == 0xe50);

struct NetworkAppLayerDispatch {
	GameObjectDispatchView::Destroy m_destroy;
	GameObjectDispatchView::TwoArguments m_applyChildTransform;
	GameObjectDispatchView::Action m_action0x08, m_action0x0c, m_initialize;
	GameObjectDispatchView::Value m_isActive;
	LegoU32(__fastcall* m_update)(GameObject*, void*, LegoFloat);
	GameObjectDispatchView::Action m_remove;
	GameObjectDispatchView::Value m_value0x20, m_value0x24;
	GameObjectDispatchView::Action m_action0x28, m_action0x2c, m_drawChild;
	GameObjectDispatchView::Update m_action0x34;
	GameObjectDispatchView::Value m_value0x38;
	GameObjectDispatchView::TwoArguments m_action0x3c;
	GameObjectDispatchView::Value m_value0x40;
	GameObjectDispatchView::ValueTwoArguments m_value0x44;
	GameObjectDispatchView::Update m_updateRegion, m_action0x4c;
	LegoU32(__fastcall* m_allocationSize)(GameObject*, void*);
};
DECOMP_SIZE_ASSERT(NetworkAppLayerDispatch, 0x54);
extern const NetworkAppLayerDispatch g_networkAppLayerDispatch;
GameObject* __fastcall ConstructNetworkAppLayer(void*, void*, GameObjectTemplate*);
LegoBool32 __fastcall HostMenuNetworkSession(void*, void*);
void __fastcall LeaveMenuNetworkHost(void*, void*);
void __fastcall LeaveMenuNetworkBrowser(void*, void*);
LegoBool32 __fastcall JoinMenuNetworkSession(void*, void*, LegoS32);
void __fastcall StartMenuNetworkGame(void*, void*);
#endif
