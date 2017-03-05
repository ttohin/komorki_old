//
//  DeadCellsLayer.h
//  komorki
//
//  Created by ttohin on 11.03.15.
//
//

#pragma once

#include "cocos2d.h"
#include "WorldUpdateResult.h"

namespace komorki
{
  class GreatPixel;
  
  namespace graphic
  {
    class DeadCellsLayer : public cocos2d::SpriteBatchNode
    {
    public:
      
      DeadCellsLayer(int a, int b, int width, int height);
      cocos2d::Sprite* CreateSprite();
      void RemoveSprite(cocos2d::Sprite* sprite);
      bool IsInAABB(const Vec2& vec);
      bool IsInAABB(const int& x, const int& y);
      cocos2d::Vec2 spriteVector(const komorki::Vec2& vec, const cocos2d::Vec2& vector = cocos2d::Vec2());
      cocos2d::Vec2 spriteVector(komorki::GreatPixel* pd, const cocos2d::Vec2& vector = cocos2d::Vec2());
      void Update(const WorldUpdateResult& updateResult, float updateTime);
      bool init();
      void Reset();
      
      int m_a1;
      int m_b1;
      int m_width;
      int m_height;
      int m_a2;
      int m_b2;
      std::list<cocos2d::Sprite*> m_spritesPull;
      unsigned int m_pullSize;
    };
  }
}

