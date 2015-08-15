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
#include "Common.h"
#include "IPixelDescriptorProvider.h"
#include "cocos2d.h"

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
      
      struct Context
      {
        PartialMap* owner;
        cocos2d::Sprite* sprite;
        cocos2d::Sprite* glow;
        cocos2d::Vec2 offset;
        Vec2 pos;
        
        void BecomeOwner(PartialMap* _owner);
        void Free(PartialMap* _owner);
        void Destory(PartialMap* _owner);
       
        Context(PartialMap *_owner);
       
        std::string Description() const;
        
        
      private:
        ~Context();
      };
      
      PartialMap();
      virtual ~PartialMap();
      
      bool Init(int a, int b, int width, int height,
                PixelDescriptorProvider* provider, cocos2d::Node* superView, const cocos2d::Vec2& offset);
      
      void Move(const Vec2& newPos);
      
      void AdoptIncomingItems();
      void DeleteOutgoingItems();
      void HandleItemsOnBounds(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime);
      void Update(const std::list<IPixelDescriptorProvider::UpdateResult>& updateResult, float updateTime);
      
      void HightlightCellOnPos(int x, int y, komorki::CellType type);
      void StopHightlighting();
      
      void Reset();
      
      void StopSmallAnimations();
      void StartSmallAnimations();
      
      void Transfrorm(const cocos2d::Vec2& pos, float scale);
      void ChangeAABB(int a, int b, int width, int height);
      
      std::string Description();
      
      int m_a1;
      int m_b1;
      int m_width;
      int m_height;
      int m_a2;
      int m_b2;
      
    private:
   
      void InitPixel(PixelDescriptor* pd);
      PartialMap::Context* AddCreature(const Vec2& source, PixelDescriptor* dest);
      PartialMap::Context* CreateCell(PixelDescriptor* dest);
      void Delete(Context* context);
      void Move(const Vec2& source, const Vec2& dest, Context* context);
      void Attack(Context* context, const Vec2& pos, const Vec2& offset);
      
      inline bool IsInAABB(const Vec2& vec);
      inline bool IsInAABB(const int& x, const int& y);
      inline Vec2 LocalVector(const komorki::Vec2& input) const;
      

      std::shared_ptr<PixelMapPartial> m_cellMap;
      std::shared_ptr<PixelDebugView> m_debugView;
      std::shared_ptr<PixelMapLightOverlay> m_lightOverlay;
      std::shared_ptr<PixelMapBackground> m_background;
      std::shared_ptr<GlowMapOverlay> m_glow;
      PixelDescriptorProvider* m_provider;
      std::list<Context*> m_upcomingContexts;
      std::list<CellDescriptor*> m_outgoingCells;
      
    };
  }
}

#endif /* defined(__prsv__PartialMap__) */
