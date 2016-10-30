//
//  StaticLightsSprite.cpp
//  Komorki
//
//  Created by Anton Simakov on 22.10.16.
//
//

#include "StaticLightsSprite.h"
#include "UIConfig.h"
#include "b2Utilites.h"
#include <cocos2d.h>


namespace
{
  unsigned int kGlowSpriteSize = 128;
}


bool StaticLightsSprite::init(komorki::IPixelDescriptorProvider* pixelMap, const komorki::Rect& rect)
{
  if ( !Node::init())
  {
    return false;
  }
  
  auto spritePatchNode = cocos2d::SpriteBatchNode::create("blank.png");
//  spritePatchNode->getTexture()->setAliasTexParameters();
  
  for (int i = 0; i < rect.size.x; ++i)
  {
    for (int j = 0; j < rect.size.y; ++j)
    {
      komorki::Vec2 pixelPos(i + rect.origin.x, j + rect.origin.y);
      
      auto light = pixelMap->GetDescriptor(pixelPos.x, pixelPos.y)->m_physicalDesc.light;
      
      auto sprite = cocos2d::Sprite::createWithTexture(spritePatchNode->getTexture());
      cocos2d::Vec2 pos(i, j);
      pos *= komorki::ui::kSpritePosition;
      
      const int kMaxLight = 255;
      const int kMinLight = 100;
      
      sprite->setOpacity(kMinLight + (kMaxLight - kMinLight) * light);
      sprite->setScale(komorki::ui::kSpritePosition);
      sprite->setAnchorPoint({0, 0});
      
//      sprite->setColor(cocos2d::Color3B(cRandABInt(0, 255), cRandABInt(0, 255), cRandABInt(0, 255)));

//      sprite->setBlendFunc(cocos2d::BlendFunc::ALPHA_PREMULTIPLIED);
//      sprite->setOpacityModifyRGB(false);
      sprite->setPosition(pos);
      
      spritePatchNode->addChild(sprite);
    }
  }
  
//  spritePatchNode->setPosition(cocos2d::Vec2(kGlowSpriteSize / 2, kGlowSpriteSize / 2));
//  spritePatchNode->setBlendFunc(cocos2d::BlendFunc::ALPHA_PREMULTIPLIED);
//  spritePatchNode->setOpacityModifyRGB(false);
  addChild(spritePatchNode);
  
  m_textureSize = cocos2d::Size(rect.size.x * komorki::ui::kSpritePosition,
                                rect.size.y * komorki::ui::kSpritePosition);

  return true;
}