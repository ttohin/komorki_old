//
//  GlowMapOverlay.cpp
//  prsv
//
//  Created by ttohin on 07.06.15.
//
//

#include "GlowMapOverlay.h"
#include "UIConfig.h"
#include "UICommon.h"

#define PMP_PULL_SIZE 32

namespace komorki
{
  namespace ui
  {
    USING_NS_CC;

    bool GlowMapOverlay::init()
    {
      m_pullSize = 32;
      
      if ( !SpriteBatchNode::initWithFile ("glow.png", 10))
      {
        return false;
      }
      
      return true;
    }
  
   
    cocos2d::Vec2 GlowMapOverlay::SpriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
    {
      auto result = cocos2d::Vec2(vec.x * kSpritePosition + kSpritePosition/2.f, vec.y * kSpritePosition+ kSpritePosition/2.f) + vector;
      return result;
    }
    
    cocos2d::Rect GlowMapOverlay::OffsetForType(komorki::CellDescriptor* d)
    {
      int index = 0;
      return cocos2d::Rect(index*160, 0, 160, 160);
    }
    
    Sprite* GlowMapOverlay::CreateSprite()
    {
      if( ! m_spritesPull.empty() )
      {
        auto s = m_spritesPull.front();
        s->setVisible(true);
        m_spritesPull.pop_front();
        
        return s;
      }
      else
      {
        auto s = Sprite::createWithTexture(getTexture());
        addChild(s);
//        s->setOpacity(100);
//        s->setBlendFunc(BlendFunc::ADDITIVE);
        return s;
      }
    }
    
    void GlowMapOverlay::RemoveSprite(Sprite* sprite)
    {
      if (m_spritesPull.size() < m_pullSize)
      {
        sprite->stopAllActions();
        sprite->setVisible(false);
        m_spritesPull.push_back(sprite);
      }
      else
      {
        sprite->removeFromParentAndCleanup(true);
      }
    }
    
    void GlowMapOverlay::Reset()
    {
      removeAllChildren();
      return;
//      m_pullSize += _children.size() + 1;
      
      for(auto it=_children.cbegin(); it != _children.cend(); ++it)
      {
        (*it)->stopAllActions();
        RemoveSprite(static_cast<Sprite*>(*it));
      }
      
//      m_pullSize = PMP_PULL_SIZE;
    }
    
  }
}
