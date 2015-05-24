#include "PixelMapManager.h"

#define PMM_ENABLE_LOGGING 0
static const float spriteSize = 32;
static const unsigned int kMapSegmentSize = 50;

PixelMapManager::PixelMapManager(komorki::PixelDescriptorProvider::Config* config)
: m_mapSegmentSize(kMapSegmentSize)
{
  m_provider.InitWithConfig(config);
  m_manager = std::make_shared<komorki::AsyncPixelManager>(&m_provider);
  m_lastUpdateId = 0;
}

PixelMapManager::~PixelMapManager()
{
  m_manager->Stop();
  m_manager = nullptr;
  
  CleanUserData();
  
  for (auto map : m_maps)
  {
    map->removeFromParentAndCleanup(true);
  }
  
  for (auto map : m_mapBg)
  {
    map->removeFromParentAndCleanup(true);
  }
  
  for (auto map : m_mapDebugView)
  {
    map->removeFromParentAndCleanup(true);
  }
  
  m_bg->removeFromParentAndCleanup(true);
}

void PixelMapManager::CleanUserData()
{
  komorki::Vec2 size = m_provider.GetSize();
  for (int i = 0; i < size.x; ++i)
  {
    for (int j = 0; j < size.y; ++j)
    {
      auto pixelD = m_provider.GetDescriptor(i, j);
      if (pixelD->m_cellDescriptor && pixelD->m_cellDescriptor->userData != nullptr)
      {
        delete static_cast<PixelMapPartial::PixelDescriptorContext*>(pixelD->m_cellDescriptor->userData);
        pixelD->m_cellDescriptor->userData = nullptr;
      }
    }
  }
}

void PixelMapManager::CreateMap(cocos2d::Node* superView)
{
  m_superView = superView;
  
  komorki::Vec2 size = m_provider.GetSize();
  
  m_bg = Sprite::create("mapBackground.png");
  m_superView->addChild(m_bg);
  m_bg->setScaleX(size.x * spriteSize / m_bg->getContentSize().width);
  m_bg->setScaleY(size.y * spriteSize / m_bg->getContentSize().height);
  m_bg->setPosition(size.x * spriteSize * 0.5 ,
                    size.y * spriteSize * 0.5);
  m_bg->setOpacity(120);
  
  m_mapSegmentSize = MIN(m_mapSegmentSize, size.x);
  m_mapSegmentSize = MIN(m_mapSegmentSize, size.y);
  
  int stepsX = size.x/m_mapSegmentSize + 1;
  int stepsY = size.y/m_mapSegmentSize + 1;

  for (int i = 0; i < stepsX; ++i)
  {
    for (int j = 0; j < stepsY; ++j)
    {
      int width = MIN(m_mapSegmentSize, size.x - i*m_mapSegmentSize);
      int height = MIN(m_mapSegmentSize, size.y - j*m_mapSegmentSize);
      
      if (width == 0 || height == 0)
      {
        continue;
      }
      
      m_maps.emplace_back(std::make_shared<PixelMapPartial>(i*m_mapSegmentSize, j*m_mapSegmentSize, width, height, &m_provider));
      m_mapBg.emplace_back(std::make_shared<PixelMapBackground>(i*m_mapSegmentSize, j*m_mapSegmentSize, width, height));
      m_mapDebugView.emplace_back(std::make_shared<PixelDebugView>(i*m_mapSegmentSize, j*m_mapSegmentSize, width, height, &m_provider));
    }
  }
  
  for (auto map : m_mapBg)
  {
    map->init();
    map->setPosition(Vec2(map->m_a1*spriteSize, map->m_b1*spriteSize));
    m_superView->addChild(map.get());
  }
 
  for (auto map : m_maps)
  {
    map->init();
    map->setPosition(Vec2(map->m_a1*spriteSize, map->m_b1*spriteSize));
    m_superView->addChild(map.get());
  }
  
  for (auto map : m_mapDebugView)
  {
    map->init();
    map->setPosition(Vec2(map->m_a1*spriteSize, map->m_b1*spriteSize));
    m_superView->addChild(map.get());
  }
}

void PixelMapManager::UpdateAsync(float& updateTime)
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
  
  providerUpdateTime.AddValue(lastUpdateDuration);
  
  float mapsUpdateTime = 0.0f;
  
  Update(updateTime, mapsUpdateTime);
  
  if (lastUpdateDuration > updateTime)
  {
    updateTime = lastUpdateDuration;
  }
  
  for (const auto& operation : m_operationQueue)
  {
    if (operation.type == eOperationTypeAdd)
    {
      m_provider.SetCreatureType(operation.pos, operation.cellType);
    }
    if (operation.type == eOperationTypeRemove)
    {
      m_provider.KillCellAtPostiion(operation.pos);
    }
    if(operation.type == eOperationTypeKillAll)
    {
      m_provider.KillAllCells();
    }
  }
  
  m_lastUpdateId++;
  
  m_operationQueue.clear();
  
  m_manager->StartUpdate(1);
}

bool PixelMapManager::IsAvailable()
{
  return m_manager->IsAvailable();
}

void PixelMapManager::UpdateWarp(float& updateTime, unsigned int numberOfUpdates)
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
  
  log("warp lastUpdateDuration %f", lastUpdateDuration);
  
  providerUpdateTime.AddValue(lastUpdateDuration/(float)numberOfUpdates);

  CleanUserData();
  
  for (auto map : m_maps)
  {
    map->Reset();
  }
  
  if (lastUpdateDuration > updateTime)
  {
    updateTime = lastUpdateDuration;
  }
  
  m_operationQueue.clear();
  
  m_lastUpdateId++;
  
  m_manager->StartUpdate(numberOfUpdates);
}

static int updateNumber = 0;

void PixelMapManager::Update(float updateTime, float& outUpdateTime)
{
  assert(m_lastUpdateId == m_manager->GetUpdateId());
  
  outUpdateTime = 0;
 
  struct timeval tv;
  struct timeval start_tv;
  
  gettimeofday(&start_tv, NULL);
  
  double elapsed = 0.0;
  
  if (PMM_ENABLE_LOGGING)
  {
    log("update %d", updateNumber++);
  }
 
  const std::list<komorki::PixelDescriptorProvider::UpdateResult>& result = m_manager->GetUpdateResult();

  if (PMM_ENABLE_LOGGING)
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
      
      log("%s [%d:%d]->[%d:%d] [id:%d] t:%d",
          operationType.c_str(), (int)initialPos.x, (int)initialPos.y, (int)destinationPos.x, (int)destinationPos.y, id, descriptor->m_type);
    }
  }
  
  for (auto map : m_maps)
  {
    map->PreUpdate(result);
  }
  for (auto map : m_maps)
  {
    map->Update(result, updateTime);
  }
  for (auto map : m_maps)
  {
    map->PostUpdate(result);
  }
  for (auto map : m_mapBg)
  {
    map->Update(result, updateTime);
  }
  for (auto map : m_mapDebugView)
  {
    map->Update(result, updateTime);
  }
  
  gettimeofday(&tv, NULL);
  elapsed = (tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
  
  outUpdateTime += elapsed;
  
  mapsUpdateTime.AddValue(elapsed);
  numberOfEntriesInUpdate.AddValue(result.size());
  
  if(numberOfEntriesInUpdate.number%100 == 0)
  {
    log("neu min %llu\tmax %llu\tav %f\tnumber %u",
        numberOfEntriesInUpdate.minValue, numberOfEntriesInUpdate.maxValue, numberOfEntriesInUpdate.avaregeValue, numberOfEntriesInUpdate.number);
    log("put min %f\tmax %f\tav %f\tnumber %u",
        providerUpdateTime.minValue, providerUpdateTime.maxValue, providerUpdateTime.avaregeValue, providerUpdateTime.number);
    log("mup min %f\tmax %f\tav %f\tnumber %u",
        mapsUpdateTime.minValue, mapsUpdateTime.maxValue, mapsUpdateTime.avaregeValue, mapsUpdateTime.number);
  }
}

cocos2d::Size PixelMapManager::GetTotalMapSize() const
{
  komorki::Vec2 size = m_provider.GetSize();
  return Size(size.x * spriteSize, size.y * spriteSize);
}

bool PixelMapManager::HightlightCellOnCursorPos(Vec2 cursorPosition, komorki::CellType type)
{
  komorki::Vec2 pos = PixelPosFromCursorPosition(cursorPosition);
  
  for (auto map : m_maps)
  {
    map->HightlightCellOnPos(pos.x, pos.y, type);
  }
  
  return true;
}

void PixelMapManager::StopHightlighting()
{
  for (auto map : m_maps)
  {
    map->StopHightlighting();
  }
}

bool PixelMapManager::AddCreatureAtPosition(Vec2 cursorPosition, komorki::CellType type)
{
  EditMapOperation operation;
  operation.type = eOperationTypeAdd;
  operation.cellType = type;
  operation.pos = PixelPosFromCursorPosition(cursorPosition);
  
  m_operationQueue.push_back(operation);
  
  return true;
}

void PixelMapManager::RemoveCreatureAtPostion(Vec2 cursorPosition)
{
  EditMapOperation operation;
  operation.type = eOperationTypeRemove;
  operation.pos = PixelPosFromCursorPosition(cursorPosition);
  m_operationQueue.push_back(operation);
}

komorki::Vec2 PixelMapManager::PixelPosFromCursorPosition(Vec2 cursorPosition) const
{
  Vec2 cursorP = cursorPosition;
  
  float scale = m_superView->getScale();
  Vec2 origin = m_superView->getPosition();
  
  //  cursorP *= scale;
  Vec2 brushPos = Vec2(scale*spriteSize/2.f, scale*spriteSize/2.f) + cursorP - origin;
  
  int x = floor(brushPos.x / (scale * spriteSize));
  int y = floor(brushPos.y / (scale * spriteSize));
  
  return komorki::Vec2(x, y);
}

void PixelMapManager::CleanMap()
{
  EditMapOperation operation;
  operation.type = eOperationTypeKillAll;
  m_operationQueue.push_back(operation);
}

void PixelMapManager::Reset()
{
  for (auto map : m_maps)
  {
    map->Reset();
  }
}



