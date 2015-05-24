//
//  PixelMapPartial.h
//  komorki
//
//  Created by ttohin on 11.03.15.
//
//

#ifndef __komorki__PixelMapBackground__
#define __komorki__PixelMapBackground__

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "PixelMapPartial.hpp"
#include "CellDescriptor.h"

#define ANIMATE_DEAD_CELLS 1
#define PMP_MAX_NUMBER_OF_CHILDREN 2000

class PixelMapBackground : public cocos2d::SpriteBatchNode
{
public:
  
  PixelMapBackground(int a, int b, int width, int height)
  {
    m_a1 = a;
    m_a2 = a + width;
    m_b1 = b;
    m_b2 = b + height;
    m_width = width;
    m_height = height;
    m_pullSize = PMP_PULL_SIZE;
  }
  
  Sprite* CreateSprite()
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
  
  void RemoveSprite(Sprite* sprite)
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
  
  Sprite* spriteDeadCell(komorki::CellDescriptor* cd)
  {
    Rect r = Rect(0, DEAD_CEEL_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    auto s = CreateSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    s->setOpacity(180);
    s->setLocalZOrder(-1);
    s->setAnchorPoint({0, 0});
    if(cd->m_character == komorki::eCellTypeBigBlue)
    {
      s->setScale(2*kSpriteScale);
    }
    else
    {
      s->setScale(kSpriteScale);
    }
    
    return s;
  }
  
  bool IsInAABB(const Vec2& vec)
  {
    return IsInAABB(vec.x, vec.y);
  }
  
  bool IsInAABB(const int& x, const int& y)
  {
    if(x < m_a1 || x >= m_a2 ||
       y < m_b1 || y >= m_b2)
    {
      return false;
    }
    
    return true;
  }
  
  Vec2 spriteVector(const komorki::Vec2& vec, const Vec2& vector = Vec2())
  {
    auto result = Vec2(((int)vec.x - m_a1) * kSpritePosition, ((int)vec.y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  Vec2 spriteVector(komorki::PixelDescriptor* pd, const Vec2& vector = Vec2())
  {
    auto result = Vec2(((int)pd->x - m_a1) * kSpritePosition, ((int)pd->y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  void Update(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
  {
    for (auto u : updateResult)
    {
      Vec2 initialPos = Vec2(u.desc->x, u.desc->y);
      
      komorki::Vec2 pos(u.desc->x, u.desc->y);
      
      if(!IsInAABB(initialPos))
      {
        continue;
      }
      
      if (u.deleteCreature == true)
      {
        if (ANIMATED && ANIMATE_DEAD_CELLS)
        {
          auto deadCellSprite = spriteDeadCell(u.deleteCreature.value.cellDescriptor.get());
          
          Vec2 offset = RANDOM_OFFSET_VECTOR;
          deadCellSprite->setPosition(spriteVector(pos, offset));
          
          auto fade = FadeTo::create(10, 0);
          auto removeSelf = CallFunc::create([this, deadCellSprite]()
                                             {
                                               this->RemoveSprite(deadCellSprite);
                                             });
          
          deadCellSprite->runAction(Sequence::createWithTwoActions(fade, removeSelf));
        }
        
        continue;
      }
    }

  }
  
  bool init()
  {
    if ( !SpriteBatchNode::initWithFile ("tile_32x32.png", 10))
    {
      return false;
    }
    
    Reset();
    
    return true;
  }
  
  void Reset()
  {
    removeAllChildren();
  }
  
  int m_a1;
  int m_b1;
  int m_width;
  int m_height;
  int m_a2;
  int m_b2;
  std::list<Sprite*> m_spritesPull;
  unsigned int m_pullSize;
};

#endif /* defined(__komorki__PixelMapBackground__) */
