#include "audio/service/audioservice.h"
#include "core/strings/charbuffer128.h"
#include "frontend/preview/cardisplay.h"
#include "game/character/bodypartmanager.h"
#include "game/character/bodypartresources.h"
#include "game/character/charactervisual.h"
#include "game/character/charactervisualbehavior.h"
#include "game/character/charactervisualbodylifecycle.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/objects/gameobjectmodelbinding.h"
#include "game/pause/pausedetector.h"
#include "game/runtime/gamerender.h"
#include "game/session/gamecontrollersession.h"
#include "memory/memoryheap.h"
#include "physics/dynamics/physicallifecycle.h"
#include "physics/timing/simulationtiming.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/models/modelbounds.h"
#include "render/models/modelshadow.h"
#include "render/resources/rendererrecords.h"
#include "scene/camera/controllercamera.h"
#include "scene/objects/foyermodelrender.h"
#include "scene/objects/foyertemplateinstance.h"

#include <new>
#include <string.h>

extern LegoU32 g_unk0x00572f94;

struct CharacterVisualBindingAssociation {
	void* m_binding;
	LegoFloat m_value;
};
DECOMP_SIZE_ASSERT(CharacterVisualBindingAssociation, 8);

// GLOBAL: LEGORACERS2 0x00570490
static CharacterVisualBindingAssociation g_characterVisualBindingAssociations[0xbe];
// GLOBAL: LEGORACERS2 0x00570c20
static LegoS32 g_characterVisualBindingAssociationCount;

// GLOBAL: LEGORACERS2 0x005703ac
static LegoS32 g_characterShieldHueStep;
// GLOBAL: LEGORACERS2 0x005703b0
static Vector3 g_characterShieldTexturePosition;
// GLOBAL: LEGORACERS2 0x005703d0
static Vector3 g_characterShieldTextureAngles;
// GLOBAL: LEGORACERS2 0x005703e0
static Vector3 g_characterSheenTexturePosition;
// GLOBAL: LEGORACERS2 0x005703f8
static Vector3 g_characterSheenTextureAngles;
// GLOBAL: LEGORACERS2 0x00570440
static Matrix3 g_characterSheenTextureRotation;
// GLOBAL: LEGORACERS2 0x00570abc
static LegoU8 g_characterSheenInitialized;
// GLOBAL: LEGORACERS2 0x00570ad0
static Matrix3 g_characterShieldTextureRotation;
// GLOBAL: LEGORACERS2 0x00570b14
static LegoU8 g_characterShieldInitialized;
// GLOBAL: LEGORACERS2 0x00570c24
static LegoU32 g_characterShieldHue;

struct CharacterVisualModelTemplate : GameObjectTemplate {
	LegoU32 m_properties[14];
	CharBuffer128 m_modelPath;
	LegoU32 m_unk0xec;
};
DECOMP_SIZE_ASSERT(CharacterVisualModelTemplate, 0xf0);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisualModelTemplate, m_properties) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisualModelTemplate, m_modelPath) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisualModelTemplate, m_unk0xec) == 0xec);

struct CharacterVisualBodyTemplate : CharacterVisualModelTemplate {
	CharBuffer128 m_skeletonPath;
	CharBuffer128 m_skinPath;
};
DECOMP_SIZE_ASSERT(CharacterVisualBodyTemplate, 0x1f0);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisualBodyTemplate, m_skeletonPath) == 0xf0);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisualBodyTemplate, m_skinPath) == 0x170);

// GLOBAL: LEGORACERS2 0x0056f020
static CharacterVisualBodyTemplate g_humanCharacterVisualBodyTemplate;
// GLOBAL: LEGORACERS2 0x0056ed10
static CharacterVisualBodyTemplate g_alienCharacterVisualBodyTemplate;
// GLOBAL: LEGORACERS2 0x0056f218
static CharacterVisualBodyTemplate g_ramaCharacterVisualBodyTemplate;
// GLOBAL: LEGORACERS2 0x0056f530
static CharacterVisualModelTemplate g_characterVisualHeadTemplate;

static void InitializeCharacterVisualModelTemplate(
	CharacterVisualModelTemplate* p_template,
	const LegoChar* p_name,
	const LegoChar* p_modelPath,
	LegoU32 p_secondProperty
)
{
	p_template->m_flags = 0;
	p_template->m_first = CharBuffer24(p_name);
	p_template->m_second = CharBuffer24("");
	for (LegoU32 i = 0; i < 14; ++i) {
		p_template->m_properties[i] = 0;
	}
	p_template->m_properties[6] = 0x3f800000;
	p_template->m_properties[7] = 0x3f000000;
	p_template->m_properties[8] = p_secondProperty;
	p_template->m_properties[10] = 0x42c80000;
	p_template->m_properties[11] = 0x3c23d70a;
	p_template->m_properties[12] = 0x3c23d70a;
	p_template->m_properties[13] = 0x3c23d70a;
	p_template->m_modelPath = CharBuffer128(p_modelPath);
	p_template->m_unk0xec = 1;
}

static void InitializeHumanCharacterVisualBodyTemplate();
static void InitializeHeadCharacterVisualTemplate();
static void InitializeAlienCharacterVisualBodyTemplate();
static void InitializeRamaCharacterVisualBodyTemplate();

static void InitializeCharacterVisualModelTemplates()
{
	static LegoU32 initialized;
	if (initialized == 0) {
		InitializeHumanCharacterVisualBodyTemplate();
		InitializeHeadCharacterVisualTemplate();
		InitializeAlienCharacterVisualBodyTemplate();
		InitializeRamaCharacterVisualBodyTemplate();
		initialized = 1;
	}
}

static void __fastcall SetCharacterVisualBodyPreviewAngle(GameObject*, void*, LegoFloat);

// GLOBAL: LEGORACERS2 0x00572a48
static LegoU32 g_foyerDefaultBodypartNameStorage[6];

// GLOBAL: LEGORACERS2 0x00572780
static CharacterVisualTemplate g_sparkyCharacterVisualTemplate;
// GLOBAL: LEGORACERS2 0x005727e8
static CharacterVisualTemplate g_introCharacterVisualTemplate;
// GLOBAL: LEGORACERS2 0x00572a60
static CharacterVisualTemplate g_rocketRacerCharacterVisualTemplate;
// GLOBAL: LEGORACERS2 0x00572ac8
static CharacterVisualTemplate g_rigelCharacterVisualTemplate;

static LegoU32 g_characterVisualTemplateInitialized;
static CharBuffer24* GetFoyerDefaultBodypartName();
static void InitializeFoyerDefaultBodypartName();
static void InitializeSparkyCharacterVisualTemplate();
static void InitializeRigelCharacterVisualTemplate();
static void InitializeRocketRacerCharacterVisualTemplate();
static void InitializeIntroCharacterVisualTemplate();

static void InitializeCharacterVisualTemplates()
{
	if (g_characterVisualTemplateInitialized == 0) {
		InitializeFoyerDefaultBodypartName();
		InitializeSparkyCharacterVisualTemplate();
		InitializeRigelCharacterVisualTemplate();
		InitializeRocketRacerCharacterVisualTemplate();
		InitializeIntroCharacterVisualTemplate();
		g_characterVisualTemplateInitialized = 1;
	}
}

CharacterVisualTemplate* GetSparkyCharacterVisualTemplate()
{
	InitializeCharacterVisualTemplates();
	return &g_sparkyCharacterVisualTemplate;
}

CharacterVisualTemplate* GetIntroCharacterVisualTemplate()
{
	InitializeCharacterVisualTemplates();
	return &g_introCharacterVisualTemplate;
}

CharacterVisualTemplate* GetRocketRacerCharacterVisualTemplate()
{
	InitializeCharacterVisualTemplates();
	return &g_rocketRacerCharacterVisualTemplate;
}

CharacterVisualTemplate* GetRigelCharacterVisualTemplate()
{
	InitializeCharacterVisualTemplates();
	return &g_rigelCharacterVisualTemplate;
}

struct CharacterVisualDispatch {
	GameObjectDispatchView m_base;
	GameObjectDispatchView::Value m_getObjectSize;
};
DECOMP_SIZE_ASSERT(CharacterVisualDispatch, 0x54);

struct CharacterVisualHeadInstance : GameObject {
	undefined m_tail[0x18c - 0x58];

	CharacterVisualHeadInstance(LegoU32 p_type, GameObjectTemplate* p_template);
	~CharacterVisualHeadInstance();
};
DECOMP_SIZE_ASSERT(CharacterVisualHeadInstance, 0x18c);

GameObject* __fastcall DestroyCharacterVisualHead(GameObject*, void*, LegoU32);
static void __fastcall RefreshCharacterVisualModelFlags(GameObjectModelBinding*, void*);
static void __fastcall BindCharacterVisualBodyTemplate(GameObject*, void*, GameObjectTemplate*, LegoU32);
static void __fastcall ReleaseCharacterVisualAudioSources(CharacterVisual*, void*);
static void __fastcall UnregisterCharacterVisualBindingAssociation(GameController*, void*, void*);
static void __fastcall ReleaseCharacterVisualResources(CharacterVisual*, void*);
static void __fastcall InitializeCharacterVisualResources(CharacterVisual*, void*);
static void __fastcall ResetCharacterVisualPreviewFrame(CharacterVisual*, void*);
static void __fastcall ResetCharacterVisualAnimationState(CharacterVisual*, void*);
static void __fastcall ConfigureCharacterShadowBounds(ModelShadowData*, void*, ModelBounds*, LegoFloat, LegoFloat);
static void __fastcall UpdateCharacterVisualPreviewAnimation(CharacterVisual*, void*);
static void __fastcall SetCharacterVisualBindingAssociationValue(GameController*, void*, void*, LegoFloat);
static void __fastcall RegisterCharacterVisualBindingAssociation(GameController*, void*, void*, LegoFloat);
static void InitializeCharacterVisualBindingAssociationStorage();
static void __fastcall UpdateCharacterVisualBindingTransform(GameObjectModelBinding*, void*, const LegoS32*);
static void __fastcall ReleaseCharacterVisualModelBinding(GameObjectModelBinding*, void*, LegoU32, LegoU32, LegoU32);

LegoU32 __fastcall IntegrateFoyerModelRenderMotion(GameObject*, void*);
LegoU32 __fastcall ApplyFoyerModelRenderForces(GameObject*, void*);
LegoU32 __fastcall IntegrateFoyerLifecycleMotion(PhysicalLifecycleState*, void*);
LegoU32 __fastcall ApplyFoyerLifecycleForces(PhysicalLifecycleState*, void*);
void __fastcall DrawFoyerModelRenderObject(GameObject*, void*);
GameObject* __fastcall DestroyCharacterVisualHeadLifecycle(PhysicalLifecycleState*, void*, LegoU32);

// Retail ABI function-pointer table: 0x00530880.
#define CHARACTER_CALLBACK(p_function) ((void(__cdecl*)())(p_function))
extern const PhysicalLifecycleDispatch g_characterVisualHeadLifecycleDispatch = {
	{CHARACTER_CALLBACK(&IsLifecycleActive),
	 CHARACTER_CALLBACK(&GetLifecycleParentValue),
	 CHARACTER_CALLBACK(&GameObjectBaseTrue),
	 CHARACTER_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 CHARACTER_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CHARACTER_CALLBACK(&GetLifecyclePosition),
	 CHARACTER_CALLBACK(&GetLifecycleRotation),
	 CHARACTER_CALLBACK(&GetLifecycleOrientation),
	 CHARACTER_CALLBACK(&SetLifecyclePosition),
	 CHARACTER_CALLBACK(&SetLifecycleOrientation),
	 CHARACTER_CALLBACK(&GetLifecycleParent),
	 CHARACTER_CALLBACK(&IntegrateFoyerLifecycleMotion),
	 CHARACTER_CALLBACK(&ApplyFoyerLifecycleForces),
	 CHARACTER_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CHARACTER_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CHARACTER_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 CHARACTER_CALLBACK(&CaptureLifecycleTransform),
	 CHARACTER_CALLBACK(&GetCapturedLifecycleTransform),
	 CHARACTER_CALLBACK(&DestroyCharacterVisualHeadLifecycle),
	 CHARACTER_CALLBACK(&ClearLifecycleMotion)}
};
#undef CHARACTER_CALLBACK

// Retail ABI function-pointer table: 0x005308d0 (20 slots).
extern const GameObjectDispatchView g_characterVisualHeadDispatch = {
	&DestroyCharacterVisualHead,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&InitializeFoyerModelRenderObject,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&CarDisplayModelRemove,
	&IntegrateFoyerModelRenderMotion,
	&ApplyFoyerModelRenderForces,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&DrawFoyerModelRenderObject,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	(GameObjectDispatchView::Update) &UpdateFoyerModelRenderRegion
};

// STUB: LEGORACERS2 0x0042f3a0
GameObject* __fastcall DestroyCharacterVisualHead(GameObject* p_object, void*, LegoU32 p_flags)
{
	((CharacterVisualHeadInstance*) p_object)->~CharacterVisualHeadInstance();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0043f8d0
CharacterVisualHeadInstance::CharacterVisualHeadInstance(LegoU32 p_type, GameObjectTemplate* p_template)
	: GameObject(p_type, p_template, (LegoU8*) this + 0x58)
{
	InitializeCharacterVisualHeadBase(this, p_template);
}

void InitializeCharacterVisualHeadBase(GameObject* p_object, GameObjectTemplate* p_template)
{
	InitializeFoyerModelRenderBase(p_object, p_template);
	*(LegoU32*) ((LegoU8*) p_object + 0x184) = 1;
	*(LegoU32*) ((LegoU8*) p_object + 0x180) = 0;
	*(LegoU32*) ((LegoU8*) p_object + 0x188) = 0;
	p_object->m_vtable = &g_characterVisualHeadDispatch;
	*(const PhysicalLifecycleDispatch**) ((LegoU8*) p_object + 0x58) = &g_characterVisualHeadLifecycleDispatch;
}

CharacterVisualHeadInstance::~CharacterVisualHeadInstance()
{
	m_vtable = &g_characterVisualHeadDispatch;
	*(const PhysicalLifecycleDispatch**) ((LegoU8*) this + 0x58) = &g_characterVisualHeadLifecycleDispatch;
	ReleaseFoyerModelRenderBase(this);
}

// FUNCTION: LEGORACERS2 0x0043ff70
static void __fastcall RefreshCharacterVisualModelFlags(GameObjectModelBinding* p_binding, void*)
{
	if (p_binding->m_boundMinimumX == -1 && p_binding->m_boundMinimumY == -1) {
		p_binding->m_flags &= ~0x10;
	}
	else {
		p_binding->m_flags |= 0x10;
	}
}

struct CharacterVisualBodyInstance : FoyerTemplateInstance {
	undefined m_tail[0x3a0 - 0x2a4];

	CharacterVisualBodyInstance(GameObjectTemplate* p_template);
};
DECOMP_SIZE_ASSERT(CharacterVisualBodyInstance, 0x3a0);

// STUB: LEGORACERS2 0x0044e420
void CharacterVisualTemplate::Initialize(
	LegoU32 p_field34,
	LegoU32 p_field38,
	LegoU32 p_field3c,
	LegoU32 p_field44,
	LegoU32 p_field48,
	const CharBuffer24* p_bodypartName
)
{
	m_flags = 2;
	m_first = CharBuffer24("NoName");
	m_second = CharBuffer24("");
	m_mode = p_field34;
	m_unk0x38 = p_field38;
	m_unk0x3c = p_field3c;
	m_unk0x40 = 0;
	m_unk0x44 = p_field44;
	m_unk0x48 = p_field48;
	m_bodypartName = *p_bodypartName;
}

// STUB: LEGORACERS2 0x0044e4c0
static void InitializeHumanCharacterVisualBodyTemplate()
{
	InitializeCharacterVisualModelTemplate(
		&g_humanCharacterVisualBodyTemplate,
		"NoName",
		"game data\\characters\\bodies\\models\\minifig body.md2",
		0x3e4ccccd
	);
	g_humanCharacterVisualBodyTemplate.m_skeletonPath =
		CharBuffer128("game data\\characters\\bodies\\models\\minifig body.bsb");
	g_humanCharacterVisualBodyTemplate.m_skinPath =
		CharBuffer128("game data\\characters\\bodies\\models\\minifig body.skn");
}

// STUB: LEGORACERS2 0x0044e6b0
static void InitializeHeadCharacterVisualTemplate()
{
	InitializeCharacterVisualModelTemplate(
		&g_characterVisualHeadTemplate,
		"MiniFigHead",
		"game data\\characters\\heads\\models\\alexis.md2",
		0x3f000000
	);
	for (LegoU32 i = 10; i < 14; ++i) {
		g_characterVisualHeadTemplate.m_properties[i] = 0x3f800000;
	}
	g_characterVisualHeadTemplate.m_flags = 2;
}

// STUB: LEGORACERS2 0x0044e850
static void InitializeAlienCharacterVisualBodyTemplate()
{
	InitializeCharacterVisualModelTemplate(
		&g_alienCharacterVisualBodyTemplate,
		"NoName",
		"game data\\aliens\\alien bodies\\models\\alien body.md2",
		0x3e4ccccd
	);
	g_alienCharacterVisualBodyTemplate.m_skeletonPath =
		CharBuffer128("game data\\aliens\\alien bodies\\models\\alien body.bsb");
	g_alienCharacterVisualBodyTemplate.m_skinPath =
		CharBuffer128("game data\\aliens\\alien bodies\\models\\alien body.skn");
}

// STUB: LEGORACERS2 0x0044ea40
static void InitializeRamaCharacterVisualBodyTemplate()
{
	InitializeCharacterVisualModelTemplate(
		&g_ramaCharacterVisualBodyTemplate,
		"NoName",
		"game data\\ramas\\rama bodies\\models\\rama body.md2",
		0x3e4ccccd
	);
	g_ramaCharacterVisualBodyTemplate.m_skeletonPath =
		CharBuffer128("game data\\ramas\\rama bodies\\models\\rama body.bsb");
	g_ramaCharacterVisualBodyTemplate.m_skinPath =
		CharBuffer128("game data\\ramas\\rama bodies\\models\\rama body.skn");
}

// STUB: LEGORACERS2 0x0044ec20
CharacterVisualBodyInstance::CharacterVisualBodyInstance(GameObjectTemplate* p_template)
	: FoyerTemplateInstance(p_template)
{
	*(LegoU32*) ((LegoU8*) this + 0x2e8) = 1;
	*(LegoU32*) ((LegoU8*) this + 0x2a4) = 0;
	*(LegoU32*) ((LegoU8*) this + 0x304) = 0;
	*(LegoU32*) ((LegoU8*) this + 0x32c) = 0;
	*(LegoU32*) ((LegoU8*) this + 0x334) = 0;
	*(LegoU32*) ((LegoU8*) this + 0x308) = 0;
	m_vtable = &g_characterVisualBodyDispatch.m_object;
	m_physical.m_base.m_vtable = (const GameLifecycleDispatchView*) &g_characterVisualBodyLifecycleDispatch;
	*(LegoU32*) ((LegoU8*) this + 0x39c) = 0;
}

static GameObject* __fastcall DestroyCharacterVisual(GameObject*, void*, LegoU32);
static void __fastcall InitializeCharacterVisual(GameObject*, void*);
static LegoU32 __fastcall UpdateCharacterVisual(GameObject*, void*, LegoU32);
static void __fastcall RemoveCharacterVisual(GameObject*, void*);
static LegoU32 __fastcall CharacterVisualApplyAnimation(GameObject*, void*, LegoU32, LegoU32);

// Retail table at 0x00531be4.
extern const CharacterVisualDispatch g_characterVisualDispatch = {
	{&DestroyCharacterVisual,     &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &InitializeCharacterVisual,  &GameObjectBaseTrue,
	 &UpdateCharacterVisual,      &RemoveCharacterVisual,
	 &GameObjectBaseZero,         &GameObjectBaseZero,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
	 &GameObjectBaseZero,         &CharacterVisualApplyAnimation,
	 &GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument},
	&GetCharacterVisualObjectSize
};

// STUB: LEGORACERS2 0x0044ec50
static void __fastcall InitializeCharacterVisual(GameObject* p_object, void*)
{
	CharacterVisual* visual = (CharacterVisual*) p_object;
	LegoU8* bytes = (LegoU8*) visual;
	LegoBool32 created = FALSE;
	if ((p_object->m_flags & 0x800000) == 0) {
		InitializeCharacterVisualModelTemplates();
		void* controller = *(void**) (bytes + 0x5c);
		if (controller != NULL) {
			LegoU32 index = *(LegoU32*) ((LegoU8*) controller + 0x58);
			const LegoU8* settings = (const LegoU8*) g_gameControllerSession + index * 0x344;
			*(LegoU32*) (bytes + 0x84) = *(const LegoU32*) (settings + 0x370);
			*(LegoU32*) (bytes + 0x90) = *(const LegoU32*) (settings + 0x374);
			*(LegoU32*) (bytes + 0x88) = *(const LegoU32*) (settings + 0x378);
			*(LegoU32*) (bytes + 0x8c) = *(const LegoU32*) (settings + 0x37c);
		}
		else {
			*(LegoU32*) (bytes + 0x84) = *(LegoU32*) (bytes + 0x70);
			*(LegoU32*) (bytes + 0x90) = *(LegoU32*) (bytes + 0x74);
			*(LegoU32*) (bytes + 0x88) = *(LegoU32*) (bytes + 0x78);
			*(LegoU32*) (bytes + 0x8c) = *(LegoU32*) (bytes + 0x7c);
		}
		CharacterVisualModelTemplate* bodyTemplate = &g_humanCharacterVisualBodyTemplate;
		LegoU32 species = 0;
		if (*(LegoS32*) (bytes + 0x90) == -1) {
			species = 1;
			*(LegoU32*) (bytes + 0x90) = 0;
			bodyTemplate = &g_alienCharacterVisualBodyTemplate;
		}
		else if (*(LegoS32*) (bytes + 0x90) == -2) {
			species = 2;
			*(LegoU32*) (bytes + 0x90) = 0;
			bodyTemplate = &g_ramaCharacterVisualBodyTemplate;
		}
		*(LegoU32*) (bytes + 0x80) = species;
		memcpy(bodyTemplate->m_first.m_data, bytes + 0xa4, 0x18);
		void* bodyStorage = AllocateHeapMemory(0x3a0);
		visual->m_body = bodyStorage == NULL ? NULL : new (bodyStorage) CharacterVisualBodyInstance(bodyTemplate);
		visual->m_body->m_flags |= 0x400000;
		BodypartManager* manager = (BodypartManager*) g_gameController.m_bodypartManager;
		const CharBuffer128* head = manager->GetHeadModelPath(*(LegoS32*) (bytes + 0x84), species);
		memcpy(g_characterVisualHeadTemplate.m_modelPath.m_data, head, 0x80);
		void* headStorage = AllocateHeapMemory(0x18c);
		visual->m_head = headStorage == NULL ? NULL
											 : new (headStorage)
												   CharacterVisualHeadInstance(0x800, &g_characterVisualHeadTemplate);
		visual->m_head->m_flags |= 0x400000;
		SetCharacterVisualBodyPreviewAngle(visual->m_body, NULL, 70.0f);
		LegoS32* textures = (LegoS32*) (bytes + 0xd8);
		textures[0] = textures[1] = textures[2] = -1;
		if (species == 1) {
			textures[0] =
				g_displayRenderer
					->CreateTextureResource("game data\\Aliens\\alien bodies\\textures\\default body.tga", 0, -1, 1, 1);
		}
		else if (species == 2) {
			textures[0] =
				g_displayRenderer
					->CreateTextureResource("game data\\ramas\\rama bodies\\textures\\default body.tga", 0, -1, 1, 1);
		}
		else {
			textures[0] = g_displayRenderer->CreateTextureResource(
				"game data\\characters\\bodies\\textures\\default grey torso.tga",
				0,
				-1,
				1,
				1
			);
			textures[1] =
				g_displayRenderer
					->CreateTextureResource("game data\\characters\\heads\\textures\\null face.tga", 0, -1, 1, 1);
			textures[2] = g_displayRenderer->CreateTextureResource(
				"game data\\characters\\bodies\\textures\\default grey legs.tga",
				0,
				-1,
				1,
				1
			);
		}
		SetBodypartTorsoTextureLoaded(manager, NULL, *(LegoS32*) (bytes + 0x88), species, 1);
		if (species == 0) {
			SetBodypartLegTextureLoaded(manager, NULL, *(LegoS32*) (bytes + 0x8c), 1);
			SetBodypartFaceTextureLoaded(manager, NULL, *(LegoU32*) (bytes + 0x90), 0, 1);
		}
		InitializeCharacterVisualFootstepAudio(visual, NULL);
		created = TRUE;
	}
	p_object->GameObject::Initialize();
	p_object->m_flags = (p_object->m_flags & 0xffe0dfff) | 0xc000;
	visual->m_savedHeadModel = NULL;
	*(LegoU32*) (bytes + 0x94) = 0;
	visual->m_body->m_vtable->m_initialize(visual->m_body, NULL);
	if (created) {
		GameObjectModelBinding* binding = (GameObjectModelBinding*) visual->m_body->m_unk0x48;
		ModelResource* model = (ModelResource*) binding->m_model;
		ReleaseCharacterVisualModelBinding(
			binding,
			NULL,
			1,
			(LegoU32) "game data\\common\\effects\\shadow\\mini_shadow.tga",
			0
		);
		ConfigureCharacterShadowBounds((ModelShadowData*) model->m_unk0x28, NULL, model->m_bounds, 1.75f, 3.5f);
		InitializeCharacterVisualResources(visual, NULL);
	}
	*(LegoU32*) (bytes + 0x170) = 0;
	ResetCharacterVisualPreviewFrame(visual, NULL);
	ResetCharacterVisualAnimationState(visual, NULL);
	visual->m_body->m_flags = (visual->m_body->m_flags & 0xffe0dfff) | 0xc000;
	*(CharacterVisual**) ((LegoU8*) visual->m_body + 0x39c) = visual;
	ApplyFoyerModelRenderTransform(visual->m_body, (LegoFloat*) (bytes + 0xbc), (LegoFloat*) (bytes + 0xc8));
	BindCharacterVisualBodyTemplate(visual->m_body, NULL, *(GameObjectTemplate**) (bytes + 0x5c), 0);
	visual->m_head->m_vtable->m_initialize(visual->m_head, NULL);
	visual->m_head->m_flags = (visual->m_head->m_flags & 0xffe03fff) | 0x2000;
	LegoU32 species = *(LegoU32*) (bytes + 0x80);
	visual->m_torsoTexture = *(LegoS32*) (bytes + 0xd8);
	if (species == 0) {
		visual->m_legTexture = *(LegoS32*) (bytes + 0xe0);
		visual->m_faceTexture = *(LegoS32*) (bytes + 0xdc);
		visual->SetFaceGroup(*(LegoS32*) (bytes + 0x90));
		visual->SetFaceVariant(0);
		visual->SetTorsoTexture(*(LegoS32*) (bytes + 0x88), 0);
		visual->SetLegTexture(*(LegoS32*) (bytes + 0x8c));
	}
	else {
		visual->SetTorsoTexture(*(LegoS32*) (bytes + 0x88), species);
	}
	void* controller = *(void**) (bytes + 0x5c);
	if (controller != NULL) {
		*(LegoU32*) ((LegoU8*) visual->m_body + 0xf4) = *(LegoU32*) ((LegoU8*) controller + 0x60);
	}
	*(LegoU32*) (bytes + 0x60) = 0;
	SetCharacterVisualMode(visual, NULL, visual->m_mode);
	*(LegoU32*) (bytes + 0x18c) = 0;
	*(LegoU32*) (bytes + 0x190) = 0;
	*(LegoU32*) (bytes + 0x194) = 0;
}

// STUB: LEGORACERS2 0x0044f0d0
static void __fastcall RemoveCharacterVisual(GameObject* p_object, void*)
{
	CharacterVisual* visual = (CharacterVisual*) p_object;
	if (visual->m_savedHeadModel != NULL) {
		*(ModelResource**) ((LegoU8*) visual->m_head->m_unk0x48 + 8) = visual->m_savedHeadModel;
	}
	if (visual->m_head != NULL) {
		visual->m_head->m_vtable->m_remove(visual->m_head, NULL);
	}
	ReleaseCharacterVisualResources(visual, NULL);
	if (visual->m_body != NULL) {
		GameObjectModelBinding* binding = (GameObjectModelBinding*) visual->m_body->m_unk0x48;
		ReleaseCharacterVisualModelBinding(binding, NULL, 0, 0, 1);
		visual->m_body->m_vtable->m_remove(visual->m_body, NULL);
	}
	visual->m_object.GameObject::Remove();
	ReleaseCharacterVisualAudioSources(visual, NULL);
	LegoS32 mode = *(LegoS32*) ((LegoU8*) visual + 0x80);
	if (mode <= 0 || mode > 2) {
		g_displayRenderer->ReleaseTextureResourceReference(*(LegoU32*) ((LegoU8*) visual + 0xe0), 1);
		g_displayRenderer->ReleaseTextureResourceReference(*(LegoU32*) ((LegoU8*) visual + 0xdc), 1);
	}
	g_displayRenderer->ReleaseTextureResourceReference(*(LegoU32*) ((LegoU8*) visual + 0xd8), 1);
	BodypartManager* manager = (BodypartManager*) g_gameController.m_bodypartManager;
	if (manager != NULL) {
		LegoU8* bytes = (LegoU8*) visual;
		void* controller = *(void**) (bytes + 0x5c);
		LegoU32 species = *(LegoU32*) (bytes + 0x80);
		if (controller != NULL) {
			LegoU32 index = *(LegoU32*) ((LegoU8*) controller + 0x58);
			const LegoU8* settings = (const LegoU8*) g_gameControllerSession + index * 0x344;
			if (species == 0) {
				SetBodypartFaceTextureLoaded(manager, NULL, *(const LegoU32*) (settings + 0x374), 0, 0);
				SetBodypartLegTextureLoaded(manager, NULL, *(const LegoS32*) (settings + 0x37c), 0);
			}
			SetBodypartTorsoTextureLoaded(manager, NULL, *(const LegoS32*) (settings + 0x378), species, 0);
		}
		else {
			if (species == 0) {
				SetBodypartFaceTextureLoaded(manager, NULL, *(LegoU32*) (bytes + 0x74), 0, 0);
				SetBodypartLegTextureLoaded(manager, NULL, *(LegoS32*) (bytes + 0x7c), 0);
			}
			SetBodypartTorsoTextureLoaded(manager, NULL, *(LegoS32*) (bytes + 0x78), species, 0);
		}
	}
	if (visual->m_head != NULL) {
		visual->m_head->m_vtable->m_destroy(visual->m_head, NULL, 1);
	}
	if (visual->m_body != NULL) {
		visual->m_body->m_vtable->m_destroy(visual->m_body, NULL, 1);
	}
}

// STUB: LEGORACERS2 0x0044f250
static LegoU32 __fastcall UpdateCharacterVisual(GameObject* p_object, void*, LegoU32 p_elapsed)
{
	CharacterVisual* visual = (CharacterVisual*) p_object;
	LegoU32 updateFlags = 1;
	LegoU32 animationState = 0;
	LegoU32 displayMode = 1;
	LegoFloat elapsed = 0.0f;
	GameObjectTemplate* source = (GameObjectTemplate*) *(LegoU32*) ((LegoU8*) visual + 0x5c);
	if (source != NULL) {
		if (g_unk0x00572f94 == 0) {
			elapsed = *(LegoFloat*) ((LegoU8*) source + 0x70);
		}
		animationState = *(LegoU32*) ((LegoU8*) source + 0x84);
		displayMode = *(LegoU32*) ((LegoU8*) source + 0x80);
		if (*(LegoU32*) ((LegoU8*) source + 0xdc) == 1) {
			updateFlags = 0;
		}
	}
	GameObject* body = visual->m_body;
	GameObject* head = visual->m_head;
	void* bodyData = *(void**) ((LegoU8*) body + 0x48);
	void* headData = *(void**) ((LegoU8*) head + 0x48);
	void* bodyModel = *(void**) ((LegoU8*) bodyData + 8);
	void* bodyModelState = *(void**) ((LegoU8*) bodyModel + 0x28);
	*(LegoU32*) ((LegoU8*) bodyModelState + 0x10) = updateFlags;
	*(LegoFloat*) ((LegoU8*) head + 0x180) = elapsed;
	*(LegoFloat*) ((LegoU8*) body + 0x180) = elapsed;
	*(LegoS32*) ((LegoU8*) headData + 0xa8) = *(LegoS16*) ((LegoU8*) visual + 0x188);
	RefreshCharacterVisualModelFlags((GameObjectModelBinding*) headData, NULL);
	UpdateCharacterVisualBindingTransform(
		(GameObjectModelBinding*) headData,
		NULL,
		(LegoS32*) ((LegoU8*) visual + 0x180)
	);
	*(LegoU32*) ((LegoU8*) headData + 0xb4) |= 0x20;
	LegoS32 headFrame = (LegoS32) * (LegoS16*) ((LegoU8*) visual + 0x188);
	*(LegoS32*) ((LegoU8*) bodyData + 0xa8) = headFrame;
	if (headFrame == -1 && *(LegoS32*) ((LegoU8*) bodyData + 0xac) == -1) {
		*(LegoU32*) ((LegoU8*) bodyData + 0xb4) &= ~0x10;
	}
	else {
		*(LegoU32*) ((LegoU8*) bodyData + 0xb4) |= 0x10;
	}
	UpdateCharacterVisualBindingTransform(
		(GameObjectModelBinding*) bodyData,
		NULL,
		(LegoS32*) ((LegoU8*) visual + 0x180)
	);
	*(LegoU32*) ((LegoU8*) bodyData + 0xb4) |= 0x20;
	UpdateCharacterVisualPreviewAnimation(visual, NULL);
	if (displayMode != 0) {
		*(LegoU32*) ((LegoU8*) head + 0x184) = 1;
		*(LegoU32*) ((LegoU8*) body + 0x184) = 1;
	}
	else {
		*(LegoU32*) ((LegoU8*) head + 0x188) = animationState;
		*(LegoU32*) ((LegoU8*) head + 0x184) = 0;
		*(LegoU32*) ((LegoU8*) body + 0x188) = animationState;
		*(LegoU32*) ((LegoU8*) body + 0x184) = 0;
	}
	LegoFloat frameTime = *(LegoFloat*) &p_elapsed;
	switch (visual->m_mode) {
	case 1:
		UpdateCharacterDrivingAnimation(visual, NULL, frameTime);
		break;
	case 2:
		UpdateCharacterRunningAnimation(visual, NULL, frameTime);
		break;
	case 3:
		UpdateCharacterIdleAnimation(visual, NULL, frameTime);
		break;
	case 4:
		UpdateCharacterSelectionAnimation(visual, NULL, frameTime);
		break;
	case 6:
		UpdateCharacterHiddenAnimation(visual, NULL, frameTime);
		break;
	default:
		UpdateCharacterPassiveAnimation(visual, NULL, frameTime);
		break;
	}
	return 1;
}

// STUB: LEGORACERS2 0x0044f450
CharacterVisual::CharacterVisual(GameObjectTemplate* p_template) : m_object(0xa00, p_template, NULL)
{
	const CharacterVisualTemplate* source = (const CharacterVisualTemplate*) p_template;
	LegoU32* bodyparts = (LegoU32*) ((LegoU8*) this + 0xa4);
	LegoU32 i;
	const LegoU32* sourceBodyparts = (const LegoU32*) source->m_bodypartName.m_data;

	m_mode = source->m_unk0x38;
	*(LegoU32*) ((LegoU8*) this + 0x5c) = source->m_mode;
	*(LegoU32*) ((LegoU8*) this + 0x70) = source->m_unk0x3c;
	*(LegoU32*) ((LegoU8*) this + 0x74) = source->m_unk0x40;
	*(LegoU32*) ((LegoU8*) this + 0x78) = source->m_unk0x44;
	*(LegoU32*) ((LegoU8*) this + 0x7c) = source->m_unk0x48;
	for (i = 0; i < 6; ++i) {
		bodyparts[i] = sourceBodyparts[i];
	}
	*(LegoU32*) ((LegoU8*) this + 0xbc) = 0;
	*(LegoU32*) ((LegoU8*) this + 0xc0) = 0;
	*(LegoU32*) ((LegoU8*) this + 0xc4) = 0;
	m_previewOrientation.m_x = 0.0f;
	m_previewOrientation.m_y = 0.0f;
	m_previewOrientation.m_z = 0.0f;
	m_previewOrientation.m_w = 1.0f;
	*(LegoU32*) ((LegoU8*) this + 0x180) = 0xffffffff;
	*(LegoU16*) ((LegoU8*) this + 0x184) = 0xffff;
	*((LegoU8*) this + 0x186) = 0x0f;
	*((LegoU8*) this + 0x187) = 3;
	m_object.m_vtable = &g_characterVisualDispatch.m_base;
}

CharacterVisual::~CharacterVisual()
{
	m_object.m_vtable = &g_characterVisualDispatch.m_base;
}

// FUNCTION: LEGORACERS2 0x0044f530
LegoU32 __fastcall GetCharacterVisualObjectSize(GameObject*, void*)
{
	return 0x198;
}

// STUB: LEGORACERS2 0x0044f540
static GameObject* __fastcall DestroyCharacterVisual(GameObject* p_object, void*, LegoU32 p_flags)
{
	CharacterVisual* visual = (CharacterVisual*) p_object;
	visual->~CharacterVisual();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// FUNCTION: LEGORACERS2 0x00450e40
static LegoU32 __fastcall CharacterVisualApplyAnimation(GameObject* p_object, void*, LegoU32 p_first, LegoU32 p_second)
{
	UpdateControllerFollowCamera(
		*(void**) ((LegoU8*) p_object + 0x5c),
		NULL,
		(GameObject*) p_first,
		(LegoU32*) p_second,
		6.0f,
		1.0f
	);
	return 1;
}

// STUB: LEGORACERS2 0x00451210
static void __fastcall ReleaseCharacterVisualAudioSources(CharacterVisual* p_visual, void*)
{
	AudioService* audio = GetAudioService(1);
	LegoU8* body = (LegoU8*) p_visual->m_body;
	audio->m_vtable->m_releaseSource(audio, NULL, (AudioSourceRecord*) *(LegoU32*) (body + 0x310));
	audio->m_vtable->m_releaseSource(audio, NULL, (AudioSourceRecord*) *(LegoU32*) (body + 0x30c));
}

// FUNCTION: LEGORACERS2 0x00451250
static void __fastcall SetCharacterVisualPreviewFrame(CharacterVisual* p_visual, void*, const LegoU32* p_words)
{
	*(LegoU32*) ((LegoU8*) p_visual + 0x180) = p_words[0];
	*(LegoU32*) ((LegoU8*) p_visual + 0x184) = p_words[1];
}

// FUNCTION: LEGORACERS2 0x00451270
static void __fastcall SetCharacterVisualAnimationState(CharacterVisual* p_visual, void*, LegoU32 p_value)
{
	*(LegoU32*) ((LegoU8*) p_visual + 0x188) = p_value;
}

// FUNCTION: LEGORACERS2 0x00451280
static void __fastcall ResetCharacterVisualPreviewFrame(CharacterVisual* p_visual, void*)
{
	*(LegoU32*) ((LegoU8*) p_visual + 0x180) = 0xffffffff;
}

// FUNCTION: LEGORACERS2 0x00451290
static void __fastcall ResetCharacterVisualAnimationState(CharacterVisual* p_visual, void*)
{
	*(LegoU32*) ((LegoU8*) p_visual + 0x188) = 0xffffffff;
}

// STUB: LEGORACERS2 0x004512a0
static void __fastcall InitializeCharacterVisualResources(CharacterVisual* p_visual, void*)
{
	RendererAuxiliaryResourceRecord material;
	memset(&material, 0, sizeof(material));
	for (LegoU32 i = 0; i < 4; ++i) {
		material.m_scaledColor1[i] = 1.0f;
	}
	material.m_modeFactor = 0.5f;
	material.m_mode = 1;
	strcpy(material.m_path, "SHIELD");
	*(LegoS32*) ((LegoU8*) p_visual + 0x17c) = g_displayRenderer->AcquireAuxiliaryResource(&material, 1);
	*(LegoS32*) ((LegoU8*) p_visual + 0x178) =
		g_displayRenderer->CreateTextureResource("game data\\common\\textures\\shield\\shield.tga", 1, -1, 1, 1);
	*(LegoU32*) ((LegoU8*) p_visual + 0x170) = 0;
	*(LegoU32*) ((LegoU8*) p_visual + 0x174) = 0;
	RegisterCharacterVisualBindingAssociation(&g_gameController, NULL, p_visual->m_head->m_unk0x48, 0.0f);
	RegisterCharacterVisualBindingAssociation(&g_gameController, NULL, p_visual->m_body->m_unk0x48, 0.0f);
}

// STUB: LEGORACERS2 0x004513f0
static void __fastcall ReleaseCharacterVisualResources(CharacterVisual* p_visual, void*)
{
	UnregisterCharacterVisualBindingAssociation(&g_gameController, NULL, p_visual->m_body->m_unk0x48);
	UnregisterCharacterVisualBindingAssociation(&g_gameController, NULL, p_visual->m_head->m_unk0x48);
	g_displayRenderer->ReleaseTextureResourceReference(*(LegoU32*) ((LegoU8*) p_visual + 0x178), 1);
	g_displayRenderer->ReleaseAuxiliaryResource(*(LegoS32*) ((LegoU8*) p_visual + 0x17c));
}

// STUB: LEGORACERS2 0x00451440
static void __fastcall UpdateCharacterVisualPreviewAnimation(CharacterVisual* p_visual, void*)
{
	LegoU8* bytes = (LegoU8*) p_visual;
	LegoFloat timeLeft = *(LegoFloat*) (bytes + 0x170);
	if (timeLeft > 0.0f) {
		if (g_unk0x00572f94 != 1) {
			LegoFloat endTime = *(LegoFloat*) (bytes + 0x174);
			LegoFloat progress;
			if (endTime - 0.2f < timeLeft) {
				progress = 1.0f - ((endTime - timeLeft) * 5.0f);
			}
			else if (endTime - 2.0f < timeLeft) {
				progress = 0.0f;
			}
			else {
				LegoFloat fadeTime = endTime - 2.0f;
				progress = (fadeTime - timeLeft) / fadeTime;
			}
			SetCharacterVisualBindingAssociationValue(&g_gameController, NULL, p_visual->m_head->m_unk0x48, progress);
			SetCharacterVisualBindingAssociationValue(&g_gameController, NULL, p_visual->m_body->m_unk0x48, progress);
			timeLeft = *(LegoFloat*) (bytes + 0x170) - g_simulationFrameRateScale * 0.0333333351f;
			*(LegoFloat*) (bytes + 0x170) = timeLeft;
			if (timeLeft >= 0.0f) {
				return;
			}
			*(LegoFloat*) (bytes + 0x170) = 0.0f;
		}
		ResetCharacterVisualPreviewFrame(p_visual, NULL);
		ResetCharacterVisualAnimationState(p_visual, NULL);
	}
}

// STUB: LEGORACERS2 0x00451540
void __fastcall BeginCharacterShieldEffect(CharacterVisual* p_visual, void*, LegoFloat p_duration)
{
	LegoU8* bytes = (LegoU8*) p_visual;
	if (g_unk0x00572f94 != 1 && *(LegoFloat*) (bytes + 0x170) <= 0.0f) {
		LegoU32 words[2];
		words[0] = 1;
		words[1] = *(LegoU16*) (bytes + 0x178);
		SetCharacterVisualPreviewFrame(p_visual, NULL, words);
		SetCharacterVisualAnimationState(p_visual, NULL, *(LegoU32*) (bytes + 0x17c));
		UpdateCharacterVisualPreviewAnimation(p_visual, NULL);
		*(LegoFloat*) (bytes + 0x170) = p_duration;
		*(LegoFloat*) (bytes + 0x174) = p_duration;
	}
}

// STUB: LEGORACERS2 0x004524c0
static void __fastcall BindCharacterVisualBodyTemplate(
	GameObject* p_object,
	void*,
	GameObjectTemplate* p_template,
	LegoU32 p_value
)
{
	LegoU8* body = (LegoU8*) p_object;
	*(LegoU32*) (body + 0x2ac) = 0;
	*(LegoU32*) (body + 0x2b0) = 0x3f800000;
	*(LegoU32*) (body + 0x2b4) = 0;
	*(LegoU32*) (body + 0x2c4) = 0;
	*(LegoU32*) (body + 0x2c8) = 0x3f800000;
	*(LegoU32*) (body + 0x2a8) = 0;
	*(LegoU32*) (body + 0x2f8) = 0;
	*(LegoU32*) (body + 0x2f0) = 0;
	*(LegoU32*) (body + 0x2cc) = 0;
	*(LegoU32*) (body + 0x2b8) = 0;
	*(LegoU32*) (body + 0x2fc) = 0;
	*(LegoU32*) (body + 0x2f4) = 0x3f800000;
	*(LegoU32*) (body + 0x2bc) = 0;
	*(LegoU32*) (body + 0x2c0) = 0;
	*(LegoU32*) (body + 0x2d0) = 0;
	*(LegoU32*) (body + 0x2ec) = (LegoU32) p_template;
	*(LegoU32*) (body + 0x2d4) = 0x3f800000;
	*(LegoU32*) (body + 0x2a4) = p_value;
	*(LegoU32*) (body + 0x2d8) = 0;
	*(LegoU32*) (body + 0x2dc) = 0;
	*(LegoU32*) (body + 0x2e0) = 0;
	*(LegoU32*) (body + 0x2e4) = 0;
}

// STUB: LEGORACERS2 0x0045e500
static void InitializeCharacterVisualBindingAssociationStorage()
{
	for (LegoU32 i = 0; i < 0xbe; ++i) {
		g_characterVisualBindingAssociations[i].m_binding = NULL;
		g_characterVisualBindingAssociations[i].m_value = 0.0f;
	}
}

// STUB: LEGORACERS2 0x0045e520
LegoU32 __cdecl CharacterShieldResourceCallback(RendererAuxiliaryResourceRecord* p_material, LegoU32 p_phase)
{
	if ((g_characterShieldInitialized & 1) == 0) {
		g_characterShieldInitialized |= 1;
	}
	if ((g_characterShieldInitialized & 2) == 0) {
		g_characterShieldInitialized |= 2;
		g_characterShieldTexturePosition.m_x = g_characterShieldTexturePosition.m_y =
			g_characterShieldTexturePosition.m_z = 0.0f;
	}
	if ((g_characterShieldInitialized & 4) == 0) {
		g_characterShieldInitialized |= 4;
		g_characterShieldTextureAngles.m_x = g_characterShieldTextureAngles.m_y = g_characterShieldTextureAngles.m_z =
			0.0f;
	}
	if ((g_characterShieldInitialized & 8) == 0) {
		g_characterShieldInitialized |= 8;
		g_characterShieldHueStep = 0x1680000 / ((LegoS32) g_simulationFrameRate * 5);
	}
	if (p_phase == 0) {
		void* binding = g_displayRenderer->m_unk0x4cc;
		((const RendererMainDispatch*) g_displayRenderer->m_vtable)
			->m_setTextureTransform(
				g_displayRenderer,
				NULL,
				1,
				&g_characterShieldTextureRotation,
				&g_characterShieldTexturePosition
			);
		for (LegoS32 i = 0; i < g_characterVisualBindingAssociationCount; ++i) {
			if (g_characterVisualBindingAssociations[i].m_binding == binding) {
				p_material->m_modeFactor = g_characterVisualBindingAssociations[i].m_value;
				return 1;
			}
		}
	}
	else if (p_phase == 1) {
		g_characterShieldTextureAngles.m_x += 0.03f;
		g_characterShieldTextureAngles.m_y += 0.05f;
		g_characterShieldTextureAngles.m_z += 0.02f;
		g_characterShieldTextureRotation.FUN_004a4f60(&g_characterShieldTextureAngles.m_x);
		g_characterShieldHue += g_characterShieldHueStep;
		LegoU32 color;
		BuildAnimationHueColor(&color, (g_characterShieldHue >> 16) % 360, 0x3ff, 0x3ff);
		const LegoU8* channels = (const LegoU8*) &color;
		p_material->m_scaledColor1[0] = channels[2] * (1.0f / 255.0f);
		p_material->m_scaledColor1[1] = channels[1] * (1.0f / 255.0f);
		p_material->m_scaledColor1[2] = channels[0] * (1.0f / 255.0f);
		p_material->m_scaledColor1[3] = channels[3] * (1.0f / 255.0f);
	}
	return 1;
}

// STUB: LEGORACERS2 0x0045e7a0
LegoU32 __cdecl CharacterSheenResourceCallback(RendererAuxiliaryResourceRecord* p_material, LegoU32 p_phase)
{
	if ((g_characterSheenInitialized & 1) == 0) {
		g_characterSheenInitialized |= 1;
	}
	if ((g_characterSheenInitialized & 2) == 0) {
		g_characterSheenInitialized |= 2;
		g_characterSheenTexturePosition.m_x = g_characterSheenTexturePosition.m_y =
			g_characterSheenTexturePosition.m_z = 0.0f;
	}
	if ((g_characterSheenInitialized & 4) == 0) {
		g_characterSheenInitialized |= 4;
		g_characterSheenTextureAngles.m_x = g_characterSheenTextureAngles.m_y = g_characterSheenTextureAngles.m_z =
			0.0f;
	}
	if (p_phase == 0) {
		void* binding = g_displayRenderer->m_unk0x4cc;
		((const RendererMainDispatch*) g_displayRenderer->m_vtable)
			->m_setTextureTransform(
				g_displayRenderer,
				NULL,
				1,
				&g_characterSheenTextureRotation,
				&g_characterSheenTexturePosition
			);
		for (LegoS32 i = 0; i < g_characterVisualBindingAssociationCount; ++i) {
			if (g_characterVisualBindingAssociations[i].m_binding == binding) {
				p_material->m_modeFactor = g_characterVisualBindingAssociations[i].m_value;
				return 1;
			}
		}
	}
	else if (p_phase == 1) {
		g_characterSheenTextureAngles.m_x += 0.03f;
		g_characterSheenTextureAngles.m_y += 0.05f;
		g_characterSheenTextureAngles.m_z += 0.02f;
		g_characterSheenTextureRotation.FUN_004a4f60(&g_characterSheenTextureAngles.m_x);
	}
	return 1;
}

// STUB: LEGORACERS2 0x0045e930
static void __fastcall RegisterCharacterVisualBindingAssociation(
	GameController*,
	void*,
	void* p_binding,
	LegoFloat p_value
)
{
	static LegoBool32 initialized;
	if (!initialized) {
		InitializeCharacterVisualBindingAssociationStorage();
		initialized = TRUE;
	}
	LegoS32 count = g_characterVisualBindingAssociationCount;
	if (count != 0xbe) {
		g_characterVisualBindingAssociationCount = count + 1;
		g_characterVisualBindingAssociations[count].m_binding = p_binding;
		g_characterVisualBindingAssociations[count].m_value = p_value;
	}
}

// STUB: LEGORACERS2 0x0045e960
static void __fastcall UnregisterCharacterVisualBindingAssociation(GameController*, void*, void* p_binding)
{
	LegoS32 count = g_characterVisualBindingAssociationCount;
	LegoS32 index = 0;
	while (index < count) {
		if (g_characterVisualBindingAssociations[index].m_binding == p_binding) {
			g_characterVisualBindingAssociations[index] = g_characterVisualBindingAssociations[count - 1];
			count = --g_characterVisualBindingAssociationCount;
			--index;
		}
		++index;
	}
}

// FUNCTION: LEGORACERS2 0x0045e9b0
static void __fastcall SetCharacterVisualBindingAssociationValue(
	GameController*,
	void*,
	void* p_binding,
	LegoFloat p_value
)
{
	for (LegoS32 index = 0; index < g_characterVisualBindingAssociationCount; ++index) {
		if (g_characterVisualBindingAssociations[index].m_binding == p_binding) {
			g_characterVisualBindingAssociations[index].m_value = p_value;
		}
	}
}

// STUB: LEGORACERS2 0x0047b520
static void InitializeFoyerDefaultBodypartName()
{
	new (g_foyerDefaultBodypartNameStorage) CharBuffer24("noname");
}

static CharBuffer24* GetFoyerDefaultBodypartName()
{
	return (CharBuffer24*) g_foyerDefaultBodypartNameStorage;
}

// STUB: LEGORACERS2 0x0047b7b0
static void InitializeSparkyCharacterVisualTemplate()
{
	g_sparkyCharacterVisualTemplate.Initialize(0, 6, 0, 0, 0, GetFoyerDefaultBodypartName());
}

// STUB: LEGORACERS2 0x0047b970
static void InitializeRigelCharacterVisualTemplate()
{
	g_rigelCharacterVisualTemplate.Initialize(0, 6, 0, 0, 0, GetFoyerDefaultBodypartName());
}

// STUB: LEGORACERS2 0x0047bac0
static void InitializeRocketRacerCharacterVisualTemplate()
{
	g_rocketRacerCharacterVisualTemplate.Initialize(0, 6, 0, 0, 0, GetFoyerDefaultBodypartName());
}

// STUB: LEGORACERS2 0x0047be50
static void InitializeIntroCharacterVisualTemplate()
{
	g_introCharacterVisualTemplate.Initialize(0, 6, 0, 0, 0, GetFoyerDefaultBodypartName());
}

// STUB: LEGORACERS2 0x00493440
GameObject* __fastcall DestroyCharacterVisualHeadLifecycle(PhysicalLifecycleState* p_state, void*, LegoU32 p_flags)
{
	return DestroyCharacterVisualHead((GameObject*) ((LegoU8*) p_state - 0x58), NULL, p_flags);
}

// STUB: LEGORACERS2 0x004aa260
static void __fastcall ReleaseCharacterVisualModelBinding(
	GameObjectModelBinding* p_binding,
	void*,
	LegoU32 p_option,
	LegoU32 p_texture,
	LegoU32 p_generation
)
{
	p_binding->m_flags = ((p_option & 1) << 1) | (p_binding->m_flags & ~2);
	if (p_binding->m_model != NULL) {
		ConfigureModelShadow(
			(ModelResource*) p_binding->m_model,
			NULL,
			p_option,
			(const LegoChar*) p_texture,
			p_generation
		);
	}
}

// STUB: LEGORACERS2 0x004aa2a0
static void __fastcall UpdateCharacterVisualBindingTransform(
	GameObjectModelBinding* p_binding,
	void*,
	const LegoS32* p_values
)
{
	LegoU8* bytes = (LegoU8*) p_binding;
	*(LegoS32*) (bytes + 0xac) = p_values[0];
	*(LegoS32*) (bytes + 0xb0) = p_values[1];
	g_displayRenderer->UpdateTextureStageFlags((RendererDefaultSlot*) (bytes + 0xac), 0);
	if (p_binding->m_boundMinimumX == -1 && p_binding->m_boundMinimumY == -1) {
		p_binding->m_flags &= ~0x10;
	}
	else {
		p_binding->m_flags |= 0x10;
	}
}

// STUB: LEGORACERS2 0x004c2770
static void __fastcall SetCharacterVisualBodyPreviewAngle(GameObject* p_object, void*, LegoFloat p_angle)
{
	static const LegoFloat kPi = 3.1415927410125732f;
	static const LegoFloat kDegreesToRadians = 0.0055555556900799274f;
	LegoFloat cosine;
	__asm {
		fld kPi
		fmul p_angle
		fmul kDegreesToRadians
		fcos
		fstp cosine
	}
	*(LegoFloat*) ((LegoU8*) p_object + 0x1b8) = cosine;
}

// STUB: LEGORACERS2 0x004c75d0
static void __fastcall ConfigureCharacterShadowBounds(
	ModelShadowData* p_shadow,
	void*,
	ModelBounds* p_bounds,
	LegoFloat p_scaleX,
	LegoFloat p_scaleZ
)
{
	Vector3 minimum;
	Vector3 maximum;
	minimum.m_x = minimum.m_y = minimum.m_z = 2147483648.0f;
	maximum.m_x = maximum.m_y = maximum.m_z = -2147483648.0f;
	for (LegoU32 i = 0; i < 8; ++i) {
		const Vector3* point = &p_bounds->m_corners[i];
		if (point->m_x < minimum.m_x) {
			minimum.m_x = point->m_x;
		}
		if (point->m_y < minimum.m_y) {
			minimum.m_y = point->m_y;
		}
		if (point->m_z < minimum.m_z) {
			minimum.m_z = point->m_z;
		}
		if (point->m_x > maximum.m_x) {
			maximum.m_x = point->m_x;
		}
		if (point->m_y > maximum.m_y) {
			maximum.m_y = point->m_y;
		}
		if (point->m_z > maximum.m_z) {
			maximum.m_z = point->m_z;
		}
	}
	LegoFloat halfX = (maximum.m_x - minimum.m_x) * 0.5f;
	LegoFloat halfZ = (maximum.m_z - minimum.m_z) * 0.5f;
	Vector3 center;
	center.m_x = halfX + minimum.m_x;
	center.m_y = 0.0f;
	center.m_z = halfZ + minimum.m_z;
	memcpy(p_shadow->m_unk0x1c, &center, sizeof(center));
	p_shadow->m_scaleX = halfX * p_scaleX;
	p_shadow->m_scaleY = halfZ * p_scaleZ;
}
