//
//  CellCanvasSprite.h
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#ifndef __prsv__CellCanvasSprite__
#define __prsv__CellCanvasSprite__

#include "cocos2d.h"

class CellCanvasSprite : public cocos2d::Node
{
public:
  virtual ~CellCanvasSprite() {}
  virtual bool init();
};

#endif /* defined(__prsv__CellCanvasSprite__) */
