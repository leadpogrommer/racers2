#ifndef LR2_GAMEOBJECTMODELBINDING_H
#define LR2_GAMEOBJECTMODELBINDING_H

#include "decomp.h"
#include "gamemanager.h"
#include "types.h"

#include <stddef.h>

struct GameObjectModelBinding;

struct GameObjectModelBindingDispatch {
	GameObjectModelBinding*(__fastcall* m_destroy)(GameObjectModelBinding*, void*, LegoU32);
	void(__fastcall* m_update)(GameObjectModelBinding*, void*);
};
DECOMP_SIZE_ASSERT(GameObjectModelBindingDispatch, 8);

struct GameObjectModelBinding {
	const GameObjectModelBindingDispatch* m_vtable; // +0x00
	GameObject* m_owner;                            // +0x04
	void* m_model;                                  // +0x08
	LegoChar m_path[128];                           // +0x0c
	LegoFloat m_rotation[4];                        // +0x8c
	LegoFloat m_position[3];                        // +0x9c
	LegoS32 m_boundMinimumX;                        // +0xa8
	LegoS32 m_boundMinimumY;                        // +0xac
	LegoU16 m_boundMinimumZ;                        // +0xb0
	LegoU8 m_layer;                                 // +0xb2
	LegoU8 m_alpha;                                 // +0xb3
	LegoU32 m_flags;                                // +0xb4
	LegoFloat m_unk0xB8;                            // +0xb8
	LegoFloat m_unk0xBC;                            // +0xbc
	LegoU32 m_unk0xC0;                              // +0xc0
	LegoFloat m_clipScale;                          // +0xc4
	LegoFloat m_clipNear;                           // +0xc8
	LegoFloat m_clipFar;                            // +0xcc
	void* m_bounds;                                 // +0xd0
	LegoU32 m_unk0xD4;                              // +0xd4
	LegoU32 m_unk0xD8;                              // +0xd8
	LegoU32 m_unk0xDC;                              // +0xdc
	LegoU32 m_unk0xE0;                              // +0xe0
};
DECOMP_SIZE_ASSERT(GameObjectModelBinding, 0xe4);
DECOMP_STATIC_ASSERT(offsetof(GameObjectModelBinding, m_path) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(GameObjectModelBinding, m_rotation) == 0x8c);
DECOMP_STATIC_ASSERT(offsetof(GameObjectModelBinding, m_position) == 0x9c);
DECOMP_STATIC_ASSERT(offsetof(GameObjectModelBinding, m_flags) == 0xb4);
DECOMP_STATIC_ASSERT(offsetof(GameObjectModelBinding, m_clipScale) == 0xc4);
DECOMP_STATIC_ASSERT(offsetof(GameObjectModelBinding, m_bounds) == 0xd0);

extern const GameObjectModelBindingDispatch g_gameObjectModelBindingDispatch;
void __fastcall ResetGameObjectModelBindingDispatch(GameObjectModelBinding*, void*);
void __fastcall InitializeGameObjectModelBinding(GameObjectModelBinding*, void*);
void __fastcall ReleaseGameObjectModelBindingResources(GameObjectModelBinding*, void*);
void __fastcall DestroyGameObjectModelBindingForOwner(GameObject*, void*);
void __fastcall UpdateGameObjectModelBinding(GameObjectModelBinding*, void*);
GameObjectModelBinding* __fastcall DestroyGameObjectModelBinding(GameObjectModelBinding*, void*, LegoU32 p_flags);
void __fastcall ConstructGameObjectModelBinding(
	GameObject* p_owner,
	void* p_unusedEdx,
	const LegoU8* p_path,
	const LegoFloat* p_position,
	const LegoFloat* p_rotation,
	LegoU32 p_flags
);

#endif // LR2_GAMEOBJECTMODELBINDING_H
