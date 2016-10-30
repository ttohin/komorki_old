//
//  PixelMapBackground.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "PixelMapBackground.h"
#include "UIConfig.h"
#include "UICommon.h"

#define ANIMATE_DEAD_CELLS 1
#define PMP_MAX_NUMBER_OF_CHILDREN 2000
#define PMP_PULL_SIZE 32
#define ANIMATED 1
#define CREATURE_LINE_START 0
#define CREATURE_LINE_END 4
#define CREATURE_LINE 0
#define GROWND_START 0
#define GROWND_END  4
#define GROWND_LINE CREATURE_LINE_END
#define DEAD_CEEL_LINE (GROWND_LINE + 1)

namespace komorki
{
namespace ui
{
  
USING_NS_CC;

  PixelMapBackground::PixelMapBackground(int a, int b, int width, int height)
  {
    m_a1 = a;
    m_a2 = a + width;
    m_b1 = b;
    m_b2 = b + height;
    m_width = width;
    m_height = height;
    m_pullSize = PMP_PULL_SIZE;
  }
  
  Sprite* PixelMapBackground::CreateSprite()
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
  
  void PixelMapBackground::RemoveSprite(Sprite* sprite)
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
  
  Sprite* PixelMapBackground::spriteDeadCell(komorki::CellDescriptor* cd)
  {
    cocos2d::Rect r = cocos2d::Rect(0, DEAD_CEEL_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    auto s = CreateSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    s->setOpacity(180);
    s->setLocalZOrder(-1);
    s->setAnchorPoint({0, 0});
    s->setScale(kSpriteScale);
    
    return s;
  }
  
  bool PixelMapBackground::IsInAABB(const Vec2& vec)
  {
    return IsInAABB(vec.x, vec.y);
  }
  
  bool PixelMapBackground::IsInAABB(const int& x, const int& y)
  {
    if(x < m_a1 || x >= m_a2 ||
       y < m_b1 || y >= m_b2)
    {
      return false;
    }
    
    return true;
  }
  
  cocos2d::Vec2 PixelMapBackground::spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)vec.x - m_a1) * kSpritePosition, ((int)vec.y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  cocos2d::Vec2 spriteVectorWithOutOffset(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)vec.x) * kSpritePosition, ((int)vec.y ) * kSpritePosition) + vector;
    return result;
  }
  
  cocos2d::Vec2 PixelMapBackground::spriteVector(komorki::PixelDescriptor* pd, const cocos2d::Vec2& vector)
  {
    auto result = cocos2d::Vec2(((int)pd->x - m_a1) * kSpritePosition, ((int)pd->y - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  void PixelMapBackground::Update(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
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
          auto context = static_cast<PixelMap::ObjectContext*>(u.deleteCreature.value.cellDescriptor->userData);
          if (context && context->GetType() == PixelMap::ContextType::SinglePixel)
          {
            auto singleCellContext = static_cast<PixelMap::SingleCellContext*>(context);
            
            auto posOffset = singleCellContext->m_posOffset;
            auto offset = singleCellContext->m_offset;
            auto size = singleCellContext->m_size;
            auto pos = singleCellContext->m_pos;
            
            cocos2d::Vec2 rectOffset = spriteVectorWithOutOffset(size, {0, 0}) * 0.5;
            
            auto s = CreateSprite();
            s->setTextureRect(singleCellContext->m_textureRect);
            s->setScale(kSpriteScale);
            s->setAnchorPoint({0.5, 0.5});
            
            s->setPosition(spriteVectorWithOutOffset(pos + posOffset, offset + rectOffset));
            s->setTag(static_cast<int>(komorki::PixelDescriptor::CreatureType));
            s->setColor(cocos2d::Color3B(20, 20, 20));
            s->setOpacity(110);
            
            auto fade = FadeTo::create(5, 0);
            auto removeSelf = CallFunc::create([this, s]()
                                               {
                                                 this->RemoveSprite(s);
                                               });
            
            s->runAction(Sequence::createWithTwoActions(fade, removeSelf));
          }
        }
        
        continue;
      }
    }
    
  }
  
  bool PixelMapBackground::init()
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
  
  void PixelMapBackground::Reset()
  {
    removeAllChildren();
  }
  

  
}
}
