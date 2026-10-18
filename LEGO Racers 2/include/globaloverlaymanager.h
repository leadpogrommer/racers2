#ifndef LR2_GLOBALOVERLAYMANAGER_H
#define LR2_GLOBALOVERLAYMANAGER_H

#include "decomp.h"
#include "gamemanager.h"
#include "rendererrecords.h"
#include "types.h"

#include <stddef.h>

struct LocalizedStringTable;

struct GlobalOverlayRecord {
	const void* m_vtable;                  // 0x00
	LegoU16 m_text[0x100];                 // 0x04
	LegoU32 m_flags;                       // 0x204
	void* m_font;                          // 0x208
	LegoU32 m_color;                       // 0x20c
	LegoFloat m_positionX;                 // 0x210
	LegoFloat m_positionY;                 // 0x214
	LegoFloat m_clipLeft;                  // 0x218
	LegoFloat m_clipTop;                   // 0x21c
	LegoFloat m_clipRight;                 // 0x220
	LegoFloat m_clipBottom;                // 0x224
	LegoU32 m_animationFlags;              // 0x228
	LegoS32 m_listId;                      // 0x22c
	LegoFloat m_scale;                     // 0x230
	LegoFloat m_elapsed;                   // 0x234
	LegoU32 m_type;                        // 0x238
	LegoU8 m_characterAnimationIndices[8]; // 0x23c
	LegoS32 m_textLength;                  // 0x244
	LegoFloat m_animationTime;             // 0x248
	LegoS32 m_characterIndex;              // 0x24c
	LegoFloat m_measuredWidth;             // 0x250
	void InitializeFromParameters(const LegoU32* p_parameters, LegoS32 p_stringIndex);
	void InitializeText(const LegoU16* p_text, LegoU32 p_flags, LegoFloat p_time);
	void InitializeTextByIndex(LegoS32 p_stringIndex, LegoU32 p_flags, LegoFloat p_time);
	void SetPosition(const LegoFloat* p_position, LegoU32 p_mode);
	void SetScale(LegoFloat p_scale);
	void Release();
};

struct GlobalOverlayListNode {
	void* m_value;
	GlobalOverlayListNode* m_next;
};

struct GlobalOverlayManager : GameObject {
	undefined m_overlayEntries[300][0x58];     // 0x58, opaque 0x58-byte records
	GlobalOverlayListNode m_overlayNodes[300]; // 0x6778, compact links for the 0x58-stride pool
	GlobalOverlayListNode* m_overlayNodePool;  // 0x70d8
	GlobalOverlayListNode* m_overlayHeads[16]; // 0x70dc
	GlobalOverlayRecord m_records[200];        // 0x711c, stride 0x254
	GlobalOverlayListNode m_recordNodes[200];  // 0x242bc
	GlobalOverlayListNode* m_recordNodeRoot;   // 0x248fc
	GlobalOverlayListNode* m_recordHeads[16];  // 0x24900
	LegoS32 m_recordListCount;                 // 0x24940
	LegoU32 m_unk0x24944;
	LocalizedStringTable* m_localizedStrings;
	LegoU32 m_unk0x2494c;

	GlobalOverlayManager(GameObjectTemplate* p_template);
	void UpdateRecords(void* p_argument);
	void SetLocalizedStrings(LocalizedStringTable* p_strings);
	GlobalOverlayRecord* AllocateRecord(const LegoU32* p_parameters, LegoS32 p_listId);
	void MoveRecord(GlobalOverlayRecord* p_record, LegoS32 p_sourceList, LegoS32 p_destinationList);
};

DECOMP_SIZE_ASSERT(GlobalOverlayRecord, 0x254);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayRecord, m_flags) == 0x204);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayRecord, m_color) == 0x20c);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayRecord, m_animationFlags) == 0x228);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayRecord, m_listId) == 0x22c);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayRecord, m_textLength) == 0x244);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayRecord, m_measuredWidth) == 0x250);
DECOMP_SIZE_ASSERT(GlobalOverlayListNode, 0x08);
DECOMP_SIZE_ASSERT(GlobalOverlayManager, 0x24950);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_overlayEntries) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_overlayHeads) == 0x70dc);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_overlayNodes) == 0x6778);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_overlayNodePool) == 0x70d8);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_records) == 0x711c);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_recordNodes) == 0x242bc);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_recordNodeRoot) == 0x248fc);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_recordHeads) == 0x24900);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_recordListCount) == 0x24940);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_unk0x24944) == 0x24944);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayManager, m_unk0x2494c) == 0x2494c);

extern GlobalOverlayManager& g_globalOverlayManager;
void InitializeGlobalOverlayManagerAndRegisterExit();
void InitializeGlobalOverlayManager();
void RegisterGlobalOverlayManagerExit();

struct GlobalOverlayRecordDispatch {
	LegoFloat(__fastcall* m_measureTextWidth)(GlobalOverlayRecord*, void*);
	void(__fastcall* m_advanceTextAnimation)(GlobalOverlayRecord*, void*, LegoFloat);
	void(__fastcall* m_draw)(GlobalOverlayRecord*, void*, RendererDefaultRecord*);
	GameObjectDispatchView m_managerDispatch;
};

extern const GlobalOverlayRecordDispatch g_globalOverlayRecordDispatch;

GameObject* __fastcall GlobalOverlayManagerDestroy(GameObject*, void*, LegoU32 p_flags);
LegoU32 __fastcall GlobalOverlayManagerUpdateRecords(GameObject*, void*, LegoU32 p_argument);
LegoFloat __fastcall MeasureGlobalOverlayText(GlobalOverlayRecord*, void*);
void __fastcall AdvanceGlobalOverlayText(GlobalOverlayRecord*, void*, LegoFloat p_delta);
void __fastcall DrawGlobalOverlayText(GlobalOverlayRecord*, void*, RendererDefaultRecord*);

void __fastcall RenderGlobalOverlayPass(GlobalOverlayManager* p_manager, void*, LegoU32 p_mode);

#endif // LR2_GLOBALOVERLAYMANAGER_H
