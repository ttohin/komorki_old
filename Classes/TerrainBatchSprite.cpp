//
//  TerrainBatchSprite.cpp
//  ground
//
//  Created by ttohin on 05.09.15.
//
//

#include "TerrainBatchSprite.h"

USING_NS_CC;

static const float kSpriteSize = 32;
static const float kScale = 0.5;


cocos2d::Sprite* TerrainBatchSprite::waterSprite()
{
  auto s = Sprite::createWithTexture(getTexture());
  
  Rect r = Rect(kSpriteSize * 0, 2 * kSpriteSize, kSpriteSize, kSpriteSize);
  
  s->setTextureRect(r);
  s->setScale(kScale);
  addChild(s);
  
  return s;
}

cocos2d::Color3B TerrainBatchSprite::colorForInfo(const TerrainInfo& terrainInfo)
{
  switch (terrainInfo.resultLevel) {
    case TerrainLevel::Background:
      return Color3B(26, 46, 44);
    case TerrainLevel::Foreground:
      return Color3B(154, 19, 2);
    case TerrainLevel::Ground:
      return Color3B(131, 15, 3);
    default:
      return Color3B(255, 255, 255);
  }
}

cocos2d::Sprite* TerrainBatchSprite::spriteForDescriptor(const TerrainInfo& terrainInfo)
{
  if (terrainInfo.pos == TerrainPos::Unknown)
  {
    return nullptr;
  }
  
  auto s = Sprite::createWithTexture(getTexture());
 
  Rect r = OffsetForPos(terrainInfo.pos);
  s->setColor(colorForInfo(terrainInfo));
  s->setRotation(Rotation(terrainInfo.rotation));
  
  s->setTextureRect(r);
  s->setScale(kScale);
  addChild(s);
  
  return s;
}

bool TerrainBatchSprite::init(const TerrainAnalizer::Result& buffer)
{
  if ( !SpriteBatchNode::initWithFile ("ground.png", 100))
  {
    return false;
  }
  
  getTexture()->setAliasTexParameters();
 
  buffer.background->ForEach([&](const int& x, const int& y, const TerrainInfo& value)
                               {
                                 auto s = waterSprite();
                                 Vec2 pos = {x * kSpriteSize * kScale, y * kSpriteSize * kScale};
                                 pos += {kScale * kSpriteSize / 2.f, kScale * kSpriteSize / 2.f};
                                 s->setPosition(pos);
                               });
  
  buffer.background->ForEach([&](const int& x, const int& y, const TerrainInfo& value)
                   {
                     auto s = spriteForDescriptor(value);
                     if (s == nullptr) return;
                     Vec2 pos = {x * kSpriteSize * kScale, y * kSpriteSize * kScale};
                     pos += {kScale * kSpriteSize / 2.f, kScale * kSpriteSize / 2.f};
                     s->setPosition(pos);
                   });
  buffer.ground->ForEach([&](const int& x, const int& y, const TerrainInfo& value)
                               {
                                 auto s = spriteForDescriptor(value);
                                 if (s == nullptr) return;
                                 Vec2 pos = {x * kSpriteSize * kScale, y * kSpriteSize * kScale};
                                 pos += {kScale * kSpriteSize / 2.f, kScale * kSpriteSize / 2.f};
                                 s->setPosition(pos);
                               });
  buffer.foreground->ForEach([&](const int& x, const int& y, const TerrainInfo& value)
                               {
                                 auto s = spriteForDescriptor(value);
                                 if (s == nullptr) return;
                                 Vec2 pos = {x * kSpriteSize * kScale, y * kSpriteSize * kScale};
                                 pos += {kScale * kSpriteSize / 2.f, kScale * kSpriteSize / 2.f};
                                 s->setPosition(pos);
                               });
  return true;
}

cocos2d::Rect TerrainBatchSprite::OffsetForPos(TerrainPos pos)
{
  int randIndex = rand()%2;
  switch (pos) {
    case TerrainPos::Border:
      return Rect(1 * kSpriteSize, randIndex * kSpriteSize, kSpriteSize, kSpriteSize);
    case TerrainPos::Corner:
      return Rect(2 * kSpriteSize, randIndex * kSpriteSize, kSpriteSize, kSpriteSize);
    case TerrainPos::BorderToCornerLeft:
      return Rect(3 * kSpriteSize, randIndex * kSpriteSize, kSpriteSize, kSpriteSize);
    case TerrainPos::BorderToCornerRight:
      return Rect(4 * kSpriteSize, randIndex * kSpriteSize, kSpriteSize, kSpriteSize);
    case TerrainPos::Fill:
      return Rect(5 * kSpriteSize, randIndex * kSpriteSize, kSpriteSize, kSpriteSize);
    case TerrainPos::InnnerCornter:
      return Rect(0 * kSpriteSize, randIndex * kSpriteSize, kSpriteSize, kSpriteSize);
    default:
      assert(0);
  }
  return Rect::ZERO;
}

float TerrainBatchSprite::Rotation(TerrainRotation rotation)
{
  switch (rotation) {
    case TerrainRotation::Zero:
      return 0.f;
    case TerrainRotation::PI_2:
      return 90.f;
    case TerrainRotation::PI:
      return 180.f;
    case TerrainRotation::PI_3_2:
      return 270.f;
  }
  
  return 0.f;
}
