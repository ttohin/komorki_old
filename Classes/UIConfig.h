//
//  UIConfig.h
//  prsv
//
//  Created by Anton Simakov on 06.06.15.
//
//

#ifndef prsv_UIConfig_h
#define prsv_UIConfig_h

#include "cocos2d.h"
#include "Common.h"

namespace komorki
{
  namespace graphic
  {
    extern const float kTileFrameSize;
    extern const int kSpritePosition;
    extern const double kSpriteScale;
    extern const float kViewportMargin;
    extern const int kSegmentSize;
    extern const bool kAnimated;
    extern const bool kRedrawEachUpdate;
    extern const bool kSimpleDraw;
    extern const bool kRandomColorPerPartialMap;
    extern const float kLightMapScale;
    extern const unsigned int kNumberOfUpdatesOnStartup;
    extern const komorki::Vec2 kCellsTextureSizeInPixels;
    extern const unsigned int kCellShapeSegments;
    extern const unsigned int kSpriteBatchNodePullSize;
    
  }
}

#endif
