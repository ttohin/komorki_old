//
//  CellContext.cpp
//  Komorki
//
//  Created by user on 02.03.17.
//
//

#include "CellContext.h"
#include "Logging.h"
#include "UICommon.h"
#include "PartialMap.h"
#include "UIConfig.h"
#include "SpriteBatch.h"
#include "CellDescriptor.h"

namespace
{
  static const auto kSmallAnimationsTag = 99;
}


namespace komorki
{
  namespace graphic
  {
    CellContext::CellContext(PartialMapPtr _owner,
                             const cocos2d::Rect& textureRect,
                             Vec2ConstRef origin,
                             const Rect& rect,
                             CellDescriptor* cell)
    : ObjectContext(_owner)
    {
      m_cell = cell;
      m_posOffset = rect.origin - origin;
      m_offset = RandomVectorOffset();
      m_size = rect.size;
      m_pos = GetPosInOwnerBase(origin);
      m_textureRect = textureRect;
      
      cocos2d::Vec2 rectOffset = spriteVector(m_size) * 0.5;
      
      auto s = m_owner->m_cellMap->CreateSprite();
      s->setTextureRect(textureRect);
      s->setScale(kSpriteScale);
      s->setAnchorPoint({0.5, 0.5});
      
      s->setPosition(spriteVector(m_pos + m_posOffset, m_offset + rectOffset));
      s->setTag(static_cast<int>(komorki::GreatPixel::CreatureType));
      m_sprite = s;
      
      LOG_W("%s, %s", __FUNCTION__, Description().c_str());
    }
    
    void CellContext::Move(Vec2ConstRef src, Vec2ConstRef dest, float animationTime)
    {
      Vec2 localSrc = GetPosInOwnerBase(src);
      Vec2 localDest = GetPosInOwnerBase(dest);
      
      auto randOffset = RandomVectorOffset();
      cocos2d::Vec2 offset = m_offset;
      m_offset = randOffset;
      cocos2d::Vec2 rectOffset = spriteVector(m_size) * 0.5;
      
      if (m_owner->m_enableAnimations)
      {
        m_sprite->stopAllActionsByTag(0);
        m_sprite->setPosition(spriteVector(localSrc + m_posOffset, offset + rectOffset));
        auto moveTo = cocos2d::MoveTo::create(animationTime, spriteVector(localDest + m_posOffset, randOffset + rectOffset));
        moveTo->setTag(0);
        m_sprite->runAction(moveTo);
      }
      else
      {
        m_sprite->setPosition(spriteVector(localDest + m_posOffset, randOffset + rectOffset));
      }
      
      m_pos = GetPosInOwnerBase(dest);
    }
    
    void CellContext::ChangeRect(CellDescriptor* cd, const Rect& newRect, float animationDuration)
    {
      m_pos = GetPosInOwnerBase(cd->parent->GetPos());
      m_posOffset = newRect.origin - cd->parent->GetPos();
      
      cocos2d::Vec2 scaleRatio((float)newRect.size.x / (float)m_size.x,
                               (float)newRect.size.y / (float)m_size.y);
      
      m_size = newRect.size;
      
      cocos2d::Vec2 rectOffset = spriteVector(m_size) * 0.5;
      
      m_sprite->stopAllActionsByTag(0);
      
      if (m_owner->m_enableAnimations)
      {
        auto moveTo = cocos2d::MoveTo::create(animationDuration, spriteVector(m_pos + m_posOffset, m_offset + rectOffset));
        auto scaleAction = cocos2d::ScaleTo::create(animationDuration, kSpriteScale * scaleRatio.x, kSpriteScale * scaleRatio.y);
        auto playSmallAnimation = cocos2d::CallFunc::create([this]()
                                                            {
                                                            });
        
        auto spawn = cocos2d::Spawn::create(moveTo, scaleAction, NULL);
        auto seq = cocos2d::Sequence::create(spawn, playSmallAnimation, NULL);
        seq->setTag(0);
        m_sprite->runAction(seq);
      }
      else
      {
        m_sprite->setPosition(spriteVector(m_pos + m_posOffset, m_offset + rectOffset));
        m_sprite->setScale(kSpriteScale * scaleRatio.x, kSpriteScale * scaleRatio.y);
      }
    }
    
    void CellContext::BecomeOwner(PartialMapPtr _owner)
    {
      LOG_W("%s, %s. New owner: %p", __FUNCTION__, Description().c_str(), _owner.get());
      
      m_sprite->retain();
      m_sprite->removeFromParentAndCleanup(true);
      _owner->m_cellMap->addChild(m_sprite);
      m_sprite->release();
      m_sprite->setPosition(spriteVector(m_pos, m_offset));
      
      m_owner = _owner;
    }
    
    void CellContext::Destory(PartialMapPtr _owner)
    {
      LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner.get());
      
      assert(m_sprite);
      m_owner->m_cellMap->RemoveSprite(m_sprite);
      
      delete this;
    }
    
    void CellContext::Attack(const Vec2& pos, const Vec2& attackOffset, float animationDuration)
    {
      Vec2 localSrc = GetPosInOwnerBase(pos);
      
      cocos2d::Vec2 offset = m_offset;
      cocos2d::Vec2 rectOffset = spriteVector(m_size) * 0.5;
      
      if (m_owner->m_enableAnimations)
      {
        m_sprite->stopAllActionsByTag(10);
        m_sprite->setPosition(spriteVector(localSrc + m_posOffset, offset + rectOffset));
        auto m1 = cocos2d::MoveTo::create(animationDuration * 0.3, 0.5 * spriteVector(attackOffset) + spriteVector(localSrc + m_posOffset, offset + rectOffset));
        auto m2 = cocos2d::MoveTo::create(animationDuration * 0.3, spriteVector(localSrc + m_posOffset, offset + rectOffset));
        auto s1 = cocos2d::ScaleTo::create(animationDuration*0.3, kSpriteScale * 1.2, kSpriteScale * 1.2);
        auto s2 = cocos2d::ScaleTo::create(animationDuration*0.3, kSpriteScale, kSpriteScale);
        auto spawn1 = cocos2d::Spawn::createWithTwoActions(m1, s1);
        auto spawn2 = cocos2d::Spawn::createWithTwoActions(m2, s2);
        auto seq = cocos2d::Sequence::createWithTwoActions(spawn1, spawn2);
        seq->setTag(10);
        m_sprite->runAction(seq);
      }
    }
    
    void CellContext::EnableSmallAnimations(bool enable)
    {
      if (enable == false)
      {
        m_sprite->stopAllActionsByTag(kSmallAnimationsTag);
        return;
      }
      
      if (m_owner->m_enableAnimations == false)
      {
        return;
      }
      
      m_sprite->stopAllActionsByTag(kSmallAnimationsTag);
      bool scaleDirection = rand()%2;
      auto s1 = cocos2d::ScaleTo::create(2, kSpriteScale * 0.8, kSpriteScale * 0.8);
      auto s2 = cocos2d::ScaleTo::create(2, kSpriteScale * 1.1, kSpriteScale * 1.1);
      cocos2d::ActionInterval* loop = nullptr;
      if (scaleDirection)
        loop = cocos2d::RepeatForever::create(cocos2d::Sequence::create(s1, s2, NULL));
      else
        loop = cocos2d::RepeatForever::create(cocos2d::Sequence::create(s2, s1, NULL));;
      loop->setTag(kSmallAnimationsTag);
      m_sprite->runAction(loop);
    }
    
    void CellContext::CellDead()
    {
      auto s = m_owner->m_background->CreateSprite();
      
      s->setTextureRect(m_textureRect);
      s->setPosition(m_sprite->getPosition());

      auto fade = cocos2d::FadeTo::create(5, 0);
      auto bgLayer = m_owner->m_background;
      auto removeSelf = cocos2d::CallFunc::create([bgLayer, s]()
                                         {
                                           bgLayer->RemoveSprite(s);
                                         });
      
      s->runAction(cocos2d::Sequence::createWithTwoActions(fade, removeSelf));
    }
  }
}
