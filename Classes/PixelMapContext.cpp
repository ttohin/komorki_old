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
  
  AmorphCellContext::PolymorphShapeContext AmorphCellContext::GetSprite(int x, int y)
  {
    std::string key = GetKey(x, y);
    auto s = m_spriteMap[key];
    return s;
  }
  
  AmorphCellContext::PolymorphShapeContext AmorphCellContext::PopSprite(int x, int y)
  {
    std::string key = GetKey(x, y);
    auto s = m_spriteMap[key];
    m_spriteMap.erase(key);
    return s;
  }
  
  void AmorphCellContext::SetSprite(cocos2d::Sprite* s, int x, int y)
  {
    AmorphCellContext::PolymorphShapeContext c;
    c.sprite = s;
    c.pos = Vec2(x, y);
    m_spriteMap[GetKey(x, y)] = c;
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
  }
  
  void AmorphCellContext::AddSprite(Vec2ConstRef pos)
  {
    auto s = m_owner->m_cellMap->CreateSprite();
    s->setTextureRect(m_textureRect);
    s->setScale(kSpriteScale);
    s->setAnchorPoint({0.5, 0.5});
    s->setScale(kSpriteScale);
    
    cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
    s->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(pos), m_offset));
    s->setTag(static_cast<int>(komorki::PixelDescriptor::CreatureType + 10));
    
    SetSprite(s, pos.x, pos.y);
    
  }
  
  void AmorphCellContext::MoveAmorphCells(Vec2ConstRef source,
                                          komorki::Morphing& morph,
                                          float animationDuration)
  {
    SpriteList toRemove;
    for (auto& m : morph.vec)
    {
      assert(m.dir == Morph::Outside || m.dir == Morph::Inside);
      
      Vec2 pos = m.pos;
      Vec2 offset = m.delta;
      
      AmorphCellContext::PolymorphShapeContext polymoprhContext;
      
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
      if (polymoprhContext.sprite == nullptr)
      {
        isNewSprite = true;
        auto s = CreateSprite();
        s->setTextureRect(m_textureRect);
        s->setScale(kSpriteScale);
        s->setAnchorPoint({0.5, 0.5});
        
        s->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(m.pos), m_offset));
        s->setTag(static_cast<int>(komorki::PixelDescriptor::CreatureType + 10));
        
        polymoprhContext.sprite = s;
        polymoprhContext.pos = m.pos;
        
        SetSprite(s, polymoprhContext.pos.x, polymoprhContext.pos.y);
      }
      
      assert(polymoprhContext.sprite);
      
      if (m_owner->m_enableAnimations)
      {
        polymoprhContext.sprite->stopAllActionsByTag(0);
        polymoprhContext.sprite->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(pos)));
        auto moveTo = cocos2d::MoveTo::create(animationDuration, rectOffset + spriteVector(GetPosInOwnerBase(pos + offset)));
        
        if (m.dir == Morph::Inside)
        {
          auto removeSelf = cocos2d::FadeOut::create(animationDuration);
          auto removeSeq = cocos2d::Spawn::createWithTwoActions(moveTo, removeSelf);
          polymoprhContext.sprite->runAction(removeSeq);
          removeSeq->setTag(0);
          if (!isNewSprite) {
            toRemove.push_back(polymoprhContext);
          }
        }
        else
        {
          polymoprhContext.sprite->runAction(moveTo);
          moveTo->setTag(0);
        }
        
        polymoprhContext.sprite->setScale(kSpriteScale);
      }
      else
      {
        polymoprhContext.sprite->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(pos + offset)));
      }
    }
    
    m_spritesPull.insert(m_spritesPull.end(), toRemove.begin(), toRemove.end());
    
    for (auto& s : m_spriteMap)
    {
      auto it = std::find_if(m_spritesPull.begin(), m_spritesPull.end(), [&] (const PolymorphShapeContext& testShape)
      {
        return testShape.sprite == s.second.sprite;
      });
      assert(it == m_spritesPull.end());
    }
    
    morph.proccessed = true;
  }
  
  cocos2d::Sprite* AmorphCellContext::CreateSprite()
  {
    if( ! m_spritesPull.empty() )
    {
      auto s = m_spritesPull.front();
      s.sprite->setVisible(true);
      s.sprite->setOpacity(255);
      s.sprite->setLocalZOrder(0);
      m_spritesPull.pop_front();
      
      for (auto& sm : m_spriteMap)
      {
        assert(sm.second.sprite != s.sprite);
      }
      
      return s.sprite;
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
  
  void AmorphCellContext::BecomeOwner(PartialMap* _owner)
  {
    LOG_W("%s, %s. New owner: %p", __FUNCTION__, Description().c_str(), _owner);
    m_owner = _owner;

    for (auto& s : m_spritesPull)
    {
      auto source = s;
      source.sprite->retain();
      source.sprite->stopAllActions();
      source.sprite->removeFromParentAndCleanup(true);
      m_owner->m_cellMap->addChild(source.sprite);
      source.sprite->release();
    }
    
    for (auto &s : m_spriteMap)
    {
      auto source = s.second.sprite;
      source->retain();
      source->stopAllActions();
      source->removeFromParentAndCleanup(true);
      m_owner->m_cellMap->addChild(source);
      source->release();
      
      cocos2d::Vec2 rectOffset = spriteVector({1, 1}) * 0.5;
      
      source->setPosition(rectOffset + spriteVector(GetPosInOwnerBase(s.second.pos), m_offset));
    }
  }
  
  void AmorphCellContext::Destory(PartialMap* _owner)
  {
    LOG_W("%s, %s. caller: %p", __FUNCTION__, Description().c_str(), _owner);
    assert(m_owner == _owner || m_owner == nullptr);
    
    for (auto& s : m_spritesPull)
    {
      assert(s.sprite);
      assert(s.sprite->getParent() == _owner->m_cellMap.get());
      s.sprite->removeFromParentAndCleanup(true);
    }
    
    for (auto& s : m_spriteMap)
    {
      assert(s.second.sprite);
      assert(s.second.sprite->getParent() == _owner->m_cellMap.get());
      s.second.sprite->removeFromParentAndCleanup(true);
    }
    delete this;
  }
  
} //namespace PixelMap
}
}

