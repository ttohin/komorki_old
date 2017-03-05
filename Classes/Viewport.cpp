//
//  Viewport.cpp
//  prsv
//
//  Created by ttohin on 07.06.15.
//
//

#include "Viewport.h"
#include "PartialMap.h"
#include "AsyncPixelWorld.h"
#include "UIConfig.h"
#include "Random.h"
#include "Logging.h"
#include "CellDescriptor.h"

namespace komorki
{
  namespace graphic
  {
    PixelPos Ceil(const PixelPos& value, const PixelPos& precision)
    {
      double scaledValue = (double) value / precision;
      return std::ceil(scaledValue) * precision;
    }
    
    Rect ResizeByStep(const Rect& original, const Rect& destination, const PixelPos& step)
    {
      Rect result = original;
      if (original.Top() > destination.Top())
      {
        PixelPos offset = original.Top() - destination.Top();
        result.size.y -= Ceil(offset, step);
      }
      if (original.Bottom() < destination.Bottom())
      {
        PixelPos offset = destination.Bottom() - original.Bottom();
        result.MoveBotton(Ceil(offset, step));
      }
      if (original.Left() < destination.Left())
      {
        PixelPos offset = destination.Left() - original.Left();
        result.MoveLeft(Ceil(offset, step));
      }
      if (original.Right() > destination.Right())
      {
        PixelPos offset = original.Right() - destination.Right();
        result.size.x -= Ceil(offset, step);
      }
      return result;
    }
    
    Rect ExtendRectWithStep(const Rect& original, const Rect& destination, const PixelPos& step)
    {
      Rect result = original;
      if (original.Top() < destination.Top())
      {
        PixelPos offset = destination.Top() - original.Top();
        result.size.y += Ceil(offset, step);
      }
      if (original.Bottom() > destination.Bottom())
      {
        PixelPos offset = original.Bottom() - destination.Bottom();
        result.MoveBotton(-Ceil(offset, step));
      }
      if (original.Left() > destination.Left())
      {
        PixelPos offset = original.Left() - destination.Left();
        result.MoveLeft(-Ceil(offset, step));
      }
      if (original.Right() < destination.Right())
      {
        PixelPos offset = destination.Right() - original.Right();
        result.size.x += Ceil(offset, step);
      }
      return result;
    }
    
    void Viewport::Move(const cocos2d::Vec2& offset)
    {
      cocos2d::Vec2 newpos = m_superView->getPosition() + offset;
      m_superView->setPosition(newpos);
    }
    
    void Zoom(const cocos2d::Vec2& base,
              const cocos2d::Vec2& point,
              const float& scale,
              const float& scaleOffset,
              cocos2d::Vec2& outBase,
              float& outScale)
    {
      float newScale = scale - scaleOffset;
      float scaleRatio = newScale/scale;
      
      outScale = newScale;
      
      outBase.x = point.x  - scaleRatio * (point.x -  base.x);
      outBase.y = point.y  - scaleRatio * (point.y - base.y);
    }
    
    void Viewport::Zoom(const cocos2d::Vec2& point, float scaleOffset)
    {
      cocos2d::Vec2 newSuperViewPos;
      float newSuperViewScale;
      graphic::Zoom(m_superView->getPosition(), point, m_superView->getScale(), scaleOffset, newSuperViewPos, newSuperViewScale);
      m_superView->setPosition(newSuperViewPos);
      m_superView->setScale(newSuperViewScale);
    }
    
    void Viewport::Calculate()
    {
      float pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
      float screenScale = 1.0 / m_superView->getScale();
      
      cocos2d::Rect rect;
      rect.size = m_pixelWorldPos.size * screenScale;
      rect.origin = m_pixelWorldPos.origin - m_superView->getPosition() * screenScale / pixelsScale;
      
      Rect pixelRect = PixelRectInner(rect, m_initialScale);
      Rect innerPrevRect = ResizeByStep(m_pos, pixelRect, kSegmentSize);
      Rect extendedRect = ExtendRectWithStep(innerPrevRect, pixelRect, kSegmentSize);
      Vec2 size = m_provider->GetSize();
      extendedRect = extendedRect.Extract({Vec2(0, 0), size});
      
      if ( extendedRect != m_pos )
      {
        m_performMove = true;
      }
    }
    
    Viewport::Viewport(cocos2d::Node* superView,
                           const cocos2d::Size& originalSize,
                           const std::shared_ptr<IPixelWorld>& provider)
    
    {
      assert(superView);
      
      m_originalSize = originalSize;
      m_superView = superView;
      m_lightNode = cocos2d::Node::create();
      m_mainView = cocos2d::Node::create();
      m_superView->addChild(m_lightNode);
      m_superView->addChild(m_mainView);
      m_performMove = false;
      
      m_initialScale = 0.2;
      m_enableSmallAnimations = true;
      m_enableAnimations = true;
      
      m_pixelWorldPos.origin = {0, 0};
      m_pixelWorldPos.size = originalSize;
      
      m_provider = provider;
      
      m_manager = std::make_shared<AsyncPixelWorld>(m_provider.get());
      m_lastUpdateId = 0;
      
      m_mapManager.m_mainNode = m_mainView;
      m_mapManager.m_lightNode = m_lightNode;
      m_mapManager.m_provider = m_provider;
      m_mainView->setName("SuperView");
      
      CreateMap();
      
      Test();
    }
    
    Viewport::~Viewport()
    {
      m_manager->Stop();
    }
    
    void Viewport::Test()
    {
      {
        Rect r1 = {{0,0}, {60,70}};
        Rect existingRect = {{0,0}, {50,50}};
        Rect expected2 = {{50,0}, {10,50}};
        Rect expected3 = {{0,50}, {50,20}};
        Rect expected4 = {{50,50}, {10,20}};
        
        std::vector<Rect> res;
        assert(komorki::SplitRectOnChunks(r1, existingRect, 50, res));
        assert(res.size() == 3);
        assert(res.end() != std::find(res.begin(), res.end(), expected2));
        assert(res.end() != std::find(res.begin(), res.end(), expected3));
        assert(res.end() != std::find(res.begin(), res.end(), expected4));
        
      }
      
      {
        Rect r1 = {{0,0}, {100,100}};
        Rect r2 = {{-10,10}, {120,60}};
        Rect expected = {{0,20}, {100,40}};
        Rect r = ResizeByStep(r1, r2, 20);
        assert(r == expected);
      }
      
      {
        Rect r1 = {{2,2}, {2,2}};
        Rect r2 = {{0,0}, {10,10}};
        Rect expected = {{2,2}, {2,2}};
        Rect r = ResizeByStep(r1, r2, 10);
        assert(r == expected);
      }
      
      {
        Rect r1 = {{0,0}, {30,30}};
        Rect r2 = {{-5,-15}, {40,50}};
        Rect expected = {{-10,-20}, {50,60}};
        Rect r = ExtendRectWithStep(r1, r2, 10);
        assert(r == expected);
      }
      
      {
        float f1 = 10.1;
        int r1 = std::ceil(f1);
        assert(r1 == 11);
      }
      {
        float f1 = 10.1;
        int r1 = std::floor(f1);
        assert(r1 == 10);
      }
      
      {
        float f1 = 10.9;
        int r1 = std::ceil(f1);
        assert(r1 == 11);
      }
      {
        float f1 = 10.9;
        int r1 = std::floor(f1);
        assert(r1 == 10);
      }
      
      {
        Rect r1 = {{0,0}, {10,10}};
        Rect r2 = {{0,0}, {20,20}};
        assert(r1.In(r2));
      }
      
      {
        Rect r1 = {{0,0}, {10,10}};
        Rect r2 = {{-4,-2}, {20,20}};
        assert(r1.In(r2));
      }
      
      {
        Rect r1 = {{0,0}, {10,10}};
        Rect r2 = {{0,0}, {20,20}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{0,0}, {10,10}}));
      }
      {
        Rect r1 = {{-1,-1}, {10,10}};
        Rect r2 = {{0,0}, {20,20}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{0,0}, {9,9}}));
      }
      {
        Rect r1 = {{10,10}, {10,10}};
        Rect r2 = {{0,0}, {20,20}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{10,10}, {10,10}}));
      }
      {
        Rect r1 = {{15,15}, {10,10}};
        Rect r2 = {{0,0}, {20,20}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{15,15}, {5,5}}));
      }
      {
        Rect r1 = {{0,0}, {10,10}};
        Rect r2 = {{5,5}, {2,2}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{5,5}, {2,2}}));
      }
      {
        Rect r1 = {{-10,0}, {10,10}};
        Rect r2 = {{0,0}, {15,15}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{0,0}, {0,0}}));
      }
      {
        Rect r1 = {{-10,0}, {12,12}};
        Rect r2 = {{0,0}, {15,15}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{0,0}, {2,12}}));
      }
      {
        Rect r1 = {{10,10}, {150,150}};
        Rect r2 = {{0,0}, {300,200}};
        Rect re = r1.Extract(r2);
        assert(re == Rect({{10,10}, {150,150}}));
      }
      
      {
        cocos2d::Rect r1 = {0, 0, 320, 320};
        Rect r2 = this->PixelRect(r1, 0.1);
        assert(r2 == Rect({{0,0}, {100, 100}}));
      }
      {
        cocos2d::Rect r1 = {0, 0, 320.2, 320.2};
        Rect r2 = this->PixelRect(r1, 0.1);
        assert(r2 == Rect({{0,0}, {101, 101}}));
      }
      
      {
        cocos2d::Rect r1 = {0, 0, 320, 320};
        cocos2d::Rect r2 = {32, 0, 288, 320};
        cocos2d::Rect r3 = {32.01, 0, 288, 320};
        Rect r1p = this->PixelRect(r1, 0.1);
        Rect r2p = this->PixelRectInner(r2, 0.1);
        Rect r3p = this->PixelRectInner(r3, 0.1);
        assert(r2p.In(r1p));
        assert(!r3p.In(r1p));
      }
      {
        cocos2d::Rect r2 = {0, 32, 320, 288};
        cocos2d::Rect r3 = {0, 32.01, 320, 288};
        Rect r1p = {{0, 0}, {100, 100}};
        Rect r2p = this->PixelRectInner(r2, 0.1);
        Rect r3p = this->PixelRectInner(r3, 0.1);
        assert(r2p.In(r1p));
        assert(!r3p.In(r1p));
      }
      {
        Vec2 a(100, 100);
        assert(a.Normalize() == Vec2(1,1));
        Vec2 b(100, 99);
        assert(b.Normalize() == Vec2(1,1));
        Vec2 c(100, 50);
        assert(c.Normalize() == Vec2(1,1));
        Vec2 d(100, 49);
        assert(d.Normalize() == Vec2(1,0));
        Vec2 e(100, 1);
        assert(e.Normalize() == Vec2(1,0));
      }
      {
        Vec2 e(-100, 1);
        assert(e.Normalize() == Vec2(-1,0));
      }
      {
        Vec2 e(-100, -1);
        assert(e.Normalize() == Vec2(-1,0));
      }
      {
        Vec2 e(-100, 50);
        assert(e.Normalize() == Vec2(-1,1));
      }
    }
    
    void Viewport::CreateMap()
    {
      Vec2 size = m_provider->GetSize();
      for (int i = 0; i < size.x; ++i)
      {
        for (int j = 0; j < size.y; ++j)
        {
          auto pd = m_provider->GetDescriptor(i, j);
          if (pd->m_cellDescriptor)
          {
            assert(pd->m_cellDescriptor->userData == nullptr);
          }
        }
      }
      
      float pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
      float screenScale = 1.0 / m_superView->getScale();
      
      // map size in points that visible in viewport
      m_pixelWorldPos.size = (m_pixelWorldPos.size + cocos2d::Size(kViewportMargin, kViewportMargin)) * screenScale;
      m_pixelWorldPos.origin += -(m_superView->getPosition() - m_superViewOffset) * screenScale / pixelsScale;
      
      auto observableRect = cocos2d::Rect(m_pixelWorldPos.origin.x,
                                          m_pixelWorldPos.origin.y,
                                          m_pixelWorldPos.size.width,
                                          m_pixelWorldPos.size.height);
      
      // visible map in pixels
      Rect rect = PixelRectInner(observableRect, m_initialScale);
      
      Rect pixelMapRect = rect;
      pixelMapRect.size.x = Ceil(pixelMapRect.size.x, kSegmentSize);
      pixelMapRect.size.y = Ceil(pixelMapRect.size.y, kSegmentSize);
      pixelMapRect = pixelMapRect.Extract({Vec2(0, 0), size});
      rect = rect.Extract({Vec2(0, 0), size});
      
      pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
      
      float pixelSize = kSpritePosition * m_initialScale * pixelsScale;
      
      Vec2 offsetInPixels = m_visibleRect.origin - rect.origin;
      cocos2d::Vec2 offset = m_superView->getPosition() - cocos2d::Vec2(offsetInPixels.x * pixelSize, offsetInPixels.y * pixelSize);
      
      m_superView->setPosition(offset);
      m_superViewOffset = offset;
      m_superView->setScale(1.f);
      
      std::vector<Rect> mapRects;
      bool res = SplitRectOnChunks(pixelMapRect, {{0, 0}, {0, 0}}, mapRects);
      assert(res);
      
      PartialMapsManager::CreateMapArgs args;
      res = CreatePartialMapsInRects(mapRects,
                                     -pixelMapRect.origin,
                                     cocos2d::Vec2::ZERO,
                                     pixelsScale,
                                     args);
      for (auto& arg : args)
      {
        m_mapManager.CreateMap(arg);
      }
      assert(res);
      
      m_prevPos = pixelMapRect;
      m_pos = pixelMapRect;
      m_scale = pixelsScale;
      m_visibleRect = rect;
    }
    
    void Viewport::CreatePixelMaps(const Rect& rect, const cocos2d::Vec2& offset, float scale)
    {
      if (rect.size.x < 0 || rect.size.y < 0)
      {
        assert(0);
      }
      
      if (rect.size.x == 0 || rect.size.y == 0)
      {
        assert(0);
        return;
      }
      
      m_mapSegmentSize = MIN(kSegmentSize, rect.size.x);
      m_mapSegmentSize = MIN(m_mapSegmentSize, rect.size.y);
      
      int stepsX = rect.size.x/m_mapSegmentSize + 1;
      int stepsY = rect.size.y/m_mapSegmentSize + 1;
      
      for (int i = 0; i < stepsX; ++i)
      {
        for (int j = 0; j < stepsY; ++j)
        {
          int width = MIN(m_mapSegmentSize, rect.size.x - i*m_mapSegmentSize);
          int height = MIN(m_mapSegmentSize, rect.size.y - j*m_mapSegmentSize);
          
          if (width <= 0 || height <= 0)
          {
            continue;
          }
          
//          auto map = std::make_shared<graphic::PartialMap>();
          
          Rect rectForMap = Rect(rect.origin.x + i*m_mapSegmentSize,
                                 rect.origin.y +j*m_mapSegmentSize,
                                 width,
                                 height);
          
//          map->InitF(rect.origin.x + i*m_mapSegmentSize,
//                    rect.origin.y +j*m_mapSegmentSize,
//                    width,
//                    height,
//                    m_provider.get(),
//                    m_mainView,
//                    m_lightNode,
//                    cocos2d::Vec2::ZERO);
//          map->Transfrorm(offset + cocos2d::Vec2(i*m_mapSegmentSize * kSpritePosition * m_initialScale * scale,
//                                                 j*m_mapSegmentSize * kSpritePosition * m_initialScale * scale),
//                          m_initialScale * scale);
//          m_maps.push_back(map);
          
          PartialMapsManager::CreateMapArg createMapArg;
          createMapArg.animated = m_enableAnimations;
          createMapArg.enableSmallAnimations = m_enableSmallAnimations;
          createMapArg.rect = rectForMap;
          createMapArg.graphicPos = offset + cocos2d::Vec2(i*m_mapSegmentSize * kSpritePosition * m_initialScale * scale,
                                                           j*m_mapSegmentSize * kSpritePosition * m_initialScale * scale);
          createMapArg.scale = scale;
          
          m_mapManager.CreateMap(createMapArg);
          
        }
      }
    }
    
    void Viewport::Resize(const cocos2d::Size& size)
    {
      float resizeRatio =  m_originalSize.width / size.width;
      m_superView->setScale(resizeRatio);
      
      m_originalSize = size;
      m_performMove = true;
    }
    
    void Viewport::UpdateAsync(float& updateTime)
    {
      if ( false == m_manager->IsAvailable() )
      {
        return;
      }
      
      assert(m_lastUpdateId >= m_manager->GetUpdateId());
      if ( m_lastUpdateId != m_manager->GetUpdateId() )
      {
        return;
      }
      
      float mapsUpdateTime = 0.0f;
      Update(updateTime, mapsUpdateTime);
      
      if (mapsUpdateTime > updateTime)
      {
        updateTime = mapsUpdateTime;
      }
      
      m_lastUpdateId++;
      
      m_manager->StartUpdate(1);
    }
    
    bool Viewport::IsAvailable()
    {
      return m_manager->IsAvailable();
    }
    
    cocos2d::Node* Viewport::GetRootNode() const
    {
      return m_superView;
    }
    
    cocos2d::Node* Viewport::GetMainNode() const
    {
      return m_mainView;
    }
    
    cocos2d::Node* Viewport::GetLightNode() const
    {
      return m_lightNode;
      
    }
    
    void Viewport::Update(float updateTime, float& outUpdateTime)
    {
      assert(m_lastUpdateId == m_manager->GetUpdateId());
      
      
      outUpdateTime = 0;
      
      struct timeval tv;
      struct timeval start_tv;
      
      gettimeofday(&start_tv, NULL);
      
      double elapsed = 0.0;
      
      WorldUpdateResult& worldUpdateResult = m_manager->GetUpdateResult();
      
      LOG_W("Update %d. updates: %lu", m_lastUpdateId, worldUpdateResult.list.size());
      
      
#ifdef LOG_UPDATES
      for (const auto& u : worldUpdateResult)
      {
        std::string operationType;
        
        GreatPixel* descriptor = static_cast<GreatPixel*>(u.desc);
        
        Vec2 destinationPos(0,0);
        Vec2 initialPos = Vec2(u.desc->x, u.desc->y);
        
        int id = descriptor->m_cellDescriptor ? descriptor->m_cellDescriptor->m_id : 0;
        
        auto m = u.movement;
        
        if(u.movement == true)
          operationType = "movement";
        else if(u.action == true)
          operationType = "action";
        else if (u.addCreature == true)
          operationType = "addCreature";
        else if (u.deleteCreature == true)
          operationType = "delete";
        else if (u.morph== true)
          operationType = "morph";
        
        if(u.addCreature == true)
          destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
        
        if (m == true)
          destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
        
        LOG_W("%s [%s]->[%s] [id:%d] t:%d",
              operationType.c_str(), initialPos.Description().c_str(), destinationPos.Description().c_str(), id, descriptor->m_type);
      }
#endif
      
      if (m_performMove)
      {
        Rect upcommingRect = GetCurrentVisibleRect();
        float upcommingRectSquare = upcommingRect.size.x * upcommingRect.size.y;
        if (upcommingRectSquare > 10 * 50 * 50)
        {
          m_enableSmallAnimations = false;
        }
        else
        {
          m_enableSmallAnimations = true;
        }
        
        if (upcommingRectSquare > 20 * 50 * 50)
        {
          m_enableAnimations = false;
        }
        else
        {
          m_enableAnimations = true;
        }
        
      }
    
      PartialMapsManager::RemoveMapArgs mapsToRemove;
      PartialMapsManager::CreateMapArgs newMaps;
      
      if (m_performMove)
      {
        m_performMove = false;
        PerformMove(newMaps, mapsToRemove);
      }
      
      m_mapManager.m_pos = m_pos;
      m_mapManager.Update(newMaps, mapsToRemove, worldUpdateResult, updateTime);
      
      gettimeofday(&tv, NULL);
      elapsed = (tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
      
      outUpdateTime = m_manager->GetLastUpdateTime() + elapsed;
      
      if (m_lastUpdateId >= 3)
      {
        m_mapsUpdateTime.AddValue(elapsed);
        m_numberOfUpdates.AddValue(worldUpdateResult.list.size());
        m_updateTime.AddValue(m_manager->GetLastUpdateTime());
      }
      
      if (m_numberOfUpdates.number % 100 == 0 && m_numberOfUpdates.number != 0)
      {
        cocos2d::log("Update time: %s", m_updateTime.ToString().c_str());
        cocos2d::log("Map update: %s", m_mapsUpdateTime.ToString().c_str());
        cocos2d::log("Number of diff: %s", m_numberOfUpdates.ToString().c_str());
      }
      
      HealthCheck();
    }
    
    void Viewport::HealthCheck()
    {
      if (kRedrawEachUpdate) return;
      
      Vec2 size = m_provider->GetSize();
      for (int i = 0; i < size.x; ++i)
      {
        for (int j = 0; j < size.y; ++j)
        {
          auto pd = m_provider->GetDescriptor(i, j);
          if (pd->m_cellDescriptor && pd->m_cellDescriptor->parent == pd)
          {
            Vec2 point(i, j);
            if (point.In(m_pos))
            {
              assert(pd->m_cellDescriptor->userData);
            }
            else
            {
              assert(!pd->m_cellDescriptor->userData);
            }
          }
        }
      }
    }
    
    void Viewport::PerformMove(PartialMapsManager::CreateMapArgs& newMapsArgs,
                               PartialMapsManager::RemoveMapArgs& mapsToRemove)
    {
      float pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
      float screenScale = 1.0 / m_superView->getScale();
      
      m_pixelWorldPos.size = m_pixelWorldPos.size * screenScale;
      m_pixelWorldPos.origin += -(m_superView->getPosition() - m_superViewOffset) * screenScale / pixelsScale;
      
      auto observableRect = cocos2d::Rect(m_pixelWorldPos.origin.x,
                                          m_pixelWorldPos.origin.y,
                                          m_pixelWorldPos.size.width,
                                          m_pixelWorldPos.size.height);
      
      Rect rect = PixelRectInner(observableRect, m_initialScale);
      Rect innerPrevRect = ResizeByStep(m_pos, rect, kSegmentSize);
      Rect extendedRect = ExtendRectWithStep(innerPrevRect, rect, kSegmentSize);
      Rect reusedRect = m_pos.Extract(extendedRect);
      
      Vec2 size = m_provider->GetSize();
      extendedRect = extendedRect.Extract({Vec2(0, 0), size});
      
      assert(reusedRect.size.x % kSegmentSize == 0);
      assert(reusedRect.size.y % kSegmentSize == 0);
      assert(reusedRect.origin.x % kSegmentSize == 0);
      assert(reusedRect.origin.y % kSegmentSize == 0);
      
      pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
      
      float pixelSize = kSpritePosition * m_initialScale * pixelsScale;
      
      Vec2 offsetInPixels = m_visibleRect.origin - rect.origin;
      Vec2 offsetFromRect = rect.origin - extendedRect.origin;
      cocos2d::Vec2 offset = m_superView->getPosition() - cocos2d::Vec2(offsetInPixels.x * pixelSize, offsetInPixels.y * pixelSize);
      
      m_superView->setPosition(offset);
      m_superViewOffset = offset;
      m_superView->setScale(1.f);
      
      auto currentMaps = m_mapManager.GetMaps();
      bool res = RemoveMapsOutsideOfRect(extendedRect, currentMaps, mapsToRemove);
      assert(res);
      res = MoveMaps(-extendedRect.origin, -cocos2d::Vec2(offsetFromRect.x, offsetFromRect.y) * pixelSize, pixelsScale, currentMaps);
      assert(res);
      
      std::vector<Rect> mapRects;
      res = SplitRectOnChunks(extendedRect, reusedRect, mapRects);
      assert(res);
      
      res = CreatePartialMapsInRects(mapRects,
                                     -extendedRect.origin,
                                     -cocos2d::Vec2(offsetFromRect.x, offsetFromRect.y) * pixelSize,
                                     pixelsScale,
                                     newMapsArgs);

      assert(res);
      
      m_prevPos = extendedRect;
      m_pos = extendedRect;
      m_visibleRect = rect;
    }
    
    Rect Viewport::PixelRect(const cocos2d::Rect& rect, float scale) const
    {
      Rect result;
      
      result.origin.x = std::floor(rect.origin.x/(kSpritePosition * scale));
      result.size.x = std::ceil(rect.size.width/(kSpritePosition * scale));
      result.origin.y = std::floor(rect.origin.y/(kSpritePosition * scale));
      result.size.y = std::ceil(rect.size.height/(kSpritePosition * scale));
      
      return result;
    }
    
    Rect Viewport::PixelRectInner(const cocos2d::Rect& rect, float scale) const
    {
      Rect result;
      
      result.origin.x = std::ceil(rect.origin.x/(kSpritePosition * scale));
      result.size.x = std::floor(rect.size.width/(kSpritePosition * scale));
      result.origin.y = std::ceil(rect.origin.y/(kSpritePosition * scale));
      result.size.y = std::floor(rect.size.height/(kSpritePosition * scale));
      
      return result;
    }
    
    cocos2d::Rect Viewport::CocosRect(const Rect& rect, float scale) const
    {
      auto result = cocos2d::Rect();
      result.origin.x = rect.origin.x * (kSpritePosition * scale);
      result.origin.y = rect.origin.y * (kSpritePosition * scale);
      result.size.width = rect.size.x * (kSpritePosition * scale);
      result.size.height = rect.size.y * (kSpritePosition * scale);
      return result;
    }
    
    bool Viewport::RemoveMapsOutsideOfRect(const Rect& rect,
                                           const Maps& currentMaps,
                                           PartialMapsManager::RemoveMapArgs& mapsToRemove)
    {
      for (const auto& map : currentMaps)
      {
        Rect mapRect;
        mapRect.origin.x = map.second->m_a1;
        mapRect.origin.y = map.second->m_b1;
        mapRect.size.x = map.second->m_width;
        mapRect.size.y = map.second->m_height;
        
        Rect newMapRect = mapRect.Extract(rect);
        if (newMapRect.size.x == 0 || newMapRect.size.y == 0)
        {
          mapsToRemove.push_back(mapRect.origin);
        }
      }
      
      return true;
    }
    
    bool Viewport::MoveMaps(const Vec2& offset, const cocos2d::Vec2& pointOffset, float scale, const Maps& maps)
    {
      for (auto map : maps)
      {
        cocos2d::Vec2 resultOffset = cocos2d::Vec2(map.second->m_a1 + offset.x,
                                                   map.second->m_b1 + offset.y) *  kSpritePosition * m_initialScale * scale;
        resultOffset += pointOffset;
        map.second->Transfrorm(resultOffset,
                               m_initialScale * scale);
      }
      
      return true;
    }
    
    bool Viewport::SplitRectOnChunks(const Rect& rect, const Rect& existingRect, std::vector<Rect>& result) const
    {
      return komorki::SplitRectOnChunks(rect, existingRect, kSegmentSize, result);
    }
    
    bool Viewport::CreatePartialMapsInRects(const std::vector<Rect>& rects,
                                                const Vec2& pixelOffset,
                                                const cocos2d::Vec2& offset,
                                                float scale,
                                            PartialMapsManager::CreateMapArgs& newMapsArgs)
    {
      for (auto rect : rects)
      {
        cocos2d::Vec2 offsetPoints = cocos2d::Vec2(rect.origin.x + pixelOffset.x,
                                                   rect.origin.y + pixelOffset.y) *  kSpritePosition * m_initialScale * scale;
        offsetPoints += offset;

        PartialMapsManager::CreateMapArg createMapArg;
        createMapArg.animated = m_enableAnimations;
        createMapArg.enableSmallAnimations = m_enableSmallAnimations;
        createMapArg.rect = rect;
        createMapArg.graphicPos = offsetPoints;
        createMapArg.scale = m_initialScale * scale;
        
        newMapsArgs.push_back(createMapArg);
      }
      
      return true;
    }
    
    Rect Viewport::GetUpcommingRect() const
    {
      float pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
      float screenScale = 1.0 / m_superView->getScale();
      
      cocos2d::Size pixelWorldSize = m_pixelWorldPos.size * screenScale;
      cocos2d::Vec2 pixelWorldPos = m_pixelWorldPos.origin -(m_superView->getPosition() - m_superViewOffset) * screenScale / pixelsScale;
      
      auto observableRect = cocos2d::Rect(pixelWorldPos.x,
                                          pixelWorldPos.y,
                                          pixelWorldSize.width,
                                          pixelWorldSize.height);
      
      Rect rect = PixelRectInner(observableRect, m_initialScale);
      Rect innerPrevRect = ResizeByStep(m_pos, rect, kSegmentSize);
      Rect extendedRect = ExtendRectWithStep(innerPrevRect, rect, kSegmentSize);
      
      return extendedRect;
    }
    
    Rect Viewport::GetCurrentVisibleRect() const
    {
      float pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
      float screenScale = 1.0 / m_superView->getScale();
      
      cocos2d::Size pixelWorldSize = m_pixelWorldPos.size * screenScale;
      cocos2d::Vec2 pixelWorldPos = m_pixelWorldPos.origin -(m_superView->getPosition() - m_superViewOffset) * screenScale / pixelsScale;
      
      auto observableRect = cocos2d::Rect(pixelWorldPos.x,
                                          pixelWorldPos.y,
                                          pixelWorldSize.width,
                                          pixelWorldSize.height);
      
      Rect rect = PixelRectInner(observableRect, m_initialScale);
      
      return rect;
    }

  }
}



