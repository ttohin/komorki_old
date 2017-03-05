//
//  SpriteBatchNode.cpp
//  Komorki
//
//  Created on 05.03.17.
//

#include "SpriteBatch.h"

USING_NS_CC;

namespace komorki
{
  namespace graphic
  {
    SpriteBatch::SpriteBatch()
    {
    }
    
    SpriteBatch::~SpriteBatch()
    {
    }
    
    Sprite* SpriteBatch::CreateSprite()
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
    
    void SpriteBatch::RemoveSprite(Sprite* sprite)
    {
      assert(sprite->getParent() == this);
      if (m_spritesPull.size() < kSpriteBatchNodePullSize)
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
    
    bool SpriteBatch::init(const std::string& textureFileName)
    {
      if ( !SpriteBatchNode::initWithFile (textureFileName, 20))
      {
        return false;
      }
      
      getTexture()->setAliasTexParameters();
      
      return true;
    }
    
    void SpriteBatch::Reset()
    {
      removeAllChildren();
      m_spritesPull.clear();
    }
    
  }
}
