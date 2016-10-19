//
//  CellCanvasSprite.cpp
//  prsv
//
//  Created by Anton Simakov on 19.10.16.
//
//

#include "CellCanvasSprite.h"
#include "b2Utilites.h"

USING_NS_CC;

bool CellCanvasSprite::init()
{
  if(!Node::init())
  {
    return false;
  }
  
  for (int i = 0; i < 32; i++)
  {
    for (int j = 0; j < 32; j++)
    {
      auto a = Sprite::create("blank.png");
      a->setScale(32, 32);
      
      int r = cRandABInt(0, 255);
      int g = cRandABInt(0, 255);
      int b = cRandABInt(0, 255);
      a->setColor(cocos2d::Color3B(r, g, b));
      a->setPosition(i * 32 + 16, j * 32 + 16);
      
      addChild(a);
    }
  }

  
  return true;
}