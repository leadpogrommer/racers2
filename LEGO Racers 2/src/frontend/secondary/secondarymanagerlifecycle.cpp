#include "frontend/preview/secondarycarselector.h"
#include "frontend/preview/secondarycharacterselector.h"
#include "frontend/secondary/secondarybrickresources.h"
#include "frontend/secondary/secondarydeferred.h"
#include "frontend/secondary/secondarymanager.h"
#include "frontend/secondary/secondarymenuactions.h"
#include "frontend/secondary/secondarymenuinteraction.h"
#include "frontend/secondary/secondarysavedgame.h"
#include "game/modes/controllerdeferred.h"
#include "game/objects/gameobjectdispatch.h"
#include "memory/memoryheap.h"

// GLOBAL: LEGORACERS2 0x0056b950
static void* g_secondaryLanguageRecord;

// These object-specific releases are called by retail with the object in ECX.
// Their implementations are recovered in their owning resource units.
void __fastcall ReleaseSecondaryObject88(void*, void*);
void __fastcall ReleaseSecondaryObject84(void*, void*);
void __fastcall ReleaseSecondaryObject80(void*, void*);
void __fastcall ReleaseSecondaryObject6c(void*, void*);
void __fastcall ReleaseSecondaryObject70(void*, void*);

#include "audio/reverb/reverbmanager.h"
#include "audio/service/audioservice.h"
#include "core/application/application.h"
#include "core/application/commandline.h"
#include "core/debug/diagnostics.h"
#include "frontend/dialogue/npctextmanager.h"
#include "frontend/foyer/foyerselectionquery.h"
#include "frontend/menu/menubackground.h"
#include "frontend/menu/menumanager.h"
#include "frontend/menu/menumanagercurrentaction.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/preview/characterselectionposition.h"
#include "frontend/resources/controllereventrecords.h"
#include "frontend/resources/controllerloadingprogress.h"
#include "frontend/widgets/menupanel.h"
#include "frontend/widgets/menupanelset.h"
#include "frontend/widgets/menuwidget.h"
#include "game/character/bodypartresources.h"
#include "game/character/charactervisual.h"
#include "game/controller/gamecontroller.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/controller/gamecontrollerinitialization.h"
#include "game/objects/gameobjectrenderprep.h"
#include "game/pause/globalpause.h"
#include "game/runtime/gameframe.h"
#include "game/session/gamecontrollerplayers.h"
#include "game/session/gamecontrollersession.h"
#include "game/session/gamecontrollersessionqueries.h"
#include "game/state/gamepreferences.h"
#include "io/files/filemanager.h"
#include "io/files/userdatapath.h"
#include "io/localization/localizedstringtable.h"
#include "physics/dynamics/physicallifecycle.h"
#include "render/buffers/rendererbufferdispatch.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/device/displayconfig.h"
#include "render/fonts/bitmapfontset.h"
#include "render/models/modelbounds.h"
#include "render/models/modelresource.h"
#include "render/models/modelresourcecache.h"
#include "render/models/modelresourcechunks.h"
#include "render/models/modelresourceloading.h"
#include "render/models/modelshadow.h"
#include "scene/animation/foyerskeleton.h"
#include "scene/lighting/gamelight.h"
#include "video/movieplayer.h"

#include <math.h>
#include <new.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SecondaryContentArchiveManager;
struct SecondaryContentArchiveDispatch {
	typedef void(__fastcall* Action)(SecondaryContentArchiveManager*, void*);
	typedef void(__fastcall* OneArgument)(SecondaryContentArchiveManager*, void*, void*);
	typedef void(__fastcall* TwoArguments)(SecondaryContentArchiveManager*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* ThreeArguments)(SecondaryContentArchiveManager*, void*, LegoU32, LegoU32, LegoU32);
	SecondaryContentArchiveManager*(__fastcall* m_destroy)(SecondaryContentArchiveManager*, void*, LegoU32);
	OneArgument m_action0x04;
	void*(__fastcall* m_create)(
		SecondaryContentArchiveManager*,
		void*,
		const LegoChar*,
		const LegoChar*,
		const LegoChar*,
		LegoU32,
		LegoU32,
		const LegoChar*
	);
	OneArgument m_release;
	LegoU32(__fastcall* m_query)(SecondaryContentArchiveManager*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
	TwoArguments m_action0x14;
	OneArgument m_action0x18;
	ThreeArguments m_action0x1c;
	ThreeArguments m_action0x20;
	ThreeArguments m_action0x24;
	OneArgument m_action0x28;
	OneArgument m_action0x2c;
	Action m_action0x30;
};
struct SecondaryContentArchiveManager {
	const SecondaryContentArchiveDispatch* m_vtable;
};
SecondaryContentArchiveManager* GetSecondaryContentArchiveManager();

void* __fastcall ConstructSecondaryCharacterSelection(void*, void*);
void* __fastcall ConstructSecondaryCarSelection(void*, void*);
void* __fastcall ConstructSecondaryAuxiliary(void*, void*);
void __fastcall InitializeSecondaryAuxiliary(void*, void*);
void* __fastcall ConstructSecondaryTextOverlays(void*, void*);
void __fastcall InitializeSecondaryModeState(void*, void*);
void __fastcall ResetSecondaryManagerModeStateAtEntry(void*, void*);
void __fastcall UpdateSecondaryPointer(void*, void*, const LegoU8*);
void __fastcall ResetSecondaryInactivityTimer(void*, void*);
void __fastcall EnterSecondaryManagerMode(GameObject*, void*, LegoU32);
void __fastcall FinishSecondaryCharacterSelection(void*, void*);
void __fastcall FinishSecondarySessionSelection(void*, void*);
void __fastcall SetMenuManagerSelectedInput(MenuManager*, void*, LegoU32);
void __fastcall PopMenuManagerAction(MenuManager*, void*);

void __fastcall ClearSecondaryManagerAuxiliaryResources(void*, void*, LegoU32, LegoU32, LegoU32);
void __fastcall ReleaseSecondaryAuxiliaryModel(void*, void*);

void __fastcall ResetSecondaryInactivityTimer(void* p_timer, void*);
void __fastcall PopMenuManagerAction(MenuManager* p_menu, void*);
void __fastcall SetMenuManagerSelectedInput(MenuManager* p_menu, void*, LegoU32 p_input);
void* __fastcall ConstructSecondaryAuxiliary(void* p_object, void*);
void __fastcall InitializeSecondaryAuxiliary(void* p_object, void*);
void __fastcall ReleaseSecondaryObject88(void* p_object, void*);
void* __fastcall ConstructSecondaryTextOverlays(void* p_object, void*);
void __fastcall UpdateSecondaryPointer(void* p_object, void*, const LegoU8* p_input);
static void* __fastcall ConstructSecondarySelectionGrid(void* p_object, void*, LegoU32 p_capacity);
static void __fastcall DestroySecondarySelectionGrid(void* p_object, void*);
void* __fastcall ConstructSecondaryCharacterSelection(void* p_object, void*);
void __fastcall ReleaseSecondaryObject70(void* p_object, void*);
void* __fastcall ConstructSecondaryCarSelection(void* p_object, void*);
void __fastcall ReleaseSecondaryObject6c(void* p_object, void*);
static SecondaryContentArchiveManager* __fastcall ConstructSecondaryContentArchiveManager(
	SecondaryContentArchiveManager* p_manager,
	void*
);
static SecondaryContentArchiveManager* __fastcall DestroySecondaryContentArchiveManager(
	SecondaryContentArchiveManager* p_manager,
	void*,
	LegoU32 p_flags
);
static void* __fastcall CreateSecondaryLanguageRecord(
	SecondaryContentArchiveManager*,
	void*,
	const LegoChar*,
	const LegoChar*,
	const LegoChar*,
	LegoU32,
	LegoU32,
	const LegoChar*
);
static LegoU32 __fastcall QuerySecondaryLanguageRecord(
	SecondaryContentArchiveManager*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
);
static void __fastcall SecondaryContentArchiveThreeArguments(
	SecondaryContentArchiveManager*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32
);
SecondaryContentArchiveManager* GetSecondaryContentArchiveManager();
static void OnSecondaryContentArchiveExit();

// GLOBAL: LEGORACERS2 0x0053eecc
static const LegoChar* g_secondarySaveDirectory = "Saved Games";
// GLOBAL: LEGORACERS2 0x0053eed0
static const LegoChar* g_secondarySaveFilename = "GameSave";

static LegoU32 __fastcall ComputeSecondarySaveChecksum(void*, void*, const LegoU8*, LegoS32);
LegoBool32 __fastcall LoadSecondarySessionData(void*, void*, const void*);

// GLOBAL: LEGORACERS2 0x00561b30
static LegoFloat g_secondarySaveMessagePosition[2];

struct SecondaryAmbientLightTemplate {
	GameObjectTemplate m_base;
	LegoU32 m_color;
};
struct SecondaryDirectionalLightTemplate {
	GameObjectTemplate m_base;
	D3DCOLORVALUE m_ambient;
	D3DCOLORVALUE m_diffuse;
	D3DCOLORVALUE m_specular;
	Vector3 m_direction;
};
struct SecondaryCarResourceTemplate {
	GameObjectTemplate m_base;
	CharBuffer128 m_allBricksPath;
	CharBuffer128 m_genericBricksPath;
	CharBuffer128 m_worldBricksPath;
	void** m_defaultCars;
	LegoS32 m_defaultCarCount;
	GameControllerItemBlock** m_savedCars;
	LegoS32 m_savedCarCount;
	LegoU32 m_loadMode;
	LegoU32 m_createDisplay;
	LegoFloat m_progressStart;
	LegoFloat m_progressEnd;
};
DECOMP_SIZE_ASSERT(SecondaryAmbientLightTemplate, 0x38);
DECOMP_SIZE_ASSERT(SecondaryDirectionalLightTemplate, 0x70);
DECOMP_SIZE_ASSERT(SecondaryCarResourceTemplate, 0x1d4);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarResourceTemplate, m_defaultCars) == 0x1b4);
static void __fastcall InitializeSecondaryDirectionalLight(GameObject*, void*);
static GameObject* __fastcall DestroySecondaryDirectionalLight(GameObject*, void*, LegoU32);
static void __fastcall InitializeSecondaryAmbientLight(GameObject*, void*);
static void __fastcall AllocateSecondaryLightProperties(GameLightProperties*, void*);
static void __fastcall ReleaseSecondaryLightProperties(GameLightProperties*, void*);
static LegoS32 __fastcall AllocateSecondaryRendererLight(DisplayRenderer*, void*);
static void __fastcall ReleaseSecondaryRendererLight(DisplayRenderer*, void*, LegoS32);
void __fastcall InitializeSecondaryCarResources(GameObject*, void*);
void __fastcall ReleaseSecondaryCarResources(GameObject*, void*);
extern const GameObjectDispatchView g_secondaryAmbientLightDispatch = {
	&GameObjectBaseDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&InitializeSecondaryAmbientLight,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&GameObjectBaseRemove,
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
extern const GameObjectDispatchView g_secondaryDirectionalLightDispatch = {
	&DestroySecondaryDirectionalLight,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&InitializeSecondaryDirectionalLight,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&GameObjectBaseRemove,
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
extern const GameObjectDispatchView g_secondaryCarResourceDispatch = {
	&GameObjectBaseDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&InitializeSecondaryCarResources,
	&GameObjectBaseZero,
	&GameObjectBaseZeroOneArgument,
	&ReleaseSecondaryCarResources,
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

static GameObject* ConstructSecondaryAmbientLight(SecondaryAmbientLightTemplate* p_template)
{
	void* storage = AllocateHeapMemory(0x90);
	if (storage == NULL) {
		return NULL;
	}
	GameObject* object = new (storage) GameObject(0x900, &p_template->m_base, NULL);
	memcpy((LegoU8*) object + 0x58, p_template, sizeof(*p_template));
	object->m_vtable = &g_secondaryAmbientLightDispatch;
	return object;
}
static GameObject* ConstructSecondaryDirectionalLight(SecondaryDirectionalLightTemplate* p_template)
{
	void* storage = AllocateHeapMemory(0xa0);
	if (storage == NULL) {
		return NULL;
	}
	GameObject* object = new (storage) GameObject(0x900, &p_template->m_base, NULL);
	GameDirectionalLight* light = (GameDirectionalLight*) object;
	light->m_properties.m_ambient = p_template->m_ambient;
	light->m_properties.m_diffuse = p_template->m_diffuse;
	light->m_properties.m_specular = p_template->m_specular;
	light->m_properties.m_enabled = 0;
	light->m_properties.m_lightIndex = -1;
	light->m_direction = p_template->m_direction;
	object->m_vtable = &g_secondaryDirectionalLightDispatch;
	light->m_direction.Normalize();
	return object;
}
static GameObject* ConstructSecondaryCarResources(SecondaryCarResourceTemplate* p_template)
{
	void* storage = AllocateHeapMemory(0x27a10);
	if (storage == NULL) {
		return NULL;
	}
	GameObject* object = new (storage) GameObject(0x900, &p_template->m_base, NULL);
	LegoU8* bytes = (LegoU8*) object;
	*(LegoFloat*) (bytes + 0x58) = 0.4f;
	*(LegoFloat*) (bytes + 0x5c) = 0.16f;
	memcpy(bytes + 0x60, &p_template->m_allBricksPath, 0x180);
	memcpy(bytes + 0x1e0, &p_template->m_defaultCars, 6 * 4);
	for (LegoU32 entry = 0; entry < 0x200; ++entry) {
		new (bytes + 0x200 + entry * 0x13c) CharBuffer128;
		for (LegoU32 item = 0; item < 4; ++item) {
			((GameControllerItem*) (bytes + 0x288 + entry * 0x13c + item * 0x28))->Initialize();
		}
	}
	*(LegoFloat*) (bytes + 0x279fc) = p_template->m_progressStart;
	*(LegoFloat*) (bytes + 0x27a00) = p_template->m_progressEnd;
	object->m_vtable = &g_secondaryCarResourceDispatch;
	return object;
}
// GLOBAL: LEGORACERS2 0x005538c8
static CharBuffer128 g_secondaryAllBrickPath("game data\\objects\\bricks\\UniqueBricks.txt");
// GLOBAL: LEGORACERS2 0x005537e8
CharBuffer128 g_secondaryGenericBrickPath("game data\\objects\\bricks\\UniqueBricksGeneric.txt");
// GLOBAL: LEGORACERS2 0x00553140
CharBuffer128 g_secondaryWorldBrickPaths[5] = {
	CharBuffer128("game data\\objects\\bricks\\UniqueBricksSandy.txt"),
	CharBuffer128("game data\\objects\\bricks\\UniqueBricksArctic.txt"),
	CharBuffer128("game data\\objects\\bricks\\UniqueBricksAdventure.txt"),
	CharBuffer128("game data\\objects\\bricks\\UniqueBricksMars.txt"),
	CharBuffer128("game data\\objects\\bricks\\UniqueBricksCarCraze.txt")
};
// GLOBAL: LEGORACERS2 0x00561b48
static CharBuffer24 g_secondaryDefaultBodypartName("noname");
// GLOBAL: LEGORACERS2 0x0067a418
static GameObjectTemplate g_secondaryLightBaseTemplate;
static void InitializeSecondaryLightTemplate();
static void InitializeSecondarySaveMessagePosition();
struct SecondaryLightTemplateStartup {
	SecondaryLightTemplateStartup()
	{
		InitializeSecondaryLightTemplate();
		InitializeSecondarySaveMessagePosition();
	}
};
static SecondaryLightTemplateStartup g_secondaryLightTemplateStartup;

void __fastcall ReleaseSecondaryAuxiliaryResources(void*, void*);
void __fastcall LoadSecondaryAuxiliaryResources(void*, void*);
void __fastcall InitializeSecondaryAuxiliaryModel(void*, void*);
void __fastcall ReleaseSecondaryModelResources(void*, void*);

void __fastcall LoadSecondaryAuxiliaryTextureRecords(void*, void*);
// The scratch storage is reserved through the next established pointer-tuning
// block. The message routine uses it without a length check, as retail does.
// GLOBAL: LEGORACERS2 0x00561760
static LegoU16 g_secondaryMessageScratch[0x130];
// GLOBAL: LEGORACERS2 0x005619c0
static LegoFloat g_secondaryPointerOrigin[2];
// GLOBAL: LEGORACERS2 0x005619c8
static LegoFloat g_secondaryPointerSize[2];
static void InitializeSecondaryPointerSize();
static void InitializeSecondaryPointerOrigin();
struct SecondaryPointerTuningStartup {
	SecondaryPointerTuningStartup()
	{
		InitializeSecondaryPointerSize();
		InitializeSecondaryPointerOrigin();
		InitializeMenuPointerOffsets();
	}
};
static SecondaryPointerTuningStartup g_secondaryPointerTuningStartup;

struct SecondaryInputHistoryEntry {
	LegoU32 m_control;
	LegoU32 m_axis;
	LegoFloat m_time;
};
struct SecondaryInputSequenceState {
	SecondaryInputHistoryEntry m_history[20];
	LegoS32 m_first;
	LegoS32 m_last;
	const LegoS32* m_sequences[16][2];
	LegoU32 m_flags;
};
DECOMP_SIZE_ASSERT(SecondaryInputSequenceState, 0x17c);
static void __fastcall AddSecondaryModeInput(SecondaryInputSequenceState*, void*, LegoU32, LegoU32);
static void __fastcall CheckSecondaryInputSequences(SecondaryInputSequenceState*, void*);
static void __fastcall ApplySecondaryInputSequence(SecondaryInputSequenceState*, void*, LegoU32);
static void __fastcall UnlockAllSecondaryWorlds(GameControllerSelection*, void*);
static void __fastcall UnlockAllSecondaryRewards(GameControllerSelection*, void*);
static void __fastcall UnlockAllSecondaryBonusFlags(LegoU32*, void*);
LegoBool32 __fastcall ApplySecondarySessionCheat(void*, void*, LegoU32);

static LegoU32 __stdcall GetSecondaryPortraitAtlasMask(LegoS32);
static void __fastcall LoadSecondaryNpcPortraits(void*, void*, LegoS32);
static void __fastcall ReleaseSecondaryNpcPortraits(void*, void*, LegoS32);
static const LegoS32* __stdcall GetSecondaryWorldSpeakers(LegoS32, LegoS32*);
static LegoU32 __stdcall GetSecondaryVoiceMask(LegoS32);

static void __fastcall SetSecondarySessionName(void*, void*, const GameControllerName*, LegoS32);
static void __fastcall SetSecondarySessionCharacter(void*, void*, const LegoU32*, LegoU32, LegoS32);
static void __fastcall SetSecondarySessionCar(void*, void*, const GameControllerItemBlock*, LegoS32, LegoS32);
void __fastcall SetFoyerSceneTransition(GameObject*, void*, LegoU32);
static LegoS32 __fastcall GetSecondaryNestedMenuAction(MenuManager*, void*);
void __fastcall PopSecondaryMenuActions(MenuManager*, void*, LegoU32, LegoS32);
void __fastcall BeginMenuTransition(GameObject*, void*, LegoU32);
void __fastcall ResetSecondaryMenuAction(MenuManager*, void*, LegoU32);

static void __fastcall MergeSecondarySelection(GameControllerSelection*, void*, const GameControllerSelection*);
static void __fastcall UnlockSecondaryTrackProgress(LegoU32*, void*);
static void __fastcall UnlockSecondaryBonusProgress(LegoU32*, void*);
static void __fastcall SetSecondaryTrackProgress(LegoU32*, void*, LegoU32, LegoU32);
static void __fastcall SetSecondaryBonusProgress(LegoU32*, void*, LegoU32, LegoU32);
static void __fastcall UnlockSecondaryModeProgress(LegoU32*, void*);
static void __fastcall SetSecondaryModeProgress(LegoU32*, void*, LegoU32);
static void __fastcall RefreshSecondaryProgressUnlocks(void*, void*);
void __fastcall RestoreSecondaryBonusObject(GameObject*, void*);
void __fastcall RefreshSecondaryModeWidgets(void*, void*);
LegoU32 __fastcall CountFoyerTrackAndBonusProgress(const LegoU32*, void*);
LegoU32 __fastcall CountFoyerTrackProgress(const LegoU32*, void*);
LegoU32 __fastcall CountFoyerModeProgress(const LegoU32*, void*);
LegoU32 __fastcall IsFoyerTrackProgressSet(const LegoU32*, void*, LegoU32, LegoU32);
LegoU32 __fastcall IsFoyerBonusProgressSet(const LegoU32*, void*, LegoU32, LegoU32);
LegoU32 __fastcall GetFoyerModeProgressValue(const LegoU32*, void*, LegoU32);
LegoU32 __fastcall IsFoyerModeProgressSet(const LegoU32*, void*, LegoU32);

static SecondaryCarCollisionStore* __fastcall ConstructSecondaryCarCollisionStore(
	SecondaryCarCollisionStore*,
	void*,
	GameObject*
);
static void __fastcall ReleaseSecondaryCarCollisionStore(SecondaryCarCollisionStore*, void*);
static bool ParseSecondaryBrickLine(LegoChar**, const LegoChar*, SecondaryBrickParameters*, LegoU32);
static void __fastcall AppendSecondaryBrickResource(SecondaryCarResourcePool*, void*, const SecondaryBrickParameters*);
static void __fastcall ReleaseSecondaryBrickModels(SecondaryCarResourcePool*, void*);
void __fastcall LoadSecondaryBrickModels(SecondaryCarResourcePool*, void*, LegoU32);

struct SecondaryVoiceBank {
	LegoS32 m_count;
	const LegoChar* m_namePrefix;
	const LegoChar* m_pathPrefix;
};
DECOMP_SIZE_ASSERT(SecondaryVoiceBank, 12);
// GLOBAL: LEGORACERS2 0x005436e0
static const SecondaryVoiceBank g_secondaryVoiceBanks[6] = {
	{7, "JibberBerg", "Berg\\berg"},
	{10, "JibberGenFemale", "Generic Females\\Genf3"},
	{8, "JibberGenMale", "Generic Men\\Genmn1"},
	{10, "JibberMartian", "Martians\\Alien"},
	{10, "JibberWarrior", "Warrior\\War"},
	{6, "JibberAngry", "Generic Men\\Angry"}
};
struct SecondaryVoiceSourceTemplate {
	CharBuffer24 m_ownerName;
	CharBuffer24 m_sourceName;
	CharBuffer128 m_path;
	CharBuffer24 m_empty;
	LegoFloat m_gain[3];
	LegoU32 m_flags[4];
	LegoFloat m_falloff;
	LegoFloat m_range;
	LegoU32 m_values[11];
};
DECOMP_SIZE_ASSERT(SecondaryVoiceSourceTemplate, 0x118);
// GLOBAL: LEGORACERS2 0x00543c14
static const LegoS32 g_secondaryWorldSpeakers543c14[27] = {0,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
														   21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 38, 60, 46};
// GLOBAL: LEGORACERS2 0x00543c84
static const LegoS32 g_secondaryWorldSpeakers543c84[9] = {0, 38, 39, 40, 41, 42, 43, 44, 45};
// GLOBAL: LEGORACERS2 0x00543cac
static const LegoS32 g_secondaryWorldSpeakers543cac[8] = {0, 31, 32, 33, 34, 35, 36, 37};
// GLOBAL: LEGORACERS2 0x00543cd0
static const LegoS32 g_secondaryWorldSpeakers543cd0[8] = {0, 46, 47, 48, 49, 50, 51, 52};
// GLOBAL: LEGORACERS2 0x00543cf4
static const LegoS32 g_secondaryWorldSpeakers543cf4[16] = {0, 54, 55, 56, 57, 58, 59, 60, 38, 46, 53, 8, 9, 10, 11, 31};
// GLOBAL: LEGORACERS2 0x00570c54
static LegoS32 g_secondaryDefaultSpeakers[1];

const LegoChar* __fastcall GetSecondaryBrickPath(SecondaryCarResourcePool*, void*, LegoS32, LegoS32*);
static LegoS32 __fastcall FindSecondaryBrickPath(SecondaryCarResourcePool*, void*, const LegoChar*);
static LegoU32 __fastcall IsSecondaryBrickLoaded(SecondaryCarResourcePool*, void*, LegoS32);
static LegoU32 __fastcall LoadSecondaryBrickMask(SecondaryCarResourcePool*, void*, const LegoChar*, LegoU32*, LegoS32);
static void __fastcall LoadSecondaryCarCollisionStore(SecondaryCarCollisionStore*, void*);
static void __stdcall CalculateSecondaryBrickBounds(SecondaryBrickResource*);
void __fastcall RotateSecondaryBrickModel(SecondaryCarResourcePool*, void*, SecondaryBrickResource*, LegoS32);
void __fastcall AlignSecondaryBrickModel(SecondaryCarResourcePool*, void*, SecondaryBrickResource*, LegoS32);
ModelResource* __fastcall CopySecondaryModelResource(ModelResource*, void*, const ModelResource*);
static LegoU32 __stdcall GetSecondaryPinVertexCount(ModelPrimitiveTextureRecord*);
static LegoBool32 __fastcall FindSecondaryPinAnchor(
	SecondaryCarResourcePool*,
	void*,
	ModelPrimitiveTextureRecord*,
	Vector3*
);
static LegoS32 RoundSecondaryGridValue(LegoFloat p_value)
{
	LegoS32 result;
	__asm { fld p_value
 fistp result }
	return result;
}

static void __fastcall CopySecondaryModelPrimitive(
	ModelPrimitiveTextureRecord*,
	void*,
	const ModelPrimitiveTextureRecord*
);
static ModelShadowData* __fastcall CopySecondaryModelShadow(ModelShadowData*, void*, const ModelShadowData*);
static void __fastcall AddSecondaryRendererTextureReference(DisplayRenderer*, void*, LegoS32);

// GLOBAL: LEGORACERS2 0x00553a50
static LegoU32 g_secondaryBrickLoadMasks[512];

// GLOBAL: LEGORACERS2 0x00553948
static LegoChar g_secondaryBrickCategoryScratch[0x100];
// GLOBAL: LEGORACERS2 0x0053e24c
static const LegoChar g_secondaryBrickCategoryNames[6][24] = {"CHASSIS", "SQUARE", "SLOPED", "WEIRD", "WINDOW", "TRIM"};
struct SecondaryCategoryWheel {
	GlobalOverlaySprite* m_icons[7];
	GlobalOverlaySprite* m_shadows[7];
	GlobalOverlaySprite* m_previousIcons[6];
	GlobalOverlaySprite* m_previousShadows[6];
	LegoFloat m_angles[6];
	LegoU32 m_resourcesReady;
	LegoU32 m_enabled;
	LegoS32 m_count;
	LegoS32 m_previousCount;
	LegoFloat m_currentAngle;
	LegoFloat m_targetAngle;
	LegoS32 m_mode;
	LegoS32 m_highlighted;
	LegoFloat m_highlightTime;
	LegoU32 m_transition;
	LegoFloat m_transitionTime;
};
struct SecondaryCategoryWheelTuning {
	LegoFloat m_position[2];
	LegoFloat m_headerSize[2];
	LegoFloat m_shadowOffset[2];
	LegoFloat m_firstBasis[2];
	LegoFloat m_centerOffset[2];
	LegoFloat m_fullAngle;
	LegoFloat m_phase;
	LegoFloat m_secondBasis[2];
	LegoFloat m_iconSize[2];
	LegoFloat m_angularSpeed;
};
DECOMP_SIZE_ASSERT(SecondaryCategoryWheel, 0xac);
DECOMP_SIZE_ASSERT(SecondaryCategoryWheelTuning, 0x44);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCategoryWheel, m_angles) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCategoryWheel, m_transitionTime) == 0xa8);
// GLOBAL: LEGORACERS2 0x0055e610
static SecondaryCategoryWheelTuning g_secondaryCategoryWheelTuning;
static const LegoFloat g_secondaryCategoryWheelPi = 3.1415927410125732f;
static void __stdcall ProjectSecondaryWheelIcon(LegoFloat, LegoFloat*, LegoFloat*);
static void __fastcall CreateSecondaryWheelSprites(SecondaryCategoryWheel*, void*, LegoU32);
static void __fastcall SnapshotSecondaryWheelIcons(SecondaryCategoryWheel*, void*, LegoS32);
static LegoS32 __fastcall CreateSecondaryWheelIcons(SecondaryCategoryWheel*, void*, LegoS32);
static void InitializeSecondaryWheelFullAngle();
static void InitializeSecondaryWheelPosition();
static void InitializeSecondaryWheelHeaderSize();
static void InitializeSecondaryWheelIconSize();
static void InitializeSecondaryWheelCenterOffset();
static void InitializeSecondaryWheelFirstBasis();
static void InitializeSecondaryWheelSecondBasis();
static void InitializeSecondaryWheelPhase();
static void InitializeSecondaryWheelAngularSpeed();
static void InitializeSecondaryWheelShadowOffset();
struct SecondaryCategoryWheelTuningStartup {
	SecondaryCategoryWheelTuningStartup()
	{
		InitializeSecondaryWheelFullAngle();
		InitializeSecondaryWheelPosition();
		InitializeSecondaryWheelHeaderSize();
		InitializeSecondaryWheelIconSize();
		InitializeSecondaryWheelCenterOffset();
		InitializeSecondaryWheelFirstBasis();
		InitializeSecondaryWheelSecondBasis();
		InitializeSecondaryWheelPhase();
		InitializeSecondaryWheelAngularSpeed();
		InitializeSecondaryWheelShadowOffset();
	}
};
static SecondaryCategoryWheelTuningStartup g_secondaryCategoryWheelTuningStartup;
static LegoU8 RoundSecondaryWheelAlpha(LegoFloat p_alpha)
{
	LegoS32 alpha;
	__asm {
		fld p_alpha
		fistp alpha
	}
	return (LegoU8) alpha;
}
static void RemoveAndDestroySecondaryObject(void* p_object)
{
	if (p_object != NULL) {
		GameObject* object = (GameObject*) p_object;
		object->m_vtable->m_remove(object, NULL);
		if (p_object != NULL) {
			object->m_vtable->m_destroy(object, NULL, 1);
		}
	}
}

void __fastcall SetSecondaryManagerAuxiliaryEnabled(void*, void*, LegoU32);

// The PC service has no removable-media backend. Its full retail table is at
// 0x0053648c; the construction/destruction table is at 0x005364c0.
extern "C" int __cdecl _purecall();
static SecondaryContentArchiveManager* __fastcall DestroySecondaryContentArchiveManager(
	SecondaryContentArchiveManager*,
	void*,
	LegoU32
);
static void* __fastcall CreateSecondaryLanguageRecord(
	SecondaryContentArchiveManager*,
	void*,
	const LegoChar*,
	const LegoChar*,
	const LegoChar*,
	LegoU32,
	LegoU32,
	const LegoChar*
);
static LegoU32 __fastcall QuerySecondaryLanguageRecord(
	SecondaryContentArchiveManager*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
);
static void __fastcall SecondaryContentArchiveThreeArguments(
	SecondaryContentArchiveManager*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32
);

extern const SecondaryContentArchiveDispatch g_secondaryContentArchiveBaseDispatch = {
	&DestroySecondaryContentArchiveManager,
	(SecondaryContentArchiveDispatch::OneArgument) &_purecall,
	(void*(__fastcall*) (SecondaryContentArchiveManager*,
						 void*,
						 const LegoChar*,
						 const LegoChar*,
						 const LegoChar*,
						 LegoU32,
						 LegoU32,
						 const LegoChar*) ) &
		_purecall,
	(SecondaryContentArchiveDispatch::OneArgument) &_purecall,
	(LegoU32(__fastcall*)(SecondaryContentArchiveManager*, void*, LegoU32, LegoU32, LegoU32, LegoU32)) & _purecall,
	(SecondaryContentArchiveDispatch::TwoArguments) &_purecall,
	(SecondaryContentArchiveDispatch::OneArgument) &_purecall,
	(SecondaryContentArchiveDispatch::ThreeArguments) &_purecall,
	(SecondaryContentArchiveDispatch::ThreeArguments) &_purecall,
	(SecondaryContentArchiveDispatch::ThreeArguments) &_purecall,
	(SecondaryContentArchiveDispatch::OneArgument) &_purecall,
	(SecondaryContentArchiveDispatch::OneArgument) &_purecall,
	(SecondaryContentArchiveDispatch::Action) &_purecall
};
extern const SecondaryContentArchiveDispatch g_secondaryContentArchiveDispatch = {
	&DestroySecondaryContentArchiveManager,
	(SecondaryContentArchiveDispatch::OneArgument) &GameObjectBaseNoOpOneArgument,
	&CreateSecondaryLanguageRecord,
	(SecondaryContentArchiveDispatch::OneArgument) &GameObjectBaseNoOpOneArgument,
	&QuerySecondaryLanguageRecord,
	(SecondaryContentArchiveDispatch::TwoArguments) &GameObjectBaseNoOpTwoArguments,
	(SecondaryContentArchiveDispatch::OneArgument) &GameObjectBaseNoOpOneArgument,
	&SecondaryContentArchiveThreeArguments,
	&SecondaryContentArchiveThreeArguments,
	&SecondaryContentArchiveThreeArguments,
	(SecondaryContentArchiveDispatch::OneArgument) &GameObjectBaseNoOpOneArgument,
	(SecondaryContentArchiveDispatch::OneArgument) &GameObjectBaseNoOpOneArgument,
	(SecondaryContentArchiveDispatch::Action) &GameObjectBaseNoOp
};
// GLOBAL: LEGORACERS2 0x0067a3e0
static SecondaryContentArchiveManager g_secondaryContentArchive;
// GLOBAL: LEGORACERS2 0x0067a3e4
static LegoU8 g_secondaryContentArchiveInitialized;

static void OnSecondaryContentArchiveExit();

// STUB: LEGORACERS2 0x004034e0
static SecondaryCarCollisionStore* __fastcall ConstructSecondaryCarCollisionStore(
	SecondaryCarCollisionStore* p_store,
	void*,
	GameObject* p_owner
)
{
	p_store->m_owner = p_owner;
	p_store->m_collisionData = GetMemoryHeap()->Allocate(140000, "Memory for storing car builder collision data ");
	return p_store;
}

// STUB: LEGORACERS2 0x00403510
static void __fastcall ReleaseSecondaryCarCollisionStore(SecondaryCarCollisionStore* p_store, void*)
{
	GetMemoryHeap()->Free(p_store->m_collisionData);
}

// STUB: LEGORACERS2 0x004035d0
static void __fastcall LoadSecondaryCarCollisionStore(SecondaryCarCollisionStore* p_store, void*)
{
	SecondaryCarResourcePool* pool = (SecondaryCarResourcePool*) p_store->m_owner;
	LegoU8* cursor = (LegoU8*) p_store->m_collisionData;
	p_store->m_count = pool->m_entryCount;
	for (LegoS32 index = 0; index < p_store->m_count; ++index) {
		if (IsSecondaryBrickLoaded(pool, NULL, index) == 0) {
			p_store->m_records[index] = NULL;
			p_store->m_payloads[index] = NULL;
		}
		else {
			p_store->m_records[index] = cursor;
			p_store->m_payloads[index] = cursor + 12;
			LegoS32 category;
			CharBuffer128 path(GetSecondaryBrickPath(pool, NULL, index, &category));
			strcpy(path.FindAfterDot(), "BCD");
			WinFile file(path.m_data, 0, NULL);
			LegoU32 bytes = file.GetManagedSize();
			file.Read(p_store->m_records[index], bytes, 1);
			file.CloseManaged();
			cursor += bytes;
		}
	}
}

// STUB: LEGORACERS2 0x00404300
void __fastcall InitializeSecondaryCarResources(GameObject* p_object, void*)
{
	SecondaryCarResourcePool* pool = (SecondaryCarResourcePool*) p_object;
	pool->Initialize();
	pool->m_entryCount = 0;
	WinFile file(pool->m_paths[0].m_data, 0, NULL);
	LegoU32 bytes = file.GetManagedSize();
	LegoChar* buffer = (LegoChar*) GetMemoryHeap()->Allocate(bytes + 64, "temp buffer for ubnames");
	file.Read(buffer, bytes, 1);
	file.CloseManaged();
	LegoChar* end = buffer + bytes - 1;
	*end = 0;
	LegoChar* cursor = buffer;
	SecondaryBrickParameters parameters;
	memset(parameters.m_path, 0, sizeof(parameters.m_path));
	bool finished;
	do {
		finished = ParseSecondaryBrickLine(&cursor, end, &parameters, 0);
		if (parameters.m_path[0] != 0) {
			AppendSecondaryBrickResource(pool, NULL, &parameters);
		}
	} while (!finished);
	GetMemoryHeap()->Free(buffer);
	if (pool->m_createCollisionStore != 0) {
		SecondaryCarCollisionStore* store = (SecondaryCarCollisionStore*) AllocateHeapMemory(0x100c);
		pool->m_collisionStore = store == NULL ? NULL : ConstructSecondaryCarCollisionStore(store, NULL, pool);
	}
	else {
		pool->m_collisionStore = NULL;
	}
	LoadSecondaryBrickModels(pool, NULL, pool->m_loadMode);
}

// STUB: LEGORACERS2 0x00404420
void __fastcall ReleaseSecondaryCarResources(GameObject* p_object, void*)
{
	SecondaryCarResourcePool* pool = (SecondaryCarResourcePool*) p_object;
	ReleaseSecondaryBrickModels(pool, NULL);
	if (pool->m_collisionStore != NULL) {
		ReleaseSecondaryCarCollisionStore(pool->m_collisionStore, NULL);
		FreeHeapMemory(pool->m_collisionStore);
	}
	pool->Remove();
}

// STUB: LEGORACERS2 0x00404450
static bool ParseSecondaryBrickLine(
	LegoChar** p_cursor,
	const LegoChar* p_end,
	SecondaryBrickParameters* p_result,
	LegoU32 p_skipAttributes
)
{
	if (**p_cursor == '#') {
		do {
			if (*p_cursor >= p_end) {
				break;
			}
			++*p_cursor;
		} while (**p_cursor != '\n');
		++*p_cursor;
		p_result->m_path[0] = 0;
		return *p_cursor >= p_end;
	}
	LegoChar* categoryStart = *p_cursor;
	LegoChar* previousSlash = categoryStart - 1;
	LegoU32 pathLength = 0;
	bool searching = true;
	do {
		LegoChar* current = *p_cursor;
		p_result->m_path[pathLength++] = *current;
		if (current[-3] == '.' && (current[-2] == 'm' || current[-2] == 'M') &&
			(current[-1] == 'd' || current[-1] == 'D') && current[0] == '2') {
			searching = false;
		}
		if (*current == '\\') {
			categoryStart = previousSlash + 1;
			previousSlash = current;
		}
		++*p_cursor;
		if (*p_cursor >= p_end || **p_cursor == '\n') {
			++*p_cursor;
			p_result->m_path[0] = 0;
			return *p_cursor >= p_end;
		}
	} while (searching);
	p_result->m_path[pathLength] = 0;
	if (p_skipAttributes != 0) {
		while (**p_cursor != '\n' && *p_cursor < p_end) {
			++*p_cursor;
		}
		++*p_cursor;
		return *p_cursor >= p_end;
	}
	LegoU32 categoryLength = 0;
	while (categoryStart < previousSlash) {
		g_secondaryBrickCategoryScratch[categoryLength++] = *categoryStart++;
	}
	g_secondaryBrickCategoryScratch[categoryLength] = 0;
	_strupr(g_secondaryBrickCategoryScratch);
	p_result->m_category = -1;
	for (LegoU32 category = 0; category < 6; ++category) {
		if (strcmp(g_secondaryBrickCategoryScratch, g_secondaryBrickCategoryNames[category]) == 0) {
			p_result->m_category = category;
			break;
		}
	}
	LegoS32 scaleMode = 0;
	LegoS32 offsetMode = 0;
	LegoBool32 orientationMode = FALSE;
	LegoFloat scale = 1.0f;
	LegoFloat scaleFraction = 1.0f;
	LegoFloat offset = 0.0f;
	LegoFloat offsetSign = 1.0f;
	LegoFloat offsetFraction = 1.0f;
	LegoS32 orientation = 0;
	while (**p_cursor != '\r') {
		LegoChar* current = *p_cursor;
		LegoChar character = *current;
		if (character == '\n' || current >= p_end) {
			break;
		}
		if (character == 'S' && current[1] == 'C' && current[2] == 'A') {
			scaleMode = 1;
			orientationMode = FALSE;
			offsetMode = 0;
			scale = 0.0f;
			scaleFraction = 1.0f;
		}
		if (character == 'O' && current[1] == 'F' && current[2] == 'F') {
			scaleMode = 0;
			orientationMode = FALSE;
			offsetMode = 1;
			offsetFraction = 1.0f;
		}
		else {
			if (character == 'O' && current[1] == 'R' && current[2] == 'I') {
				scaleMode = 0;
				orientationMode = TRUE;
				offsetMode = 0;
			}
			if (scaleMode != 0) {
				if (character >= '0' && character <= '9') {
					if (scaleMode == 1) {
						scale = scale * 10.0f + (character - '0');
					}
					else {
						scaleFraction *= 0.1f;
						scale = (character - '0') * scaleFraction + scale;
					}
				}
				else if (character == '.') {
					++scaleMode;
				}
			}
			if (orientationMode && character >= '0' && character <= '9') {
				orientation = orientation * 10 + character - '0';
			}
		}
		if (offsetMode != 0) {
			if (character >= '0' && character <= '9') {
				if (offsetMode == 1) {
					offset = offset * 10.0f + (character - '0');
				}
				else {
					offsetFraction *= 0.1f;
					offset = (character - '0') * offsetFraction + offset;
				}
			}
			else if (character == '.') {
				++offsetMode;
			}
			else if (character == '-') {
				offsetSign = -1.0f;
			}
		}
		++*p_cursor;
	}
	p_result->m_scale = scale;
	p_result->m_orientation = orientation;
	p_result->m_offset = offsetSign * offset * offsetSign;
	*p_cursor += 2;
	return *p_cursor >= p_end;
}

// STUB: LEGORACERS2 0x004047b0
static void __fastcall AppendSecondaryBrickResource(
	SecondaryCarResourcePool* p_pool,
	void*,
	const SecondaryBrickParameters* p_parameters
)
{
	SecondaryBrickResource* entry = &p_pool->m_entries[p_pool->m_entryCount];
	entry->m_loaded = 0;
	memcpy(&entry->m_path, p_parameters->m_path, 128);
	entry->m_category = p_parameters->m_category;
	entry->m_scale = p_parameters->m_scale;
	entry->m_orientation = p_parameters->m_orientation;
	entry->m_offset = p_parameters->m_offset;
	entry->m_worldMask = 0;
	++p_pool->m_entryCount;
}

// STUB: LEGORACERS2 0x00404830
const LegoChar* __fastcall GetSecondaryBrickPath(
	SecondaryCarResourcePool* p_pool,
	void*,
	LegoS32 p_index,
	LegoS32* p_category
)
{
	if (p_index >= p_pool->m_entryCount) {
		ReportFatalError("invalid name ID in uniqueblocknames");
	}
	*p_category = p_pool->m_entries[p_index].m_category;
	return p_pool->m_entries[p_index].m_path.m_data;
}

// STUB: LEGORACERS2 0x00404870
static LegoS32 __fastcall FindSecondaryBrickPath(SecondaryCarResourcePool* p_pool, void*, const LegoChar* p_path)
{
	for (LegoS32 index = 0; index < p_pool->m_entryCount; ++index) {
		if (strcmp(p_pool->m_entries[index].m_path.m_data, p_path) == 0) {
			return index;
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x004048e0
static LegoU32 __fastcall IsSecondaryBrickLoaded(SecondaryCarResourcePool* p_pool, void*, LegoS32 p_index)
{
	if (p_index >= p_pool->m_entryCount) {
		ReportFatalError("invalid name ID in uniqueblocknames");
	}
	return p_pool->m_entries[p_index].m_loaded;
}

// STUB: LEGORACERS2 0x00404920
LegoFloat __fastcall GetSecondaryBrickScale(SecondaryCarResourcePool* p_pool, void*, LegoS32 p_index)
{
	if (p_index >= p_pool->m_entryCount) {
		ReportFatalError("invalid name ID in uniqueblocknames");
	}
	return p_pool->m_entries[p_index].m_scale;
}

// STUB: LEGORACERS2 0x00404960
LegoS32 __fastcall GetSecondaryBrickOrientation(SecondaryCarResourcePool* p_pool, void*, LegoS32 p_index)
{
	if (p_index >= p_pool->m_entryCount) {
		ReportFatalError("invalid name ID in uniqueblocknames");
	}
	return p_pool->m_entries[p_index].m_orientation;
}

// STUB: LEGORACERS2 0x004049a0
LegoFloat __fastcall GetSecondaryBrickOffset(SecondaryCarResourcePool* p_pool, void*, LegoS32 p_index)
{
	if (p_index >= p_pool->m_entryCount) {
		ReportFatalError("invalid name ID in uniqueblocknames");
	}
	return p_pool->m_entries[p_index].m_offset;
}

// STUB: LEGORACERS2 0x004049e0
LegoU32 __fastcall GetSecondaryBrickWorldMask(SecondaryCarResourcePool* p_pool, void*, LegoS32 p_index)
{
	if (p_index >= p_pool->m_entryCount) {
		ReportFatalError("invalid name ID in uniqueblocknames");
	}
	return p_pool->m_entries[p_index].m_worldMask;
}

// STUB: LEGORACERS2 0x00404a40
void __fastcall LoadSecondaryBrickModels(SecondaryCarResourcePool* p_pool, void*, LegoU32 p_loadVariants)
{
	LegoFloat progressRange = p_pool->m_progressEnd - p_pool->m_progressStart;
	void* progress = NULL;
	if (p_pool->m_progressStart >= 0.0f && p_pool->m_progressEnd >= 0.0f) {
		progress = g_gameController.m_unk0xd4;
	}
	for (LegoS32 index = 0; index < p_pool->m_entryCount; ++index) {
		g_secondaryBrickLoadMasks[index] = 0;
	}
	LoadSecondaryBrickMask(p_pool, NULL, p_pool->m_paths[1].m_data, g_secondaryBrickLoadMasks, -1);
	LoadSecondaryBrickMask(p_pool, NULL, p_pool->m_paths[2].m_data, g_secondaryBrickLoadMasks, -1);
	for (LegoS32 car = 0; car < p_pool->m_defaultCarCount + p_pool->m_savedCarCount; ++car) {
		const LegoU32* configuration =
			(const LegoU32*) (car < p_pool->m_defaultCarCount ? p_pool->m_defaultCars[car]
															  : p_pool->m_savedCars[car - p_pool->m_defaultCarCount]);
		for (LegoS32 brick = 0; brick < (LegoS32) configuration[0]; ++brick) {
			const LegoU32* item = configuration + 12 + brick * 5;
			g_secondaryBrickLoadMasks[item[0]] |= 1;
			g_secondaryBrickLoadMasks[item[0]] |= 1 << (item[1] & 31);
		}
	}
	p_pool->m_remainingEntries = p_pool->m_entryCount;
	LegoFloat progressStep = 0.8f / p_pool->m_entryCount;
	for (LegoS32 brick = 0; brick < p_pool->m_entryCount; ++brick) {
		if (progress != NULL) {
			SetControllerSelectionProgressValue(
				progress,
				NULL,
				(brick + 1) * progressStep * progressRange + p_pool->m_progressStart
			);
			UpdateControllerSelectionProgress(progress, NULL, 0);
		}
		SecondaryBrickResource& entry = p_pool->m_entries[brick];
		LegoU32 mask = g_secondaryBrickLoadMasks[brick];
		if (mask == 0) {
			entry.m_loaded = 0;
			for (LegoS32 variant = 0; variant < 4; ++variant) {
				entry.m_models[variant].m_resource = NULL;
			}
		}
		else {
			entry.m_loaded = 1;
			LegoS32 category;
			const LegoChar* path = GetSecondaryBrickPath(p_pool, NULL, brick, &category);
			entry.m_models[0].m_resource = LoadModelResource(path, 1, 0, 1);
			entry.m_nameId = brick;
			CalculateSecondaryBrickBounds(&entry);
			ModelResource* base = entry.m_models[0].m_resource;
			for (LegoS32 variant = 1; variant < 4; ++variant) {
				SecondaryCarModel& model = entry.m_models[variant];
				if (p_loadVariants != 0 && (mask & (1 << variant)) != 0 && category != 0) {
					model.m_minimum = entry.m_models[0].m_minimum;
					model.m_maximum = entry.m_models[0].m_maximum;
					ModelResource* clone = (ModelResource*) AllocateHeapMemory(sizeof(ModelResource));
					if (clone != NULL) {
						clone->m_signature = 0x324c444d;
						clone->m_hasSpecialPrimitives = 0;
						clone->m_unk0x08 = 0;
						clone->m_groups = NULL;
						clone->m_specialGroup = NULL;
						clone->m_bounds = NULL;
						clone->m_skin = NULL;
						clone->m_unk0x1c = NULL;
						clone->m_groupCount = 0;
						clone->m_unk0x24 = 1;
						clone->m_unk0x28 = NULL;
						clone->m_resources.Reset();
						clone->m_collisionData = NULL;
						clone->m_collisionDataSize = 0;
					}
					model.m_resource = clone;
					CopySecondaryModelResource(clone, NULL, base);
					RotateSecondaryBrickModel(p_pool, NULL, &entry, variant);
					AlignSecondaryBrickModel(p_pool, NULL, &entry, variant);
				}
				else {
					model.m_resource = NULL;
				}
			}
			AlignSecondaryBrickModel(p_pool, NULL, &entry, 0);
		}
	}
	if (progress != NULL) {
		SetControllerSelectionProgressValue(progress, NULL, progressRange * 0.9f + p_pool->m_progressStart);
		UpdateControllerSelectionProgress(progress, NULL, 0);
	}
	if (p_pool->m_createCollisionStore != 0) {
		LoadSecondaryCarCollisionStore(p_pool->m_collisionStore, NULL);
	}
	if (progress != NULL) {
		SetControllerSelectionProgressValue(progress, NULL, progressRange + p_pool->m_progressStart);
		UpdateControllerSelectionProgress(progress, NULL, 0);
	}
}

// STUB: LEGORACERS2 0x00404e00
static void __fastcall ReleaseSecondaryBrickModels(SecondaryCarResourcePool* p_pool, void*)
{
	for (LegoS32 brick = p_pool->m_entryCount - 1; brick >= 0; --brick) {
		for (LegoS32 variant = 3; variant >= 0; --variant) {
			ModelResource* model = p_pool->m_entries[brick].m_models[variant].m_resource;
			if (model != NULL) {
				if (variant == 0) {
					g_modelResourceCache.Release(model);
				}
				else {
					model->~ModelResource();
					FreeHeapMemory(model);
				}
			}
		}
	}
}

// STUB: LEGORACERS2 0x00404e70
static LegoU32 __fastcall LoadSecondaryBrickMask(
	SecondaryCarResourcePool* p_pool,
	void*,
	const LegoChar* p_path,
	LegoU32* p_masks,
	LegoS32 p_world
)
{
	if (p_path[0] == 0) {
		return 0;
	}
	WinFile file(p_path, 0, NULL);
	LegoU32 bytes = file.GetManagedSize();
	LegoChar* buffer = (LegoChar*) GetMemoryHeap()->Allocate(bytes + 64, "temp buffer for Unique Blocks mask");
	file.Read(buffer, bytes, 1);
	file.CloseManaged();
	LegoChar* end = buffer + bytes - 1;
	*end = 0;
	LegoChar* cursor = buffer;
	SecondaryBrickParameters parameters;
	memset(parameters.m_path, 0, 128);
	bool finished;
	do {
		finished = ParseSecondaryBrickLine(&cursor, end, &parameters, 1);
		if (parameters.m_path[0] != 0) {
			LegoS32 index = FindSecondaryBrickPath(p_pool, NULL, parameters.m_path);
			if (index >= 0) {
				if (p_masks != NULL) {
					p_masks[index] |= 0xf;
				}
				if (p_world >= 0 && p_world < 5) {
					p_pool->m_entries[index].m_worldMask |= 1 << p_world;
				}
			}
		}
	} while (!finished);
	GetMemoryHeap()->Free(buffer);
	return 1;
}

// STUB: LEGORACERS2 0x00404f90
void __fastcall LoadSecondaryBuilderWorldMasks(
	SecondaryCarResourcePool* p_pool,
	void*,
	LegoS32 p_world,
	const CharBuffer128* p_generic,
	const CharBuffer128* p_worldPath,
	const CharBuffer128* p_extra
)
{
	LoadSecondaryBrickMask(p_pool, NULL, p_generic->m_data, NULL, p_world);
	LoadSecondaryBrickMask(p_pool, NULL, p_worldPath->m_data, NULL, p_world);
	LoadSecondaryBrickMask(p_pool, NULL, p_extra->m_data, NULL, p_world);
}

// STUB: LEGORACERS2 0x00404ff0
void __fastcall RotateSecondaryBrickModel(
	SecondaryCarResourcePool*,
	void*,
	SecondaryBrickResource* p_entry,
	LegoS32 p_variant
)
{
	SecondaryCarModel& target = p_entry->m_models[p_variant];
	ModelTextureGroup* group = &target.m_resource->m_groups[0];
	for (LegoS32 primitive = 0; primitive < group->m_primitiveCount; ++primitive) {
		RendererVertexBuffer* vertices = group->m_primitives[primitive].m_vertexBuffer;
		const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		LegoU32 count = dispatch->m_getCount(vertices, NULL);
		dispatch->m_lock(vertices, NULL, 0, count, 0, 0);
		dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		dispatch->m_setCursor(vertices, NULL, 0);
		count = dispatch->m_getCount(vertices, NULL);
		for (LegoS32 vertex = 0; vertex < (LegoS32) count; ++vertex) {
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			Vector3* position = dispatch->m_getCurrentPosition(vertices, NULL);
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			Vector3* normal = dispatch->m_getCurrentNormal(vertices, NULL);
			if (p_variant == 1) {
				LegoFloat value = position->m_z;
				position->m_z = -position->m_x;
				position->m_x = value;
				value = normal->m_z;
				normal->m_z = -normal->m_x;
				normal->m_x = value;
			}
			else if (p_variant == 2) {
				position->m_x = -position->m_x;
				position->m_z = -position->m_z;
				normal->m_x = -normal->m_x;
				normal->m_z = -normal->m_z;
			}
			else if (p_variant == 3) {
				LegoFloat value = position->m_x;
				position->m_x = -position->m_z;
				position->m_z = value;
				value = normal->m_z;
				normal->m_z = normal->m_x;
				normal->m_x = -value;
			}
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			dispatch->m_advanceCursor(vertices, NULL);
		}
		dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		dispatch->m_unlock(vertices, NULL, 1);
	}
	for (LegoS32 texturePrimitive = 0; texturePrimitive < group->m_primitiveCount; ++texturePrimitive) {
		ModelPrimitiveTextureRecord* record = &group->m_primitives[texturePrimitive];
		if (GetSecondaryPinVertexCount(record) != 0) {
			ModelPrimitiveTextureSlot* slot = NULL;
			for (LegoS16 index = 0; index < record->m_slotCount; ++index) {
				if (record->m_slots[index].m_unk0x00 == 0) {
					slot = &record->m_slots[index];
					break;
				}
			}
			LegoS32 coordinate = slot->m_coordinateSet & 15;
			RendererVertexBuffer* vertices = record->m_vertexBuffer;
			const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			LegoU32 count = dispatch->m_getCount(vertices, NULL);
			dispatch->m_lock(vertices, NULL, 0, count, 0, 0);
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			dispatch->m_setCursor(vertices, NULL, 0);
			count = dispatch->m_getCount(vertices, NULL);
			for (LegoS32 vertex = 0; vertex < (LegoS32) count; ++vertex) {
				dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				dispatch->m_getCurrentPosition(vertices, NULL);
				dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				RendererTextureCoordinate* texture =
					dispatch->m_getCurrentTextureCoordinate(vertices, NULL, coordinate);
				if (p_variant == 1) {
					LegoFloat value = texture->m_v;
					texture->m_v = -texture->m_u;
					texture->m_u = value;
				}
				else if (p_variant == 2) {
					texture->m_u = -texture->m_u;
					texture->m_v = -texture->m_v;
				}
				else if (p_variant == 3) {
					LegoFloat value = texture->m_v;
					texture->m_v = texture->m_u;
					texture->m_u = -value;
				}
				dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				dispatch->m_advanceCursor(vertices, NULL);
			}
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			dispatch->m_unlock(vertices, NULL, 1);
		}
	}
	Vector3 minimum = p_entry->m_models[0].m_minimum;
	Vector3 maximum = p_entry->m_models[0].m_maximum;
	LegoFloat minX = minimum.m_x, minZ = minimum.m_z, maxX = maximum.m_x, maxZ = maximum.m_z;
	if (p_variant == 1) {
		minX = minimum.m_z;
		minZ = -minimum.m_x;
		maxX = maximum.m_z;
		maxZ = -maximum.m_x;
	}
	else if (p_variant == 2) {
		minX = -minimum.m_x;
		minZ = -minimum.m_z;
		maxX = -maximum.m_x;
		maxZ = -maximum.m_z;
	}
	else if (p_variant == 3) {
		minX = -minimum.m_z;
		minZ = minimum.m_x;
		maxX = -maximum.m_z;
		maxZ = maximum.m_x;
	}
	if (maxX < minX) {
		LegoFloat value = maxX;
		maxX = minX;
		minX = value;
	}
	if (maxZ < minZ) {
		LegoFloat value = maxZ;
		maxZ = minZ;
		minZ = value;
	}
	target.m_minimum.m_x = minX;
	target.m_minimum.m_y = minimum.m_y;
	target.m_minimum.m_z = minZ;
	target.m_maximum.m_x = maxX;
	target.m_maximum.m_y = maximum.m_y;
	target.m_maximum.m_z = maxZ;
}

// STUB: LEGORACERS2 0x00405330
void __fastcall AlignSecondaryBrickModel(
	SecondaryCarResourcePool* p_pool,
	void*,
	SecondaryBrickResource* p_entry,
	LegoS32 p_variant
)
{
	LegoFloat inverseX = 1.0f / p_pool->m_unk0x58;
	LegoFloat inverseY = 1.0f / p_pool->m_unk0x5c;
	SecondaryCarModel& target = p_entry->m_models[p_variant];
	ModelTextureGroup* group = &target.m_resource->m_groups[0];
	Vector3 originalMinimum = target.m_minimum;
	Vector3 extents;
	extents.m_x = target.m_maximum.m_x - originalMinimum.m_x;
	extents.m_y = target.m_maximum.m_y - originalMinimum.m_y;
	extents.m_z = target.m_maximum.m_z - originalMinimum.m_z;
	LegoFloat centerX = extents.m_x * 0.5f + originalMinimum.m_x;
	LegoFloat centerZ = extents.m_z * 0.5f + originalMinimum.m_z;
	Vector3 shift = {0.0f, 0.0f, 0.0f};
	LegoBool32 anchored = FALSE;
	for (LegoS32 primitive = 0; primitive < group->m_primitiveCount; ++primitive) {
		ModelPrimitiveTextureRecord* record = &group->m_primitives[primitive];
		Vector3 anchor;
		if (GetSecondaryPinVertexCount(record) != 0 && FindSecondaryPinAnchor(p_pool, NULL, record, &anchor)) {
			LegoFloat x = (anchor.m_x - centerX) * inverseX;
			LegoFloat y = (anchor.m_y - originalMinimum.m_y) * inverseY;
			LegoFloat z = (anchor.m_z - centerZ) * inverseX;
			shift.m_x = x - RoundSecondaryGridValue(x);
			shift.m_y = y - RoundSecondaryGridValue(y);
			shift.m_z = z - RoundSecondaryGridValue(z);
			if (shift.m_x > 0.0f) {
				shift.m_x -= 1.0f;
			}
			if (shift.m_y > 0.4f) {
				shift.m_y -= 1.0f;
			}
			if (shift.m_y < -0.6f) {
				shift.m_y += 1.0f;
			}
			if (shift.m_z > 0.0f) {
				shift.m_z -= 1.0f;
			}
			shift.m_x *= p_pool->m_unk0x58;
			shift.m_y *= p_pool->m_unk0x5c;
			shift.m_z *= p_pool->m_unk0x58;
			anchored = TRUE;
			break;
		}
	}
	if (!anchored) {
		if ((RoundSecondaryGridValue(extents.m_x * inverseX) & 1) != 0) {
			shift.m_x = -(p_pool->m_unk0x58 * 0.5f);
		}
		if ((RoundSecondaryGridValue(extents.m_z * inverseX) & 1) != 0) {
			shift.m_z = -(p_pool->m_unk0x58 * 0.5f);
		}
	}
	Vector3 seat;
	if (p_entry->m_category == 0 &&
		FindBuilderSeatPosition(p_pool, NULL, p_entry->m_nameId, p_variant, &seat, 0) != 0) {
		LegoFloat normalized = (seat.m_y - originalMinimum.m_y) * inverseY;
		shift.m_y = -((RoundSecondaryGridValue(normalized + 0.49f) - normalized) * p_pool->m_unk0x5c);
	}
	LegoFloat minimumX = centerX + shift.m_x;
	LegoFloat minimumZ = centerZ + shift.m_z;
	LegoFloat maximumX = minimumX;
	LegoFloat maximumZ = minimumZ;
	for (; target.m_minimum.m_x + 0.01f < minimumX; minimumX -= p_pool->m_unk0x58) {
	}
	for (; target.m_minimum.m_z + 0.01f < minimumZ; minimumZ -= p_pool->m_unk0x58) {
	}
	target.m_minimum.m_x = minimumX;
	target.m_minimum.m_y = originalMinimum.m_y;
	target.m_minimum.m_z = minimumZ;
	for (; maximumX < target.m_maximum.m_x - 0.01f; maximumX += p_pool->m_unk0x58) {
	}
	LegoFloat maximumY = originalMinimum.m_y;
	while (maximumY < target.m_maximum.m_y) {
		maximumY += p_pool->m_unk0x5c;
	}
	for (; maximumZ < target.m_maximum.m_z - 0.01f; maximumZ += p_pool->m_unk0x58) {
	}
	target.m_maximum.m_x = maximumX;
	target.m_maximum.m_y = maximumY;
	target.m_maximum.m_z = maximumZ;
	target.m_origin = target.m_minimum;
	target.m_origin.m_x += (RoundSecondaryGridValue((maximumX - minimumX) * inverseX) >> 1) * p_pool->m_unk0x58;
	target.m_origin.m_y += shift.m_y;
	target.m_origin.m_z += (RoundSecondaryGridValue((maximumZ - minimumZ) * inverseX) >> 1) * p_pool->m_unk0x58;
	target.m_origin.m_x *= -1.0f;
	target.m_origin.m_y *= -1.0f;
	target.m_origin.m_z *= -1.0f;
}

// STUB: LEGORACERS2 0x00405820
static void __stdcall CalculateSecondaryBrickBounds(SecondaryBrickResource* p_entry)
{
	Vector3 minimum = {9999.9f, 9999.9f, 9999.9f};
	Vector3 maximum = {-9999.9f, -9999.9f, -9999.9f};
	ModelResource* model = p_entry->m_models[0].m_resource;
	ModelTextureGroup* group = &model->m_groups[0];
	for (LegoS32 primitive = 0; primitive < group->m_primitiveCount; ++primitive) {
		RendererVertexBuffer* vertices = group->m_primitives[primitive].m_vertexBuffer;
		const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		LegoU32 count = dispatch->m_getCount(vertices, NULL);
		dispatch->m_lock(vertices, NULL, 0, count, 0, 0);
		dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		dispatch->m_setCursor(vertices, NULL, 0);
		count = dispatch->m_getCount(vertices, NULL);
		for (LegoS32 index = 0; index < (LegoS32) count; ++index) {
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			Vector3* position = dispatch->m_getCurrentPosition(vertices, NULL);
			if (position->m_x < minimum.m_x) {
				minimum.m_x = position->m_x;
			}
			if (maximum.m_x < position->m_x) {
				maximum.m_x = position->m_x;
			}
			if (position->m_y < minimum.m_y) {
				minimum.m_y = position->m_y;
			}
			if (maximum.m_y < position->m_y) {
				maximum.m_y = position->m_y;
			}
			if (position->m_z < minimum.m_z) {
				minimum.m_z = position->m_z;
			}
			if (maximum.m_z < position->m_z) {
				maximum.m_z = position->m_z;
			}
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			dispatch->m_advanceCursor(vertices, NULL);
		}
		dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		dispatch->m_unlock(vertices, NULL, 0);
	}
	p_entry->m_models[0].m_minimum = minimum;
	p_entry->m_models[0].m_maximum = maximum;
}

// STUB: LEGORACERS2 0x00405a00
static LegoU32 __stdcall GetSecondaryPinVertexCount(ModelPrimitiveTextureRecord* p_primitive)
{
	LegoS32 texture = -1;
	for (LegoS16 index = 0; index < p_primitive->m_slotCount; ++index) {
		if (p_primitive->m_slots[index].m_unk0x00 == 0) {
			texture = p_primitive->m_slots[index].m_texture;
			break;
		}
	}
	const RendererMainDispatch* renderer = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	const LegoChar* path = renderer->m_getTextureRecord(g_displayRenderer, NULL, texture)->m_resourcePath;
	if ((ContainsSeatPositionTag("PIN", path) && !ContainsSeatPositionTag("PINK", path)) ||
		ContainsSeatPositionTag("HOLE", path) || ContainsSeatPositionTag("UND", path)) {
		RendererVertexBuffer* vertices = p_primitive->m_vertexBuffer;
		return ((const RendererVertexBufferDispatch*) vertices->m_vtable)->m_getCount(vertices, NULL);
	}
	return 0;
}

// STUB: LEGORACERS2 0x00405ab0
static LegoBool32 __fastcall FindSecondaryPinAnchor(
	SecondaryCarResourcePool* p_pool,
	void*,
	ModelPrimitiveTextureRecord* p_primitive,
	Vector3* p_anchor
)
{
	RendererVertexBuffer* vertices = p_primitive->m_vertexBuffer;
	const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
	LegoU32 count = dispatch->m_getCount(vertices, NULL);
	if (count == 0) {
		return FALSE;
	}
	Vector3 minimum = {3.402823466e+38f, 3.402823466e+38f, 3.402823466e+38f};
	Vector3 maximum = {-3.402823466e+38f, -3.402823466e+38f, -3.402823466e+38f};
	dispatch->m_lock(vertices, NULL, 0, count, 0, 0);
	for (LegoS32 index = 0; index < (LegoS32) count; ++index) {
		dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		dispatch->m_setCursor(vertices, NULL, index);
		dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		Vector3* position = dispatch->m_getCurrentPosition(vertices, NULL);
		if (position->m_x < minimum.m_x) {
			minimum.m_x = position->m_x;
		}
		if (position->m_y < minimum.m_y) {
			minimum.m_y = position->m_y;
		}
		if (position->m_z < minimum.m_z) {
			minimum.m_z = position->m_z;
		}
		if (maximum.m_x < position->m_x) {
			maximum.m_x = position->m_x;
		}
		if (maximum.m_y < position->m_y) {
			maximum.m_y = position->m_y;
		}
		if (maximum.m_z < position->m_z) {
			maximum.m_z = position->m_z;
		}
	}
	dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
	dispatch->m_unlock(vertices, NULL, 0);
	LegoFloat centerZ = (minimum.m_z + maximum.m_z) * 0.5f;
	LegoFloat width = (1.0f / p_pool->m_unk0x58) * (maximum.m_x - minimum.m_x);
	if (width < 0.02f) {
		return FALSE;
	}
	if (width >= 0.9f) {
		if (fabs(width - RoundSecondaryGridValue(width)) >= 0.05000000074505806) {
			return FALSE;
		}
	}
	else {
		minimum.m_x = p_pool->m_unk0x58 * 0.5f + (maximum.m_x + minimum.m_x) * 0.5f;
	}
	p_anchor->m_x = minimum.m_x;
	p_anchor->m_y = maximum.m_y;
	LegoFloat depth = (1.0f / p_pool->m_unk0x58) * (maximum.m_z - minimum.m_z);
	if (depth >= 0.02f) {
		if (depth < 0.9f) {
			p_anchor->m_z = p_pool->m_unk0x58 * 0.5f + centerZ;
			return TRUE;
		}
		if (fabs(depth - RoundSecondaryGridValue(depth)) < 0.05000000074505806) {
			p_anchor->m_z = minimum.m_z;
			return TRUE;
		}
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x00406dc0
void __fastcall ResetSecondaryInactivityTimer(void* p_timer, void*)
{
	*(LegoFloat*) ((LegoU8*) p_timer + 0x60) = 45.0f;
}

// STUB: LEGORACERS2 0x0040b020
static void __fastcall UnlockAllSecondaryWorlds(GameControllerSelection* p_selection, void*)
{
	p_selection->m_values[0] = -1;
	p_selection->m_values[1] = -1;
	for (LegoS32 world = 0; world < 5; ++world) {
		p_selection->m_values[world + 2] = world;
	}
}

// STUB: LEGORACERS2 0x0040b040
static void __fastcall UnlockAllSecondaryRewards(GameControllerSelection* p_selection, void*)
{
	for (LegoU32 world = 0; world < 5; ++world) {
		for (LegoU32 reward = 0; reward < 2; ++reward) {
			if (!p_selection->HasReward(world, reward)) {
				p_selection->UnlockReward(world, reward, 0);
			}
		}
	}
}

// STUB: LEGORACERS2 0x0040b250
static void __fastcall MergeSecondarySelection(
	GameControllerSelection* p_destination,
	void*,
	const GameControllerSelection* p_source
)
{
	p_destination->m_values[0] |= p_source->m_values[0];
	p_destination->m_values[1] |= p_source->m_values[1];
	LegoS32 count = 0;
	while (count < 5 && p_destination->m_values[count + 2] != -1) {
		++count;
	}
	LegoS32* append = &p_destination->m_values[count + 2];
	for (LegoS32 index = 0; index < 5; ++index) {
		LegoS32 world = p_source->m_values[index + 2];
		if (world == -1) {
			break;
		}
		LegoS32 existing = 0;
		while (existing < 5 && p_destination->m_values[existing + 2] != world) {
			++existing;
		}
		if (existing >= 5) {
			*append++ = world;
		}
	}
}

// STUB: LEGORACERS2 0x0040b350
static void __fastcall UnlockAllSecondaryBonusFlags(LegoU32* p_flags, void*)
{
	*p_flags = 0xffffffff;
}

// STUB: LEGORACERS2 0x00410b60
static void __fastcall SetSecondarySessionName(
	void* p_object,
	void*,
	const GameControllerName* p_name,
	LegoS32 p_player
)
{
	memcpy((LegoU8*) p_object + 8 + p_player * 20, p_name, 20);
}

// STUB: LEGORACERS2 0x00410b80
static void __fastcall SetSecondarySessionCharacter(
	void* p_object,
	void*,
	const LegoU32* p_parts,
	LegoU32 p_value,
	LegoS32 p_player
)
{
	memcpy((LegoU8*) p_object + 0x30 + p_player * 16, p_parts, 16);
	*(LegoU32*) ((LegoU8*) p_object + 0x50 + p_player * 4) = p_value;
}

// STUB: LEGORACERS2 0x00410bc0
static void __fastcall SetSecondarySessionCar(
	void* p_object,
	void*,
	const GameControllerItemBlock* p_car,
	LegoS32 p_player,
	LegoS32 p_carSet
)
{
	memcpy((LegoU8*) p_object + 0x58 + p_player * 0xec4 + p_carSet * 0x2f4, p_car, 0x2f4);
}

// STUB: LEGORACERS2 0x00410c40
static void __fastcall UnlockSecondaryTrackProgress(LegoU32* p_progress, void*)
{
	for (LegoU32 world = 0; world < 5; ++world) {
		for (LegoU32 track = 1; track <= 4; ++track) {
			if (!IsFoyerTrackProgressSet(p_progress, NULL, world, track)) {
				SetSecondaryTrackProgress(p_progress, NULL, world, track);
			}
		}
	}
}

// STUB: LEGORACERS2 0x00410c80
static void __fastcall UnlockSecondaryBonusProgress(LegoU32* p_progress, void*)
{
	for (LegoU32 world = 0; world < 5; ++world) {
		for (LegoU32 bonus = 0; bonus < 3; ++bonus) {
			if (!IsFoyerBonusProgressSet(p_progress, NULL, world, bonus)) {
				SetSecondaryBonusProgress(p_progress, NULL, world, bonus);
			}
		}
	}
}

// STUB: LEGORACERS2 0x00410d40
static void __fastcall SetSecondaryTrackProgress(LegoU32* p_progress, void*, LegoU32 p_world, LegoU32 p_track)
{
	p_progress[0] |= 1 << ((p_track + p_world * 4 - 1) & 31);
	RefreshSecondaryProgressUnlocks(g_gameController.m_unk0x8c, NULL);
}

// STUB: LEGORACERS2 0x00410d90
static void __fastcall SetSecondaryBonusProgress(LegoU32* p_progress, void*, LegoU32 p_world, LegoU32 p_bonus)
{
	p_progress[1] |= 1 << ((p_bonus + p_world * 3) & 31);
	RefreshSecondaryProgressUnlocks(g_gameController.m_unk0x8c, NULL);
}

// STUB: LEGORACERS2 0x00410e10
static void __fastcall UnlockSecondaryModeProgress(LegoU32* p_progress, void*)
{
	for (LegoU32 mode = 0; mode < 3; ++mode) {
		SetSecondaryModeProgress(p_progress, NULL, mode);
		for (LegoU32 index = 0; index < 10; ++index) {
			IncrementSecondaryModeProgress(p_progress, NULL, mode);
		}
	}
}

// STUB: LEGORACERS2 0x00410e40
static void __fastcall SetSecondaryModeProgress(LegoU32* p_progress, void*, LegoU32 p_mode)
{
	p_progress[0] |= 1 << ((p_mode * 5 + 4) & 31);
	RefreshSecondaryProgressUnlocks(g_gameController.m_unk0x8c, NULL);
}

// STUB: LEGORACERS2 0x00411520
static void __fastcall RefreshSecondaryProgressUnlocks(void* p_object, void*)
{
	if (g_gameControllerSession->m_unk0x04 == 0) {
		return;
	}
	LegoU8* state = (LegoU8*) p_object;
	LegoU32* progress = (LegoU32*) (state + 0x1df4);
	LegoU32 trackCount = CountFoyerTrackProgress(progress, NULL);
	LegoU32 totalCount = CountFoyerTrackAndBonusProgress(progress, NULL);
	LegoU32 modeCount = CountFoyerModeProgress((LegoU32*) (state + 0x1dfc), NULL);
	GameControllerSelection* selection = (GameControllerSelection*) (state + 0x1e04);
	for (LegoU32 world = 0; world < 5; ++world) {
		const FoyerProgressThreshold& threshold = g_foyerProgressThresholds[world];
		if (!((SecondaryManagerMediaState*) selection)->IsSlotEnabled(world) &&
			(LegoS32) trackCount >= (LegoS32) threshold.m_tracks &&
			(LegoS32) totalCount >= (LegoS32) threshold.m_tracksAndBonuses &&
			(LegoS32) modeCount >= (LegoS32) threshold.m_modes) {
			selection->UnlockWorld(world);
		}
		for (LegoU32 track = 1; track <= 5; ++track) {
			if ((world != 0 || track != 5) && !selection->IsTrackUnlocked(world, track) &&
				((SecondaryManagerMediaState*) selection)->IsSlotEnabled(world)) {
				if (world == 0 && track != 1 && !IsFoyerTrackProgressSet(progress, NULL, 0, track - 1)) {
					continue;
				}
				if (world != 0 && track > 4) {
					LegoBool32 complete = TRUE;
					for (LegoU32 previous = 1; previous < 5; ++previous) {
						if (!IsFoyerTrackProgressSet(progress, NULL, world, previous)) {
							complete = FALSE;
						}
					}
					if (!complete) {
						continue;
					}
				}
				selection->UnlockTrack(world, track);
			}
		}
	}
	MergeSecondarySelection(&g_gameController.m_selection, NULL, selection);
	g_gameController.m_selection.UnlockDefaults(1, 0);
}

// STUB: LEGORACERS2 0x00411700
LegoBool32 __fastcall LoadSecondarySessionData(void* p_object, void*, const void* p_data)
{
	const LegoU32* words = (const LegoU32*) p_data;
	if (words[0] == 0) {
		return TRUE;
	}
	if (words[0x78b] != 0xb00bface) {
		return FALSE;
	}
	memcpy((LegoU8*) p_object + 0x1e34, p_data, 0x1e34);
	return TRUE;
}

// STUB: LEGORACERS2 0x004117f0
void __fastcall FinishSecondarySessionSelection(void* p_object, void*)
{
	LegoU8* state = (LegoU8*) p_object;
	LegoS32& mode = *(LegoS32*) (state + 0x1de8);
	switch (mode) {
	case 0:
	case 2:
		mode = -1;
		g_gameControllerSession->m_mode = 7;
		g_gameController.m_state = 1;
		g_gameController.m_stateStarted = 0;
		return;
	case 1: {
		for (LegoS32 player = 0; player < *(LegoS32*) (state + 4); ++player) {
			LegoU8* sessionPlayer = (LegoU8*) g_gameControllerSession + player * 0x344;
			if (g_gameControllerSession->m_selectedMode == 0) {
				SetSecondarySessionName(
					p_object,
					NULL,
					(const GameControllerName*) ((LegoU8*) g_gameControllerSettings + 4 + player * 20),
					player
				);
				SetSecondarySessionCharacter(
					p_object,
					NULL,
					(const LegoU32*) (sessionPlayer + 0x370),
					*(LegoU32*) (sessionPlayer + 0x380),
					player
				);
			}
			SetSecondarySessionCar(
				p_object,
				NULL,
				(const GameControllerItemBlock*) (sessionPlayer + 0x6c),
				player,
				g_gameControllerSession->m_selectedMode
			);
		}
		mode = -1;
		g_gameControllerSession->m_mode = 7;
		g_gameController.m_state = 1;
		g_gameController.m_stateStarted = 0;
		return;
	}
	case 3:
		if (*(LegoS32*) (state + 0x1de4) > 6 && g_gameControllerSession->m_selectedMode == 0) {
			mode = 4;
			EnterSecondaryManagerMode(g_gameController.m_secondaryManager, NULL, 0x2f);
			SetNpcTextManagerText(g_gameController.m_npcTextManager, NULL, 0, 0x14f, -1.0f, 0);
			return;
		}
		ClearSecondaryNpcTextMode(g_gameController.m_npcTextManager, NULL, 0);
		break;
	case 4:
		SetNpcTextManagerText(g_gameController.m_npcTextManager, NULL, 0, 0x150, 8.0f, 0);
		break;
	default:
		return;
	}
	SetFoyerSceneTransition(g_gameController.m_foyerManager, NULL, 0);
	mode = -1;
}

// STUB: LEGORACERS2 0x00411eb0
LegoBool32 __fastcall ApplySecondarySessionCheat(void* p_object, void*, LegoU32 p_cheat)
{
	bool worlds = false, rewards = false, bonusFlags = false, tracks = false, bonuses = false, modes = false,
		 finalMode = false, starter = false;
	LegoS32 targetWorld = -1;
	switch (p_cheat) {
	case 0:
		worlds = rewards = bonusFlags = tracks = bonuses = modes = finalMode = starter = true;
		break;
	case 1:
		worlds = tracks = modes = starter = true;
		break;
	case 2:
		rewards = true;
		break;
	case 3:
		modes = true;
		break;
	case 4:
		bonuses = true;
		break;
	case 5:
	case 6:
	case 7:
	case 8:
		targetWorld = p_cheat - 4;
		modes = targetWorld == 4;
		starter = true;
		break;
	default:
		return FALSE;
	}
	LegoU8* state = (LegoU8*) p_object;
	GameControllerSelection* selection = (GameControllerSelection*) (state + 0x1e04);
	LegoU32* progress = (LegoU32*) (state + 0x1df4);
	if (worlds) {
		for (LegoU32 world = 0; world < 5; ++world) {
			if (!((SecondaryManagerMediaState*) selection)->IsSlotEnabled(world)) {
				selection->UnlockWorld(world);
			}
		}
	}
	if (rewards) {
		UnlockAllSecondaryRewards(selection, NULL);
		MergeSecondarySelection(&g_gameController.m_selection, NULL, selection);
	}
	if (bonusFlags) {
		UnlockAllSecondaryBonusFlags((LegoU32*) (state + 0x1e24), NULL);
	}
	if (tracks) {
		UnlockSecondaryTrackProgress(progress, NULL);
	}
	if (bonuses) {
		UnlockSecondaryBonusProgress(progress, NULL);
		for (LegoS32 index = 0; index < 3; ++index) {
			GameObject* object = *(GameObject**) (state + 0x3c68 + index * 4);
			if (object != NULL && *(LegoU32*) ((LegoU8*) object + 0xb4) != 0) {
				RestoreSecondaryBonusObject(object, NULL);
			}
		}
	}
	if (modes) {
		UnlockSecondaryModeProgress((LegoU32*) (state + 0x1dfc), NULL);
		if (g_gameController.m_unk0x88 != NULL) {
			RefreshSecondaryModeWidgets(g_gameController.m_unk0x88, NULL);
		}
	}
	if (finalMode) {
		*(LegoU32*) (state + 0x1e00) = 1;
	}
	if (starter) {
		for (LegoU32 track = 1; track < 5; ++track) {
			SetSecondaryTrackProgress(progress, NULL, 0, track);
		}
		*(LegoU32*) (state + 0x1de4) = 7;
	}
	if (targetWorld != -1) {
		for (LegoU32 world = 1; world < 4; ++world) {
			for (LegoU32 track = 1; track < 5; ++track) {
				if ((LegoS32) CountFoyerTrackAndBonusProgress(progress, NULL) <
					(LegoS32) g_foyerProgressThresholds[targetWorld].m_tracksAndBonuses) {
					SetSecondaryTrackProgress(progress, NULL, world, track);
				}
			}
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004120d0
static void InitializeSecondaryWheelFullAngle()
{
	g_secondaryCategoryWheelTuning.m_fullAngle = g_secondaryCategoryWheelPi + g_secondaryCategoryWheelPi;
}

// STUB: LEGORACERS2 0x004120f0
static void InitializeSecondaryWheelPosition()
{
	g_secondaryCategoryWheelTuning.m_position[0] = 0.425f;
	g_secondaryCategoryWheelTuning.m_position[1] = 0.0f + 0.554f;
}

// STUB: LEGORACERS2 0x00412120
static void InitializeSecondaryWheelHeaderSize()
{
	g_secondaryCategoryWheelTuning.m_headerSize[0] = 1.0f * 0.684f;
	g_secondaryCategoryWheelTuning.m_headerSize[1] = 1.0f * 0.703f;
}

// STUB: LEGORACERS2 0x00412160
static void InitializeSecondaryWheelIconSize()
{
	g_secondaryCategoryWheelTuning.m_iconSize[0] = 1.0f * 0.125f;
	g_secondaryCategoryWheelTuning.m_iconSize[1] = 1.0f * 0.167f;
}

// STUB: LEGORACERS2 0x004121a0
static void InitializeSecondaryWheelCenterOffset()
{
	g_secondaryCategoryWheelTuning.m_centerOffset[0] = 0.0205f;
	g_secondaryCategoryWheelTuning.m_centerOffset[1] = -0.0169f;
}

// STUB: LEGORACERS2 0x004121d0
static void InitializeSecondaryWheelFirstBasis()
{
	g_secondaryCategoryWheelTuning.m_firstBasis[0] = 1.0f * 0.6428f;
	g_secondaryCategoryWheelTuning.m_firstBasis[1] = 1.0f * 0.766f;
}

// STUB: LEGORACERS2 0x00412210
static void InitializeSecondaryWheelSecondBasis()
{
	g_secondaryCategoryWheelTuning.m_secondBasis[0] = 1.0f * 0.766f;
	g_secondaryCategoryWheelTuning.m_secondBasis[1] = 1.0f * -0.6428f;
}

// STUB: LEGORACERS2 0x00412250
static void InitializeSecondaryWheelPhase()
{
	g_secondaryCategoryWheelTuning.m_phase = g_secondaryCategoryWheelPi * 0.5f;
}

// STUB: LEGORACERS2 0x00412280
static void InitializeSecondaryWheelAngularSpeed()
{
	g_secondaryCategoryWheelTuning.m_angularSpeed = g_secondaryCategoryWheelPi * 1.5f;
}

// STUB: LEGORACERS2 0x004122b0
static void InitializeSecondaryWheelShadowOffset()
{
	g_secondaryCategoryWheelTuning.m_shadowOffset[0] = 0.015f;
	g_secondaryCategoryWheelTuning.m_shadowOffset[1] = 0.04f;
}
// STUB: LEGORACERS2 0x004122d0
void* __fastcall ConstructSecondaryAuxiliary(void* p_object, void*)
{
	LegoU32* words = (LegoU32*) p_object;
	for (LegoU32 index = 0; index < 7; ++index) {
		words[index] = 0;
		words[index + 7] = 0;
	}
	for (index = 0; index < 6; ++index) {
		words[index + 14] = 0;
		words[index + 20] = 0;
	}
	return p_object;
}

// STUB: LEGORACERS2 0x00412300
void __fastcall InitializeSecondaryAuxiliary(void* p_object, void*)
{
	LegoU32* words = (LegoU32*) p_object;
	for (LegoU32 index = 0x20; index <= 0x29; ++index) {
		words[index] = 0;
	}
	words[0x26] = 0xffffffff;
	words[0x27] = 0xffffffff;
	LegoFloat* values = (LegoFloat*) ((LegoU8*) p_object + 0x68);
	for (index = 0; index < 6; ++index) {
		values[index] = -1.0f;
	}
}

// STUB: LEGORACERS2 0x00412360
void __fastcall ReleaseSecondaryObject88(void* p_object, void*)
{
	ClearSecondaryManagerAuxiliaryResources(p_object, NULL, 1, 1, 1);
	LegoU32* active = (LegoU32*) ((LegoU8*) p_object + 0x80);
	if (*active != 0) {
		ReleaseSecondaryAuxiliaryModel(g_gameController.m_unk0x90, NULL);
		*active = 0;
	}
}

// GLOBAL: LEGORACERS2 0x0052fb78
static const LegoS32 g_secondaryModeRecords00[25] = {0, 3, 1, 2, 3, 1, 1, 3, 1, 3, 3, 1, 0,
													 1, 1, 2, 1, 1, 1, 1, 1, 3, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fbe8
static const LegoS32 g_secondaryModeRecords01[19] = {1, 1, 1, 2, 2, 1, 1, 1, 1, 0, 1, 1, 3, 2, 1, 0, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fc40
static const LegoS32 g_secondaryModeRecords02[19] = {2, 1, 1, 1, 2, 1, 2, 1, 1, 3, 1, 1, 0, 2, 1, 3, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fc98
static const LegoS32 g_secondaryModeRecords03[19] = {3, 1, 1, 0, 1, 1, 2, 3, 1, 3, 1, 1, 1, 1, 1, 2, 3, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fcf0
static const LegoS32 g_secondaryModeRecords04[37] = {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
													 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 3, 1, 1, 1, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fe50
static const LegoS32 g_secondaryModeRecords05[22] = {0, 2, 1, 1, 2, 1, 0, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fd90
static const LegoS32 g_secondaryModeRecords06[22] = {0, 2, 1, 1, 2, 1, 0, 2, 1, 1, 2, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fdf0
static const LegoS32 g_secondaryModeRecords07[22] = {0, 2, 1, 1, 2, 1, 0, 2, 1, 1, 2, 1, 3, 1, 1, 0, 1, 1, 1, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052feb0
static const LegoS32 g_secondaryModeRecords08[22] = {0, 2, 1, 1, 2, 1, 0, 2, 1, 1, 2, 1, 3, 1, 1, 1, 1, 1, 0, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x0052ff10
static const LegoS32 g_secondaryModeRecords09[25] = {1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 3,
													 1, 1, 0, 1, 1, 1, 1, 1, 2, 2, 1, -1};

// GLOBAL: LEGORACERS2 0x0052ff80
static const LegoS32 g_secondaryModeRecords10[25] = {0, 1, 1, 2, 3, 1, 1, 1, 1, 2, 3, 1, 0,
													 1, 1, 2, 3, 1, 1, 1, 1, 2, 3, 1, -1};

// GLOBAL: LEGORACERS2 0x0052fff0
static const LegoS32 g_secondaryModeRecords11[13] = {0, 4, 1, 1, 4, 1, 2, 4, 1, 3, 4, 1, -1};

// GLOBAL: LEGORACERS2 0x00530030
static const LegoS32 g_secondaryModeRecords12[19] = {0, 3, 1, 1, 3, 1, 2, 3, 1, 3, 3, 1, 0, 3, 1, 1, 3, 1, -1};

// GLOBAL: LEGORACERS2 0x00530088
static const LegoS32 g_secondaryModeRecords13[61] = {0, 1, 1, 1, 1, 1, 2, 1, 1, 3, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1,
													 3, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 3, 1, 1, 1, 1, 1, 0, 1, 1,
													 2, 1, 1, 3, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 3, 1, 1, -1};

// GLOBAL: LEGORACERS2 0x00530188
static const LegoS32 g_secondaryModeRecords14[4] = {0, 10, 1, -1};

// GLOBAL: LEGORACERS2 0x005301a0
static const LegoS32 g_secondaryModeRecords15[4] = {1, 10, 1, -1};

// GLOBAL: LEGORACERS2 0x0053ec88
static const LegoS32* g_secondaryModeRecordTables[16] = {
	g_secondaryModeRecords00,
	g_secondaryModeRecords01,
	g_secondaryModeRecords02,
	g_secondaryModeRecords03,
	g_secondaryModeRecords04,
	g_secondaryModeRecords05,
	g_secondaryModeRecords06,
	g_secondaryModeRecords07,
	g_secondaryModeRecords08,
	g_secondaryModeRecords09,
	g_secondaryModeRecords10,
	g_secondaryModeRecords11,
	g_secondaryModeRecords12,
	g_secondaryModeRecords13,
	g_secondaryModeRecords14,
	g_secondaryModeRecords15
};

// STUB: LEGORACERS2 0x00412390
void __fastcall LoadSecondaryAuxiliaryResources(void* p_object, void*)
{
	LegoU32* active = (LegoU32*) ((LegoU8*) p_object + 0x80);
	if (*active == 0) {
		*active = 1;
		LoadSecondaryAuxiliaryTextureRecords(g_gameController.m_unk0x90, NULL);
	}
}

// STUB: LEGORACERS2 0x004123b0
void __fastcall ReleaseSecondaryAuxiliaryResources(void* p_object, void*)
{
	ClearSecondaryManagerAuxiliaryResources(p_object, NULL, 1, 1, 1);
	LegoU32* active = (LegoU32*) ((LegoU8*) p_object + 0x80);
	if (*active != 0) {
		*active = 0;
		ReleaseSecondaryAuxiliaryModel(g_gameController.m_unk0x90, NULL);
	}
}

// STUB: LEGORACERS2 0x004123e0
static void __fastcall CreateSecondaryWheelSprites(SecondaryCategoryWheel* p_wheel, void*, LegoU32 p_previous)
{
	LegoU32 parameters[21];
	for (LegoU32 word = 0; word < 8; ++word) {
		parameters[word] = 0;
	}
	parameters[6] = parameters[7] = 0x3f800000;
	parameters[10] = 0xffffffff;
	parameters[11] = 0;
	// Texture coordinates and tile-size words remain untouched until texture copy.
	parameters[16] = 0;
	parameters[17] = 7;
	parameters[18] = 1;
	parameters[19] = parameters[20] = 0;
	LegoS32 count = 0;
	LegoU32 shadowColor = 0x80000000;
	for (LegoU32 icon = 0; icon < 6; ++icon) {
		if (p_wheel->m_angles[icon] >= 0.0f) {
			GlobalOverlaySprite** sprite = p_previous ? &p_wheel->m_previousIcons[icon] : &p_wheel->m_icons[icon];
			GlobalOverlaySprite** shadow = p_previous ? &p_wheel->m_previousShadows[icon] : &p_wheel->m_shadows[icon];
			*sprite = g_globalOverlayManager.AllocateSprite(parameters);
			(*sprite)->SetList(4);
			(*sprite)->SetSize(g_secondaryCategoryWheelTuning.m_iconSize);
			*shadow = g_globalOverlayManager.AllocateSprite(parameters);
			(*shadow)->SetList(3);
			(*shadow)->SetSize(g_secondaryCategoryWheelTuning.m_iconSize);
			(*shadow)->SetColor(&shadowColor);
			++count;
		}
	}
	if (p_previous != 0 || count <= 0) {
		return;
	}
	parameters[17] = 1;
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	if (p_wheel->m_icons[6] == NULL) {
		GlobalOverlaySprite* header = g_globalOverlayManager.AllocateSprite(parameters);
		p_wheel->m_icons[6] = header;
		header->SetList(2);
		header->SetPosition(g_secondaryCategoryWheelTuning.m_position);
		header->SetSize(g_secondaryCategoryWheelTuning.m_headerSize);
		memcpy(&header->m_texture, (LegoU8*) resources + 0x1378, 0x14);
	}
	if (p_wheel->m_shadows[6] == NULL) {
		GlobalOverlaySprite* shadow = g_globalOverlayManager.AllocateSprite(parameters);
		p_wheel->m_shadows[6] = shadow;
		shadow->SetList(1);
		LegoFloat position[2] = {
			g_secondaryCategoryWheelTuning.m_position[0] +
				g_secondaryCategoryWheelTuning.m_headerSize[0] * g_secondaryCategoryWheelTuning.m_shadowOffset[0],
			g_secondaryCategoryWheelTuning.m_position[1] +
				g_secondaryCategoryWheelTuning.m_headerSize[1] * g_secondaryCategoryWheelTuning.m_shadowOffset[1]
		};
		shadow->SetPosition(position);
		shadow->SetSize(g_secondaryCategoryWheelTuning.m_headerSize);
		shadow->SetColor(&shadowColor);
		memcpy(&shadow->m_texture, (LegoU8*) resources + 0x1378, 0x14);
	}
}
// STUB: LEGORACERS2 0x00412770
void __fastcall UpdateSecondaryAuxiliary(void* p_object, void*, LegoU32 p_frameParameter)
{
	SecondaryCategoryWheel* wheel = (SecondaryCategoryWheel*) p_object;
	if (wheel->m_enabled == 0 || wheel->m_resourcesReady == 0) {
		return;
	}
	union FrameBits {
		LegoU32 m_word;
		LegoFloat m_float;
	} frame;
	frame.m_word = p_frameParameter;
	LegoFloat elapsed = frame.m_float;
	MenuManager* menu = (MenuManager*) g_gameController.m_menuManager;
	LegoS32 selection = -1;
	if (menu->m_unk0x198 != 0) {
		selection = *(LegoS32*) ((LegoU8*) menu + menu->m_unk0x198 * 0x14 + 0x50);
	}
	LegoS32 candidate = selection;
	if ((LegoS32) menu->m_unk0x1a0 != wheel->m_mode) {
		wheel->m_transition = 1;
		wheel->m_transitionTime = 0.5f;
		SnapshotSecondaryWheelIcons(wheel, NULL, wheel->m_mode);
		wheel->m_previousCount = wheel->m_count;
		wheel->m_count = CreateSecondaryWheelIcons(wheel, NULL, menu->m_unk0x1a0);
		wheel->m_currentAngle = *(LegoFloat*) ((LegoU8*) wheel + 0x68 + candidate * 4);
		wheel->m_highlightTime = 0.0f;
		wheel->m_mode = menu->m_unk0x1a0;
	}
	LegoU8 previousAlpha = 0;
	if (wheel->m_transition != 0) {
		wheel->m_transitionTime -= elapsed;
		if (wheel->m_transitionTime <= 0.0f) {
			wheel->m_transition = 0;
			ClearSecondaryManagerAuxiliaryResources(wheel, NULL, wheel->m_count == 0, 0, 1);
		}
		previousAlpha = RoundSecondaryWheelAlpha((1.0f / 0.5f) * wheel->m_transitionTime * 255.0f);
		for (LegoU32 index = 0; index < 6; ++index) {
			if (wheel->m_previousIcons[index] != NULL) {
				wheel->m_previousIcons[index]->SetAlpha(previousAlpha);
			}
			if (wheel->m_previousShadows[index] != NULL) {
				wheel->m_previousShadows[index]->SetAlpha(previousAlpha >> 1);
			}
		}
		if (wheel->m_previousCount > 0 && wheel->m_count == 0) {
			if (wheel->m_icons[6] != NULL) {
				wheel->m_icons[6]->SetAlpha(previousAlpha);
			}
			if (wheel->m_shadows[6] != NULL) {
				wheel->m_shadows[6]->SetAlpha(previousAlpha >> 1);
			}
		}
	}
	if (wheel->m_count == 0) {
		return;
	}
	if (candidate >= 0 && candidate < 6) {
		while (wheel->m_angles[candidate] < 0.0f) {
			if (++candidate > 5) {
				candidate = 0;
			}
			if (candidate == selection) {
				break;
			}
		}
		if (wheel->m_angles[candidate] >= 0.0f) {
			wheel->m_targetAngle = wheel->m_angles[candidate];
		}
	}
	LegoFloat distance = (LegoFloat) fabs(wheel->m_targetAngle - wheel->m_currentAngle);
	LegoFloat fullAngle = g_secondaryCategoryWheelPi + g_secondaryCategoryWheelPi;
	if (distance > g_secondaryCategoryWheelPi) {
		distance = fullAngle - distance;
	}
	LegoBool32 moving = FALSE;
	if (distance > 0.01f) {
		moving = TRUE;
		LegoFloat step = g_secondaryCategoryWheelTuning.m_angularSpeed * elapsed;
		if (fabs(step) <= distance) {
			LegoFloat target = wheel->m_targetAngle;
			while (target < wheel->m_currentAngle) {
				target += fullAngle;
			}
			LegoFloat direction = target - wheel->m_currentAngle < g_secondaryCategoryWheelPi ? 1.0f : -1.0f;
			wheel->m_currentAngle = direction * step + wheel->m_currentAngle;
			if (wheel->m_currentAngle < 0.0f) {
				wheel->m_currentAngle += fullAngle;
			}
			if (wheel->m_currentAngle > fullAngle) {
				wheel->m_currentAngle -= fullAngle;
			}
		}
		else {
			wheel->m_currentAngle = wheel->m_targetAngle;
		}
	}
	wheel->m_highlightTime -= elapsed;
	if (wheel->m_highlightTime <= 0.0f) {
		if (moving) {
			wheel->m_highlighted = -1;
			wheel->m_highlightTime = 0.0f;
		}
		else {
			wheel->m_highlighted = candidate;
			wheel->m_highlightTime = 0.3f + 0.3f;
		}
	}
	for (LegoU32 index = 0; index < 6; ++index) {
		if (wheel->m_angles[index] >= 0.0f) {
			LegoFloat position[2];
			LegoFloat scale;
			ProjectSecondaryWheelIcon(wheel->m_currentAngle - wheel->m_angles[index], position, &scale);
			wheel->m_icons[index]->SetPosition(position);
			if ((LegoS32) index == wheel->m_highlighted) {
				LegoFloat pulse =
					wheel->m_highlightTime <= 0.3f ? wheel->m_highlightTime : (0.3f + 0.3f) - wheel->m_highlightTime;
				scale = (((1.1f - 1.0f) / 0.3f) * pulse + 1.0f) * scale;
			}
			LegoFloat size[2] = {
				g_secondaryCategoryWheelTuning.m_iconSize[0] * scale,
				g_secondaryCategoryWheelTuning.m_iconSize[1] * scale
			};
			wheel->m_icons[index]->SetSize(size);
			LegoU8 alpha = wheel->m_transition == 0 ? 255 : (LegoU8) (-previousAlpha - 1);
			wheel->m_icons[index]->SetAlpha(alpha);
			position[0] += size[0] * g_secondaryCategoryWheelTuning.m_shadowOffset[0];
			position[1] += g_secondaryCategoryWheelTuning.m_shadowOffset[1] * size[1];
			wheel->m_shadows[index]->SetPosition(position);
			wheel->m_shadows[index]->SetSize(size);
			wheel->m_shadows[index]->SetAlpha(alpha >> 1);
		}
	}
	if (wheel->m_transition != 0 && wheel->m_previousCount == 0) {
		LegoU8 alpha = (LegoU8) (-previousAlpha - 1);
		wheel->m_icons[6]->SetAlpha(alpha);
		wheel->m_shadows[6]->SetAlpha(alpha >> 1);
		return;
	}
	wheel->m_icons[6]->SetAlpha(255);
	wheel->m_shadows[6]->SetAlpha(128);
}

// STUB: LEGORACERS2 0x00412ca0
static void __stdcall ProjectSecondaryWheelIcon(LegoFloat p_angle, LegoFloat* p_position, LegoFloat* p_scale)
{
	LegoFloat fullAngle = g_secondaryCategoryWheelPi + g_secondaryCategoryWheelPi;
	double angle = p_angle;
	while (angle < 0.0) {
		angle += fullAngle;
	}
	while (angle > fullAngle) {
		angle -= fullAngle;
	}
	LegoFloat first =
		(LegoFloat) sin((angle + g_secondaryCategoryWheelTuning.m_phase) - g_secondaryCategoryWheelPi * 0.5f);
	double second = -sin(angle + g_secondaryCategoryWheelTuning.m_phase) * 0.6896551251411438f;
	p_position[0] = (LegoFloat) ((g_secondaryCategoryWheelTuning.m_firstBasis[0] * first +
								  g_secondaryCategoryWheelTuning.m_secondBasis[0] * second) *
								 0.35f);
	p_position[1] = (LegoFloat) ((g_secondaryCategoryWheelTuning.m_firstBasis[1] * first +
								  g_secondaryCategoryWheelTuning.m_secondBasis[1] * second) *
								 0.35f);
	p_position[0] = g_secondaryCategoryWheelTuning.m_position[0] + p_position[0];
	p_position[1] = g_secondaryCategoryWheelTuning.m_position[1] + p_position[1];
	p_position[0] = g_secondaryCategoryWheelTuning.m_centerOffset[0] + p_position[0];
	p_position[1] = g_secondaryCategoryWheelTuning.m_centerOffset[1] + p_position[1];
	double depth = sin(g_secondaryCategoryWheelPi * 0.5f + angle);
	*p_scale = (LegoFloat) ((depth > 0.0 ? 1.5f : 0.5f) * depth + 1.0f);
}

// STUB: LEGORACERS2 0x00412dd0
static void __fastcall SnapshotSecondaryWheelIcons(SecondaryCategoryWheel* p_wheel, void*, LegoS32 p_oldMode)
{
	if (p_oldMode == -1) {
		return;
	}
	CreateSecondaryWheelSprites(p_wheel, NULL, 1);
	for (LegoU32 icon = 0; icon < 6; ++icon) {
		if (p_wheel->m_angles[icon] >= 0.0f) {
			GlobalOverlaySprite* current = p_wheel->m_icons[icon];
			LegoFloat size[2];
			current->GetSize(size);
			p_wheel->m_previousIcons[icon]->SetSize(size);
			LegoFloat position[2] = {current->m_left + size[0] * 0.5f, current->m_top + size[1] * 0.5f};
			p_wheel->m_previousIcons[icon]->SetPosition(position);
			memcpy(&p_wheel->m_previousIcons[icon]->m_texture, &current->m_texture, 0x14);
			p_wheel->m_previousShadows[icon]->SetSize(size);
			position[0] += g_secondaryCategoryWheelTuning.m_shadowOffset[0] * size[0];
			position[1] += g_secondaryCategoryWheelTuning.m_shadowOffset[1] * size[1];
			p_wheel->m_previousShadows[icon]->SetPosition(position);
			memcpy(&p_wheel->m_previousShadows[icon]->m_texture, &current->m_texture, 0x14);
		}
	}
}

// STUB: LEGORACERS2 0x00412f70
static LegoS32 __fastcall CreateSecondaryWheelIcons(SecondaryCategoryWheel* p_wheel, void*, LegoS32 p_mode)
{
	LegoS32 count = 0;
	LegoS32 textures[6] = {-1, -1, -1, -1, -1, -1};
	switch (p_mode) {
	case 1:
		textures[0] = 0xfa;
		textures[1] = 0xfb;
		textures[2] = 0xfc;
		textures[3] = 0xfd;
		textures[4] = 0xfe;
		textures[5] = 0x10b;
		count = 6;
		break;
	case 2:
		textures[0] = 0xff;
		textures[1] = 0x100;
		textures[2] = 0x101;
		textures[3] = 0x102;
		textures[4] = 0x103;
		textures[5] = 0x10b;
		count = 6;
		break;
	case 0x13:
	case 0x15:
		textures[0] = 0x100;
		textures[1] = 0x105;
		textures[2] = 0x104;
		textures[5] = 0x10b;
		count = 4;
		break;
	case 0x1b:
		textures[0] = 0x106;
		textures[1] = 0x108;
		textures[2] = 0x109;
		textures[3] = 0x107;
		textures[4] = 0x10a;
		textures[5] = 0x10b;
		count = 6;
		break;
	}
	LegoS32 skipped = 0;
	for (LegoU32 icon = 0; icon < 6; ++icon) {
		if (textures[icon] == -1) {
			++skipped;
			p_wheel->m_angles[icon] = -1.0f;
		}
		else {
			p_wheel->m_angles[icon] =
				((g_secondaryCategoryWheelPi + g_secondaryCategoryWheelPi) / count) * ((LegoS32) icon - skipped);
		}
	}
	ClearSecondaryManagerAuxiliaryResources(p_wheel, NULL, 0, 1, 0);
	CreateSecondaryWheelSprites(p_wheel, NULL, 0);
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	for (icon = 0; icon < 6; ++icon) {
		if (p_wheel->m_angles[icon] != -1.0f) {
			if (p_wheel->m_icons[icon] != NULL) {
				memcpy(&p_wheel->m_icons[icon]->m_texture, &resources->m_records[textures[icon]], 0x14);
			}
			if (p_wheel->m_shadows[icon] != NULL) {
				memcpy(&p_wheel->m_shadows[icon]->m_texture, &resources->m_records[textures[icon]], 0x14);
			}
			if (p_wheel->m_icons[icon] != NULL) {
				LegoFloat position[2] = {0.5f, 0.5f};
				LegoFloat size[2] = {0.2f, 0.2f};
				p_wheel->m_icons[icon]->SetPosition(position);
				p_wheel->m_icons[icon]->SetSize(size);
			}
		}
	}
	return count;
}
// STUB: LEGORACERS2 0x004175d0
void __fastcall InitializeSecondaryModeState(void* p_object, void*)
{
	const LegoS32** ranges = (const LegoS32**) ((LegoU8*) p_object + 0xf8);
	for (LegoU32 index = 0; index < 16; ++index) {
		const LegoS32* current = g_secondaryModeRecordTables[index];
		ranges[index * 2] = current;
		while (*current >= 0) {
			current += 3;
		}
		ranges[index * 2 + 1] = current;
	}
	*(LegoU32*) ((LegoU8*) p_object + 0x178) = 0;
}
// STUB: LEGORACERS2 0x00417640
void __fastcall MergeSecondaryModeInput(void* p_object, void*, SecondaryManagerMediaState* p_input)
{
	SecondaryManagerMediaSlot combined = {0, 0, 0, 0};
	if (p_input == NULL) {
		return;
	}
	for (LegoU32 slot = 0; slot < 3; ++slot) {
		if (p_input->IsSlotEnabled(slot)) {
			MergeSecondaryMediaSlot(&combined, NULL, p_input->GetSlot(slot));
		}
	}
	if (combined.m_stateChangeMask != 0) {
		for (LegoU32 control = 0; control < 4; ++control) {
			if ((combined.m_stateChangeMask & (1u << control)) != 0) {
				AddSecondaryModeInput((SecondaryInputSequenceState*) p_object, NULL, control, 1);
				return;
			}
		}
	}
	else if (combined.m_resourceChangeMask != 0) {
		for (LegoU32 control = 0; control < 16; ++control) {
			if ((combined.m_resourceChangeMask & (1u << control)) != 0) {
				AddSecondaryModeInput((SecondaryInputSequenceState*) p_object, NULL, control, 0);
				return;
			}
		}
	}
}

// STUB: LEGORACERS2 0x00417700
static void __fastcall AddSecondaryModeInput(
	SecondaryInputSequenceState* p_state,
	void*,
	LegoU32 p_control,
	LegoU32 p_axis
)
{
	LegoS32 first = p_state->m_first;
	if (first == -1) {
		p_state->m_first = p_state->m_last = 0;
	}
	else {
		++p_state->m_last;
		if (p_state->m_last >= 20) {
			p_state->m_last = 0;
		}
		if (p_state->m_last == first) {
			p_state->m_first = first + 1;
		}
		if (p_state->m_first >= 20) {
			p_state->m_first = 0;
		}
	}
	SecondaryInputHistoryEntry* entry = &p_state->m_history[p_state->m_last];
	entry->m_control = p_control;
	entry->m_axis = p_axis;
	entry->m_time = ReadSignedClockSeconds();
	CheckSecondaryInputSequences(p_state, NULL);
}

// STUB: LEGORACERS2 0x00417790
static void __fastcall CheckSecondaryInputSequences(SecondaryInputSequenceState* p_state, void*)
{
	if (p_state->m_last == -1) {
		return;
	}
	LegoFloat now = ReadSignedClockSeconds();
	for (LegoU32 sequence = 0; sequence < 16; ++sequence) {
		const LegoS32* begin = p_state->m_sequences[sequence][0];
		if (begin == NULL) {
			continue;
		}
		const LegoS32* current = p_state->m_sequences[sequence][1] - 3;
		LegoS32 index = p_state->m_last;
		LegoS32 repeats = current[1];
		LegoBool32 repeating = FALSE;
		LegoFloat previousTime = now;
		for (;;) {
			SecondaryInputHistoryEntry* event = &p_state->m_history[index];
			if (event->m_control != (LegoU32) current[0] || event->m_axis != (LegoU32) current[2] ||
				now - event->m_time > 10.0f) {
				break;
			}
			LegoFloat limit = repeating ? 0.3f : 2.0f;
			if (limit < previousTime - event->m_time) {
				break;
			}
			previousTime = event->m_time;
			--repeats;
			if (repeats <= 0) {
				current -= 3;
				if (current < begin) {
					ApplySecondaryInputSequence(p_state, NULL, sequence);
					return;
				}
				repeats = current[1];
				repeating = FALSE;
			}
			else {
				repeating = TRUE;
			}
			if (index == p_state->m_first) {
				break;
			}
			if (--index < 0) {
				index = 19;
			}
		}
	}
}

// STUB: LEGORACERS2 0x004178a0
static void __fastcall ApplySecondaryInputSequence(SecondaryInputSequenceState* p_state, void*, LegoU32 p_sequence)
{
	if (g_gameControllerSession->m_unk0x04 == 0 ||
		!ApplySecondarySessionCheat(g_gameController.m_unk0x8c, NULL, p_sequence)) {
		GameControllerSelection* selection = (GameControllerSelection*) ((LegoU8*) g_gameControllerSettings + 0x220);
		switch (p_sequence) {
		case 0:
		case 1:
			UnlockAllSecondaryWorlds(selection, NULL);
			if (p_sequence == 0) {
				UnlockAllSecondaryBonusFlags((LegoU32*) ((LegoU8*) g_gameControllerSettings + 0x240), NULL);
			}
			p_state->m_flags |= 1u << (p_sequence & 31);
			break;
		case 2:
			UnlockAllSecondaryRewards(selection, NULL);
			p_state->m_flags |= 4;
			break;
		case 5:
		case 6:
		case 7:
		case 8: {
			LegoU32 world = p_sequence - 4;
			if (((LegoU32) selection->m_values[0] & (1u << world)) != 0) {
				return;
			}
			selection->UnlockWorld(world);
			selection->UnlockDefaults(1, 0);
			p_state->m_flags |= 1u << (p_sequence & 31);
			break;
		}
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			if (p_sequence == 11 && !FUN_004b4a60("fol")) {
				return;
			}
			p_state->m_flags ^= 1u << (p_sequence & 31);
			break;
		default:
			return;
		}
	}
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	AudioSourceRecord* sound = (AudioSourceRecord*) secondary->m_unk0x154[4];
	sound->m_vtable->m_clearSourceFlag(sound, NULL);
	sound->m_vtable->m_resetPlaybackState(sound, NULL);
}

// STUB: LEGORACERS2 0x00419520
void __fastcall UpdateSecondaryPanels(MenuPanelSet* p_panels, void*, LegoU32 p_frameParameter)
{
	MenuManager* menu = (MenuManager*) g_gameController.m_menuManager;
	if (p_panels->m_active == 0 || p_panels->m_resourceMode == 0) {
		return;
	}
	if (menu->m_unk0x19c != (LegoU32) p_panels->m_unk0x20) {
		p_panels->m_unk0x20 = menu->m_unk0x19c;
		LegoS32 panel = 3;
		for (LegoU32 widget = 0; widget < 18; ++widget) {
			MenuWidgetBase* control = menu->m_widgets[widget];
			if (control != NULL && control->m_state != 0 && control->m_unk0x1c != 0) {
				p_panels->m_panels[panel++]->Activate(1, control);
			}
		}
		while (panel < 6) {
			p_panels->m_panels[panel++]->Deactivate();
		}
	}
	if (menu->m_unk0x1a0 != (LegoU32) p_panels->m_unk0x24) {
		p_panels->m_unk0x24 = menu->m_unk0x1a0;
		for (LegoU32 panel = 3; panel < 6; ++panel) {
			p_panels->m_panels[panel]->BeginClosing();
		}
	}
	MenuPanel* choices = p_panels->m_panels[2];
	if (menu->m_unk0x208 == 0) {
		if (choices->m_contextType != -1) {
			choices->Deactivate();
		}
	}
	else if (choices->m_contextType == -1) {
		choices->Activate(2, menu->m_extraChoiceSelector);
	}
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	MenuPanel* message = p_panels->m_panels[1];
	if (((GlobalOverlayRecord**) secondary->m_unk0x8c)[0] == NULL) {
		if (message->m_contextType != -1 && message->m_transition != 1 && message->m_transition != 2) {
			message->BeginClosing();
		}
	}
	else if (message->m_contextType == -1) {
		message->Activate(3, secondary->m_unk0x8c);
	}
	union FrameBits {
		LegoU32 m_word;
		LegoFloat m_float;
	} elapsed;
	elapsed.m_word = p_frameParameter;
	for (LegoU32 panel = 0; panel < 6; ++panel) {
		p_panels->m_panels[panel]->Update(elapsed.m_float);
	}
}

// STUB: LEGORACERS2 0x00422620
void __fastcall CompleteSecondaryMenuAction(MenuManager* p_menu, void*, LegoU32 p_success)
{
	LegoU8* menu = (LegoU8*) p_menu;
	LegoS32 action = *(LegoS32*) (menu + 0x44 + p_menu->m_unk0x198 * 0x14);
	LegoBool32 advance = FALSE;
	LegoBool32 reset = FALSE;
	if (action == 0x2a) {
		switch (p_menu->m_unk0x204) {
		case 1:
		case 3:
		case 4:
			if (GetSecondaryNestedMenuAction(p_menu, NULL) == 0x2e && p_success != 0) {
				advance = TRUE;
			}
			else if (GetSecondaryPendingMenuAction(p_menu, NULL) == 0x2b && p_success != 0) {
				reset = TRUE;
			}
			else {
				PopSecondaryMenuActions(p_menu, NULL, 1, 1);
				p_menu->m_unk0x204 = 0;
				return;
			}
			break;
		case 2:
			if (p_success != 0) {
				advance = TRUE;
			}
			else {
				RestoreMenuManagerAction(p_menu, NULL, 0, 0x19);
			}
			break;
		}
	}
	else if (action == 0x2b) {
		reset = TRUE;
	}
	else if (action == 0x2e) {
		advance = TRUE;
	}
	if (advance) {
		BeginMenuTransition(p_menu, NULL, 1);
		p_menu->m_unk0x1d8 = 4;
	}
	else if (reset) {
		ResetSecondaryMenuAction(p_menu, NULL, 1);
	}
	p_menu->m_unk0x204 = 0;
}

// STUB: LEGORACERS2 0x00422ff0
void __fastcall SetMenuManagerSelectedInput(MenuManager* p_menu, void*, LegoU32 p_input)
{
	*(LegoU32*) ((LegoU8*) p_menu + 0x21c) = p_input;
}

// STUB: LEGORACERS2 0x00423200
static LegoS32 __fastcall GetSecondaryNestedMenuAction(MenuManager* p_menu, void*)
{
	LegoS32 depth = p_menu->m_unk0x198;
	if (depth < 3) {
		return -1;
	}
	LegoU8* stack = (LegoU8*) p_menu + depth * 0x14;
	if (*(LegoU32*) (stack + 0x34) != *(LegoU32*) (stack + 0x20)) {
		return -1;
	}
	return *(LegoS32*) (stack + 0x1c);
}

// STUB: LEGORACERS2 0x004239f0
void* __fastcall ConstructSecondaryTextOverlays(void* p_object, void*)
{
	((GlobalOverlayRecord**) p_object)[0] = NULL;
	((GlobalOverlayRecord**) p_object)[1] = NULL;
	return p_object;
}

// STUB: LEGORACERS2 0x00423a30
void __fastcall InitializeSecondaryMessage(
	void* p_object,
	void*,
	LegoU32 p_stringIndex,
	const LegoFloat* p_position,
	LegoFloat p_maximumWidth
)
{
	GlobalOverlayRecord** records = (GlobalOverlayRecord**) p_object;
	for (LegoU32 index = 0; index < 2; ++index) {
		if (records[index] == NULL) {
			LegoU32 parameters[13];
			parameters[0] = (LegoU32) g_gameController.m_fontSet->m_fonts[1];
			parameters[1] = 0xffffffff;
			// Retail leaves the initial position words untouched.
			parameters[4] = parameters[5] = 0;
			parameters[6] = parameters[7] = 0x3f800000;
			parameters[8] = 0;
			parameters[9] = 11;
			parameters[10] = 0x3f800000;
			parameters[11] = parameters[12] = 0;
			records[index] = g_globalOverlayManager.AllocateRecord(parameters, -1);
		}
		records[index]->InitializeTextByIndex(-1, 0, 0.0f);
	}
	const LegoU16* text = NULL;
	GetLocalizedStringTable()->GetString(p_stringIndex, &text);
	LegoFloat scale = 0.6f;
	if (text == NULL) {
		return;
	}
	LegoFloat position[2] = {p_position[0], p_position[1]};
	records[0]->SetScale(scale);
	records[0]->SetPosition(position, 0xffffffff);
	records[0]->InitializeText(text, 9, 0.0f);
	if (!(p_maximumWidth < records[0]->m_measuredWidth)) {
		return;
	}
	LegoS32 length = 0;
	while (text[length] != 0) {
		++length;
	}
	LegoS32 split = length >> 1;
	for (LegoS32 distance = 0; distance <= split; ++distance) {
		if (text[split + distance] == ' ') {
			split += distance;
			break;
		}
		if (text[split - distance] == ' ') {
			split -= distance;
			break;
		}
	}
	LegoS32 cursor = 0;
	for (LegoS32 character = 0; character <= split; ++character) {
		g_secondaryMessageScratch[cursor++] = text[character];
	}
	g_secondaryMessageScratch[cursor++] = 0;
	LegoU16* secondLine = &g_secondaryMessageScratch[cursor];
	records[0]->SetScale(scale);
	records[0]->InitializeText(g_secondaryMessageScratch, 9, 0.0f);
	if (p_maximumWidth < records[0]->m_measuredWidth) {
		scale = (p_maximumWidth / records[0]->m_measuredWidth) * scale;
	}
	for (character = split + 1; character < length; ++character) {
		g_secondaryMessageScratch[cursor++] = text[character];
	}
	g_secondaryMessageScratch[cursor] = 0;
	records[1]->SetScale(scale);
	records[1]->InitializeText(secondLine, 9, 0.0f);
	if (p_maximumWidth < records[1]->m_measuredWidth) {
		scale = (p_maximumWidth / records[1]->m_measuredWidth) * scale;
	}
	records[0]->SetScale(scale);
	records[0]->SetPosition(position, 0xffffffff);
	records[0]->InitializeText(g_secondaryMessageScratch, 9, 0.0f);
	position[1] = 0.1f * scale + position[1];
	records[1]->SetScale(scale);
	records[1]->SetPosition(position, 0xffffffff);
	records[1]->InitializeText(secondLine, 9, 0.0f);
}

// STUB: LEGORACERS2 0x00423e40
static void InitializeSecondaryPointerSize()
{
	g_secondaryPointerSize[0] = 0.9f;
	g_secondaryPointerSize[1] = 0.1f;
}

// STUB: LEGORACERS2 0x00423e70
static void InitializeSecondaryPointerOrigin()
{
	g_secondaryPointerOrigin[0] = 0.048828125f;
	g_secondaryPointerOrigin[1] = 0.0833333358168602f;
}

// STUB: LEGORACERS2 0x00423ec0
void __fastcall InitializeSecondaryPointer(void* p_object, void*)
{
	LegoU32* words = (LegoU32*) p_object;
	words[0] = ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x78->IsSlotEnabled(3) != 0;
	union PointerParameters {
		LegoU32 m_words[21];
		LegoFloat m_values[21];
	} parameters;
	parameters.m_values[0] = g_secondaryPointerSize[0];
	parameters.m_values[1] = g_secondaryPointerSize[1];
	parameters.m_values[2] = g_secondaryPointerSize[0] + g_secondaryPointerOrigin[0];
	parameters.m_values[3] = g_secondaryPointerSize[1] + g_secondaryPointerOrigin[1];
	parameters.m_values[4] = parameters.m_values[5] = 0.0f;
	parameters.m_values[6] = parameters.m_values[7] = 1.0f;
	parameters.m_words[10] = 0xffffffff;
	memcpy(&parameters.m_words[11], (LegoU8*) g_gameController.m_unk0x90 + 0x1508, 5 * 4);
	for (LegoU32 index = 16; index < 21; ++index) {
		parameters.m_words[index] = 0;
	}
	parameters.m_words[20] = 15;
	// Retail leaves tile size words 8 and 9 untouched.
	GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters.m_words);
	words[1] = (LegoU32) sprite;
	sprite->SetVisible(words[0]);
}

// STUB: LEGORACERS2 0x00423fd0
void __fastcall ReleaseSecondaryPointer(void* p_object, void*)
{
	LegoU32* words = (LegoU32*) p_object;
	if (words[1] != 0) {
		((GlobalOverlaySprite*) words[1])->Release();
		words[1] = 0;
	}
	words[0] = 0;
}

// STUB: LEGORACERS2 0x00423ff0
void __fastcall UpdateSecondaryPointer(void* p_object, void*, const LegoU8* p_input)
{
	LegoU32* words = (LegoU32*) p_object;
	if (words[0] == 0) {
		return;
	}
	GlobalOverlaySprite* pointer = (GlobalOverlaySprite*) words[1];
	LegoFloat position[2];
	position[0] = pointer->m_left + g_menuPointerOffsetX;
	position[1] = pointer->m_top + g_menuPointerOffsetY;
	position[0] += (LegoS8) p_input[2] * 0.25f * 0.007874015718698502f;
	if (position[0] < 0.0f) {
		position[0] = 0.0f;
	}
	else if (position[0] > 1.0f) {
		position[0] = 1.0f;
	}
	position[1] += (LegoS8) p_input[3] * 0.25f * 0.007874015718698502f;
	if (position[1] < 0.0f) {
		position[1] = 0.0f;
	}
	else if (position[1] > 1.0f) {
		position[1] = 1.0f;
	}
	position[0] -= g_menuPointerOffsetX;
	position[1] -= g_menuPointerOffsetY;
	pointer->SetPosition(position);
	LegoU32 previous = words[2];
	words[2] = ((LegoU32) p_input[0] << 8) | p_input[1];
	words[3] = ~previous & words[2];
}

// STUB: LEGORACERS2 0x00426f00
static void InitializeSecondarySaveMessagePosition()
{
	g_secondarySaveMessagePosition[0] = 0.5f;
	g_secondarySaveMessagePosition[1] = 0.5f;
}

// STUB: LEGORACERS2 0x004270a0
void SecondaryManager::Initialize()
{
	GameObject::Initialize();
	DetectKeyboardLanguage();
	EnumerateDisplayModes();
	Application* application = GetApplication();
	LegoBool32 alternateRegion = application->m_vtable->m_value0x7c(application, NULL);
	if (g_secondaryLanguageRecord == NULL) {
		SecondaryContentArchiveManager* archive = GetSecondaryContentArchiveManager();
		g_secondaryLanguageRecord = archive->m_vtable->m_create(
			archive,
			NULL,
			alternateRegion ? "BASLUS-" : "BESLES-",
			alternateRegion ? "20042" : "50443",
			"LEGO_RC2",
			0x9df0,
			0x0c,
			"LEGO RACERS 2"
		);
	}

	m_unk0x78 = (SecondaryManagerMediaState*) AllocateHeapMemory(sizeof(SecondaryManagerMediaState));
	if (m_unk0x78 != NULL) {
		m_unk0x78->m_enabledSlots = 0;
		for (LegoU32 i = 0; i < 4; ++i) {
			SecondaryManagerMediaSlot* slot = &m_unk0x78->m_slots[i];
			slot->m_stateChangeMask = 0;
			slot->m_state = 0;
			slot->m_resourceChangeMask = 0;
			slot->m_resourceId = 0;
		}
		m_unk0x78->m_currentSlot = 0;
	}
	m_unk0x78->InitializeBindings();
	m_unk0x7c = AllocateHeapMemory(0x10);
	if (m_unk0x7c != NULL) {
		memset(m_unk0x7c, 0, 0x10);
	}
	GameObjectBaseNoOp((GameObject*) m_unk0x7c, NULL);

	SecondaryRenderOwnerTemplate ownerTemplate;
	ownerTemplate.m_base.m_flags = 0;
	ownerTemplate.m_base.m_first = CharBuffer24("NoName");
	ownerTemplate.m_base.m_second = CharBuffer24("");
	for (LegoU32 model = 0; model < 4; ++model) {
		ownerTemplate.m_modelIds[model] = 0;
	}
	ownerTemplate.m_orientation.m_x = 0.0f;
	ownerTemplate.m_orientation.m_y = 0.0f;
	ownerTemplate.m_orientation.m_z = 0.0f;
	ownerTemplate.m_orientation.m_w = 1.0f;
	ownerTemplate.m_position.m_x = 0.0f;
	ownerTemplate.m_position.m_y = 0.0f;
	ownerTemplate.m_position.m_z = 0.0f;
	*(LegoU8*) &ownerTemplate.m_flags = 0x80;
	ownerTemplate.m_nearDistance = 0.01f;
	GameObject* owner = (GameObject*) AllocateHeapMemory(0x350);
	if (owner != NULL) {
		owner = ConstructSecondaryRenderOwner(owner, NULL, &ownerTemplate);
	}
	m_unk0x110 = owner;
	owner->m_vtable->m_initialize(owner, NULL);
	owner->m_flags |= 0x400000;
	*(LegoU32*) ((LegoU8*) owner + 0x1cc) = 0;

	for (LegoU32 car = 0; car < 0x2e; ++car) {
		GameControllerItemBlock* configuration =
			(GameControllerItemBlock*) GetMemoryHeap()->Allocate(0x2f4, "Secondaries CCD Store");
		m_secondaryData[car] = configuration;
		CharBuffer128 path(g_defaultPlayerCarPaths[car]);
		LoadDefaultPlayerCar(&path, configuration);
	}
	m_characterSelection = AllocateHeapMemory(0x27b4);
	if (m_characterSelection != NULL) {
		m_characterSelection = ConstructSecondaryCharacterSelection(m_characterSelection, NULL);
	}
	m_carSelection = AllocateHeapMemory(0x27e4);
	if (m_carSelection != NULL) {
		m_carSelection = ConstructSecondaryCarSelection(m_carSelection, NULL);
	}
	void* backgroundStorage = AllocateHeapMemory(sizeof(MenuBackground));
	m_unk0x80 = backgroundStorage == NULL ? NULL : new (backgroundStorage) MenuBackground;
	((MenuBackground*) m_unk0x80)->Initialize();
	m_unk0x84 = AllocateHeapMemory(sizeof(MenuPanelSet));
	if (m_unk0x84 != NULL) {
		memset(m_unk0x84, 0, 0x18);
	}
	((MenuPanelSet*) m_unk0x84)->Initialize();
	m_unk0x88 = AllocateHeapMemory(0xac);
	if (m_unk0x88 != NULL) {
		m_unk0x88 = ConstructSecondaryAuxiliary(m_unk0x88, NULL);
	}
	InitializeSecondaryAuxiliary(m_unk0x88, NULL);
	m_unk0x8c = AllocateHeapMemory(8);
	if (m_unk0x8c != NULL) {
		m_unk0x8c = ConstructSecondaryTextOverlays(m_unk0x8c, NULL);
	}
	GameObjectBaseNoOp((GameObject*) m_unk0x8c, NULL);
	m_unk0x17c = AllocateHeapMemory(0x17c);
	if (m_unk0x17c != NULL) {
		ResetSecondaryManagerModeStateAtEntry(m_unk0x17c, NULL);
	}
	InitializeSecondaryModeState(m_unk0x17c, NULL);
	LoadSavedGame();
}

// STUB: LEGORACERS2 0x00427480
void SecondaryManager::Remove()
{
	RemoveAndDestroySecondaryObject(m_unk0x74);
	m_unk0x74 = NULL;

	for (LegoS32 moreIndex = 2; moreIndex >= 0; --moreIndex) {
		RemoveAndDestroySecondaryObject(m_moreResourcePointers[moreIndex]);
		m_moreResourcePointers[moreIndex] = NULL;
	}
	RemoveAndDestroySecondaryObject(m_unk0x134);
	m_unk0x134 = NULL;
	RemoveAndDestroySecondaryObject((void*) m_unk0x12c);
	m_unk0x12c = 0;
	for (LegoS32 resourceIndex = 2; resourceIndex >= 0; --resourceIndex) {
		RemoveAndDestroySecondaryObject(m_resourcePointers[resourceIndex]);
		m_resourcePointers[resourceIndex] = NULL;
	}

	if (m_unk0x17c != NULL) {
		FreeHeapMemory(m_unk0x17c);
		m_unk0x17c = NULL;
	}

	ReleaseSecondaryObject8c(m_unk0x8c, NULL);
	if (m_unk0x8c != NULL) {
		GameObjectBaseNoOp((GameObject*) m_unk0x8c, NULL);
		FreeHeapMemory(m_unk0x8c);
		m_unk0x8c = NULL;
	}
	ReleaseSecondaryObject88(m_unk0x88, NULL);
	if (m_unk0x88 != NULL) {
		FreeHeapMemory(m_unk0x88);
		m_unk0x88 = NULL;
	}
	((MenuPanelSet*) m_unk0x84)->Release();
	if (m_unk0x84 != NULL) {
		GameObjectBaseNoOp((GameObject*) m_unk0x84, NULL);
		FreeHeapMemory(m_unk0x84);
		m_unk0x84 = NULL;
	}
	((MenuBackground*) m_unk0x80)->~MenuBackground();
	if (m_unk0x80 != NULL) {
		FreeHeapMemory(m_unk0x80);
		m_unk0x80 = NULL;
	}
	void** at6c = &m_carSelection;
	if (*at6c != NULL) {
		ReleaseSecondaryObject6c(*at6c, NULL);
		FreeHeapMemory(*at6c);
	}
	void** at70 = &m_characterSelection;
	if (*at70 != NULL) {
		ReleaseSecondaryObject70(*at70, NULL);
		FreeHeapMemory(*at70);
	}

	for (LegoS32 dataIndex = 0x2d; dataIndex >= 0; --dataIndex) {
		if (m_secondaryData[dataIndex] != NULL) {
			GetMemoryHeap()->Free(m_secondaryData[dataIndex]);
			m_secondaryData[dataIndex] = NULL;
		}
	}
	GameObject* renderOwner = (GameObject*) m_unk0x110;
	renderOwner->m_vtable->m_remove(renderOwner, NULL);
	if (renderOwner != NULL) {
		renderOwner->m_vtable->m_destroy(renderOwner, NULL, 1);
	}

	if (m_unk0x7c != NULL) {
		GameObjectBaseNoOp((GameObject*) m_unk0x7c, NULL);
		FreeHeapMemory(m_unk0x7c);
		m_unk0x7c = NULL;
	}
	if (m_unk0x78 != NULL) {
		GameObjectBaseNoOp((GameObject*) m_unk0x78, NULL);
		FreeHeapMemory(m_unk0x78);
		m_unk0x78 = NULL;
	}
	if (g_secondaryLanguageRecord != NULL) {
		SecondaryContentArchiveManager* archive = GetSecondaryContentArchiveManager();
		archive->m_vtable->m_release(archive, NULL, g_secondaryLanguageRecord);
		g_secondaryLanguageRecord = NULL;
	}
	GameObject::Remove();
}

// STUB: LEGORACERS2 0x00427a00
LegoU32 SecondaryManager::Update(LegoU32 p_frameParameter)
{
	if (m_state == 9) {
		return 0;
	}
	LegoU32 handled = 1;
	m_unk0x78->Refresh(0xffffffff);
	UpdateSecondaryPointer(m_unk0x7c, NULL, (const LegoU8*) &m_unk0x78->m_currentSlot);
	if (m_unk0x78->HasActiveSlot()) {
		ResetSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
	}
	switch (m_state) {
	case 0:
		if (m_startupPath.m_data[0] != '\0') {
			m_state = 8;
		}
		else {
			LoadMenuResources(-1);
			if (*(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x5c) == 3) {
				LegoU32 mode = 1;
				if (m_requestedState == 0) {
					Application* application = GetApplication();
					LegoS32 language;
					if (application->m_vtable->m_value0x7c(application, NULL) == 0 &&
						!AccessGameLanguagePreference(0, &language)) {
						mode = 0x0b;
					}
					m_requestedState = 1;
				}
				EnterSecondaryManagerMode(this, NULL, mode);
				if (mode == 0x2c) {
					EnterSpecialMode();
				}
			}
			else {
				EnterMainMenu();
			}
		}
		break;
	case 1:
		if (PrepareSplash(1)) {
			g_gameController.PlayStartupMovies();
			RestoreStartupRendering();
			m_state = 9;
			BeginStartup();
		}
		break;
	case 2:
		UpdateMenuMode(0, p_frameParameter);
		break;
	case 3:
	case 4:
	case 5:
		UpdateSpecialMode(p_frameParameter);
		break;
	case 6: {
		LegoS32 result = UpdateSecondaryCharacterSelection(m_characterSelection, NULL, p_frameParameter, m_unk0x78);
		if (result != 0) {
			FinishSecondaryCharacterSelection(m_characterSelection, NULL);
			if (g_gameControllerSession->m_unk0x04 != 0 && result != -1) {
				BeginCarSelection(
					~*(LegoU32*) ((LegoU8*) g_gameController.m_menuManager + 0x218) & 1,
					g_gameControllerSession->m_selectedMode
				);
			}
			else {
				EnterMainMenu();
			}
		}
		break;
	}
	case 7: {
		LegoS32 result = UpdateSecondaryCarSelection(m_carSelection, NULL, p_frameParameter, m_unk0x78);
		if (result == 1) {
			FinishSecondaryCarSelection(m_carSelection, NULL);
			if (g_gameControllerSession->m_unk0x04 != 0) {
				MenuManager* menu = (MenuManager*) g_gameController.m_menuManager;
				if (g_gameControllerSession->m_unk0x00 == 1 && (~*(LegoU32*) ((LegoU8*) menu + 0x218) & 1) == 0) {
					if (g_gameControllerSession->m_selectedMode == 0) {
						SetMenuManagerSelectedInput(menu, NULL, 2);
						EnterSecondaryManagerMode(this, NULL, 0x1f);
					}
					else {
						BeginCarSelection(1, g_gameControllerSession->m_selectedMode);
					}
				}
				else {
					FinishSecondarySessionSelection(g_gameController.m_unk0x8c, NULL);
					PopMenuManagerAction(menu, NULL);
					ReleaseMenuResources(1);
					m_state = 9;
				}
			}
			else {
				EnterMainMenu();
			}
		}
		else if (result == 2) {
			FinishSecondaryCarSelection(m_carSelection, NULL);
			if (g_gameControllerSession->m_unk0x04 != 0) {
				BeginCharacterSelection(~*(LegoU32*) ((LegoU8*) g_gameController.m_menuManager + 0x218) & 1);
			}
			else {
				EnterMainMenu();
			}
		}
		break;
	}
	case 8: {
		AudioService* audio = GetAudioService(TRUE);
		if (audio->m_vtable->m_getStatus(audio, NULL) == 0) {
			audio->m_vtable->m_shutdown(audio, NULL);
		}
		MoviePlayer* player = GetMoviePlayer();
		player->m_vtable->m_play(player, NULL, m_startupPath.m_data, TRUE);
		audio->m_vtable->m_restart(audio, NULL);
		GetInitializedSimulationScheduler()->Reset(0);
		if (g_gameControllerSession->m_unk0x04 != 0) {
			m_startupPath.m_data[0] = '\0';
			GameControllerItemBlock* car =
				GetSecondarySessionCar(g_gameController.m_unk0x8c, NULL, 0, g_gameControllerSession->m_selectedMode);
			if (*(LegoU32*) car == 0) {
				LoadMenuResources(g_gameControllerSession->m_selectedMode);
				BeginCarSelection(0, g_gameControllerSession->m_selectedMode);
			}
			else {
				FinishSecondarySessionSelection(g_gameController.m_unk0x8c, NULL);
				m_state = 9;
			}
		}
		break;
	}
	default:
		handled = 0;
	}
	union FrameBits {
		LegoU32 m_word;
		LegoFloat m_float;
	} elapsed;
	elapsed.m_word = p_frameParameter;
	((MenuBackground*) m_unk0x80)->Update(elapsed.m_float);
	return handled;
}

// STUB: LEGORACERS2 0x00427df0
void SecondaryManager::EnterMainMenu()
{
	m_state = 2;
	RestoreMenuManagerAction((MenuManager*) g_gameController.m_menuManager, NULL, 0, -1);
	((MenuBackground*) m_unk0x80)->SetMode(2);
	((MenuPanelSet*) m_unk0x84)->SetActive(1);
	SetSecondaryManagerAuxiliaryEnabled(m_unk0x88, NULL, 1);
}

void __fastcall MergeSecondaryModeInput(void*, void*, SecondaryManagerMediaState*);
void __fastcall UpdateSecondaryPanels(MenuPanelSet*, void*, LegoU32);
void __fastcall UpdateSecondaryAuxiliary(void*, void*, LegoU32);
void __fastcall SetFoyerManagerMenuMode(GameObject*, void*);
void FUN_0042a860();
void __fastcall RequestSecondaryPauseLeave(GlobalPauseController*, void*, LegoU32);
struct SecondaryNetworkDispatch {
	GameObjectDispatchView m_base;
	undefined m_unk0x50[8];
	void(__fastcall* m_action0x58)(GameObject*, void*);
	void(__fastcall* m_action0x5c)(GameObject*, void*, LegoU32);
};

// STUB: LEGORACERS2 0x00427eb0
void SecondaryManager::UpdateMenuMode(LegoU32 p_mode, LegoU32 p_frameParameter)
{
	SecondaryManagerMediaState* input = NULL;
	if (p_mode == 0) {
		if (g_unk0x00679fe0 != 0) {
			m_unk0x78->Refresh(0xffffffff);
		}
		input = m_unk0x78;
		if (input != NULL) {
			MergeSecondaryModeInput(m_unk0x17c, NULL, input);
		}
	}
	MenuManager* menu = (MenuManager*) g_gameController.m_menuManager;
	LegoBool32 finish = FALSE;
	LegoU32 result = UpdateSecondaryMenuManager(menu, NULL, p_frameParameter, 0, p_mode, input);
	switch (result) {
	case 1:
		ReturnToController(0);
		break;
	case 2:
		BeginCharacterSelection(~menu->m_unk0x218 & 1);
		break;
	case 3:
		BeginCarSelection(~menu->m_unk0x218 & 1, g_gameControllerSession->m_selectedMode);
		break;
	case 4:
		if (m_unk0x64 != 0) {
			ReleaseMenuResources(1);
		}
		m_state = 9;
		FinishSecondarySessionSelection(g_gameController.m_unk0x8c, NULL);
		break;
	case 5:
		FUN_0042a860();
		break;
	case 6:
		EnterSpecialMode();
		break;
	case 7:
		if (g_unk0x00679fe0 != 0) {
			RequestSecondaryPauseLeave(&g_globalPauseController, NULL, 1);
		}
		finish = TRUE;
		break;
	case 8:
		if (g_unk0x00679fe0 != 0) {
			RequestSecondaryPauseLeave(&g_globalPauseController, NULL, 0);
		}
		if (*(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x5c) == 3) {
			if (g_gameControllerSession->m_mode == 3) {
				RestartSecondaryBattle(g_gameController.m_battleManager, NULL);
			}
			else if (g_gameControllerSession->m_mode == 4) {
				GameObject* target = g_gameController.m_unk0xbc;
				((const SecondaryNetworkDispatch*) target->m_vtable)->m_action0x58(target, NULL);
			}
			else {
				RestartSecondaryRace(g_gameController.m_raceManager, NULL, 0);
			}
		}
		else {
			SendSecondaryNetworkContinue(g_gameController.m_networkApp, NULL);
		}
		finish = TRUE;
		break;
	case 9:
		if (g_unk0x00679fe0 != 0) {
			RequestSecondaryPauseLeave(&g_globalPauseController, NULL, 0);
		}
		if (g_gameControllerSession->m_unk0x04 == 0) {
			if (g_gameControllerSession->m_mode == 2) {
				if (*(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x5c) == 3) {
					LegoU8* session = (LegoU8*) g_gameControllerSession;
					LegoS32 entry = *(LegoS32*) (session + 0x1a70);
					StartSecondarySessionRace(
						g_gameController.m_raceManager,
						NULL,
						*(LegoU32*) (session + 0x1a78 + entry * 4),
						*(LegoU32*) (session + 0x1aa0 + entry * 4),
						*(LegoU32*) (session + 0x1ac8 + entry * 4)
					);
				}
				else {
					SendSecondaryNetworkNextRace(g_gameController.m_networkApp, NULL);
				}
			}
		}
		else if (g_gameController.m_state == 4) {
			FinishSecondaryRace(g_gameController.m_raceManager, NULL, 0);
		}
		else if (g_gameController.m_state == 6) {
			GameObject* target = g_gameController.m_unk0xbc;
			((const SecondaryNetworkDispatch*) target->m_vtable)->m_action0x5c(target, NULL, 0);
		}
		finish = TRUE;
		break;
	case 10:
		*(LegoU32*) ((LegoU8*) g_gameController.m_foyerManager + 0xe8) = 1;
		if (g_gameController.m_state == 3) {
			SetFoyerManagerMenuMode(g_gameController.m_foyerManager, NULL);
		}
		else if (g_gameController.m_state == 4) {
			FinishSecondaryRace(g_gameController.m_raceManager, NULL, 0);
		}
		else if (g_gameController.m_state == 6) {
			GameObject* target = g_gameController.m_unk0xbc;
			((const SecondaryNetworkDispatch*) target->m_vtable)->m_action0x5c(target, NULL, 0);
		}
		finish = TRUE;
		break;
	case 11:
		RequestSecondaryNetworkExit(g_gameController.m_unk0xbc, NULL);
		finish = TRUE;
		break;
	case 12:
		if (g_unk0x00679fe0 != 0) {
			RequestSecondaryPauseLeave(&g_globalPauseController, NULL, 0);
		}
		RestartSecondaryRace(g_gameController.m_raceManager, NULL, 1);
		finish = TRUE;
		break;
	case 13:
		if (g_unk0x00679fe0 != 0) {
			RequestSecondaryPauseLeave(&g_globalPauseController, NULL, 0);
		}
		if (*(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x5c) == 3) {
			if (g_gameController.m_state == 5) {
				FinishSecondaryBattle(g_gameController.m_battleManager, NULL);
			}
			else if (g_gameController.m_state == 4) {
				FinishSecondaryRace(g_gameController.m_raceManager, NULL, 1);
			}
			else if (g_gameController.m_state == 3) {
				SetFoyerManagerMenuMode(g_gameController.m_foyerManager, NULL);
			}
			else if (g_gameController.m_state == 6) {
				GameObject* target = g_gameController.m_unk0xbc;
				((const SecondaryNetworkDispatch*) target->m_vtable)->m_action0x5c(target, NULL, 1);
			}
		}
		else {
			SendSecondaryNetworkRestart(g_gameController.m_networkApp, NULL);
		}
		finish = TRUE;
		break;
	case 14:
		finish = TRUE;
		break;
	}
	if (finish) {
		m_state = 9;
	}
	if (m_state != 2 && m_state != 3 && m_state != 4 && m_state != 5) {
		((MenuBackground*) m_unk0x80)->SetMode(0);
		((MenuPanelSet*) m_unk0x84)->SetActive(0);
		SetSecondaryManagerAuxiliaryEnabled(m_unk0x88, NULL, 0);
	}
	UpdateSecondaryPanels((MenuPanelSet*) m_unk0x84, NULL, p_frameParameter);
	UpdateSecondaryAuxiliary(m_unk0x88, NULL, p_frameParameter);
}

void __fastcall InitializeSecondaryMessage(void*, void*, LegoU32, const LegoFloat*, LegoFloat);
void __fastcall CompleteSecondaryMenuAction(MenuManager*, void*, LegoU32);

// STUB: LEGORACERS2 0x00428270
void SecondaryManager::UpdateSpecialMode(LegoU32 p_frameParameter)
{
	if (m_unk0x5c == 1) {
		LegoU32 message;
		if (m_state == 3) {
			m_saveLoadResult = LoadSavedGame();
			m_unk0x5c = 10;
			message = 0x116 - (m_saveLoadResult != 0);
		}
		else {
			PrepareSavedGame();
			CharBuffer128 path;
			BuildUserDataPath(path.m_data, FALSE);
			strcat(path.m_data, "\\");
			strcat(path.m_data, g_secondarySaveDirectory);
			CreateDirectory(path.m_data, NULL);
			strcat(path.m_data, "\\");
			strcat(path.m_data, g_secondarySaveFilename);
			WinFile file(path.m_data, 1, NULL);
			m_saveLoadResult = file.WriteExact(&g_secondarySavedGame, sizeof(g_secondarySavedGame));
			file.CloseManaged();
			Application* first = GetApplication();
			Application* second = GetApplication();
			ApplicationDisplaySettings* display = first->m_vtable->m_displaySettings(first, NULL);
			second->m_vtable->m_setDisplaySettings(second, NULL, display, TRUE);
			m_unk0x5c = 10;
			message = 0x119 - (m_saveLoadResult != 0);
		}
		InitializeSecondaryMessage(m_unk0x8c, NULL, message, g_secondarySaveMessagePosition, 0.8f);
	}
	else if (m_unk0x5c == 10) {
		GlobalOverlayRecord** records = (GlobalOverlayRecord**) &m_unk0x258;
		for (LegoU32 index = 0; index < 3; ++index) {
			if (records[index] != NULL) {
				records[index]->Release();
				records[index] = NULL;
			}
		}
		if (m_state == 5) {
			ReleaseSecondaryObject8c(m_unk0x8c, NULL);
			EnterSecondaryManagerMode(this, NULL, 1);
		}
		else {
			if (g_unk0x00679fe0 != 0) {
				m_unk0x78->Refresh(0xffffffff);
			}
			if (m_unk0x23c == 0 && !m_unk0x78->HasActiveSlot()) {
				UpdateSecondaryPanels((MenuPanelSet*) m_unk0x84, NULL, p_frameParameter);
				return;
			}
			if (m_saveLoadResult != 0) {
				if (m_state == 3) {
					if (g_gameControllerSession->m_unk0x04 == 0) {
						ApplySavedSettings();
					}
					else {
						ApplySecondarySessionData(g_gameController.m_unk0x8c, NULL);
					}
				}
				else {
					LegoS32 language = g_gameControllerSettings->m_language;
					AccessGameLanguagePreference(1, &language);
				}
			}
			CompleteSecondaryMenuAction((MenuManager*) g_gameController.m_menuManager, NULL, m_saveLoadResult);
			ReleaseSecondaryObject8c(m_unk0x8c, NULL);
			m_state = 2;
		}
	}
	UpdateSecondaryPanels((MenuPanelSet*) m_unk0x84, NULL, p_frameParameter);
}

// STUB: LEGORACERS2 0x00428540
void SecondaryManager::PrepareSavedGame()
{
	memcpy(&g_secondarySavedGame, g_gameControllerSettings, 0x7fb8);
	memcpy(g_secondarySavedGame.m_session, (LegoU8*) g_gameController.m_unk0x8c + 0x1e34, 0x1e34);
	g_secondarySavedGame.m_settingsChecksum =
		ComputeSecondarySaveChecksum(this, NULL, (const LegoU8*) &g_secondarySavedGame, 0x7fb8);
	g_secondarySavedGame.m_sessionChecksum = ComputeSecondarySaveChecksum(
		this,
		NULL,
		g_secondarySavedGame.m_session,
		sizeof(g_secondarySavedGame.m_session)
	);
}

// STUB: LEGORACERS2 0x004285a0
LegoBool32 SecondaryManager::ValidateSavedGame()
{
	if (g_secondarySavedGame.m_cookie != 0xf00cf00c) {
		return FALSE;
	}
	if (g_secondarySavedGame.m_settingsChecksum !=
		ComputeSecondarySaveChecksum(this, NULL, (const LegoU8*) &g_secondarySavedGame, 0x7fb8)) {
		return FALSE;
	}
	memcpy(g_gameControllerSettings, &g_secondarySavedGame, 0x7fb8);
	if (g_secondarySavedGame.m_sessionChecksum != ComputeSecondarySaveChecksum(
													  this,
													  NULL,
													  g_secondarySavedGame.m_session,
													  sizeof(g_secondarySavedGame.m_session)
												  )) {
		return FALSE;
	}
	return LoadSecondarySessionData(g_gameController.m_unk0x8c, NULL, g_secondarySavedGame.m_session);
}

// STUB: LEGORACERS2 0x00428620
static LegoU32 __fastcall ComputeSecondarySaveChecksum(void*, void*, const LegoU8* p_data, LegoS32 p_bytes)
{
	LegoU32 sum = 0;
	LegoS32 count = p_bytes - 1;
	while (count > 0) {
		sum += (LegoU32) p_data[0] * p_data[1];
		++p_data;
		--count;
	}
	return sum;
}

// STUB: LEGORACERS2 0x004293d0
void SecondaryManager::EnterSpecialMode()
{
	m_state = 5;
	m_unk0x5c = 1;
	m_unk0x250 = 0;
}

// STUB: LEGORACERS2 0x00429400
LegoBool32 SecondaryManager::LoadSavedGame()
{
	GetFileManager()->SetFlags(1, TRUE);
	LegoBool32 loaded = FALSE;
	CharBuffer128 path;
	BuildUserDataPath(path.m_data, FALSE);
	strcat(path.m_data, "\\");
	strcat(path.m_data, g_secondarySaveDirectory);
	strcat(path.m_data, "\\");
	strcat(path.m_data, g_secondarySaveFilename);
	if (ManagedPathExists(path.m_data)) {
		WinFile file(path.m_data, 0, NULL);
		if (file.GetManagedSize() == sizeof(g_secondarySavedGame)) {
			LegoBool32 read = file.Read(&g_secondarySavedGame, sizeof(g_secondarySavedGame), 1);
			file.CloseManaged();
			if (read) {
				loaded = ValidateSavedGame();
			}
		}
	}
	GetFileManager()->SetFlags(1, FALSE);
	if (loaded) {
		ApplySavedSettings();
	}
	return loaded;
}

// STUB: LEGORACERS2 0x00429580
void SecondaryManager::ApplySavedSettings()
{
	for (LegoS32 player = 0; player < 2; ++player) {
		g_gameControllerSettings->SetPlayerName(&g_gameControllerSettings->m_names[player], player);
	}
	ApplySavedAudioLevels();
	ApplySavedAudioQuality();
	LegoU32* settings = (LegoU32*) g_gameControllerSettings;
	ApplyMenuGraphicsQuality(g_displayRenderer, NULL, settings[0x20c / 4], settings[0x210 / 4], settings[0x214 / 4]);
	LegoS32 language;
	if (AccessGameLanguagePreference(0, &language)) {
		g_gameControllerSettings->m_language = language;
	}
	ApplyMenuLanguage(this, NULL, g_gameControllerSettings->m_language);
}

// STUB: LEGORACERS2 0x00429730
void SecondaryManager::BeginCharacterSelection(LegoU32 p_player)
{
	BeginSecondaryCharacterSelection(m_characterSelection, NULL, p_player);
	m_state = 6;
}

// STUB: LEGORACERS2 0x00429750
void SecondaryManager::BeginCarSelection(LegoU32 p_player, LegoS32 p_carSet)
{
	LegoU32 enableCharacter = 1;
	if (g_gameControllerSession->m_unk0x04 != 0 && p_carSet != 0) {
		enableCharacter = 0;
	}
	BeginSecondaryCarSelection(m_carSelection, NULL, p_player, p_carSet, enableCharacter);
	m_state = 7;
}

void __fastcall SetSecondaryOverlayMode(GlobalOverlayManager*, void*, LegoU32);

void __fastcall InitializeSecondaryAuxiliaryModel(void*, void*);
void __fastcall InitializeSecondaryVoiceResources(void*, void*, LegoS32);
void __fastcall LoadSecondaryControllerPreview(void*, void*, LegoU32);
void __fastcall LoadSecondaryAuxiliaryResources(void*, void*);
void __fastcall InitializeSecondaryPointer(void*, void*);
void __fastcall RefreshSecondaryResolution(SecondaryManager*, void*);

// STUB: LEGORACERS2 0x004297d0
void SecondaryManager::LoadMenuResources(LegoS32 p_carSet)
{
	m_unk0x64 = 1;
	m_loadedCarSet = p_carSet;
	const RendererMainDispatch* rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	rendererDispatch->m_setTextureQuality(g_displayRenderer, NULL, ((LegoU32*) g_gameControllerSettings)[0x21c / 4]);
	rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 0);
	SetSecondaryOverlayMode(&g_globalOverlayManager, NULL, 2);
	void* progress = g_gameController.m_unk0xd4;
	InitializeControllerSelectionProgress(progress, NULL, 1);
	rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	if (!rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 2)) {
		FUN_0042a860();
	}
	UpdateControllerSelectionProgress(progress, NULL, 1);
	rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 1);
	UpdateGameObjectRenderGeometry(m_unk0x110, NULL);
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_addListener(audio, NULL, (const LegoU32*) ((LegoU8*) m_unk0x110 + 0x10), (LegoU32) m_unk0x110);
	audio = GetAudioService(TRUE);
	audio->m_vtable->m_setParameters(audio, NULL, 0.0f, 0.0f);
	if (g_gameController.m_reverbManager != NULL) {
		((ReverbManager*) g_gameController.m_reverbManager)->Reset();
	}
	SetPhysicalLifecycleStep(0.0f);
	LoadControllerSceneResources(&g_gameController, NULL, 1);
	SetControllerSelectionProgressValue(progress, NULL, 0.05f);
	UpdateControllerSelectionProgress(progress, NULL, 0);
	InitializeSecondaryAuxiliaryModel(g_gameController.m_unk0x90, NULL);
	SetControllerSelectionProgressValue(progress, NULL, 0.1f);
	UpdateControllerSelectionProgress(progress, NULL, 0);
	InitializeSecondaryCharacterResources(g_gameController.m_bodypartManager, NULL);
	SetControllerSelectionProgressValue(progress, NULL, 0.2f);
	UpdateControllerSelectionProgress(progress, NULL, 0);
	InitializeSecondaryVoiceResources(g_gameController.m_npcTextManager, NULL, -1);
	SetControllerSelectionProgressValue(progress, NULL, 0.3f);
	UpdateControllerSelectionProgress(progress, NULL, 0);

	SecondaryRenderOwnerTemplate ownerTemplate;
	ownerTemplate.m_base.m_flags = 0;
	ownerTemplate.m_base.m_first = CharBuffer24("NoName");
	ownerTemplate.m_base.m_second = CharBuffer24("");
	for (LegoU32 index = 0; index < 4; ++index) {
		ownerTemplate.m_modelIds[index] = 0;
	}
	ownerTemplate.m_orientation.m_x = 0.0f;
	ownerTemplate.m_orientation.m_y = 0.0f;
	ownerTemplate.m_orientation.m_z = 0.0f;
	ownerTemplate.m_orientation.m_w = 1.0f;
	ownerTemplate.m_position.m_x = 0.0f;
	ownerTemplate.m_position.m_y = 0.0f;
	ownerTemplate.m_position.m_z = 0.0f;
	*(LegoU8*) &ownerTemplate.m_flags = 0x80;
	ownerTemplate.m_nearDistance = 0.01f;
	for (index = 0; index < 3; ++index) {
		GameObject* object = (GameObject*) AllocateHeapMemory(0x350);
		if (object != NULL) {
			object = ConstructSecondaryRenderOwner(object, NULL, &ownerTemplate);
		}
		m_resourcePointers[index] = object;
		object->m_vtable->m_initialize(object, NULL);
		object->m_flags |= 0x400000;
		*(LegoS32*) ((LegoU8*) object + 0x1c8) = -1;
		*(LegoU32*) ((LegoU8*) object + 0x1c0) = 0;
		*(LegoFloat*) ((LegoU8*) object + 0x318) = 0.0f;
		*(LegoFloat*) ((LegoU8*) object + 0x1ac) = 128.0f;
	}
	SetControllerSelectionProgressValue(progress, NULL, 0.32f);
	UpdateControllerSelectionProgress(progress, NULL, 0);
	CharacterVisualTemplate visualTemplate;
	visualTemplate.Initialize(0, 5, 0, 0, 0, &g_secondaryDefaultBodypartName);
	void* visualStorage = AllocateHeapMemory(sizeof(CharacterVisual));
	CharacterVisual* visual = visualStorage == NULL ? NULL : new (visualStorage) CharacterVisual(&visualTemplate);
	m_unk0x12c = (LegoU32) visual;
	visual->m_object.m_vtable->m_initialize(&visual->m_object, NULL);
	SetControllerSelectionProgressValue(progress, NULL, 0.33f);
	UpdateControllerSelectionProgress(progress, NULL, 0);

	SecondaryAmbientLightTemplate ambientTemplate;
	ambientTemplate.m_base.m_flags = 0;
	ambientTemplate.m_base.m_first = CharBuffer24("NoName");
	ambientTemplate.m_base.m_second = CharBuffer24("");
	ambientTemplate.m_color = 0xff969696;
	GameObject* ambient = ConstructSecondaryAmbientLight(&ambientTemplate);
	m_unk0x134 = ambient;
	ambient->m_vtable->m_initialize(ambient, NULL);
	SecondaryDirectionalLightTemplate lightTemplate;
	lightTemplate.m_base = g_secondaryLightBaseTemplate;
	lightTemplate.m_ambient.r = lightTemplate.m_ambient.g = lightTemplate.m_ambient.b = 0.5f;
	lightTemplate.m_ambient.a = 1.0f;
	lightTemplate.m_diffuse.r = lightTemplate.m_diffuse.g = lightTemplate.m_diffuse.b = lightTemplate.m_diffuse.a =
		1.0f;
	lightTemplate.m_specular.r = lightTemplate.m_specular.g = lightTemplate.m_specular.b = 0.5f;
	lightTemplate.m_specular.a = 1.0f;
	lightTemplate.m_direction.m_x = 0.0f;
	lightTemplate.m_direction.m_y = -1.0f;
	lightTemplate.m_direction.m_z = 0.0f;
	for (index = 0; index < 3; ++index) {
		GameObject* light = ConstructSecondaryDirectionalLight(&lightTemplate);
		m_moreResourcePointers[index] = light;
		light->m_vtable->m_initialize(light, NULL);
		((GameDirectionalLight*) light)->m_properties.SetEnabled(0);
	}
	SetControllerSelectionProgressValue(progress, NULL, 0.34f);
	UpdateControllerSelectionProgress(progress, NULL, 0);

	CharBuffer128 emptyPath("");
	const CharBuffer128* genericPath;
	const CharBuffer128* worldPath;
	if (p_carSet < 0) {
		genericPath = &g_secondaryAllBrickPath;
		worldPath = &emptyPath;
	}
	else {
		genericPath = &g_secondaryGenericBrickPath;
		worldPath = &g_secondaryWorldBrickPaths[p_carSet];
	}
	GameControllerItemBlock* savedCars[40];
	LegoS32 savedCarCount = 0;
	for (LegoS32 world = 0; world < 5; ++world) {
		if (world == p_carSet || p_carSet == -1) {
			for (LegoS32 car = 0; car < 8; ++car) {
				savedCars[savedCarCount++] = (GameControllerItemBlock*) ((LegoU8*) g_gameControllerSettings + 0x94c +
																		 world * 0x17a0 + car * 0x2f4);
			}
		}
	}
	SecondaryCarResourceTemplate carTemplate;
	carTemplate.m_base.m_flags = 2;
	carTemplate.m_base.m_first = CharBuffer24("NoName");
	carTemplate.m_base.m_second = CharBuffer24("");
	carTemplate.m_allBricksPath = g_secondaryAllBrickPath;
	carTemplate.m_genericBricksPath = *genericPath;
	carTemplate.m_worldBricksPath = *worldPath;
	carTemplate.m_defaultCars = m_secondaryData;
	carTemplate.m_defaultCarCount = 0x2e;
	carTemplate.m_savedCars = savedCars;
	carTemplate.m_savedCarCount = savedCarCount;
	carTemplate.m_loadMode = 1;
	carTemplate.m_createDisplay = 1;
	carTemplate.m_progressStart = 0.34f;
	carTemplate.m_progressEnd = 0.7f;
	GameObject* carResources = ConstructSecondaryCarResources(&carTemplate);
	m_unk0x74 = carResources;
	carResources->m_vtable->m_initialize(carResources, NULL);
	LoadSecondaryCarSelector(m_carSelection, NULL, g_gameController.m_fontSet->m_fonts[1], p_carSet);
	SetControllerSelectionProgressValue(progress, NULL, 0.8f);
	UpdateControllerSelectionProgress(progress, NULL, 0);
	if (p_carSet == -1) {
		LoadSecondaryCharacterSelector(m_characterSelection, NULL, g_gameController.m_fontSet->m_fonts[1]);
		SetControllerSelectionProgressValue(progress, NULL, 0.9f);
		UpdateControllerSelectionProgress(progress, NULL, 0);
		LoadSecondaryControllerPreview(g_gameController.m_unk0x90, NULL, 0);
		SetControllerSelectionProgressValue(progress, NULL, 0.95f);
		UpdateControllerSelectionProgress(progress, NULL, 0);
		((MenuBackground*) m_unk0x80)->Load();
		LoadSecondaryAuxiliaryResources(m_unk0x88, NULL);
		LoadLocalizedMenuResources();
	}
	audio = GetAudioService(TRUE);
	audio->m_vtable->m_unk0x38(audio, NULL);
	SetControllerSelectionProgressValue(progress, NULL, 1.0f);
	UpdateControllerSelectionProgress(progress, NULL, 0);
	FinalizeControllerSelectionProgress(g_gameController.m_unk0xd4, NULL);
	rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	if (!rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 2)) {
		FUN_0042a860();
	}
	RefreshSecondaryResolution(this, NULL);
	rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	((LegoU32*) g_gameControllerSettings)[0x21c / 4] = rendererDispatch->m_getTextureQuality(g_displayRenderer, NULL);
	InitializeSecondaryPointer(m_unk0x7c, NULL);
	audio = GetAudioService(TRUE);
	audio->m_vtable->m_setMusicEnabled(audio, NULL, 0);
	StartControllerMusic(&g_gameController, NULL, 0, 1);
	GetInitializedSimulationScheduler()->Reset(0);
}
// STUB: LEGORACERS2 0x0042a2e0
static void __fastcall InitializeSecondaryDirectionalLight(GameObject* p_object, void*)
{
	GameDirectionalLight* light = (GameDirectionalLight*) p_object;
	if ((light->m_flags & 0x800000) == 0) {
		AllocateSecondaryLightProperties(&light->m_properties, NULL);
	}
	light->GameObject::Initialize();
	light->UpdateRendererLight();
	light->m_properties.SetEnabled((light->m_flags & 0x18) == 0);
}

// STUB: LEGORACERS2 0x0042a330
static GameObject* __fastcall DestroySecondaryDirectionalLight(GameObject* p_object, void*, LegoU32 p_flags)
{
	GameDirectionalLight* light = (GameDirectionalLight*) p_object;
	ReleaseSecondaryLightProperties(light == NULL ? NULL : &light->m_properties, NULL);
	light->GameObject::~GameObject();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(light);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0042a360
static void __fastcall InitializeSecondaryAmbientLight(GameObject* p_object, void*)
{
	p_object->Initialize();
	if ((p_object->m_flags & 0x10) == 0) {
		((GameAmbientLight*) p_object)->ApplyToRenderer();
	}
}

void __fastcall PrepareSecondarySessionPlayers(GameObject*, void*);
void __fastcall ReleaseSecondaryAuxiliaryResources(void*, void*);
void __fastcall ReleaseSecondaryControllerPreview(void*, void*, LegoU32);
void __fastcall ReleaseSecondaryVoiceResources(void*, void*, LegoS32);
void __fastcall ReleaseSecondaryModelResources(void*, void*);
void __fastcall ReleaseSecondaryPointer(void*, void*);

// STUB: LEGORACERS2 0x0042a380
void SecondaryManager::ReleaseMenuResources(LegoU32 p_mode)
{
	if (!PrepareSplash(1)) {
		return;
	}
	m_unk0x64 = 0;
	if (p_mode != 0) {
		PrepareSecondarySessionPlayers(this, NULL);
	}
	SetSecondaryOverlayMode(&g_globalOverlayManager, NULL, 0);
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_stopCdTrack(audio, NULL);
	ReleaseSecondaryPointer(m_unk0x7c, NULL);
	if (m_loadedCarSet == -1) {
		ReleaseLocalizedMenuResources();
		ReleaseSecondaryAuxiliaryResources(m_unk0x88, NULL);
		((MenuBackground*) m_unk0x80)->Release();
		ReleaseSecondaryControllerPreview(g_gameController.m_unk0x90, NULL, 0);
		ReleaseSecondaryCharacterSelector(m_characterSelection, NULL);
	}
	ReleaseSecondaryCarSelector(m_carSelection, NULL);
	RemoveAndDestroySecondaryObject(m_unk0x74);
	m_unk0x74 = NULL;
	for (LegoS32 index = 2; index >= 0; --index) {
		RemoveAndDestroySecondaryObject(m_moreResourcePointers[index]);
		m_moreResourcePointers[index] = NULL;
	}
	RemoveAndDestroySecondaryObject(m_unk0x134);
	m_unk0x134 = NULL;
	RemoveAndDestroySecondaryObject((void*) m_unk0x12c);
	m_unk0x12c = 0;
	for (index = 2; index >= 0; --index) {
		RemoveAndDestroySecondaryObject(m_resourcePointers[index]);
		m_resourcePointers[index] = NULL;
	}
	ReleaseSecondaryVoiceResources(g_gameController.m_npcTextManager, NULL, -1);
	ReleaseSecondaryCharacterResources(g_gameController.m_bodypartManager, NULL);
	ReleaseSecondaryModelResources(g_gameController.m_unk0x90, NULL);
	ReleaseControllerSceneResources(&g_gameController, NULL, 1);
	audio = GetAudioService(TRUE);
	audio->m_vtable->m_removeListener(audio, NULL, (LegoU32) m_unk0x110);
	*(LegoU32*) ((LegoU8*) m_unk0x110 + 0x1c0) = 0;
	DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
	audio = GetAudioService(TRUE);
	audio->m_vtable->m_update(audio, NULL, 0xffffffff);
	audio = GetAudioService(TRUE);
	audio->m_vtable->m_unk0x50(audio, NULL, 0);
}

// STUB: LEGORACERS2 0x0042a550
void SecondaryManager::LoadLocalizedMenuResources()
{
	if (m_unk0x4d0 != 0) {
		return;
	}
	CharBuffer128 path;
	m_unk0x4d0 = (LegoU32) GetMemoryHeap()->Allocate(0xea600, "Language strings store");
	LegoS32 languages = 8;
	Application* application = GetApplication();
	if (application->m_vtable->m_value0x7c(application, NULL) == 0) {
		languages = ~8;
	}
	LegoU8* cursor = (LegoU8*) m_unk0x4d0;
	for (LegoU32 index = 0; index < 9; ++index) {
		if ((languages & (1 << index)) == 0) {
			m_unk0x4d4[index] = 0;
		}
		else {
			sprintf(path.m_data, "%s.%s", g_gameLocalizationPath.m_data, g_gameLanguageSuffixes[index]);
			WinFile file(path.m_data, 0, NULL);
			LegoU32 bytes = file.GetManagedSize();
			file.Read(cursor, bytes, 1);
			file.CloseManaged();
			m_unk0x4d4[index] = (LegoU32) cursor;
			cursor += 0x1d4c0;
		}
	}
}

// STUB: LEGORACERS2 0x0042a660
void SecondaryManager::ReleaseLocalizedMenuResources()
{
	if (m_unk0x4d0 != 0) {
		for (LegoS32 index = 8; index >= 0; --index) {
			m_unk0x4d4[index] = 0;
		}
		if (m_unk0x4d0 != 0) {
			GetMemoryHeap()->Free((void*) m_unk0x4d0);
			m_unk0x4d0 = 0;
		}
	}
}

// STUB: LEGORACERS2 0x0042a770
void SecondaryManager::ReturnToController(LegoU32 p_hideMenu)
{
	if (m_unk0x64 != 0) {
		ReleaseMenuResources(1);
	}
	else {
		PrepareSecondarySessionPlayers(this, NULL);
	}
	g_gameController.m_state = 1;
	g_gameController.m_stateStarted = 0;
	m_state = 9;
	if (p_hideMenu != 0) {
		((MenuBackground*) m_unk0x80)->SetMode(0);
		((MenuPanelSet*) m_unk0x84)->SetActive(0);
		SetSecondaryManagerAuxiliaryEnabled(m_unk0x88, NULL, 0);
	}
}

// STUB: LEGORACERS2 0x0042aac0
void __fastcall RefreshSecondaryResolution(SecondaryManager* p_manager, void*)
{
	SetGameObjectViewport(
		p_manager->m_unk0x110,
		NULL,
		0,
		0,
		g_displayConfiguration->m_width,
		g_displayConfiguration->m_height
	);
}

// STUB: LEGORACERS2 0x0042dd40
static const LegoFloat* __fastcall GetSecondaryProgressLayoutPosition(const void* p_array, void*, LegoU32 p_index)
{
	return (const LegoFloat*) ((const LegoU8*) p_array + p_index * 8);
}

// STUB: LEGORACERS2 0x00430090
static void __fastcall RefreshSecondaryProgressWidget(void* p_object, void*)
{
	if (g_gameControllerSession->m_unk0x04 == 0) {
		return;
	}
	LegoU8* widget = (LegoU8*) p_object;
	LegoU32 progress = *(LegoU32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1dfc);
	GlobalOverlayRecord** records = (GlobalOverlayRecord**) (widget + 0x74);
	BitmapFont* font = g_gameController.m_fontSet->m_fonts[3];
	for (LegoS32 index = 0; index < 3; ++index) {
		records[index]->SetFont(font);
	}
	for (LegoS32 record = 0; record < 3; ++record) {
		records[record]->SetList(4);
	}
	for (LegoS32 mode = 0; mode < 3; ++mode) {
		LegoChar text[16];
		sprintf(text, "%d", GetFoyerModeProgressValue(&progress, NULL, mode));
		records[mode]->InitializeNarrowText(text, 4, 0.0f);
	}
	const LegoU8* layout = *(const LegoU8**) (widget + 0x60) + 0x64;
	for (LegoS32 positionIndex = 0; positionIndex < 3; ++positionIndex) {
		const LegoFloat* first = GetSecondaryProgressLayoutPosition(layout, NULL, positionIndex * 2);
		LegoFloat position[2] = {first[0], first[1]};
		const LegoFloat* second = GetSecondaryProgressLayoutPosition(layout, NULL, 0x29 + positionIndex);
		position[0] += second[0];
		position[1] += second[1];
		records[positionIndex]->SetPosition(position, 0xffffffff);
	}
	GlobalOverlaySprite** sprites = (GlobalOverlaySprite**) (widget + 0xb4);
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	for (LegoS32 sprite = 0; sprite < 3; ++sprite) {
		LegoU32 textureIndex = sprite * 2;
		if (!IsFoyerModeProgressSet(&progress, NULL, sprite)) {
			++textureIndex;
		}
		memcpy(&sprites[sprite]->m_texture, &resources->m_records[textureIndex], sizeof(ControllerEventRecord));
		sprites[sprite]->SetVisible(1);
	}
}

// STUB: LEGORACERS2 0x00433130
void __fastcall RefreshSecondaryModeWidgets(void* p_object, void*)
{
	LegoU8* manager = (LegoU8*) p_object;
	GameObject** widgets = (GameObject**) (manager + 0x58);
	for (LegoS32 index = 0; index < *(LegoS32*) (manager + 0x60); ++index) {
		if (*(LegoU32*) ((LegoU8*) widgets[index] + 0x58) != 7) {
			RefreshSecondaryProgressWidget(widgets[index], NULL);
		}
	}
}

// STUB: LEGORACERS2 0x00434e70
static LegoU32 __stdcall GetSecondaryPortraitAtlasMask(LegoS32 p_world)
{
	switch (p_world) {
	case 0:
		return 0xfc;
	case 1:
		return 0x380;
	case 2:
		return 0x1083;
	case 3:
		return 0x1c00;
	case 4:
		return 0xa0a;
	default:
		return 0x90;
	}
}

// STUB: LEGORACERS2 0x00434ed0
static void __fastcall LoadSecondaryNpcPortraits(void* p_object, void*, LegoS32 p_world)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) p_object;
	if ((records->m_flags & 4) != 0) {
		return;
	}
	records->m_flags |= 4;
	LegoU32 mask = GetSecondaryPortraitAtlasMask(p_world);
	static const LegoChar* const paths[13] = {
		"Game Data\\HUD\\Text box faces\\face frame 01.tga",
		"Game Data\\HUD\\Text box faces\\face frame 02.tga",
		"Game Data\\HUD\\Text box faces\\face frame 03.tga",
		"Game Data\\HUD\\Text box faces\\face frame 04.tga",
		"Game Data\\HUD\\Text box faces\\face frame 05.tga",
		"Game Data\\HUD\\Text box faces\\face frame 06.tga",
		"Game Data\\HUD\\Text box faces\\face frame 07.tga",
		"Game Data\\HUD\\Text box faces\\face frame 08.tga",
		"Game Data\\HUD\\Text box faces\\face frame 09.tga",
		"Game Data\\HUD\\Text box faces\\face frame 10.tga",
		"Game Data\\HUD\\Text box faces\\face frame 11.tga",
		"Game Data\\HUD\\Text box faces\\face frame 12.tga",
		"Game Data\\HUD\\Text box faces\\face frame 13.tga",
	};
	static const LegoFloat coordinates[4][4] = {
		{0.001914062537f, 0.998085916f, 0.4941796958f, 0.505820334f},
		{0.501914084f, 0.998085916f, 0.994179666f, 0.505820334f},
		{0.001914062537f, 0.4980859458f, 0.4941796958f, 0.005820312537f},
		{0.501914084f, 0.4980859458f, 0.994179666f, 0.005820312537f},
	};
	for (LegoS32 atlas = 0; atlas < 13; ++atlas) {
		if ((mask & (1 << atlas)) == 0) {
			continue;
		}
		LegoS32 texture = g_displayRenderer->CreateTextureResource(paths[atlas], 0, -1, 1, 1);
		LegoS32 count = atlas == 12 ? 3 : 4;
		for (LegoS32 quadrant = 0; quadrant < count; ++quadrant) {
			LegoS32 index = atlas < 12 ? 50 + atlas * 4 + quadrant : 97 + quadrant;
			if (atlas == 11) {
				static const LegoS32 indices[4] = {94, 81, 95, 96};
				index = indices[quadrant];
			}
			ControllerEventRecord& record = records->m_records[index];
			record.m_index = texture;
			if (atlas < 9 || (atlas == 9 && quadrant < 2)) {
				record.m_u = coordinates[quadrant][0];
				record.m_v = coordinates[quadrant][1];
				record.m_uEnd = coordinates[quadrant][2];
				record.m_vEnd = coordinates[quadrant][3];
			}
			else {
				LegoS32 x = (quadrant & 1) * 128;
				LegoS32 y = (quadrant >> 1) * 128;
				records->SetPixelTextureRegion(x, y, x + 127, y + 127, index, 0.00390625f, 0.00390625f);
			}
		}
	}
}

// STUB: LEGORACERS2 0x00435870
static void __fastcall ReleaseSecondaryNpcPortraits(void* p_object, void*, LegoS32 p_world)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) p_object;
	if ((records->m_flags & 4) != 0) {
		records->m_flags &= ~4u;
		LegoU32 mask = GetSecondaryPortraitAtlasMask(p_world);
		for (LegoS32 atlas = 12; atlas >= 0; --atlas) {
			if ((mask & (1 << atlas)) != 0) {
				LegoS32 index = atlas == 12 ? 97 : 50 + atlas * 4;
				g_displayRenderer->ReleaseTextureResourceReference(records->m_records[index].m_index, 1);
			}
		}
	}
}

// STUB: LEGORACERS2 0x00435970
static void __stdcall FormatSecondaryPortraitTexturePath(LegoChar* p_destination, LegoS32 p_index)
{
	sprintf(p_destination, "Game Data\\HUD\\Text box faces\\Player faces %d.tga", (p_index - 100) / 4 + 1);
}

// STUB: LEGORACERS2 0x00435a30
static void __fastcall LoadSecondaryPortraitTexture(ControllerEventRecordTable* p_records, void*, LegoS32 p_index)
{
	CharBuffer128 path;
	FormatSecondaryPortraitTexturePath(path.m_data, p_index);
	LegoU8 x = (p_index & 1) * 128;
	LegoU8 y = ((p_index & 3) >> 1) * 128;
	ControllerEventRecord& record = p_records->m_records[p_index];
	record.m_index = g_displayRenderer->CreateTextureResource(path.m_data, 0, -1, 1, 1);
	record.m_u = (x + 0.49f) * 0.00390625f;
	record.m_v = 1.0f - (y + 0.49f) * 0.00390625f;
	record.m_uEnd = ((LegoU8) (x + 127) - 0.49f) * 0.00390625f;
	record.m_vEnd = 1.0f - ((LegoU8) (y + 127) - 0.49f) * 0.00390625f;
}

// STUB: LEGORACERS2 0x00435b40
void __fastcall LoadSecondaryControllerPreview(void* p_object, void*, LegoU32 p_player)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) p_object;
	if (p_player == 0) {
		if ((records->m_flags & 8) == 0) {
			records->m_flags |= 8;
			for (LegoS32 index = 100; index < 164; ++index) {
				LoadSecondaryPortraitTexture(records, NULL, index);
			}
		}
	}
	else if (records->m_firstIndex == -1 || records->m_secondIndex == -1) {
		LegoS32 index = GetNpcTextPlayerPortrait((const LegoU32*) p_player);
		if (records->m_firstIndex == -1) {
			records->m_firstIndex = index;
		}
		else {
			records->m_secondIndex = index;
		}
		LoadSecondaryPortraitTexture(records, NULL, index);
	}
}

// STUB: LEGORACERS2 0x00435bc0
void __fastcall ReleaseSecondaryControllerPreview(void* p_object, void*, LegoU32 p_mode)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) p_object;
	if (p_mode == 0) {
		if ((records->m_flags & 8) != 0) {
			records->m_flags &= ~8u;
			for (LegoS32 index = 163; index >= 100; --index) {
				g_displayRenderer->ReleaseTextureResourceReference(records->m_records[index].m_index, 1);
			}
		}
	}
	else if (records->m_firstIndex != -1) {
		LegoS32* index = &records->m_secondIndex;
		for (LegoS32 count = 0; count < 2; ++count, --index) {
			if (*index != -1) {
				g_displayRenderer->ReleaseTextureResourceReference(records->m_records[*index].m_index, 1);
				*index = -1;
			}
		}
	}
}

// STUB: LEGORACERS2 0x00436f50
void __fastcall LoadSecondaryAuxiliaryTextureRecords(void* p_object, void*)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) p_object;
	if ((records->m_flags & 0x80) != 0) {
		return;
	}
	records->m_flags |= 0x80;
	static const LegoChar* const paths[19] = {
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\circular thing.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\one player icon.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\two player icon.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\builder icon.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\options logo.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\load and save icon.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Mode Icon Adventure.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Mode Icon Arcade.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Mode Icon Best Times.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Mode Icon Time Attack.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Mode Icon Bonus Games.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Mode Icon Battle.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Mode Icon League.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\World Icon Sandy Bay.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\World Icon Arctic.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\World Icon Adventurers.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\World Icon LOM.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\World Icon Xalax.tga",
		"Game Data\\Frontend\\Frontend bits\\Circular Thing\\Exit Icon.tga",
	};
	for (LegoU32 index = 0; index < 19; ++index) {
		ControllerEventRecord& record = records->m_records[249 + index];
		record.m_index = g_displayRenderer->CreateTextureResource(paths[index], 0, -1, 1, 1);
		if (index < 17) {
			record.m_u = 0.0f;
			record.m_v = 1.0f;
			record.m_uEnd = 1.0f;
			record.m_vEnd = 0.0f;
		}
		else {
			records->SetFullTextureRegion(249 + index);
		}
	}
}

// STUB: LEGORACERS2 0x00437350
void __fastcall ReleaseSecondaryAuxiliaryModel(void* p_object, void*)
{
	LegoU32* flags = (LegoU32*) p_object;
	if ((*flags & 0x80) != 0) {
		*flags &= ~0x80;
		LegoS32* texture = (LegoS32*) ((LegoU8*) p_object + 0x14e0);
		for (LegoS32 index = 0; index < 0x13; ++index) {
			g_displayRenderer->ReleaseTextureResourceReference(*texture, 1);
			texture -= 5;
		}
	}
}

// STUB: LEGORACERS2 0x00437380
void __fastcall InitializeSecondaryAuxiliaryModel(void* p_object, void*)
{
	LegoU32* words = (LegoU32*) p_object;
	if ((words[0] & 0x100) == 0) {
		words[0] |= 0x100;
		LegoS32 texture =
			g_displayRenderer
				->CreateTextureResource("Game Data\\Frontend\\Frontend bits\\Misc\\Cursors.tga", 0, -1, 1, 1);
		LegoU32* records = (LegoU32*) ((LegoU8*) p_object + 0x14f4);
		records[0] = texture;
		records[1] = 0x3d97d70a;
		records[2] = 0x3f7e0a3d;
		records[3] = 0x3eea0a3d;
		records[4] = 0x3cbeb852;
		records[5] = texture;
		records[6] = 0x3f10fae1;
		records[7] = 0x3f7e0a3d;
		records[8] = 0x3f73051f;
		records[9] = 0x3cbeb852;
	}
}

// STUB: LEGORACERS2 0x00437400
void __fastcall ReleaseSecondaryModelResources(void* p_object, void*)
{
	LegoU32* words = (LegoU32*) p_object;
	if ((words[0] & 0x100) != 0) {
		words[0] &= ~0x100;
		LegoS32 texture = *(LegoS32*) ((LegoU8*) p_object + 0x1508);
		g_displayRenderer->ReleaseTextureResourceReference(texture, 1);
	}
}

// STUB: LEGORACERS2 0x00439b50
static void __fastcall SetSecondaryObjectEnabled(GameObject* p_object, void*, LegoU32 p_enabled)
{
	LegoU32 disabled = p_enabled == 0;
	if (((p_object->m_flags >> 1) & 1) != disabled) {
		p_object->m_flags = (p_object->m_flags & ~2u) | (disabled << 1);
	}
	if (((p_object->m_flags >> 3) & 1) != disabled) {
		p_object->m_flags = (p_object->m_flags & ~8u) | (disabled << 3);
		if ((p_object->m_flags & 8) == 0) {
			p_object->m_flags |= 0x2000000;
		}
	}
	if (((p_object->m_flags >> 2) & 1) != disabled) {
		p_object->m_flags = (p_object->m_flags & ~4u) | (disabled << 2);
	}
}

// STUB: LEGORACERS2 0x0043c140
static void __fastcall SetSecondaryBonusObjectState(GameObject* p_object, void*, LegoU32 p_show)
{
	LegoU8* object = (LegoU8*) p_object;
	if (p_show != 0) {
		SetSecondaryObjectEnabled(p_object, NULL, 1);
		*(LegoU32*) (object + 0xb4) = 2;
		*(LegoFloat*) (object + 0xb8) = 1.0f;
	}
	else {
		*(LegoU32*) (object + 0xb4) = 3;
		*(LegoFloat*) (object + 0xb8) = 0.0f;
	}
}

// STUB: LEGORACERS2 0x0043c190
void __fastcall RestoreSecondaryBonusObject(GameObject* p_object, void*)
{
	SetSecondaryBonusObjectState(p_object, NULL, 0);
}

// STUB: LEGORACERS2 0x004600a0
static const LegoS32* __stdcall GetSecondaryWorldSpeakers(LegoS32 p_world, LegoS32* p_count)
{
	*p_count = 0;
	switch (p_world) {
	case 0:
		*p_count = 27;
		return g_secondaryWorldSpeakers543c14;
	case 1:
		*p_count = 9;
		return g_secondaryWorldSpeakers543c84;
	case 2:
		*p_count = 8;
		return g_secondaryWorldSpeakers543cac;
	case 3:
		*p_count = 8;
		return g_secondaryWorldSpeakers543cd0;
	case 4:
		*p_count = 16;
		return g_secondaryWorldSpeakers543cf4;
	default:
		*p_count = 1;
		return g_secondaryDefaultSpeakers;
	}
}

// STUB: LEGORACERS2 0x00460130
static LegoU32 __stdcall GetSecondaryVoiceMask(LegoS32 p_world)
{
	LegoS32 count;
	const LegoS32* speakers = GetSecondaryWorldSpeakers(p_world, &count);
	LegoU32 mask = 0;
	for (LegoS32 index = 0; index < count; ++index) {
		LegoS32 category = g_npcTextSpeakers[speakers[index]].m_voiceCategory;
		if (category != -1) {
			mask |= 1 << (category & 31);
		}
	}
	return p_world == -1 ? 0 : mask | 0x26;
}

// STUB: LEGORACERS2 0x00460190
void __fastcall InitializeSecondaryVoiceResources(void* p_object, void*, LegoS32 p_world)
{
	LoadSecondaryNpcPortraits(g_gameController.m_unk0x90, NULL, p_world);
	LegoU32 mask = GetSecondaryVoiceMask(p_world);
	AudioService* audio = GetAudioService(TRUE);
	CharBuffer24 name;
	CharBuffer128 path;
	for (LegoS32 category = 0; category < 6; ++category) {
		const SecondaryVoiceBank& bank = g_secondaryVoiceBanks[category];
		if ((mask & (1 << category)) != 0) {
			for (LegoS32 index = 1; index <= bank.m_count; ++index) {
				sprintf(name.m_data, "%s%d", bank.m_namePrefix, index);
				sprintf(
					path.m_data,
					index < 10 ? "Game Data\\Sounds\\Voices\\%s0%d.vag" : "Game Data\\Sounds\\Voices\\%s%d.vag",
					bank.m_pathPrefix,
					index
				);
				SecondaryVoiceSourceTemplate source;
				memcpy(&source.m_ownerName, (LegoU8*) p_object + 0x10, 24);
				source.m_sourceName = name;
				source.m_path = path;
				source.m_empty = CharBuffer24("Empty");
				source.m_gain[0] = source.m_gain[1] = source.m_gain[2] = 1.0f;
				source.m_flags[0] = source.m_flags[1] = 0;
				source.m_flags[2] = 1;
				source.m_flags[3] = 5;
				source.m_falloff = 30.0f;
				source.m_range = 150.0f;
				memset(source.m_values, 0, sizeof(source.m_values));
				audio->m_vtable->m_addSource(audio, NULL, (const LegoU32*) &source, (LegoU32) p_object, 0);
			}
		}
	}
}

// STUB: LEGORACERS2 0x004603e0
void __fastcall ReleaseSecondaryVoiceResources(void* p_object, void*, LegoS32 p_world)
{
	LegoU32 mask = GetSecondaryVoiceMask(p_world);
	AudioService* audio = GetAudioService(TRUE);
	CharBuffer24 name;
	for (LegoS32 category = 5; category >= 0; --category) {
		const SecondaryVoiceBank& bank = g_secondaryVoiceBanks[category];
		if ((mask & (1 << category)) != 0) {
			for (LegoS32 index = bank.m_count; index > 0; --index) {
				sprintf(name.m_data, "%s%d", bank.m_namePrefix, index);
				AudioSourceRecord* source = audio->m_vtable->m_findSource(audio, NULL, (LegoU32) p_object, name.m_data);
				if (source != NULL) {
					audio->m_vtable->m_releaseSource(audio, NULL, source);
				}
			}
		}
	}
	ReleaseSecondaryNpcPortraits(g_gameController.m_unk0x90, NULL, p_world);
}

// STUB: LEGORACERS2 0x004609e0
static void* __fastcall ConstructSecondarySelectionGrid(void* p_object, void*, LegoU32 p_capacity)
{
	LegoU8* object = (LegoU8*) p_object;
	*(LegoU32*) (object + 0x1d08) = p_capacity;
	*(LegoU32*) (object + 0x1e5c) = 0;
	*(LegoU32*) (object + 0x1e84) = 0;
	*(LegoU32*) (object + 0x1ebc) = 0;
	*(LegoU32*) (object + 0x1ec4) = 0xffffffff;
	*(LegoU32*) (object + 0x1ec8) = 0;
	ConstructFoyerSkeletonRecord((FoyerSkeletonRecord*) (object + 0x1ef8), NULL);
	memset(object + 0x1d90, 0, 0x32 * 4);
	memset(object + 0x1e60, 0, 8 * 4);
	memset(object + 0x1e88, 0, 8 * 4);
	return p_object;
}

// STUB: LEGORACERS2 0x00460a50
static void __fastcall DestroySecondarySelectionGrid(void* p_object, void*)
{
	GameObjectBaseNoOp((GameObject*) ((LegoU8*) p_object + 0x1ef8), NULL);
}

// STUB: LEGORACERS2 0x00463ec0
void* __fastcall ConstructSecondaryCharacterSelection(void* p_object, void*)
{
	LegoU8* object = (LegoU8*) p_object;
	*(LegoU32*) object = 0xffffffff;
	ConstructFoyerSkeletonRecord((FoyerSkeletonRecord*) (object + 0x0c), NULL);
	ConstructSecondarySelectionGrid(object + 0x410, NULL, 0x28);
	*(LegoU32*) (object + 0x2720) = 0xffffffff;
	*(LegoU32*) (object + 0x2728) = 0xffffffff;
	*(LegoU32*) (object + 0x2730) = 0xffffffff;
	*(LegoU32*) (object + 0x2798) = 0;
	*(LegoU32*) (object + 0x279c) = 0;
	*(LegoU32*) (object + 0x27a0) = 0;
	*(LegoU32*) (object + 0x27a4) = 0;
	*(LegoU32*) (object + 0x273c) = 0xffffffff;
	*(LegoU32*) (object + 0x2740) = 0xffffffff;
	return p_object;
}

// STUB: LEGORACERS2 0x00463f20
void __fastcall ReleaseSecondaryObject70(void* p_object, void*)
{
	DestroySecondarySelectionGrid((LegoU8*) p_object + 0x410, NULL);
	GameObjectBaseNoOp((GameObject*) ((LegoU8*) p_object + 0x0c), NULL);
}

// STUB: LEGORACERS2 0x00467af0
void* __fastcall ConstructSecondaryCarSelection(void* p_object, void*)
{
	LegoU8* object = (LegoU8*) p_object;
	*(LegoU32*) object = 0;
	*(LegoU32*) (object + 4) = 0xffffffff;
	*(LegoU32*) (object + 8) = 0xffffffff;
	*(LegoU32*) (object + 0x14) = 0;
	ConstructFoyerSkeletonRecord((FoyerSkeletonRecord*) (object + 0x8c), NULL);
	ConstructSecondarySelectionGrid(object + 0x490, NULL, 800);
	*(LegoU32*) (object + 0x27bc) = 0xffffffff;
	*(LegoU32*) (object + 0x27dc) = 0xffffffff;
	*(LegoU32*) (object + 0x27a0) = 0xffffffff;
	*(LegoU32*) (object + 0x27a4) = 0xffffffff;
	*(LegoU32*) (object + 0x27cc) = 0xffffffff;
	*(LegoU32*) (object + 0x27d0) = 0xffffffff;
	*(LegoU32*) (object + 0x27d4) = 0xffffffff;
	for (LegoU32 offset = 0x74; offset <= 0x88; offset += 4) {
		*(LegoU32*) (object + offset) = 0;
	}
	return p_object;
}

// STUB: LEGORACERS2 0x00467b80
void __fastcall ReleaseSecondaryObject6c(void* p_object, void*)
{
	DestroySecondarySelectionGrid((LegoU8*) p_object + 0x490, NULL);
	GameObjectBaseNoOp((GameObject*) ((LegoU8*) p_object + 0x8c), NULL);
}

// STUB: LEGORACERS2 0x004a9070
ModelResource* __fastcall CopySecondaryModelResource(ModelResource* p_destination, void*, const ModelResource* p_source)
{
	memcpy(p_destination, p_source, sizeof(*p_destination));
	if (p_destination->m_bounds != NULL) {
		p_destination->m_bounds = (ModelBounds*) AllocateHeapMemory(sizeof(ModelBounds));
		if (p_destination->m_bounds != NULL) {
			p_destination->m_bounds->m_isElongatedXZ = 1;
			p_destination->m_bounds->m_radius = 0.0f;
		}
		memcpy(p_destination->m_bounds, p_source->m_bounds, sizeof(ModelBounds));
	}
	p_destination->m_resources.Reset();
	LegoS32 groupCount = p_destination->m_groupCount;
	ModelTextureGroup* groups = (ModelTextureGroup*) AllocateHeapMemory(groupCount * sizeof(ModelTextureGroup));
	if (groups != NULL) {
		for (LegoS32 index = 0; index < groupCount; ++index) {
			new (groups + index) ModelTextureGroup;
		}
	}
	p_destination->m_groups = groups;
	for (LegoS32 index = 0; index < groupCount; ++index) {
		groups[index].m_type = index + 1;
	}
	for (LegoS32 groupIndex = 0; groupIndex < p_destination->m_groupCount; ++groupIndex) {
		ModelTextureGroup& group = p_destination->m_groups[groupIndex];
		const ModelTextureGroup& source = p_source->m_groups[groupIndex];
		group.m_primitiveCount = source.m_primitiveCount;
		LegoS32 count = group.m_primitiveCount;
		LegoU32* allocation = (LegoU32*) AllocateHeapMemory(count * sizeof(ModelPrimitiveTextureRecord) + 4);
		ModelPrimitiveTextureRecord* primitives = NULL;
		if (allocation != NULL) {
			allocation[0] = count;
			primitives = (ModelPrimitiveTextureRecord*) (allocation + 1);
			for (LegoS32 primitive = 0; primitive < count; ++primitive) {
				new (primitives + primitive) ModelPrimitiveTextureRecord;
			}
		}
		group.m_primitives = primitives;
		for (LegoS32 primitive = 0; primitive < group.m_primitiveCount; ++primitive) {
			CopySecondaryModelPrimitive(primitives + primitive, NULL, source.m_primitives + primitive);
		}
	}
	if (p_destination->m_unk0x28 != NULL) {
		ModelShadowData* shadow = (ModelShadowData*) AllocateHeapMemory(sizeof(ModelShadowData));
		if (shadow != NULL) {
			shadow->m_edgeRecords = NULL;
			shadow->m_faceRecords = NULL;
			shadow->m_edgeRecordCount = 0;
			shadow->m_faceRecordCount = 0;
			shadow->m_unk0x10 = 1;
			shadow->m_scaleX = shadow->m_scaleY = 1.0f;
			memset(shadow->m_unk0x1c, 0, sizeof(shadow->m_unk0x1c));
			shadow->m_textureId = -1;
			shadow->m_textureReferenceMask = 0;
			shadow->m_geometryReferenceMask = 0;
			shadow->m_referenceCount = 0;
		}
		p_destination->m_unk0x28 = shadow;
		CopySecondaryModelShadow(shadow, NULL, (const ModelShadowData*) p_source->m_unk0x28);
	}
	if (p_destination->m_collisionData != NULL && p_destination->m_collisionDataSize != 0) {
		p_destination->m_collisionData =
			GetMemoryHeap()->Allocate(p_destination->m_collisionDataSize, "New Collision Data");
		memcpy(p_destination->m_collisionData, p_source->m_collisionData, p_destination->m_collisionDataSize);
		LoadCollisionResource(p_destination->m_collisionData);
		return p_destination;
	}
	p_destination->m_collisionData = NULL;
	p_destination->m_collisionDataSize = 0;
	return p_destination;
}

// STUB: LEGORACERS2 0x004a9300
static ModelPrimitiveTextureRecord* __fastcall CopySecondaryTranslatedPrimitive(
	ModelPrimitiveTextureRecord* p_destination,
	void*,
	const ModelPrimitiveTextureRecord* p_source,
	const Vector3* p_translation
)
{
	memcpy(p_destination, p_source, sizeof(*p_destination));
	if (p_destination->m_vertexBuffer != NULL) {
		RendererVertexBuffer* source = p_destination->m_vertexBuffer;
		const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) source->m_vtable;
		LegoBool32 translated = fabs(-p_translation->m_x) > 0.0001f || fabs(-p_translation->m_y) > 0.0001f ||
								fabs(-p_translation->m_z) > 0.0001f;
		LegoU32 mode = translated ? 1 : source->m_unk0x04;
		LegoS32 textureCount = dispatch->m_getTextureCount(source, NULL);
		dispatch = (const RendererVertexBufferDispatch*) source->m_vtable;
		LegoS32 count = dispatch->m_getCount(source, NULL);
		dispatch = (const RendererVertexBufferDispatch*) source->m_vtable;
		LegoU32 attributes = dispatch->m_getAttributes(source, NULL);
		const RendererMainDispatch* renderer = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
		RendererVertexBuffer* vertices =
			renderer->m_createVertexBuffer(g_displayRenderer, NULL, attributes, count, textureCount, mode);
		p_destination->m_vertexBuffer = vertices;
		dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
		dispatch->m_copyVerticesFrom(vertices, NULL, p_source->m_vertexBuffer, 0);
		if (translated) {
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			count = dispatch->m_getCount(vertices, NULL);
			if (dispatch->m_lock(vertices, NULL, 0, count, 0, 0) == TRUE) {
				dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				dispatch->m_setCursor(vertices, NULL, 0);
				LegoS32 vertex = 0;
				dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				count = dispatch->m_getCount(vertices, NULL);
				while (vertex < count) {
					dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
					Vector3* position = dispatch->m_getCurrentPosition(vertices, NULL);
					position->m_x += p_translation->m_x;
					position->m_y += p_translation->m_y;
					position->m_z += p_translation->m_z;
					dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
					dispatch->m_advanceCursor(vertices, NULL);
					++vertex;
					dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
					count = dispatch->m_getCount(vertices, NULL);
				}
				dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
				dispatch->m_unlock(vertices, NULL, 1);
			}
			dispatch = (const RendererVertexBufferDispatch*) vertices->m_vtable;
			dispatch->m_setMode(vertices, NULL, vertices->m_unk0x04);
		}
	}
	if (p_destination->m_indexBuffer != NULL) {
		const RendererMainDispatch* renderer = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
		RendererIndexBuffer* source = p_source->m_indexBuffer;
		RendererIndexBuffer* indices =
			renderer->m_createIndexBuffer(g_displayRenderer, NULL, source->m_unk0x0c, source->m_count);
		p_destination->m_indexBuffer = indices;
		const RendererIndexBufferDispatch* dispatch = (const RendererIndexBufferDispatch*) indices->m_vtable;
		LegoS32 count = (source->m_writtenEnd - source->m_data) >> 1;
		if (dispatch->m_lock(indices, NULL, 0, count, 1) == TRUE) {
			LegoU32 bytes = ((source->m_writtenEnd - source->m_data) >> 1) * 2;
			memcpy(indices->m_writtenEnd, source->m_data, bytes);
			indices->m_writtenEnd += bytes;
			dispatch = (const RendererIndexBufferDispatch*) indices->m_vtable;
			dispatch->m_unlock(indices, NULL, 1, 1);
		}
	}
	if (p_destination->m_heapData != NULL) {
		p_destination->m_heapData = NULL;
	}
	const RendererMainDispatch* renderer = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	renderer->m_prepareDrawPlan(g_displayRenderer, NULL, (RendererStagePlanRequest*) p_destination);
	return p_destination;
}

// STUB: LEGORACERS2 0x004a9580
static void __fastcall CopySecondaryModelPrimitive(
	ModelPrimitiveTextureRecord* p_destination,
	void*,
	const ModelPrimitiveTextureRecord* p_source
)
{
	Vector3 translation = {0.0f, 0.0f, 0.0f};
	CopySecondaryTranslatedPrimitive(p_destination, NULL, p_source, &translation);
}

// STUB: LEGORACERS2 0x004b3530
void __fastcall SetSecondaryOverlayMode(GlobalOverlayManager* p_manager, void*, LegoU32 p_mode)
{
	p_manager->m_recordListCount = p_mode;
}

// STUB: LEGORACERS2 0x004bb780
void __fastcall RequestSecondaryPauseLeave(GlobalPauseController* p_pause, void*, LegoU32 p_resumeInput)
{
	p_pause->m_leaveRequested = 1;
	p_pause->m_resumeInput = p_resumeInput;
}

// STUB: LEGORACERS2 0x004bece0
static void __fastcall AllocateSecondaryLightProperties(GameLightProperties* p_properties, void*)
{
	p_properties->m_lightIndex = AllocateSecondaryRendererLight(GetStaticDisplayRenderer(), NULL);
}

// STUB: LEGORACERS2 0x004bed00
static void __fastcall ReleaseSecondaryLightProperties(GameLightProperties* p_properties, void*)
{
	if (p_properties->m_lightIndex != -1 && GetStaticDisplayRenderer()->m_device != NULL) {
		ReleaseSecondaryRendererLight(GetStaticDisplayRenderer(), NULL, p_properties->m_lightIndex);
		p_properties->m_lightIndex = -1;
	}
}

// STUB: LEGORACERS2 0x004bf0a0
static LegoS32 __fastcall AllocateSecondaryRendererLight(DisplayRenderer* p_renderer, void*)
{
	LegoS32 index = 0;
	RendererLightRecord* light = &p_renderer->m_lights[0];
	while ((light->m_flags & 1) != 0) {
		++index;
		++light;
		if (index >= 32) {
			return 0;
		}
	}
	p_renderer->m_lightCount = index + 1;
	light->m_flags = 1;
	GetStaticDisplayRenderer()->m_device->LightEnable(light->m_deviceIndex, TRUE);
	light->m_flags |= 8;
	light->m_flags |= 2;
	return light->m_deviceIndex;
}

// STUB: LEGORACERS2 0x004bf0f0
static void __fastcall ReleaseSecondaryRendererLight(DisplayRenderer* p_renderer, void*, LegoS32 p_index)
{
	RendererLightRecord* light = &p_renderer->m_lights[p_index];
	if ((light->m_flags & 8) != 0) {
		GetStaticDisplayRenderer()->m_device->LightEnable(light->m_deviceIndex, FALSE);
		light->m_flags &= ~8;
	}
	light->m_flags = 0;
	if (p_index == p_renderer->m_lightCount - 1) {
		--p_renderer->m_lightCount;
	}
}
// STUB: LEGORACERS2 0x004c0ec0
static void __fastcall AddSecondaryRendererTextureReference(DisplayRenderer* p_renderer, void*, LegoS32 p_index)
{
	if (p_index != -1) {
		RendererResourceRef* reference = p_renderer->FindTextureReferenceById(p_index, 1);
		if (reference != NULL) {
			++reference->m_referenceCount;
		}
	}
}

// STUB: LEGORACERS2 0x004c1cd0
static SecondaryContentArchiveManager* __fastcall ConstructSecondaryContentArchiveManager(
	SecondaryContentArchiveManager* p_manager,
	void*
)
{
	p_manager->m_vtable = &g_secondaryContentArchiveDispatch;
	return p_manager;
}

// STUB: LEGORACERS2 0x004c1ce0
static SecondaryContentArchiveManager* __fastcall DestroySecondaryContentArchiveManager(
	SecondaryContentArchiveManager* p_manager,
	void*,
	LegoU32 p_flags
)
{
	p_manager->m_vtable = &g_secondaryContentArchiveBaseDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_manager);
	}
	return p_manager;
}

// STUB: LEGORACERS2 0x004c1d00
static void* __fastcall CreateSecondaryLanguageRecord(
	SecondaryContentArchiveManager*,
	void*,
	const LegoChar*,
	const LegoChar*,
	const LegoChar*,
	LegoU32,
	LegoU32,
	const LegoChar*
)
{
	return NULL;
}

// STUB: LEGORACERS2 0x004c1d10
static LegoU32 __fastcall QuerySecondaryLanguageRecord(
	SecondaryContentArchiveManager*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
)
{
	return 0;
}

// STUB: LEGORACERS2 0x004c1d20
static void __fastcall SecondaryContentArchiveThreeArguments(
	SecondaryContentArchiveManager*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32
)
{
}

// STUB: LEGORACERS2 0x004c1d30
SecondaryContentArchiveManager* GetSecondaryContentArchiveManager()
{
	if ((g_secondaryContentArchiveInitialized & 1) == 0) {
		g_secondaryContentArchiveInitialized |= 1;
		ConstructSecondaryContentArchiveManager(&g_secondaryContentArchive, NULL);
		atexit(&OnSecondaryContentArchiveExit);
	}
	return &g_secondaryContentArchive;
}

// STUB: LEGORACERS2 0x004c1d70
static void OnSecondaryContentArchiveExit()
{
	g_secondaryContentArchive.m_vtable = &g_secondaryContentArchiveBaseDispatch;
}

// STUB: LEGORACERS2 0x004c2ef0
static void InitializeSecondaryLightTemplate()
{
	g_secondaryLightBaseTemplate.m_flags = 2;
	g_secondaryLightBaseTemplate.m_first = CharBuffer24("NoName");
	g_secondaryLightBaseTemplate.m_second = CharBuffer24("");
}

// STUB: LEGORACERS2 0x004c6d60
static ModelShadowData* __fastcall CopySecondaryModelShadow(
	ModelShadowData* p_destination,
	void*,
	const ModelShadowData* p_source
)
{
	p_destination->m_edgeRecordCount = p_source->m_edgeRecordCount;
	p_destination->m_faceRecordCount = p_source->m_faceRecordCount;
	p_destination->m_unk0x10 = p_source->m_unk0x10;
	p_destination->m_textureId = p_source->m_textureId;
	p_destination->m_textureReferenceMask = 0;
	p_destination->m_geometryReferenceMask = 0;
	p_destination->m_referenceCount = 0;
	if (p_source->m_textureId != -1) {
		AddSecondaryRendererTextureReference(g_displayRenderer, NULL, p_destination->m_textureId);
		p_destination->m_textureReferenceMask |= 1 << (p_destination->m_referenceCount & 31);
	}
	if (p_source->m_faceRecords != NULL) {
		p_destination->m_faceRecords =
			GetMemoryHeap()->Allocate(p_destination->m_faceRecordCount * 12, "Shadow face info");
		p_destination->m_edgeRecords =
			GetMemoryHeap()->Allocate(p_destination->m_edgeRecordCount * 36, "Shadow edge info");
		memcpy(p_destination->m_edgeRecords, p_source->m_edgeRecords, p_destination->m_edgeRecordCount * 36);
		memcpy(p_destination->m_faceRecords, p_source->m_faceRecords, p_destination->m_faceRecordCount * 12);
		p_destination->m_geometryReferenceMask |= 1 << (p_destination->m_referenceCount & 31);
	}
	++p_destination->m_referenceCount;
	return p_destination;
}
