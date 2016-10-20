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
#include "CellShapes.h"

#define CREATURE_LINE_START 0
#define CREATURE_LINE_END 4
#define CREATURE_LINE 0
#define GROWND_START 0
#define GROWND_END  4
#define GROWND_LINE CREATURE_LINE_END

#define RECREATE_EACH_UPDATE 0
#define PMP_ENABLE_LOGGING 1
#define PMP_PULL_SIZE 32

#define SMALL_ANIMATION_TAG 99

#ifdef DEBUG_PARTIAL_MAP
#define LOG_W(...) KOMORKI_LOG(__VA_ARGS__)
#else
#define LOG_W(...) {}
#endif

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
    
    PixelMapPartial::~PixelMapPartial()
    {
      LOG_W("%s, %p", __FUNCTION__, this);
    }
    
    int TagForType(komorki::PixelDescriptor* pd)
    {
      return static_cast<int>(pd->m_type);
    }
    
    cocos2d::Rect PixelMapPartial::OffsetForType(komorki::PixelDescriptor* pd)
    {
      komorki::PixelDescriptor::Type t = pd->m_type;
      if (t == komorki::PixelDescriptor::CreatureType)
      {
        auto d = pd->m_cellDescriptor;
        assert(d);
        int index = cRandABInt(0, 4);
        
        int line = 0;
        
        int groupIndex = 0;
        for (; groupIndex < 64; groupIndex++)
        {
          if (d->m_genom.m_groupId & (1 << groupIndex)) break;
        }
        
        line = groupIndex/30;
        index = groupIndex%30;
        
        return cocos2d::Rect(index*kTileFrameSize, line*kTileFrameSize, kTileFrameSize, kTileFrameSize);
      }
      if (t == komorki::PixelDescriptor::TerrainType)
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
      if (pixelD->m_type == komorki::PixelDescriptor::Empty
          || pixelD->m_type == komorki::PixelDescriptor::TerrainType)
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
    

    
    bool PixelMapPartial::init()
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
    
    void PixelMapPartial::Reset()
    {
      removeAllChildren();
//      m_pullSize += _children.size() + 1;
      
//      for(auto it=_children.cbegin(); it != _children.cend(); ++it)
//      {
//        (*it)->stopAllActions();
//        RemoveSprite(static_cast<Sprite*>(*it));
//      }
      
//      m_pullSize = PMP_PULL_SIZE;
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

    void PixelMapPartial::AddSprite(PixelDescriptor* pd, const Vec2& pos)
    {
      auto s = spriteForDescriptor(pd);
      
      if(pd->m_type != PixelDescriptor::Empty
        && pd->m_type != PixelDescriptor::TerrainType)
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
    
    void PixelMapPartial::AddPolymorphCreature(CellDescriptor* cd, PixelMap::AmorphCellContext* context, Vec2 partialMapOffset)
    {

    }
    
    void PixelMapPartial::MoveCreature(PixelMap::ObjectContext* context,
                                       const Vec2& source,
                                       const Vec2& dest,
                                       int duration,
                                       Morphing& morphing,
                                       Vec2 partialMapOffset,
                                       CellDescriptor* cd)
    {

    }
    
    void PlayAttackAnimation(cocos2d::Sprite* source,
                             const Vec2& pos,
                             const Vec2& direction,
                             cocos2d::Vec2& offset,
                             float updateTime,
                             const Vec2& size)
    {
      offset = RandomVectorOffset();
      
      source->stopAllActionsByTag(10);
      auto destination = spriteVector(pos, cocos2d::Vec2(kSpritePosition/2*direction.x,
                                                         kSpritePosition/2*direction.y) + RandomVectorOffset());
      auto m1 = MoveTo::create(updateTime*0.3, destination);
      auto s1 = ScaleTo::create(updateTime*0.3, kSpriteScale * size.x * 1.5, kSpriteScale * size.y * 1.5);
      auto m2 = MoveTo::create(updateTime*0.3, spriteVector(pos, offset));
      auto s2 = ScaleTo::create(updateTime*0.3, kSpriteScale * size.x, kSpriteScale * size.y);
      auto spawn1 = cocos2d::Spawn::createWithTwoActions(m1, s1);
      auto spawn2 = cocos2d::Spawn::createWithTwoActions(m2, s2);
      auto seq = Sequence::createWithTwoActions(spawn1, spawn2);
      seq->setTag(10);
      source->runAction(seq);
    }
    
//    void PixelMapPartial::Attack(PartialMap::Context* context, const Vec2& pos, const Vec2& direction, const Vec2& partialMapOffset)
//    {
//      if (kAnimated == false)
//      {
//        return;
//      }
//      
//      if (context->IsMultiShape())
//      {
//        for (auto& s : context->spriteMap)
//        {
//          auto source = s.second.sprite;
//          PlayAttackAnimation(source, s.second.pos - partialMapOffset, direction, context->offset, m_updateTime, context->size);
//        }
//
//        return;
//      }
//      
//      auto source = context->sprite;
//      PlayAttackAnimation(source, pos + context->posOffset, direction, context->offset, m_updateTime, context->size);
//    }
    
    
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
      
//      for(auto it = _children.cbegin(); it != _children.cend(); ++it)
//      {
//        if ((*it)->getTag() == static_cast<int>(komorki::PixelDescriptor::CreatureType))
//        {
//          PlaySmallAnimation(static_cast<Sprite*>(*it));
//        }
//      }
    }
    
  }
}
