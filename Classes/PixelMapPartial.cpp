//
//  PixelMapPartial.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "PixelMapPartial.h"

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "CellDescriptor.h"
#include "UIConfig.h"
#include "UICommon.h"
#include "Utilities.h"

#define CREATURE_LINE_START 0
#define CREATURE_LINE_END 4
#define CREATURE_LINE 0
#define GROWND_START 0
#define GROWND_END  4
#define GROWND_LINE CREATURE_LINE_END

#define RECREATE_EACH_UPDATE 0
#define ANIMATED 1
#define PMP_ENABLE_LOGGING 1
#define PMP_PULL_SIZE 32

#define SMALL_ANIMATION_TAG 99

USING_NS_CC;

namespace komorki
{
  namespace ui
  {
    PixelMapPartial::PixelMapPartial()
    {
      m_hlSprite = nullptr;
      m_pullSize = PMP_PULL_SIZE;
      m_enableSmallAnimations = true;
    }
    
    int TagForType(komorki::PixelDescriptor* pd)
    {
      return static_cast<int>(pd->m_type);
    }
    
    cocos2d::Rect PixelMapPartial::OffsetForType(komorki::PixelDescriptor* pd)
    {
      komorki::PixelDescriptor::Type t = pd->m_type;
      if(t == komorki::PixelDescriptor::CreatureType)
      {
        auto d = pd->m_cellDescriptor;
        assert(d);
        int index = cRandABInt(0, 4);
        
        if (d->m_character == komorki::eCellTypeBigBlue)
        {
          return cocos2d::Rect(index*kTileFrameSize*2, 6*kTileFrameSize, kTileFrameSize*2, kTileFrameSize*2);
        }
        
        int line = 0;
        if (d->m_character == komorki::eCellTypeGreen) line = 0;
        else if (d->m_character == komorki::eCellTypeSalad) line = 1;
        else if (d->m_character == komorki::eCellTypeHunter) line = 2;
        else if (d->m_character == komorki::eCellTypeImprovedSalad) line = 3;
        else assert(0);
        
        return cocos2d::Rect(index*kTileFrameSize, line*kTileFrameSize, kTileFrameSize, kTileFrameSize);
      }
      if(t == komorki::PixelDescriptor::TerrainType)
      {
        return cocos2d::Rect(cRandABInt(GROWND_START, GROWND_END)*kTileFrameSize, GROWND_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
      }
      
      return cocos2d::Rect(9, 0, 1, 1);
    }
    
    Sprite* PixelMapPartial::CreateSprite()
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
        s->setAnchorPoint({0, 0});
        addChild(s);
        return s;
      }
    }
    
    void PixelMapPartial::RemoveSprite(Sprite* sprite)
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
    
    Sprite* PixelMapPartial::spriteForDescriptor(komorki::PixelDescriptor* pixelD)
    {
      if (pixelD->m_type == komorki::PixelDescriptor::Empty)
      {
        return nullptr;
      }
      
      cocos2d::Rect r = OffsetForType(pixelD);
      auto s = CreateSprite();
      s->setTextureRect(r);
      s->setScale(kSpriteScale);
      s->setTag(TagForType(pixelD));
      
      return s;
    }
    
    cocos2d::Vec2 PixelMapPartial::spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
    {
      auto result = cocos2d::Vec2(vec.x * kSpritePosition, vec.y * kSpritePosition) + vector;
      return result;
    }
    
    bool PixelMapPartial::init()
    {
      if ( !SpriteBatchNode::initWithFile ("tile_32x32.png", 10))
      {
        return false;
      }
      
      getTexture()->setAliasTexParameters();
      Reset();
      
      return true;
    }
    
    void PixelMapPartial::Reset()
    {
#warning unimplemented
      
      //        m_pullSize += _children.size() + 1;
      //
//              for(auto it=_children.cbegin(); it != _children.cend(); ++it)
//              {
//                (*it)->stopAllActions();
//                RemoveSprite(static_cast<Sprite*>(*it));
//              }
      //
      //        m_pullSize = PMP_PULL_SIZE;
      //
      //        for (int i = m_a1; i < m_a2; ++i)
      //        {
      //          for (int j = m_b1; j < m_b2; ++j)
      //          {
      //            auto pixelD = m_provider->GetDescriptor(i, j);
      //            auto s = spriteForDescriptor(pixelD);
      //            if (s == nullptr)
      //            {
      //              continue;
      //            }
      //
      //            auto offset = RandomVectorOffset();
      //            s->setPosition(spriteVector(komorki::Vec2(i, j), offset));
      //          }
      //        }
      //
      //        for (auto s : m_spritesPull)
      //        {
      //          s->removeFromParentAndCleanup(true);
      //        }
      //        m_spritesPull.clear();
    }
    
    void PixelMapPartial::HightlightCellOnPos(int x, int y, komorki::CellType type)
    {
      if (m_hlSprite)
      {
        m_hlSprite->removeFromParentAndCleanup(true);
        m_hlSprite = nullptr;
      }
      
      auto s = Sprite::createWithTexture(getTexture());
      addChild(s, 999);
      
      int index = 0;
      int line = (CREATURE_LINE_START + type)%CREATURE_LINE_END;
      auto r = cocos2d::Rect(index*kTileFrameSize, line*kTileFrameSize, kTileFrameSize, kTileFrameSize);
      
      s->setTextureRect(r);
      s->setScale(kSpriteScale);
      s->setPosition(spriteVector(komorki::Vec2(x, y), cocos2d::Vec2::ZERO));
      
      m_hlSprite = s;
    }
    
    void PixelMapPartial::StopHightlighting()
    {
      if (m_hlSprite)
      {
        m_hlSprite->removeFromParentAndCleanup(true);
        m_hlSprite = nullptr;
      }
    }
    
    void PixelMapPartial::SetUpdateTime(float updateTime)
    {
      m_updateTime = updateTime;
    }
    
    void PixelMapPartial::Delete(PartialMap::Context* context)
    {
      assert(context->sprite);
      assert(context->sprite->getParent() == this);
      RemoveSprite(context->sprite);
    }
    
    void PixelMapPartial::AddSprite(PixelDescriptor* pd, const Vec2& pos)
    {
      auto s = spriteForDescriptor(pd);
      
      if(pd->m_type != PixelDescriptor::Empty)
      {
        assert(s);
      }
      else
      {
        assert(s == nullptr);
      }
      
      if (s)
      {
        s->setPosition(spriteVector(pos));
      }
      
    }
    
    void PixelMapPartial::AddCreature(CellDescriptor* cd, PartialMap::Context* context)
    {
      cocos2d::Rect r = OffsetForType(cd->parent);
      auto s = CreateSprite();
      s->setTextureRect(r);
      s->setScale(kSpriteScale);
      s->setOpacity(180);
      s->setAnchorPoint({0, 0});
      context->offset = RandomVectorOffset();
      
      s->setPosition(spriteVector(context->pos, context->offset));
      s->setTag(static_cast<int>(komorki::PixelDescriptor::CreatureType));
     
      if (m_enableSmallAnimations)
      {
        PlaySmallAnimation(s);
      }
      
      context->sprite = s;
    }
    
    void PixelMapPartial::AdoptSprite(PartialMap::Context* context)
    {
      auto source = context->sprite;
      source->retain();
      source->removeFromParentAndCleanup(true);
      this->addChild(source);
      source->release();
      source->setPosition(spriteVector(context->pos, context->offset));
      
      source->stopAllActionsByTag(SMALL_ANIMATION_TAG);
      
      if (m_enableSmallAnimations)
      {
        PlaySmallAnimation(source);
      }
    }
    
    void PixelMapPartial::MoveCreature(PartialMap::Context* context, const Vec2& source, const Vec2& dest)
    {
      assert(std::abs(source.x - dest.x) <= 2);
      assert(std::abs(source.y - dest.y) <= 2);
      
      auto randOffset = RandomVectorOffset();
      cocos2d::Vec2 offset = context->offset;
      Sprite* s = context->sprite;
      context->offset = randOffset;
      
      if (ANIMATED)
      {
        s->stopAllActionsByTag(0);
        s->setPosition(spriteVector(source, offset));
        auto moveTo = MoveTo::create(m_updateTime*0.9, spriteVector(dest, randOffset));
        moveTo->setTag(0);
        s->runAction(moveTo);
      }
      else
      {
        s->setPosition(spriteVector(dest, randOffset));
      }
    }
    
    void PixelMapPartial::Attack(PartialMap::Context* context, const Vec2& pos, const Vec2& direction)
    {
      auto source = context->sprite;
      auto offset = RandomVectorOffset();
      context->offset = offset;
      
      source->stopAllActionsByTag(10);
      auto destination = spriteVector(pos, cocos2d::Vec2(kSpritePosition/2*direction.x,
                                                         kSpritePosition/2*direction.y) + RandomVectorOffset());
      auto m1 = MoveTo::create(m_updateTime*0.3, destination);
      auto s1 = ScaleTo::create(m_updateTime*0.3, kSpriteScale * 1.5);
      auto m2 = MoveTo::create(m_updateTime*0.3, spriteVector(pos, offset));
      auto s2 = ScaleTo::create(m_updateTime*0.3, kSpriteScale);
      auto spawn1 = cocos2d::Spawn::createWithTwoActions(m1, s1);
      auto spawn2 = cocos2d::Spawn::createWithTwoActions(m2, s2);
      auto seq = Sequence::createWithTwoActions(spawn1, spawn2);
      seq->setTag(10);
      source->runAction(seq);
    }
    
    void PixelMapPartial::StopSmallAnimations()
    {
      if (!m_enableSmallAnimations) {
        return;
      }
      
      m_enableSmallAnimations = false;
      
      for(auto it = _children.cbegin(); it != _children.cend(); ++it)
      {
        if ((*it)->getTag() == static_cast<int>(komorki::PixelDescriptor::CreatureType))
        {
          (*it)->stopAllActionsByTag(SMALL_ANIMATION_TAG);
        }
      }
    }
    
    void PixelMapPartial::StartSmallAnimations()
    {
      if (m_enableSmallAnimations) {
        return;
      }
      
      m_enableSmallAnimations = true;
      
      for(auto it = _children.cbegin(); it != _children.cend(); ++it)
      {
        if ((*it)->getTag() == static_cast<int>(komorki::PixelDescriptor::CreatureType))
        {
          PlaySmallAnimation(static_cast<Sprite*>(*it));
        }
      }
    }
    
    void PixelMapPartial::PlaySmallAnimation(cocos2d::Sprite* s)
    {
      bool scaleDirection = rand()%2;
      auto s1 = ScaleTo::create(2, kSpriteScale*0.8);
      auto s2 = ScaleTo::create(2, kSpriteScale*1.1);
      ActionInterval* loop = nullptr;
      if (scaleDirection)
        loop = RepeatForever::create(Sequence::create(s1, s2, NULL));
      else
        loop = RepeatForever::create(Sequence::create(s2, s1, NULL));;
      loop->setTag(SMALL_ANIMATION_TAG);
      s->runAction(loop);
    }
  }
}
