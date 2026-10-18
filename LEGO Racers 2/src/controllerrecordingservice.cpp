#include "controllerrecordingservice.h"

#include "collisionqueries.h"
#include "diagnostics.h"
#include "directplay8.h"
#include "inputcontrollermanager.h"
#include "memoryheap.h"

#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>

// GLOBAL: LEGORACERS2 0x00654768
static LegoU32 g_controllerRecordingServiceStorage[(0x230) / sizeof(LegoU32)];

// GLOBAL: LEGORACERS2 0x00549fbc
ControllerRecordingService* g_controllerRecordingService =
	(ControllerRecordingService*) g_controllerRecordingServiceStorage;

DECOMP_STATIC_ASSERT(sizeof(g_controllerRecordingServiceStorage) == 0x230);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_cancelPeerOperation) == 0x20);

struct ControllerRecordingProviderEntry {
	LegoS16 m_key;
	LegoU16 m_unk0x02;
	LegoU32 m_operationId;
	LegoU32 m_state;
	LegoU32 m_timestamp;
	void CancelPending(IDirectPlay8Peer* p_peer);
	void SetKey(LegoU16 p_key);
};
DECOMP_SIZE_ASSERT(ControllerRecordingProviderEntry, 0x10);

struct ControllerRecordingProviderMap {
	volatile LegoU32 m_lock;
	ControllerRecordingProviderEntry* m_entries;
	LegoS32 m_count;
	LegoS32 m_usedCount;
	LegoS32 m_expirationThreshold;
	void Initialize(LegoS32 p_capacity);
	void Shutdown(IDirectPlay8Peer* p_peer);
	ControllerRecordingProviderEntry* Add(LegoU16 p_key);
	LegoU32 FindValue(LegoS16 p_key);
	LegoBool32 SetState(LegoS16 p_key, LegoU32 p_state);
	void Remove(LegoS16 p_key);
	void PruneExpiredEntries();
};
DECOMP_SIZE_ASSERT(ControllerRecordingProviderMap, 0x14);

struct ControllerRecordingObjectEntry {
	void* m_object;
	LegoU32 m_active;
	LegoU32 m_unk0x08;
};
DECOMP_SIZE_ASSERT(ControllerRecordingObjectEntry, 0x0c);

struct ControllerRecordingObjectMap {
	volatile LegoU32 m_lock;
	ControllerRecordingObjectEntry* m_allocation;
	ControllerRecordingObjectEntry* m_entries;
	LegoS32 m_count;
	LegoU32 m_unk0x10;
	LegoU32 m_unk0x14;
	LegoU32 m_unk0x18;
	LegoU32 m_unk0x1c;
	LegoU32 m_unk0x20;
	LegoU32 m_unk0x24;
	LegoU32 m_nextId;
	void Initialize(LegoS32 p_capacity, LegoS32 p_firstSpan, LegoS32 p_secondSpan);
	void Shutdown();
	void* AllocatePacket(LegoU32 p_bytes);
	void* CreatePacket(const LegoU8* p_data, LegoU32 p_size);
	void ClearMatchingId(LegoS16 p_id, LegoU32 p_unused);
};
DECOMP_SIZE_ASSERT(ControllerRecordingObjectMap, 0x2c);

struct ControllerRecordingMessageRecord {
	LegoU32 m_userContext;
	LegoU32 m_messageId;
	void* m_message;
	LegoU32 m_payloadSize;
	LegoU32 m_flags;
};
DECOMP_SIZE_ASSERT(ControllerRecordingMessageRecord, 0x14);

struct ControllerRecordingMessageQueue {
	volatile LegoU32 m_lock;
	LegoU8* m_buffer;
	LegoU8* m_read;
	LegoU8* m_write;
	LegoU8* m_end;
	LegoU32 m_capacity;
	LegoU32 m_accountedBytes;
	LegoU32 m_pendingBytes;
	LegoU32 m_queuedCount;
	void Initialize(LegoU32 p_capacity);
	void Shutdown();
	LegoBool32 EnsureBytes(LegoU32 p_bytes, LegoU32* p_accountedBytes);
	LegoBool32 Enqueue(ControllerRecordingMessageRecord* p_message);
	LegoBool32 Dequeue(ControllerRecordingMessageRecord** p_message);
};

DECOMP_SIZE_ASSERT(ControllerRecordingMessageQueue, 0x24);

struct ControllerRecordingProviderOption {
	WCHAR m_name[0x100];
	LegoU32 m_type;
};
DECOMP_SIZE_ASSERT(ControllerRecordingProviderOption, 0x204);

// GLOBAL: LEGORACERS2 0x00653b44
static LegoU8 g_providerConfigurationInitialized;
// GLOBAL: LEGORACERS2 0x00654998
static ControllerRecordingProviderOption g_tcpProviderOptions[1];
// GLOBAL: LEGORACERS2 0x00653130
static ControllerRecordingProviderOption g_serialProviderOptions[5];
// GLOBAL: LEGORACERS2 0x00653b50
static ControllerRecordingProviderOption g_modemProviderOptions[6];

// GLOBAL: LEGORACERS2 0x00535bd8
static const GUID g_controllerRecordingApplicationGuid =
	{0x69e5d608, 0x7fa4, 0x11d2, {0x90, 0xdc, 0x00, 0x40, 0x95, 0x03, 0xb8, 0xd0}};

static HRESULT WINAPI ControllerRecordingPeerMessageHandler(PVOID p_userContext, DWORD p_messageId, PVOID p_message);
static HRESULT WINAPI ControllerRecordingLobbyMessageHandler(PVOID p_userContext, DWORD p_messageId, PVOID p_message);

static LegoBool32 __fastcall ServiceInitialize(
	ControllerRecordingService* p_this,
	void*,
	const ControllerRecordingServiceSettings* p_settings,
	void* p_context
)
{
	return p_this->Initialize(p_settings, p_context);
}
static void __fastcall ServiceSetUnknown2c(ControllerRecordingService* p_this, void*, LegoU32 p_value)
{
	p_this->SetUnknownWord0x2c(p_value);
}
static void __fastcall ServiceShutdown(ControllerRecordingService* p_this, void*)
{
	p_this->Shutdown();
}
static LegoBool32 __fastcall ServiceEnumerateDevices(ControllerRecordingService* p_this, void*, LegoS32 p_index)
{
	return p_this->EnumerateDevicesForProvider(p_index);
}
static LegoBool32 __fastcall ServiceCopyDeviceNames(
	ControllerRecordingService* p_this,
	void*,
	LegoChar* p_out,
	LegoS32* p_count
)
{
	return p_this->CopyDeviceNames(p_out, p_count);
}
static LegoBool32 __fastcall ServiceCopySessionNames(
	ControllerRecordingService* p_this,
	void*,
	LegoChar* p_out,
	LegoS32* p_count
)
{
	return p_this->CopySessionNames(p_out, p_count);
}
static LegoBool32 __fastcall ServiceSelectProvider(
	ControllerRecordingService* p_this,
	void*,
	LegoS32 p_index,
	const LegoChar** p_options,
	LegoU32 p_count
)
{
	return p_this->SelectServiceProvider(p_index, p_options, p_count);
}
static void __fastcall ServiceEnumerateHosts(ControllerRecordingService* p_this, void*)
{
	p_this->EnumerateHosts();
}
static void __fastcall ServiceCancelPeerOperation(ControllerRecordingService* p_this, void*)
{
	p_this->CancelPeerOperation();
}
static LegoBool32 __fastcall ServiceCopySessionRecords(
	ControllerRecordingService* p_this,
	void*,
	LegoU8* p_out,
	LegoS32* p_count
)
{
	return p_this->CopySessionRecords(p_out, p_count);
}
static LegoBool32 __fastcall ServiceHostSession(ControllerRecordingService* p_this, void*, const LegoChar* p_name)
{
	return p_this->HostSession(p_name);
}
static LegoBool32 __fastcall ServiceConnectToSession(ControllerRecordingService* p_this, void*, LegoS32 p_index)
{
	return p_this->ConnectToSession(p_index);
}
static LegoBool32 __fastcall ServiceResetNegotiatedState(ControllerRecordingService* p_this, void*)
{
	return p_this->ResetNegotiatedState();
}
static LegoBool32 __fastcall ServiceInitializePeer(ControllerRecordingService* p_this, void*)
{
	return p_this->InitializePeer();
}
static LegoBool32 __fastcall ServiceRestartPeerSession(ControllerRecordingService* p_this, void*)
{
	return p_this->RestartPeerSession();
}
static void __fastcall ServiceSetUnknownd8(ControllerRecordingService* p_this, void*, LegoU32 p_value)
{
	p_this->SetUnknownWord0xd8(p_value);
}
static LegoU32 __fastcall ServiceGetUnknownd8(ControllerRecordingService* p_this, void*)
{
	return p_this->GetUnknownWord0xd8();
}
static LegoU32 __fastcall ServiceGetUnknown20(ControllerRecordingService* p_this, void*)
{
	return p_this->GetUnknownWord0x20();
}
static LegoU32 __fastcall ServiceProcessValue(
	ControllerRecordingService* p_this,
	void*,
	const LegoU32* p_payload,
	LegoU32 p_bytes,
	LegoU8 p_flags,
	LegoU32 p_value,
	LegoU32* p_key
)
{
	return p_this->ProcessValue((const LegoU8*) p_payload, p_bytes, p_flags, p_value, p_key);
}
static LegoU32 __fastcall ServiceGetProviderValue(ControllerRecordingService* p_this, void*, LegoU32 p_key)
{
	return p_this->GetProviderKeyValue(p_key);
}
static void __fastcall ServiceRemoveProviderValue(ControllerRecordingService* p_this, void*, LegoU32 p_key)
{
	p_this->RemoveProviderKey(p_key);
}
static LegoBool32 __fastcall ServiceWaitForInput(ControllerRecordingService* p_this, void*)
{
	return p_this->WaitForInput();
}
static void __fastcall ServiceSetCurrentRecordName(ControllerRecordingService* p_this, void*, LegoChar* p_name)
{
	p_this->SetCurrentRecordName(p_name);
}
static void __fastcall ServiceCopyInputStatistics(ControllerRecordingService* p_this, void*, LegoU32* p_output)
{
	p_this->CopyInputStatistics(p_output);
}
static LegoU32 __fastcall ServiceGetCurrentRecordValue(ControllerRecordingService* p_this, void*)
{
	return p_this->GetCurrentRecordValue();
}
static LegoU32 __fastcall ServiceFindOccupiedRecordValue(ControllerRecordingService* p_this, void*)
{
	return p_this->FindOccupiedRecordValue();
}
static void __fastcall ServiceCopyRecordValues(
	ControllerRecordingService* p_this,
	void*,
	LegoU32* p_values,
	LegoU32* p_count
)
{
	p_this->CopyRecordValues(p_values, p_count);
}
static void __fastcall ServiceAssignRecordOwner(
	ControllerRecordingService* p_this,
	void*,
	LegoU32 p_value,
	InputController* p_controller
)
{
	p_this->AssignRecordOwner(p_value, p_controller);
}
static void __fastcall ServiceRecordInput(
	ControllerRecordingService* p_this,
	void*,
	InputController* p_controller,
	const LegoU32* p_input,
	LegoU32 p_frame
)
{
	p_this->RecordInput(p_controller, p_input, p_frame);
}
static void __fastcall ServiceUpdateInputRecord(
	ControllerRecordingService* p_this,
	void*,
	InputController* p_controller,
	LegoU32 p_frame
)
{
	p_this->UpdateInputRecord(p_controller, p_frame);
}
static LegoBool32 __fastcall ServiceHasInputFrame(
	ControllerRecordingService* p_this,
	void*,
	InputController* p_controller,
	LegoU32 p_frame
)
{
	return p_this->HasInputFrame(p_controller, p_frame);
}
static void __fastcall ServiceRequestInputFrame(
	ControllerRecordingService* p_this,
	void*,
	InputController* p_controller,
	LegoU32 p_frame
)
{
	p_this->RequestInputFrame(p_controller, p_frame);
}
static LegoU32 __fastcall ServiceGetRequestedInputValue(ControllerRecordingService* p_this, void*)
{
	return p_this->GetRequestedInputValue();
}
static void __fastcall ServiceSetRequestedInputValue(ControllerRecordingService* p_this, void*, LegoU32 p_value)
{
	p_this->SetRequestedInputValue(p_value);
}

const ControllerRecordingServiceDispatchView g_controllerRecordingServiceDispatch = {
	ServiceInitialize,
	ServiceSetUnknown2c,
	ServiceShutdown,
	ServiceEnumerateDevices,
	ServiceCopyDeviceNames,
	ServiceCopySessionNames,
	ServiceSelectProvider,
	ServiceEnumerateHosts,
	ServiceCancelPeerOperation,
	ServiceCopySessionRecords,
	ServiceHostSession,
	ServiceConnectToSession,
	ServiceResetNegotiatedState,
	ServiceInitializePeer,
	ServiceRestartPeerSession,
	ServiceSetUnknownd8,
	ServiceGetUnknownd8,
	ServiceGetUnknown20,
	ServiceProcessValue,
	ServiceGetProviderValue,
	ServiceRemoveProviderValue,
	ServiceWaitForInput,
	ServiceSetCurrentRecordName,
	ServiceCopyInputStatistics,
	ServiceGetCurrentRecordValue,
	ServiceFindOccupiedRecordValue,
	ServiceCopyRecordValues,
	ServiceAssignRecordOwner,
	ServiceRecordInput,
	ServiceUpdateInputRecord,
	ServiceHasInputFrame,
	ServiceRequestInputFrame,
	ServiceGetRequestedInputValue,
	ServiceSetRequestedInputValue
};

// STUB: LEGORACERS2 0x004b1410
void InitializeControllerRecordingServiceStorage()
{
	ControllerRecordingService* service = g_controllerRecordingService;
	service->m_vtable = &g_controllerRecordingServiceDispatch;
	LegoU8* bytes = (LegoU8*) service;
	*(LegoU32*) (bytes + 0x3c) = 0xffffffff;
	*(LegoU32*) (bytes + 0x04) = 0;
	*(LegoU32*) (bytes + 0x08) = 0;
	*(LegoU32*) (bytes + 0x0c) = 0;
	*(LegoU32*) (bytes + 0x14) = 0;
	*(LegoU32*) (bytes + 0x18) = 0;
	*(LegoU32*) (bytes + 0x1c) = 0;
	*(LegoU32*) (bytes + 0x20) = 0;
	*(LegoU32*) (bytes + 0x28) = 0;
	*(LegoU32*) (bytes + 0x30) = 0;
	*(LegoU32*) (bytes + 0x38) = 0;
	*(LegoU32*) (bytes + 0x40) = 0;
	*(LegoU32*) (bytes + 0x44) = 0;
	*(LegoU32*) (bytes + 0x48) = 0;
	*(LegoU32*) (bytes + 0x4c) = 0;
	*(LegoU32*) (bytes + 0x5c) = 0;
	*(LegoU32*) (bytes + 0x60) = 0;
	*(LegoU32*) (bytes + 0x68) = 0;
	*(LegoU32*) (bytes + 0x6c) = 0;
	*(LegoU32*) (bytes + 0x70) = 0;
	*(LegoU32*) (bytes + 0x74) = 0;
	*(LegoU32*) (bytes + 0x78) = 0;
	*(LegoU32*) (bytes + 0x7c) = 0;
	*(LegoU16*) (bytes + 0x80) = 0x10;
	*(LegoU16*) (bytes + 0x82) = 0;
	*(LegoU32*) (bytes + 0x84) = 0;
	*(LegoU32*) (bytes + 0x88) = 0;
	*(LegoU32*) (bytes + 0x90) = 0;
	*(LegoU32*) (bytes + 0x94) = 0;
	*(LegoU16*) (bytes + 0x98) = 0x10;
	*(LegoU16*) (bytes + 0x9a) = 0;
	*(LegoU32*) (bytes + 0x9c) = 0;
	*(LegoU32*) (bytes + 0xa0) = 0;
	*(LegoU32*) (bytes + 0xb0) = 0;
	*(LegoU32*) (bytes + 0xb4) = 0;
	*(LegoU32*) (bytes + 0xb8) = 0;
	*(LegoU32*) (bytes + 0xbc) = 0;
	*(LegoU32*) (bytes + 0xc0) = 0;
	*(LegoU32*) (bytes + 0xc4) = 0;
	*(LegoU32*) (bytes + 0xc8) = 0;
	*(LegoU32*) (bytes + 0xcc) = 0;
	*(LegoU32*) (bytes + 0xd4) = 0;
	*(LegoU32*) (bytes + 0xd8) = 0;
}

// FUNCTION: LEGORACERS2 0x004b14e0
void ControllerRecordingService::SetUnknownWord0x2c(LegoU32 p_value)
{
	*(LegoU32*) ((LegoU8*) this + 0x2c) = p_value;
}

// FUNCTION: LEGORACERS2 0x004b14f0
void ControllerRecordingService::SetUnknownWord0xd8(LegoU32 p_value)
{
	*(LegoU32*) ((LegoU8*) this + 0xd8) = p_value;
}

// FUNCTION: LEGORACERS2 0x004b1500
LegoU32 ControllerRecordingService::GetUnknownWord0xd8()
{
	return *(LegoU32*) ((LegoU8*) this + 0xd8);
}

// FUNCTION: LEGORACERS2 0x004b1510
LegoU32 ControllerRecordingService::GetUnknownWord0x20()
{
	return *(LegoU32*) ((LegoU8*) this + 0x20);
}

// FUNCTION: LEGORACERS2 0x004b1520
LegoU32 ControllerRecordingService::GetRequestedInputValue()
{
	return m_requestedInputValue;
}

// FUNCTION: LEGORACERS2 0x004b1530
void ControllerRecordingService::SetRequestedInputValue(LegoU32 p_value)
{
	m_currentRecord = NULL;
	m_requestedInputValue = p_value;
}

// STUB: LEGORACERS2 0x004b1550
LegoBool32 ControllerRecordingService::Initialize(
	const ControllerRecordingServiceSettings* p_settings,
	void* p_callbackContext
)
{
	m_recordCapacity = p_settings->m_recordCapacity;
	m_sessionCapacity = p_settings->m_sessionCapacity;
	m_deviceCapacity = p_settings->m_deviceCapacity;
	m_serviceProviderCapacity = p_settings->m_serviceProviderCapacity;
	m_receivedInputCallback = p_callbackContext;

	m_records = (ControllerRecordingRecord*) GetMemoryHeap()->Allocate(
		m_recordCapacity * sizeof(ControllerRecordingRecord),
		"Network players"
	);
	m_sessionRecords =
		GetMemoryHeap()->Allocate(m_sessionCapacity * sizeof(ControllerRecordingSessionRecord), "Network sessions");
	m_deviceRecords = GetMemoryHeap()->Allocate(m_deviceCapacity * 0x90, "Network devices");
	m_serviceProviderRecords = GetMemoryHeap()->Allocate(m_serviceProviderCapacity * 0x9c, "Network service providers");

	((ControllerRecordingObjectMap*) ((LegoU8*) this + 0x70))
		->Initialize(
			p_settings->m_objectMapCapacity,
			p_settings->m_objectMapFirstSpan,
			p_settings->m_objectMapSecondSpan
		);
	((ControllerRecordingMessageQueue*) ((LegoU8*) this + 0x9c))->Initialize(p_settings->m_receiveBufferCapacity);
	((ControllerRecordingProviderMap*) ((LegoU8*) this + 0xc0))->Initialize(p_settings->m_providerMapCapacity);

	if (CoCreateInstance(CLSID_DirectPlay8Peer, NULL, 1, IID_IDirectPlay8Peer, (void**) &m_primaryProvider) != 0) {
		return FALSE;
	}
	if (CoCreateInstance(
			CLSID_DirectPlay8LobbiedApplication,
			NULL,
			1,
			IID_IDirectPlay8LobbiedApplication,
			(void**) &m_secondaryProvider
		) != 0) {
		return FALSE;
	}
	if (((IDirectPlay8Peer*) m_primaryProvider)->Initialize(NULL, ControllerRecordingPeerMessageHandler, 0) != 0) {
		return FALSE;
	}
	DPNHANDLE lobbyHandle = 0;
	if (((IDirectPlay8LobbiedApplication*) m_secondaryProvider)
			->Initialize(NULL, ControllerRecordingLobbyMessageHandler, &lobbyHandle, 0) != 0) {
		return FALSE;
	}
	*(LegoU32*) ((LegoU8*) this + 0x1c) = lobbyHandle != 0;
	m_providerAssociations.Initialize();
	m_currentRecord = NULL;
	return EnumerateServiceProviders();
}

// STUB: LEGORACERS2 0x004b1700
void ControllerRecordingService::Shutdown()
{
	IDirectPlay8LobbiedApplication* lobby = (IDirectPlay8LobbiedApplication*) m_secondaryProvider;
	if (lobby != NULL) {
		lobby->Close(0);
		lobby->Release();
		m_secondaryProvider = NULL;
	}
	IDirectPlay8Peer* peer = (IDirectPlay8Peer*) m_primaryProvider;
	if (peer != NULL) {
		((ControllerRecordingMessageQueue*) ((LegoU8*) this + 0x9c))->Shutdown();
		((ControllerRecordingProviderMap*) ((LegoU8*) this + 0xc0))->Shutdown(peer);
		((ControllerRecordingObjectMap*) ((LegoU8*) this + 0x70))->Shutdown();
		m_vtable->m_resetNegotiatedState(this, NULL);
		if (m_deviceAddress != NULL) {
			((IUnknown*) m_deviceAddress)->Release();
			m_deviceAddress = NULL;
		}
		if (m_serviceProviderAddress != NULL) {
			((IUnknown*) m_serviceProviderAddress)->Release();
			m_serviceProviderAddress = NULL;
		}
		peer->Release();
		m_primaryProvider = NULL;
	}
	if (m_serviceProviderRecords != NULL) {
		GetMemoryHeap()->Free(m_serviceProviderRecords);
		m_serviceProviderRecords = NULL;
	}
	if (m_records != NULL) {
		GetMemoryHeap()->Free(m_records);
		m_records = NULL;
	}
	if (m_sessionRecords != NULL) {
		GetMemoryHeap()->Free(m_sessionRecords);
		m_sessionRecords = NULL;
	}
	if (m_deviceRecords != NULL) {
		GetMemoryHeap()->Free(m_deviceRecords);
		m_deviceRecords = NULL;
	}
}

// STUB: LEGORACERS2 0x004b17e0
ControllerRecordingRecord* ControllerRecordingService::AllocateRecord()
{
	if (m_recordCount >= (LegoS32) m_recordCapacity) {
		return NULL;
	}

	ControllerRecordingRecord* record = &m_records[m_recordCount++];
	record->m_inputValues[0] = 0;
	*(LegoU32*) ((LegoU8*) record + 0x4c) = 0;
	return record;
}

// STUB: LEGORACERS2 0x004b1810
ControllerRecordingRecord* ControllerRecordingService::FindRecordByKey(LegoU32 p_key)
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_key == p_key) {
			return record;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004b1850
void ControllerRecordingService::RemoveRecordByKey(LegoU32 p_key)
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_key != p_key) {
			continue;
		}

		if (m_currentRecord == record) {
			m_currentRecord = NULL;
		}
		if (m_recordRemovedCallback != NULL) {
			LegoU32 maximumInput = 0;
			for (LegoS32 valueOffset = 0x144; valueOffset >= 0x48; valueOffset -= 4) {
				LegoU32 value = *(LegoU32*) ((LegoU8*) record + valueOffset);
				if (value > maximumInput && value != 0xffffffff) {
					maximumInput = value;
				}
			}
			((void(__cdecl*)(LegoU32, LegoU32)) m_recordRemovedCallback)(record->m_value, maximumInput);
		}

		for (LegoS32 moveIndex = index; moveIndex + 1 < m_recordCount; ++moveIndex) {
			memcpy(&m_records[moveIndex], &m_records[moveIndex + 1], sizeof(ControllerRecordingRecord));
		}
		--m_recordCount;
		return;
	}
}

// STUB: LEGORACERS2 0x004b1910
ControllerRecordingSessionRecord* ControllerRecordingService::AllocateSessionRecord()
{
	LegoS32 count = *(LegoS32*) ((LegoU8*) this + 0x58);
	LegoS32 capacity = *(LegoS32*) ((LegoU8*) this + 0x54);
	if (count >= capacity) {
		return NULL;
	}

	ControllerRecordingSessionRecord* record =
		(ControllerRecordingSessionRecord*) ((LegoU8*) *(void**) ((LegoU8*) this + 0x50) + count * 0xd0);
	record->m_peerAddress = NULL;
	record->m_lobbyAddress = NULL;
	*(LegoS32*) ((LegoU8*) this + 0x58) = count + 1;
	return record;
}

// STUB: LEGORACERS2 0x004b1960
void ControllerRecordingService::RemoveSessionRecord(ControllerRecordingSessionRecord* p_record)
{
	LegoS32 count = *(LegoS32*) ((LegoU8*) this + 0x58);
	LegoU8* records = *(LegoU8**) ((LegoU8*) this + 0x50);
	for (LegoS32 index = 0; index < count; ++index) {
		ControllerRecordingSessionRecord* record =
			(ControllerRecordingSessionRecord*) (records + index * sizeof(ControllerRecordingSessionRecord));
		if (record != p_record) {
			continue;
		}
		if (record->m_peerAddress != NULL) {
			((IUnknown*) record->m_peerAddress)->Release();
		}
		if (record->m_lobbyAddress != NULL) {
			((IUnknown*) record->m_lobbyAddress)->Release();
		}
		for (LegoS32 moveIndex = index; moveIndex + 1 < count; ++moveIndex) {
			memcpy(records + moveIndex * 0xd0, records + (moveIndex + 1) * 0xd0, 0xd0);
		}
		*(LegoS32*) ((LegoU8*) this + 0x58) = count - 1;
		return;
	}
}

// STUB: LEGORACERS2 0x004b19f0
LegoBool32 ControllerRecordingService::EnumerateServiceProviders()
{
	DWORD bufferBytes = 0;
	DWORD providerCount = 0;
	IDirectPlay8Peer* peer = (IDirectPlay8Peer*) m_primaryProvider;
	HRESULT result = peer->EnumServiceProviders(NULL, NULL, NULL, &bufferBytes, &providerCount, 0);
	if (result != DPN_OK && result != DPNERR_BUFFERTOOSMALL) {
		return FALSE;
	}

	const LegoChar allocationTag[] = "DirectPlay service providers";
	DPN_SERVICE_PROVIDER_INFO* providers =
		(DPN_SERVICE_PROVIDER_INFO*) GetMemoryHeap()->AllocateFromEnd(bufferBytes, allocationTag);
	result = peer->EnumServiceProviders(NULL, NULL, providers, &bufferBytes, &providerCount, 0);
	if (result != DPN_OK) {
		GetMemoryHeap()->Free(providers);
		return FALSE;
	}

	LegoS32 count = (LegoS32) providerCount;
	if (count > (LegoS32) m_serviceProviderCapacity) {
		count = (LegoS32) m_serviceProviderCapacity;
	}
	m_serviceProviderCount = count;

	static const GUID tcpProvider = {0xebfe7ba0, 0xd162, 0x11d2, {0xae, 0x0f, 0x00, 0x60, 0x97, 0xb0, 0x14, 0x11}};
	static const GUID ipxProvider = {0x743b5d60, 0xd162, 0x11d2, {0xae, 0x0f, 0x00, 0x60, 0x97, 0xb0, 0x14, 0x11}};
	static const GUID serialProvider = {0x6d4a3650, 0xd162, 0x11d2, {0xae, 0x0f, 0x00, 0x60, 0x97, 0xb0, 0x14, 0x11}};
	static const GUID modemProvider = {0x53934290, 0xd162, 0x11d2, {0xae, 0x0f, 0x00, 0x60, 0x97, 0xb0, 0x14, 0x11}};
	LegoU8* output = (LegoU8*) m_serviceProviderRecords;
	for (LegoS32 index = 0; index < count; ++index) {
		DPN_SERVICE_PROVIDER_INFO* input = &providers[index];
		LegoU8* record = output + index * 0x9c;
		memcpy(record, &input->guid, sizeof(GUID));
		WideCharToMultiByte(CP_ACP, 0, input->pwszName, -1, (LPSTR) (record + 0x10), 0x7f, NULL, NULL);

		if (memcmp(&input->guid, &tcpProvider, sizeof(GUID)) == 0) {
			if ((g_providerConfigurationInitialized & 1) == 0) {
				g_providerConfigurationInitialized |= 1;
				g_tcpProviderOptions[0].m_type = 2;
				wcscpy(g_tcpProviderOptions[0].m_name, L"port");
			}
			*(LegoU32*) (record + 0x90) = 0;
			*(LegoU32**) (record + 0x94) = (LegoU32*) g_tcpProviderOptions;
			*(LegoU32*) (record + 0x98) = 1;
		}
		else if (memcmp(&input->guid, &ipxProvider, sizeof(GUID)) == 0) {
			*(LegoU32*) (record + 0x90) = 1;
			*(LegoU32*) (record + 0x94) = 0;
			*(LegoU32*) (record + 0x98) = 0;
		}
		else if (memcmp(&input->guid, &serialProvider, sizeof(GUID)) == 0) {
			if ((g_providerConfigurationInitialized & 2) == 0) {
				g_providerConfigurationInitialized |= 2;
				wcscpy(g_serialProviderOptions[0].m_name, L"device");
				g_serialProviderOptions[0].m_type = 3;
				wcscpy(g_serialProviderOptions[1].m_name, L"baud");
				g_serialProviderOptions[1].m_type = 2;
				wcscpy(g_serialProviderOptions[2].m_name, L"flowcontrol");
				g_serialProviderOptions[2].m_type = 1;
				wcscpy(g_serialProviderOptions[3].m_name, L"parity");
				g_serialProviderOptions[3].m_type = 1;
				wcscpy(g_serialProviderOptions[4].m_name, L"stopbits");
				g_serialProviderOptions[4].m_type = 1;
			}
			*(LegoU32*) (record + 0x90) = 2;
			*(LegoU32**) (record + 0x94) = (LegoU32*) g_serialProviderOptions;
			*(LegoU32*) (record + 0x98) = 5;
		}
		else if (memcmp(&input->guid, &modemProvider, sizeof(GUID)) == 0) {
			if ((g_providerConfigurationInitialized & 4) == 0) {
				g_providerConfigurationInitialized |= 4;
				wcscpy(g_modemProviderOptions[0].m_name, L"device");
				g_modemProviderOptions[0].m_type = 3;
				wcscpy(g_modemProviderOptions[1].m_name, L"baud");
				g_modemProviderOptions[1].m_type = 2;
				wcscpy(g_modemProviderOptions[2].m_name, L"flowcontrol");
				g_modemProviderOptions[2].m_type = 1;
				wcscpy(g_modemProviderOptions[3].m_name, L"parity");
				g_modemProviderOptions[3].m_type = 1;
				wcscpy(g_modemProviderOptions[4].m_name, L"stopbits");
				g_modemProviderOptions[4].m_type = 1;
				wcscpy(g_modemProviderOptions[5].m_name, L"phonenumber");
				g_modemProviderOptions[5].m_type = 1;
			}
			*(LegoU32*) (record + 0x90) = 3;
			*(LegoU32**) (record + 0x94) = (LegoU32*) g_modemProviderOptions;
			*(LegoU32*) (record + 0x98) = 6;
		}
		else {
			*(LegoU32*) (record + 0x90) = 4;
			*(LegoU32*) (record + 0x94) = 0;
			*(LegoU32*) (record + 0x98) = 0;
		}
	}

	GetMemoryHeap()->Free(providers);
	return TRUE;
}

// STUB: LEGORACERS2 0x004b1db0
LegoBool32 ControllerRecordingService::EnumerateDevicesForProvider(LegoS32 p_providerIndex)
{
	DWORD bufferSize = *(LegoU32*) ((LegoU8*) this + 0x64) * sizeof(DPN_SERVICE_PROVIDER_INFO);
	DWORD deviceCount = 0;
	const LegoChar allocationTag[] = "Temporary network devices";
	DPN_SERVICE_PROVIDER_INFO* providers =
		(DPN_SERVICE_PROVIDER_INFO*) GetMemoryHeap()->AllocateFromEnd(bufferSize, allocationTag);

	const GUID* providerGuid = (const GUID*) ((LegoU8*) *(void**) ((LegoU8*) this + 0x30) + p_providerIndex * 0x9c);
	HRESULT result = ((IDirectPlay8Peer*) m_primaryProvider)
						 ->EnumServiceProviders(providerGuid, NULL, providers, &bufferSize, &deviceCount, 0);
	if (result != DPN_OK) {
		GetMemoryHeap()->Free(providers);
		return FALSE;
	}

	*(LegoU32*) ((LegoU8*) this + 0x68) = deviceCount;
	LegoU8* outputRecords = *(LegoU8**) ((LegoU8*) this + 0x60);
	for (LegoU32 index = 0; index < deviceCount; ++index) {
		LegoU8* output = outputRecords + index * 0x90;
		memcpy(output, &providers[index].guid, sizeof(GUID));
		WideCharToMultiByte(CP_ACP, 0, providers[index].pwszName, -1, (LPSTR) (output + 0x10), 0x7f, NULL, NULL);
	}

	GetMemoryHeap()->Free(providers);
	return TRUE;
}

// STUB: LEGORACERS2 0x004b1ee0
void ControllerRecordingService::EnumerateHosts()
{
	m_vtable->m_cancelPeerOperation(this, NULL);

	DPN_APPLICATION_DESC application;
	memset(&application, 0, sizeof(application));
	application.dwSize = sizeof(application);
	*(LegoU32*) ((LegoU8*) &application + 0x10) = (LegoU32) m_serviceProviderAddress;
	application.guidApplication = g_controllerRecordingApplicationGuid;

	LegoU32 iteration = *(LegoU32*) ((LegoU8*) this + 0x4c) + 1;
	*(LegoU32*) ((LegoU8*) this + 0x4c) = iteration;
	m_sessionCount = 0;
	((IDirectPlay8Peer*) m_primaryProvider)
		->EnumHosts(
			&application,
			(IDirectPlay8Address*) m_serviceProviderAddress,
			(IDirectPlay8Address*) m_deviceAddress,
			NULL,
			0,
			0xffffffff,
			0,
			0xffffffff,
			(void*) iteration,
			(DPNHANDLE*) &m_pendingPeerOperation,
			0
		);
}

// FUNCTION: LEGORACERS2 0x004b1f80
void ControllerRecordingService::CancelPeerOperation()
{
	if (m_pendingPeerOperation != NULL) {
		((IDirectPlay8Peer*) m_primaryProvider)->CancelAsyncOperation((DPNHANDLE) m_pendingPeerOperation, 2);
		m_pendingPeerOperation = NULL;
	}
}

// STUB: LEGORACERS2 0x004b1fa0
LegoBool32 ControllerRecordingService::SelectServiceProvider(
	LegoS32 p_providerIndex,
	const LegoChar** p_optionValues,
	LegoU32 p_optionCount
)
{
	if (m_serviceProviderCount <= p_providerIndex) {
		return FALSE;
	}
	m_selectedServiceProvider = p_providerIndex;
	LegoU8* provider = (LegoU8*) m_serviceProviderRecords + p_providerIndex * 0x9c;
	IDirectPlay8Address* serviceAddress = NULL;
	if (CoCreateInstance(
			CLSID_DirectPlay8Address,
			NULL,
			1,
			IID_IDirectPlay8Address,
			(void**) &m_serviceProviderAddress
		) != 0) {
		return FALSE;
	}
	if (CoCreateInstance(CLSID_DirectPlay8Address, NULL, 1, IID_IDirectPlay8Address, (void**) &m_deviceAddress) != 0) {
		return FALSE;
	}
	serviceAddress = (IDirectPlay8Address*) m_serviceProviderAddress;
	if (serviceAddress->SetSP((const GUID*) provider) != 0) {
		return FALSE;
	}
	if (((IDirectPlay8Address*) m_deviceAddress)->SetSP((const GUID*) provider) != 0) {
		return FALSE;
	}

	LegoU32 optionCount = *(LegoU32*) (provider + 0x98);
	if (p_optionCount != optionCount) {
		return FALSE;
	}
	const ControllerRecordingProviderOption* options =
		(const ControllerRecordingProviderOption*) *(void**) (provider + 0x94);
	LegoChar optionText[0x100];
	for (LegoU32 index = 0; index < optionCount; ++index) {
		LegoU32 optionType = options[index].m_type;
		if (optionType == 1) {
			strcpy(optionText, p_optionValues[index]);
		}
		else if (optionType == 2) {
			atol(p_optionValues[index]);
		}
		else if (optionType == 3) {
			LegoS32 deviceIndex = atol(p_optionValues[index]);
			if (m_deviceCount <= deviceIndex) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004b2160
LegoBool32 ControllerRecordingService::HostSession(const LegoChar* p_sessionName)
{
	if (m_selectedServiceProvider == -1) {
		return FALSE;
	}

	WCHAR playerName[24];
	DPN_PLAYER_INFO playerInfo;
	memset(&playerInfo, 0, sizeof(playerInfo));
	playerInfo.dwSize = sizeof(playerInfo);
	playerInfo.dwInfoFlags = DPNINFO_NAME;
	MultiByteToWideChar(CP_ACP, 0, "UNINITIALISED", -1, playerName, 0x2f);
	playerInfo.pwszName = playerName;
	IDirectPlay8Peer* peer = (IDirectPlay8Peer*) m_primaryProvider;
	if (peer->SetPeerInfo(&playerInfo, NULL, NULL, 0x80000000) != DPN_OK) {
		return FALSE;
	}

	DPN_APPLICATION_DESC application;
	memset(&application, 0, sizeof(application));
	application.dwSize = sizeof(application);
	application.guidApplication = g_controllerRecordingApplicationGuid;
	application.dwMaxPlayers = m_recordCapacity;
	WCHAR sessionName[256];
	MultiByteToWideChar(CP_ACP, 0, p_sessionName, -1, sessionName, 0x1ff);
	application.pwszSessionName = sessionName;
	if (peer->Host(&application, (IDirectPlay8Address**) &m_deviceAddress, 1, NULL, NULL, NULL, 0) != DPN_OK) {
		return FALSE;
	}

	m_unk0x20 = 2;
	m_unk0xd8 = 0;
	return TRUE;
}

// STUB: LEGORACERS2 0x004b22c0
LegoBool32 ControllerRecordingService::ConnectToSession(LegoS32 p_sessionIndex)
{
	m_vtable->m_cancelPeerOperation(this, NULL);
	if (m_selectedServiceProvider == -1 || m_sessionCount <= p_sessionIndex) {
		return FALSE;
	}

	ControllerRecordingSessionRecord* record =
		(ControllerRecordingSessionRecord*) ((LegoU8*) m_sessionRecords + p_sessionIndex * 0xd0);
	HRESULT result = ((IDirectPlay8Peer*) m_primaryProvider)
						 ->Connect(
							 (DPN_APPLICATION_DESC*) record,
							 (IDirectPlay8Address*) record->m_peerAddress,
							 (IDirectPlay8Address*) record->m_lobbyAddress,
							 NULL,
							 NULL,
							 NULL,
							 0,
							 NULL,
							 NULL,
							 (DPNHANDLE*) &m_pendingPeerOperation,
							 0
						 );
	if (result != DPNSUCCESS_PENDING) {
		return FALSE;
	}
	m_unk0x20 = 1;
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004b2340
LegoBool32 ControllerRecordingService::ResetNegotiatedState()
{
	m_unk0x20 = 0;
	if (((IDirectPlay8Peer*) m_primaryProvider)->Close(0) != 0) {
		return FALSE;
	}
	m_unk0xd8 = 0;
	return TRUE;
}

// STUB: LEGORACERS2 0x004b2370
LegoBool32 ControllerRecordingService::RestartPeerSession()
{
	m_unk0x20 = 0;
	IDirectPlay8Peer* peer = (IDirectPlay8Peer*) m_primaryProvider;
	if (peer->TerminateSession(NULL, 0, 0) != 0) {
		return FALSE;
	}
	if (peer->Close(0) != 0) {
		return FALSE;
	}
	if (peer->Initialize(NULL, ControllerRecordingPeerMessageHandler, 0) != 0) {
		return FALSE;
	}
	m_unk0xd8 = 0;
	return TRUE;
}

// STUB: LEGORACERS2 0x004b23e0
LegoBool32 ControllerRecordingService::InitializePeer()
{
	IDirectPlay8Peer* peer = (IDirectPlay8Peer*) m_primaryProvider;
	if (peer->Close(0) != 0) {
		return FALSE;
	}
	return peer->Initialize(NULL, ControllerRecordingPeerMessageHandler, 0) == 0;
}

// FUNCTION: LEGORACERS2 0x004b2410
LegoBool32 ControllerRecordingService::WaitForInput()
{
	ControllerRecordingMessageQueue* queue = (ControllerRecordingMessageQueue*) ((LegoU8*) this + 0x9c);
	ControllerRecordingMessageRecord* message;
	while (queue->Dequeue(&message) == TRUE) {
		HandleDirectPlayMessage(message);
	}
	m_providerAssociations.PruneStaleAssociations();
	((ControllerRecordingProviderMap*) ((LegoU8*) this + 0xc0))->PruneExpiredEntries();
	return TRUE;
}

// STUB: LEGORACERS2 0x004b2470
LegoBool32 ControllerRecordingService::CopyDeviceNames(LegoChar* p_output, LegoS32* p_count)
{
	LegoS32 count = *(LegoS32*) ((LegoU8*) this + 0x68);
	if (*p_count <= count) {
		count = *p_count;
	}
	*p_count = count;

	LegoU8* records = *(LegoU8**) ((LegoU8*) this + 0x60);
	for (LegoS32 index = 0; index < count; ++index) {
		strcpy(p_output + index * 0x80, (LegoChar*) (records + index * 0x90 + 0x10));
	}

	return (LegoS32) * (LegoS32*) ((LegoU8*) this + 0x68) <= *p_count;
}

// STUB: LEGORACERS2 0x004b24f0
LegoBool32 ControllerRecordingService::CopySessionNames(LegoChar* p_output, LegoS32* p_count)
{
	LegoS32 count = *(LegoS32*) ((LegoU8*) this + 0x38);
	if (*p_count <= count) {
		count = *p_count;
	}
	*p_count = count;

	LegoU8* records = *(LegoU8**) ((LegoU8*) this + 0x30);
	for (LegoS32 index = 0; index < count; ++index) {
		LegoU8* record = records + index * 0x9c;
		LegoU8* output = (LegoU8*) p_output + index * 0x84;
		strcpy((LegoChar*) output, (LegoChar*) (record + 0x10));
		*(LegoU32*) (output + 0x80) = *(LegoU32*) (record + 0x90);
	}

	return (LegoS32) * (LegoS32*) ((LegoU8*) this + 0x38) <= *p_count;
}

// STUB: LEGORACERS2 0x004b2580
LegoBool32 ControllerRecordingService::CopySessionRecords(LegoU8* p_output, LegoS32* p_count)
{
	volatile LegoU32* lock = (volatile LegoU32*) ((LegoU8*) this + 0xd4);
	while (*lock == 1) {
	}
	*lock = 1;

	LegoS32 actualCount = *(LegoS32*) ((LegoU8*) this + 0x58);
	LegoS32 count = actualCount;
	if (*p_count <= count) {
		count = *p_count;
	}
	*p_count = count;

	LegoU8* records = *(LegoU8**) ((LegoU8*) this + 0x50);
	for (LegoS32 index = 0; index < count; ++index) {
		LegoU8* record = records + index * 0xd0;
		LegoU8* output = p_output + index * 0x8c;
		strcpy((LegoChar*) output, (LegoChar*) (record + 0x50));
		*(LegoU32*) (output + 0x80) = *(LegoU32*) (record + 0x28);
		*(LegoU32*) (output + 0x84) = *(LegoU32*) (record + 0x2c);
		*(LegoU32*) (output + 0x88) = 0;
	}

	*lock = 0;
	return actualCount <= *p_count;
}

// STUB: LEGORACERS2 0x004b2670
LegoU32 ControllerRecordingService::FindRecordKeyByValue(LegoU32 p_value)
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_value == p_value) {
			return record->m_key;
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x004b26b0
LegoU32 ControllerRecordingService::GetOccupiedRecordKey()
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_occupied == 1) {
			return record->m_key;
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x004b26f0
ControllerRecordingRecord* ControllerRecordingService::FindRecordForCurrentKey()
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_key == m_currentKey) {
			return record;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004b2730
void ControllerRecordingService::SetCurrentRecordName(LegoChar* p_name)
{
	ControllerRecordingRecord* record = FindRecordForCurrentKey();
	if (record == NULL || p_name == NULL) {
		return;
	}

	LegoU32 length = 0;
	while (p_name[length] != '\0') {
		length++;
	}
	if (length > 0x1f) {
		p_name[0x1f] = '\0';
		length = 0x1f;
	}
	for (LegoU32 index = 0; index < length; ++index) {
		record->m_name[index] = p_name[index];
	}
	record->m_name[length] = '\0';
}

// FUNCTION: LEGORACERS2 0x004b2790
void ControllerRecordingService::SetRecordValue(LegoU32 p_key, LegoU32 p_value)
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_key == p_key) {
			record->m_value = p_value;
		}
	}
}

// STUB: LEGORACERS2 0x004b27d0
LegoU32 ControllerRecordingService::GetCurrentRecordValue()
{
	ControllerRecordingRecord* record = FindRecordForCurrentKey();
	return record == NULL ? 0 : record->m_value;
}

// FUNCTION: LEGORACERS2 0x004b27e0
LegoU32 ControllerRecordingService::GetCurrentKey()
{
	return m_currentKey;
}

// STUB: LEGORACERS2 0x004b27f0
LegoU32 ControllerRecordingService::FindOccupiedRecordValue()
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_occupied == 1) {
			return record->m_value;
		}
	}
	return 0;
}

// FUNCTION: LEGORACERS2 0x004b2830
void ControllerRecordingService::CopyRecordValues(LegoU32* p_values, LegoU32* p_count)
{
	*p_count = (LegoU32) m_recordCount;
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		p_values[index] = m_records[index].m_value;
	}
}

// STUB: LEGORACERS2 0x004b2870
void ControllerRecordingService::AssignRecordOwner(LegoU32 p_value, InputController* p_controller)
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_value == p_value) {
			record->m_ownerKey = (LegoU32) p_controller;
			p_controller->m_disabled = p_value == m_vtable->m_getCurrentRecordValue(this, NULL) ? 0 : 1;
		}
	}
}

// FUNCTION: LEGORACERS2 0x004b28e0
void ControllerRecordingService::CopyInputStatistics(LegoU32* p_output)
{
	DWORD queuedMessages = 0;
	DWORD queuedBytes = 0;
	((IDirectPlay8Peer*) m_primaryProvider)->GetSendQueueInfo(m_currentKey, &queuedMessages, &queuedBytes, 0);
	p_output[0] = queuedMessages;
	p_output[1] = queuedBytes;
	p_output[2] = *(LegoU32*) ((LegoU8*) this + 0xbc);
	p_output[3] = *(LegoU32*) ((LegoU8*) this + 0xb4) - *(LegoU32*) ((LegoU8*) this + 0xb8);
}

// FUNCTION: LEGORACERS2 0x004f2fd0
static HRESULT WINAPI ControllerRecordingPeerMessageHandler(PVOID p_userContext, DWORD p_messageId, PVOID p_message)
{
	return (HRESULT) ((ControllerRecordingService*) g_controllerRecordingServiceStorage)
		->DispatchDirectPlayMessage((LegoU32) p_userContext, p_messageId, p_message, 0);
}

// FUNCTION: LEGORACERS2 0x004f2ff0
static HRESULT WINAPI ControllerRecordingLobbyMessageHandler(PVOID p_userContext, DWORD p_messageId, PVOID p_message)
{
	((ControllerRecordingService*) g_controllerRecordingServiceStorage)
		->DispatchDirectPlayMessage((LegoU32) p_userContext, p_messageId, p_message, 1);
	return 0;
}

// STUB: LEGORACERS2 0x004f3010
LegoU32 ControllerRecordingService::DispatchDirectPlayMessage(
	LegoU32 p_userContext,
	LegoU32 p_messageId,
	void* p_message,
	LegoU32 p_flags
)
{
	ControllerRecordingMessageRecord record;
	record.m_userContext = p_userContext;
	record.m_messageId = p_messageId;
	record.m_message = p_message;
	record.m_payloadSize = GetDirectPlayMessagePayloadSize(p_messageId, p_flags);
	record.m_flags = p_flags;

	if ((LegoS32) record.m_payloadSize > 0) {
		ControllerRecordingMessageQueue* queue = (ControllerRecordingMessageQueue*) ((LegoU8*) this + 0x9c);
		queue->Enqueue(&record);
		return 0;
	}
	return HandleDirectPlayMessage(&record);
}

// STUB: LEGORACERS2 0x004f3080
LegoU32 __stdcall GetDirectPlayMessagePayloadSize(LegoU32 p_messageId, LegoU32 p_flags)
{
	if (p_flags == 1) {
		return 0;
	}

	switch (p_messageId) {
	case 0xffff0005:
	case 0xffff0011:
		return 0x18;
	case 0xffff0007:
	case 0xffff000d:
		return 0x0c;
	case 0xffff0009:
	case 0xffff0016:
		return 0x10;
	case 0xffff0014:
		return 0x14;
	default:
		return 0;
	}
}

// STUB: LEGORACERS2 0x004f3100
LegoU32 ControllerRecordingService::HandleDirectPlayMessage(void* p_messageRecord)
{
	ControllerRecordingMessageRecord* record = (ControllerRecordingMessageRecord*) p_messageRecord;
	if (record->m_flags == 1) {
		return 0;
	}

	LegoU8* message = (LegoU8*) record->m_message;
	switch (record->m_messageId) {
	case DPN_MSGID_CONNECT_COMPLETE: {
		DPNMSG_CONNECT_COMPLETE* connect = (DPNMSG_CONNECT_COMPLETE*) message;
		*(LegoU32*) ((LegoU8*) this + 0x5c) = 0;
		m_unk0x20 = connect->hResultCode != 0 ? 0 : 2;
		return 0;
	}
	case DPN_MSGID_CREATE_PLAYER: {
		DPNMSG_CREATE_PLAYER* create = (DPNMSG_CREATE_PLAYER*) message;
		DWORD infoSize = 0;
		IDirectPlay8Peer* peer = (IDirectPlay8Peer*) m_primaryProvider;
		HRESULT result = peer->GetPeerInfo(create->dpnidPlayer, NULL, &infoSize, 0);
		if (result != DPN_OK && result != DPNERR_BUFFERTOOSMALL) {
			break;
		}

		const LegoChar allocationTag[] = "DirectPlay peer info";
		DPN_PLAYER_INFO* info = (DPN_PLAYER_INFO*) GetMemoryHeap()->AllocateFromEnd(infoSize, allocationTag);
		memset(info, 0, infoSize);
		info->dwSize = 0x18;
		result = peer->GetPeerInfo(create->dpnidPlayer, info, &infoSize, 0);
		if (result == DPN_OK) {
			ControllerRecordingRecord* newRecord = AllocateRecord();
			newRecord->m_key = create->dpnidPlayer;
			if (info->pwszName != NULL) {
				WideCharToMultiByte(CP_ACP, 0, info->pwszName, -1, newRecord->m_name, 0x1f, NULL, NULL);
			}
			newRecord->m_inputCount = 0;
			newRecord->m_count = 0;
			for (LegoS32 index = 0; index < 0x40; ++index) {
				newRecord->m_inputValues[index] = 0xffffffff;
				newRecord->m_frameNumbers[index] = 0xffffffff;
			}
			LegoU32 newValue = (LegoU32) m_recordCount;
			++m_recordCount;
			newRecord->m_value = newValue;
			newRecord->m_occupied = (info->dwPlayerFlags & 4) != 0 ? 1 : 0;
			if ((info->dwPlayerFlags & 2) != 0) {
				m_currentKey = create->dpnidPlayer;
			}
			else if (GetOccupiedRecordKey() == m_currentKey) {
				RecordPeerState(create->dpnidPlayer, newValue);
			}
		}
		GetMemoryHeap()->Free(info);
		return 0;
	}
	case DPN_MSGID_DESTROY_PLAYER: {
		DPNMSG_DESTROY_PLAYER* destroy = (DPNMSG_DESTROY_PLAYER*) message;
		RemoveRecordByKey(destroy->dpnidPlayer);
		return 0;
	}
	case DPN_MSGID_ENUM_HOSTS_QUERY:
	case DPN_MSGID_INDICATE_CONNECT:
		return m_unk0xd8 == 0 ? 0x80004005 : 0;
	case DPN_MSGID_ENUM_HOSTS_RESPONSE: {
		DPNMSG_ENUM_HOSTS_RESPONSE* response = (DPNMSG_ENUM_HOSTS_RESPONSE*) message;
		DPN_APPLICATION_DESC* application = (DPN_APPLICATION_DESC*) response->pApplicationDescription;
		volatile LegoU32* lock = (volatile LegoU32*) ((LegoU8*) this + 0xd4);
		while (*lock == 1) {
		}
		*lock = 1;

		ControllerRecordingSessionRecord* session = NULL;
		LegoS32 sessionCount = *(LegoS32*) ((LegoU8*) this + 0x58);
		LegoU8* sessions = *(LegoU8**) ((LegoU8*) this + 0x50);
		for (LegoS32 index = 0; index < sessionCount; ++index) {
			LegoU8* candidate = sessions + index * 0xd0;
			if (memcmp(candidate + 8, (LegoU8*) application + 8, sizeof(GUID)) == 0) {
				session = (ControllerRecordingSessionRecord*) candidate;
				if (session->m_peerAddress != NULL) {
					((IUnknown*) session->m_peerAddress)->Release();
					session->m_peerAddress = NULL;
				}
				if (session->m_lobbyAddress != NULL) {
					((IUnknown*) session->m_lobbyAddress)->Release();
					session->m_lobbyAddress = NULL;
				}
				break;
			}
		}
		if (session == NULL) {
			session = AllocateSessionRecord();
		}
		if (session != NULL) {
			if (response->pAddressSender->Duplicate((IDirectPlay8Address**) &session->m_peerAddress) != DPN_OK) {
				RemoveSessionRecord(session);
				return 0;
			}
			if (response->pAddressDevice->Duplicate((IDirectPlay8Address**) &session->m_lobbyAddress) != DPN_OK) {
				RemoveSessionRecord(session);
				return 0;
			}
			if (application->pwszSessionName != NULL) {
				WideCharToMultiByte(
					CP_ACP,
					0,
					application->pwszSessionName,
					-1,
					(LegoChar*) ((LegoU8*) session + 0x50),
					0x7f,
					NULL,
					NULL
				);
			}
			memcpy(session, application, 0x48);
			*(LegoU32*) ((LegoU8*) session + 0x30) = 0;
			*(LegoU32*) ((LegoU8*) session + 0x34) = 0;
		}
		*lock = 0;
		return 0;
	}
	case DPN_MSGID_HOST_MIGRATE: {
		DPNMSG_HOST_MIGRATE* migrate = (DPNMSG_HOST_MIGRATE*) message;
		if (migrate->dpnidNewHost == m_currentKey) {
			PrintDiagnostic("Asked to become the new host");
		}
		return 0;
	}
	case DPN_MSGID_RECEIVE: {
		DPNMSG_RECEIVE* receive = (DPNMSG_RECEIVE*) message;
		ControllerRecordingRecord* sender = FindRecordByKey(receive->dpnidSender);
		if (sender != NULL && m_receivedInputCallback != NULL) {
			const LegoU32* input = (const LegoU32*) ((LegoU8*) message + 0x20);
			if ((LegoS32) input[0] < 0) {
				ProcessSpecialInputCommand(input, receive->dwReceiveDataSize - 8, sender->m_value);
				return 0;
			}
			((void(__cdecl*)(const LegoU32*, LegoU32, LegoU32))
				 m_receivedInputCallback)(input, receive->dwReceiveDataSize - 8, sender->m_value);
		}
		break;
	}
	case DPN_MSGID_SEND_COMPLETE: {
		DPNMSG_SEND_COMPLETE* complete = (DPNMSG_SEND_COMPLETE*) message;
		LegoU32 userContext = (LegoU32) complete->pvUserContext;
		((ControllerRecordingProviderMap*) ((LegoU8*) this + 0xc0))
			->SetState((LegoS16) userContext, (LegoU32) complete->hResultCode);
		((ControllerRecordingObjectMap*) ((LegoU8*) this + 0x70))->ClearMatchingId((LegoS16) userContext, 1);
		return 0;
	}
	case DPN_MSGID_TERMINATE_SESSION:
		m_unk0x20 = 0;
		return 0;
	}
	return 0;
}

// STUB: LEGORACERS2 0x004f3550
void ControllerRecordingService::ProcessSpecialInputCommand(const LegoU32* p_command, LegoU32, LegoU32 p_recordValue)
{
	LegoS32 command = (LegoS32) p_command[0];
	if (command < -9) {
		if (command == -10) {
			SetRecordValue(p_command[1], p_command[2]);
			return;
		}
		if (command == -1000) {
			SendRequestedInput(p_command, p_recordValue);
			return;
		}
		if (command == -100) {
			ReceiveRemoteInput(p_command, p_recordValue);
			return;
		}
	}
	else if (command == -1) {
		SetRecordValue(p_command[1], p_command[2]);
		if (m_vtable->m_getCurrentRecordValue(this, NULL) != p_command[2]) {
			LegoU32 request[3];
			request[0] = 0xfffffff6;
			request[1] = GetCurrentKey();
			request[2] = m_vtable->m_getCurrentRecordValue(this, NULL);
			LegoU32* providerKey = m_providerAssociations.AllocateAssociation();
			m_vtable->m_processValue(this, NULL, request, 0x0c, 3, p_command[2], providerKey);
		}
	}
}

// STUB: LEGORACERS2 0x004f3620
void ControllerRecordingService::RecordPeerState(LegoU32 p_playerId, LegoU32 p_recordIndex)
{
	LegoU32 request[3];
	request[0] = 0xffffffff;
	request[1] = p_playerId;
	request[2] = p_recordIndex;
	LegoU32* providerKey = m_providerAssociations.AllocateAssociation();
	m_vtable->m_processValue(this, NULL, request, 0x0c, 0x0b, p_recordIndex, providerKey);
}

// STUB: LEGORACERS2 0x004f3670
LegoU32 ControllerRecordingService::ProcessValue(
	const LegoU8* p_payload,
	LegoU32 p_payloadBytes,
	LegoU8 p_flags,
	LegoU32 p_recordValue,
	LegoU32* p_payloadKey
)
{
	if (m_unk0x20 != 2) {
		return 3;
	}

	LegoU32 targetId = 0;
	if ((p_flags & 8) == 0) {
		if ((p_flags & 0x10) != 0) {
			targetId = GetOccupiedRecordKey();
		}
		else {
			targetId = FindRecordKeyByValue(p_recordValue);
		}
	}

	ControllerRecordingObjectMap* objectMap = (ControllerRecordingObjectMap*) ((LegoU8*) this + 0x70);
	LegoU8* packet = (LegoU8*) objectMap->CreatePacket(p_payload, p_payloadBytes);
	if (packet == NULL) {
		return 3;
	}

	LegoU16 packetTag = (LegoU16) ((LegoU32) packet >> 16);
	if ((p_flags & 4) != 0) {
		LegoU32 predicate = IsCollisionSweepPositionClear((CollisionProxy*) targetId);
		if (predicate == 1) {
			return 4;
		}
		packetTag = (LegoU16) (predicate >> 16);
	}

	LegoU32 providerKey = ((LegoU32) packetTag << 16) | *(LegoU16*) (packet + 6);
	ControllerRecordingProviderMap* providerMap = (ControllerRecordingProviderMap*) ((LegoU8*) this + 0xc0);
	ControllerRecordingProviderEntry* providerEntry = NULL;
	LegoU32 status = 1;
	LegoBool32 hasProviderAssociation = (p_flags & 0x20) == 0;
	if (!hasProviderAssociation) {
		status = 0x80000001;
	}
	else {
		providerEntry = providerMap->Add((LegoU16) providerKey);
	}

	if ((p_flags & 1) != 0) {
		status |= 0x0c;
	}
	if ((p_flags & 2) == 0) {
		status |= 0x10;
	}
	if ((p_flags & 0x40) != 0) {
		status |= 0x20;
	}

	DPN_BUFFER_DESC buffer;
	buffer.dwBufferSize = *(LegoU32*) packet;
	buffer.pBufferData = packet;
	LegoU16 payloadKey = *(LegoU16*) (packet + 6);
	*p_payloadKey = payloadKey;
	LegoU32 packetSize = payloadKey;
	DPNHANDLE* operationId = (DPNHANDLE*) ((LegoU8*) providerEntry + 4);

	HRESULT result = ((IDirectPlay8Peer*) m_primaryProvider)
						 ->SendTo(targetId, &buffer, 1, m_unk0x6c, (void*) packetSize, operationId, status);
	if (result != 0 && result != (HRESULT) 0x0015800e) {
		return 3;
	}
	if (!hasProviderAssociation) {
		return 2;
	}
	return providerMap->FindValue((LegoS16) payloadKey);
}

// FUNCTION: LEGORACERS2 0x004f37d0
LegoU32 ControllerRecordingService::GetProviderKeyValue(LegoU32 p_key)
{
	ControllerRecordingProviderMap* map = (ControllerRecordingProviderMap*) ((LegoU8*) this + 0xc0);
	return map->FindValue((LegoS16) p_key);
}

// FUNCTION: LEGORACERS2 0x004f37f0
void ControllerRecordingService::RemoveProviderKey(LegoU32 p_key)
{
	ControllerRecordingProviderMap* providerMap = (ControllerRecordingProviderMap*) ((LegoU8*) this + 0xc0);
	ControllerRecordingObjectMap* objectMap = (ControllerRecordingObjectMap*) ((LegoU8*) this + 0x70);
	providerMap->Remove((LegoS16) p_key);
	objectMap->ClearMatchingId((LegoS16) p_key, 0);
}

// STUB: LEGORACERS2 0x004f3820
void ControllerRecordingObjectMap::Initialize(LegoS32 p_capacity, LegoS32 p_firstSpan, LegoS32 p_secondSpan)
{
	LegoS32 firstDimension = (LegoS32) ((LegoU32) (p_secondSpan + 8) >> 1);
	LegoS32 secondDimension = p_secondSpan + 8;
	LegoS32 quarterCapacity = p_capacity >> 2;
	LegoS32 halfCapacity = p_capacity >> 1;
	m_count = p_capacity & 0xfc;
	m_unk0x10 = (LegoU32) firstDimension;
	m_unk0x14 = (LegoU32) secondDimension;
	m_unk0x18 = (LegoU32) (p_firstSpan + 8);
	m_unk0x1c = (LegoU32) quarterCapacity;
	m_unk0x20 = (LegoU32) halfCapacity;
	m_unk0x24 = (LegoU32) quarterCapacity;

	LegoU32 allocationSize = (LegoU32) (firstDimension + p_firstSpan + 8) * (LegoU32) quarterCapacity +
							 (LegoU32) halfCapacity * (LegoU32) secondDimension;
	const LegoChar allocationTag[] = "Network packets";
	m_allocation = (ControllerRecordingObjectEntry*) GetMemoryHeap()->Allocate(allocationSize, allocationTag);
	m_entries = (ControllerRecordingObjectEntry*) AllocateHeapMemory((LegoU32) p_capacity * 0x0c);

	if (m_entries != NULL) {
		for (LegoS32 index = 0; index < p_capacity; ++index) {
			m_entries[index].m_object = NULL;
			m_entries[index].m_active = 0;
			m_entries[index].m_unk0x08 = 0;
		}
	}

	ControllerRecordingObjectEntry* entry = m_entries;
	LegoU8* cursor = (LegoU8*) m_allocation;
	for (LegoS32 firstIndex = 0; firstIndex < quarterCapacity; ++firstIndex) {
		entry->m_active = 0;
		entry->m_unk0x08 = (LegoU32) firstDimension;
		entry->m_object = cursor;
		cursor += firstDimension;
		++entry;
	}
	for (LegoS32 secondIndex = 0; secondIndex < halfCapacity; ++secondIndex) {
		entry->m_active = 0;
		entry->m_unk0x08 = (LegoU32) secondDimension;
		entry->m_object = cursor;
		cursor += secondDimension;
		++entry;
	}
	for (LegoS32 thirdIndex = 0; thirdIndex < quarterCapacity; ++thirdIndex) {
		entry->m_active = 0;
		entry->m_unk0x08 = (LegoU32) (p_firstSpan + 8);
		entry->m_object = cursor;
		cursor += p_firstSpan + 8;
		++entry;
	}
}

// STUB: LEGORACERS2 0x004f3930
void ControllerRecordingObjectMap::Shutdown()
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	FreeHeapMemory(m_entries);
	m_entries = NULL;
	GetMemoryHeap()->Free(m_allocation);
	m_allocation = NULL;
	m_lock = 0;
}

// STUB: LEGORACERS2 0x004f3980
void* ControllerRecordingObjectMap::CreatePacket(const LegoU8* p_data, LegoU32 p_size)
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	LegoU8* packet = (LegoU8*) AllocatePacket(p_size + 8);
	if (packet == NULL) {
		m_lock = 0;
		return NULL;
	}

	LegoU16 id = (LegoU16) m_nextId + 1;
	if (id < 0x10) {
		id = 0x10;
	}
	m_nextId = (m_nextId & 0xffff0000) | id;
	*(LegoU32*) packet = p_size + 8;
	*(LegoU16*) (packet + 4) = 0;
	*(LegoU16*) (packet + 6) = id;
	memcpy(packet + 8, p_data, p_size);
	LegoU16 checksum = 0;
	for (LegoU32 index = 0; index < p_size + 8; ++index) {
		checksum = (LegoU16) (checksum + packet[index]);
	}
	*(LegoU16*) (packet + 4) = checksum;
	m_lock = 0;
	return packet;
}

// STUB: LEGORACERS2 0x004f3a30
void* ControllerRecordingObjectMap::AllocatePacket(LegoU32 p_bytes)
{
	ControllerRecordingObjectEntry* entries = m_entries;
	ControllerRecordingObjectEntry* end = entries + m_count;
	ControllerRecordingObjectEntry* candidate = entries;
	if (p_bytes > m_unk0x10) {
		candidate += m_unk0x1c;
	}
	if (p_bytes > m_unk0x14) {
		candidate += m_unk0x20;
	}
	while (candidate < end) {
		if (candidate->m_active != 1 && candidate->m_unk0x08 >= p_bytes) {
			candidate->m_active = 1;
			return candidate->m_object;
		}
		++candidate;
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004f3a90
void ControllerRecordingObjectMap::ClearMatchingId(LegoS16 p_id, LegoU32)
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	for (LegoS32 index = 0; index < m_count; ++index) {
		ControllerRecordingObjectEntry* entry = &m_entries[index];
		LegoS16 id = entry->m_active == 1 ? *(LegoS16*) ((LegoU8*) entry->m_object + 6) : 0;
		if (id == p_id) {
			entry->m_active = 0;
			break;
		}
	}
	m_lock = 0;
}

// FUNCTION: LEGORACERS2 0x004f3b00
void ControllerRecordingProviderEntry::CancelPending(IDirectPlay8Peer* p_peer)
{
	if (m_state == 1) {
		p_peer->CancelAsyncOperation(m_operationId, 0);
	}
}

// FUNCTION: LEGORACERS2 0x004f3b20
void ControllerRecordingProviderEntry::SetKey(LegoU16 p_key)
{
	m_key = p_key;
	DWORD now = timeGetTime();
	m_timestamp = now;
	m_state = 1;
}

// STUB: LEGORACERS2 0x004f3b40
void ControllerRecordingProviderMap::Initialize(LegoS32 p_capacity)
{
	m_count = p_capacity;
	m_usedCount = 0;
	m_expirationThreshold = ((p_capacity + (p_capacity >> 31 & 3)) >> 2) * 3;
	m_entries = (ControllerRecordingProviderEntry*) AllocateHeapMemory((LegoU32) p_capacity << 4);
	if (m_entries != NULL) {
		for (LegoS32 index = 0; index < p_capacity; ++index) {
			m_entries[index].m_key = 0;
			m_entries[index].m_operationId = 0;
			m_entries[index].m_state = 0;
		}
	}
}

// STUB: LEGORACERS2 0x004f3ba0
void ControllerRecordingProviderMap::Shutdown(IDirectPlay8Peer* p_peer)
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	for (LegoS32 index = 0; index < m_count; ++index) {
		ControllerRecordingProviderEntry* entry = &m_entries[index];
		if (entry->m_state != 0) {
			entry->CancelPending(p_peer);
			entry->m_key = 0;
			entry->m_operationId = 0;
			entry->m_state = 0;
		}
	}
	p_peer->CancelAsyncOperation(0, 4);
	FreeHeapMemory(m_entries);
	m_entries = NULL;
	m_usedCount = 0;
	m_count = 0;
	m_lock = 0;
}

// STUB: LEGORACERS2 0x004f3c20
ControllerRecordingProviderEntry* ControllerRecordingProviderMap::Add(LegoU16 p_key)
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	for (LegoS32 index = 0; index < m_count; ++index) {
		ControllerRecordingProviderEntry* entry = &m_entries[index];
		if (entry->m_state == 0) {
			++m_usedCount;
			entry->SetKey(p_key);
			m_lock = 0;
			return entry;
		}
	}
	m_lock = 0;
	return NULL;
}

// FUNCTION: LEGORACERS2 0x004f3c90
LegoBool32 ControllerRecordingProviderMap::SetState(LegoS16 p_key, LegoU32 p_state)
{
	LegoU32 state = p_state != 0 ? 3 : 2;
	while (m_lock == 1) {
	}
	m_lock = 1;
	for (LegoS32 index = 0; index < m_count; ++index) {
		ControllerRecordingProviderEntry* entry = &m_entries[index];
		if (entry->m_key == p_key) {
			entry->m_state = state;
			m_lock = 0;
			return TRUE;
		}
	}
	m_lock = 0;
	return FALSE;
}

// STUB: LEGORACERS2 0x004f3d00
LegoU32 ControllerRecordingProviderMap::FindValue(LegoS16 p_key)
{
	LegoU32 value = 0;
	while (m_lock == 1) {
	}
	m_lock = 1;
	for (LegoS32 index = 0; index < m_count; ++index) {
		if (m_entries[index].m_key == p_key) {
			value = m_entries[index].m_state;
			break;
		}
	}
	m_lock = 0;
	return value;
}

// STUB: LEGORACERS2 0x004f3d60
void ControllerRecordingProviderMap::Remove(LegoS16 p_key)
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	for (LegoS32 index = 0; index < m_count; ++index) {
		ControllerRecordingProviderEntry* entry = &m_entries[index];
		if (entry->m_key == p_key) {
			m_usedCount--;
			entry->m_key = 0;
			entry->m_operationId = 0;
			entry->m_state = 0;
			break;
		}
	}
	m_lock = 0;
}

// STUB: LEGORACERS2 0x004f3dd0
void ControllerRecordingProviderMap::PruneExpiredEntries()
{
	DWORD now = timeGetTime();
	while (m_lock == 1) {
	}
	m_lock = 1;
	if (m_usedCount > m_expirationThreshold) {
		for (;;) {
			LegoS32 oldestIndex = -1;
			DWORD greatestAge = 0;
			for (LegoS32 index = 0; index < m_count; ++index) {
				ControllerRecordingProviderEntry* entry = &m_entries[index];
				if (entry->m_state != 0) {
					DWORD age = now - entry->m_timestamp;
					if (age > greatestAge) {
						oldestIndex = index;
						greatestAge = age;
					}
				}
			}
			if (oldestIndex == -1) {
				break;
			}
			ControllerRecordingProviderEntry* entry = &m_entries[oldestIndex];
			--m_usedCount;
			entry->m_key = 0;
			entry->m_operationId = 0;
			entry->m_state = 0;
			if (m_usedCount <= m_expirationThreshold) {
				m_lock = 0;
				return;
			}
		}
	}
	m_lock = 0;
}

// STUB: LEGORACERS2 0x004f3e70
void ControllerRecordingMessageQueue::Initialize(LegoU32 p_capacity)
{
	LegoChar* allocationTag = "Network receive buffer";
	LegoU8* buffer = (LegoU8*) GetMemoryHeap()->Allocate(p_capacity, allocationTag);
	m_buffer = buffer;
	m_read = buffer;
	m_write = buffer;
	m_end = buffer + p_capacity;
	m_capacity = p_capacity;
	m_queuedCount = 0;
	m_pendingBytes = 0;
	m_accountedBytes = 0;
}

// STUB: LEGORACERS2 0x004f3eb0
void ControllerRecordingMessageQueue::Shutdown()
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	FreeHeapMemory(m_buffer);
	m_buffer = NULL;
	m_lock = 0;
}

// STUB: LEGORACERS2 0x004f3ef0
LegoBool32 ControllerRecordingMessageQueue::EnsureBytes(LegoU32 p_bytes, LegoU32* p_accountedBytes)
{
	LegoU32 newAccountedBytes = *p_accountedBytes + p_bytes;
	if ((LegoS32) newAccountedBytes > (LegoS32) m_capacity) {
		return FALSE;
	}
	*p_accountedBytes = newAccountedBytes;
	return TRUE;
}

// STUB: LEGORACERS2 0x004f3f20
LegoBool32 ControllerRecordingMessageQueue::Enqueue(ControllerRecordingMessageRecord* p_message)
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	if (m_buffer == NULL) {
		m_lock = 0;
		return FALSE;
	}

	LegoU32 accountedBytes = m_accountedBytes;
	LegoU32 payloadSize = p_message->m_payloadSize;
	LegoU32 extensionSize = 0;
	LegoU8* extension = NULL;
	LegoU8* payload = (LegoU8*) p_message->m_message;
	LegoU32 messageSize = payloadSize + sizeof(ControllerRecordingMessageRecord);
	if (p_message->m_messageId == 0xffff0011) {
		extension = *(LegoU8**) (payload + 0x0c);
		extensionSize = *(LegoU32*) (payload + 0x10);
		messageSize += extensionSize;
	}

	LegoU8* write = m_write;
	if (write + sizeof(LegoU32) > m_end) {
		if (EnsureBytes((LegoU32) (m_end - write), &accountedBytes) == FALSE) {
			m_lock = 0;
			return FALSE;
		}
		write = m_buffer;
	}
	if (EnsureBytes(sizeof(LegoU32), &accountedBytes) == FALSE) {
		m_lock = 0;
		return FALSE;
	}
	*(LegoU32*) write = messageSize;
	write += sizeof(LegoU32);
	if (write + messageSize > m_end) {
		if (EnsureBytes((LegoU32) (m_end - write), &accountedBytes) == FALSE) {
			m_lock = 0;
			return FALSE;
		}
		write = m_buffer;
	}

	LegoU8* queuedPayload = write + sizeof(ControllerRecordingMessageRecord);
	p_message->m_message = queuedPayload;
	memcpy(write, p_message, sizeof(ControllerRecordingMessageRecord));
	memcpy(queuedPayload, payload, payloadSize);
	if (extensionSize > 0) {
		memcpy(queuedPayload + payloadSize, extension, extensionSize);
		write = queuedPayload + payloadSize + extensionSize;
	}
	else {
		write = queuedPayload + payloadSize;
	}

	m_write = write;
	m_accountedBytes = accountedBytes;
	++m_queuedCount;
	m_lock = 0;
	return TRUE;
}

// STUB: LEGORACERS2 0x004f40d0
LegoBool32 ControllerRecordingMessageQueue::Dequeue(ControllerRecordingMessageRecord** p_message)
{
	while (m_lock == 1) {
	}
	m_lock = 1;
	m_accountedBytes -= m_pendingBytes;
	m_pendingBytes = 0;
	if (m_buffer == NULL || m_read == m_write) {
		m_lock = 0;
		return FALSE;
	}

	LegoU8* read = m_read;
	if (read + sizeof(LegoU32) > m_end) {
		m_pendingBytes = (LegoU32) (m_end - read);
		read = m_buffer;
	}
	LegoU32 messageSize = *(LegoU32*) read;
	read += sizeof(LegoU32);
	m_pendingBytes += sizeof(LegoU32);
	if (read + messageSize > m_end) {
		m_pendingBytes += (LegoU32) (m_end - read);
		read = m_buffer;
	}
	*p_message = (ControllerRecordingMessageRecord*) read;
	m_read = read + messageSize;
	m_pendingBytes += messageSize;
	--m_queuedCount;
	m_lock = 0;
	return TRUE;
}

// STUB: LEGORACERS2 0x004f4190
void ControllerRecordingService::RecordInput(InputController*, const LegoU32* p_input, LegoU32 p_frame)
{
	ControllerRecordingRecord* currentRecord = FindRecordForCurrentKey();
	if (currentRecord == NULL) {
		return;
	}

	LegoU32 modeValue = m_vtable->m_findOccupiedRecordValue(this, NULL);
	LegoU32 currentValue = m_vtable->m_getCurrentRecordValue(this, NULL);
	LegoU32 sample[4];
	sample[0] = (LegoU32) -100;
	sample[1] = currentValue;
	sample[2] = *p_input;
	sample[3] = p_frame;
	LegoU32 callbackOutput = 0;
	LegoU32 processedValue = 0;

	if (modeValue != currentValue) {
		for (LegoS32 differingIndex = 63; differingIndex >= 0; --differingIndex) {
			if (currentRecord->m_frameNumbers[differingIndex] == p_frame) {
				return;
			}
		}

		processedValue = m_vtable->m_processValue(this, NULL, sample, 0x10, 0x12, 0, &callbackOutput);
		if (processedValue == 3 || processedValue == 4) {
			return;
		}

		currentRecord = FindRecordForCurrentKey();
		currentRecord->m_frameNumbers[currentRecord->m_count] = p_frame;
		currentRecord->m_count++;
		if (currentRecord->m_count == 64) {
			currentRecord->m_count = 0;
		}
		return;
	}

	LegoBool32 frameFound = FALSE;
	for (LegoS32 matchingIndex = 63; matchingIndex >= 0; --matchingIndex) {
		if (currentRecord->m_frameNumbers[matchingIndex] == p_frame) {
			frameFound = TRUE;
			break;
		}
	}
	if (!frameFound) {
		processedValue = m_vtable->m_processValue(this, NULL, sample, 0x10, 0x4a, 0, &callbackOutput);
		if (processedValue != 3 && processedValue != 4) {
			currentRecord = FindRecordForCurrentKey();
			currentRecord->m_frameNumbers[currentRecord->m_count] = p_frame;
			currentRecord->m_count++;
			if (currentRecord->m_count == 64) {
				currentRecord->m_count = 0;
			}
		}
	}

	currentRecord = FindRecordForCurrentKey();
	LegoU32 currentMaximumFrame = 0;
	for (LegoS32 currentIndex = 63; currentIndex >= 0; --currentIndex) {
		LegoU32 frame = currentRecord->m_frameNumbers[currentIndex];
		if (frame != 0xffffffff && frame > currentMaximumFrame) {
			currentMaximumFrame = frame;
		}
	}

	for (LegoS32 recordIndex = 0; recordIndex < m_recordCount; ++recordIndex) {
		ControllerRecordingRecord* record = &m_records[recordIndex];
		if (record->m_occupied != 0) {
			continue;
		}

		LegoU32 maximumRecordedFrame = 0;
		for (LegoS32 recordFrameIndex = 63; recordFrameIndex >= 0; --recordFrameIndex) {
			LegoU32 frame = record->m_frameNumbers[recordFrameIndex];
			if (frame != 0xffffffff && frame > maximumRecordedFrame) {
				maximumRecordedFrame = frame;
			}
		}

		LegoU32 maximumInputFrame = 0;
		for (LegoS32 inputFrameIndex = 63; inputFrameIndex >= 0; --inputFrameIndex) {
			LegoU32 frame = record->m_inputValues[inputFrameIndex];
			if (frame != 0xffffffff && frame > maximumInputFrame) {
				maximumInputFrame = frame;
			}
		}

		LegoU32 tolerance = g_inputControllerManager->m_unk0x1c;
		if (tolerance < currentMaximumFrame && maximumInputFrame < currentMaximumFrame - tolerance) {
			m_vtable->m_requestInputFrame(this, NULL, (InputController*) record->m_ownerKey, maximumInputFrame + 1);
		}

		while (maximumRecordedFrame < maximumInputFrame) {
			maximumRecordedFrame++;
			LegoU32* input = ((InputController*) record->m_ownerKey)->GetRecordedInput(maximumRecordedFrame);
			sample[0] = (LegoU32) -100;
			sample[1] = record->m_value;
			sample[2] = *input;
			sample[3] = maximumRecordedFrame;
			m_vtable->m_processValue(this, NULL, sample, 0x10, 0x4a, 0, &callbackOutput);
		}
	}
}

// STUB: LEGORACERS2 0x004f4450
void ControllerRecordingService::UpdateInputRecord(InputController* p_controller, LegoU32 p_frame)
{
	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_ownerKey == (LegoU32) p_controller) {
			LegoU32 request = 0xfffffc18;
			m_vtable->m_processValue(this, NULL, &request, 8, 2, record->m_value, &p_frame);
			return;
		}
	}
}

// STUB: LEGORACERS2 0x004f44c0
void ControllerRecordingService::ReceiveRemoteInput(const LegoU32* p_command, LegoU32)
{
	LegoS32 recordIndex = 0;
	ControllerRecordingRecord* record = NULL;
	for (; recordIndex < m_recordCount; ++recordIndex) {
		ControllerRecordingRecord* candidate = &m_records[recordIndex];
		if (candidate->m_key == p_command[1]) {
			record = candidate;
			break;
		}
	}
	if (recordIndex >= m_recordCount) {
		return;
	}

	InputController* controller = (InputController*) record->m_ownerKey;
	controller->RecordInputRange(&p_command[2], p_command[3], p_command[3]);
	LegoU32 inputIndex = record->m_inputCount;
	record->m_inputValues[inputIndex] = p_command[3];
	++record->m_inputCount;
	if (record->m_inputCount == 0x40) {
		record->m_inputCount = 0;
	}

	if (m_vtable->m_findOccupiedRecordValue(this, NULL) != m_vtable->m_getCurrentRecordValue(this, NULL)) {
		return;
	}
	for (LegoS32 index = 0; index < 0x40; ++index) {
		if (record->m_frameNumbers[index] == p_command[3]) {
			return;
		}
	}

	LegoU32 packet[4];
	packet[0] = p_command[0];
	packet[1] = p_command[1];
	packet[2] = p_command[2];
	packet[3] = p_command[3];
	LegoU32 output = 0;
	m_vtable->m_processValue(this, NULL, packet, 0x10, 0x4a, 0, &output);
	LegoU32 frameIndex = record->m_count;
	record->m_frameNumbers[frameIndex] = p_command[3];
	++record->m_count;
	if (record->m_count == 0x40) {
		record->m_count = 0;
	}
}

// STUB: LEGORACERS2 0x004f4600
void ControllerRecordingService::SendRequestedInput(const LegoU32* p_command, LegoU32 p_recordValue)
{
	ControllerRecordingRecord* record = FindRecordForCurrentKey();
	LegoU32 maximumFrame = 0;
	for (LegoS32 index = 0x3f; index >= 0; --index) {
		LegoU32 frame = record->m_frameNumbers[index];
		if (frame > maximumFrame && frame != 0xffffffff) {
			maximumFrame = frame;
		}
	}
	LegoU32 requestedFrame = p_command[1];
	if (requestedFrame <= maximumFrame) {
		return;
	}
	LegoU32* input = ((InputController*) record->m_ownerKey)->GetRecordedInput(requestedFrame);
	LegoU32 packet[4];
	packet[0] = 0xffffff9c;
	packet[1] = m_vtable->m_getCurrentRecordValue(this, NULL);
	packet[2] = input[0];
	packet[3] = requestedFrame;
	LegoU32 output = 0;
	m_vtable->m_processValue(this, NULL, packet, 0x10, 2, p_recordValue, &output);
}

// STUB: LEGORACERS2 0x004f46a0
LegoBool32 ControllerRecordingService::HasInputFrame(InputController* p_controller, LegoU32 p_frame)
{
	if (m_recordCount <= 0) {
		return TRUE;
	}

	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_ownerKey == (LegoU32) p_controller) {
			for (LegoS32 frameIndex = 0x3f; frameIndex >= 0; --frameIndex) {
				if (record->m_frameNumbers[frameIndex] == p_frame) {
					return TRUE;
				}
			}
			return FALSE;
		}
	}

	return TRUE;
}

// STUB: LEGORACERS2 0x004f4710
void ControllerRecordingService::RequestInputFrame(InputController* p_controller, LegoU32 p_frame)
{
	m_requestedFrame = p_frame;
	m_requestedController = p_controller;
	m_currentRecord = NULL;

	for (LegoS32 index = 0; index < m_recordCount; ++index) {
		ControllerRecordingRecord* record = &m_records[index];
		if (record->m_ownerKey == (LegoU32) p_controller) {
			m_currentRecord = record;
			break;
		}
	}
	if (m_currentRecord == NULL) {
		return;
	}

	InputControllerManager* manager = g_inputControllerManager;
	manager->SetPaused(1);
	manager->GetTimingState((ControllerTimingState*) ((LegoU8*) manager + 0x9c));
	while (PollRequestedInput(0) == FALSE) {
		m_vtable->m_waitForInput(this, NULL);
	}
	manager->SetPaused(0);
}

// STUB: LEGORACERS2 0x004f47c0
LegoBool32 ControllerRecordingService::PollRequestedInput(LegoU32)
{
	ControllerRecordingRecord* record = (ControllerRecordingRecord*) m_currentRecord;
	if (record == NULL) {
		return TRUE;
	}

	for (LegoS32 index = 0x3f; index >= 0; --index) {
		if (record->m_frameNumbers[index] == m_requestedFrame) {
			m_currentRecord = NULL;
			return TRUE;
		}
	}

	LegoU32 seconds;
	LegoU32 microseconds;
	g_inputControllerManager->GetInputRequestElapsedTime(&seconds, &microseconds);
	if (seconds != 0 || microseconds > 90000) {
		m_vtable->m_fetchInput(this, NULL, m_requestedController, m_requestedFrame);
		g_inputControllerManager->GetTimingState((ControllerTimingState*) ((LegoU8*) g_inputControllerManager + 0x9c));
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x004f4870
void ControllerRecordingProviderAssociations::Initialize()
{
	for (LegoU32 index = 0; index < 64; ++index) {
		m_providerKeys[index] = 0;
		m_active[index] = 0;
	}
}

// STUB: LEGORACERS2 0x004f4890
LegoU32* ControllerRecordingProviderAssociations::AllocateAssociation()
{
	for (LegoU32 index = 0; index < 64; ++index) {
		if (m_active[index] == 0) {
			m_active[index] = 1;
			return &m_providerKeys[index];
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004f48c0
void ControllerRecordingProviderAssociations::PruneStaleAssociations()
{
	ControllerRecordingService* service = g_controllerRecordingService;
	for (LegoU32 index = 0; index < 64; ++index) {
		if (m_active[index] == 1 &&
			service->m_vtable->m_getProviderKeyValue(service, NULL, m_providerKeys[index]) > 1) {
			service->m_vtable->m_removeProviderKey(service, NULL, m_providerKeys[index]);
			m_active[index] = 0;
		}
	}
}
