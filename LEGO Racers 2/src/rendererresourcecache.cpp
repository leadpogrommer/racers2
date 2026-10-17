#include "memoryheap.h"
#include "renderer.h"
#include "rendererrecords.h"
#include "simulationtiming.h"
#include "winfile.h"

#include <new.h>
#include <stdlib.h>
#include <string.h>

extern "C" char* __cdecl _strupr(char* p_text);
extern LegoU32 ApplicationTruePolicy();

typedef LegoBool32(__fastcall* CacheSetPath)(RendererResourceCacheEntry*, void*, const LegoChar*);
typedef LegoBool32(__fastcall* CacheLoad)(RendererResourceCacheEntry*, void*);
typedef void(__fastcall* CacheAdvance)(RendererResourceCacheEntry*, void*);
typedef void(__fastcall* CacheRelease)(RendererResourceCacheEntry*, void*);
typedef LegoU32(__fastcall* CacheTrue)(RendererResourceCacheEntry*, void*);

struct RendererResourceCacheDispatch {
	CacheSetPath m_setPath;
	CacheLoad m_load;
	CacheAdvance m_advance;
	CacheRelease m_release;
	CacheTrue m_true;
};

typedef int(__cdecl* PureCall)();

struct RendererResourceCacheAbstractDispatch {
	PureCall m_callbacks[5];
};
DECOMP_SIZE_ASSERT(RendererResourceCacheDispatch, 0x14);
DECOMP_SIZE_ASSERT(RendererResourceCacheAbstractDispatch, 0x14);

LegoBool32 __fastcall InvokeResourceCacheSetPath(RendererResourceCacheEntry* p_entry, void*, const LegoChar* p_path)
{
	return p_entry->SetPath(p_path);
}

LegoBool32 __fastcall InvokeResourceCacheLoad(RendererResourceCacheEntry* p_entry, void*)
{
	return p_entry->LoadAnimation();
}

void __fastcall InvokeResourceCacheAdvance(RendererResourceCacheEntry* p_entry, void*)
{
	p_entry->AdvanceFrame();
}

void __fastcall InvokeResourceCacheRelease(RendererResourceCacheEntry* p_entry, void*)
{
	p_entry->ReleaseAnimationReferences();
}

extern const RendererResourceCacheDispatch g_rendererResourceCacheDispatch = {
	InvokeResourceCacheSetPath,
	InvokeResourceCacheLoad,
	InvokeResourceCacheAdvance,
	InvokeResourceCacheRelease,
	(CacheTrue) ApplicationTruePolicy,
};

extern "C" int __cdecl _purecall();

extern const RendererResourceCacheAbstractDispatch g_rendererResourceCacheAbstractDispatch = {
	{_purecall, _purecall, _purecall, _purecall, _purecall},
};

namespace
{
LegoS32 RoundFloatWithCurrentFpuMode(LegoFloat p_value)
{
	LegoS32 result;
	// Retail uses FISTP with the process's current x87 rounding mode.
	// clang-format off
	__asm {
		fld p_value
		fistp result
	}
	// clang-format on
	return result;
}
} // namespace

// STUB: LEGORACERS2 0x004be2f0
RendererResourceCacheEntry::RendererResourceCacheEntry(LegoU8 p_type)
{
	m_vtable = (void*) &g_rendererResourceCacheAbstractDispatch;
	m_flags0x06 = (LegoU16) ((m_flags0x06 & 0xc000) | ((p_type & 0x0f) << 2));
}

void RendererResourceCacheEntry::InitializeRecord()
{
	new (this) RendererResourceCacheEntry(1);
	m_frameCount = 0;
	m_elapsedTicks = 0;
	m_totalDuration = 0;
	m_vtable = (void*) &g_rendererResourceCacheDispatch;
}

// FUNCTION: LEGORACERS2 0x004c0070
LegoBool32 RendererResourceCacheEntry::SetPath(const LegoChar* p_path)
{
	strcpy(m_path, p_path);
	m_resourceId = (LegoS16) g_displayRenderer->CreateTextureResource("", 0, 0, 0, 0);
	return TRUE;
}

// STUB: LEGORACERS2 0x004c00c0
LegoBool32 RendererResourceCacheEntry::LoadAnimation()
{
	WinFile file(m_path, 0, NULL);
	MemoryHeap* heap = GetMemoryHeap();
	DWORD fileSize = file.GetManagedSize();
	LegoU8* buffer = (LegoU8*) heap->AllocateFromEnd(fileSize, "Temporary IFL file");
	LegoU8* end = buffer + fileSize;

	m_flags0x06 &= 0xfffd;
	m_elapsedTicks = 0;
	m_frameCount = 0;
	file.Read(buffer, fileSize, 1);
	file.CloseManaged();

	LegoChar path[152];
	strcpy(path, m_path);
	LegoChar* directory = path;
	for (LegoS32 pathIndex = (LegoS32) strlen(path) - 2; pathIndex > 0; --pathIndex) {
		if (path[pathIndex] == '\\') {
			directory = path + pathIndex + 1;
			break;
		}
	}

	LegoU8* line = buffer;
	while (line < end) {
		LegoU8* lineEnd = line;
		while (lineEnd < end && (LegoS8) *lineEnd >= 0x20) {
			++lineEnd;
		}

		if (lineEnd > line && *line != ';') {
			*lineEnd = '\0';
			_strupr((LegoChar*) line);
			LegoChar* extension = strstr((LegoChar*) line, ".TGA");
			if (extension != NULL) {
				extension[4] = '\0';
				strcpy(directory, (LegoChar*) line);
				LegoS32 frameIndex = (LegoS32) m_frameCount;
				m_frameResourceIds[frameIndex] = (LegoS16) g_displayRenderer->CreateTextureResource(path, 0, -1, 1, 1);

				LegoChar durationText[12];
				LegoChar* durationOutput = durationText;
				LegoU8* digit = (LegoU8*) extension + 5;
				while (digit < lineEnd) {
					if (*digit >= '0' && *digit <= '9') {
						*durationOutput++ = (LegoChar) *digit;
					}
					if (durationOutput >= durationText + 9) {
						break;
					}
					++digit;
				}
				*durationOutput = '\0';

				LegoS16 duration = durationOutput == durationText ? 1 : (LegoS16) atoi(durationText);
				m_frameEndTimes[frameIndex] = duration;
				++m_frameCount;
			}
		}

		line = lineEnd;
		while (line < end && (LegoS8) *line < 0x20) {
			++line;
		}
	}

	heap->Free(buffer);
	if ((LegoS32) m_frameCount < 1) {
		return FALSE;
	}

	LegoU32 frameRate = g_simulationFrameRate;
	const LegoFloat frameRateScale = 0.03333333507180214f;
	LegoFloat stepMilliseconds = g_simulationStepMilliseconds;
	LegoFloat durationScale;
	// Retail keeps the integer-to-float conversion and both multiplies on the
	// x87 stack, then rounds once when it stores this scale as a float.
	// clang-format off
	__asm {
		fild frameRate
		fmul frameRateScale
		fmul stepMilliseconds
		fstp durationScale
	}
	// clang-format on

	RendererDispatchView* rendererDispatch = (RendererDispatchView*) g_displayRenderer->m_vtable;
	// The texture-selection callback occupies slot +0x11c and takes the base
	// resource ID followed by the first frame resource ID.
	typedef void(__fastcall * SelectCacheTexture)(DisplayRenderer*, void*, LegoS32, LegoS32);
	SelectCacheTexture selectTexture = *(SelectCacheTexture*) ((LegoU8*) rendererDispatch + 0x11c);
	selectTexture(g_displayRenderer, NULL, m_resourceId, m_frameResourceIds[0]);

	LegoS16* durations = m_frameEndTimes;
	for (LegoU32 durationIndex = 1; (LegoS32) durationIndex < (LegoS32) m_frameCount; ++durationIndex) {
		durations[durationIndex] = (LegoS16) (durations[durationIndex] + durations[durationIndex - 1]);
	}
	for (LegoU32 scaleIndex = 0; (LegoS32) scaleIndex < (LegoS32) m_frameCount; ++scaleIndex) {
		volatile LegoFloat scaled = (LegoFloat) (LegoS32) durations[scaleIndex] * durationScale;
		durations[scaleIndex] = (LegoS16) RoundFloatWithCurrentFpuMode(scaled);
	}
	m_totalDuration = m_frameEndTimes[m_frameCount - 1];
	return TRUE;
}

// STUB: LEGORACERS2 0x004c03c0
void RendererResourceCacheEntry::AdvanceFrame()
{
	if ((LegoS32) m_frameCount <= 0) {
		return;
	}

	LegoU32 elapsedBeforeAdvance = m_elapsedTicks;
	LegoU32 framePosition = elapsedBeforeAdvance % m_totalDuration;
	m_elapsedTicks += (LegoU32) RoundFloatWithCurrentFpuMode(g_simulationStepMilliseconds);

	LegoS16 position = (LegoS16) framePosition;
	LegoS16* durations = m_frameEndTimes;
	LegoU32 frameIndex = 0;
	while ((LegoS32) frameIndex < (LegoS32) m_frameCount && durations[frameIndex] <= position) {
		++frameIndex;
	}
	if ((LegoS32) frameIndex == (LegoS32) m_frameCount) {
		return;
	}

	typedef void(__fastcall * SelectCacheTexture)(DisplayRenderer*, void*, LegoS32, LegoS32);
	SelectCacheTexture selectTexture = *(SelectCacheTexture*) ((LegoU8*) g_displayRenderer->m_vtable + 0x11c);
	selectTexture(g_displayRenderer, NULL, m_resourceId, m_frameResourceIds[frameIndex]);
}

// FUNCTION: LEGORACERS2 0x004c0450
void RendererResourceCacheEntry::ReleaseAnimationReferences()
{
	g_displayRenderer->ReleaseTextureResourceReference(m_resourceId, 0);
	for (LegoU32 i = 0; (LegoS32) i < (LegoS32) m_frameCount; ++i) {
		g_displayRenderer->ReleaseTextureResourceReference(m_frameResourceIds[i], 1);
	}
}
