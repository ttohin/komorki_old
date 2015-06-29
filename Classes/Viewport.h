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

namespace komorki
{
  class AsyncPixelManager;
  
  namespace ui
  {
    class PartialMap;
    
    class Viewport
    {
    public:
     
      void Move(const cocos2d::Vec2& ofset);
      void Zoom(const cocos2d::Vec2& point, float scaleOffset);
      void Calculate();
      void PerformMove();
      
      Viewport(cocos2d::Node* superView, PixelDescriptorProvider::Config* config, const cocos2d::Size& originalSize);
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
      
    private:
      
      struct Rect
      {
        Vec2 origin;
        Vec2 size;
        Rect Extract(const Rect& rect) const;
        bool In(const Rect& rect) const;
        
        bool operator==(const Rect& rect) const;
        bool operator!=(const Rect& rect) const;
      };
      
      void CreateMap(const cocos2d::Rect& viewSize, float scale);
      void CreatePixelMaps(const Rect& rect, const cocos2d::Vec2& offset, float scale);
      
      Rect PixelRect(const cocos2d::Rect& rect);
      Rect PixelRectInner(const cocos2d::Rect& rect);
      cocos2d::Rect CocosRect(const Rect& rect);
      
      Rect m_prevPos;
      Rect m_pos;
      Rect m_visibleRect;
      cocos2d::Rect m_pixelWorldPos;
      cocos2d::Size m_originalSize;
      
      int m_mapSegmentSize;
      
      cocos2d::Node* m_superView;
      
      bool m_performMove;
      float m_initialScale;
      
      std::shared_ptr<PixelDescriptorProvider> m_provider;
      std::shared_ptr<AsyncPixelManager> m_manager;
      std::vector<std::shared_ptr<PartialMap> > m_maps;
      unsigned char m_lastUpdateId;
      
    };
  }
}

#endif /* defined(__prsv__Viewport__) */
