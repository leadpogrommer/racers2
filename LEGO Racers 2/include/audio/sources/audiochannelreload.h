#ifndef LR2_AUDIOCHANNELRELOAD_H
#define LR2_AUDIOCHANNELRELOAD_H

#include "audio/service/audioservice.h"
#include "audio/streaming/audiocommentary.h"
#include "core/strings/charbuffer128.h"

class DigitalMusicController;
LegoBool32 __fastcall LoadAudioChannelFile(AudioChannelRecord*, void*, CharBuffer128 p_path);
void __fastcall RestoreCommentaryPlaybackBuffer(CommentaryAudioStream*, void*, IDirectSound8*, LegoU32, LegoU32);
void __fastcall RestoreDigitalMusicOutput(DigitalMusicController*, void*, IDirectSound8*, LegoU32, LegoU32);
void __fastcall SetDigitalMusicBufferGain(DigitalMusicController*, void*, LegoFloat);
LegoFloat __cdecl MapGameAudioGain(LegoFloat);
void __fastcall SetDigitalMusicGain(AudioService*, void*, LegoFloat, LegoU32);
void __fastcall GetDigitalMusicGain(AudioService*, void*, LegoFloat*, LegoFloat*);
#endif
