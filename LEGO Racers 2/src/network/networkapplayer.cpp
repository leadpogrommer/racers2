#include "network/networkapplayer.h"

#include "frontend/foyer/foyermanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "frontend/secondary/secondarymenuinteraction.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/modes/controllerdeferred.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/recording/controllerrecordingservice.h"
#include "game/session/gamecontrollersession.h"
#include "input/controller/inputcontrollermanager.h"
#include "memory/memoryheap.h"
#include "network/networkdeferred.h"

#include <new.h>
#include <string.h>

static GameObject* __fastcall DestroyNetworkAppLayer(GameObject*, void*, LegoU32);
static LegoU32 __fastcall GetNetworkAppLayerAllocationSize(GameObject*, void*);
static LegoU32 __fastcall UpdateNetworkAppLayer(GameObject*, void*, LegoFloat);
static void __fastcall RemoveNetworkAppLayer(GameObject* p_object, void*)
{
	p_object->Remove();
}
static void __fastcall ResetNetworkPlayers(NetworkAppLayer*, void*);
static void __cdecl OnNetworkPeerDeparture(LegoU32, LegoU32);
static void __fastcall ScheduleNetworkPeerDeparture(NetworkAppLayer*, void*, LegoU32, LegoU32);
static NetworkChatState* __fastcall InitializeNetworkChat(NetworkChatState*, void*);
static void __fastcall ClearNetworkChatInput(NetworkChatState*, void*);
static void __fastcall ClearNetworkChatHistory(NetworkChatState*, void*);
static LegoU32* __fastcall AllocateNetworkPayloadKey(NetworkPayloadTracker*, void*);
static void __fastcall PruneNetworkPayloadKeys(NetworkPayloadTracker*, void*);
static NetworkTextInputQueue* __fastcall InitializeNetworkTextQueue(NetworkTextInputQueue*, void*);
static void __fastcall ClearNetworkTextQueue(NetworkTextInputQueue*, void*);
void __fastcall UpdateNetworkHost(NetworkAppLayer*, void*, LegoFloat);
void __fastcall UpdateNetworkClient(NetworkAppLayer*, void*, LegoFloat);

template <class T>
static T& NetworkRecordField(NetworkPlayerRecord* p_record, LegoU32 p_offset)
{
	return *(T*) ((LegoU8*) p_record + p_offset);
}
void __fastcall InitializeSecondaryMessage(void*, void*, LegoU32, const LegoFloat*, LegoFloat);
void __fastcall RefreshMenuDiscoveredNetworkSessions(GameObject*, void*);
void __fastcall PopSecondaryMenuActions(MenuManager*, void*, LegoU32, LegoS32);
void __fastcall UpdateNetworkChat(NetworkChatState*, void*);

static LegoU16 __fastcall DequeueNetworkTextCharacter(NetworkTextInputQueue*, void*);
static void __fastcall AdvanceNetworkChatHistory(NetworkChatState*, void*);
static void __fastcall SendNetworkChatInput(NetworkChatState*, void*);

// Retail ABI dispatch table: 0x005342bc, 21 slots through the size callback.
const NetworkAppLayerDispatch g_networkAppLayerDispatch = {
	&DestroyNetworkAppLayer,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&FoyerManagerInitialize,
	&GameObjectBaseZero,
	&UpdateNetworkAppLayer,
	&RemoveNetworkAppLayer,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument,
	&GetNetworkAppLayerAllocationSize
};

// GLOBAL: LEGORACERS2 0x00578070
static LegoFloat g_networkElapsedTime;

GameObject* __fastcall ConstructNetworkAppLayer(void* p_storage, void*, GameObjectTemplate* p_template)
{
	return new (p_storage) NetworkAppLayer(p_template);
}

// STUB: LEGORACERS2 0x004935b0
NetworkAppLayer::NetworkAppLayer(GameObjectTemplate* p_template)
	: GameObject(0x900, p_template, NULL), m_serverName("TEST SERVER")
{
	m_state = 0;
	m_menuState = 3;
	m_sessionState = 11;
	m_discoveryState = 13;
	InitializeNetworkChat(&m_chat, NULL);
	m_discoveryTime = 0.0f;
	for (LegoS32 player = 0; player < 8; ++player) {
		NetworkPlayerRecord& record = m_players[player];
		record.m_name.Initialize();
		for (LegoS32 word = 0; word < 4; ++word) {
			record.m_state[word] = 0;
		}
		for (LegoS32 item = 0; item < 36; ++item) {
			record.m_items[item].Initialize();
		}
	}
	m_unk0x2a84 = 0;
	m_unk0x27f0[0] = m_unk0x27f0[1] = m_unk0x27f0[2] = -1;
	m_vtable = (const GameObjectDispatchView*) &g_networkAppLayerDispatch;
	m_state = 1;
}

// STUB: LEGORACERS2 0x00493680
static LegoU32 __fastcall GetNetworkAppLayerAllocationSize(GameObject*, void*)
{
	return 0x2b0c;
}

// STUB: LEGORACERS2 0x00493690
static GameObject* __fastcall DestroyNetworkAppLayer(GameObject* p_object, void*, LegoU32 p_flags)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_networkAppLayerDispatch;
	// The embedded queue destructor at 479030 is a bare RET.
	p_object->GameObject::~GameObject();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004936d0
static void __fastcall ResetNetworkPlayers(NetworkAppLayer* p_network, void*)
{
	for (LegoS32 player = 0; player < 8; ++player) {
		NetworkPlayerRecord& record = p_network->m_players[player];
		memset(&record, 0, sizeof(record));
		record.m_key = -1;
		*(LegoU32*) ((LegoU8*) &record + 0x1c) = 0;
		*(LegoU32*) ((LegoU8*) &record + 0x2c) = -1;
	}
}

// STUB: LEGORACERS2 0x00493740
static void __cdecl OnNetworkPeerDeparture(LegoU32 p_key, LegoU32 p_delay)
{
	if (g_gameController.m_networkApp != NULL) {
		ScheduleNetworkPeerDeparture((NetworkAppLayer*) g_gameController.m_networkApp, NULL, p_key, p_delay);
	}
}

// STUB: LEGORACERS2 0x00493760
static void __fastcall ScheduleNetworkPeerDeparture(NetworkAppLayer* p_network, void*, LegoU32 p_key, LegoU32 p_delay)
{
	LegoS32 index;
	for (index = 0; index < 8 && p_network->m_players[index].m_key != (LegoS32) p_key; ++index) {
	}
	if (index == 8) {
		index = -1;
	}
	LegoU32 frame = g_inputControllerManager->m_unk0x1c + p_delay;
	*(LegoU32*) ((LegoU8*) &p_network->m_players[index] + 0x2c) = frame;
	LegoU32 message[3] = {0x19, p_key, g_inputControllerManager->m_unk0x1c + p_delay};
	LegoU32* payloadKey = AllocateNetworkPayloadKey(&p_network->m_payloads, NULL);
	g_controllerRecordingService->m_vtable
		->m_processValue(g_controllerRecordingService, NULL, message, sizeof(message), 0x4b, 0, payloadKey);
}

// STUB: LEGORACERS2 0x004937f0
LegoBool32 __fastcall HostMenuNetworkSession(void* p_object, void*)
{
	NetworkAppLayer* network = (NetworkAppLayer*) p_object;
	ControllerRecordingService* service = g_controllerRecordingService;
	service->m_vtable->m_setUnknownWord0xd8(service, NULL, 0);
	network->m_serverName = CharBuffer24("xxx's Game");
	for (LegoS32 index = 0; index < 3; ++index) {
		network->m_serverName.m_data[index] = (LegoChar) g_gameControllerSettings->m_names[0].m_text[index];
	}
	LegoBool32 result = service->m_vtable->m_hostSession(service, NULL, network->m_serverName.m_data);
	if (result != 0) {
		network->m_sessionState = 1;
		service->m_vtable->m_setCurrentRecordName(service, NULL, "TESTNAME");
		ClearNetworkChatHistory(&network->m_chat, NULL);
		ClearNetworkChatInput(&network->m_chat, NULL);
		service->m_vtable->m_setUnknownWord0x2c(service, NULL, (LegoU32) &OnNetworkPeerDeparture);
	}
	return result;
}

// STUB: LEGORACERS2 0x004938b0
void __fastcall LeaveMenuNetworkHost(void* p_object, void*)
{
	NetworkAppLayer* network = (NetworkAppLayer*) p_object;
	ControllerRecordingService* service = g_controllerRecordingService;
	service->m_vtable->m_restartPeerSession(service, NULL);
	service->m_vtable->m_setUnknownWord0x2c(service, NULL, 0);
	ResetNetworkPlayers(network, NULL);
	network->m_sessionState = 0;
	service->m_vtable->m_setUnknownWord0xd8(service, NULL, 0);
}

// STUB: LEGORACERS2 0x00493930
void __fastcall LeaveMenuNetworkBrowser(void* p_object, void*)
{
	NetworkAppLayer* network = (NetworkAppLayer*) p_object;
	ControllerRecordingService* service = g_controllerRecordingService;
	if (network->m_discoveryState == 1) {
		service->m_vtable->m_cancelPeerOperation(service, NULL);
	}
	service->m_vtable->m_setUnknownWord0x2c(service, NULL, 0);
	service->m_vtable->m_initializePeer(service, NULL);
	ResetNetworkPlayers(network, NULL);
	network->m_discoveryState = 0;
}

// STUB: LEGORACERS2 0x004939c0
LegoBool32 __fastcall JoinMenuNetworkSession(void* p_object, void*, LegoS32 p_selected)
{
	NetworkAppLayer* network = (NetworkAppLayer*) p_object;
	if (p_selected >= network->m_discoveredSessionCount) {
		return FALSE;
	}
	ControllerRecordingService* service = g_controllerRecordingService;
	service->m_vtable->m_cancelPeerOperation(service, NULL);
	service->m_vtable->m_connectToSession(service, NULL, p_selected);
	network->m_discoveryState = 2;
	LegoFloat center[2] = {0.5f, 0.5f};
	InitializeSecondaryMessage(
		((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x8c,
		NULL,
		0x10e,
		center,
		0.8f
	);
	network->m_messageTime = 10000.0f;
	return TRUE;
}

// STUB: LEGORACERS2 0x00493a40
static LegoU32 __fastcall UpdateNetworkAppLayer(GameObject* p_object, void*, LegoFloat p_elapsed)
{
	NetworkAppLayer* network = (NetworkAppLayer*) p_object;
	g_networkElapsedTime += p_elapsed;
	if (network->m_state == 7 && (network->m_menuState == 1 || network->m_menuState == 2)) {
		g_controllerRecordingService->m_vtable->m_waitForInput(g_controllerRecordingService, NULL);
		PruneNetworkPayloadKeys(&network->m_payloads, NULL);
	}
	if (network->m_messageTime >= 0.0f) {
		network->m_messageTime -= p_elapsed;
		if (network->m_messageTime <= 0.0f) {
			ReleaseSecondaryObject8c(((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x8c, NULL);
		}
	}
	if (((LegoS32) network->m_sessionState >= 7 && (LegoS32) network->m_sessionState < 10) ||
		((LegoS32) network->m_discoveryState >= 9 && (LegoS32) network->m_discoveryState < 12)) {
		for (LegoS32 player = 0; player < (LegoS32) g_gameControllerSession->m_unk0x18; ++player) {
			void* racer = (void*) g_gameControllerSession->m_unk0x28[player];
			LegoS32 key = *(LegoS32*) ((LegoU8*) racer + 0x5c), index;
			for (index = 0; index < 8 && network->m_players[index].m_key != key; ++index) {
			}
			if (index == 8) {
				index = -1;
			}
			NetworkPlayerRecord& record = network->m_players[index];
			LegoU32& deadline = *(LegoU32*) ((LegoU8*) &record + 0x2c);
			if (deadline != (LegoU32) -1) {
				LegoU32 offset = g_inputControllerManager->m_unk0x1c;
				if (deadline <= g_inputControllerManager->GetCurrentFrame() - offset) {
					SetNetworkRacerState(racer, NULL, 2);
					deadline = -1;
					memset(&record, 0, sizeof(record));
					record.m_key = -1;
					*(LegoU32*) ((LegoU8*) &record + 0x28) = 0;
					deadline = -1;
				}
			}
		}
	}
	if (network->m_menuState == 1) {
		UpdateNetworkHost(network, NULL, p_elapsed);
	}
	else if (network->m_menuState == 2) {
		UpdateNetworkClient(network, NULL, p_elapsed);
	}
	return 1;
}

// STUB: LEGORACERS2 0x00493c40
void __fastcall UpdateNetworkHost(NetworkAppLayer* p_network, void*, LegoFloat p_elapsed)
{
	if ((LegoS32) p_network->m_sessionState >= 0) {
		ProcessNetworkMessages(p_network, NULL, p_elapsed);
	}
	for (LegoS32 index = 0; index < 8; ++index) {
		NetworkPlayerRecord& record = p_network->m_players[index];
		if (NetworkRecordField<LegoU32>(&record, 0x28) != 0) {
			NetworkRecordField<LegoFloat>(&record, 0x24) += p_elapsed;
		}
	}
	ControllerRecordingService* service = g_controllerRecordingService;
	switch (p_network->m_sessionState) {
	case 1: {
		LegoU32 key = service->m_vtable->m_getCurrentRecordValue(service, NULL);
		LegoS32 index;
		for (index = 0; index < 8 && p_network->m_players[index].m_key != (LegoS32) key; ++index) {
		}
		if (index == 8) {
			return;
		}
		InitializeNetworkLocalPlayer(p_network, NULL);
		service->m_vtable->m_setUnknownWord0xd8(service, NULL, 1);
		p_network->m_sessionState = 2;
		p_network->m_chat.m_sender = service->m_vtable->m_getCurrentRecordValue(service, NULL);
		return;
	}
	case 2:
		UpdateNetworkChat(&p_network->m_chat, NULL);
		return;
	case 3:
		if (PrepareNetworkGame(p_network, NULL, p_elapsed) != 0) {
			ConfigureNetworkGame(p_network, NULL);
			p_network->m_sessionState = 4;
			p_network->m_discoveryTime = 0.0f;
		}
		return;
	case 4:
	case 6: {
		LegoS32 count = 0;
		for (LegoS32 index = 0; index < 8; ++index) {
			if (NetworkRecordField<LegoU32>(&p_network->m_players[index], 0x1c) != 0) {
				++count;
			}
		}
		if (count == *(LegoS32*) ((LegoU8*) p_network + 0x2800)) {
			p_network->m_discoveryTime -= p_elapsed;
		}
		if (p_network->m_discoveryTime < 0.0f) {
			if (p_network->m_sessionState == 6) {
				FinishNetworkGame(p_network, NULL);
			}
			else {
				BeginNetworkGame(p_network, NULL);
			}
			for (LegoS32 index = 0; index < 8; ++index) {
				NetworkRecordField<LegoU32>(&p_network->m_players[index], 0x1c) = 0;
			}
		}
		return;
	}
	}
}

// STUB: LEGORACERS2 0x00493dd0
void __fastcall UpdateNetworkClient(NetworkAppLayer* p_network, void*, LegoFloat p_elapsed)
{
	if ((LegoS32) p_network->m_discoveryState > 2) {
		ProcessNetworkMessages(p_network, NULL, p_elapsed);
	}
	ControllerRecordingService* service = g_controllerRecordingService;
	switch (p_network->m_discoveryState) {
	case 1:
		p_network->m_discoveryTime -= p_elapsed;
		if (p_network->m_discoveryTime < 0.0f) {
			service->m_vtable->m_enumerateHosts(service, NULL);
			p_network->m_discoveryTime = 5.0f;
		}
		RefreshMenuDiscoveredNetworkSessions(p_network, NULL);
		return;
	case 2: {
		LegoU32 state = service->m_vtable->m_getUnknownWord0x20(service, NULL);
		if (state == 2) {
			p_network->m_discoveryState = 3;
			p_network->m_discoveryTime = 0.0f;
			ClearNetworkChatHistory(&p_network->m_chat, NULL);
			ClearNetworkChatInput(&p_network->m_chat, NULL);
		}
		else if (state == 0) {
			LegoFloat center[2] = {0.5f, 0.5f};
			InitializeSecondaryMessage(
				((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x8c,
				NULL,
				0x10d,
				center,
				0.8f
			);
			p_network->m_messageTime = 5.0f;
			LeaveMenuNetworkBrowser(p_network, NULL);
			PopSecondaryMenuActions((MenuManager*) g_gameController.m_menuManager, NULL, 1, 1);
		}
		return;
	}
	case 3:
		p_network->m_discoveryTime += p_elapsed;
		if (p_network->m_discoveryTime > 3.0f) {
			LegoU32 key = service->m_vtable->m_getCurrentRecordValue(service, NULL);
			LegoS32 index;
			for (index = 0; index < 8 && p_network->m_players[index].m_key != (LegoS32) key; ++index) {
			}
			if (index == 8) {
				return;
			}
			p_network->m_discoveryTime = 0.0f;
			InitializeNetworkLocalPlayer(p_network, NULL);
			SendNetworkPlayerCar(p_network, NULL, -1);
			SendNetworkPlayerCharacter(p_network, NULL, -1);
			SendNetworkPlayerState(p_network, NULL, -1);
			p_network->m_discoveryState = 4;
			p_network->m_messageTime = 0.0f;
			p_network->m_chat.m_sender = service->m_vtable->m_getCurrentRecordValue(service, NULL);
		}
		return;
	case 4:
		UpdateNetworkChat(&p_network->m_chat, NULL);
		return;
	case 5:
		if (PrepareNetworkGame(p_network, NULL, p_elapsed) != 0) {
			ConfigureNetworkGame(p_network, NULL);
			p_network->m_discoveryState = 6;
		}
		return;
	case 10: {
		if (*(LegoU32*) ((LegoU8*) p_network + 0x2a88) != 0) {
			if (g_gameControllerSession->m_mode == 3) {
				FinishSecondaryBattle(g_gameController.m_battleManager, NULL);
			}
			else {
				FinishSecondaryRace(g_gameController.m_raceManager, NULL, 1);
			}
			LeaveMenuNetworkBrowser(p_network, NULL);
			PopSecondaryMenuActions((MenuManager*) g_gameController.m_menuManager, NULL, 1, 1);
			return;
		}
		LegoS32 count = 0;
		for (LegoS32 index = 0; index < 8; ++index) {
			if (NetworkRecordField<LegoU32>(&p_network->m_players[index], 0x1c) != 0) {
				++count;
			}
		}
		LegoS32 expected = *(LegoS32*) ((LegoU8*) p_network + 0x2800);
		if ((g_gameControllerSession->m_mode == 3 && count == expected - 1) || count == expected) {
			LegoU32 message[2] = {0xe, 0};
			service->m_vtable->m_processValue(
				service,
				NULL,
				message,
				sizeof(message),
				0x13,
				0,
				AllocateNetworkPayloadKey(&p_network->m_payloads, NULL)
			);
			p_network->m_discoveryState = 11;
			for (LegoS32 index = 0; index < 8; ++index) {
				NetworkRecordField<LegoU32>(&p_network->m_players[index], 0x1c) = 0;
			}
		}
		return;
	}
	}
}

// STUB: LEGORACERS2 0x004943b0
void __fastcall StartMenuNetworkGame(void* p_object, void*)
{
	NetworkAppLayer* network = (NetworkAppLayer*) p_object;
	if (network->m_sessionState == 2) {
		LegoU32 message[2] = {0x15, 0};
		g_controllerRecordingService->m_vtable->m_processValue(
			g_controllerRecordingService,
			NULL,
			message,
			sizeof(message),
			0xb,
			0,
			AllocateNetworkPayloadKey(&network->m_payloads, NULL)
		);
		for (LegoS32 index = 0; index < 8; ++index) {
			NetworkRecordField<LegoU32>(&network->m_players[index], 0x28) = 1;
			NetworkRecordField<LegoFloat>(&network->m_players[index], 0x24) = 0.0f;
		}
	}
}
// STUB: LEGORACERS2 0x00494f20
static NetworkTextInputQueue* __fastcall InitializeNetworkTextQueue(NetworkTextInputQueue* p_queue, void*)
{
	p_queue->m_writeIndex = p_queue->m_readIndex = p_queue->m_busy = 0;
	ClearNetworkTextQueue(p_queue, NULL);
	return p_queue;
}

// STUB: LEGORACERS2 0x00494fb0
static LegoU16 __fastcall DequeueNetworkTextCharacter(NetworkTextInputQueue* p_queue, void*)
{
	if (p_queue->m_writeIndex == p_queue->m_readIndex || p_queue->m_busy != 0) {
		return 0;
	}
	p_queue->m_busy = 1;
	LegoU16 character = p_queue->m_characters[p_queue->m_readIndex];
	if (++p_queue->m_readIndex == 64) {
		p_queue->m_readIndex = 0;
	}
	p_queue->m_busy = 0;
	return character;
}

// STUB: LEGORACERS2 0x00495010
static void __fastcall ClearNetworkTextQueue(NetworkTextInputQueue* p_queue, void*)
{
	memset(p_queue->m_characters, 0, sizeof(p_queue->m_characters));
}

// STUB: LEGORACERS2 0x00495020
static NetworkChatState* __fastcall InitializeNetworkChat(NetworkChatState* p_chat, void*)
{
	p_chat->m_historyIndex = 0;
	p_chat->m_changed = 1;
	InitializeNetworkTextQueue(&p_chat->m_queue, NULL);
	ClearNetworkChatInput(p_chat, NULL);
	ClearNetworkChatHistory(p_chat, NULL);
	return p_chat;
}

// STUB: LEGORACERS2 0x00495060
static void __fastcall ClearNetworkChatInput(NetworkChatState* p_chat, void*)
{
	memset(p_chat->m_input, 0, sizeof(p_chat->m_input));
	p_chat->m_inputLength = 0;
	p_chat->m_changed = 1;
}

// STUB: LEGORACERS2 0x00495080
static void __fastcall ClearNetworkChatHistory(NetworkChatState* p_chat, void*)
{
	for (LegoS32 index = 0; index < 10; ++index) {
		// The last two WCHARs in each history record remain untouched.
		memset(p_chat->m_history[index], 0, 24 * sizeof(LegoU16));
	}
	p_chat->m_historyIndex = 9;
	p_chat->m_changed = 1;
}

// STUB: LEGORACERS2 0x004950c0
static void __fastcall AdvanceNetworkChatHistory(NetworkChatState* p_chat, void*)
{
	if (++p_chat->m_historyIndex == 10) {
		p_chat->m_historyIndex = 0;
	}
	memset(p_chat->m_history[p_chat->m_historyIndex], 0, 24 * sizeof(LegoU16));
}

// STUB: LEGORACERS2 0x00495150
static void __fastcall SendNetworkChatInput(NetworkChatState* p_chat, void*)
{
	NetworkAppLayer* network = (NetworkAppLayer*) g_gameController.m_networkApp;
	network->m_unk0x2a84 = 0;
	if (p_chat->m_input[0] != 0) {
		struct ChatMessage {
			LegoU32 m_type;
			LegoU16 m_text[24];
		} message;
		message.m_type = 1;
		const GameControllerName* name = NULL;
		for (LegoS32 index = 0; index < 8; ++index) {
			if (network->m_players[index].m_key == (LegoS32) p_chat->m_sender) {
				name = &network->m_players[index].m_name;
				break;
			}
		}
		AdvanceNetworkChatHistory(p_chat, NULL);
		LegoU16* history = p_chat->m_history[p_chat->m_historyIndex];
		for (LegoS32 initial = 0; initial < 3; ++initial) {
			history[initial] = message.m_text[initial] = name->m_text[initial];
		}
		history[3] = message.m_text[3] = ':';
		history[4] = message.m_text[4] = ' ';
		for (LegoS32 character = 0; character < 19; ++character) {
			history[5 + character] = message.m_text[5 + character] = p_chat->m_input[character];
		}
		LegoU32 payloadKey;
		g_controllerRecordingService->m_vtable->m_processValue(
			g_controllerRecordingService,
			NULL,
			(const LegoU32*) &message,
			sizeof(message),
			0x4a,
			p_chat->m_sender,
			&payloadKey
		);
		ClearNetworkChatInput(p_chat, NULL);
		p_chat->m_changed = 1;
	}
}

// STUB: LEGORACERS2 0x004952b0
void __fastcall UpdateNetworkChat(NetworkChatState* p_chat, void*)
{
	while (p_chat->m_queue.m_writeIndex != p_chat->m_queue.m_readIndex) {
		LegoU16 character = DequeueNetworkTextCharacter(&p_chat->m_queue, NULL);
		switch (character) {
		case 0:
			break;
		case 8:
			if (p_chat->m_inputLength > 0) {
				p_chat->m_input[--p_chat->m_inputLength] = 0;
				p_chat->m_changed = 1;
			}
			break;
		case 10:
		case 13:
			SendNetworkChatInput(p_chat, NULL);
			break;
		default:
			if (p_chat->m_inputLength < 18) {
				p_chat->m_input[p_chat->m_inputLength++] = character;
				p_chat->m_changed = 1;
			}
			break;
		}
	}
}
// STUB: LEGORACERS2 0x00495360
static LegoU32* __fastcall AllocateNetworkPayloadKey(NetworkPayloadTracker* p_tracker, void*)
{
	for (LegoS32 index = 0; index < 128; ++index) {
		if (p_tracker->m_active[index] == 0) {
			p_tracker->m_active[index] = 1;
			return &p_tracker->m_keys[index];
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x00495390
static void __fastcall PruneNetworkPayloadKeys(NetworkPayloadTracker* p_tracker, void*)
{
	ControllerRecordingService* service = g_controllerRecordingService;
	for (LegoS32 index = 0; index < 128; ++index) {
		if (p_tracker->m_active[index] != 0 &&
			(LegoS32) service->m_vtable->m_getProviderKeyValue(service, NULL, p_tracker->m_keys[index]) > 1) {
			service->m_vtable->m_removeProviderKey(service, NULL, p_tracker->m_keys[index]);
			p_tracker->m_active[index] = 0;
		}
	}
}
