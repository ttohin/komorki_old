//
//  PixelMapPartial.h
//  komorki
//
//  Created by ttohin on 11.03.15.
//
//

#ifndef __komorki__PixelMapPartial__
#define __komorki__PixelMapPartial__

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include "CellDescriptor.h"

#define CREATURE_LINE_START 0
#define CREATURE_LINE_END 4
#define CREATURE_LINE 0
#define GROWND_START 0
#define GROWND_END  4
#define GROWND_LINE CREATURE_LINE_END
#define DEAD_CEEL_LINE (GROWND_LINE + 1)

#define RECREATE_EACH_UPDATE 0
#define ANIMATED 1
#define PMP_ENABLE_LOGGING 1
#define PMP_DEBUG_FRAMES 0
#define PMP_PULL_SIZE 32

#define RANDOM_OFFSET 0.f//cRandEps(0.f, kTileFrameSize*0.1f)
#define RANDOM_OFFSET_VECTOR Vec2(RANDOM_OFFSET, RANDOM_OFFSET)

static unsigned long long NumberOfUpdates = 0;
static unsigned long long MinUpdateSize = 0;
static unsigned long long MaxUpdateSize = 0;
static unsigned long long TotalSize = 0;
static double AvaregeUpdateSize = 0;

static double kTileFrameSize = 32;
static int kSpritePosition = 32;
static double kSpriteScale = kSpritePosition/kTileFrameSize;

USING_NS_CC;

class PixelMapPartial : public cocos2d::SpriteBatchNode
{
public:
  
  struct PixelDescriptorContext
  {
    Sprite* sprite;
    Vec2 offset;
    int i;
    int j;
  };
  
  PixelMapPartial(int a, int b, int width, int height, komorki::PixelDescriptorProvider* provider)
  {
    m_a1 = a;
    m_a2 = a + width;
    m_b1 = b;
    m_b2 = b + height;
    m_width = width;
    m_height = height;
    m_provider = provider;
    m_hlSprite = nullptr;
    m_pullSize = PMP_PULL_SIZE;
  }
  
  Rect OffsetForType(komorki::PixelDescriptor* pd)
  {
    komorki::PixelDescriptor::Type t = pd->m_type;
    if(t == komorki::PixelDescriptor::CreatureType)
    {
      auto d = pd->m_cellDescriptor;
      assert(d);
      int index = cRandABInt(0, 4);
      
      if (d->m_character == komorki::eCellTypeBigBlue)
      {
        return Rect(index*kTileFrameSize*2, 6*kTileFrameSize, kTileFrameSize*2, kTileFrameSize*2);
      }
      
      int line = 0;
      if (d->m_character == komorki::eCellTypeGreen) line = 0;
      else if (d->m_character == komorki::eCellTypeSalad) line = 1;
      else if (d->m_character == komorki::eCellTypeHunter) line = 2;
      else if (d->m_character == komorki::eCellTypeImprovedSalad) line = 3;
      else assert(0);
      
      return Rect(index*kTileFrameSize, line*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    }
    if(t == komorki::PixelDescriptor::TerrainType)
    {
      return Rect(cRandABInt(GROWND_START, GROWND_END)*kTileFrameSize, GROWND_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    }
    
    return Rect(9, 0, 1, 1);
  }
  
  Sprite* CreateSprite()
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
  
  void RemoveSprite(Sprite* sprite)
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
  
  Sprite* spriteForDescriptor(komorki::PixelDescriptor* pixelD)
  {
    if (pixelD->m_type == komorki::PixelDescriptor::Empty)
    {
      return nullptr;
    }
    
    if(pixelD->m_cellDescriptor != nullptr &&
       pixelD->m_cellDescriptor->parent != pixelD)
    {
      return nullptr;
    }
    
    PixelDescriptorContext* context = new PixelDescriptorContext();
    
    Rect r = OffsetForType(pixelD);
    auto s = CreateSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    if (pixelD->m_type == komorki::PixelDescriptor::CreatureType)
    {
      assert(pixelD->m_cellDescriptor);
      assert(pixelD->m_cellDescriptor->userData == nullptr);
     
#if RECREATE_EACH_UPDATE == 0
      pixelD->m_cellDescriptor->userData = context;
#endif
      s->setOpacity(180);
    }
    else
    {
    }
    s->setAnchorPoint({0, 0});
    context->sprite = s;
    
    return s;
  }
  
  bool IsInAABB(const Vec2& vec)
  {
    return IsInAABB(vec.x, vec.y);
  }
  
  bool IsInAABB(const int& x, const int& y)
  {
    if(x < m_a1 || x >= m_a2 ||
       y < m_b1 || y >= m_b2)
    {
      return false;
    }
    
    return true;
  }
  
  Vec2 spriteVector(const komorki::Vec2& vec, const Vec2& vector = Vec2())
  {
    auto result = Vec2(((int)vec.x - m_a1) * kSpritePosition, ((int)vec.y - m_b1) * kSpritePosition) + vector;
    return result;
  }

  void PreUpdate(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult)
  {
#if RECREATE_EACH_UPDATE == 0
    for (auto context : m_upcomingDescriptors)
    {
      auto source = context->sprite;
      source->retain();
      source->removeFromParentAndCleanup(true);
      this->addChild(source);
      source->release();
      source->setPosition(spriteVector(komorki::Vec2(context->i, context->j), context->offset));
    }
    
    m_upcomingDescriptors.clear();
#endif
  }
  
  void PostUpdate(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult)
  {
#if RECREATE_EACH_UPDATE == 0
    for (auto u : updateResult)
    {
      Vec2 destinationPos(0,0);
      Vec2 initialPos = Vec2(u.desc->x, u.desc->y);
      
      auto m = u.movement;
      auto a = u.action;
      
      if (u.deleteCreature == true || a == true)
        continue;
      
      if(u.addCreature == true)
      {
        destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
      }
      
      if (m == true) {
        destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
      }
      
      if(!IsInAABB(initialPos))
      {
        if(IsInAABB(destinationPos))
        {
          komorki::PixelDescriptor* newDescriptor = nullptr;
          
          if (m == true)
          {
            newDescriptor = m.value.destinationDesc;
          }
          else if (u.addCreature == true)
          {
            newDescriptor = u.addCreature.value.destinationDesc;
          }
          
          auto context = static_cast<PixelDescriptorContext*>(newDescriptor->m_cellDescriptor->userData);
          context->i = destinationPos.x;
          context->j = destinationPos.y;
          
          m_upcomingDescriptors.push_back(context);
        }
        
        continue;
      }
    }
#endif
  }
  
  void Update(const std::list<komorki::PixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
  {
#if RECREATE_EACH_UPDATE == 0
    for (auto u : updateResult)
    {
      std::string operationType;
      
//      komorki::PixelDescriptor* descriptor = static_cast<komorki::PixelDescriptor*>(u.desc);
     
      Vec2 destinationPos(0,0);
      Vec2 initialPos = Vec2(u.desc->x, u.desc->y);
      komorki::Vec2 pos(u.desc->x, u.desc->y);
     
      auto m = u.movement;
      auto a = u.action;
      
      if(u.movement == true)
        operationType = "movement";
      else if(u.action == true)
        operationType = "action";
      else if (u.addCreature == true)
        operationType = "addCreature";
      else if (u.deleteCreature == true)
        operationType = "delete";
      
      if(u.addCreature == true)
      {
        destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
      }
      
      if(m == true)
      {
        destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
      }
      
      if(!IsInAABB(initialPos))
      {
        continue;
      }
      
      PixelDescriptorContext* context = static_cast<PixelDescriptorContext*>(u.userData);
      {
        Sprite* source = nullptr;
        if (context)
        {
          source = context->sprite;
        }
      }
      
      if (u.deleteCreature == true)
      {
        if(context == nullptr)
        {
          continue;
        }
        
        auto source = context->sprite;
        source->stopAllActions();
        RemoveSprite(source);
        
        delete context;
        continue;
      }
      
      if (u.addCreature == true)
      {
        auto newDescriptor = u.addCreature.value.destinationDesc;
        auto s = spriteForDescriptor(newDescriptor);

        Vec2 offset = RANDOM_OFFSET_VECTOR;
        auto context = static_cast<PixelDescriptorContext*>(newDescriptor->m_cellDescriptor->userData);
        context->offset = offset;
        
        if (ANIMATED)
        {
          s->setPosition(spriteVector(pos));
        }
        else
        {
          s->setPosition(spriteVector(komorki::Vec2(newDescriptor->x, newDescriptor->y), offset));
        }
        
        if (ANIMATED)
        {
          auto moveTo = MoveTo::create(updateTime*0.9, spriteVector(komorki::Vec2(newDescriptor->x, newDescriptor->y), offset));
          s->runAction(moveTo);
        }
        
        continue;
      }
      
      if(m == true)
      {
        auto source = context->sprite;
       
        Vec2 offset = RANDOM_OFFSET_VECTOR;
        komorki::Vec2 dest(m.value.destinationDesc->x, m.value.destinationDesc->y);
        
        if (ANIMATED)
        {
          source->stopAllActions();
          source->setPosition(spriteVector(pos, context->offset));
          auto moveTo = MoveTo::create(updateTime*0.9, spriteVector(dest, offset));
          source->runAction(moveTo);
        }
        else
        {
          source->setPosition(spriteVector(dest, offset));
        }
        
        source->setScale(kSpriteScale);
        context->offset = offset;
      }
      else if (a == true)
      {
        if (!ANIMATED)
        {
          continue;
        }
        
        auto source = context->sprite;
        Vec2 offset = RANDOM_OFFSET_VECTOR;
        context->offset = offset;
      
        source->stopAllActions();
        auto destination = spriteVector(pos, Vec2(kSpritePosition/2*a.value.delta.x,
                                                             kSpritePosition/2*a.value.delta.y) + RANDOM_OFFSET_VECTOR);
        auto m1 = MoveTo::create(updateTime*0.3, destination);
        auto s1 = ScaleTo::create(updateTime*0.3, kSpriteScale * 1.5);
        auto m2 = MoveTo::create(updateTime*0.3, spriteVector(pos, offset));
        auto s2 = ScaleTo::create(updateTime*0.3, kSpriteScale);
        auto spawn1 = cocos2d::Spawn::createWithTwoActions(m1, s1);
        auto spawn2 = cocos2d::Spawn::createWithTwoActions(m2, s2);
        source->runAction(Sequence::createWithTwoActions(spawn1, spawn2));
      } // else if (a == true)
    }
#else
  Reset();
#endif
  }
  
  bool init()
  {
    if ( !SpriteBatchNode::initWithFile ("tile_32x32.png", 10))
    {
      return false;
    }
    
    getTexture()->setAliasTexParameters();
    Reset();
    
    return true;
  }
  
  void Reset()
  {
    m_upcomingDescriptors.clear();
    
    m_pullSize += _children.size() + 1;
    
    for(auto it=_children.cbegin(); it != _children.cend(); ++it)
    {
      (*it)->stopAllActions();
      RemoveSprite(static_cast<Sprite*>(*it));
    }
    
    m_pullSize = PMP_PULL_SIZE;
    
    if (PMP_DEBUG_FRAMES)
    {
      for (int i = m_a1; i < m_a2; ++i)
      {
        auto r = Rect(0, GROWND_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
        auto s = Sprite::createWithTexture(this->getTexture(), r);
        s->setScale(kSpriteScale);
        s->setOpacity(200);
        addChild(s);
        s->setPosition(Vec2((i - m_a1)*kSpritePosition, (0)*kSpritePosition));
      }
      
      for (int i = m_a1; i < m_a2; ++i)
      {
        auto r = Rect(0, GROWND_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
        auto s = Sprite::createWithTexture(this->getTexture(), r);
        s->setScale(kSpriteScale);
        s->setOpacity(200);
        addChild(s);
        s->setPosition(Vec2((i - m_a1)*kSpritePosition, (m_height - 1)*kSpritePosition));
      }
      
      for (int j = m_b1; j < m_b2; ++j)
      {
        auto r = Rect(0, GROWND_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
        auto s = Sprite::createWithTexture(this->getTexture(), r);
        s->setScale(kSpriteScale);
        s->setOpacity(200);
        addChild(s);
        s->setPosition(Vec2((0)*kSpritePosition, (j-m_b1)*kSpritePosition));
      }
      
      for (int j = m_b1; j < m_b2; ++j)
      {
        auto r = Rect(0, GROWND_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
        auto s = Sprite::createWithTexture(this->getTexture(), r);
        s->setScale(kSpriteScale);
        s->setOpacity(200);
        addChild(s);
        s->setPosition(Vec2((m_width - 1 )*kSpritePosition, (j-m_b1)*kSpritePosition));
      }
    }
    
    for (int i = m_a1; i < m_a2; ++i)
    {
      for (int j = m_b1; j < m_b2; ++j)
      {
        auto pixelD = m_provider->GetDescriptor(i, j);
        auto s = spriteForDescriptor(pixelD);
        if (s == nullptr)
        {
          continue;
        }
        
        Vec2 offset = RANDOM_OFFSET_VECTOR;
        s->setPosition(spriteVector(komorki::Vec2(i, j), offset));
      }
    }
    
    for (auto s : m_spritesPull)
    {
      s->removeFromParentAndCleanup(true);
    }
    m_spritesPull.clear();
  }
  
  void HightlightCellOnPos(int x, int y, komorki::CellType type)
  {
    if (m_hlSprite)
    {
      m_hlSprite->removeFromParentAndCleanup(true);
      m_hlSprite = nullptr;
    }
    
    if (!IsInAABB(x, y))
    {
      return;
    }
    
    auto s = Sprite::createWithTexture(getTexture());
    addChild(s, 999);
  
    int index = 0;
    int line = (CREATURE_LINE_START + type)%CREATURE_LINE_END;
    auto r = Rect(index*kTileFrameSize, line*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    s->setPosition(spriteVector(komorki::Vec2(x, y), Vec2::ZERO));
    
    m_hlSprite = s;
  }
  
  void StopHightlighting()
  {
    if (m_hlSprite)
    {
      m_hlSprite->removeFromParentAndCleanup(true);
      m_hlSprite = nullptr;
    }
  }
  
  int m_a1;
  int m_b1;
  int m_width;
  int m_height;
  int m_a2;
  int m_b2;
  Sprite* m_hlSprite;
  komorki::PixelDescriptorProvider* m_provider;
  std::list<Sprite*> m_spritesPull;
  unsigned int m_pullSize;
  std::vector<PixelDescriptorContext*> m_upcomingDescriptors;
};

#endif /* defined(__komorki__PixelMapPartial__) */
