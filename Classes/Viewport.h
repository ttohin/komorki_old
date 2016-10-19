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
#include "PixelDescriptorProvider.h"
#include "Statistic.hpp"

namespace komorki
{
  class AsyncPixelManager;
  
  namespace ui
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
               const std::shared_ptr<komorki::IPixelDescriptorProvider>& provider);
      
      ~Viewport();
      void Test();
      void CreateMap();
      void Resize(const cocos2d::Size& originalSize);
      cocos2d::Size GetTotalMapSize() const;
      bool HightlightCellOnCursorPos(cocos2d::Vec2 cursorPosition, komorki::CellType type);
      void StopHightlighting();
      bool AddCreatureAtPosition(cocos2d::Vec2 cursorPosition, komorki::CellType type);
      void RemoveCreatureAtPostion(cocos2d::Vec2 cursorPosition);
      void CleanMap();
      void Reset();
      
      void Update(float updateTime, float& outUpdateTime);
      void UpdateAsync(float& updateTime);
      void UpdateWarp(float& updateTime, unsigned int numberOfUpdates);
      bool IsAvailable();
      cocos2d::Node* GetRootNode() const;
      cocos2d::Node* GetMainNode() const;
      cocos2d::Node* GetLightNode() const;
      
    private:
      
      typedef std::shared_ptr<PartialMap> PartialMapPtr;
      typedef std::vector<PartialMapPtr> MapList;
     
      void HealthCheck();
      
      void PerformMove(MapList& mapsToCreate, MapList& mapsToRemove);
      void CreateMap(const cocos2d::Rect& viewSize, float scale);
      void CreatePixelMaps(const Rect& rect, const cocos2d::Vec2& offset, float scale);
      
      Rect PixelRect(const cocos2d::Rect& rect, float scale) const;
      Rect PixelRectInner(const cocos2d::Rect& rect, float scale) const;
      cocos2d::Rect CocosRect(const Rect& rect, float scale) const;
      bool RemoveMapsOutsideOfRect(const Rect& rect, MapList& toRemove);
      bool MoveMaps(const Vec2& offset, const cocos2d::Vec2& pointOffset, float scale);
      bool SplitRectOnChunks(const Rect& rect, const Rect& existingRect, std::vector<Rect>& result) const;
      bool CreatePartialMapsInRects(const std::vector<Rect>& rects,
                                    const Vec2& pixelOffset,
                                    const cocos2d::Vec2& offset,
                                    float scale,
                                    MapList& maps);
      Rect GetUpcommingRect() const;
      Rect GetCurrentVisibleRect() const;
      
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
     
      std::shared_ptr<IPixelDescriptorProvider> m_provider;
      std::shared_ptr<AsyncPixelManager> m_manager;
      MapList m_maps;
      MapList m_mapsToRemove;
      MapList m_mapsToCreate;
      unsigned char m_lastUpdateId;
      StatisticCounter<double> m_updateTime;
      StatisticCounter<double> m_mapsUpdateTime;
      StatisticCounter<int> m_numberOfUpdates;
    };
  }
}

#endif /* defined(__prsv__Viewport__) */
