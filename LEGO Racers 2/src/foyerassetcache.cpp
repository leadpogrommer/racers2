#include "foyerassetcache.h"

#include "bodypartmanager.h"
#include "charactervisual.h"
#include "foyertemplateinstance.h"
#include "gamecontroller.h"
#include "memoryheap.h"

#include <new>
#include <stdlib.h>
#include <string.h>

// GLOBAL: LEGORACERS2 0x00572748
FoyerAssetCache g_foyerAssetCache;

// GLOBAL: LEGORACERS2 0x00572778
LegoU8 g_foyerAssetCacheInitialized;

void* __fastcall LoadSparkyFoyerAsset(FoyerAssetCache*, void*);
void* __fastcall LoadDefaultSizeFoyerAsset(FoyerAssetCache*, void*);
void* __fastcall LoadRigelFoyerAsset(FoyerAssetCache*, void*);
void* __fastcall LoadRocketRacerFoyerAsset(FoyerAssetCache*, void*);
void* __fastcall LoadPlayerFoyerAsset(FoyerAssetCache*, void*);
void* __fastcall LoadIntroFoyerAsset(FoyerAssetCache*, void*);

void __cdecl FoyerAssetCacheExitCallback();
void __fastcall ResetFoyerAssetCache(FoyerAssetCache*, void*);

struct FoyerCacheModelTemplate : GameObjectTemplate {
	LegoU32 m_properties[14];
	CharBuffer128 m_modelPath;
	LegoU32 m_modelFlags;
	CharBuffer128 m_animationPath;
	CharBuffer128 m_skinPath;
};
DECOMP_SIZE_ASSERT(FoyerCacheModelTemplate, 0x1f0);
DECOMP_STATIC_ASSERT(offsetof(FoyerCacheModelTemplate, m_modelPath) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(FoyerCacheModelTemplate, m_animationPath) == 0xf0);
DECOMP_STATIC_ASSERT(offsetof(FoyerCacheModelTemplate, m_skinPath) == 0x170);

// GLOBAL: LEGORACERS2 0x0056e2a0
static FoyerCacheModelTemplate g_defaultSizeFoyerTemplate;
// GLOBAL: LEGORACERS2 0x00572850
static FoyerCacheModelTemplate g_playerFoyerTemplate;

static void InitializeFoyerCacheModelTemplate(
	FoyerCacheModelTemplate* p_template,
	LegoU32 p_flags,
	const LegoChar* p_name,
	LegoFloat p_property7,
	LegoFloat p_property8,
	LegoFloat p_property10,
	LegoFloat p_scale,
	const LegoChar* p_model,
	const LegoChar* p_animation,
	const LegoChar* p_skin
)
{
	p_template->m_flags = p_flags;
	p_template->m_first = CharBuffer24(p_name);
	p_template->m_second = CharBuffer24("");
	memset(p_template->m_properties, 0, sizeof(p_template->m_properties));
	LegoFloat* values = (LegoFloat*) p_template->m_properties;
	values[6] = 1.0f;
	values[7] = p_property7;
	values[8] = p_property8;
	values[10] = p_property10;
	values[11] = values[12] = values[13] = p_scale;
	p_template->m_modelPath = CharBuffer128(p_model);
	p_template->m_modelFlags = 1;
	p_template->m_animationPath = CharBuffer128(p_animation);
	p_template->m_skinPath = CharBuffer128(p_skin);
}

static void InitializeDefaultSizeFoyerTemplate();
static void InitializePlayerFoyerTemplate();

static GameObjectTemplate* GetDefaultSizeFoyerTemplate()
{
	static LegoBool32 initialized;
	if (!initialized) {
		InitializeDefaultSizeFoyerTemplate();
		initialized = TRUE;
	}
	return &g_defaultSizeFoyerTemplate;
}

static GameObjectTemplate* GetPlayerFoyerTemplate()
{
	static LegoBool32 initialized;
	if (!initialized) {
		InitializePlayerFoyerTemplate();
		initialized = TRUE;
	}
	return &g_playerFoyerTemplate;
}

// Character previews are cached as the visual object followed by the body and
// head objects created by its initializer. The cache retains the visual object;
// callers receive its body object, matching the retail API.
static GameObject* CreateCharacterVisual(CharacterVisualTemplate* p_template)
{
	void* storage = AllocateHeapMemory(0x198);
	if (storage == NULL) {
		return NULL;
	}
	CharacterVisual* visual = new (storage) CharacterVisual((GameObjectTemplate*) p_template);
	visual->m_object.m_flags |= 0x400000;
	LegoU32* words = (LegoU32*) ((LegoU8*) &visual->m_object + 0xbc);
	words[0] = words[1] = words[2] = 0;
	words[3] = words[4] = words[5] = 0;
	words[6] = 0x3f800000;
	visual->m_object.m_vtable->m_initialize(&visual->m_object, NULL);
	return (GameObject*) visual;
}

static void PrepareCharacterParts(
	const LegoChar* p_name,
	CharacterVisualTemplate* p_template,
	LegoBool32 p_duplicateHead,
	LegoBool32 p_hasLegs
)
{
	BodypartManager* manager = (BodypartManager*) g_gameController.m_bodypartManager;
	LegoU32 species = 0;
	p_template->m_unk0x3c = manager->FindHeadModel(p_name, &species);
	if (p_duplicateHead) {
		p_template->m_unk0x40 = p_template->m_unk0x3c;
	}
	p_template->m_unk0x44 = manager->FindTorsoTexture(p_name, &species);
	p_template->m_unk0x48 = p_hasLegs ? manager->FindLegTexture(p_name) : -1;
}

static void* LoadCharacterVisual(
	FoyerAssetCache* p_cache,
	LegoU32 p_slot,
	CharacterVisualTemplate* p_template,
	LegoS32* p_references
)
{
	GameObject** object = (GameObject**) ((LegoU8*) p_cache + p_slot);
	if (*object == NULL) {
		*object = CreateCharacterVisual(p_template);
	}
	++*p_references;
	return *object != NULL ? (void*) ((CharacterVisual*) *object)->m_body : NULL;
}

static void* LoadTemplateInstance(
	FoyerAssetCache* p_cache,
	LegoU32 p_slot,
	GameObjectTemplate* p_template,
	LegoS32* p_references,
	LegoBool32 p_defaultSize
)
{
	GameObject** object = (GameObject**) ((LegoU8*) p_cache + p_slot);
	if (*object == NULL) {
		void* storage = AllocateHeapMemory(0x2a4);
		*object = storage != NULL ? (GameObject*) new (storage) FoyerTemplateInstance(p_template) : NULL;
		if (*object != NULL) {
			if (p_defaultSize) {
				(*object)->m_flags |= 0x400000;
			}
			(*object)->m_vtable->m_initialize(*object, NULL);
			if (p_defaultSize) {
				(*object)->m_flags |= 2;
			}
			else {
				(*object)->m_flags |= 0x400000;
			}
		}
	}
	++*p_references;
	return *object;
}

// STUB: LEGORACERS2 0x00445ce0
static void InitializeDefaultSizeFoyerTemplate()
{
	InitializeFoyerCacheModelTemplate(
		&g_defaultSizeFoyerTemplate,
		0,
		"BIG STOMPY MECH RACER",
		0.5f,
		0.2f,
		2000.0f,
		0.005f,
		"game data\\animation\\bluemech\\bluemech.md2",
		"game data\\animation\\bluemech\\bluemech.bsb",
		"game data\\animation\\bluemech\\bluemech.skn"
	);
}

// STUB: LEGORACERS2 0x0047b590
FoyerAssetCache* __cdecl GetFoyerAssetCache()
{
	if ((g_foyerAssetCacheInitialized & 1) == 0) {
		g_foyerAssetCacheInitialized |= 1;
		ResetFoyerAssetCache(&g_foyerAssetCache, NULL);
		atexit(FoyerAssetCacheExitCallback);
	}
	return &g_foyerAssetCache;
}

// STUB: LEGORACERS2 0x0047b5d0
void __cdecl FoyerAssetCacheExitCallback()
{
}

// STUB: LEGORACERS2 0x0047b5e0
void __fastcall ResetFoyerAssetCache(FoyerAssetCache* p_cache, void*)
{
	memset(p_cache, 0, sizeof(*p_cache));
}

// STUB: LEGORACERS2 0x0047b610
void* __stdcall AcquireFoyerAsset(LegoU32 p_type)
{
	FoyerAssetCache* cache = GetFoyerAssetCache();
	switch (p_type) {
	case 1:
		return LoadSparkyFoyerAsset(cache, NULL);
	case 2:
		return LoadDefaultSizeFoyerAsset(cache, NULL);
	case 3:
		return LoadRigelFoyerAsset(cache, NULL);
	case 4:
		return LoadRocketRacerFoyerAsset(cache, NULL);
	case 5:
		return LoadPlayerFoyerAsset(cache, NULL);
	case 6:
		return LoadIntroFoyerAsset(cache, NULL);
	default:
		return NULL;
	}
}

// STUB: LEGORACERS2 0x0047b670
void __fastcall ReleaseFoyerAsset(FoyerAssetCache* p_cache, void*, LegoU32 p_type)
{
	GameObject** object;
	LegoS32* references;
	switch (p_type) {
	case 1:
		object = &p_cache->m_sparky;
		references = &p_cache->m_sparkyReferences;
		break;
	case 2:
		object = &p_cache->m_defaultSize;
		references = &p_cache->m_defaultSizeReferences;
		break;
	case 3:
		object = &p_cache->m_rigel;
		references = &p_cache->m_rigelReferences;
		break;
	case 4:
		object = &p_cache->m_rocketRacer;
		references = &p_cache->m_rocketRacerReferences;
		break;
	case 5:
		object = &p_cache->m_player;
		references = &p_cache->m_playerReferences;
		break;
	case 6:
		object = &p_cache->m_intro;
		references = &p_cache->m_introReferences;
		break;
	default:
		return;
	}

	--*references;
	if (*references == 0) {
		GameObject* resource = *object;
		resource->m_vtable->m_remove(resource, NULL);
		resource->m_vtable->m_destroy(resource, NULL, 1);
		*object = NULL;
	}
}

// STUB: LEGORACERS2 0x0047b7d0
void* __fastcall LoadSparkyFoyerAsset(FoyerAssetCache* p_cache, void*)
{
	if (p_cache->m_sparky == NULL) {
		PrepareCharacterParts("sparky", GetSparkyCharacterVisualTemplate(), TRUE, TRUE);
	}
	return LoadCharacterVisual(p_cache, 0, GetSparkyCharacterVisualTemplate(), &p_cache->m_sparkyReferences);
}

// STUB: LEGORACERS2 0x0047b8f0
void* __fastcall LoadDefaultSizeFoyerAsset(FoyerAssetCache* p_cache, void*)
{
	return LoadTemplateInstance(p_cache, 8, GetDefaultSizeFoyerTemplate(), &p_cache->m_defaultSizeReferences, 1);
}

// STUB: LEGORACERS2 0x0047b990
void* __fastcall LoadRigelFoyerAsset(FoyerAssetCache* p_cache, void*)
{
	if (p_cache->m_rigel == NULL) {
		CharacterVisualTemplate* model = GetRigelCharacterVisualTemplate();
		BodypartManager* manager = (BodypartManager*) g_gameController.m_bodypartManager;
		LegoU32 species = 0;
		model->m_unk0x3c = manager->FindHeadModel("rigel", &species);
		model->m_unk0x44 = manager->FindTorsoTexture("rigel", &species);
		model->m_unk0x40 = -1;
	}
	return LoadCharacterVisual(p_cache, 0x10, GetRigelCharacterVisualTemplate(), &p_cache->m_rigelReferences);
}

// STUB: LEGORACERS2 0x0047bae0
void* __fastcall LoadRocketRacerFoyerAsset(FoyerAssetCache* p_cache, void*)
{
	if (p_cache->m_rocketRacer == NULL) {
		PrepareCharacterParts("rocket racer", GetRocketRacerCharacterVisualTemplate(), TRUE, TRUE);
	}
	return LoadCharacterVisual(
		p_cache,
		0x18,
		GetRocketRacerCharacterVisualTemplate(),
		&p_cache->m_rocketRacerReferences
	);
}

// STUB: LEGORACERS2 0x0047bc10
static void InitializePlayerFoyerTemplate()
{
	InitializeFoyerCacheModelTemplate(
		&g_playerFoyerTemplate,
		2,
		"NoName",
		5.0f,
		0.3f,
		100.0f,
		0.01f,
		"game data\\animation\\theberg\\theberg.md2",
		"game data\\animation\\theberg\\theberg.bsb",
		"game data\\animation\\martian\\martian.skn"
	);
}

// STUB: LEGORACERS2 0x0047bdf0
void* __fastcall LoadPlayerFoyerAsset(FoyerAssetCache* p_cache, void*)
{
	return LoadTemplateInstance(p_cache, 0x20, GetPlayerFoyerTemplate(), &p_cache->m_playerReferences, 0);
}

// STUB: LEGORACERS2 0x0047be70
void* __fastcall LoadIntroFoyerAsset(FoyerAssetCache* p_cache, void*)
{
	if (p_cache->m_intro == NULL) {
		PrepareCharacterParts("sam sanister", GetIntroCharacterVisualTemplate(), TRUE, TRUE);
	}
	return LoadCharacterVisual(p_cache, 0x28, GetIntroCharacterVisualTemplate(), &p_cache->m_introReferences);
}
