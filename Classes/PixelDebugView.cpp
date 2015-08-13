//
//  PixelDebugView.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "PixelDebugView.h"
#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "PixelMapPartial.h"
#include "CellDescriptor.h"
#include "UIConfig.h"

#define ANIMATE_DEAD_CELLS 1
#define PMP_MAX_NUMBER_OF_CHILDREN 2000


namespace komorki
{
namespace ui
{
  USING_NS_CC;

  PixelDebugView::PixelDebugView(int a, int b, int width, int height, komorki::PixelDescriptorProvider* provider)
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
  
  Sprite* PixelDebugView::CreateSprite()
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
      auto s = Sprite::createWithTexture(getTexture());
      addChild(s);
      return s;
    }
  }
  
  void PixelDebugView::RemoveSprite(Sprite* sprite)
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
  
  Sprite* PixelDebugView::ColorSprite(int i)
  {
    cocos2d::Rect r = cocos2d::Rect(8 + i, 0, 1, 1);
    auto s = CreateSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    s->setOpacity(180);
    s->setLocalZOrder(-1);
    s->setAnchorPoint({0, 0});
    s->setScale(kTileFrameSize, kTileFrameSize);
    
    return s;
  }
  
  Sprite* PixelDebugView::HealthSprite(komorki::PixelDescriptor* pixelD)
  {
    cocos2d::Rect r = cocos2d::Rect(1, 1, 1, 1);
    auto s = CreateSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    s->setOpacity(180);
    s->setLocalZOrder(-1);
    s->setAnchorPoint({0, 0});
    s->setScale(kTileFrameSize, 5);
    
    float healthRatio = pixelD->m_cellDescriptor->m_health/ (2.0 * pixelD->m_cellDescriptor->m_baseHealth);
    s->setScaleX(healthRatio * kTileFrameSize);
    if (pixelD->m_cellDescriptor->m_character == komorki::eCellTypeBigBlue) {
      s->setScaleX(s->getScaleX() * 2);
    }
    
    return s;
  }
  
  
  bool PixelDebugView::IsInAABB(const Vec2& vec)
  {
    return IsInAABB(vec.x, vec.y);
  }
  
  bool PixelDebugView::IsInAABB(const int& x, const int& y)
  {
    if(x < m_a1 || x >= m_a2 ||
       y < m_b1 || y >= m_b2)
    {
      return false;
    }
    
    return true;
  }
  
  cocos2d::Vec2 PixelDebugView::spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)vec.x - m_a1) * kSpritePosition, ((int)vec.y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  cocos2d::Vec2 PixelDebugView::spriteVector(komorki::PixelDescriptor* pd, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)pd->x - m_a1) * kSpritePosition, ((int)pd->y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  void PixelDebugView::Update(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
  {
    
    removeAllChildren();
    
    return;
    for (int i = m_a1; i < m_a2; ++i)
    {
      for (int j = m_b1; j < m_b2; ++j)
      {
        auto pixelD = m_provider->GetDescriptor(i, j);
        if (pixelD->m_cellDescriptor && pixelD->m_cellDescriptor->parent == pixelD)
        {
          auto healthSprite = HealthSprite(pixelD);
          healthSprite->setPosition(this->spriteVector(pixelD, cocos2d::Vec2(0, kTileFrameSize + 2)));
        }
        
      }
    }
    
    return;
    
    
    for (int i = m_a1; i < m_a2; ++i)
    {
      for (int j = m_b1; j < m_b2; ++j)
      {
        auto pixelD = m_provider->GetDescriptor(i, j);
        if (pixelD->m_cellDescriptor && pixelD->m_cellDescriptor->m_character == komorki::eCellTypeSalad && pixelD->m_cellDescriptor->parent == pixelD)
        {
          int i = 0;
          auto cd = pixelD->m_cellDescriptor;
          cd->AroundRandom([this, &i](komorki::PixelDescriptor* pd, bool& stop)
                           {
                             if (pd == nullptr)
                             {
                               return ;
                             }
                             
                             auto s = this->ColorSprite(i++);
                             s->setPosition(this->spriteVector(pd));
                           });
        }
        
      }
    }
    
    
  }
  
  bool PixelDebugView::init()
  {
    if ( !SpriteBatchNode::initWithFile ("debugFrames.png", 10))
    {
      return false;
    }
    getTexture()->setAliasTexParameters();
    
    Reset();
    
    return true;
  }
  
  void PixelDebugView::Reset()
  {
    removeAllChildren();
  }
  
}
}
  