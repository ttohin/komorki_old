//
//  SpriteBatch.h
//  Komorki
//
//  Created on 05.03.17.
//

#pragma once

#include <string>
#include "cocos2d.h"
#include "UIConfig.h"

namespace komorki
{
  namespace graphic
  {
    class SpriteBatch : public cocos2d::SpriteBatchNode
    {
    public:
      
      static uint instanceCounter;
      
      SpriteBatch();
      virtual ~SpriteBatch();
      
      cocos2d::Sprite* CreateSprite();
      void RemoveSprite(cocos2d::Sprite* sprite);
      
      bool init(const std::string& textureFileName);
      void Reset();
      
    private:
      std::list<cocos2d::Sprite*> m_spritesPull;
    };
  }
}


