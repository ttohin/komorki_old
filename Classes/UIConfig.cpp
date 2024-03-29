//
//  UIConfig.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "UIConfig.h"

namespace komorki
{
  namespace graphic
  {
    const float kTileFrameSize = 32.f;
    const int kSpritePosition = 32;
    const double kSpriteScale = kSpritePosition/kTileFrameSize;
    const float kViewportMargin = 00.f;
    const int kSegmentSize = 50;
    const bool kAnimated = true;
    const bool kRedrawEachUpdate = false;
    const bool kSimpleDraw = false;
    const bool kRandomColorPerPartialMap = true;
    const float kLightMapScale = 4.f;
    const unsigned int kNumberOfUpdatesOnStartup = 50;
    const komorki::Vec2 kCellsTextureSizeInPixels = komorki::Vec2(32, 32);
    const unsigned int kCellShapeSegments = 4;
    const unsigned int kSpriteBatchNodePullSize = 32;
  }
}
