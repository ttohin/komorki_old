//
//  Viewport.h
//  prsv
//
//  Created by ttohin on 07.06.15.
//
//

#ifndef __prsv__Viewport__
#define __prsv__Viewport__

#include <memory>
#include "cocos2d.h"
#include "Common.h"
#include "PixelWorld.h"
#include "Statistic.hpp"
#include "PartialMapsManager.h"

namespace komorki
{
  class AsyncPixelWorld;
  
  namespace graphic
  {
    class PartialMap;
    
    class Viewport
    {
    public:
     
      typedef std::shared_ptr<Viewport> Ptr;
      
      void Move(const cocos2d::Vec2& ofset);
      void Zoom(const cocos2d::Vec2& point, float scaleOffset);
      void Calculate();
      
      Viewport(cocos2d::Node* superView,
               const cocos2d::Size& originalSize,
               const std::shared_ptr<komorki::IPixelWorld>& provider);
      
      ~Viewport();
      void Test();
      void CreateMap();
      void Resize(const cocos2d::Size& originalSize);
      
      void Update(float updateTime, float& outUpdateTime);
      void UpdateAsync(float& updateTime);
      bool IsAvailable();
      cocos2d::Node* GetRootNode() const;
      cocos2d::Node* GetMainNode() const;
      cocos2d::Node* GetLightNode() const;
      
    private:
      
      typedef std::shared_ptr<PartialMap> PartialMapPtr;
      typedef std::vector<PartialMapPtr> MapList;
     
      void HealthCheck();
      
      void PerformMove(PartialMapsManager::CreateMapArgs& newMapsArgs,
                       PartialMapsManager::RemoveMapArgs& mapsToRemove);
      void CreateMap(const cocos2d::Rect& viewSize, float scale);
      void CreatePixelMaps(const Rect& rect, const cocos2d::Vec2& offset, float scale);
      
      Rect PixelRect(const cocos2d::Rect& rect, float scale) const;
      Rect PixelRectInner(const cocos2d::Rect& rect, float scale) const;
      cocos2d::Rect CocosRect(const Rect& rect, float scale) const;
      bool RemoveMapsOutsideOfRect(const Rect& rect, const Maps& currentMaps, PartialMapsManager::RemoveMapArgs& mapsToRemove);
      bool SplitRectOnChunks(const Rect& rect, const Rect& existingRect, std::vector<Rect>& result) const;
      bool CreatePartialMapsInRects(const std::vector<Rect>& rects,
                                    PartialMapsManager::CreateMapArgs& newMapsArgs);
      Rect GetUpcommingRect() const;
      cocos2d::Rect GetCurrentGraphicRect() const;
      komorki::Rect GetLoadedPixelRect() const;
      
      Rect m_prevPos;
      Rect m_pos;
      Rect m_visibleRect;
      float m_scale;
      cocos2d::Rect m_pixelWorldPos;
      cocos2d::Size m_originalSize;
      cocos2d::Vec2 m_superViewOffset;
      
      int m_mapSegmentSize;
      
      cocos2d::Node* m_mainView;
      cocos2d::Node* m_superView;
      cocos2d::Node* m_lightNode;
      
      bool m_performMove;
      float m_initialScale;
      bool m_enableSmallAnimations;
      bool m_enableAnimations;
     
      std::shared_ptr<IPixelWorld> m_provider;
      std::shared_ptr<AsyncPixelWorld> m_manager;
      unsigned char m_lastUpdateId;
      StatisticCounter<double> m_updateTime;
      StatisticCounter<double> m_mapsUpdateTime;
      StatisticCounter<int> m_numberOfUpdates;
      PartialMapsManager m_mapManager;
      
      // offset from bottom left corner of world to SuperView corener
      cocos2d::Vec2 tt_globalGraphicalOffset;
      float tt_scale;
      Rect tt_loadedPixelRect;
      cocos2d::Size tt_viewSize;
    };
  }
}

#endif /* defined(__prsv__Viewport__) */
