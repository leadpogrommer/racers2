#include "bodypartmanager.h"
#include "characterpartpreset.h"
#include "commandline.h"
#include "foyermanager.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollerplayers.h"
#include "gamecontrollersession.h"
#include "gamecontrollersessionqueries.h"
#include "npctextmanager.h"
#include "secondarymanager.h"

#include <string.h>

LegoU32 __cdecl GetSecondaryWorldRacerCount(LegoU32);
void __fastcall ConfigureControllerInputBindings(GameControllerPairGroups*, void*, InputController*);
void __fastcall AssignSecondarySessionPlayers(SecondaryManager*, void*);
LegoFloat __fastcall GetFoyerModeProgressFraction(const LegoU32*, void*, LegoU32);
static LegoU32 GetSecondaryDefaultRacerCount();

static LegoBool32 __fastcall ChooseSecondarySessionCharacters(SecondaryManager*, void*, LegoU32[8][4], LegoU32*);
void __fastcall FindNamedCharacterBodyparts(BodypartManager*, void*, const LegoChar*, LegoU32*);

// GLOBAL: LEGORACERS2 0x0053ee34
extern const LegoS32 g_secondaryWorldCarCounts[5] = {7, 8, 7, 8, 6};
// GLOBAL: LEGORACERS2 0x0053ee48
extern const LegoS32 g_secondaryWorldCarStarts[5] = {0, 7, 16, 23, 31};
// GLOBAL: LEGORACERS2 0x0053ee5c
extern const LegoS32 g_secondarySpecialCarIndices[2] = {15, 37};
// GLOBAL: LEGORACERS2 0x0053ee64
extern const LegoU32 g_secondarySessionRaceValues[26] = {2, 2, 3, 3, 3, 0, 2, 3, 3, 3, 2, 3, 2,
														 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 3, 4, 3};

static StartupPlayerSettings* GetSecondarySessionPlayer(LegoU32 p_index)
{
	return (StartupPlayerSettings*) ((LegoU8*) GetGameControllerSession() + 0x4c + p_index * 0x344);
}

// STUB: LEGORACERS2 0x00428650
void __fastcall PrepareSecondarySessionPlayers(GameObject* p_object, void*)
{
	SecondaryManager* manager = (SecondaryManager*) p_object;
	GameControllerSession* session = GetGameControllerSession();
	GameControllerSettings* settings = GetGameControllerSettings();
	FoyerManager* foyer = (FoyerManager*) g_gameController.m_foyerManager;
	if (FUN_004b4a60("/foyer")) {
		session->m_mode = 7;
		foyer->m_unk0xe4[1] = 0;
	}
	if (FUN_004b4a60("/explore")) {
		session->m_mode = 7;
		foyer->m_unk0xe4[1] = 1;
	}
	LegoS32 humanCount;
	if (session->m_unk0x00 == 1) {
		session->m_unk0x18 = 2;
		humanCount = 2;
	}
	else {
		humanCount = 1;
		switch (session->m_mode) {
		case 4:
			session->m_unk0x18 = GetSecondaryWorldRacerCount(session->m_selectedMode);
			break;
		case 5:
			session->m_unk0x18 = 2;
			break;
		case 6:
			session->m_unk0x18 =
				session->m_selectedMode == 0 || session->m_track == 5 ? 2 : GetSecondaryDefaultRacerCount();
			break;
		case 7:
			session->m_unk0x18 = 1;
			break;
		case 8:
			session->m_unk0x18 = 8;
			humanCount = 0;
			break;
		default:
			if (session->m_unk0x00 != 2) {
				session->m_unk0x18 = GetSecondaryDefaultRacerCount();
			}
			if (FUN_004b4a60("/racers")) {
				session->m_unk0x18 = FUN_004b4b30("/racers");
			}
		}
	}
	if (session->m_unk0x00 != 2) {
		switch (session->m_mode) {
		case 0:
		case 1:
			session->m_unk0x14 =
				*(LegoU32*) ((LegoU8*) settings + ((session->m_selectedMode + 2) * 5 + session->m_track) * 4);
			break;
		case 2:
			session->m_unk0x14 =
				*(LegoU32*) ((LegoU8*) session + *(LegoU32*) ((LegoU8*) session + 0x1a70) * 4 + 0x1ac8);
			break;
		case 5:
			session->m_unk0x14 = 999;
			break;
		case 6:
			session->m_unk0x14 = g_secondarySessionRaceValues[session->m_selectedMode * 5 + session->m_track];
			break;
		case 8:
			session->m_unk0x14 = 1;
			break;
		}
		LegoS32 mode = session->m_mode;
		session->m_unk0x24 = mode < 3 || (mode > 5 && mode != 7) ? settings->m_unk0xa4 : 0;
		LegoS32 i;
		for (i = 0; i < humanCount; ++i) {
			StartupPlayerSettings* player = GetSecondarySessionPlayer(i);
			player->m_unk0x0c = 1;
			player->m_unk0x08 = 1;
			player->m_unk0x18 = session->m_mode == 5 ? 9 : 0;
		}
		for (; i < (LegoS32) session->m_unk0x18; ++i) {
			StartupPlayerSettings* player = GetSecondarySessionPlayer(i);
			player->m_unk0x0c = session->m_mode == 5 ? 4 : 2;
			player->m_unk0x18 = session->m_mode == 5 ? 0x10 : 0;
			if (session->m_mode == 4) {
				player->m_unk0x08 = 3;
			}
			else if (session->m_mode == 6) {
				LegoU32 kind = 1;
				if (session->m_track == 5) {
					switch (session->m_selectedMode) {
					case 1:
						kind = 4;
						break;
					case 2:
						kind = 5;
						break;
					case 3:
						kind = 2;
						break;
					case 4:
						kind = 6;
						break;
					}
				}
				player->m_unk0x08 = kind;
			}
			else {
				player->m_unk0x08 = 1;
			}
		}
	}
	LegoU32 bindingIndex = 0;
	for (LegoS32 i = 0; i < (LegoS32) session->m_unk0x18; ++i) {
		StartupPlayerSettings* player = GetSecondarySessionPlayer(i);
		if (player->m_unk0x0c == 1 && bindingIndex < 2) {
			player->m_unk0x1c = *(LegoU32*) ((LegoU8*) settings + 0xa8 + bindingIndex * 4);
			GameControllerPairGroups* bindings =
				(GameControllerPairGroups*) ((LegoU8*) settings + 0xb4 + bindingIndex * 0xac);
			ConfigureControllerInputBindings(bindings, NULL, g_gameController.m_controllers[i]);
			++bindingIndex;
		}
		else {
			player->m_unk0x1c = 0;
		}
	}
	session->m_unk0x1c = session->m_unk0x00 == 1 ? settings->m_unk0x94 : 0;
	AssignSecondarySessionPlayers(manager, NULL);
	for (LegoS32 j = 0; j < (LegoS32) session->m_unk0x18; ++j) {
		StartupPlayerSettings* player = GetSecondarySessionPlayer(j);
		LegoU32 preset;
		const LegoU32* choices = (const LegoU32*) ((LegoU8*) player + 0x24);
		preset = choices[0] < 3 ? 0 : choices[1];
		player->m_partPreset = &g_characterPartPresets[preset];
	}
	for (LegoS32 k = 0; k < (LegoS32) session->m_unk0x18; ++k) {
		StartupPlayerSettings* player = GetSecondarySessionPlayer(k);
		if (player->m_unk0x0c == 1) {
			if (session->m_unk0x04 == 0) {
				player->m_unk0x338 = session->m_mode == 5 ? 0x3ecccccd : 0;
				player->m_unk0x33c = 0;
				player->m_unk0x340 = session->m_mode == 5 ? 0x3ecccccd : 0;
			}
			else {
				LegoFloat* gains = (LegoFloat*) &player->m_unk0x338;
				const LegoU32* progress = (const LegoU32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1dfc);
				gains[0] = GetFoyerModeProgressFraction(progress, NULL, 0);
				gains[1] = GetFoyerModeProgressFraction(progress, NULL, 1);
				gains[2] = GetFoyerModeProgressFraction(progress, NULL, 2);
			}
			if ((*(LegoU32*) ((LegoU8*) manager->m_unk0x17c + 0x178) & 0x2000) != 0) {
				player->m_unk0x338 = player->m_unk0x340 = 0x3f800000;
			}
		}
		else if (player->m_unk0x08 == 5) {
			player->m_unk0x338 = 0x3f800000;
			player->m_unk0x33c = 0;
			player->m_unk0x340 = 0x3dcccccd;
		}
		else if (player->m_unk0x08 == 6) {
			player->m_unk0x338 = 0x3f800000;
			player->m_unk0x33c = 0x3f000000;
			player->m_unk0x340 = 0x3f19999a;
		}
		else {
			player->m_unk0x338 = player->m_unk0x33c = player->m_unk0x340 = 0;
		}
	}
	for (LegoS32 m = 0; m < (LegoS32) session->m_unk0x18; ++m) {
		StartupPlayerSettings* player = GetSecondarySessionPlayer(m);
		player->m_unk0x14 = player->m_unk0x08 == 2 && player->m_unk0x0c != 1 ? 6 : m + 1;
	}
}

// STUB: LEGORACERS2 0x00428bf0
static LegoU32 GetSecondaryDefaultRacerCount()
{
	LegoU32 value = *(LegoU32*) ((LegoU8*) GetGameControllerSettings() + 0x20c);
	LegoU32 count = (LegoS32) value <= 2 ? 4 : (LegoS32) value <= 4 ? 6 : 8;
	if (FUN_004b4a60("/cars")) {
		LegoU32 requested = FUN_004b4b30("/cars");
		if (requested == 4 || requested == 6 || requested == 8) {
			return requested;
		}
	}
	return count;
}

// STUB: LEGORACERS2 0x00428c50
void __fastcall AssignSecondarySessionPlayers(SecondaryManager* p_manager, void*)
{
	LegoU32 parts[8][4] = {{0}};
	LegoU32 cars[8];
	LegoBool32 overrideHumans = ChooseSecondarySessionCharacters(p_manager, NULL, parts, cars);
	GameControllerSession* session = GetGameControllerSession();
	for (LegoS32 i = 0; i < (LegoS32) session->m_unk0x18; ++i) {
		StartupPlayerSettings* player = GetSecondarySessionPlayer(i);
		if ((player->m_unk0x0c == 1 && overrideHumans) || player->m_unk0x0c == 2) {
			memcpy(&player->m_carConfiguration, p_manager->m_secondaryData[cars[i]], 0x2f4);
			memcpy(player->m_unk0x324, parts[i], 0x10);
		}
		else if (player->m_unk0x0c == 1) {
			if (session->m_unk0x04 != 0) {
				LegoU32 secondPlayer = i != 0;
				const LegoU32* savedParts =
					GetSecondarySessionCharacter(g_gameController.m_unk0x8c, NULL, secondPlayer, &player->m_unk0x334);
				memcpy(player->m_unk0x324, savedParts, 0x10);
				const void* car;
				if (session->m_mode == 4 && session->m_selectedMode == 0) {
					car = p_manager->m_secondaryData[(0x234 - 0x180) / 4];
				}
				else {
					car =
						GetSecondarySessionCar(g_gameController.m_unk0x8c, NULL, secondPlayer, session->m_selectedMode);
				}
				memcpy(&player->m_carConfiguration, car, 0x2f4);
			}
		}
		else if (player->m_unk0x0c == 4) {
			StartupPlayerSettings* source = GetSecondarySessionPlayer(player->m_unk0x18 & 3);
			memcpy(&player->m_carConfiguration, &source->m_carConfiguration, 0x2f4);
			memcpy(player->m_unk0x324, source->m_unk0x324, 0x10);
		}
	}
}

// STUB: LEGORACERS2 0x00428e40
static LegoBool32 __fastcall ChooseSecondarySessionCharacters(
	SecondaryManager* p_manager,
	void*,
	LegoU32 p_parts[8][4],
	LegoU32* p_cars
)
{
	// GLOBAL: LEGORACERS2 0x0053eed4
	static const LegoS32 standardCharacters[8] = {0, 5, 2, 1, 3, 7, 6, 4};
	// GLOBAL: LEGORACERS2 0x0053eef4
	static const LegoS32 finalWorldCharacters[8] = {53, 54, 55, 56, 57, 58, 59, 60};
	GameControllerSession* session = GetGameControllerSession();
	LegoS32 carCount = g_secondaryWorldCarCounts[session->m_selectedMode];
	LegoS32 firstCar = g_secondaryWorldCarStarts[session->m_selectedMode];
	for (LegoS32 i = 0; i < (LegoS32) session->m_unk0x18; ++i) {
		StartupPlayerSettings* player = GetSecondarySessionPlayer(i);
		if (player->m_unk0x0c != 2) {
			continue;
		}
		LegoS32 character;
		switch (session->m_selectedMode) {
		case 1:
		case 3:
			character = standardCharacters[i];
			break;
		case 2:
			character = session->m_mode == 6 && session->m_track == 5 ? 31 : standardCharacters[i];
			break;
		case 4:
			character = session->m_mode == 6 && session->m_track == 5 ? 53 : finalWorldCharacters[i];
			break;
		default:
			if (session->m_mode != 6) {
				character = standardCharacters[i];
			}
			else {
				switch (session->m_track) {
				case 2:
					character = 9;
					break;
				case 3:
					character = 10;
					break;
				case 4:
					character = 11;
					break;
				default:
					character = 8;
				}
			}
		}
		const NpcTextSpeaker* speaker = &g_npcTextSpeakers[character];
		FindNamedCharacterBodyparts(
			(BodypartManager*) g_gameController.m_bodypartManager,
			NULL,
			speaker->m_name,
			p_parts[i]
		);
		p_cars[i] = speaker->m_characterIndex;
		if ((LegoS32) p_cars[i] == -2) {
			p_cars[i] = i % carCount + firstCar;
		}
	}
	if (session->m_selectedMode == 3 && (*(LegoU32*) ((LegoU8*) p_manager->m_unk0x17c + 0x178) & 0x200) != 0) {
		LegoU32 cassiopeia[4] = {0};
		FindNamedCharacterBodyparts(
			(BodypartManager*) g_gameController.m_bodypartManager,
			NULL,
			g_npcTextSpeakers[5].m_name,
			cassiopeia
		);
		for (LegoS32 j = 0; j < (LegoS32) session->m_unk0x18; ++j) {
			if (GetSecondarySessionPlayer(j)->m_unk0x0c == 1) {
				memcpy(p_parts[j], cassiopeia, 0x10);
				p_cars[j] = 0x1a;
			}
		}
		return TRUE;
	}
	return FALSE;
}

// FUNCTION: LEGORACERS2 0x00498270
LegoU32 __cdecl GetSecondaryWorldRacerCount(LegoU32 p_world)
{
	LegoU32 counts[5] = {8, 8, 8, 8, 8};
	return counts[p_world];
}
