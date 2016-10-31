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


//namespace
//{
//  float kSpriteSize = 32 / 4;
//}

unsigned int CellCanvasSprite::kSpriteSize = 32 / 4;

bool CellCanvasSprite::init()
{
  if(!Node::init())
  {
    return false;
  }
  
  m_rootNode = cocos2d::Node::create();
  addChild(m_rootNode);
  
  m_rootNode->setPosition(cocos2d::Vec2(kSpriteSize, kSpriteSize) * 0.5);
  
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
    m_rootNode->addChild(background);
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
    m_rootNode->addChild(a);
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
    m_rootNode->addChild(a);
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
    m_rootNode->addChild(a);
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
    m_rootNode->addChild(a);
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
    m_rootNode->addChild(a);
  }
}

cocos2d::Color3B BorderColor()
{
  int r = cRandABInt(0, 20);
  int g = cRandABInt(0, 20);
  int b = cRandABInt(0, 20);
  return cocos2d::Color3B(r, g, b);
}

cocos2d::Color3B BodyColor()
{
  int r = cRandABInt(20, 255);
  int g = cRandABInt(20, 255);
  int b = cRandABInt(20, 255);
  return cocos2d::Color3B(r, g, b);
}

void CellCanvasSprite::AddSimpleSprites(int _x, int _y, const komorki::ShapeAnalizer::Part& value)
{
  cocos2d::Vec2 pos(_x * kSpriteSize, _y * kSpriteSize);
  
  if (value.type == komorki::ShapeAnalizer::PartType::Corner)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize);
//    a->setOpacity(200);
    
    cocos2d::Vec2 offset;
    offset = -OffsetForPart(value) * (kSpriteSize * 0.2);
    
    a->setColor(m_borderColor);
    a->setPosition(pos + offset);
    m_rootNode->addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::Border)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize);
//    a->setOpacity(200);
    
    cocos2d::Vec2 offset;
    offset = -OffsetForPart(value) * (kSpriteSize * 0.1);
    
    a->setColor(m_borderColor);
    a->setPosition(pos + offset);
    m_rootNode->addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::InnnerCornter)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize * 0.5);
//    a->setOpacity(200);
    
    cocos2d::Vec2 offset;
    offset = OffsetForPart(value) * (kSpriteSize * 0.2);
    
    a->setColor(m_borderColor);
    a->setPosition(pos + offset);
    m_rootNode->addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::Bridge)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize * 1.2);
//    a->setOpacity(200);
    
    cocos2d::Vec2 offset;
    a->setColor(m_borderColor);
    a->setPosition(pos + offset);
    m_rootNode->addChild(a);
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::Fill)
  {
    auto a = Sprite::create("blank.png");
    a->setScale(kSpriteSize * cRandAB(1.5, 1.9));

    cocos2d::Vec2 offset;
    a->setColor(m_bodyColor);
    a->setPosition(pos + offset);
    a->setLocalZOrder(10);
    m_rootNode->addChild(a);
    
    if(cBoolRandPercent(0.1))
    {
      auto a = Sprite::create("blank.png");
      a->setScale(kSpriteSize * cRandAB(0.4, 1.0));

      cocos2d::Vec2 offset;
      a->setColor(m_borderColor);
      a->setPosition(pos);
      a->setOpacity(100);
      a->setLocalZOrder(11);
      m_rootNode->addChild(a);
    }
  }
  else if (value.type == komorki::ShapeAnalizer::PartType::BorderRightToCorner ||
           value.type == komorki::ShapeAnalizer::PartType::BorderLeftToCorner ||
           value.type == komorki::ShapeAnalizer::PartType::BorderBottomToCorner ||
           value.type == komorki::ShapeAnalizer::PartType::BorderTopToCorner)
  {
    auto a = Sprite::create("blank.png");
    
    a->setScale(kSpriteSize * 1.2);
//    a->setOpacity(200);
    
    cocos2d::Vec2 offset;
    offset = OffsetForBorderToCorner(value) * (kSpriteSize * 0.2);
    
    a->setColor(m_borderColor);
    a->setPosition(pos + offset);
    m_rootNode->addChild(a);
  }
}

void CellCanvasSprite::SetBuffer(const komorki::ShapeAnalizer::ResultBuffer& buffer,
                                 komorki::Vec2ConstRef offset,
                                 bool drowCorner)
{
  m_bodyColor = BodyColor();
  
  if (drowCorner)
  {
    m_borderColor.r = m_bodyColor.r * 0.3;
    m_borderColor.g = m_bodyColor.g * 0.3;
    m_borderColor.b = m_bodyColor.b * 0.3;
  }
  else{
    m_borderColor = m_bodyColor;
  }
  
  buffer->ForEach([&](int _x, int _y, const komorki::ShapeAnalizer::Part& value){
    this->AddSimpleSprites(offset.x + _x, offset.y + _y, value);
  });
}


