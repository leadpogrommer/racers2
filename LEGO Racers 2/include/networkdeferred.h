#ifndef LR2_NETWORKDEFERRED_H
#define LR2_NETWORKDEFERRED_H
#include "networkapplayer.h"

// Unfinished operations reached only after selecting network play, or after
// disconnecting a racer. These declarations are ABI contracts, not recovery
// completion claims; the separate implementation unit fails immediately.
void __fastcall ProcessNetworkMessages(NetworkAppLayer*, void*, LegoFloat);
void __fastcall InitializeNetworkLocalPlayer(NetworkAppLayer*, void*);
LegoBool32 __fastcall PrepareNetworkGame(NetworkAppLayer*, void*, LegoFloat);
void __fastcall ConfigureNetworkGame(NetworkAppLayer*, void*);
void __fastcall FinishNetworkGame(NetworkAppLayer*, void*);
void __fastcall BeginNetworkGame(NetworkAppLayer*, void*);
void __fastcall SendNetworkPlayerCar(NetworkAppLayer*, void*, LegoS32);
void __fastcall SendNetworkPlayerCharacter(NetworkAppLayer*, void*, LegoS32);
void __fastcall SendNetworkPlayerState(NetworkAppLayer*, void*, LegoS32);
void __fastcall SetNetworkRacerState(void*, void*, LegoU32);
#endif
