//
//  PixelMapPartial.h
//  prsv
//
//  Created by ttohin on 11.03.15.
//
//

#ifndef __prsv__PixelMapPartial__
#define __prsv__PixelMapPartial__

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"

#define CREATURE_START 0
#define CREATURE_END 4
#define CREATURE_LINE 0
#define GROWND_START 0
#define GROWND_END  4
#define GROWND_LINE 4

#define ANIMATED 1
#define PMP_ENABLE_LOGGING 0
#define PMP_DEBUG_FRAMES 0
#define PMP_PULL_SIZE 9640
#define PMP_ENABLE_PULL 0

#define RANDOM_OFFSET cRandEps(0.f, kTileFrameSize*0.1f)
#define RANDOM_OFFSET_VECTOR Vec2(RANDOM_OFFSET, RANDOM_OFFSET)

static unsigned long long NumberOfUpdates = 0;
static unsigned long long MinUpdateSize = 0;
static unsigned long long MaxUpdateSize = 0;
static unsigned long long TotalSize = 0;
static double AvaregeUpdateSize = 0;

static double kTileFrameSize = 32;
static int kSpritePosition = 32;
static double kSpriteScale = 32.f/kTileFrameSize;

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
  
  PixelMapPartial(int a, int b, int width, int height, PixelDescriptorProvider* provider)
  {
    m_a1 = a;
    m_a2 = a + width;
    m_b1 = b;
    m_b2 = b + height;
    m_width = width;
    m_height = height;
    m_provider = provider;
  }
  
  Rect OffsetForType(IPixelDescriptor* d)
  {
    IPixelDescriptor::Type t = d->GetType();
    if(t == IPixelDescriptor::CreatureType)
    {
      int index = cRandABInt(0, 4);
      int line = (CREATURE_START + d->m_character)%CREATURE_END;
      return Rect(index*kTileFrameSize, line*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    }
    if(t == IPixelDescriptor::TerrainType)
    {
      return Rect(cRandABInt(GROWND_START, GROWND_END)*kTileFrameSize, GROWND_LINE*kTileFrameSize, kTileFrameSize, kTileFrameSize);
    }
    
    return Rect(9, 0, 1, 1);
  }
  
#if PMP_ENABLE_PULL == 1
  Sprite* createSprite()
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
      return s;
    }
  }
  
  void RemoveSprite(Sprite* sprite)
  {
    sprite->setVisible(false);
    m_spritesPull.push_back(sprite);
  }
#else
  Sprite* createSprite()
  {
    auto s = Sprite::createWithTexture(getTexture());
    addChild(s);
    return s;
  }
  
  void RemoveSprite(Sprite* sprite)
  {
    sprite->removeFromParentAndCleanup(true);
  }
#endif
  
  Sprite* spriteForDescriptor(IPixelDescriptor* pixelD)
  {
    if (pixelD->GetType() == IPixelDescriptor::Empty)
    {
      return nullptr;
    }
    
    PixelDescriptorContext* context;
    pixelD->userData = new PixelDescriptorContext;
    context = (PixelDescriptorContext *)pixelD->userData;
    
    Rect r = OffsetForType(pixelD);
    auto s = createSprite();
    s->setTextureRect(r);
    s->setScale(kSpriteScale);
    if (pixelD->GetType() == IPixelDescriptor::CreatureType)
    {
      s->setOpacity(180);
    }
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
  
  Vec2 spriteVector(const pr::Vec2& vec, const Vec2& vector = Vec2())
  {
    auto result = Vec2(((int)vec.first - m_a1) * kSpritePosition, ((int)vec.second - m_b1) * kSpritePosition) + vector;
    return result;
  }
  
  void PreUpdate()
  {
  }

  void PreUpdate(const std::vector<IPixelDescriptorProvider::UpdateResult>& updateResult)
  {
    for (auto context : m_upcomingDescriptors)
    {
      auto source = context->sprite;
      source->retain();
      source->removeFromParentAndCleanup(true);
      this->addChild(source);
      source->release();
      source->setPosition(spriteVector(std::make_pair(context->i, context->j), context->offset));
    }
    
    m_upcomingDescriptors.clear();

  }
  
  void PostUpdate(const std::vector<IPixelDescriptorProvider::UpdateResult>& updateResult)
  {
    for (auto u : updateResult)
    {
      
      Vec2 destinationPos(0,0);
      Vec2 initialPos = Vec2(u.source.first, u.source.second);
      
      pr::Vec2 pos = u.source;
      auto m = u.movement;
      auto a = u.action;
      
      if (u.shouldDelete || a == true)
        continue;
      
      if(u.addCreature == true)
      {
        destinationPos = Vec2(u.addCreature.value.to.first, u.addCreature.value.to.second);
      }
      
      if (m == true) {
        destinationPos = Vec2(m.value.destination.first, m.value.destination.second);
      }
      
      if(!IsInAABB(initialPos))
      {
        if(IsInAABB(destinationPos))
        {
          IPixelDescriptor* newDescriptor = nullptr;
          
          if (m == true)
          {
            newDescriptor = m.value.destinationDesc;
          }
          else if (u.addCreature == true)
          {
            newDescriptor = u.addCreature.value.destinationDesc;
          }
          
          auto context = (PixelDescriptorContext*)newDescriptor->userData;
          context->i = destinationPos.x;
          context->j = destinationPos.y;
          
          m_upcomingDescriptors.push_back(context);
        }
        
        continue;
      }
    }
  }
  
  void Update(const std::vector<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime)
  {
    for (auto u : updateResult) {
      
      std::string operationType;
      
      PixelDescriptor* descriptor = (PixelDescriptor*)u.desc;
     
      Vec2 destinationPos(0,0);
      Vec2 initialPos = Vec2(u.source.first, u.source.second);
     
      pr::Vec2 pos = u.source;
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
        destinationPos = Vec2(u.addCreature.value.to.first, u.addCreature.value.to.second);
      }
      
      if(m == true)
      {
        destinationPos = Vec2(m.value.destination.first, m.value.destination.second);
      }
      
      if(!IsInAABB(initialPos))
      {
        continue;
      }
      
      PixelDescriptorContext* context = (PixelDescriptorContext*)u.userData;
      {
        Sprite* source = nullptr;
        if (context)
        {
          source = context->sprite;
        }
        
        if(PMP_ENABLE_LOGGING)
        {
          log("c[0x%08x] s[0x%08x] d[0x%08x] [%d:%d][%d:%d] %s [%d:%d]->[%d:%d] [id:%d] type:%d",
              context, source, descriptor,
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
        
        auto source = context->sprite;
        delete context;
        RemoveSprite(source);
        continue;
      }
      
      if (u.addCreature == true)
      {
        auto newDescriptor = u.addCreature.value.destinationDesc;
        auto s = spriteForDescriptor(newDescriptor);

        Vec2 offset = RANDOM_OFFSET_VECTOR;
        PixelDescriptorContext* context = (PixelDescriptorContext*)newDescriptor->userData;
        context->offset = offset;
        
        if (ANIMATED) {
          s->setPosition(spriteVector(pos));
        }
        else {
          s->setPosition(spriteVector(u.addCreature.value.to, offset));
        }
        
        if (ANIMATED) {
          auto moveTo = MoveTo::create(updateTime*0.9, spriteVector(u.addCreature.value.to, offset));
          s->runAction(moveTo);
        }
        
        continue;
      }
      
      if(m == true)
      {
        auto source = context->sprite;
       
        Vec2 offset = RANDOM_OFFSET_VECTOR;
        
        if (ANIMATED) {
          source->setPosition(spriteVector(pos, context->offset));
          auto moveTo = MoveTo::create(updateTime*0.9, spriteVector(m.value.destination, offset));
          source->runAction(moveTo);
        }
        else {
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
        if (!ANIMATED) {
          continue;
        }
        
        auto source = context->sprite;
        Vec2 offset = RANDOM_OFFSET_VECTOR;
        context->offset = offset;
      
        source->stopAllActions();
        auto destination = spriteVector(a.value.source, Vec2(kSpritePosition/2*a.value.delta.first,
                                                             kSpritePosition/2*a.value.delta.second) + RANDOM_OFFSET_VECTOR);;
        auto m1 = MoveTo::create(updateTime*0.3, destination);
        auto s1 = ScaleTo::create(updateTime*0.3, kSpriteScale * 1.5);
        auto m2 = MoveTo::create(updateTime*0.3, spriteVector(pos, offset));
        auto s2 = ScaleTo::create(updateTime*0.3, kSpriteScale);
        auto spawn1 = cocos2d::Spawn::createWithTwoActions(m1, s1);
        auto spawn2 = cocos2d::Spawn::createWithTwoActions(m2, s2);
        source->runAction(Sequence::createWithTwoActions(spawn1, spawn2));
      }
      
    }
    
  }
  
  bool init()
  {
    if ( !SpriteBatchNode::initWithFile ("tile_32x32.png", 10))
    {
      return false;
    }
   
    #if PMP_ENABLE_PULL == 1
    for(int i = 0; i < PMP_PULL_SIZE; ++i )
    {
      auto s = Sprite::createWithTexture(this->getTexture());
      s->setVisible(false);
      s->setScale(kSpriteScale);
      addChild(s);
      m_spritesPull.push_back(s);
    }
    #endif
    
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
        
        s->setPosition(Vec2((i - m_a1)*kSpritePosition, (j-m_b1)*kSpritePosition));
      }
    }
    
    return true;
  }
  
  int m_a1;
  int m_b1;
  int m_width;
  int m_height;
  int m_a2;
  int m_b2;
  PixelDescriptorProvider* m_provider;
  std::list<Sprite*> m_spritesPull;
  std::vector<PixelDescriptorContext*> m_upcomingDescriptors;
};

#endif /* defined(__prsv__PixelMapPartial__) */
