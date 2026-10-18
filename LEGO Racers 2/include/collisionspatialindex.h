#ifndef LR2_COLLISIONSPATIALINDEX_H
#define LR2_COLLISIONSPATIALINDEX_H

#include "collisionproxy.h"

struct SpatialSlotNode {
	void Initialize();
	void InsertBefore(void* p_value, SpatialSlotNode* p_next);
	void InsertAfter(void* p_value, SpatialSlotNode* p_previous);
	void Unlink();
	void* m_value;
	SpatialSlotNode* m_previous;
	SpatialSlotNode* m_next;
};
DECOMP_SIZE_ASSERT(SpatialSlotNode, 0x0c);

class SpatialSlotAllocator {
public:
	SpatialSlotAllocator(LegoS32 p_capacity);
	void Reset();
	SpatialSlotNode* AllocateBefore(void* p_value, SpatialSlotNode* p_next);
	SpatialSlotNode* AllocateAfter(void* p_value, SpatialSlotNode* p_previous);
	void RemoveValue(void* p_value, SpatialSlotNode** p_head);
	LegoBool32 ReleaseNode(SpatialSlotNode** p_node);
	LegoS32 GetNodeIndex(SpatialSlotNode* p_node);
	void* GetNodeValue(LegoS32 p_index);
	SpatialSlotNode* m_nodes;
	SpatialSlotNode* m_free;
	LegoS32 m_count;
	LegoS32 m_capacity;
};

class SpatialLinkRegistry;
typedef LegoBool32(__cdecl* SpatialValueCompare)(void*, void*);
struct SpatialListDispatch {
	typedef SpatialSlotNode*(__fastcall* Insert)(SpatialLinkRegistry*, void*, void*);
	typedef void(__fastcall* Update)(SpatialLinkRegistry*, void*, SpatialSlotNode*);
	Insert m_insert;
	Update m_update;
};

class SpatialLinkRegistry {
public:
	SpatialLinkRegistry(SpatialSlotAllocator* p_allocator); // 0x004f5140
	LegoS32 Insert(SpatialIndexLink* p_link);               // 0x004f5160
	LegoS32 Remove(SpatialIndexLink* p_link);               // 0x004f5180
	SpatialLinkRegistry* InitializeList(SpatialSlotAllocator* p_allocator, SpatialValueCompare p_compare);
	void ClearHead();
	SpatialSlotNode* InsertValue(void* p_value);
	void UpdateNode(SpatialSlotNode* p_node);
	void RemoveValue(void* p_value);
	void RemoveNode(SpatialSlotNode* p_node);
	LegoS32 GetNodeIndex(SpatialSlotNode* p_node);
	void* GetNodeValue(LegoS32 p_index);
	const SpatialListDispatch* m_vtable;
	SpatialSlotNode* m_head;
	SpatialSlotAllocator* m_allocator;
	SpatialValueCompare m_compare;
};

class DynamicSpatialIndex;
struct CollisionQueryBounds;
struct CollisionQueryResult;

struct SpatialGridCell {
	void Initialize();
	void Insert(void* p_value, SpatialSlotAllocator* p_allocator);
	void Remove(void* p_value, SpatialSlotAllocator* p_allocator);
	SpatialSlotNode* m_head;
};
DECOMP_SIZE_ASSERT(SpatialGridCell, 4);

class StaticSpatialGrid {
public:
	StaticSpatialGrid(
		LegoU32 p_size,
		const Vector3* p_origin,
		LegoFloat p_scale,
		LegoU32 p_capacity,
		SpatialSlotAllocator* p_allocator
	);                                                      // 0x00508690
	void Insert(SpatialIndexLink* p_link);                  // 0x00508740
	void Remove(SpatialIndexLink* p_link);                  // 0x00508840
	void Update(SpatialIndexLink* p_link);                  // 0x00508890
	void PrepareQuery(DynamicSpatialIndex* p_dynamicIndex); // 0x005088b0
	LegoBool32 IntersectSegment(const CollisionQueryBounds*, const Vector3*, const Vector3*, CollisionQueryResult*);
	SpatialGridCell* GetCell(LegoS32 p_x, LegoS32 p_z); // 0x00508500
	Vector3 m_origin;
	LegoFloat m_diameter;
	LegoFloat m_inverseCellSize;
	LegoS32 m_size;
	LegoU32 m_shift;
	SpatialSlotAllocator* m_allocator;
	SpatialGridCell* m_cells;
};

struct SpatialAxisIndex;
struct SpatialAxisDispatch {
	typedef SpatialSlotNode*(__fastcall* Insert)(SpatialAxisIndex*, void*, void*);
	typedef void(__fastcall* Update)(SpatialAxisIndex*, void*, void*);
	Insert m_insert;
	Update m_update; // 0x04
};

struct SpatialAxisIndex {
	SpatialAxisIndex* InitializeFirst(SpatialLinkRegistry* p_registry, SpatialSlotAllocator* p_allocator);
	SpatialAxisIndex* InitializeSecond(SpatialLinkRegistry* p_registry, SpatialSlotAllocator* p_allocator);
	void InsertFirst(SpatialIndexLink* p_link);  // 0x00508330
	void InsertSecond(SpatialIndexLink* p_link); // 0x005083e0
	void Remove(void* p_handle);                 // 0x00507ca0
	void UpdateFirst(SpatialSlotNode* p_node);   // 0x00508010
	void UpdateSecond(SpatialSlotNode* p_node);  // 0x00508100
	const SpatialAxisDispatch* m_vtable;
	SpatialSlotNode* m_head;           // 0x04
	SpatialSlotAllocator* m_allocator; // 0x08
	SpatialValueCompare m_compare;     // 0x0c
	SpatialLinkRegistry* m_registry;   // 0x10
};

struct SpatialPairNode {
	LegoU32 m_value;
	LegoU32 m_next;
};

struct SpatialPairList {
	SpatialPairList* Initialize(LegoU32 p_capacity, SpatialLinkRegistry* p_registry);
	LegoBool32 AddPair(LegoS32 p_first, LegoS32 p_second);
	LegoBool32 CanPair(LegoS32 p_first, LegoS32 p_second);
	void ProcessCommonPairs(const SpatialPairList* p_other);
	LegoU32 m_unk0x00;
	LegoU32 m_headIndex;
	SpatialPairNode* m_nodes;
	SpatialPairNode* m_tail;
	LegoU32 m_count;
	LegoU32 m_capacity;
	LegoU32 m_unk0x18;
	SpatialLinkRegistry* m_registry;
};
DECOMP_SIZE_ASSERT(SpatialPairList, 0x20);

class DynamicSpatialIndex {
public:
	DynamicSpatialIndex(LegoU32 p_capacity, SpatialSlotAllocator* p_allocator, SpatialLinkRegistry* p_registry);
	void ResetQuery(); // 0x00507d60
	LegoU32 CollectCandidates(const CollisionQueryBounds* p_bounds, LegoU32* p_tags);
	LegoBool32 IntersectSegment(const CollisionQueryBounds*, const Vector3*, const Vector3*, CollisionQueryResult*);
	SpatialAxisIndex m_axes[2];
	SpatialPairList m_xPairs;          // 0x28
	SpatialPairList m_zPairs;          // 0x48
	SpatialSlotAllocator* m_allocator; // 0x68
	SpatialLinkRegistry* m_registry;   // 0x6c
};

DECOMP_SIZE_ASSERT(SpatialSlotAllocator, 0x10);
DECOMP_SIZE_ASSERT(SpatialLinkRegistry, 0x10);
DECOMP_SIZE_ASSERT(StaticSpatialGrid, 0x24);
DECOMP_SIZE_ASSERT(SpatialAxisIndex, 0x14);
DECOMP_SIZE_ASSERT(DynamicSpatialIndex, 0x70);

class CollisionSpatialIndex {
public:
	CollisionSpatialIndex(
		LegoU32 p_size,
		const Vector3* p_origin,
		LegoFloat p_scale,
		LegoU32 p_capacity,
		LegoU32 p_staticCapacity,
		LegoU32 p_unused
	);
	LegoS32 InsertStatic(SpatialIndexLink* p_link);
	LegoS32 InsertDynamic(SpatialIndexLink* p_link);
	LegoS32 RemoveStatic(SpatialIndexLink* p_link);
	LegoS32 RemoveDynamic(SpatialIndexLink* p_link);
	void UpdateStatic(SpatialIndexLink* p_link);
	void UpdateDynamic(SpatialIndexLink* p_link);

	StaticSpatialGrid* m_staticGrid;     // 0x00
	DynamicSpatialIndex* m_dynamicIndex; // 0x04
	SpatialLinkRegistry* m_registry;     // 0x08
	SpatialSlotAllocator* m_allocator;   // 0x0c
};
DECOMP_SIZE_ASSERT(CollisionSpatialIndex, 0x10);

extern CollisionSpatialIndex* g_collisionSpatialIndex;

#endif // LR2_COLLISIONSPATIALINDEX_H
