#include "video/moviedirectshow.h"

#include "core/application/commandline.h"
#include "video/moviefilter.h"

#include <control.h>
#include <evcode.h>
#include <objbase.h>
#include <string.h>
#include <strmif.h>
#include <uuids.h>
#include <windows.h>

static const CLSID kFilterGraphClass = {0xe436ebb3, 0x524f, 0x11ce, {0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};
static const IID kGraphBuilderInterface =
	{0x56a868a9, 0x0ad4, 0x11ce, {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};
static const CLSID kMpegSplitterClass = {0x336475d0, 0x942a, 0x11ce, {0xa8, 0x70, 0x00, 0xaa, 0x00, 0x2f, 0xea, 0xb5}};
static const CLSID kDirectSoundRendererClass =
	{0x79376820, 0x07d0, 0x11cf, {0xa2, 0x4d, 0x00, 0x20, 0xaf, 0xd7, 0x97, 0x67}};
static const IID kBaseFilterInterface = {0x56a86895, 0x0ad4, 0x11ce, {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};
static const IID kMediaControlInterface =
	{0x56a868b1, 0x0ad4, 0x11ce, {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};
static const IID kMediaEventInterface = {0x56a868b6, 0x0ad4, 0x11ce, {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};
// STUB: LEGORACERS2 0x00499500
LegoU32 __cdecl PlayMpegMovie(const LegoChar* p_path, LegoBool32 p_allowSkip)
{
	WCHAR widePath[MAX_PATH];
	IGraphBuilder* graph = NULL;
	IMediaControl* control = NULL;
	IMediaEvent* events = NULL;
	IBaseFilter* source = NULL;
	IBaseFilter* splitter = NULL;
	IBaseFilter* audio = NULL;
	MovieTextureRenderer* video = NULL;
	HRESULT result;
	LegoBool32 sourceAdded = FALSE;
	LegoBool32 splitterAdded = FALSE;
	LegoBool32 audioAdded = FALSE;
	LegoBool32 videoAdded = FALSE;
	IPin* sourceOutput = NULL;
	IPin* splitterInput = NULL;
	IPin* splitterAudio = NULL;
	IPin* splitterVideo = NULL;
	IPin* audioInput = NULL;
	IPin* videoInput = NULL;
	IEnumPins* audioPins = NULL;
	IPin* audioPin = NULL;
	PIN_INFO audioPinInfo;
	ULONG pinsFetched = 0;

	memset(widePath, 0, sizeof(widePath));
	MultiByteToWideChar(CP_ACP, 0, p_path, -1, widePath, MAX_PATH);

	if (FUN_004b4a60("fol") != FALSE) {
		p_allowSkip = TRUE;
	}
	if (FAILED(
			CoCreateInstance(kFilterGraphClass, NULL, CLSCTX_INPROC_SERVER, kGraphBuilderInterface, (void**) &graph)
		)) {
		goto cleanup;
	}

	graph->QueryInterface(kMediaControlInterface, (void**) &control);
	graph->QueryInterface(kMediaEventInterface, (void**) &events);
	if (FAILED(graph->AddSourceFilter(widePath, L"SOURCE", &source))) {
		goto cleanup;
	}
	sourceAdded = TRUE;

	if (FAILED(
			CoCreateInstance(kMpegSplitterClass, NULL, CLSCTX_INPROC_SERVER, kBaseFilterInterface, (void**) &splitter)
		)) {
		goto cleanup;
	}
	if (FAILED(CoCreateInstance(
			kDirectSoundRendererClass,
			NULL,
			CLSCTX_INPROC_SERVER,
			kBaseFilterInterface,
			(void**) &audio
		))) {
		goto cleanup;
	}
	if (FAILED(graph->AddFilter(splitter, L"MPEGSPLITTER"))) {
		goto cleanup;
	}
	splitterAdded = TRUE;
	if (FAILED(graph->AddFilter(audio, L"ATDAUDIORENDERER"))) {
		goto cleanup;
	}
	audioAdded = TRUE;

	video = new MovieTextureRenderer(NULL, &result);
	if (video != NULL && SUCCEEDED(result)) {
		if (FAILED(graph->AddFilter(video, L"ATDTEXTURERENDERER"))) {
			delete video;
			video = NULL;
			goto cleanup;
		}
		videoAdded = TRUE;
	}

	if (control != NULL && events != NULL && video != NULL && SUCCEEDED(result)) {
		if (FAILED(splitter->FindPin(L"Input", &splitterInput)) || FAILED(video->FindPin(L"In", &videoInput)) ||
			FAILED(audio->EnumPins(&audioPins))) {
			goto cleanup;
		}

		/* Retail asks the audio renderer for its input pin name at run time. */
		for (;;) {
			pinsFetched = 0;
			if (audioPins->Next(1, &audioPin, &pinsFetched) != S_OK) {
				goto cleanup;
			}
			memset(&audioPinInfo, 0, sizeof(audioPinInfo));
			if (FAILED(audioPin->QueryPinInfo(&audioPinInfo))) {
				goto cleanup;
			}
			if (audioPinInfo.dir == PINDIR_INPUT) {
				result = audio->FindPin(audioPinInfo.achName, &audioInput);
				if (audioPinInfo.pFilter != NULL) {
					audioPinInfo.pFilter->Release();
				}
				audioPinInfo.pFilter = NULL;
				audioPin->Release();
				audioPin = NULL;
				if (FAILED(result)) {
					goto cleanup;
				}
				break;
			}
			if (audioPinInfo.pFilter != NULL) {
				audioPinInfo.pFilter->Release();
			}
			audioPinInfo.pFilter = NULL;
			audioPin->Release();
			audioPin = NULL;
		}

		if (FAILED(source->FindPin(L"Output", &sourceOutput)) || FAILED(splitter->FindPin(L"Audio", &splitterAudio)) ||
			FAILED(splitter->FindPin(L"Video", &splitterVideo))) {
			goto cleanup;
		}

		if (FAILED(graph->Connect(sourceOutput, splitterInput)) || FAILED(graph->Connect(splitterAudio, audioInput)) ||
			FAILED(graph->Connect(splitterVideo, videoInput))) {
			goto cleanup;
		}

		ClearMovieTextures(video, NULL);
		control->Run();
		GetAsyncKeyState(VK_ESCAPE);
		while (IsMoviePlaybackIncomplete(events)) {
			Sleep(100);
			if (p_allowSkip != FALSE && (GetAsyncKeyState(VK_ESCAPE) & 1) != 0) {
				break;
			}
		}
		control->Stop();
	}

cleanup:
	if (audioPin != NULL) {
		audioPin->Release();
	}
	if (audioPins != NULL) {
		audioPins->Release();
	}
	if (videoInput != NULL) {
		videoInput->Release();
	}
	if (splitterVideo != NULL) {
		splitterVideo->Release();
	}
	if (audioInput != NULL) {
		audioInput->Release();
	}
	if (splitterAudio != NULL) {
		splitterAudio->Release();
	}
	if (splitterInput != NULL) {
		splitterInput->Release();
	}
	if (sourceOutput != NULL) {
		sourceOutput->Release();
	}
	if (sourceAdded != FALSE) {
		graph->RemoveFilter(source);
		sourceAdded = FALSE;
	}
	if (source != NULL) {
		source->Release();
		source = NULL;
	}
	if (videoAdded != FALSE) {
		graph->RemoveFilter(video);
		videoAdded = FALSE;
		video = NULL;
	}
	else if (video != NULL) {
		delete video;
		video = NULL;
	}
	if (audioAdded != FALSE) {
		graph->RemoveFilter(audio);
	}
	if (splitterAdded != FALSE) {
		graph->RemoveFilter(splitter);
	}
	if (audio != NULL) {
		audio->Release();
	}
	if (splitter != NULL) {
		splitter->Release();
	}
	if (events != NULL) {
		events->Release();
	}
	if (control != NULL) {
		control->Release();
	}
	if (graph != NULL) {
		graph->Release();
	}
	return 1;
}

// STUB: LEGORACERS2 0x0049a1e0
LegoBool32 __stdcall IsMoviePlaybackIncomplete(IMediaEvent* p_eventSource)
{
	long eventCode = 0;
	long param1;
	long param2;
	p_eventSource->GetEvent(&eventCode, &param1, &param2, 0);
	return eventCode != EC_COMPLETE;
}
