//
//  StaticLightsLayer.hpp
//  prsv
//
//  Created by Anton Simakov on 30.05.15.
//
//

#ifndef prsv_StaticLightsLayer_hpp
#define prsv_StaticLightsLayer_hpp

#include "cocos2d.h"
#include "UIConfig.h"

namespace komorki
{
  namespace graphic
  {
    class StaticLightsLayer
    {
    public:
      static cocos2d::Sprite* create(int a, int b, int width, int height, const std::string& prefix)
      {
        std::string mapName = "Komorki/tmp/light_maps/";
        mapName += prefix + "_";
        mapName += std::to_string(a) + "_" + std::to_string(b);
        mapName += "_";
        mapName += std::to_string(width) + "_" + std::to_string(height);
        mapName += ".png";
        
        mapName = cocos2d::FileUtils::getInstance()->getWritablePath() + mapName;
        
        auto s = cocos2d::Sprite::create(mapName);
        s->getTexture()->setAliasTexParameters();
        s->setScale(kLightMapScale);
        s->setAnchorPoint({0,0});
        
        return s;
      }
    };
    
  }
}

#endif
