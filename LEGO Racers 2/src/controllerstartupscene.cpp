#include "bitmapfontset.h"
#include "foyermanager.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollerinitialization.h"
#include "gamecontrollersession.h"
#include "gameobjectdispatch.h"
#include "globaloverlaymanager.h"
#include "memoryheap.h"
#include "secondarymanager.h"
#include "secondaryrenderowner.h"
#include "systemclock.h"

#include <new>

void __fastcall BeginMenuTransition(GameObject*, void*, LegoU32);

struct ControllerStartupScene : GameObject {
	LegoU32 m_state;
	LegoS32 m_demoIndex;
	LegoFloat m_countdown;
	GlobalOverlayRecord* m_notice;
	LegoU32 m_unk0x68;
	ControllerStartupScene();
	~ControllerStartupScene();
};
DECOMP_SIZE_ASSERT(ControllerStartupScene, 0x6c);
static GameObject* __fastcall DestroyControllerStartupScene(GameObject*, void*, LegoU32);
static LegoU32 __fastcall UpdateControllerStartupScene(GameObject*, void*, LegoU32);
static LegoU32 __fastcall CountDownControllerAttractMode(ControllerStartupScene*, void*, LegoFloat);
static LegoU32 __fastcall UpdateControllerAttractNotice(ControllerStartupScene*, void*, LegoU32);
LegoBool32 __stdcall IsMenuPlayerAvailable(LegoU32);
static LegoS32 __fastcall AdvanceControllerAttractMode(ControllerStartupScene*, void*);
// Retail52f4c0, all20objectslots; thereisnoallocation-sizecallback.
extern const GameObjectDispatchView g_controllerStartupSceneDispatch = {
	&DestroyControllerStartupScene,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&FoyerManagerInitialize,
	&GameObjectBaseTrue,
	&UpdateControllerStartupScene,
	&ReleaseSecondaryRenderOwner,
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
	&GameObjectBaseNoOpOneArgument
};
// GLOBAL: LEGORACERS2 0x0053e390
static const LegoS32 g_controllerAttractScenes[5][2] = {{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}};
// GLOBAL: LEGORACERS2 0x00554280
static GameObjectTemplate g_controllerStartupSceneTemplate;

// STUB: LEGORACERS2 0x00406a50
static void InitializeControllerStartupSceneTemplate()
{
	g_controllerStartupSceneTemplate.m_flags = 10;
	g_controllerStartupSceneTemplate.m_first = CharBuffer24("NoName");
	g_controllerStartupSceneTemplate.m_second = CharBuffer24("");
}
struct ControllerStartupSceneTemplateInitializer {
	ControllerStartupSceneTemplateInitializer() { InitializeControllerStartupSceneTemplate(); }
};
static ControllerStartupSceneTemplateInitializer g_controllerStartupSceneTemplateInitializer;

// STUB: LEGORACERS2 0x00406ab0
ControllerStartupScene::ControllerStartupScene() : GameObject(0x900, &g_controllerStartupSceneTemplate, NULL)
{
	m_state = 0;
	m_notice = NULL;
	m_vtable = &g_controllerStartupSceneDispatch;
	SystemClock* clock = GetSystemClock();
	m_demoIndex = (LegoS32) clock->m_vtable->m_readMilliseconds() % 5;
}
GameObject* __fastcall ConstructControllerStartupScene(void* p_storage, void*)
{
	return new (p_storage) ControllerStartupScene;
}
// STUB: LEGORACERS2 0x00406b00
static GameObject* __fastcall DestroyControllerStartupScene(GameObject* p_object, void*, LegoU32 p_flags)
{
	((ControllerStartupScene*) p_object)->~ControllerStartupScene();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}
// STUB: LEGORACERS2 0x00406b20
ControllerStartupScene::~ControllerStartupScene()
{
	m_vtable = &g_controllerStartupSceneDispatch;
}
// STUB: LEGORACERS2 0x00406b30
static LegoU32 __fastcall UpdateControllerStartupScene(GameObject* p_object, void*, LegoU32 p_frame)
{
	ControllerStartupScene* scene = (ControllerStartupScene*) p_object;
	union {
		LegoU32 m_word;
		LegoFloat m_float;
	} elapsed;
	elapsed.m_word = p_frame;
	switch (scene->m_state) {
	case 1:
		return CountDownControllerAttractMode(scene, NULL, elapsed.m_float);
	case 2:
		return UpdateControllerAttractNotice(scene, NULL, p_frame);
	case 3:
		return IsMenuPlayerAvailable(p_frame);
	}
	return 0;
}
// STUB: LEGORACERS2 0x00406b80
static LegoU32 __fastcall CountDownControllerAttractMode(ControllerStartupScene* p_scene, void*, LegoFloat p_delta)
{
	p_scene->m_countdown -= p_delta;
	if (!(p_scene->m_countdown >= 0.0f)) {
		AdvanceControllerAttractMode(p_scene, NULL);
	}
	return 1;
}
// STUB: LEGORACERS2 0x00406bb0
static LegoU32 __fastcall UpdateControllerAttractNotice(ControllerStartupScene* p_scene, void*, LegoU32)
{
	if (g_gameController.m_state != 4) {
		return 0;
	}
	LegoBool32 raceOver = *(LegoU32*) ((LegoU8*) g_gameController.m_raceManager + 0x58) == 3;
	if (p_scene->m_notice == NULL) {
		if (raceOver) {
			LegoU32 parameters[13] = {
				(LegoU32) g_gameController.m_fontSet->m_fonts[1],
				0xffffffff,
				0x3f000000,
				0x3f4ccccd,
				0,
				0,
				0x3f800000,
				0x3f800000,
				0,
				9,
				0x3f000000,
				0,
				0
			};
			p_scene->m_notice = g_globalOverlayManager.AllocateRecord(parameters, -1);
			p_scene->m_notice->InitializeTextByIndex(0x2b, 9, 0.0f);
			p_scene->m_notice->m_flags |= 2;
		}
	}
	else if (!raceOver) {
		p_scene->m_notice->m_flags &= ~2u;
	}
	return 1;
}
// STUB: LEGORACERS2 0x00406cd0
LegoBool32 __stdcall IsMenuPlayerAvailable(LegoU32)
{
	return TRUE;
}
// STUB: LEGORACERS2 0x00406ce0
static LegoS32 __fastcall AdvanceControllerAttractMode(ControllerStartupScene* p_scene, void*)
{
	GameControllerSession* session = GetGameControllerSession();
	session->m_selectedMode = g_controllerAttractScenes[p_scene->m_demoIndex][0];
	session->m_track = g_controllerAttractScenes[p_scene->m_demoIndex][1];
	session->m_mode = 8;
	session->m_unk0x00 = 0;
	((SecondaryManager*) g_gameController.m_secondaryManager)->ReturnToController(1);
	BeginMenuTransition(g_gameController.m_menuManager, NULL, 1);
	LegoS32 next = p_scene->m_demoIndex + 1;
	p_scene->m_state = 2;
	p_scene->m_countdown = 0.0f;
	p_scene->m_demoIndex = next % 5;
	return next / 5;
}
// STUB: LEGORACERS2 0x00406d60
void __fastcall DisableSecondaryInactivityTimer(void* p_object, void*)
{
	ControllerStartupScene* scene = (ControllerStartupScene*) p_object;
	scene->m_countdown = 45.0f;
	if (scene->m_notice != NULL) {
		scene->m_notice->Release();
		scene->m_notice = NULL;
	}
	scene->m_state = 0;
}
