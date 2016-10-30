//
//  PartialMap.h
//  prsv
//
//  Created by ttohin on 06.06.15.
//
//

#ifndef __prsv__PartialMap__
#define __prsv__PartialMap__

#include <memory>
#include <unordered_map>
#include <list>
#include "Common.h"
#include "IPixelDescriptorProvider.h"
#include "cocos2d.h"
#include "TerrainBatchSprite.h"
#include "TerrainSprite.h"
#include "PixelMapContext.h"

namespace komorki
{
  class PixelDescriptorProvider;
  
  namespace ui
  {
    class PixelMapPartial;
    class PixelDebugView;
    class PixelMapLightOverlay;
    class PixelMapBackground;
    class GlowMapOverlay;
    
    class PartialMap
    {
    public:
      
      PartialMap();
      virtual ~PartialMap();
      
      bool Init(int a, int b, int width, int height,
                IPixelDescriptorProvider* provider,
                cocos2d::Node* superView,
                cocos2d::Node* lightNode,
                const cocos2d::Vec2& offset);
      
      void Move(const Vec2& newPos);
      
      void AdoptIncomingItems();
      void DeleteOutgoingItems();
      void HandleItemsOnBounds(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime);
      void Update(std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime);
      
      void HightlightCellOnPos(int x, int y, komorki::CellType type);
      void StopHightlighting();
      
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
      std::shared_ptr<PixelMapPartial> m_cellMap;
      bool m_enableAnimations = true;
      
    private:
   
      void InitPixel(PixelDescriptor* pd);
      PixelMap::ObjectContext* AddCreature(const Vec2& source, PixelDescriptor* dest, Morphing& morphing, float duration);
      PixelMap::ObjectContext* CreateCell(PixelDescriptor* dest);
      PixelMap::ObjectContext* CreatePolymorphCell(PixelDescriptor* dest);
      void Delete(PixelMap::ObjectContext* context);
      void Move(const Vec2& source, const Vec2& dest, PixelMap::ObjectContext* context, float duration, int steps, Morphing& morphing, CellDescriptor* cd);
      void Attack(PixelMap::ObjectContext* context, const Vec2& pos, const Vec2& offset, float animationDuration);
      
      inline bool IsInAABB(const Vec2& vec);
      inline bool IsInAABB(const int& x, const int& y);
      inline Vec2 LocalVector(const komorki::Vec2& input) const;

      std::shared_ptr<PixelDebugView> m_debugView;
      cocos2d::Sprite* m_lightOverlay;
      std::shared_ptr<PixelMapBackground> m_background;
      std::shared_ptr<GlowMapOverlay> m_glow;
      cocos2d::Sprite* m_terrainSprite;
      cocos2d::Sprite* m_terrainBgSprite;
      IPixelDescriptorProvider* m_provider;
      std::list<PixelMap::ObjectContext*> m_upcomingContexts;
      std::list<CellDescriptor*> m_outgoingCells;
      bool m_enableSmallAnimations = false;
    };
  }
}

#endif /* defined(__prsv__PartialMap__) */
