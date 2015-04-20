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

#define CREATURE_LINE_START 0
#define CREATURE_LINE_END 4
#define CREATURE_LINE 0
#define GROWND_START 0
#define GROWND_END  4
#define GROWND_LINE CREATURE_LINE_END
#define DEAD_CEEL_LINE (GROWND_LINE + 1)

#define ANIMATED 1
#define ANIMATE_DEAD_CELLS 1
#define PMP_ENABLE_LOGGING 0
#define PMP_DEBUG_FRAMES 0
#define PMP_PULL_SIZE 32
#define PMP_MAX_NUMBER_OF_CHILDREN 2000

#define RANDOM_OFFSET cRandEps(0.f, kTileFrameSize*0.1f)
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
  
  Rect OffsetForType(komorki::IPixelDescriptor* d)
  {
    komorki::IPixelDescriptor::Type t = d->GetType();
    if(t == komorki::IPixelDescriptor::CreatureType)
    {
      int index = cRandABInt(0, 4);
      int line = (CREATURE_LINE_START + d->m_character)%CREATURE_LINE_END;
      return Rect(index*kTileFrameSize, line*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    }
    if(t == komorki::IPixelDescriptor::TerrainType)
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
  
  Sprite* spriteForDescriptor(komorki::IPixelDescriptor* pixelD)
  {
    if (pixelD->GetType() == komorki::IPixelDescriptor::Empty)
    {
      return nullptr;
    }
    
    PixelDescriptorContext* context;
    pixelD->userData = static_cast<void*>(new PixelDescriptorContext);
    context = static_cast<PixelDescriptorContext *>(pixelD->userData);
    
    Rect r = OffsetForType(pixelD);
    auto s = CreateSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    if (pixelD->GetType() == komorki::IPixelDescriptor::CreatureType)
    {
      s->setOpacity(180);
    }
    context->sprite = s;
    
    return s;
  }
  
  Sprite* spriteDeadCell()
  {
    Rect r = Rect(0, DEAD_CEEL_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    auto s = CreateSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    s->setOpacity(180);
    s->setLocalZOrder(-1);
    
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

  void PreUpdate(const std::list<komorki::IPixelDescriptorProvider::UpdateResult>& updateResult)
  {
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

  }
  
  void PostUpdate(const std::list<komorki::IPixelDescriptorProvider::UpdateResult>& updateResult)
  {
    for (auto u : updateResult)
    {
      Vec2 destinationPos(0,0);
      Vec2 initialPos = Vec2(u.source.x, u.source.y);
      
      auto m = u.movement;
      auto a = u.action;
      
      if (u.shouldDelete || a == true)
        continue;
      
      if(u.addCreature == true)
      {
        destinationPos = Vec2(u.addCreature.value.to.x, u.addCreature.value.to.y);
      }
      
      if (m == true) {
        destinationPos = Vec2(m.value.destination.x, m.value.destination.y);
      }
      
      if(!IsInAABB(initialPos))
      {
        if(IsInAABB(destinationPos))
        {
          komorki::IPixelDescriptor* newDescriptor = nullptr;
          
          if (m == true)
          {
            newDescriptor = m.value.destinationDesc;
          }
          else if (u.addCreature == true)
          {
            newDescriptor = u.addCreature.value.destinationDesc;
          }
          
          auto context = static_cast<PixelDescriptorContext*>(newDescriptor->userData);
          context->i = destinationPos.x;
          context->j = destinationPos.y;
          
          m_upcomingDescriptors.push_back(context);
        }
        
        continue;
      }
    }
  }
  
  void Update(const std::list<komorki::IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
  {
    for (auto u : updateResult)
    {
      std::string operationType;
      
      komorki::PixelDescriptor* descriptor = static_cast<komorki::PixelDescriptor*>(u.desc);
     
      Vec2 destinationPos(0,0);
      Vec2 initialPos = Vec2(u.source.x, u.source.y);
     
      komorki::Vec2 pos = u.source;
      auto m = u.movement;
      auto a = u.action;
      
      if(u.movement == true)
        operationType = "movement";
      else if(u.action == true)
        operationType = "action";
      else if (u.addCreature == true)
        operationType = "addCreature";
      else if (u.shouldDelete == true)
        operationType = "delete";
      
      if(u.addCreature == true)
      {
        destinationPos = Vec2(u.addCreature.value.to.x, u.addCreature.value.to.y);
      }
      
      if(m == true)
      {
        destinationPos = Vec2(m.value.destination.x, m.value.destination.y);
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
        
        if(PMP_ENABLE_LOGGING)
        {
          log("c[0x%08llx] s[0x%08llx] d[0x%08llx] [%d:%d][%d:%d] %s [%d:%d]->[%d:%d] [id:%d] type:%d",
              (unsigned long long)context, (unsigned long long)source, (unsigned long long)descriptor,
              m_a1, m_a2, m_b1, m_b2, operationType.c_str(),
              (int)initialPos.x, (int)initialPos.y, (int)destinationPos.x, (int)destinationPos.y, descriptor->m_id, descriptor->GetType());
        }
      }
      
      if (u.shouldDelete)
      {
        if (descriptor->userData == context)
        {
          descriptor->userData = nullptr;
        }
        
        if(context == nullptr)
        {
          continue;
        }
        
        auto source = context->sprite;
        source->stopAllActions();
        RemoveSprite(source);
        
        delete context;
        
        if (_children.size() < PMP_MAX_NUMBER_OF_CHILDREN && ANIMATED && ANIMATE_DEAD_CELLS)
        {
          auto deadCellSprite = spriteDeadCell();
          
          Vec2 offset = RANDOM_OFFSET_VECTOR;
          deadCellSprite->setPosition(spriteVector(pos, offset));
          
          auto fade = FadeTo::create(10, 0);
          auto removeSelf = CallFunc::create([this, deadCellSprite]()
          {
            this->RemoveSprite(deadCellSprite);
          });
          
          deadCellSprite->runAction(Sequence::createWithTwoActions(fade, removeSelf));
        }
        
        continue;
      }
      
      if (u.addCreature == true)
      {
        auto newDescriptor = u.addCreature.value.destinationDesc;
        auto s = spriteForDescriptor(newDescriptor);

        Vec2 offset = RANDOM_OFFSET_VECTOR;
        auto context = static_cast<PixelDescriptorContext*>(newDescriptor->userData);
        context->offset = offset;
        
        if (ANIMATED)
        {
          s->setPosition(spriteVector(pos));
        }
        else
        {
          s->setPosition(spriteVector(u.addCreature.value.to, offset));
        }
        
        if (ANIMATED)
        {
          auto moveTo = MoveTo::create(updateTime*0.9, spriteVector(u.addCreature.value.to, offset));
          s->runAction(moveTo);
        }
        
        continue;
      }
      
      if(m == true)
      {
        auto source = context->sprite;
       
        Vec2 offset = RANDOM_OFFSET_VECTOR;
        
        if (ANIMATED)
        {
          source->setPosition(spriteVector(pos, context->offset));
          auto moveTo = MoveTo::create(updateTime*0.9, spriteVector(m.value.destination, offset));
          source->runAction(moveTo);
        }
        else
        {
          source->setPosition(spriteVector(m.value.destination, offset));
        }
        
        source->setScale(kSpriteScale);
        context->offset = offset;
      
        m.value.destinationDesc->userData = context;
        if(descriptor->userData == context)
        {
          descriptor->userData = nullptr;
        }
        
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
        auto destination = spriteVector(a.value.source, Vec2(kSpritePosition/2*a.value.delta.x,
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
  }
  
  bool init()
  {
    if ( !SpriteBatchNode::initWithFile ("tile_32x32.png", 10))
    {
      return false;
    }
    
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
