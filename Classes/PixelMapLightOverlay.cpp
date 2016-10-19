//
//  PixelMapLightOverlay.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "PixelMapLightOverlay.hpp"
#include "UIConfig.h"

namespace komorki
{
  namespace ui
  {
    USING_NS_CC;
    
    PixelMapLightOverlay::PixelMapLightOverlay(int a, int b, int width, int height, komorki::IPixelDescriptorProvider* provider)
      {
        m_a1 = a;
        m_a2 = a + width;
        m_b1 = b;
        m_b2 = b + height;
        m_width = width;
        m_height = height;
        m_pullSize = 0;
        m_provider = provider;
      }
      
      cocos2d::Sprite* PixelMapLightOverlay::CreateSprite()
      {
        if( ! m_spritesPull.empty() )
        {
          auto s = m_spritesPull.front();
          s->setVisible(true);
          s->setLocalZOrder(0);
          m_spritesPull.pop_front();
          
          return s;
        }
        else
        {
          auto s = cocos2d::Sprite::createWithTexture(getTexture());
          addChild(s);
          return s;
        }
      }
      
      void PixelMapLightOverlay::RemoveSprite(cocos2d::Sprite* sprite)
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
      
      Sprite* PixelMapLightOverlay::SpriteForLight(int value)
      {
        assert(value >= 0);
        assert(value <= 32);
        
        cocos2d::Rect r = cocos2d::Rect(value, 3, 1, 1);
        auto s = CreateSprite();
        s->setTextureRect(r);
        s->setScale(kSpriteScale);
        
        int opacity = 255;
        s->setOpacity(opacity);
        //    s->setLocalZOrder(-1);
        s->setAnchorPoint({0, 0});
        s->setScale(kTileFrameSize, kTileFrameSize);
        
        return s;
      }
      
      bool PixelMapLightOverlay::IsInAABB(const Vec2& vec)
      {
        return IsInAABB(vec.x, vec.y);
      }
      
      bool PixelMapLightOverlay::IsInAABB(const int& x, const int& y)
      {
        if(x < m_a1 || x >= m_a2 ||
           y < m_b1 || y >= m_b2)
        {
          return false;
        }
        
        return true;
      }
      
      cocos2d::Vec2 PixelMapLightOverlay::spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
      {
        auto result = cocos2d::Vec2(((int)vec.x - m_a1) * kSpritePosition, ((int)vec.y - m_b1) * kSpritePosition) + vector;
        return result;
      }
      
      bool PixelMapLightOverlay::init()
      {
        if ( !SpriteBatchNode::initWithFile ("debugFrames.png", 10))
        {
          return false;
        }
        
        getTexture()->setAliasTexParameters();

        for (int i = m_a1; i < m_a2; ++i)
        {
          for (int j = m_b1; j < m_b2; ++j)
          {
            float value = m_provider->GetDescriptor(i, j)->m_physicalDesc.light;
            
            auto s = SpriteForLight(32 * value);
            s->setPosition(spriteVector(komorki::Vec2(i, j)));
          }
        }
        
        return true;
      }
      
      void PixelMapLightOverlay::Reset()
      {
        removeAllChildren();
      }
  }
}

