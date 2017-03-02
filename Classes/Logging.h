//
//  Logging.h
//  Komorki
//
//  Created 02.03.17.
//

#pragma once

#include "cocos2d.h"

//#define KOMORKI_DEBUG_UI
#ifdef KOMORKI_DEBUG_UI
#define KOMORKI_LOG(...) cocos2d::log(__VA_ARGS__)
#else
#define KOMORKI_LOG(...) {}
#endif

//#define LOG_UPDATES

//#define DEBUG_PARTIAL_MAP
//#define DEBUG_VIEWPORT

#ifdef DEBUG_PARTIAL_MAP
#define LOG_W(...) KOMORKI_LOG(__VA_ARGS__)
#else
#define LOG_W(...) {}
#endif
