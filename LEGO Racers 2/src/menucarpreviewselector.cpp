#include "menucarpreviewselector.h"

#include "cardisplay.h"
#include "displayconfig.h"
#include "gamecontroller.h"
#include "gamelight.h"
#include "gameobjectrenderprep.h"
#include "memoryheap.h"
#include "menucarpreviewtuning.h"
#include "secondarymanager.h"
#include "secondarymanagerleases.h"
#include "vector3.h"

struct PreviewCameraRectangle {
	undefined m_prefix[0x1b0];
	LegoS32 m_left;
	LegoS32 m_top;
	LegoS32 m_width;
	LegoS32 m_height;
};

static LegoS32 RoundPreviewPixel(LegoFloat p_value)
{
	LegoS32 result;
	__asm {
		fld p_value
		fistp result
	}
	return result;
}

static void __fastcall CarPreviewUpdateBounds(MenuWidgetBase* p_widget, void*)
{
	((MenuCarPreviewSelector*) p_widget)->UpdateBounds();
}

static void __fastcall CarPreviewSetFont(MenuWidgetBase* p_widget, void*, BitmapFont* p_font)
{
	((MenuCarPreviewSelector*) p_widget)->SetFont(p_font);
}

static void __fastcall CarPreviewRelease(MenuWidgetBase* p_widget, void*)
{
	((MenuCarPreviewSelector*) p_widget)->ReleaseResources();
}

static void __fastcall CarPreviewReset(MenuWidgetBase* p_widget, void*)
{
	((MenuCarPreviewSelector*) p_widget)->Reset();
}

static LegoU32 __fastcall CarPreviewInput(MenuWidgetBase* p_widget, void*, LegoU32* p_result, const LegoU32* p_input)
{
	return ((MenuCarPreviewSelector*) p_widget)->HandleInput(p_result, p_input);
}

static void __fastcall CarPreviewClip(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuCarPreviewSelector*) p_widget)->SetClipRectangle(p_topLeft, p_bottomRight);
}

const MenuWidgetDispatch g_menuCarPreviewSelectorDispatch =
	{CarPreviewUpdateBounds, CarPreviewSetFont, CarPreviewRelease, CarPreviewReset, CarPreviewInput, CarPreviewClip};

// STUB: LEGORACERS2 0x0042b690
MenuCarPreviewSelector::MenuCarPreviewSelector() : MenuWidgetBase(-1)
{
	m_vtable = &g_menuCarPreviewSelectorDispatch;
	m_cameraLease = -1;
	m_disabled = 0;
	m_count = 0;
	m_selection = 0;
	m_camera = NULL;
	m_carDisplay = NULL;
	m_ambient = NULL;
	m_ambientLease = -1;
	m_lights[0] = m_lights[1] = NULL;
	m_lightLeases[0] = m_lightLeases[1] = -1;
	m_size[0] = m_size[1] = 0.0f;
	m_position[0] = m_position[1] = 0.0f;
}

// STUB: LEGORACERS2 0x0042b710
void MenuCarPreviewSelector::SetFont(BitmapFont* p_font)
{
	m_font = p_font;
	m_carDisplay = AllocateHeapMemory(sizeof(CarDisplay));
	if (m_carDisplay != NULL) {
		CarDisplay* display = (CarDisplay*) m_carDisplay;
		display->m_resourceTable = ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x74;
		display->m_activeCount = 0;
		display->m_selectedChoice = 0;
	}
	InitializeCarDisplay((CarDisplay*) m_carDisplay);
}

// STUB: LEGORACERS2 0x0042b770
void MenuCarPreviewSelector::ReleaseResources()
{
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (m_cameraLease != -1) {
		ReleaseMenuPreviewCamera(manager, NULL, &m_camera, m_cameraLease);
		m_cameraLease = -1;
	}
	ReleaseLighting();
	ReleaseCarDisplay((CarDisplay*) m_carDisplay);
	FreeHeapMemory(m_carDisplay);
}

// STUB: LEGORACERS2 0x0042b7c0
void MenuCarPreviewSelector::Initialize(
	LegoS32 p_initialValue,
	const LegoFloat* p_position,
	LegoFloat p_scale,
	LegoU32 p_disabled,
	LegoBool32 p_lighting,
	LegoU32 p_wrap
)
{
	BeginState();
	m_initialValue = p_initialValue;
	m_unk0x14 = p_wrap;
	m_disabled = p_disabled;
	m_count = 0;
	m_selection = 0;
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	m_cameraLease = AcquireMenuPreviewCamera(manager, NULL, &m_camera);
	m_size[0] = p_scale * g_menuCarPreviewTuning.m_size[0];
	m_size[1] = p_scale * g_menuCarPreviewTuning.m_size[1];
	m_position[0] = p_position[0];
	m_position[1] = p_position[1] + m_size[1] * 0.5f;
	((CarDisplay*) m_carDisplay)->m_resourceTable =
		((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x74;
	if (p_lighting != FALSE) {
		InitializeLighting();
	}
	ConfigureViewport(m_position, m_size);
	if (m_disabled == 0) {
		m_vtable->m_updateBounds(this, NULL);
		LegoFloat center[2] = {(m_bounds[0] + m_bounds[2]) * 0.5f, (m_bounds[1] + m_bounds[3]) * 0.5f};
		CreateChildSprites(center, center[0] - m_bounds[0], -1, 1.0f);
	}
	m_vtable->m_updateBounds(this, NULL);
}

// FUNCTION: LEGORACERS2 0x0042b900
void MenuCarPreviewSelector::AddChoice(LegoS32 p_choice)
{
	m_choices[m_count] = p_choice;
	++m_count;
}

// STUB: LEGORACERS2 0x0042b920
void MenuCarPreviewSelector::SelectChoice(LegoS32 p_index)
{
	static const LegoFloat identity[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	m_selection = p_index;
	ApplyCarDisplayChoice(
		(CarDisplay*) m_carDisplay,
		NULL,
		(const LegoU32*) (size_t) (LegoU32) m_choices[p_index],
		&g_menuCarPreviewTuning.m_vehicleParameters[0],
		identity,
		1
	);
}

// STUB: LEGORACERS2 0x0042b990
LegoU32 MenuCarPreviewSelector::HandleInput(LegoU32* p_result, const LegoU32* p_input)
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
				void* choice = (void*) (size_t) m_choices[m_selection];
				if (*(LegoU32*) choice != 0 || m_selection == previous) {
					break;
				}
			} while (TRUE);
			if (m_selection != previous) {
				SelectChoice(m_selection);
				*p_result = 3;
			}
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x0042baa0
void MenuCarPreviewSelector::Reset()
{
	if (m_disabled == 0) {
		ReleaseChildSprites();
	}
	ResetState();
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (m_cameraLease != -1) {
		ReleaseMenuPreviewCamera(manager, NULL, &m_camera, m_cameraLease);
		m_cameraLease = -1;
	}
	ReleaseLighting();
	ResetCarDisplay((CarDisplay*) m_carDisplay);
}

// STUB: LEGORACERS2 0x0042bb00
void MenuCarPreviewSelector::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	LegoFloat size[2] = {p_bottomRight[0] - p_topLeft[0], p_bottomRight[1] - p_topLeft[1]};
	if (m_size[0] < size[0]) {
		size[0] = m_size[0];
	}
	if (m_size[1] < size[1]) {
		size[1] = m_size[1];
	}
	ConfigureViewport(m_position, size);
}

// STUB: LEGORACERS2 0x0042bb90
void MenuCarPreviewSelector::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	LegoFloat width = (LegoFloat) (LegoS32) g_displayConfiguration->m_width;
	LegoFloat height = (LegoFloat) (LegoS32) g_displayConfiguration->m_height;
	if (0.001f <= width && 0.001f <= height) {
		PreviewCameraRectangle* camera = (PreviewCameraRectangle*) m_camera;
		LegoFloat inverseWidth = 1.0f / width;
		LegoFloat inverseHeight = 1.0f / height;
		LegoFloat left = (LegoFloat) camera->m_left * inverseWidth;
		LegoFloat top = (LegoFloat) camera->m_top * inverseHeight;
		LegoFloat right = (LegoFloat) camera->m_width * inverseWidth + left;
		LegoFloat bottom = (LegoFloat) camera->m_height * inverseHeight + top;
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

// FUNCTION: LEGORACERS2 0x0042bc90
LegoS32 MenuCarPreviewSelector::GetChoice(LegoS32* p_choice)
{
	*p_choice = m_choices[m_selection];
	return m_selection;
}

// STUB: LEGORACERS2 0x0042bcb0
void MenuCarPreviewSelector::InitializeLighting()
{
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (m_ambientLease == -1) {
		m_ambientLease = AcquireMenuPreviewAmbient(manager, NULL, &m_ambient);
	}
	for (LegoS32 i = 0; i < 2; ++i) {
		if (m_lightLeases[i] == -1) {
			m_lightLeases[i] = AcquireMenuPreviewLight(manager, NULL, &m_lights[i]);
		}
	}
	GameAmbientLight* ambient = (GameAmbientLight*) m_ambient;
	for (LegoS32 c = 0; c < 4; ++c) {
		ambient->m_color[c] = g_menuCarPreviewTuning.m_ambientColor[c];
	}
	ambient->ApplyToRenderer();
	GameDirectionalLight* first = (GameDirectionalLight*) m_lights[0];
	GameDirectionalLight* second = (GameDirectionalLight*) m_lights[1];
	for (LegoS32 channel = 0; channel < 4; ++channel) {
		((LegoFloat*) &first->m_properties.m_ambient)[channel] = g_menuCarPreviewTuning.m_firstAmbient[channel];
		((LegoFloat*) &first->m_properties.m_diffuse)[channel] = g_menuCarPreviewTuning.m_firstDiffuse[channel];
		((LegoFloat*) &first->m_properties.m_specular)[channel] = g_menuCarPreviewTuning.m_firstSpecular[channel];
		((LegoFloat*) &second->m_properties.m_ambient)[channel] = g_menuCarPreviewTuning.m_secondAmbient[channel];
		((LegoFloat*) &second->m_properties.m_diffuse)[channel] = g_menuCarPreviewTuning.m_secondDiffuse[channel];
		((LegoFloat*) &second->m_properties.m_specular)[channel] = g_menuCarPreviewTuning.m_secondSpecular[channel];
	}
	first->m_direction = g_menuCarPreviewTuning.m_firstDirection;
	second->m_direction = g_menuCarPreviewTuning.m_secondDirection;
	first->m_properties.SetEnabled(TRUE);
	first->UpdateRendererLight();
	second->m_properties.SetEnabled(TRUE);
	second->UpdateRendererLight();
}

// STUB: LEGORACERS2 0x0042bea0
void MenuCarPreviewSelector::ReleaseLighting()
{
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (m_ambientLease != -1) {
		ReleaseMenuPreviewAmbient(manager, NULL, &m_ambient, m_ambientLease);
		m_ambientLease = -1;
	}
	for (LegoS32 i = 0; i < 2; ++i) {
		if (m_lightLeases[i] != -1) {
			ReleaseMenuPreviewLight(manager, NULL, &m_lights[i], m_lightLeases[i]);
			m_lightLeases[i] = -1;
		}
	}
}

// STUB: LEGORACERS2 0x0042bf00
void MenuCarPreviewSelector::ConfigureViewport(const LegoFloat* p_position, const LegoFloat* p_size)
{
	if (m_camera != NULL) {
		if (0.01f <= p_size[0] && 0.01f <= p_size[1]) {
			// Retail's viewport origin is the supplied position plus half-size.
			// Initialize stores the vertically centered selector position here.
			LegoFloat centerX = p_position[0] + p_size[0] * 0.5f;
			LegoFloat centerY = p_position[1] + p_size[1] * 0.5f;
			LegoFloat width = (LegoFloat) g_displayConfiguration->m_width;
			LegoFloat height = (LegoFloat) g_displayConfiguration->m_height;
			SetGameObjectViewport(
				m_camera,
				NULL,
				RoundPreviewPixel(centerX * width),
				RoundPreviewPixel(centerY * height),
				RoundPreviewPixel(p_size[0] * width),
				RoundPreviewPixel(p_size[1] * height)
			);
			((LegoFloat*) m_camera)[0x69] = 25.0f;
			UpdateGameObjectRenderGeometry(m_camera, NULL);
			Vector3 target = {
				g_menuCarPreviewTuning.m_cameraPosition.m_x + g_menuCarPreviewTuning.m_targetOffset.m_x,
				g_menuCarPreviewTuning.m_cameraPosition.m_y + g_menuCarPreviewTuning.m_targetOffset.m_y,
				g_menuCarPreviewTuning.m_cameraPosition.m_z + g_menuCarPreviewTuning.m_targetOffset.m_z
			};
			Vector3 up = {0.0f, 1.0f, 0.0f};
			SetGameObjectLookAt(m_camera, NULL, &g_menuCarPreviewTuning.m_cameraPosition, &target, &up);
		}
		else {
			((LegoU32*) m_camera)[0x70] = 0;
		}
	}
}
