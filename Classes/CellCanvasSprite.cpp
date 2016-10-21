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


namespace
{
  float kSpriteSize = 32;
}

bool CellCanvasSprite::init()
{
  if(!Node::init())
  {
    return false;
  }
  
//  for (int i = 0; i < 32; i++)
//  {
//    for (int j = 0; j < 32; j++)
//    {
//      auto a = Sprite::create("blank.png");
//      a->setScale(32, 32);
//      
//      int r = cRandABInt(0, 255);
//      int g = cRandABInt(0, 255);
//      int b = cRandABInt(0, 255);
//      a->setColor(cocos2d::Color3B(r, g, b));
//      a->setPosition(i * 32 + 16, j * 32 + 16);
//      
//      addChild(a);
//    }
//  }

  
  return true;
}



cocos2d::Vec2 OffsetForPart(const komorki::ShapeAnalizer::Part& part)
{
  switch (part.pos) {
    case komorki::ShapeAnalizer::PartPosition::TopLeft:
      return cocos2d::Vec2(-1, 1);
    case komorki::ShapeAnalizer::PartPosition::TopRight:
      return cocos2d::Vec2(1, 1);
    case komorki::ShapeAnalizer::PartPosition::BottomRight:
      return cocos2d::Vec2(1, -1);
    case komorki::ShapeAnalizer::PartPosition::BottomLeft:
      return cocos2d::Vec2(-1, -1);
    case komorki::ShapeAnalizer::PartPosition::Top:
      return cocos2d::Vec2(0, 1);
    case komorki::ShapeAnalizer::PartPosition::Bottom:
      return cocos2d::Vec2(0, -1);
    case komorki::ShapeAnalizer::PartPosition::Left:
      return cocos2d::Vec2(-1, 0);
    case komorki::ShapeAnalizer::PartPosition::Right:
      return cocos2d::Vec2(1, 0);
      
    default:
      return cocos2d::Vec2(0, 0);
  }
}

cocos2d::Vec2 OffsetForBorderToCorner(const komorki::ShapeAnalizer::Part& part)
{
  cocos2d::Vec2 result;
  if (part.type == komorki::ShapeAnalizer::PartType::BorderRightToCorner) result = cocos2d::Vec2(1, 0);
  if (part.type == komorki::ShapeAnalizer::PartType::BorderLeftToCorner) result = cocos2d::Vec2(-1, 0);
  if (part.type == komorki::ShapeAnalizer::PartType::BorderBottomToCorner) result = cocos2d::Vec2(0, -1);
  if (part.type == komorki::ShapeAnalizer::PartType::BorderTopToCorner) result = cocos2d::Vec2(0, 1);
  
  return result + OffsetForPart(part);
}

cocos2d::Size SizeForBorderToCorner(const komorki::ShapeAnalizer::Part& part)
{
  if (part.type == komorki::ShapeAnalizer::PartType::BorderRightToCorner) return cocos2d::Size(1, 2);
  if (part.type == komorki::ShapeAnalizer::PartType::BorderLeftToCorner) return cocos2d::Size(1, 2);
  if (part.type == komorki::ShapeAnalizer::PartType::BorderBottomToCorner) return cocos2d::Size(2, 1);
  if (part.type == komorki::ShapeAnalizer::PartType::BorderTopToCorner) return cocos2d::Size(2, 1);
  
  return cocos2d::Size(1, 1);
}

void CellCanvasSprite::AddDebugSprites(int _x, int _y, const komorki::ShapeAnalizer::Part& value)
{
  cocos2d::Vec2 pos(_x * kSpriteSize, _y * kSpriteSize);
  
  auto background = Sprite::create("blank.png");
  background->setScale(kSpriteSize * 0.9);
  background->setPosition(pos + cocos2d::Vec2(kSpriteSize * 0.1, kSpriteSize * 0.1));
  {
    int r = 255;
    int g = 255;
    int b = 255;
    background->setOpacity(200);
    background->setColor(cocos2d::Color3B(r, g, b));
  }
  
  if (value.type != komorki::ShapeAnalizer::PartType::Empty)
  {
    addChild(background);
  }
  
  if (value.type == komorki::ShapeAnalizer::PartType::Corner)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize);
    
    a->setScale(kSpriteSize / 2);
    a->setOpacity(150);
    
    cocos2d::Vec2 offset;
    offset = OffsetForPart(value) * (kSpriteSize / 2);
    
    int r = 255;
    int g = 0;
    int b = 0;
    a->setColor(cocos2d::Color3B(r, g, b));
    a->setPosition(pos + offset);
    addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::Border)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize);
    
    a->setScale(kSpriteSize / 2);
    a->setOpacity(150);
    
    cocos2d::Vec2 offset;
    offset = OffsetForPart(value) * (kSpriteSize / 2);
    
    int r = 255;
    int g = 0;
    int b = 255;
    a->setColor(cocos2d::Color3B(r, g, b));
    a->setPosition(pos + offset);
    addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::InnnerCornter)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize);
    
    a->setScale(kSpriteSize / 2);
    a->setOpacity(150);
    
    cocos2d::Vec2 offset;
    offset = OffsetForPart(value) * (kSpriteSize * 0.55);
    
    int r = 0;
    int g = 255;
    int b = 255;
    a->setColor(cocos2d::Color3B(r, g, b));
    a->setPosition(pos + offset);
    addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::Bridge)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize);
    
    a->setScale(kSpriteSize / 2);
    a->setOpacity(150);
    
    cocos2d::Vec2 offset;
    offset = OffsetForPart(value) * (kSpriteSize * 0.7);
    
    int r = 100;
    int g = 0;
    int b = 255;
    a->setColor(cocos2d::Color3B(r, g, b));
    a->setPosition(pos + offset);
    addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::BorderRightToCorner ||
           value.type == komorki::ShapeAnalizer::PartType::BorderLeftToCorner ||
           value.type == komorki::ShapeAnalizer::PartType::BorderBottomToCorner ||
           value.type == komorki::ShapeAnalizer::PartType::BorderTopToCorner)
  {
    auto a = Sprite::create("blank.png");
    
    cocos2d::Size size = SizeForBorderToCorner(value);
    
    a->setScale(size.width * kSpriteSize / 2, size.height * kSpriteSize / 2);
    a->setOpacity(150);
    
    cocos2d::Vec2 offset;
    offset = OffsetForBorderToCorner(value) * (kSpriteSize / 2);
    
    int r = 0;
    int g = 255;
    int b = 0;
    a->setColor(cocos2d::Color3B(r, g, b));
    a->setPosition(pos + offset);
    addChild(a);
  }
}

void CellCanvasSprite::SetBuffer(const komorki::ShapeAnalizer::ResultBuffer& buffer)
{
  buffer->ForEach([&](int _x, int _y, const komorki::ShapeAnalizer::Part& value){
    this->AddDebugSprites(_x, _y, value);
  });
}


