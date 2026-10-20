#include "frontend/preview/menucharacterpreviewselector.h"
#include "frontend/preview/menucharacterpreviewtuning.h"
#include "frontend/secondary/secondarymanagerleases.h"
#include "game/character/charactervisual.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameanimatedobject.h"

struct PreviewPositionDispatch {
	undefined m_prefix[0x20];
	void(__fastcall* m_setPosition)(GameLifecycleComponent*, void*, const Vector3*);
};
DECOMP_STATIC_ASSERT(offsetof(PreviewPositionDispatch, m_setPosition) == 0x20);

// STUB: LEGORACERS2 0x0042c510
void MenuCharacterPreviewSelector::Initialize(
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
	m_count = m_selection = 0;
	SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
	m_previewLease = AcquireMenuPreviewCharacter(manager, NULL, &m_previewObject);
	m_cameraLease = AcquireMenuPreviewCamera(manager, NULL, &m_camera);
	m_size[0] = p_scale * g_menuCharacterPreviewTuning.m_size[0];
	m_size[1] = p_scale * g_menuCharacterPreviewTuning.m_size[1];
	m_position[0] = p_position[0];
	m_position[1] = p_position[1] + m_size[1] * 0.5f;
	if (p_lighting != 0) {
		InitializeLighting();
	}
	ConfigureViewport(m_position, m_size);
	((CharacterVisual*) m_previewObject)->PrepareForIdlePreview();
	GameAnimatedObject* body = (GameAnimatedObject*) ((CharacterVisual*) m_previewObject)->m_body;
	const PreviewPositionDispatch* transformDispatch = (const PreviewPositionDispatch*) body->m_transform.m_vtable;
	transformDispatch->m_setPosition(&body->m_transform, NULL, &g_menuCharacterPreviewTuning.m_characterPosition);
	if (m_disabled == 0) {
		m_vtable->m_updateBounds(this, NULL);
		LegoFloat center[2] = {(m_bounds[0] + m_bounds[2]) * 0.5f, (m_bounds[1] + m_bounds[3]) * 0.5f};
		CreateChildSprites(center, center[0] - m_bounds[0], -1, 1.0f);
	}
	m_vtable->m_updateBounds(this, NULL);
}
