//
//  UICommon.h
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#ifndef __prsv__UICommon__
#define __prsv__UICommon__

#include "cocos2d.h"
#include "Common.h"

namespace komorki
{
  namespace ui
  {
    float RandomOffset();
    cocos2d::Vec2 RandomVectorOffset();
    cocos2d::Vec2 spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector = cocos2d::Vec2());
  }
}

#endif /* defined(__prsv__UICommon__) */
