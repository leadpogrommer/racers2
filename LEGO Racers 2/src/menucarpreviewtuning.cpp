#include "menucarpreviewtuning.h"

// GLOBAL: LEGORACERS2 0x0056b958
MenuCarPreviewTuning g_menuCarPreviewTuning;

// STUB: LEGORACERS2 0x0042b3b0
static void InitializeMenuCarPreviewCameraDistance()
{
	g_menuCarPreviewTuning.m_cameraDistance = 6.2831854820251465f;
}

// FUNCTION: LEGORACERS2 0x0042b3d0
static void InitializeMenuCarPreviewSize()
{
	g_menuCarPreviewTuning.m_size[0] = 0.4f;
	g_menuCarPreviewTuning.m_size[1] = 0.5f;
}

// FUNCTION: LEGORACERS2 0x0042b400
static void InitializeMenuCarPreviewVehicleParameters()
{
	g_menuCarPreviewTuning.m_vehicleParameters[0] = 100.0f;
	g_menuCarPreviewTuning.m_vehicleParameters[1] = 0.0f;
	g_menuCarPreviewTuning.m_vehicleParameters[2] = 0.0f;
}

// FUNCTION: LEGORACERS2 0x0042b430
static void InitializeMenuCarPreviewCameraPosition()
{
	g_menuCarPreviewTuning.m_cameraPosition.m_x = 116.0f;
	g_menuCarPreviewTuning.m_cameraPosition.m_y = 8.4f;
	g_menuCarPreviewTuning.m_cameraPosition.m_z = 10.0f;
}

// FUNCTION: LEGORACERS2 0x0042b460
static void InitializeMenuCarPreviewTargetOffset()
{
	g_menuCarPreviewTuning.m_targetOffset.m_x = -16.2f;
	g_menuCarPreviewTuning.m_targetOffset.m_y = -6.0f;
	g_menuCarPreviewTuning.m_targetOffset.m_z = -10.0f;
}

// FUNCTION: LEGORACERS2 0x0042b490
static void InitializeMenuCarPreviewAmbientColor()
{
	g_menuCarPreviewTuning.m_ambientColor[0] = 150;
	g_menuCarPreviewTuning.m_ambientColor[1] = 150;
	g_menuCarPreviewTuning.m_ambientColor[2] = 150;
	g_menuCarPreviewTuning.m_ambientColor[3] = 255;
}

// STUB: LEGORACERS2 0x0042b4c0
static void InitializeMenuCarPreviewFirstAmbient()
{
	for (LegoS32 i = 0; i < 3; ++i) {
		g_menuCarPreviewTuning.m_firstAmbient[i] = 0.5f;
	}
	g_menuCarPreviewTuning.m_firstAmbient[3] = 1.0f;
}

// STUB: LEGORACERS2 0x0042b500
static void InitializeMenuCarPreviewFirstDiffuse()
{
	for (LegoS32 i = 0; i < 4; ++i) {
		g_menuCarPreviewTuning.m_firstDiffuse[i] = 1.0f;
	}
}

// STUB: LEGORACERS2 0x0042b540
static void InitializeMenuCarPreviewFirstSpecular()
{
	for (LegoS32 i = 0; i < 3; ++i) {
		g_menuCarPreviewTuning.m_firstSpecular[i] = 0.5f;
	}
	g_menuCarPreviewTuning.m_firstSpecular[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042b580
static void InitializeMenuCarPreviewFirstDirection()
{
	g_menuCarPreviewTuning.m_firstDirection.m_x = 0.0f;
	g_menuCarPreviewTuning.m_firstDirection.m_y = -0.7070000171661377f;
	g_menuCarPreviewTuning.m_firstDirection.m_z = -0.7070000171661377f;
}

// STUB: LEGORACERS2 0x0042b5b0
static void InitializeMenuCarPreviewSecondAmbient()
{
	for (LegoS32 i = 0; i < 3; ++i) {
		g_menuCarPreviewTuning.m_secondAmbient[i] = 0.5f;
	}
	g_menuCarPreviewTuning.m_secondAmbient[3] = 1.0f;
}

// STUB: LEGORACERS2 0x0042b5f0
static void InitializeMenuCarPreviewSecondDiffuse()
{
	for (LegoS32 i = 0; i < 4; ++i) {
		g_menuCarPreviewTuning.m_secondDiffuse[i] = 1.0f;
	}
}

// STUB: LEGORACERS2 0x0042b630
static void InitializeMenuCarPreviewSecondSpecular()
{
	for (LegoS32 i = 0; i < 3; ++i) {
		g_menuCarPreviewTuning.m_secondSpecular[i] = 0.5f;
	}
	g_menuCarPreviewTuning.m_secondSpecular[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042b670
static void InitializeMenuCarPreviewSecondDirection()
{
	g_menuCarPreviewTuning.m_secondDirection.m_x = -0.7070000171661377f;
	g_menuCarPreviewTuning.m_secondDirection.m_y = 0.0f;
	g_menuCarPreviewTuning.m_secondDirection.m_z = -0.7070000171661377f;
}

// Called in the same order as the retail .CRT initializer entries.
void InitializeMenuCarPreviewTuning()
{
	InitializeMenuCarPreviewCameraDistance();
	InitializeMenuCarPreviewSize();
	InitializeMenuCarPreviewVehicleParameters();
	InitializeMenuCarPreviewCameraPosition();
	InitializeMenuCarPreviewTargetOffset();
	InitializeMenuCarPreviewAmbientColor();
	InitializeMenuCarPreviewFirstAmbient();
	InitializeMenuCarPreviewFirstDiffuse();
	InitializeMenuCarPreviewFirstSpecular();
	InitializeMenuCarPreviewFirstDirection();
	InitializeMenuCarPreviewSecondAmbient();
	InitializeMenuCarPreviewSecondDiffuse();
	InitializeMenuCarPreviewSecondSpecular();
	InitializeMenuCarPreviewSecondDirection();
}

struct MenuCarPreviewTuningAtStartup {
	MenuCarPreviewTuningAtStartup() { InitializeMenuCarPreviewTuning(); }
};
static MenuCarPreviewTuningAtStartup g_menuCarPreviewTuningAtStartup;
