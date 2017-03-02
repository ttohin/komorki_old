//
//  UICommon.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "UICommon.h"
#include "UIConfig.h"
#include "Random.h"

namespace komorki
{
  namespace graphic
  {
    float RandomOffset()
    {
      return cRandEps(0.0f, kTileFrameSize*0.1f);
    }
    
    cocos2d::Vec2 RandomVectorOffset()
    {
      return cocos2d::Vec2(RandomOffset(), RandomOffset());
    }
    
    cocos2d::Vec2 spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
    {
      return cocos2d::Vec2(vec.x * kSpritePosition, vec.y * kSpritePosition) + vector;
    }
  }
}
