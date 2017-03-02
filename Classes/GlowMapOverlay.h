//
//  GlowMapOverlay.h
//  prsv
//
//  Created by ttohin on 07.06.15.
//
//

#ifndef __prsv__GlowMapOverlay__
#define __prsv__GlowMapOverlay__

#include "cocos2d.h"
#include "PartialMap.h"

namespace komorki
{
  namespace ui
  {
    class GlowMapOverlay : public cocos2d::SpriteBatchNode
    {
    public:
      bool init();
   
      cocos2d::Vec2 SpriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector = cocos2d::Vec2());
      cocos2d::Rect OffsetForType(komorki::CellDescriptor* pd);
      cocos2d::Sprite* CreateSprite();
      void RemoveSprite(cocos2d::Sprite* sprite);
      void Reset();
      
    private:
      std::list<cocos2d::Sprite*> m_spritesPull;
      unsigned int m_pullSize;
      float m_updateTime;
    };
  }
}

#endif /* defined(__prsv__GlowMapOverlay__) */
