#ifndef LR2_CONTROLLERRECORDINGSERVICE_H
#define LR2_CONTROLLERRECORDINGSERVICE_H

#include "inputcontroller.h"
#include "types.h"

#include <stddef.h>

struct ControllerRecordingService;
class InputControllerManager;

struct ControllerRecordingServiceSettings {
	LegoU32 m_recordCapacity;
	LegoU32 m_sessionCapacity;
	LegoU32 m_deviceCapacity;
	LegoU32 m_serviceProviderCapacity;
	LegoU32 m_objectMapCapacity;
	LegoU32 m_providerMapCapacity;
	LegoU32 m_objectMapFirstSpan;
	LegoU32 m_objectMapSecondSpan;
	LegoU32 m_unk0x20;
	LegoU32 m_receiveBufferCapacity;
};
DECOMP_SIZE_ASSERT(ControllerRecordingServiceSettings, 0x28);

// Typed view of the complete 34-slot retail dispatch table.
struct ControllerRecordingServiceDispatchView {
	LegoBool32(__fastcall* m_initialize)(
		ControllerRecordingService*,
		void*,
		const ControllerRecordingServiceSettings*,
		void*
	);
	void(__fastcall* m_setUnknownWord0x2c)(ControllerRecordingService*, void*, LegoU32);
	void(__fastcall* m_shutdown)(ControllerRecordingService*, void*);
	LegoBool32(__fastcall* m_enumerateDevicesForProvider)(ControllerRecordingService*, void*, LegoS32);
	LegoBool32(__fastcall* m_copyDeviceNames)(ControllerRecordingService*, void*, LegoChar*, LegoS32*);
	LegoBool32(__fastcall* m_copySessionNames)(ControllerRecordingService*, void*, LegoChar*, LegoS32*);
	LegoBool32(__fastcall* m_selectServiceProvider)(
		ControllerRecordingService*,
		void*,
		LegoS32,
		const LegoChar**,
		LegoU32
	);
	void(__fastcall* m_enumerateHosts)(ControllerRecordingService*, void*);
	void(__fastcall* m_cancelPeerOperation)(ControllerRecordingService*, void*); // +0x20
	LegoBool32(__fastcall* m_copySessionRecords)(ControllerRecordingService*, void*, LegoU8*, LegoS32*);
	LegoBool32(__fastcall* m_hostSession)(ControllerRecordingService*, void*, const LegoChar*);
	LegoBool32(__fastcall* m_connectToSession)(ControllerRecordingService*, void*, LegoS32);
	LegoBool32(__fastcall* m_resetNegotiatedState)(ControllerRecordingService*, void*); // +0x30
	LegoBool32(__fastcall* m_initializePeer)(ControllerRecordingService*, void*);
	LegoBool32(__fastcall* m_restartPeerSession)(ControllerRecordingService*, void*);
	void(__fastcall* m_setUnknownWord0xd8)(ControllerRecordingService*, void*, LegoU32);
	LegoU32(__fastcall* m_getUnknownWord0xd8)(ControllerRecordingService*, void*);
	LegoU32(__fastcall* m_getUnknownWord0x20)(ControllerRecordingService*, void*);
	LegoU32(__fastcall* m_processValue)(
		ControllerRecordingService*,
		void*,
		const LegoU32*,
		LegoU32,
		LegoU8,
		LegoU32,
		LegoU32*
	);                                                                                                    // +0x48
	LegoU32(__fastcall* m_getProviderKeyValue)(ControllerRecordingService*, void*, LegoU32);              // +0x4c
	void(__fastcall* m_removeProviderKey)(ControllerRecordingService*, void*, LegoU32);                   // +0x50
	LegoBool32(__fastcall* m_waitForInput)(ControllerRecordingService*, void*);                           // +0x54
	void(__fastcall* m_setCurrentRecordName)(ControllerRecordingService*, void*, LegoChar*);              // +0x58
	void(__fastcall* m_copyInputStatistics)(ControllerRecordingService*, void*, LegoU32*);                // +0x5c
	LegoU32(__fastcall* m_getCurrentRecordValue)(ControllerRecordingService*, void*);                     // +0x60
	LegoU32(__fastcall* m_findOccupiedRecordValue)(ControllerRecordingService*, void*);                   // +0x64
	void(__fastcall* m_copyRecordValues)(ControllerRecordingService*, void*, LegoU32*, LegoU32*);         // +0x68
	void(__fastcall* m_assignRecordOwner)(ControllerRecordingService*, void*, LegoU32, InputController*); // +0x6c
	void(__fastcall*
			 m_recordInput)(ControllerRecordingService*, void*, InputController*, const LegoU32*, LegoU32); // +0x70
	void(__fastcall* m_fetchInput)(ControllerRecordingService*, void*, InputController*, LegoU32);          // +0x74
	LegoBool32(__fastcall* m_hasInputFrame)(ControllerRecordingService*, void*, InputController*, LegoU32); // +0x78
	void(__fastcall* m_requestInputFrame)(ControllerRecordingService*, void*, InputController*, LegoU32);   // +0x7c
	LegoBool32(__fastcall* m_isAvailable)(ControllerRecordingService*, void*);                              // +0x80
	void(__fastcall* m_clearOrSetMode)(ControllerRecordingService*, void*, LegoU32);                        // +0x84
};
DECOMP_SIZE_ASSERT(ControllerRecordingServiceDispatchView, 0x88);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_resetNegotiatedState) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_recordInput) == 0x70);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_setCurrentRecordName) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_copyInputStatistics) == 0x5c);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_copyRecordValues) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_assignRecordOwner) == 0x6c);

DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingServiceDispatchView, m_hasInputFrame) == 0x78);

struct ControllerRecordingRecord {
	LegoU32 m_key;         // +0x00
	LegoU32 m_value;       // +0x04
	LegoChar m_name[0x20]; // +0x08
	LegoU32 m_occupied;    // +0x28
	undefined m_unk0x2c[0x18];
	LegoU32 m_ownerKey;         // +0x44
	LegoU32 m_inputValues[64];  // +0x48
	LegoU32 m_frameNumbers[64]; // +0x148
	LegoU32 m_count;            // +0x248
	LegoU32 m_inputCount;       // +0x24c
};
DECOMP_SIZE_ASSERT(ControllerRecordingRecord, 0x250);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingRecord, m_ownerKey) == 0x44);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingRecord, m_inputValues) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingRecord, m_frameNumbers) == 0x148);

struct ControllerRecordingSessionRecord {
	undefined m_unk0x00[0x48];
	void* m_peerAddress;
	void* m_lobbyAddress;
	undefined m_unk0x50[0x80];
};
DECOMP_SIZE_ASSERT(ControllerRecordingSessionRecord, 0xd0);

struct ControllerRecordingProviderAssociations {
	LegoU32 m_providerKeys[64];
	LegoU8 m_active[64];
	void Initialize();
	LegoU32* AllocateAssociation();
	void PruneStaleAssociations();
};
DECOMP_SIZE_ASSERT(ControllerRecordingProviderAssociations, 0x140);

struct ControllerRecordingService {
	LegoBool32 Initialize(const ControllerRecordingServiceSettings* p_settings, void* p_callbackContext);
	void Shutdown();
	LegoBool32 EnumerateServiceProviders();
	void EnumerateHosts();
	LegoBool32 SelectServiceProvider(LegoS32 p_providerIndex, const LegoChar** p_optionValues, LegoU32 p_optionCount);
	LegoBool32 HostSession(const LegoChar* p_sessionName);
	LegoBool32 ConnectToSession(LegoS32 p_sessionIndex);
	void CancelPeerOperation();
	LegoBool32 ResetNegotiatedState();
	LegoBool32 RestartPeerSession();
	LegoBool32 InitializePeer();
	LegoBool32 WaitForInput();
	LegoBool32 EnumerateDevicesForProvider(LegoS32 p_providerIndex);
	LegoBool32 CopyDeviceNames(LegoChar* p_output, LegoS32* p_count);
	LegoBool32 CopySessionNames(LegoChar* p_output, LegoS32* p_count);
	LegoBool32 CopySessionRecords(LegoU8* p_output, LegoS32* p_count);
	void SetUnknownWord0x2c(LegoU32 p_value);
	void SetUnknownWord0xd8(LegoU32 p_value);
	LegoU32 GetUnknownWord0xd8();
	LegoU32 GetUnknownWord0x20();
	ControllerRecordingRecord* FindRecordForCurrentKey();
	LegoU32 GetOccupiedRecordKey();
	LegoU32 FindRecordKeyByValue(LegoU32 p_value);
	LegoU32 GetCurrentRecordValue();
	LegoU32 GetCurrentKey();
	void CopyInputStatistics(LegoU32* p_output);
	LegoU32 FindOccupiedRecordValue();
	ControllerRecordingRecord* AllocateRecord();
	ControllerRecordingRecord* FindRecordByKey(LegoU32 p_key);
	void RemoveRecordByKey(LegoU32 p_key);
	ControllerRecordingSessionRecord* AllocateSessionRecord();
	void RemoveSessionRecord(ControllerRecordingSessionRecord* p_record);
	void SetCurrentRecordName(LegoChar* p_name);
	void SetRecordValue(LegoU32 p_key, LegoU32 p_value);
	void CopyRecordValues(LegoU32* p_values, LegoU32* p_count);
	void AssignRecordOwner(LegoU32 p_value, InputController* p_controller);
	LegoU32 GetProviderKeyValue(LegoU32 p_key);
	void RemoveProviderKey(LegoU32 p_key);
	LegoU32 ProcessValue(
		const LegoU8* p_payload,
		LegoU32 p_payloadBytes,
		LegoU8 p_flags,
		LegoU32 p_recordValue,
		LegoU32* p_payloadKey
	);
	void RecordInput(InputController* p_controller, const LegoU32* p_input, LegoU32 p_frame);
	void ProcessSpecialInputCommand(const LegoU32* p_command, LegoU32 p_payloadBytes, LegoU32 p_recordValue);
	void ReceiveRemoteInput(const LegoU32* p_command, LegoU32 p_recordValue);
	void SendRequestedInput(const LegoU32* p_command, LegoU32 p_recordValue);
	LegoU32 DispatchDirectPlayMessage(LegoU32 p_userContext, LegoU32 p_messageId, void* p_message, LegoU32 p_flags);
	LegoU32 HandleDirectPlayMessage(void* p_messageRecord);
	void UpdateInputRecord(InputController* p_controller, LegoU32 p_frame);
	LegoU32 GetRequestedInputValue();
	void SetRequestedInputValue(LegoU32 p_value);
	LegoBool32 HasInputFrame(InputController* p_controller, LegoU32 p_frame);
	void RequestInputFrame(InputController* p_controller, LegoU32 p_frame);
	LegoBool32 PollRequestedInput(LegoU32 p_unused);
	void RecordPeerState(LegoU32 p_playerId, LegoU32 p_recordIndex);
	const ControllerRecordingServiceDispatchView* m_vtable; // +0x00
	void* m_primaryProvider;                                // +0x04
	void* m_secondaryProvider;                              // +0x08
	ControllerRecordingRecord* m_records;                   // +0x0c
	LegoU32 m_recordCapacity;                               // +0x10
	LegoS32 m_recordCount;                                  // +0x14
	LegoU32 m_unk0x18;
	LegoU32 m_unk0x1c;
	LegoU32 m_unk0x20;
	LegoU32 m_currentKey;           // +0x24
	void* m_receivedInputCallback;  // +0x28
	void* m_recordRemovedCallback;  // +0x2c
	void* m_serviceProviderRecords; // +0x30
	LegoU32 m_serviceProviderCapacity;
	LegoS32 m_serviceProviderCount;
	LegoS32 m_selectedServiceProvider; // +0x3c, -1 before selection
	void* m_serviceProviderAddress;    // +0x40
	void* m_deviceAddress;             // +0x44
	void* m_pendingPeerOperation;      // +0x48
	undefined m_unk0x4c[4];
	void* m_sessionRecords;    // +0x50
	LegoU32 m_sessionCapacity; // +0x54
	LegoS32 m_sessionCount;    // +0x58
	LegoU32 m_unk0x5c;
	void* m_deviceRecords;    // +0x60
	LegoU32 m_deviceCapacity; // +0x64
	LegoS32 m_deviceCount;    // +0x68
	LegoU32 m_unk0x6c;
	undefined m_unk0x70[0x68];
	LegoU32 m_unk0xd8;
	LegoU32 m_requestedInputValue;          // +0xdc
	LegoU32 m_requestedFrame;               // +0xe0
	InputController* m_requestedController; // +0xe4
	void* m_currentRecord;                  // +0xe8
	LegoU32 m_unk0xec;
	ControllerRecordingProviderAssociations m_providerAssociations; // +0xf0
};
DECOMP_SIZE_ASSERT(ControllerRecordingService, 0x230);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingService, m_records) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingService, m_recordCount) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingService, m_requestedInputValue) == 0xdc);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingService, m_currentRecord) == 0xe8);
DECOMP_STATIC_ASSERT(offsetof(ControllerRecordingService, m_providerAssociations) == 0xf0);

extern ControllerRecordingService* g_controllerRecordingService;
extern const ControllerRecordingServiceDispatchView g_controllerRecordingServiceDispatch;
void InitializeControllerRecordingServiceStorage();
LegoU32 __stdcall GetDirectPlayMessagePayloadSize(LegoU32 p_messageId, LegoU32 p_flags);

#endif // LR2_CONTROLLERRECORDINGSERVICE_H
