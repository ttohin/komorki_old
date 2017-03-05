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
#include "CellsLayer.h"
#include "DeadCellsLayer.h"


namespace komorki
{
  namespace graphic
  {
    AmorphCellContext::PolymorphShapeContext::Ptr AmorphCellContext::GetSprite(int x, int y)
    {
      std::string key = GetKey(x, y);
      auto s = m_spriteMap[key];
      return s;
    }
    
    AmorphCellContext::PolymorphShapeContext::Ptr AmorphCellContext::PopSprite(int x, int y)
    {
      std::string key = GetKey(x, y);
      auto s = m_spriteMap[key];
      m_spriteMap.erase(key);
      return s;
    }
    
    void AmorphCellContext::SetSprite(PolymorphShapeContext::Ptr s, int x, int y)
    {
      m_spriteMap[GetKey(x, y)] = s;
    }
    
    std::string AmorphCellContext::GetKey(int x, int y) const
    {
      //    const unsigned kMaxPolymorphSize = 256;
      //    unsigned long long key = (x + kMaxPolymorphSize) + (y + kMaxPolymorphSize) * kMaxPolymorphSize;
      //    return key;
      return std::to_string(x) + "_" + std::to_string(y);
    }
    
    AmorphCellContext::AmorphCellContext(PartialMapPtr _owner, const cocos2d::Rect& textureRect, CellDescriptor* cell)
    : ObjectContext(_owner)
    {
      m_cell = cell;
      m_textureRect = textureRect;
      
      m_centerSprite = CreateSprite();
      m_centerSprite->setTextureRect(m_textureRect);
      m_centerSprite->setColor(cocos2d::Color3B(200, 200, 200));
      m_centerSprite->setOpacity(kCenterAmorpghCellOpacity);
      m_centerSprite->setScale(kCenterCellScale);
      m_centerSprite->setAnchorPoint({0.5, 0.5});
      m_centerSprite->setLocalZOrder(10);
      
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
      SetSprite(context, pos.x, pos.y);
      
      m_aabb = aabb;
      
      {
        cocos2d::Vec2 centerPos = GetCenterPos();
        m_centerSprite->setPosition(centerPos);
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
          polymoprhContext = PopSprite(pos.x,
                                       pos.y);
        }
        else
        {
          offset = -m.delta;
          pos = m.pos + m.delta;
          
          polymoprhContext = GetSprite(m.pos.x,
                                       m.pos.y);
        }
        
        cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
        bool isNewSprite = false;
        if (!polymoprhContext)
        {
          polymoprhContext = CreateContext(aabb, pos);
          isNewSprite = true;
          
          SetSprite(polymoprhContext, m.pos.x, m.pos.y);
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
      
      {
        cocos2d::Vec2 centerPos = GetCenterPos();
        auto moveTo = cocos2d::MoveTo::create(animationDuration,
                                              centerPos);
        m_centerSprite->runAction(moveTo);
      }
    }
    
    void AmorphCellContext::AnimatePart(PolymorphShapeContext::Ptr& context, const Rect& aabb, float animationDuration)
    {
      cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
      
      float scale;
      cocos2d::Vec2 offset;
      CalcScale(aabb, context->targetPos, offset, scale);
      
      if (m_owner->m_enableAnimations)
      {
        context->sprite->stopAllActionsByTag(0);
        context->sprite->setPosition(context->offset + rectOffset + spriteVector(GetPosInOwnerBase(context->originalPos)));
        auto moveTo = cocos2d::MoveTo::create(animationDuration,
                                              offset + rectOffset + spriteVector(GetPosInOwnerBase(context->targetPos)));
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
            moveSpawn->setTag(0);
          }
          else
          {
            context->sprite->setScale(0.2);
            context->sprite->setOpacity(0);
            auto fadeAction = cocos2d::FadeTo::create(animationDuration, kMaxAmorpghCellOpacity);
            auto fadeSeq = cocos2d::Spawn::createWithTwoActions(moveSpawn, fadeAction);
            context->sprite->runAction(fadeSeq);
            moveSpawn->setTag(0);
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
        context->sprite->setPosition(offset + rectOffset + spriteVector(GetPosInOwnerBase(context->targetPos)));
      }
      
      context->offset = offset;
    }
    
    cocos2d::Sprite* AmorphCellContext::CreateSprite()
    {
      if( ! m_spritesPull.empty() )
      {
        auto s = m_spritesPull.front();
        s->sprite->setVisible(true);
        s->sprite->setOpacity(kMaxAmorpghCellOpacity);
        s->sprite->setLocalZOrder(0);
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
      //    if (m_spritesPull.size() < 10)
      //    {
      //      sprite->stopAllActions();
      //      sprite->setVisible(false);
      //      m_spritesPull.push_back(sprite);
      //    }
      //    else
      //    {
      //      assert(0);
      //      sprite->removeFromParentAndCleanup(true);
      //    }
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
        auto source = s;
        source->sprite->retain();
        source->sprite->stopAllActions();
        source->sprite->removeFromParentAndCleanup(true);
        _owner->m_cellMap->addChild(source->sprite);
        source->sprite->release();
      }
      
      for (auto &s : m_spriteMap)
      {
        auto source = s.second->sprite;
        source->retain();
        source->stopAllActions();
        source->removeFromParentAndCleanup(true);
        _owner->m_cellMap->addChild(source);
        source->release();
        
        cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
        source->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(s.second->originalPos), m_offset));
      }
      
      m_centerSprite->retain();
      m_centerSprite->stopAllActions();
      m_centerSprite->removeFromParentAndCleanup(true);
      _owner->m_cellMap->addChild(m_centerSprite);
      m_centerSprite->release();
      m_centerSprite->setPosition(GetCenterPos());
      
      m_owner = _owner;
    }
    
    void AmorphCellContext::Destory(PartialMapPtr _owner)
    {
      LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner.get());
      
      for (auto& s : m_spritesPull)
      {
        assert(s->sprite);
        
        m_owner->m_cellMap->RemoveSprite(s->sprite);
      }
      
      for (auto& s : m_spriteMap)
      {
        assert(s.second->sprite);
        m_owner->m_cellMap->RemoveSprite(s.second->sprite);
      }
      
      m_owner->m_cellMap->RemoveSprite(m_centerSprite);
      
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
      
      for (auto& s : m_spriteMap)
      {
        cocos2d::Vec2 originalPos = s.second->offset + rectOffset + spriteVector(GetPosInOwnerBase(s.second->targetPos));
        PlayAttackAnimation(s.second->sprite,
                            originalPos,
                            attackOffset,
                            animationDuration,
                            kSpriteScale,
                            kSpriteScale * 1.2);
      }
      
      PlayAttackAnimation(m_centerSprite,
                          GetCenterPos(),
                          attackOffset,
                          animationDuration,
                          kCenterCellScale,
                          kCenterCellScale * 1.2);
    }
    
    void AmorphCellContext::CellDead()
    {
      for (auto &spriteContext : m_spriteMap)
      {
        auto source = spriteContext.second->sprite;
        
        auto s = m_owner->m_background->CreateSprite();
        s->setTextureRect(m_textureRect);
        s->setPosition(source->getPosition());
        
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
}

