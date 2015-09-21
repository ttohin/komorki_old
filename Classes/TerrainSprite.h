//
//  TerrainSprite.h
//  prsv
//
//  Created by Anton Simakov on 12.09.15.
//
//

#ifndef prsv_TerrainSprite_h
#define prsv_TerrainSprite_h

#include "cocos2d.h"

class TerrainSprite
{
public:
  
  static cocos2d::Sprite* create(int a, int b, int width, int height)
  {
    std::string mapName = "Komorki/tmp/map";
    mapName += std::to_string(a) + "_" + std::to_string(b);
    mapName += "_";
    mapName += std::to_string(width) + "_" + std::to_string(height);
    mapName += ".png";
    
    mapName = cocos2d::FileUtils::getInstance()->getWritablePath() + mapName;

    auto s = cocos2d::Sprite::create(mapName);
    s->getTexture()->setAliasTexParameters();
    s->setAnchorPoint({0,0});
    
    return s;
  }
  
  
};

#endif
