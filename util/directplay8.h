#ifndef LR2_DIRECTPLAY8_H
#define LR2_DIRECTPLAY8_H

#include "directx8.h"

#ifndef DIRECTPLAY_VERSION
#define DIRECTPLAY_VERSION 0x0800
#endif

// The retail CoCreateInstance GUIDs identify DirectPlay8Peer and its lobbied-application
// object. Keep the SDK method lists and calling conventions intact.
#include <dplay8.h>
#include <dplobby8.h>

#endif // LR2_DIRECTPLAY8_H
