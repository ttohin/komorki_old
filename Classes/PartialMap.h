//
//  PartialMap.h
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#pragma once

#include <memory>
#include <unordered_map>
#include <list>
#include "Common.h"
#include "cocos2d.h"
#include "WorldUpdateResult.h"
#include "GreatPixel.h"

namespace komorki
{
  class IPixelWorld;
  
  namespace graphic
  {
    class CellsLayer;
    class StaticLightsLayer;
    class DeadCellsLayer;
    class DynamicLightsLayer;
    class AmorphCellContext;
    class ObjectContext;
    class CellContext;
    class TerrainSprite;
    class TerrainBatchSprite;
    class SpriteBatch;
    
    class PartialMap
    {
    public:
      
      using Ptr = std::shared_ptr<PartialMap>;
      
      static uint instanceCounter;
      
      PartialMap();
      virtual ~PartialMap();
      
      bool InitF(int a,
                int b,
                int width,
                int height,
                IPixelWorld* provider,
                cocos2d::Node* superView,
                cocos2d::Node* lightNode,
                const cocos2d::Vec2& offset);

      void EnableSmallAnimations(bool enable);
      void EnableAnimations(bool enable);
      
      void Transfrorm(const cocos2d::Vec2& pos, float scale);
      void ChangeAABB(int a, int b, int width, int height);
      
      std::string Description();
      
      int m_a1;
      int m_b1;
      int m_width;
      int m_height;
      int m_a2;
      int m_b2;
      SpriteBatch* m_cellMap;
      SpriteBatch* m_background;
      bool m_enableAnimations = true;
      
    private:

      cocos2d::Sprite* m_lightOverlay;
      cocos2d::Sprite* m_terrainSprite;
      cocos2d::Sprite* m_terrainBgSprite;
      IPixelWorld* m_provider;
      bool m_enableSmallAnimations = false;
    };
  }
}

