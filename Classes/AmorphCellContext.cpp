//
//  AmorphCellContext.cpp
//  Komorki
//
//  Created by user on 02.03.17.
//
//

#include "AmorphCellContext.h"
#include "UICommon.h"
#include "UIConfig.h"
#include "PartialMap.h"
#include "Logging.h"
#include "SpriteBatch.h"


namespace komorki
{
  namespace graphic
  {
    AmorphCellContext::PolymorphShapeContext::Ptr AmorphCellContext::GetSprite(Vec2ConstRef pos)
    {
      auto it = m_spriteMap.find(pos);
      if (it != m_spriteMap.end())
      {
        return it->second;
      }
      else
      {
        return nullptr;
      }
    }
    
    AmorphCellContext::PolymorphShapeContext::Ptr AmorphCellContext::PopSprite(Vec2ConstRef pos)
    {
      auto it = m_spriteMap.find(pos);
      if (it != m_spriteMap.end())
      {
        auto result = it->second;
        m_spriteMap.erase(pos);
        return result;
      }
      else
      {
        return nullptr;
      }
    }
    
    void AmorphCellContext::SetSprite(PolymorphShapeContext::Ptr s, Vec2ConstRef pos)
    {
      m_spriteMap.insert(std::make_pair(pos, s));
    }
    
    AmorphCellContext::AmorphCellContext(PartialMapPtr _owner, const cocos2d::Rect& textureRect, CellDescriptor* cell)
    : ObjectContext(_owner)
    {
      m_cell = cell;
      m_textureRect = textureRect;

      
      LOG_W("%s, %s", __FUNCTION__, Description().c_str());
    }
    
    void CalcScale(const Rect& aabb, Vec2ConstRef pos, cocos2d::Vec2& outOffset, float& outScale)
    {
      Vec2 maxDistance = Vec2(aabb.size.x / 2, aabb.size.y / 2);
      Vec2 center = aabb.origin + maxDistance;
      
      if (maxDistance.x == 0) maxDistance.x = 1;
      if (maxDistance.y == 0) maxDistance.y = 1;
      
      Vec2 distance = center - pos;
      float distanceRatioX = (float)distance.x/(float)maxDistance.x;
      float distanceRatioY = (float)distance.y/(float)maxDistance.y;
      
      float offsetSizePerPixel = kSpritePosition * 0.3f;
      outOffset = cocos2d::Vec2(distanceRatioX * offsetSizePerPixel, distanceRatioY * offsetSizePerPixel);
      
      float scaleRatio = 0.5 - 0.5 * std::abs(distanceRatioX) + 0.5 - 0.5 * std::abs(distanceRatioY);
      outScale = 0.85 + 0.35 * scaleRatio;
    }
    
    AmorphCellContext::PolymorphShapeContext::Ptr AmorphCellContext::CreateContext(const Rect& aabb,
                                                                                   Vec2ConstRef originalPos)
    {
      auto s = CreateSprite();
      
      s->setVisible(true);
      s->setOpacity(kMaxAmorpghCellOpacity);
      s->setLocalZOrder(0);
      s->setTextureRect(m_textureRect);
      
      float scale;
      cocos2d::Vec2 offset;
      CalcScale(aabb, originalPos, offset, scale);
      
      s->setScale(scale);
      s->setAnchorPoint({0.5, 0.5});
      
      cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
      s->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(originalPos), m_offset + offset));
      s->setTag(static_cast<int>(komorki::GreatPixel::CreatureType + 10));
      
      auto context = std::make_shared<PolymorphShapeContext>();
      context->sprite = s;
      context->originalPos = originalPos;
      context->targetPos = originalPos;
      
      return context;
    }
    
    void AmorphCellContext::AddSprite(const Rect& aabb, Vec2ConstRef pos)
    {
      auto context = CreateContext(aabb, pos);
      SetSprite(context, pos);
      
      m_aabb = aabb;
      
      if (m_spriteMap.size() == 1)
      {
        auto centerSprite = CreateSprite();
        centerSprite->setTextureRect(m_textureRect);
        centerSprite->setColor(cocos2d::Color3B(200, 200, 200));
        centerSprite->setOpacity(kCenterAmorpghCellOpacity);
        centerSprite->setScale(kCenterCellScale);
        centerSprite->setAnchorPoint({0.5, 0.5});
        centerSprite->setLocalZOrder(10);
        context->centerSprite = centerSprite;
      }
      
      for (auto& s : m_spriteMap)
      {
        float scale;
        cocos2d::Vec2 offset;
        CalcScale(aabb, s.second->originalPos, offset, scale);
        
        cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
        auto targetPos = offset + rectOffset + spriteVector(GetPosInOwnerBase(s.second->targetPos));
        s.second->sprite->setPosition(targetPos);
        s.second->sprite->setScale(scale);
        s.second->offset = offset;
        
        if (s.second->centerSprite)
        {
          s.second->centerSprite->setPosition(targetPos);
        }
      }
    }
    
    void AmorphCellContext::MoveAmorphCells(Vec2ConstRef source,
                                            const komorki::Morphing& morph,
                                            const Rect& aabb,
                                            float animationDuration)
    {
      SpriteList toRemove;
      
      for (auto& s : m_spriteMap)
      {
        s.second->originalPos = s.second->targetPos;
        s.second->fade = false;
      }
      
      for (auto& m : morph.vec)
      {
        assert(m.dir == Morph::Outside || m.dir == Morph::Inside);
        
        Vec2 pos = m.pos;
        Vec2 offset = m.delta;
        
        AmorphCellContext::PolymorphShapeContext::Ptr polymoprhContext;
        
        if (m.dir == Morph::Inside)
        {
          polymoprhContext = PopSprite(pos);
          if (polymoprhContext && polymoprhContext->centerSprite)
          {
            auto targetCenter = GetSprite(pos + offset);
            assert(!targetCenter->centerSprite);
            targetCenter->centerSprite = polymoprhContext->centerSprite;
            targetCenter->useCenterSprite = false;
          }
        }
        else
        {
          offset = -m.delta;
          pos = m.pos + m.delta;
          
          polymoprhContext = GetSprite(m.pos);
        }
        
        cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
        bool isNewSprite = false;
        if (!polymoprhContext)
        {
          polymoprhContext = CreateContext(aabb, pos);
          isNewSprite = true;
          
          SetSprite(polymoprhContext, m.pos);
        }
        
        polymoprhContext->targetPos = pos + offset;
        polymoprhContext->originalPos = pos;
        polymoprhContext->direction = m.dir;
        polymoprhContext->fade = true;
        
        if (m.dir == Morph::Inside)
        {
          if (!isNewSprite) {
            toRemove.push_back(polymoprhContext);
          }
        }
      }
      
      m_spritesPull.insert(m_spritesPull.end(), toRemove.begin(), toRemove.end());
      
#warning Pedantic code
      for (auto& s : m_spriteMap)
      {
        auto it = std::find_if(m_spritesPull.begin(), m_spritesPull.end(), [&] (const PolymorphShapeContext::Ptr& testShape)
                               {
                                 return testShape->sprite == s.second->sprite;
                               });
        assert(it == m_spritesPull.end());
      }
      
      for (auto& s : toRemove) {
        AnimatePart(s, aabb, animationDuration);
      }
      
      for (auto& s : m_spriteMap)
      {
        AnimatePart(s.second, aabb, animationDuration);
      }
      
      m_aabb = aabb;
    }
    
    void AmorphCellContext::AnimatePart(PolymorphShapeContext::Ptr& context,
                                        const Rect& aabb,
                                        float animationDuration)
    {
      cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
      
      float scale;
      cocos2d::Vec2 offset;
      CalcScale(aabb, context->targetPos, offset, scale);
      
      cocos2d::Sprite* centralSprite = nullptr;
      if (context->centerSprite && context->fade)
      {
        assert(context->useCenterSprite);
        centralSprite = context->centerSprite;
        context->centerSprite = nullptr;
        context->useCenterSprite = true;
      }
      if (context->centerSprite && !context->fade)
      {
        if (context->useCenterSprite)
          centralSprite = context->centerSprite;
        
        context->useCenterSprite = true;
      }
      
      auto sourcePos = context->offset + rectOffset + spriteVector(GetPosInOwnerBase(context->originalPos));
      auto targetPos = offset + rectOffset + spriteVector(GetPosInOwnerBase(context->targetPos));
      
      if (m_owner->m_enableAnimations)
      {
        context->sprite->stopAllActionsByTag(0);
        context->sprite->setPosition(sourcePos);
        auto moveTo = cocos2d::MoveTo::create(animationDuration,
                                              targetPos);
        auto scaleTo = cocos2d::ScaleTo::create(animationDuration, scale);
        auto moveSpawn = cocos2d::Spawn::createWithTwoActions(moveTo, scaleTo);
        moveSpawn->setTag(0);

        if (context->fade)
        {
          if (context->direction == Morph::Inside)
          {
            auto removeSelf = cocos2d::FadeOut::create(animationDuration);
            auto removeSeq = cocos2d::Spawn::createWithTwoActions(moveSpawn, removeSelf);
            context->sprite->runAction(removeSeq);
            removeSeq->setTag(0);
          }
          else
          {
            context->sprite->setScale(0.2);
            context->sprite->setOpacity(0);
            auto fadeAction = cocos2d::FadeTo::create(animationDuration, kMaxAmorpghCellOpacity);
            auto fadeSeq = cocos2d::Spawn::createWithTwoActions(moveSpawn, fadeAction);
            context->sprite->runAction(fadeSeq);
            fadeSeq->setTag(0);
          }
        }
        else
        {
          context->sprite->setOpacity(kMaxAmorpghCellOpacity);
          context->sprite->runAction(moveSpawn);
        }
      }
      else // if (m_owner->m_enableAnimations)
      {
        context->sprite->setPosition(targetPos);
      }
      
      if (centralSprite)
      {
        centralSprite->stopAllActionsByTag(0);
        centralSprite->setPosition(sourcePos);
        auto moveTo = cocos2d::MoveTo::create(animationDuration * 0.8,
                                              targetPos);
        moveTo->setTag(0);
        centralSprite->runAction(moveTo);
      }
      
      context->offset = offset;
    }
    
    cocos2d::Sprite* AmorphCellContext::CreateSprite()
    {
      if( ! m_spritesPull.empty() )
      {
        auto s = m_spritesPull.front();
        m_spritesPull.pop_front();
        
        for (auto& sm : m_spriteMap)
        {
          if (sm.second) {
            assert(sm.second->sprite != s->sprite);
          }
        }
        
        return s->sprite;
      }
      else
      {
        return m_owner->m_cellMap->CreateSprite();
      }
    }
    
    void AmorphCellContext::RemoveSprite(cocos2d::Sprite* sprite)
    {
    }
    
    cocos2d::Vec2 AmorphCellContext::GetCenterPos() const
    {
      Vec2 maxDistance = Vec2(m_aabb.size.x / 2, m_aabb.size.y / 2);
      Vec2 center = m_aabb.origin + maxDistance;
      cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
      return rectOffset + spriteVector(GetPosInOwnerBase(center));
    }
    
    void AmorphCellContext::BecomeOwner(PartialMapPtr _owner)
    {
      LOG_W("%s, %s. New owner: %p", __FUNCTION__, Description().c_str(), _owner.get());
      
      for (auto& s : m_spritesPull)
      {
        auto& source = s;
        assert(!s->centerSprite);
        source->sprite->retain();
        source->sprite->stopAllActions();
        source->sprite->removeFromParentAndCleanup(true);
        _owner->m_cellMap->addChild(source->sprite);
        source->sprite->release();
      }
      
      for (auto &s : m_spriteMap)
      {
        auto& source = s.second->sprite;
        source->retain();
        source->stopAllActions();
        source->removeFromParentAndCleanup(true);
        _owner->m_cellMap->addChild(source);
        source->release();
        
        cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
        source->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(s.second->originalPos), m_offset));
        
        if (s.second->centerSprite)
        {
          s.second->centerSprite->retain();
          s.second->centerSprite->stopAllActions();
          s.second->centerSprite->removeFromParentAndCleanup(true);
          _owner->m_cellMap->addChild(s.second->centerSprite);
          s.second->centerSprite->release();
        }
      }
      
      m_owner = _owner;
    }
    
    void AmorphCellContext::Destory(PartialMapPtr _owner)
    {
      LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner.get());
      
      for (auto& s : m_spritesPull)
      {
        assert(s->sprite);
        assert(!s->centerSprite);
        m_owner->m_cellMap->RemoveSprite(s->sprite);
      }
      
      for (auto& s : m_spriteMap)
      {
        assert(s.second->sprite);
        m_owner->m_cellMap->RemoveSprite(s.second->sprite);
        if (s.second->centerSprite)
          m_owner->m_cellMap->RemoveSprite(s.second->centerSprite);
      }
      
      delete this;
    }
    
    void PlayAttackAnimation(cocos2d::Sprite* sprite,
                             const cocos2d::Vec2& originalPos,
                             const Vec2& attackOffset,
                             float animationDuration,
                             float initialScale,
                             float targetScale)
    {
      sprite->stopAllActionsByTag(10);
      sprite->setPosition(originalPos);
      auto m1 = cocos2d::MoveTo::create(animationDuration * 0.3, 0.5 * spriteVector(attackOffset) + originalPos);
      auto m2 = cocos2d::MoveTo::create(animationDuration * 0.3, originalPos);
      auto s1 = cocos2d::ScaleTo::create(animationDuration*0.3, targetScale, targetScale);
      auto s2 = cocos2d::ScaleTo::create(animationDuration*0.3, initialScale, initialScale);
      auto spawn1 = cocos2d::Spawn::createWithTwoActions(m1, s1);
      auto spawn2 = cocos2d::Spawn::createWithTwoActions(m2, s2);
      auto seq = cocos2d::Sequence::createWithTwoActions(spawn1, spawn2);
      seq->setTag(10);
      sprite->runAction(seq);
    }
    
    //**************************************************************************************************
    void AmorphCellContext::Attack(const Vec2& pos, const Vec2& attackOffset, float animationDuration)
    {
      if (!m_owner->m_enableAnimations)
      {
        return;
      }
      
      cocos2d::Vec2 offset = m_offset;
      cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
      
      for (auto& it : m_spriteMap)
      {
        cocos2d::Vec2 originalPos = it.second->offset + rectOffset + spriteVector(GetPosInOwnerBase(it.second->targetPos));
        PlayAttackAnimation(it.second->sprite,
                            originalPos,
                            attackOffset,
                            animationDuration,
                            kSpriteScale,
                            kSpriteScale * 1.2);
        
        if (it.second->centerSprite)
        {
          PlayAttackAnimation(it.second->centerSprite,
                              originalPos,
                              attackOffset,
                              animationDuration,
                              kCenterCellScale,
                              kCenterCellScale * 1.2);
        }
      }
    }
    

    void AmorphCellContext::ToggleAnimation()
    {
    }
    
    void AmorphCellContext::CellDead()
    {
      for (auto &it : m_spriteMap)
      {
        auto source = it.second->sprite;
        
        auto s = m_owner->m_background->CreateSprite();
        s->setTextureRect(m_textureRect);
        s->setPosition(source->getPosition());
        s->setOpacity(130);
        s->setScale(source->getScale());
        
        auto fade = cocos2d::FadeTo::create(5, 0);
        auto bgLayer = m_owner->m_background;
        auto removeSelf = cocos2d::CallFunc::create([bgLayer, s]()
                                           {
                                             bgLayer->RemoveSprite(s);
                                           });
        
        auto removeSeq = cocos2d::Sequence::createWithTwoActions(fade, removeSelf);
        s->runAction(removeSeq);
      }
    }
  }
}

