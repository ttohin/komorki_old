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
#include "PixelDescriptor.h"


namespace komorki
{
  class IPixelDescriptorProvider;
  
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
    
    class PartialMap
    {
    public:
      
      PartialMap();
      virtual ~PartialMap();
      
      bool Init(int a,
                int b,
                int width,
                int height,
                IPixelDescriptorProvider* provider,
                cocos2d::Node* superView,
                cocos2d::Node* lightNode,
                const cocos2d::Vec2& offset);
      
      void Move(const Vec2& newPos);
      
      void AdoptIncomingItems();
      void DeleteOutgoingItems();
      void HandleItemsOnBounds(const WorldUpdateList& updateResult, float updateTime);
      void Update(WorldUpdateList& updateResult, float updateTime);
      
      void Reset();
      
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
      std::shared_ptr<CellsLayer> m_cellMap;
      bool m_enableAnimations = true;
      
    private:
   
      void InitPixel(PixelDescriptor* pd);
      ObjectContext* AddCreature(const Vec2& source, PixelDescriptor* dest, Morphing& morphing, float duration);
      ObjectContext* CreateCell(PixelDescriptor* dest);
      ObjectContext* CreatePolymorphCell(PixelDescriptor* dest);
      void Delete(ObjectContext* context);
      void Move(const Vec2& source, const Vec2& dest, ObjectContext* context, float duration, int steps, Morphing& morphing, CellDescriptor* cd);
      void Attack(ObjectContext* context, const Vec2& pos, const Vec2& offset, float animationDuration);
      
      inline bool IsInAABB(const Vec2& vec);
      inline bool IsInAABB(const int& x, const int& y);
      inline Vec2 LocalVector(const komorki::Vec2& input) const;

      cocos2d::Sprite* m_lightOverlay;
      std::shared_ptr<DeadCellsLayer> m_background;
      cocos2d::Sprite* m_terrainSprite;
      cocos2d::Sprite* m_terrainBgSprite;
      IPixelDescriptorProvider* m_provider;
      std::list<ObjectContext*> m_upcomingContexts;
      std::list<CellDescriptor*> m_outgoingCells;
      bool m_enableSmallAnimations = false;
    };
  }
}

