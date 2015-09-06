//
//  TerrainBatchSprite.h
//  ground
//
//  Created by ttohin on 05.09.15.
//
//

#ifndef __ground__TerrainBatchSprite__
#define __ground__TerrainBatchSprite__

#include "cocos2d.h"
#include "TerrainAnalizer.h"


class TerrainBatchSprite : public cocos2d::SpriteBatchNode
{
public:

  virtual ~TerrainBatchSprite() {}
  bool init(const TerrainAnalizer::Result& buffer);
  cocos2d::Sprite* spriteForDescriptor(const TerrainInfo& terrainInfo);
  cocos2d::Sprite* waterSprite();
  cocos2d::Color3B colorForInfo(const TerrainInfo& terrainInfo);
  
private:
  
  cocos2d::Rect OffsetForPos(TerrainPos pos);
  float Rotation(TerrainRotation rotation);
  TerrainAnalizer::Result m_source;
};

#endif /* defined(__ground__TerrainBatchSprite__) */
