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

#define ANIMATED 1

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
    
    bool GlowMapOverlay::IsGlowCell(komorki::CellDescriptor* cd) const
    {
      if (cd->m_character == eCellTypeImprovedSalad ||
          cd->m_character == eCellTypeSalad ||
          cd->m_character == eCellTypeBigBlue)
      {
        return true;
      }
      return false;
    }
    
    void GlowMapOverlay::SetUpdateTime(float updateTime)
    {
      m_updateTime = updateTime;
    }
   
    cocos2d::Vec2 GlowMapOverlay::SpriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector)
    {
      auto result = cocos2d::Vec2(vec.x * kSpritePosition + kSpritePosition/2.f, vec.y * kSpritePosition+ kSpritePosition/2.f) + vector;
      return result;
    }
    
    cocos2d::Rect GlowMapOverlay::OffsetForType(komorki::CellDescriptor* d)
    {
      int index = 0;
      if (d->m_character == komorki::eCellTypeSalad) index = 2;
      else if (d->m_character == komorki::eCellTypeBigBlue) index = 1;
      else if (d->m_character == komorki::eCellTypeImprovedSalad) index = 0;
      else assert(0);
      
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
        s->setOpacity(100);
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

    void GlowMapOverlay::Delete(PartialMap::Context* context)
    {
      if (context->glow)
      {
        RemoveSprite(context->glow);
      }
    }
    void GlowMapOverlay::AddCreature(CellDescriptor* cd, PartialMap::Context* context)
    {
      if (IsGlowCell(cd))
      {
        cocos2d::Rect r = OffsetForType(cd);
        auto s = CreateSprite();
        s->setTextureRect(r);
        
        if (cd->m_character == eCellTypeBigBlue)
        {
          s->setScale(kSpriteScale * 2);
        }
        else
        {
          s->setScale(kSpriteScale);
        }
        
        s->setPosition(SpriteVector(context->pos));
        context->glow = s;
      }
    }
    void GlowMapOverlay::AdoptSprite(PartialMap::Context* context)
    {
      auto source = context->glow;
      if (source == nullptr)
      {
        return;
      }
      source->retain();
      source->removeFromParentAndCleanup(true);
      this->addChild(source);
      source->release();
      source->setPosition(SpriteVector(context->pos, context->offset));
    }
    void GlowMapOverlay::MoveCreature(PartialMap::Context* context, const Vec2& source, const Vec2& dest)
    {
      if (context->glow == nullptr)
      {
        return;
      }
      
      assert(std::abs(source.x - dest.x) <= 2);
      assert(std::abs(source.y - dest.y) <= 2);
      
      auto randOffset = RandomVectorOffset();
      cocos2d::Vec2 offset = context->offset;
      Sprite* s = context->glow;
      
      if (ANIMATED)
      {
        s->stopAllActions();
        s->setPosition(SpriteVector(source, offset));
        auto moveTo = MoveTo::create(m_updateTime*0.9, SpriteVector(dest, randOffset));
        s->runAction(moveTo);
      }
      else
      {
        s->setPosition(SpriteVector(dest, randOffset));
      }
      
    }
    
  }
}