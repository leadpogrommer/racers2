#include "gamecontrollerplayers.h"

#include "gamecontrollerinitialization.h"
#include "gamecontrollersession.h"
#include "winfile.h"

// GLOBAL: LEGORACERS2 0x0053ed7c
const LegoChar* g_defaultPlayerCarPaths[46] = {
	"Game Data\\Car Builder\\Sandy\\Police car.ccd",
	"Game Data\\Car Builder\\Sandy\\Beach Buggy.ccd",
	"Game Data\\Car Builder\\Sandy\\CG4x4.ccd",
	"Game Data\\Car Builder\\Sandy\\Dump Truck.ccd",
	"Game Data\\Car Builder\\Sandy\\Fire Truck.ccd",
	"Game Data\\Car Builder\\Sandy\\Post Van.ccd",
	"Game Data\\Car Builder\\Sandy\\Digger.ccd",
	"Game Data\\Car Builder\\Arctic\\Snow buggy.ccd",
	"Game Data\\Car Builder\\Arctic\\Polar Scout.ccd",
	"Game Data\\Car Builder\\Arctic\\Small skidoo.ccd",
	"Game Data\\Car Builder\\Arctic\\Mid skidoo.ccd",
	"Game Data\\Car Builder\\Arctic\\Outpost Vehicle.ccd",
	"Game Data\\Car Builder\\Arctic\\Snow Plough.ccd",
	"Game Data\\Car Builder\\Arctic\\Arm Vehicle.ccd",
	"Game Data\\Car Builder\\Arctic\\Arctic surfer.ccd",
	"Game Data\\Car Builder\\Adventure\\Bugatti.ccd",
	"Game Data\\Car Builder\\Adventure\\Brown Pickup.ccd",
	"Game Data\\Car Builder\\Adventure\\Dino Catcher.ccd",
	"Game Data\\Car Builder\\Adventure\\Jeep.ccd",
	"Game Data\\Car Builder\\Adventure\\Cat Truck.ccd",
	"Game Data\\Car Builder\\Adventure\\Dino Chase Car.ccd",
	"Game Data\\Car Builder\\Adventure\\Truck.ccd",
	"Game Data\\Car Builder\\Adventure\\TriTruck.ccd",
	"Game Data\\Car Builder\\Mars\\ATV.ccd",
	"Game Data\\Car Builder\\Mars\\Buggy1.ccd",
	"Game Data\\Car Builder\\Mars\\Buggy2.ccd",
	"Game Data\\Car Builder\\Mars\\Hover1.ccd",
	"Game Data\\Car Builder\\Mars\\Hover2.ccd",
	"Game Data\\Car Builder\\Mars\\Hover3.ccd",
	"Game Data\\Car Builder\\Mars\\Moon Rover.ccd",
	"Game Data\\Car Builder\\Mars\\Moon Rover2.ccd",
	"Game Data\\Car Builder\\CarCraze\\Xalax1.ccd",
	"Game Data\\Car Builder\\CarCraze\\Xalax2.ccd",
	"Game Data\\Car Builder\\CarCraze\\Xalax3.ccd",
	"Game Data\\Car Builder\\CarCraze\\Xalax4.ccd",
	"Game Data\\Car Builder\\CarCraze\\Xalax5.ccd",
	"Game Data\\Car Builder\\CarCraze\\Xalax6.ccd",
	"Game Data\\Car Builder\\CarCraze\\Rocket Racer.ccd",
	"Game Data\\Car Builder\\CarCraze\\Chill.ccd",
	"Game Data\\Car Builder\\CarCraze\\Duster.ccd",
	"Game Data\\Car Builder\\CarCraze\\Freeze.ccd",
	"Game Data\\Car Builder\\CarCraze\\Ghost.ccd",
	"Game Data\\Car Builder\\CarCraze\\Lightor.ccd",
	"Game Data\\Car Builder\\CarCraze\\Snake.ccd",
	"Game Data\\Car Builder\\CarCraze\\Warrior.ccd",
	"Game Data\\Car Builder\\Sandy\\Bonus Car.ccd"
};

// STUB: LEGORACERS2 0x00403380
void LoadDefaultPlayerCar(const CharBuffer128* p_path, GameControllerItemBlock* p_configuration)
{
	WinFile file(p_path->m_data, 0, NULL);
	file.Read(p_configuration, 0x2f4, 1);
	file.CloseManaged();
}

// STUB: LEGORACERS2 0x0040ac30
void InitializeGameControllerPlayers()
{
	for (LegoU32 i = 0; i < 8; ++i) {
		StartupPlayerSettings* settings =
			(StartupPlayerSettings*) ((LegoU8*) g_gameControllerSession + 0x4c + i * 0x344);
		settings->m_index = i;
		settings->m_unk0x04 = -1;
		settings->m_unk0x08 = 1;
		settings->m_unk0x0c = 2;
		settings->m_unk0x10 = -1;
		settings->m_unk0x14 = i + 1;
		settings->m_unk0x18 = i == 0 ? 9 : 0;
		settings->m_unk0x1c = 0;
		CharBuffer128 path(g_defaultPlayerCarPaths[i]);
		LoadDefaultPlayerCar(&path, &settings->m_carConfiguration);
		settings->m_unk0x314.m_x = 0.0f;
		settings->m_unk0x314.m_y = 0.1f;
		settings->m_unk0x314.m_z = -0.4f;
		settings->m_partPreset = g_characterPartPresets;
		for (LegoU32 j = 0; j < 4; ++j) {
			settings->m_unk0x324[j] = 0;
		}
		settings->m_unk0x334 = 1;
		settings->m_unk0x338 = 0;
		settings->m_unk0x33c = 0;
		settings->m_unk0x340 = 0;
	}
	StartupPlayerSettings* first = (StartupPlayerSettings*) ((LegoU8*) g_gameControllerSession + 0x4c);
	first->m_unk0x0c = 1;
	first->m_unk0x10 = 0;
}
