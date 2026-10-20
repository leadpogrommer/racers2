#include "frontend/widgets/menuwidget.h"

#include "frontend/resources/controllereventrecords.h"
#include "game/controller/gamecontroller.h"

struct SecondaryManagerMenuState {
	undefined m_prefix[0x7c];
	MenuNavigationState* m_navigation;
};
DECOMP_STATIC_ASSERT(offsetof(SecondaryManagerMenuState, m_navigation) == 0x7c);

// These are zero-initialized retail globals; 0x00423ea0 assigns their values.
// GLOBAL: LEGORACERS2 0x005619d0
LegoFloat g_menuPointerOffsetX;
// GLOBAL: LEGORACERS2 0x005619d4
LegoFloat g_menuPointerOffsetY;

static void __fastcall MenuWidgetUpdateBoundsBridge(MenuWidgetBase* p_widget, void*);
static void __fastcall MenuWidgetSetFontBridge(MenuWidgetBase* p_widget, void*, BitmapFont* p_font);
static void __fastcall MenuWidgetBareReturnBridge(MenuWidgetBase*, void*);
static void __fastcall MenuWidgetResetBridge(MenuWidgetBase* p_widget, void*);
static LegoU32 __fastcall MenuWidgetZeroTwoArgumentsBridge(MenuWidgetBase*, void*, LegoU32*, const LegoU32*);
static void __fastcall MenuWidgetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
);

const MenuWidgetDispatch g_menuWidgetBaseDispatch = {
	&MenuWidgetUpdateBoundsBridge,
	&MenuWidgetSetFontBridge,
	&MenuWidgetBareReturnBridge,
	&MenuWidgetResetBridge,
	&MenuWidgetZeroTwoArgumentsBridge,
	&MenuWidgetClipBridge
};

// FUNCTION: LEGORACERS2 0x00416b80 FOLDED
void __fastcall SetMenuWidgetFont(MenuWidgetBase* p_widget, void*, BitmapFont* p_font)
{
	p_widget->m_font = p_font;
}

// STUB: LEGORACERS2 0x004196f0
MenuWidgetBase::MenuWidgetBase(LegoS32 p_initialValue)
{
	m_state = 0;
	m_initialValue = p_initialValue;
	m_unk0x14 = 0;
	m_unk0x1c = 0;
	m_unk0x20 = 0;
	m_unk0x24 = 0;
	m_vtable = &g_menuWidgetBaseDispatch;
	m_childSprites[0] = NULL;
	m_childSprites[1] = NULL;
}

// FUNCTION: LEGORACERS2 0x00419720 FOLDED
void MenuWidgetBase::SetFont(BitmapFont* p_font)
{
	m_font = p_font;
}

// STUB: LEGORACERS2 0x00419730
void MenuWidgetBase::RestoreBaseDispatch()
{
	m_vtable = &g_menuWidgetBaseDispatch;
}

// FUNCTION: LEGORACERS2 0x00419740
void MenuWidgetBase::BeginState()
{
	m_state = 1;
	m_unk0x1c = 0;
	m_unk0x20 = 0;
	m_unk0x24 = 0;
}

// FUNCTION: LEGORACERS2 0x00419760
void MenuWidgetBase::ResetState()
{
	m_initialValue = -1;
	m_state = 0;
	m_unk0x1c = 0;
	m_unk0x20 = 0;
	m_unk0x24 = 0;
}

// FUNCTION: LEGORACERS2 0x00419910
void MenuWidgetBase::ReleaseChildSprites()
{
	for (LegoS32 index = 1; index >= 0; --index) {
		if (m_childSprites[index] != NULL) {
			m_childSprites[index]->Release();
			m_childSprites[index] = NULL;
		}
	}
}

// STUB: LEGORACERS2 0x00419940
LegoS32 MenuWidgetBase::GetNavigationDirection(const LegoU32* p_input, LegoU32 p_axis)
{
	if (m_childSprites[0] == NULL || m_childSprites[1] == NULL) {
		return 0;
	}

	LegoU32 pressed = p_input[p_axis == 0 ? 2 : 3];
	if ((pressed & 3) != 0) {
		return (pressed & 1) != 0 ? -1 : 1;
	}

	// The input state is embedded in the secondary manager, not in the widget.
	SecondaryManagerMenuState* manager = (SecondaryManagerMenuState*) g_gameController.m_secondaryManager;
	MenuNavigationState* navigation = manager->m_navigation;
	if (navigation == NULL || navigation->m_active == 0) {
		return 0;
	}

	LegoS32 direction = 0;
	if (navigation->IsSpriteUnderPointer(m_childSprites[0])) {
		direction = -1;
	}
	if (navigation->IsSpriteUnderPointer(m_childSprites[1])) {
		direction = 1;
	}
	if (direction != 0) {
		navigation->ReadDirectionInput(1);
		LegoU32 flags = p_axis == 0 ? navigation->m_horizontalFlags : navigation->m_verticalFlags;
		if ((flags & 1) == 0) {
			direction = 0;
		}
	}
	return direction;
}

// FUNCTION: LEGORACERS2 0x00419a00
void MenuWidgetBase::SetChildClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	for (LegoS32 index = 0; index < 2; ++index) {
		if (m_childSprites[index] != NULL) {
			m_childSprites[index]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x00419a30
void MenuWidgetBase::UpdateBounds()
{
	m_bounds[0] = 1.0f;
	m_bounds[1] = 1.0f;
	m_bounds[2] = 0.0f;
	m_bounds[3] = 0.0f;

	for (LegoS32 index = 0; index < 2; ++index) {
		GlobalOverlaySprite* sprite = m_childSprites[index];
		if (sprite != NULL) {
			LegoFloat size[2];
			sprite->GetSize(size);
			if (sprite->m_left < m_bounds[0]) {
				m_bounds[0] = sprite->m_left;
			}
			if (sprite->m_top < m_bounds[1]) {
				m_bounds[1] = sprite->m_top;
			}
			if (m_bounds[2] < sprite->m_left + size[0]) {
				m_bounds[2] = sprite->m_left + size[0];
			}
			if (m_bounds[3] < sprite->m_top + size[1]) {
				m_bounds[3] = sprite->m_top + size[1];
			}
		}
	}
}

// FUNCTION: LEGORACERS2 0x00423ea0
void InitializeMenuPointerOffsets()
{
	g_menuPointerOffsetX = 0.005859375f;
	g_menuPointerOffsetY = 0.005208333333333333f;
}

// STUB: LEGORACERS2 0x00424130
void MenuNavigationState::GetPointerPosition(LegoFloat* p_position) const
{
	p_position[0] = m_pointer->m_x + g_menuPointerOffsetX;
	p_position[1] = m_pointer->m_y + g_menuPointerOffsetY;
}

// STUB: LEGORACERS2 0x00424160
void MenuNavigationState::ReadDirectionInput(LegoU32 p_direction)
{
	if (m_active != 0) {
		const ControllerEventRecordTable* table = (const ControllerEventRecordTable*) g_gameController.m_unk0x90;
		const LegoU32* source = (const LegoU32*) table + 1 + (0x10d - (p_direction != 0)) * 5;
		for (LegoU32 index = 0; index < 5; ++index) {
			m_pointer->m_directionInput[index] = source[index];
		}
	}
}

// STUB: LEGORACERS2 0x004241a0
LegoBool32 MenuNavigationState::IsRecordUnderPointer(const GlobalOverlayRecord* p_record) const
{
	LegoFloat recordTopLeft[2];
	LegoFloat recordBottomRight[2];
	LegoFloat pointerPosition[2];
	p_record->GetRectangle(recordTopLeft, recordBottomRight);
	GetPointerPosition(pointerPosition);
	return recordTopLeft[0] <= pointerPosition[0] && pointerPosition[0] <= recordBottomRight[0] &&
		   recordTopLeft[1] <= pointerPosition[1] && pointerPosition[1] <= recordBottomRight[1];
}

// STUB: LEGORACERS2 0x00424220
LegoBool32 MenuNavigationState::IsSpriteUnderPointer(const GlobalOverlaySprite* p_sprite) const
{
	LegoFloat spriteTopLeft[2];
	LegoFloat spriteBottomRight[2];
	LegoFloat pointerPosition[2];
	p_sprite->GetRectangle(spriteTopLeft, spriteBottomRight);
	GetPointerPosition(pointerPosition);
	return spriteTopLeft[0] <= pointerPosition[0] && pointerPosition[0] <= spriteBottomRight[0] &&
		   spriteTopLeft[1] <= pointerPosition[1] && pointerPosition[1] <= spriteBottomRight[1];
}

static void __fastcall MenuWidgetUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	p_widget->UpdateBounds();
}

static void __fastcall MenuWidgetSetFontBridge(MenuWidgetBase* p_widget, void*, BitmapFont* p_font)
{
	p_widget->SetFont(p_font);
}

static void __fastcall MenuWidgetBareReturnBridge(MenuWidgetBase*, void*)
{
}

static void __fastcall MenuWidgetResetBridge(MenuWidgetBase* p_widget, void*)
{
	p_widget->ResetState();
}

static LegoU32 __fastcall MenuWidgetZeroTwoArgumentsBridge(MenuWidgetBase*, void*, LegoU32*, const LegoU32*)
{
	return 0;
}

static void __fastcall MenuWidgetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	p_widget->SetChildClipRectangle(p_topLeft, p_bottomRight);
}
