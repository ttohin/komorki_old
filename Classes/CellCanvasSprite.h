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
#include "Common.h"

class CellCanvasSprite : public cocos2d::Node
{
public:
  
  static unsigned int kSpriteSize;
  
  virtual ~CellCanvasSprite() {}
  virtual bool init();
  void SetBuffer(const komorki::ShapeAnalizer::ResultBuffer& buffer, komorki::Vec2ConstRef offset);
  
private:
  void AddDebugSprites(int x, int y, const komorki::ShapeAnalizer::Part& value);
  void AddSimpleSprites(int x, int y, const komorki::ShapeAnalizer::Part& value);
  
  
  cocos2d::Color3B m_borderColor;
  cocos2d::Color3B m_bodyColor;
  cocos2d::Node* m_rootNode;
};

#endif /* defined(__prsv__CellCanvasSprite__) */
