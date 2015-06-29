//
//  Viewport.cpp
//  prsv
//
//  Created by ttohin on 07.06.15.
//
//

#include "Viewport.h"
#include "PartialMap.h"
#include "AsyncPixelManager.h"
#include "UIConfig.h"

using namespace komorki;

void ui::Viewport::Move(const cocos2d::Vec2& offset)
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

void ui::Viewport::Zoom(const cocos2d::Vec2& point, float scaleOffset)
{
  cocos2d::Vec2 newSuperViewPos;
  float newSuperViewScale;
  ::Zoom(m_superView->getPosition(), point, m_superView->getScale(), scaleOffset, newSuperViewPos, newSuperViewScale);
  m_superView->setPosition(newSuperViewPos);
  m_superView->setScale(newSuperViewScale);
}

void ui::Viewport::Calculate()
{
  float pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
  float screenScale = 1.0 / m_superView->getScale();
 
  cocos2d::Rect rect;
  rect.size = m_pixelWorldPos.size * screenScale;
  rect.origin = m_pixelWorldPos.origin - m_superView->getPosition() * screenScale / pixelsScale;
  
  Rect pixelRect = PixelRectInner(rect);
  
  if ( ! (screenScale > 0.5 && screenScale < 2.0) )
  {
    m_performMove = true;
    return;
  }
  
  if (pixelRect.In(m_pos) || m_pos.In(pixelRect))
  {
    return;
  }
  
  m_performMove = true;
}

ui::Viewport::Viewport(cocos2d::Node* superView, PixelDescriptorProvider::Config* config, const cocos2d::Size& originalSize)
{
  assert(superView);
  assert(config);
 
  m_originalSize = originalSize;
  m_superView = superView;
  m_performMove = false;
  
  m_initialScale = 0.1;
  
  m_pixelWorldPos.origin = {0, 0};
  m_pixelWorldPos.size = originalSize;
  
  m_provider = std::make_shared<PixelDescriptorProvider>();
  m_provider->InitWithConfig(config);
  std::list<komorki::PixelDescriptorProvider::UpdateResult> result;
  for (int i = 0; i < 100; i++)
  {
    m_provider->Update(false, result);
  }
  m_manager = std::make_shared<komorki::AsyncPixelManager>(m_provider.get());
  m_lastUpdateId = 0;
  
  CreateMap();
  
  Test();
}

ui::Viewport::~Viewport()
{
 assert(0);
}

void ui::Viewport::Test()
{
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
    Rect r2 = this->PixelRect(r1);
    assert(r2 == Rect({{0,0}, {100, 100}}));
  }
  {
    cocos2d::Rect r1 = {0, 0, 320.2, 320.2};
    Rect r2 = this->PixelRect(r1);
    assert(r2 == Rect({{0,0}, {101, 101}}));
  }
  
  {
    cocos2d::Rect r1 = {0, 0, 320, 320};
    cocos2d::Rect r2 = {32, 0, 288, 320};
    cocos2d::Rect r3 = {32.01, 0, 288, 320};
    Rect r1p = this->PixelRect(r1);
    Rect r2p = this->PixelRectInner(r2);
    Rect r3p = this->PixelRectInner(r3);
    assert(r2p.In(r1p));
    assert(!r3p.In(r1p));
  }
  {
    cocos2d::Rect r2 = {0, 32, 320, 288};
    cocos2d::Rect r3 = {0, 32.01, 320, 288};
    Rect r1p = {{0, 0}, {100, 100}};
    Rect r2p = this->PixelRectInner(r2);
    Rect r3p = this->PixelRectInner(r3);
    assert(r2p.In(r1p));
    assert(!r3p.In(r1p));
  }
}

void ui::Viewport::CreateMap()
{
  m_maps.clear();
  
  komorki::Vec2 size = m_provider->GetSize();
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

  m_pixelWorldPos.size = m_pixelWorldPos.size * screenScale;
  m_pixelWorldPos.origin += -m_superView->getPosition() * screenScale / pixelsScale;
  
  auto observableRect = cocos2d::Rect(m_pixelWorldPos.origin.x - kViewportMargin,
                                      m_pixelWorldPos.origin.y - kViewportMargin,
                                      m_pixelWorldPos.size.width + 2*kViewportMargin,
                                      m_pixelWorldPos.size.height + 2*kViewportMargin);
 
  Rect rect = PixelRectInner(observableRect);
  rect = rect.Extract({Vec2(0, 0), size});

  pixelsScale = m_originalSize.width / m_pixelWorldPos.size.width;
  
  cocos2d::Vec2 posOffset = m_superView->getPosition();
  
  float pixelSize = kSpritePosition * m_initialScale * pixelsScale;
  
  Vec2 offsetInPixels = m_pos.origin - rect.origin;
  cocos2d::Vec2 offset = m_superView->getPosition() - cocos2d::Vec2(offsetInPixels.x * pixelSize, offsetInPixels.y * pixelSize);
 
  m_superView->setPosition(offset);
  m_superView->setScale(1.f);
  
  CreatePixelMaps(rect, cocos2d::Vec2::ZERO, pixelsScale);
  
  m_prevPos = rect;
  m_pos = rect;
}

void ui::Viewport::CreatePixelMaps(const Rect& rect, const cocos2d::Vec2& offset, float scale)
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
      
      auto map = std::make_shared<komorki::ui::PartialMap>();
      map->Init(rect.origin.x + i*m_mapSegmentSize,
                rect.origin.y +j*m_mapSegmentSize,
                width,
                height,
                m_provider.get(),
                m_superView,
                cocos2d::Vec2::ZERO);
      map->Transfrorm(offset + cocos2d::Vec2(i*m_mapSegmentSize * kSpritePosition * m_initialScale * scale,
                                             j*m_mapSegmentSize * kSpritePosition * m_initialScale * scale),
                      m_initialScale * scale);
      m_maps.push_back(map);
      
    }
  }
}

void ui::Viewport::Resize(const cocos2d::Size& originalSize)
{
  m_pixelWorldPos.size = originalSize;
  m_originalSize = originalSize;
  
  m_initialScale = 0.1;
  
  CreateMap();
}

cocos2d::Size ui::Viewport::GetTotalMapSize() const
{
}
bool ui::Viewport::HightlightCellOnCursorPos(cocos2d::Vec2 cursorPosition, komorki::CellType type)
{
}
void ui::Viewport::StopHightlighting()
{
}
bool ui::Viewport::AddCreatureAtPosition(cocos2d::Vec2 cursorPosition, komorki::CellType type)
{
 assert(0);
}
void ui::Viewport::RemoveCreatureAtPostion(cocos2d::Vec2 cursorPosition)
{
 assert(0);
}
void ui::Viewport::CleanMap()
{
 assert(0);
}
void ui::Viewport::Reset()
{
 assert(0);
}
void ui::Viewport::UpdateAsync(float& updateTime)
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
  
  float lastUpdateDuration = m_manager->GetLastUpdateTime();
  
  float mapsUpdateTime = 0.0f;
  
  Update(updateTime, mapsUpdateTime);
  
  if (lastUpdateDuration > updateTime)
  {
    updateTime = lastUpdateDuration;
  }
  
  m_lastUpdateId++;
  
  m_manager->StartUpdate(1);
}

bool ui::Viewport::IsAvailable()
{
  return m_manager->IsAvailable();
}

void ui::Viewport::UpdateWarp(float& updateTime, unsigned int numberOfUpdates)
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
  
  float lastUpdateDuration = m_manager->GetLastUpdateTime();
  
  for (auto map : m_maps)
  {
    map->Reset();
  }
  
  if (lastUpdateDuration > updateTime)
  {
    updateTime = lastUpdateDuration;
  }
  
  m_lastUpdateId++;
  
  m_manager->StartUpdate(numberOfUpdates);
}

void ui::Viewport::Update(float updateTime, float& outUpdateTime)
{
  assert(m_lastUpdateId == m_manager->GetUpdateId());
  
  outUpdateTime = 0;
  
  struct timeval tv;
  struct timeval start_tv;
  
  gettimeofday(&start_tv, NULL);
  
  double elapsed = 0.0;
  
  const std::list<komorki::PixelDescriptorProvider::UpdateResult>& result = m_manager->GetUpdateResult();
  
  if (0)
  {
    for (auto u : result)
    {
      std::string operationType;
      
      komorki::PixelDescriptor* descriptor = static_cast<komorki::PixelDescriptor*>(u.desc);
      
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
      
      if(u.addCreature == true)
        destinationPos = Vec2(u.addCreature.value.destinationDesc->x, u.addCreature.value.destinationDesc->y);
      
      if (m == true)
        destinationPos = Vec2(m.value.destinationDesc->x, m.value.destinationDesc->y);
      
      cocos2d::log("%s [%d:%d]->[%d:%d] [id:%d] t:%d",
          operationType.c_str(), (int)initialPos.x, (int)initialPos.y, (int)destinationPos.x, (int)destinationPos.y, id, descriptor->m_type);
    }
  }
  
  for (auto map : m_maps)
  {
    map->PreUpdate1(result, updateTime);
  }
  for (auto map : m_maps)
  {
    map->PreUpdate2(result, updateTime);
  }
  for (auto map : m_maps)
  {
    map->Update(result, updateTime);
  }
  
  for (auto map : m_maps)
  {
    map->PostUpdate(result, updateTime);
  }
  
  gettimeofday(&tv, NULL);
  elapsed = (tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
  
  outUpdateTime += elapsed;
  
  
  if (m_performMove)
  {
    m_performMove = false;
    PerformMove();
    return;
  }
}

void ui::Viewport::PerformMove()
{
  CreateMap();
}

ui::Viewport::Rect ui::Viewport::PixelRect(const cocos2d::Rect& rect)
{
  Rect result;
  
  result.origin.x = std::floor(rect.origin.x/(kSpritePosition * m_initialScale));
  result.size.x = std::ceil(rect.size.width/(kSpritePosition * m_initialScale));
  result.origin.y = std::floor(rect.origin.y/(kSpritePosition * m_initialScale));
  result.size.y = std::ceil(rect.size.height/(kSpritePosition * m_initialScale));
  
  return result;
}

ui::Viewport::Rect ui::Viewport::PixelRectInner(const cocos2d::Rect& rect)
{
  Rect result;
  
  result.origin.x = std::ceil(rect.origin.x/(kSpritePosition * m_initialScale));
  result.size.x = std::floor(rect.size.width/(kSpritePosition * m_initialScale));
  result.origin.y = std::ceil(rect.origin.y/(kSpritePosition * m_initialScale));
  result.size.y = std::floor(rect.size.height/(kSpritePosition * m_initialScale));
  
  return result;
}

cocos2d::Rect ui::Viewport::CocosRect(const Rect& rect)
{
  auto result = cocos2d::Rect();
  result.origin.x = rect.origin.x * (kSpritePosition * m_initialScale);
  result.origin.y = rect.origin.y * (kSpritePosition * m_initialScale);
  result.size.width = rect.size.x * (kSpritePosition * m_initialScale);
  result.size.height = rect.size.y * (kSpritePosition * m_initialScale);
  return result;
}

ui::Viewport::Rect ui::Viewport::Rect::Extract(const ui::Viewport::Rect &other) const
{
  Rect result;
  result.origin.x = 0;
  result.origin.y = 0;
  result.size.x = 0;
  result.size.y = 0;
  
  int offset = other.origin.x - origin.x;
  if (offset >= 0)
  {
    if (origin.x + size.x >= other.origin.x)
    {
      result.origin.x = other.origin.x;
      result.size.x = std::min(origin.x + size.x - other.origin.x, other.size.x);
    }
    else if (origin.x + size.x < other.origin.x)
    {
      result.origin.x = 0;
      result.size.x = 0;
    }
    else
    {
      assert(0);
    }
  }
  else if (offset < 0)
  {
    if (other.origin.x + other.size.x > origin.x)
    {
      result.origin.x = origin.x;
      result.size.x = std::min(other.origin.x + other.size.x - origin.x, size.x);
    }
    else if (other.origin.x + other.size.x < origin.x)
    {
      result.origin.x = 0;
      result.size.x = 0;
    }
    else
    {
      assert(0);
    }
  }
  else
  {
    assert(0);
  }
  
  if (result.size.x == 0)
  {
    return result;
  }
  
  offset = other.origin.y - origin.y;
  if (offset >= 0)
  {
    if (origin.y + size.y >= other.origin.y)
    {
      result.origin.y = other.origin.y;
      result.size.y = std::min(origin.y + size.y - other.origin.y, other.size.y);
    }
    else if (origin.y + size.y < other.origin.y)
    {
      result.origin.y = 0;
      result.size.y = 0;
    }
    else
    {
      assert(0);
    }
  }
  else if (offset < 0)
  {
    if (other.origin.y + other.size.y > origin.y)
    {
      result.origin.y = origin.y;
      result.size.y = std::min(other.origin.y + other.size.y - origin.y, size.y);
    }
    else if (other.origin.y + other.size.y < origin.y)
    {
      result.origin.y = 0;
      result.size.y = 0;
    }
    else
    {
      assert(0);
    }
  }
  else
  {
    assert(0);
  }
  
  if (result.size.y == 0)
  {
    result.size.x = 0;
    result.origin.x = 0;
  }
  
  return result;
}

bool ui::Viewport::Rect::In(const Rect& other) const
{
  return origin.x >= other.origin.x
  && origin.x + size.x <= other.origin.x + other.size.x
  && origin.y >= other.origin.y
  && origin.y + size.y <= other.origin.y + other.size.y;
}

bool ui::Viewport::Rect::operator==(const Rect& rect) const
{
  return origin.x == rect.origin.x &&
  origin.y == rect.origin.y &&
  size.x == rect.size.x &&
  size.y == rect.size.y;
}

bool ui::Viewport::Rect::operator!=(const Rect& rect) const
{
  return !(*this == rect);
}

