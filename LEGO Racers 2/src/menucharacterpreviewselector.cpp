#include "menucharacterpreviewselector.h"

#include "charactervisual.h"
#include "displayconfig.h"
#include "gamecontroller.h"
#include "gamelight.h"
#include "gameobjectrenderprep.h"
#include "menucharacterpreviewtuning.h"
#include "secondarymanagerleases.h"

#include <string.h>

struct MenuPreviewCameraRectangleView {
	undefined m_prefix[0x1a4];
	LegoFloat m_viewScale;
	undefined m_gap0x1a8[8];
	LegoS32 m_left;
	LegoS32 m_top;
	LegoS32 m_width;
	LegoS32 m_height;
	LegoU32 m_enabled;
};
DECOMP_STATIC_ASSERT(offsetof(MenuPreviewCameraRectangleView, m_width) == 0x1b8);
DECOMP_STATIC_ASSERT(offsetof(MenuPreviewCameraRectangleView, m_enabled) == 0x1c0);

static LegoS32 RoundPreviewPixel(LegoFloat p_value)
{
	// FISTP observes the current x87 rounding mode; a C++ cast truncates.
	LegoS32 result;
	__asm {
		fld p_value
		fistp result
	}
	return result;
}

static void __fastcall CharacterPreviewBounds(MenuWidgetBase* p_widget, void*)
{
	((MenuCharacterPreviewSelector*) p_widget)->UpdateBounds();
}
static void __fastcall CharacterPreviewRelease(MenuWidgetBase* p_widget, void*)
{
	((MenuCharacterPreviewSelector*) p_widget)->ReleasePreviewResources();
}
static void __fastcall CharacterPreviewReset(MenuWidgetBase* p_widget, void*)
{
	((MenuCharacterPreviewSelector*) p_widget)->Reset();
}
static LegoU32 __fastcall CharacterPreviewInput(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
)
{
	return ((MenuCharacterPreviewSelector*) p_widget)->HandleInput(p_result, p_input);
}
static void __fastcall CharacterPreviewClip(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_first,
	const LegoFloat* p_second
)
{
	((MenuCharacterPreviewSelector*) p_widget)->SetClipRectangle(p_first, p_second);
}

const MenuWidgetDispatch g_menuCharacterPreviewSelectorDispatch = {
	CharacterPreviewBounds,
	SetMenuWidgetFont,
	CharacterPreviewRelease,
	CharacterPreviewReset,
	CharacterPreviewInput,
	CharacterPreviewClip
};

// STUB: LEGORACERS2 0x0042c410
MenuCharacterPreviewSelector::MenuCharacterPreviewSelector() : MenuWidgetBase(-1)
{
	m_disabled = 0;
	for (LegoS32 index = 0; index < 10; ++index) {
		for (LegoS32 part = 0; part < 4; ++part) {
			m_choices[index].m_parts[part] = 0;
		}
	}
	m_count = m_selection = 0;
	m_camera = m_previewObject = m_ambient = NULL;
	m_cameraLease = m_previewLease = m_ambientLease = -1;
	for (LegoS32 light = 0; light < 2; ++light) {
		m_lights[light] = NULL;
		m_lightLeases[light] = -1;
	}
	m_size[0] = m_size[1] = m_position[0] = m_position[1] = 0;
	m_vtable = &g_menuCharacterPreviewSelectorDispatch;
}

// STUB: LEGORACERS2 0x0042c4b0
void MenuCharacterPreviewSelector::ReleasePreviewResources()
{
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (m_cameraLease != -1) {
		ReleaseMenuPreviewCamera(manager, NULL, &m_camera, m_cameraLease);
		m_cameraLease = -1;
	}
	if (m_previewLease != -1) {
		ReleaseMenuPreviewCharacter(manager, NULL, &m_previewObject, m_previewLease);
		m_previewLease = -1;
	}
	ReleaseLighting();
}

// FUNCTION: LEGORACERS2 0x0042c670
void MenuCharacterPreviewSelector::AddChoice(const MenuCharacterPreviewChoice* p_choice)
{
	m_choices[m_count] = *p_choice;
	++m_count;
}

// FUNCTION: LEGORACERS2 0x0042c6b0
void MenuCharacterPreviewSelector::SelectChoice(LegoS32 p_index)
{
	m_selection = p_index;
	ApplyChoice(p_index);
}

// STUB: LEGORACERS2 0x0042c6d0
LegoU32 MenuCharacterPreviewSelector::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	if (m_disabled == 0) {
		LegoS32 direction = GetNavigationDirection(p_input, 1);
		if (direction != 0) {
			LegoS32 previous = m_selection;
			do {
				m_selection += direction;
				if (m_selection < 0) {
					m_selection = m_unk0x14 == 0 ? 0 : m_count - 1;
				}
				else if (m_count - 1 < m_selection) {
					m_selection = m_unk0x14 == 0 ? m_count - 1 : 0;
				}
				const MenuCharacterPreviewChoice& choice = m_choices[m_selection];
				if ((choice.m_parts[0] >= 0 && choice.m_parts[1] >= 0 && choice.m_parts[2] >= 0 &&
					 choice.m_parts[3] >= 0) ||
					m_selection == previous) {
					break;
				}
			} while (TRUE);
			if (m_selection != previous) {
				ApplyChoice(m_selection);
				*p_result = 3;
			}
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x0042c7a0
void MenuCharacterPreviewSelector::Reset()
{
	if (m_disabled == 0) {
		ReleaseChildSprites();
	}
	ResetState();
	ReleasePreviewResources();
}

// STUB: LEGORACERS2 0x0042c810
void MenuCharacterPreviewSelector::ApplyChoice(LegoS32 p_index)
{
	CharacterVisual* character = (CharacterVisual*) m_previewObject;
	const MenuCharacterPreviewChoice& choice = m_choices[p_index];
	character->SetHeadModel(choice.m_parts[0]);
	character->SetFaceGroup(choice.m_parts[1]);
	character->SetFaceVariant(0);
	character->SetTorsoTexture(choice.m_parts[2], 0);
	character->SetLegTexture(choice.m_parts[3]);
}

// STUB: LEGORACERS2 0x0042c880
void MenuCharacterPreviewSelector::SetClipRectangle(const LegoFloat* p_first, const LegoFloat* p_second)
{
	SetChildClipRectangle(p_first, p_second);
	LegoFloat size[2] = {p_second[0] - p_first[0], p_second[1] - p_first[1]};
	if (m_size[0] < size[0]) {
		size[0] = m_size[0];
	}
	if (m_size[1] < size[1]) {
		size[1] = m_size[1];
	}
	ConfigureViewport(m_position, size);
}

// STUB: LEGORACERS2 0x0042c910
void MenuCharacterPreviewSelector::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	LegoFloat width = (LegoFloat) (LegoS32) g_displayConfiguration->m_width;
	LegoFloat height = (LegoFloat) (LegoS32) g_displayConfiguration->m_height;
	if (0.001f <= width && 0.001f <= height) {
		MenuPreviewCameraRectangleView* camera = (MenuPreviewCameraRectangleView*) m_camera;
		LegoFloat scaleX = 1.0f / width;
		LegoFloat scaleY = 1.0f / height;
		LegoFloat left = (LegoFloat) camera->m_left * scaleX;
		LegoFloat top = (LegoFloat) camera->m_top * scaleY;
		LegoFloat right = (LegoFloat) camera->m_width * scaleX + left;
		LegoFloat bottom = (LegoFloat) camera->m_height * scaleY + top;
		if (left < m_bounds[0]) {
			m_bounds[0] = left;
		}
		if (top < m_bounds[1]) {
			m_bounds[1] = top;
		}
		if (m_bounds[2] < right) {
			m_bounds[2] = right;
		}
		if (m_bounds[3] < bottom) {
			m_bounds[3] = bottom;
		}
	}
}

// FUNCTION: LEGORACERS2 0x0042ca10
LegoS32 MenuCharacterPreviewSelector::GetChoice(MenuCharacterPreviewChoice* p_choice)
{
	*p_choice = m_choices[m_selection];
	return m_selection;
}

// STUB: LEGORACERS2 0x0042ca50
void MenuCharacterPreviewSelector::InitializeLighting()
{
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (m_ambientLease == -1) {
		m_ambientLease = AcquireMenuPreviewAmbient(manager, NULL, &m_ambient);
	}
	for (LegoS32 index = 0; index < 2; ++index) {
		if (m_lightLeases[index] == -1) {
			m_lightLeases[index] = AcquireMenuPreviewLight(manager, NULL, &m_lights[index]);
		}
	}
	GameAmbientLight* ambient = (GameAmbientLight*) m_ambient;
	memcpy(ambient->m_color, g_menuCharacterPreviewTuning.m_ambientColor, sizeof(ambient->m_color));
	ambient->ApplyToRenderer();
	GameDirectionalLight* first = (GameDirectionalLight*) m_lights[0];
	memcpy(&first->m_properties.m_ambient, g_menuCharacterPreviewTuning.m_firstAmbient, sizeof(D3DCOLORVALUE));
	memcpy(&first->m_properties.m_diffuse, g_menuCharacterPreviewTuning.m_firstDiffuse, sizeof(D3DCOLORVALUE));
	memcpy(&first->m_properties.m_specular, g_menuCharacterPreviewTuning.m_firstSpecular, sizeof(D3DCOLORVALUE));
	first->m_direction = g_menuCharacterPreviewTuning.m_firstDirection;
	first->m_properties.SetEnabled(1);
	first->UpdateRendererLight();
	GameDirectionalLight* second = (GameDirectionalLight*) m_lights[1];
	memcpy(&second->m_properties.m_ambient, g_menuCharacterPreviewTuning.m_secondAmbient, sizeof(D3DCOLORVALUE));
	memcpy(&second->m_properties.m_diffuse, g_menuCharacterPreviewTuning.m_secondDiffuse, sizeof(D3DCOLORVALUE));
	memcpy(&second->m_properties.m_specular, g_menuCharacterPreviewTuning.m_secondSpecular, sizeof(D3DCOLORVALUE));
	second->m_direction = g_menuCharacterPreviewTuning.m_secondDirection;
	second->m_properties.SetEnabled(1);
	second->UpdateRendererLight();
}

// STUB: LEGORACERS2 0x0042cc40
void MenuCharacterPreviewSelector::ReleaseLighting()
{
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (m_ambientLease != -1) {
		ReleaseMenuPreviewAmbient(manager, NULL, &m_ambient, m_ambientLease);
		m_ambientLease = -1;
	}
	for (LegoS32 index = 0; index < 2; ++index) {
		if (m_lightLeases[index] != -1) {
			ReleaseMenuPreviewLight(manager, NULL, &m_lights[index], m_lightLeases[index]);
			m_lightLeases[index] = -1;
		}
	}
}

// STUB: LEGORACERS2 0x0042cca0
void MenuCharacterPreviewSelector::ConfigureViewport(const LegoFloat* p_position, const LegoFloat* p_size)
{
	if (m_camera != NULL) {
		MenuPreviewCameraRectangleView* camera = (MenuPreviewCameraRectangleView*) m_camera;
		if (0.01f <= p_size[0] && 0.01f <= p_size[1]) {
			LegoFloat left = p_size[0] * -0.5f + p_position[0];
			LegoFloat top = p_size[1] * -0.5f + p_position[1];
			LegoFloat width = (LegoFloat) (LegoS32) g_displayConfiguration->m_width;
			LegoFloat height = (LegoFloat) (LegoS32) g_displayConfiguration->m_height;
			LegoS32 viewportHeight = RoundPreviewPixel(height * p_size[1]);
			LegoS32 viewportWidth = RoundPreviewPixel(width * p_size[0]);
			LegoS32 viewportTop = RoundPreviewPixel(top * height);
			LegoS32 viewportLeft = RoundPreviewPixel(left * width);
			SetGameObjectViewport(camera, NULL, viewportLeft, viewportTop, viewportWidth, viewportHeight);
			camera->m_viewScale = 50.0f;
			UpdateGameObjectRenderGeometry(camera, NULL);
			Vector3 target;
			target.m_x =
				g_menuCharacterPreviewTuning.m_cameraPosition.m_x + g_menuCharacterPreviewTuning.m_targetOffset.m_x;
			target.m_y =
				g_menuCharacterPreviewTuning.m_cameraPosition.m_y + g_menuCharacterPreviewTuning.m_targetOffset.m_y;
			target.m_z =
				g_menuCharacterPreviewTuning.m_cameraPosition.m_z + g_menuCharacterPreviewTuning.m_targetOffset.m_z;
			Vector3 up = {0, 1, 0};
			SetGameObjectLookAt(camera, NULL, &g_menuCharacterPreviewTuning.m_cameraPosition, &target, &up);
			return;
		}
		camera->m_enabled = 0;
	}
}
