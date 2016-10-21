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
#include "ShapeAnalizer.hpp"

class CellCanvasSprite : public cocos2d::Node
{
public:
  virtual ~CellCanvasSprite() {}
  virtual bool init();
  void SetBuffer(const komorki::ShapeAnalizer::ResultBuffer& buffer);
  
private:
  void AddDebugSprites(int x, int y, const komorki::ShapeAnalizer::Part& value);
};

#endif /* defined(__prsv__CellCanvasSprite__) */
