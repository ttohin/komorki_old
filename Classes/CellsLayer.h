//
//  CellsLayer.h
//  komorki
//
//  Created by ttohin on 11.03.15.
//
//

#ifndef __komorki__CellsLayer__
#define __komorki__CellsLayer__

#include "cocos2d.h"
#include "CellDescriptor.h"
#include "PartialMap.h"

namespace komorki
{
  class GreatPixel;
  
  namespace graphic
  {
    class AmorphCellContext;
    class CellContext;
    
    class CellsLayer : public cocos2d::SpriteBatchNode
    {
    public:
      
      CellsLayer();
      virtual ~CellsLayer();
      
      cocos2d::Sprite* CreateSprite();
      void RemoveSprite(cocos2d::Sprite* sprite);
      
      bool init();
      void Reset();
      void SetUpdateTime(float updateTime);
      
    private:
      cocos2d::Sprite* m_hlSprite;
      std::list<cocos2d::Sprite*> m_spritesPull;
      unsigned int m_pullSize;
      float m_updateTime;
    };
  }
}

#endif /* defined(__komorki__CellsLayer__) */
