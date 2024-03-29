//
//  TerrainSprite.h
//  prsv
//
//  Created by Anton Simakov on 12.09.15.
//
//

#pragma once

#include "cocos2d.h"

namespace komorki
{
  namespace graphic
  {
    class TerrainSprite
    {
    public:
      
      static cocos2d::Sprite* create(int a, int b, int width, int height, const std::string& prefix)
      {
        std::string mapName = "Komorki/tmp/terrain/map";
        mapName += prefix + "_";
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
  }
}


