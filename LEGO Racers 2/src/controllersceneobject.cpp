#include "controllersceneobject.h"

#include "audioservice.h"
#include "charbuffer128.h"
#include "foyerscenecirclepool.h"
#include "gamemanager.h"
#include "gameobjectdispatch.h"
#include "globaloverlaymanager.h"
#include "memoryheap.h"
#include "renderer.h"

#include <new>
#include <string.h>

// GLOBAL: LEGORACERS2 0x00572bb8
GameObjectTemplate g_controllerSceneObjectTemplate;

struct ControllerSceneAudioSourceDefinition {
	LegoChar m_name[24];
	LegoChar m_secondaryName[24];
	LegoChar m_path[128];
	LegoChar m_unk0xb0[24];
	LegoFloat m_gainA;
	LegoFloat m_gainB;
	LegoFloat m_maximumGain;
	LegoU32 m_musicFlag;
	LegoU32 m_spatialFlag;
	LegoU32 m_distanceFlag;
	LegoU32 m_lowFlags;
	LegoFloat m_falloff;
	LegoFloat m_range;
	LegoU32 m_unk0xec;
	LegoU32 m_unk0xf0;
	LegoU32 m_unk0xf4;
	LegoU32 m_unk0xf8;
	LegoU32 m_audioValues[6];
	LegoU32 m_disableFalloff;
};
DECOMP_SIZE_ASSERT(ControllerSceneAudioSourceDefinition, 0x118);
DECOMP_STATIC_ASSERT(offsetof(ControllerSceneAudioSourceDefinition, m_spatialFlag) == 0xd8);
DECOMP_STATIC_ASSERT(offsetof(ControllerSceneAudioSourceDefinition, m_unk0xf0) == 0xf0);

// GLOBAL: LEGORACERS2 0x00572bf0
ControllerSceneAudioSourceDefinition g_controllerSceneSoundTemplate;

// GLOBAL: LEGORACERS2 0x00572bec
LegoFloat g_controllerSceneAngularFrequency;

// GLOBAL: LEGORACERS2 0x00572d08
ControllerSceneObject* g_controllerSceneObject;

GameObject* __fastcall DestroyControllerSceneObject(GameObject*, void*, LegoU32);

namespace
{

const LegoFloat kControllerScenePi = 3.1415927410125732f;
const LegoFloat kControllerSceneQuarter = 0.25f;
const LegoFloat kControllerScenePulseAmplitude = 0.05f;
const LegoFloat kControllerScenePulseStep = 0.3333333432674408f;
const LegoFloat kControllerSceneZero = 0.0f;
const LegoFloat kControllerSceneOne = 1.0f;

void __fastcall SetControllerSceneSpriteOffset(GlobalOverlaySprite* p_sprite, void*, const LegoFloat* p_position)
{
	p_sprite->SetPosition(p_position);
}

LegoFloat AdvanceControllerScenePulse(ControllerSceneObject* p_object, LegoFloat p_elapsedSeconds)
{
	LegoFloat sineValue;
	LegoFloat* elapsed = &p_object->m_elapsed;
	LegoFloat* pulse = &p_object->m_pulse;
	__asm {
		mov ecx, elapsed
		mov edx, pulse
		fld p_elapsedSeconds
		fadd dword ptr [ecx]
		fst dword ptr [ecx]
		fmul dword ptr [kControllerScenePi]
		fmul dword ptr [kControllerSceneQuarter]
		fsin
		fmul dword ptr [kControllerScenePulseAmplitude]
		fstp dword ptr [sineValue]

		mov eax, dword ptr [sineValue]
		and eax, 07fffffffh
		mov dword ptr [sineValue], eax
		fld dword ptr [sineValue]
		fadd dword ptr [edx]
		fcom dword ptr [kControllerSceneZero]
		fst dword ptr [edx]
		fnstsw ax
		test ah, 1
		jne pulse_below_zero
		fcom dword ptr [kControllerSceneOne]
		fnstsw ax
		test ah, 041h
		jne pulse_discard
		fsub dword ptr [kControllerSceneOne]
		fstp dword ptr [edx]
		jmp pulse_complete
	pulse_discard:
		fstp st(0)
		jmp pulse_complete
	pulse_below_zero:
		fadd dword ptr [kControllerSceneOne]
		fstp dword ptr [edx]
	pulse_complete:
	}
	return p_object->m_pulse;
}
} // namespace

// Retail dispatch table at 0x005331f0 (20 slots).
const ControllerSceneDispatch g_controllerSceneObjectDispatch = {
	(GameObject * (__fastcall*) (GameObject*, void*, LegoU32)) & DestroyControllerSceneObject,
	(void(__fastcall*)(GameObject*, void*, LegoU32, LegoU32)) & GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&InitializeControllerSceneObject,
	(LegoU32(__fastcall*)(GameObject*, void*)) & GameObjectBaseTrue,
	(LegoU32(__fastcall*)(GameObject*, void*, LegoFloat)) & UpdateControllerSceneObject,
	&RemoveControllerSceneObject,
	(LegoU32(__fastcall*)(GameObject*, void*)) & GameObjectBaseZero,
	(LegoU32(__fastcall*)(GameObject*, void*)) & GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	(LegoU32(__fastcall*)(GameObject*, void*)) & GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	(LegoU32(__fastcall*)(GameObject*, void*)) & GameObjectBaseZero,
	(LegoU32(__fastcall*)(GameObject*, void*, LegoU32, LegoU32)) & GameObjectBaseZeroTwoArguments,
	(void(__fastcall*)(GameObject*, void*, LegoU32)) & GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument,
};

// STUB: LEGORACERS2 0x0047e140
void __cdecl InitializeControllerSceneAngularFrequency()
{
	g_controllerSceneAngularFrequency = kControllerScenePi * 2.0f;
}

// STUB: LEGORACERS2 0x0047e160
void __cdecl InitializeControllerSceneSoundTemplate()
{
	CharBuffer24 name("noname");
	CharBuffer24 secondaryName("FFSND");
	CharBuffer128 path("game data\\sounds\\common\\looped\\fastfwrd.vag");
	CharBuffer24 empty("Empty");
	ControllerSceneAudioSourceDefinition* definition = &g_controllerSceneSoundTemplate;
	memset(definition, 0, sizeof(*definition));
	memcpy(definition->m_name, name.m_data, sizeof(definition->m_name));
	memcpy(definition->m_secondaryName, secondaryName.m_data, sizeof(definition->m_secondaryName));
	memcpy(definition->m_path, path.m_data, sizeof(definition->m_path));
	memcpy(definition->m_unk0xb0, empty.m_data, sizeof(definition->m_unk0xb0));
	definition->m_gainA = 1.0f;
	definition->m_gainB = 1.0f;
	definition->m_maximumGain = 1.0f;
	definition->m_spatialFlag = 1;
	definition->m_distanceFlag = 1;
	definition->m_lowFlags = 5;
	definition->m_falloff = 30.0f;
	definition->m_range = 150.0f;
}

// STUB: LEGORACERS2 0x0047e290
void __cdecl InitializeControllerSceneObjectTemplate()
{
	GameObjectTemplate* objectTemplate = &g_controllerSceneObjectTemplate;
	objectTemplate->m_flags = 2;
	objectTemplate->m_first = CharBuffer24("NoName");
	objectTemplate->m_second = CharBuffer24("");
}

struct ControllerSceneTemplateStartup {
	ControllerSceneTemplateStartup()
	{
		InitializeControllerSceneAngularFrequency();
		InitializeControllerSceneSoundTemplate();
		InitializeControllerSceneObjectTemplate();
	}
};
static ControllerSceneTemplateStartup g_controllerSceneTemplateStartup;

// STUB: LEGORACERS2 0x0047e2f0
ControllerSceneObject::ControllerSceneObject() : GameObject(0x900, &g_controllerSceneObjectTemplate, NULL)
{
	m_active = 0;
	m_elapsed = 0.0f;
	m_pulse = 0.0f;
	m_vtable = (const GameObjectDispatchView*) &g_controllerSceneObjectDispatch;
}

void __fastcall DestructControllerSceneObject(GameObject*, void*);

// FUNCTION: LEGORACERS2 0x0047e330
GameObject* __fastcall DestroyControllerSceneObject(GameObject* p_object, void*, LegoU32 p_flags)
{
	DestructControllerSceneObject(p_object, NULL);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0047e350
void __fastcall DestructControllerSceneObject(GameObject* p_object, void*)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_controllerSceneObjectDispatch;
	p_object->GameObject::~GameObject();
}

// STUB: LEGORACERS2 0x0047e360
GameObject* __cdecl GetControllerSceneObject()
{
	if (g_controllerSceneObject == NULL) {
		void* memory = AllocateHeapMemory(sizeof(ControllerSceneObject));
		g_controllerSceneObject = memory != NULL ? new (memory) ControllerSceneObject() : NULL;
		g_controllerSceneObject->m_vtable->m_initialize(g_controllerSceneObject, NULL);
	}
	return g_controllerSceneObject;
}

// STUB: LEGORACERS2 0x0047e3b0
void __cdecl ResetControllerInputState()
{
	ControllerSceneObject* object = g_controllerSceneObject;
	if (object != NULL) {
		object->m_vtable->m_remove(object, NULL);
		object = g_controllerSceneObject;
		if (object != NULL) {
			object->m_vtable->m_destroy(object, NULL, 1);
		}
	}
	g_controllerSceneObject = NULL;
}

// STUB: LEGORACERS2 0x0047e3e0
void __fastcall InitializeControllerSceneObject(GameObject* p_object, void*)
{
	ControllerSceneObject* object = (ControllerSceneObject*) p_object;
	if ((object->m_flags & 0x00800000) == 0) {
		LegoU32 spriteParameters[21];
		spriteParameters[0] = 0;
		spriteParameters[1] = 0;
		spriteParameters[2] = 0x3f800000;
		spriteParameters[3] = 0x3dcccccd;
		spriteParameters[4] = 0;
		spriteParameters[5] = 0;
		spriteParameters[6] = 0x3f800000;
		spriteParameters[7] = 0x3f800000;
		// Retail leaves the tile dimensions (words 8 and 9) uninitialized.
		spriteParameters[10] = 0x20ffffff;
		spriteParameters[11] = 0xffffffff;
		spriteParameters[12] = 0;
		spriteParameters[13] = 0x3f800000;
		spriteParameters[14] = 0x3f800000;
		spriteParameters[15] = 0;
		spriteParameters[16] = 0;
		spriteParameters[17] = 1;
		spriteParameters[18] = 0;
		spriteParameters[19] = 0;
		spriteParameters[20] = 0;
		for (LegoU32 i = 0; i < 3; ++i) {
			object->m_unk0x6c[i] = g_globalOverlayManager.AllocateSprite(spriteParameters);
			((GlobalOverlaySprite*) object->m_unk0x6c[i])->SetVisible(0);
			((GlobalOverlaySprite*) object->m_unk0x6c[i])->SetList(0);
		}
		AudioService* audio = GetAudioService(TRUE);
		g_controllerSceneSoundTemplate.m_spatialFlag = 0;
		g_controllerSceneSoundTemplate.m_unk0xf0 = 1;
		audio->m_vtable
			->m_addSource(audio, NULL, (const LegoU32*) &g_controllerSceneSoundTemplate, (LegoU32) object, 0);
		CharBuffer24 sourceName("FFSND");
		typedef AudioSourceRecord*(__fastcall * FindSceneSound)(AudioService*, void*, GameObject*, const LegoChar*);
		object->m_sound = ((FindSceneSound) audio->m_vtable->m_findSource)(audio, NULL, object, sourceName.m_data);
		((AudioSourceRecord*) object->m_sound)
			->m_vtable->m_setPlaybackGains((AudioSourceRecord*) object->m_sound, NULL, 0.25f, 0.25f);

		object->m_secondaryObject = g_foyerSceneCirclePool.AllocateRecord();
		LegoU8* record = (LegoU8*) object->m_secondaryObject;
		*(LegoU32*) (record + 0x1c) = 0xffffffff;
		record[0x1f] = 0xff;
		*(LegoU32*) (record + 0x40) = 0;
		*(LegoFloat*) (record + 0x38) = 1.0f;
		*(LegoFloat*) (record + 0x3c) = 1.0f;
		*(LegoU32*) (record + 0x48) = 0;
		object->m_texture =
			g_displayRenderer->CreateTextureResource("game data\\common\\textures\\speech bubble.ifl", 0, -1, 1, 1);
		LegoU32* textureState = (LegoU32*) (record + 0x20);
		textureState[0] = object->m_texture;
		textureState[1] = 0;
		textureState[2] = 0x3f800000;
		textureState[3] = 0x3f800000;
		textureState[4] = 0;
	}
	object->GameObject::Initialize();
}

// STUB: LEGORACERS2 0x0047e620
void __fastcall RemoveControllerSceneObject(GameObject* p_object, void*)
{
	ControllerSceneObject* object = (ControllerSceneObject*) p_object;
	object->GameObject::Remove();
	for (LegoU32 i = 0; i < 3; ++i) {
		((GlobalOverlaySprite*) object->m_unk0x6c[i])->Release();
	}
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_removeSource(audio, NULL, (LegoU32) object);
	g_displayRenderer->ReleaseTextureResourceReference(object->m_texture, 1);
	g_foyerSceneCirclePool.FreeRecord(object->m_secondaryObject);
}

// STUB: LEGORACERS2 0x0047e680
LegoU32 __fastcall UpdateControllerSceneObject(GameObject* p_object, void*, LegoFloat p_elapsedSeconds)
{
	ControllerSceneObject* object = (ControllerSceneObject*) p_object;
	if (object->m_active == 0) {
		return 1;
	}
	volatile LegoFloat pulse = AdvanceControllerScenePulse(object, p_elapsedSeconds);
	for (LegoU32 i = 0; i < 3; ++i) {
		LegoFloat offset[2] = {0.0f, pulse};
		SetControllerSceneSpriteOffset((GlobalOverlaySprite*) object->m_unk0x6c[i], NULL, offset);
		pulse = pulse + kControllerScenePulseStep;
		if (pulse > kControllerSceneOne) {
			pulse -= kControllerSceneOne;
		}
	}
	return 1;
}
