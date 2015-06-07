//
//  UICommon.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "UICommon.h"
#include "UIConfig.h"
#include "b2Utilites.h"

namespace komorki
{
  namespace ui
  {
    float RandomOffset()
    {
      return cRandEps(0.0f, kTileFrameSize*0.1f);
    }
    
    cocos2d::Vec2 RandomVectorOffset()
    {
      return cocos2d::Vec2(RandomOffset(), RandomOffset());
    }
  }
}