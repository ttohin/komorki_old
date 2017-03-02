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
    }
    
    PixelMapPartial::~PixelMapPartial()
    {
      LOG_W("%s, %p", __FUNCTION__, this);
    }

    Sprite* PixelMapPartial::CreateSprite()
    {
      Sprite* s = nullptr;
      if( ! m_spritesPull.empty() )
      {
        s = m_spritesPull.front();
        m_spritesPull.pop_front();
      }
      else
      {
        s = Sprite::createWithTexture(getTexture());
        addChild(s);
      }
      
      s->setVisible(true);
      s->setLocalZOrder(0);
      s->setColor(cocos2d::Color3B::WHITE);
      s->setOpacity(255);
      
      return s;
    }
    
    void PixelMapPartial::RemoveSprite(Sprite* sprite)
    {
      assert(sprite->getParent() == this);
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
    
    void PixelMapPartial::SetUpdateTime(float updateTime)
    {
      m_updateTime = updateTime;
    }
    
  }
}
