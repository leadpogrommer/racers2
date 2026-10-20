#include "frontend/preview/menucharacterpreviewtuning.h"

// GLOBAL: LEGORACERS2 0x0056ba28
MenuCharacterPreviewTuning g_menuCharacterPreviewTuning;

// FUNCTION: LEGORACERS2 0x0042c150
static void InitializeCharacterPreviewSize()
{
	g_menuCharacterPreviewTuning.m_size[0] = 0.3f;
	g_menuCharacterPreviewTuning.m_size[1] = 0.5f;
}

// FUNCTION: LEGORACERS2 0x0042c180
static void InitializeCharacterPreviewPosition()
{
	g_menuCharacterPreviewTuning.m_characterPosition.m_x = -100.0f;
	g_menuCharacterPreviewTuning.m_characterPosition.m_y = 0;
	g_menuCharacterPreviewTuning.m_characterPosition.m_z = 0;
}

// FUNCTION: LEGORACERS2 0x0042c1b0
static void InitializeCharacterPreviewCameraPosition()
{
	g_menuCharacterPreviewTuning.m_cameraPosition.m_x = -99.6f;
	g_menuCharacterPreviewTuning.m_cameraPosition.m_y = 1.5f;
	g_menuCharacterPreviewTuning.m_cameraPosition.m_z = 3.0f;
}

// FUNCTION: LEGORACERS2 0x0042c1e0
static void InitializeCharacterPreviewTargetOffset()
{
	g_menuCharacterPreviewTuning.m_targetOffset.m_x = -0.4f;
	g_menuCharacterPreviewTuning.m_targetOffset.m_y = -0.9f;
	g_menuCharacterPreviewTuning.m_targetOffset.m_z = -3.0f;
}

// FUNCTION: LEGORACERS2 0x0042c210
static void InitializeCharacterPreviewAmbientColor()
{
	g_menuCharacterPreviewTuning.m_ambientColor[3] = 255;
	g_menuCharacterPreviewTuning.m_ambientColor[0] = 150;
	g_menuCharacterPreviewTuning.m_ambientColor[1] = 150;
	g_menuCharacterPreviewTuning.m_ambientColor[2] = 150;
}

// FUNCTION: LEGORACERS2 0x0042c240
static void InitializeCharacterPreviewFirstAmbient()
{
	g_menuCharacterPreviewTuning.m_firstAmbient[0] = 0.5f;
	g_menuCharacterPreviewTuning.m_firstAmbient[1] = 0.5f;
	g_menuCharacterPreviewTuning.m_firstAmbient[2] = 0.5f;
	g_menuCharacterPreviewTuning.m_firstAmbient[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042c280
static void InitializeCharacterPreviewFirstDiffuse()
{
	g_menuCharacterPreviewTuning.m_firstDiffuse[0] = 1.0f;
	g_menuCharacterPreviewTuning.m_firstDiffuse[1] = 1.0f;
	g_menuCharacterPreviewTuning.m_firstDiffuse[2] = 1.0f;
	g_menuCharacterPreviewTuning.m_firstDiffuse[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042c2c0
static void InitializeCharacterPreviewFirstSpecular()
{
	g_menuCharacterPreviewTuning.m_firstSpecular[0] = 0.5f;
	g_menuCharacterPreviewTuning.m_firstSpecular[1] = 0.5f;
	g_menuCharacterPreviewTuning.m_firstSpecular[2] = 0.5f;
	g_menuCharacterPreviewTuning.m_firstSpecular[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042c300
static void InitializeCharacterPreviewFirstDirection()
{
	g_menuCharacterPreviewTuning.m_firstDirection.m_x = 0;
	g_menuCharacterPreviewTuning.m_firstDirection.m_y = -0.7070000171661377f;
	g_menuCharacterPreviewTuning.m_firstDirection.m_z = -0.7070000171661377f;
}

// FUNCTION: LEGORACERS2 0x0042c330
static void InitializeCharacterPreviewSecondAmbient()
{
	g_menuCharacterPreviewTuning.m_secondAmbient[0] = 0.5f;
	g_menuCharacterPreviewTuning.m_secondAmbient[1] = 0.5f;
	g_menuCharacterPreviewTuning.m_secondAmbient[2] = 0.5f;
	g_menuCharacterPreviewTuning.m_secondAmbient[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042c370
static void InitializeCharacterPreviewSecondDiffuse()
{
	g_menuCharacterPreviewTuning.m_secondDiffuse[0] = 1.0f;
	g_menuCharacterPreviewTuning.m_secondDiffuse[1] = 1.0f;
	g_menuCharacterPreviewTuning.m_secondDiffuse[2] = 1.0f;
	g_menuCharacterPreviewTuning.m_secondDiffuse[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042c3b0
static void InitializeCharacterPreviewSecondSpecular()
{
	g_menuCharacterPreviewTuning.m_secondSpecular[0] = 0.5f;
	g_menuCharacterPreviewTuning.m_secondSpecular[1] = 0.5f;
	g_menuCharacterPreviewTuning.m_secondSpecular[2] = 0.5f;
	g_menuCharacterPreviewTuning.m_secondSpecular[3] = 1.0f;
}

// FUNCTION: LEGORACERS2 0x0042c3f0
static void InitializeCharacterPreviewSecondDirection()
{
	g_menuCharacterPreviewTuning.m_secondDirection.m_x = -0.7070000171661377f;
	g_menuCharacterPreviewTuning.m_secondDirection.m_y = 0;
	g_menuCharacterPreviewTuning.m_secondDirection.m_z = -0.7070000171661377f;
}

struct InitializeCharacterPreviewTuningAtStartup {
	InitializeCharacterPreviewTuningAtStartup()
	{
		InitializeCharacterPreviewSize();
		InitializeCharacterPreviewPosition();
		InitializeCharacterPreviewCameraPosition();
		InitializeCharacterPreviewTargetOffset();
		InitializeCharacterPreviewAmbientColor();
		InitializeCharacterPreviewFirstAmbient();
		InitializeCharacterPreviewFirstDiffuse();
		InitializeCharacterPreviewFirstSpecular();
		InitializeCharacterPreviewFirstDirection();
		InitializeCharacterPreviewSecondAmbient();
		InitializeCharacterPreviewSecondDiffuse();
		InitializeCharacterPreviewSecondSpecular();
		InitializeCharacterPreviewSecondDirection();
	}
};
static InitializeCharacterPreviewTuningAtStartup g_initializeCharacterPreviewTuningAtStartup;
