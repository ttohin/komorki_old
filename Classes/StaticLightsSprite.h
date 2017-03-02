//
//  StaticLightsSprite.h
//  Komorki
//
//  Created by Anton Simakov on 22.10.16.
//
//

#ifndef __Komorki__StaticLightsSprite__
#define __Komorki__StaticLightsSprite__

#include "cocos2d.h"
#include "IPixelWorld.h"

class StaticLightsSprite : public cocos2d::Node
{
public:
  
  virtual ~StaticLightsSprite() {}
  bool init(komorki::IPixelWorld* pixelMap, const komorki::Rect& rect);
  cocos2d::Size textureSize() const { return m_textureSize; };
  
private:
  
  cocos2d::Size m_textureSize;
};

#endif /* defined(__Komorki__StaticLightsSprite__) */
