//
//  DeadCellsLayer.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "DeadCellsLayer.h"
#include "UIConfig.h"
#include "UICommon.h"
#include "ObjectContext.h"
#include "CellContext.h"
#include "AmorphCellContext.h"
#include "CellDescriptor.h"

#define ANIMATE_DEAD_CELLS 1
#define PMP_MAX_NUMBER_OF_CHILDREN 2000
#define PMP_PULL_SIZE 32
#define ANIMATED 1


USING_NS_CC;

namespace komorki
{
namespace graphic
{
  DeadCellsLayer::DeadCellsLayer(int a, int b, int width, int height)
  {
    m_a1 = a;
    m_a2 = a + width;
    m_b1 = b;
    m_b2 = b + height;
    m_width = width;
    m_height = height;
    m_pullSize = PMP_PULL_SIZE;
  }
  
  Sprite* DeadCellsLayer::CreateSprite()
  {
    Sprite* s = nullptr;
    if( ! m_spritesPull.empty() )
    {
      s = m_spritesPull.front();
      m_spritesPull.pop_front();
    }
    else
    {
      s = Sprite::createWithTexture(getTexture());
      addChild(s);
    }
    
    s->setVisible(true);
    s->setLocalZOrder(0);
    s->setScale(kSpriteScale);
    s->setAnchorPoint({0.5, 0.5});
    s->setColor(cocos2d::Color3B(150, 150, 150));
    s->setOpacity(110);
    s->setTag(static_cast<int>(komorki::PixelDescriptor::CreatureType));
    
    return s;
  }
  
  void DeadCellsLayer::RemoveSprite(Sprite* sprite)
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
  
  bool DeadCellsLayer::IsInAABB(const Vec2& vec)
  {
    return IsInAABB(vec.x, vec.y);
  }
  
  bool DeadCellsLayer::IsInAABB(const int& x, const int& y)
  {
    if(x < m_a1 || x >= m_a2 ||
       y < m_b1 || y >= m_b2)
    {
      return false;
    }
    
    return true;
  }
  
  cocos2d::Vec2 DeadCellsLayer::spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)vec.x - m_a1) * kSpritePosition, ((int)vec.y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  cocos2d::Vec2 spriteVectorWithOutOffset(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)vec.x) * kSpritePosition, ((int)vec.y ) * kSpritePosition) + vector;
    return result;
  }
  
  cocos2d::Vec2 DeadCellsLayer::spriteVector(komorki::PixelDescriptor* pd, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)pd->x - m_a1) * kSpritePosition, ((int)pd->y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  void DeadCellsLayer::Update(const WorldUpdateList& updateResult, float updateTime)
  {
    for (auto& u : updateResult)
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
          auto context = static_cast<ObjectContext*>(u.deleteCreature.value.cellDescriptor->userData);
          if (context && context->GetType() == ContextType::SinglePixel)
          {
            auto CellContext = static_cast<class CellContext*>(context);
            
            auto posOffset = CellContext->m_posOffset;
            auto offset = CellContext->m_offset;
            auto size = CellContext->m_size;
            auto pos = CellContext->m_pos;
            
            cocos2d::Vec2 rectOffset = spriteVectorWithOutOffset(size, {0, 0}) * 0.5;
            
            auto s = CreateSprite();
            
            s->setTextureRect(CellContext->m_textureRect);
            s->setPosition(spriteVectorWithOutOffset(pos + posOffset, offset + rectOffset));
            

            
            auto fade = FadeTo::create(5, 0);
            auto removeSelf = CallFunc::create([this, s]()
                                               {
                                                 this->RemoveSprite(s);
                                               });
            
            s->runAction(Sequence::createWithTwoActions(fade, removeSelf));
          }
          else if (context && context->GetType() == ContextType::ManyPixels)
          {
            auto amorphContext = static_cast<AmorphCellContext*>(context);
            
            for (auto &spriteContext : amorphContext->m_spriteMap)
            {
              auto source = spriteContext.second->sprite;
              
              auto s = CreateSprite();
              s->setTextureRect(amorphContext->m_textureRect);
              s->setPosition(source->getPosition());

              auto fade = FadeTo::create(5, 0);
              auto removeSelf = CallFunc::create([this, s]()
                                                 {
                                                   this->RemoveSprite(s);
                                                 });
              
              s->runAction(Sequence::createWithTwoActions(fade, removeSelf));
            }
            
          }
        }
        
        continue;
      }
    }
    
  }
  
  bool DeadCellsLayer::init()
  {
    std::string mapName = "Komorki/tmp/cells.png";
    
    mapName = cocos2d::FileUtils::getInstance()->getWritablePath() + mapName;
    
    if ( !SpriteBatchNode::initWithFile (mapName, 20))
    {
      return false;
    }
    
    getTexture()->setAliasTexParameters();
    
    return true;
  }
  
  void DeadCellsLayer::Reset()
  {
    removeAllChildren();
  }
  
}
}
