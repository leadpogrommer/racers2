#include "game/controller/gamecontrollerinitialization.h"

#include "audio/service/audioservice.h"
#include "core/application/application.h"
#include "core/application/commandline.h"
#include "frontend/dialogue/npctextmanager.h"
#include "frontend/foyer/foyermanager.h"
#include "frontend/menu/menumanager.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/overlay/textboxmanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/session/gamecontrollerplayers.h"
#include "game/session/gamecontrollersession.h"
#include "game/state/gamepreferences.h"
#include "input/controller/inputcontrollermanager.h"
#include "io/localization/localizedstringtable.h"
#include "memory/memoryheap.h"
#include "physics/collision/collisionservice.h"
#include "render/fonts/bitmapfontset.h"

#include <new.h>
#include <windows.h>

namespace
{
struct StartupPlayerProfile {
	GameObjectTemplate m_parameters;
	LegoU32 m_index;
	LegoU32 m_unk0x38; // not written by the inlined constructor
	LegoU32 m_unk0x3c;
	LegoU32 m_unk0x40;
	LegoU32 m_unk0x44;
	LegoU32 m_unk0x48;
	LegoU32 m_unk0x4c;
	LegoU32 m_unk0x50;
	LegoU32 m_unk0x54;
	LegoU32 m_unk0x58;
};
DECOMP_SIZE_ASSERT(StartupPlayerProfile, 0x5c);

struct StartupOverlayCollection {
	LegoU32 m_count;
	GlobalOverlayRecord* m_records[180];
	undefined m_unk0x2d4[0x2ec - 0x2d4];
	LegoU32 m_active;
};
DECOMP_STATIC_ASSERT(offsetof(StartupOverlayCollection, m_active) == 0x2ec);
DECOMP_SIZE_ASSERT(StartupOverlayCollection, 0x2f0);

struct StartupAuxiliaryDispatch {
	void(__fastcall* m_initialize)(void*, void*);
};

static void InitializeAuxiliaryObject(void* p_object)
{
	(*(const StartupAuxiliaryDispatch**) p_object)->m_initialize(p_object, NULL);
}

static void SetTemplate(GameObjectTemplate* p_template, const LegoChar* p_name, LegoU32 p_flags)
{
	p_template->m_flags = p_flags;
	p_template->m_first = CharBuffer24(p_name);
	p_template->m_second = CharBuffer24("");
}

static void ActivateStartupObject(GameObject* p_object)
{
	p_object->m_flags |= 0x400000;
	p_object->m_vtable->m_initialize(p_object, NULL);
}
} // namespace

// STUB: LEGORACERS2 0x00407250
void GameController::Initialize()
{
	if ((m_flags & 0x800000) == 0) {
		m_startupReady = IsRunningFromOpticalMedia() == FALSE;
		if (!m_startupReady) {
			return;
		}
		ConfigureGameScreenshotDirectory();
		InitializeCollisionServices();
		LegoU32 i;
		for (i = 0; i < 8; ++i) {
			g_inputControllerManager->ConfigureController((LegoU32) this, i, TRUE, FALSE);
			m_controllers[i] = g_inputControllerManager->FindOrCreateController((LegoU32) this, i, FALSE, TRUE);
		}
		InitializeGameControllerDefaults(this, NULL);
		ResetGameControllerSession(this, NULL);
		InitializeGameControllerPlayers();

		void* storage = AllocateHeapMemory(0xd4);
		m_localizedStrings = storage == NULL ? NULL : new (storage) LocalizedStringTable;
		m_localizedStrings->Load(&g_gameLocalizationPath, &g_gameLanguageSuffixes[0][0], 9);
		m_localizedStrings->SelectLanguage(GetGameControllerSettings()->m_language, NULL, 0x10);
		g_globalOverlayManager.SetLocalizedStrings(m_localizedStrings);
		InitializeLocalizedPlayerNames();
		storage = AllocateHeapMemory(0x20);
		m_fontSet = storage == NULL ? NULL : new (storage) BitmapFontSet;

		// Keep the local templates alive throughout this method, as in the
		// retail stack frame; GameObject construction retains their pointers.
		GameObjectTemplate templates[13];
		SetTemplate(&templates[0], "Pause Detector", 10);
		storage = AllocateHeapMemory(0x58);
		m_pauseDetector = storage == NULL ? NULL : ConstructPauseDetector(storage, NULL, &templates[0]);
		ActivateStartupObject(m_pauseDetector);

		storage = AllocateHeapMemory(0x1c);
		m_unk0x70 = storage == NULL ? NULL : ConstructGamePauseObserver(storage);
		InitializeAuxiliaryObject(m_unk0x70);
		AudioService* audio = GetAudioService(TRUE);
		audio->m_vtable->m_unk0x54(audio, NULL, (LegoU32) "host0:..\\");

		BodypartManagerTemplate bodyparts;
		SetTemplate(&bodyparts.m_base, "Bodypart Manager", 2);
		bodyparts.m_paths[0] = CharBuffer128("game data\\characters\\heads\\heads.txt");
		bodyparts.m_paths[1] = CharBuffer128("game data\\characters\\bodies\\torsos.txt");
		bodyparts.m_paths[2] = CharBuffer128("game data\\characters\\bodies\\legs.txt");
		bodyparts.m_paths[3] = CharBuffer128("game data\\characters\\heads\\faces.txt");
		bodyparts.m_paths[4] = CharBuffer128("game data\\Aliens\\alien heads\\heads.txt");
		bodyparts.m_paths[5] = CharBuffer128("game data\\Aliens\\alien bodies\\torsos.txt");
		bodyparts.m_paths[6] = CharBuffer128("game data\\Ramas\\rama heads\\heads.txt");
		bodyparts.m_paths[7] = CharBuffer128("game data\\Ramas\\Rama bodies\\torsos.txt");
		storage = AllocateHeapMemory(0x49c);
		m_bodypartManager = storage == NULL ? NULL : ConstructBodypartManager(storage, NULL, &bodyparts);
		ActivateStartupObject(m_bodypartManager);
		storage = AllocateHeapMemory(0x15ec);
		m_unk0x90 = storage == NULL ? NULL : ConstructControllerEventRecords(storage, NULL);
		storage = AllocateHeapMemory(0x3c74);
		m_unk0x8c = storage == NULL ? NULL : ConstructControllerSaveRecords(storage, NULL);

		SetTemplate(&templates[2], "Secondary Manager", 2);
		storage = AllocateHeapMemory(0x4f8);
		m_secondaryManager = storage == NULL ? NULL : new (storage) SecondaryManager(&templates[2]);
		ActivateStartupObject(m_secondaryManager);
		SetTemplate(&templates[3], "KillSphereSupervisor", 2);
		storage = AllocateHeapMemory(600);
		m_killSphereSupervisor = storage == NULL ? NULL : ConstructKillSphereSupervisor(storage, NULL, &templates[3]);
		ActivateStartupObject(m_killSphereSupervisor);
		SetTemplate(&templates[4], "ReverbManager", 10);
		storage = AllocateHeapMemory(0xbc);
		m_reverbManager = storage == NULL ? NULL : ConstructReverbManager(storage, NULL, &templates[4]);
		ActivateStartupObject(m_reverbManager);
		SetTemplate(&templates[5], "Battle Manager", 2);
		storage = AllocateHeapMemory(0xc4);
		m_battleManager = storage == NULL ? NULL : ConstructBattleManager(storage, NULL, &templates[5]);
		ActivateStartupObject(m_battleManager);
		SetTemplate(&templates[6], "Foyer Manager", 2);
		storage = AllocateHeapMemory(0x15c);
		m_foyerManager = storage == NULL ? NULL : new (storage) FoyerManager(&templates[6]);
		ActivateStartupObject(m_foyerManager);
		SetTemplate(&templates[7], "Text Box Manager", 2);
		storage = AllocateHeapMemory(0x5c);
		m_textBoxManager = storage == NULL ? NULL : new (storage) TextBoxManager(&templates[7]);
		ActivateStartupObject(m_textBoxManager);
		SetTemplate(&templates[8], "NPC Text Manager", 2);
		storage = AllocateHeapMemory(0x70);
		m_npcTextManager = storage == NULL ? NULL : new (storage) NpcTextManager(&templates[8]);
		ActivateStartupObject(m_npcTextManager);
		SetTemplate(&templates[9], "SecondaryFX Manager", 2);
		storage = AllocateHeapMemory(0x5c);
		m_secondaryFxManager = storage == NULL ? NULL : ConstructSecondaryFxManager(storage, NULL, &templates[9]);
		ActivateStartupObject(m_secondaryFxManager);
		SetTemplate(&templates[10], "Menu Manager", 10);
		storage = AllocateHeapMemory(0x2b4);
		m_menuManager = storage == NULL ? NULL : new (storage) MenuManager(&templates[10]);
		ActivateStartupObject(m_menuManager);

		storage = AllocateHeapMemory(0xa4);
		m_unk0xb4 = storage == NULL ? NULL : ConstructControllerAuxiliaryState(storage);
		InitializeAuxiliaryObject(m_unk0xb4);
		SetTemplate(&templates[11], "Network App Layer", 2);
		storage = AllocateHeapMemory(0x2b0c);
		m_networkApp = storage == NULL ? NULL : ConstructNetworkAppLayer(storage, NULL, &templates[11]);
		ActivateStartupObject(m_networkApp);
		storage = AllocateHeapMemory(0x6c);
		m_unk0xc8 = storage == NULL ? NULL : ConstructControllerStartupScene(storage, NULL);
		ActivateStartupObject(m_unk0xc8);

		StartupPlayerSettings* playerSettings = (StartupPlayerSettings*) ((LegoU8*) GetGameControllerSession() + 0x4c);
		for (i = 0; i < 8; ++i) {
			StartupPlayerProfile* profile = (StartupPlayerProfile*) AllocateHeapMemory(0x5c);
			if (profile != NULL) {
				SetTemplate(&profile->m_parameters, "EMPTY", 2);
				profile->m_index = playerSettings[i].m_index;
				profile->m_unk0x3c = playerSettings[i].m_unk0x08;
				profile->m_unk0x40 = playerSettings[i].m_unk0x0c;
				profile->m_unk0x44 = playerSettings[i].m_unk0x18;
				profile->m_unk0x48 = 0;
				profile->m_unk0x4c = playerSettings[i].m_unk0x1c;
				profile->m_unk0x50 = playerSettings[i].m_unk0x338;
				profile->m_unk0x54 = playerSettings[i].m_unk0x33c;
				profile->m_unk0x58 = playerSettings[i].m_unk0x340;
			}
			m_playerProfiles[i] = profile;
		}
		storage = AllocateHeapMemory(0x98);
		m_unk0xd4 = storage == NULL ? NULL : ConstructControllerSelectionState(storage, NULL);
		SetTemplate(&templates[12], "ScreenPrinter", 2);
		storage = AllocateHeapMemory(0x68);
		m_screenPrinter = storage == NULL ? NULL : ConstructScreenPrinter(storage, NULL, &templates[12]);
		ActivateStartupObject(m_screenPrinter);
		if (FUN_004b4a60("_overdraw")) {
			GetApplication()->m_unk0x10 = 1;
		}
		RegisterGameControllerCallbacks();

		StartupOverlayCollection* overlays = (StartupOverlayCollection*) AllocateHeapMemory(0x2f0);
		if (overlays != NULL) {
			overlays->m_count = 0;
			for (i = 0; i < 180; ++i) {
				overlays->m_records[i] = NULL;
			}
			overlays->m_active = 0;
		}
		m_unk0xd8 = overlays;
	}
	GameObject::Initialize();
}

// STUB: LEGORACERS2 0x0042af30
LegoBool32 IsRunningFromOpticalMedia()
{
	if (GetDriveType(NULL) == DRIVE_CDROM) {
		extern void FUN_0042a860();
		FUN_0042a860();
		return TRUE;
	}
	return FALSE;
}
