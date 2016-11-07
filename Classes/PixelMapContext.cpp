//
//  PixelMapContext.cpp
//  prsv
//
//  Created by Anton Simakov on 09.07.16.
//
//

#include "PixelMapContext.h"
#include "PartialMap.h"
#include "UICommon.h"
#include "UIConfig.h"
#include "PixelMapPartial.h"

using namespace komorki::ui;
using namespace komorki::ui::PixelMap;

namespace
{
  const float kMaxAmorpghCellOpacity = 200;
  const float kCenterAmorpghCellOpacity = 200;
  const float kCenterCellScale = 0.6;
}

#ifdef DEBUG_PARTIAL_MAP
#define LOG_W(...) KOMORKI_LOG(__VA_ARGS__)
#else
#define LOG_W(...) {}
#endif

#define SMALL_ANIMATION_TAG 99

namespace komorki
{
namespace ui
{
namespace PixelMap
{
  ObjectContext::ObjectContext(PartialMap *_owner)
  {
    LOG_W("%s, %s", __FUNCTION__, Description().c_str());
    m_owner = _owner;
  }
  
  ObjectContext::~ObjectContext()
  {
    LOG_W("%s, %s", __FUNCTION__, Description().c_str());
  }
  
  void ObjectContext::Free(PartialMap* _owner)
  {
    LOG_W("%s, %s: argument: %p", __FUNCTION__, Description().c_str(), _owner);
    if (m_owner == _owner)
    {
      m_owner = nullptr;
    }
  }
  
  std::string ObjectContext::Description() const
  {
    std::stringstream ss;
    ss <<
    static_cast<const void*>(this) <<
    "ownwer: " <<
    static_cast<const void*>(m_owner) <<
    ";";
    return ss.str();
  }
  

  
  void ObjectContext::ForceDestory(PartialMap* _owner)
  {
    LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner);
    delete this;
  }
  
  
  Vec2 ObjectContext::GetPosInOwnerBase(Vec2ConstRef pos) const
  {
    return pos - Vec2(m_owner->m_a1, m_owner->m_b1);
  }

  SingleCellContext::SingleCellContext(PartialMap *_owner,
                                       const cocos2d::Rect& textureRect,
                                       Vec2ConstRef origin,
                                       const Rect& rect)
  : ObjectContext(_owner)
  {
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
    s->setTag(static_cast<int>(komorki::PixelDescriptor::CreatureType));
    m_sprite = s;
  }
  
  void SingleCellContext::Move(Vec2ConstRef src, Vec2ConstRef dest, float animationTime)
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
  
  void SingleCellContext::ChangeRect(CellDescriptor* cd, const Rect& newRect, float animationDuration)
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

  void SingleCellContext::BecomeOwner(PartialMap* _owner)
  {
    LOG_W("%s, %s. New owner: %p", __FUNCTION__, Description().c_str(), _owner);
    m_owner = _owner;
    
    m_sprite->retain();
    m_sprite->removeFromParentAndCleanup(true);
    m_owner->m_cellMap->addChild(m_sprite);
    m_sprite->release();
    m_sprite->setPosition(spriteVector(m_pos, m_offset));
  }
  
  void SingleCellContext::Destory(PartialMap* _owner)
  {
    LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner);
    assert(m_owner == _owner || m_owner == nullptr);
    
    assert(m_sprite);
    assert(m_sprite->getParent() == _owner->m_cellMap.get());
    _owner->m_cellMap->RemoveSprite(m_sprite);
    
    delete this;
  }
  
  void SingleCellContext::Attack(const Vec2& pos, const Vec2& attackOffset, float animationDuration)
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
  
  void SingleCellContext::EnableSmallAnimations(bool enable)
  {
    if (enable == false)
    {
      m_sprite->stopAllActionsByTag(SMALL_ANIMATION_TAG);
      return;
    }
    
    if (m_owner->m_enableAnimations == false)
    {
      return;
    }
    
    m_sprite->stopAllActionsByTag(SMALL_ANIMATION_TAG);
    bool scaleDirection = rand()%2;
    auto s1 = cocos2d::ScaleTo::create(2, kSpriteScale * 0.8, kSpriteScale * 0.8);
    auto s2 = cocos2d::ScaleTo::create(2, kSpriteScale * 1.1, kSpriteScale * 1.1);
    cocos2d::ActionInterval* loop = nullptr;
    if (scaleDirection)
      loop = cocos2d::RepeatForever::create(cocos2d::Sequence::create(s1, s2, NULL));
    else
      loop = cocos2d::RepeatForever::create(cocos2d::Sequence::create(s2, s1, NULL));;
    loop->setTag(SMALL_ANIMATION_TAG);
    m_sprite->runAction(loop);
  }
  
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
  
  AmorphCellContext::AmorphCellContext(PartialMap *owner, const cocos2d::Rect& textureRect)
  : ObjectContext(owner)
  {
    m_textureRect = textureRect;
    
    m_centerSprite = CreateSprite();
    m_centerSprite->setTextureRect(m_textureRect);
    m_centerSprite->setColor(cocos2d::Color3B(200, 200, 200));
    m_centerSprite->setOpacity(kCenterAmorpghCellOpacity);
    m_centerSprite->setScale(kCenterCellScale);
    m_centerSprite->setAnchorPoint({0.5, 0.5});
    m_centerSprite->setLocalZOrder(10);
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
    s->setTag(static_cast<int>(komorki::PixelDescriptor::CreatureType + 10));
    
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
                                          komorki::Morphing& morph,
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
    
    morph.proccessed = true;
    
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
  
  void AmorphCellContext::BecomeOwner(PartialMap* _owner)
  {
    LOG_W("%s, %s. New owner: %p", __FUNCTION__, Description().c_str(), _owner);
    m_owner = _owner;

    for (auto& s : m_spritesPull)
    {
      auto source = s;
      source->sprite->retain();
      source->sprite->stopAllActions();
      source->sprite->removeFromParentAndCleanup(true);
      m_owner->m_cellMap->addChild(source->sprite);
      source->sprite->release();
    }
    
    for (auto &s : m_spriteMap)
    {
      auto source = s.second->sprite;
      source->retain();
      source->stopAllActions();
      source->removeFromParentAndCleanup(true);
      m_owner->m_cellMap->addChild(source);
      source->release();
      
      cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
      source->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(s.second->originalPos), m_offset));
    }
    
    m_centerSprite->retain();
    m_centerSprite->stopAllActions();
    m_centerSprite->removeFromParentAndCleanup(true);
    m_owner->m_cellMap->addChild(m_centerSprite);
    m_centerSprite->release();
    m_centerSprite->setPosition(GetCenterPos());
  }
  
  void AmorphCellContext::Destory(PartialMap* _owner)
  {
    LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner);
    assert(m_owner == _owner || m_owner == nullptr);
    
    for (auto& s : m_spritesPull)
    {
      assert(s->sprite);
      
      if (m_owner)
        m_owner->m_cellMap->RemoveSprite(s->sprite);
      else
        s->sprite->removeFromParentAndCleanup(true);
    }
    
    for (auto& s : m_spriteMap)
    {
      assert(s.second->sprite);
      if (m_owner)
        m_owner->m_cellMap->RemoveSprite(s.second->sprite);
      else
        s.second->sprite->removeFromParentAndCleanup(true);
    }
    
    if (m_owner)
      m_owner->m_cellMap->RemoveSprite(m_centerSprite);
    else
      m_centerSprite->removeFromParentAndCleanup(true);
    
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
} //namespace PixelMap
}
}

