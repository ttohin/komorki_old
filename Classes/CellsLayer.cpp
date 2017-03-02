//
//  CellsLayer.cpp
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#include "CellsLayer.h"

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "Random.h"
#include "CellDescriptor.h"
#include "UIConfig.h"
#include "UICommon.h"
#include "Utilities.h"
#include "CellShapes.h"
#include "Logging.h"

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

USING_NS_CC;

namespace komorki
{
  namespace graphic
  {
    CellsLayer::CellsLayer()
    {
      m_hlSprite = nullptr;
      m_pullSize = PMP_PULL_SIZE;
    }
    
    CellsLayer::~CellsLayer()
    {
      LOG_W("%s, %p", __FUNCTION__, this);
    }

    Sprite* CellsLayer::CreateSprite()
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
    
    void CellsLayer::RemoveSprite(Sprite* sprite)
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
    
    bool CellsLayer::init()
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
    
    void CellsLayer::Reset()
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
    
    void CellsLayer::SetUpdateTime(float updateTime)
    {
      m_updateTime = updateTime;
    }
    
  }
}
