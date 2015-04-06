#ifndef __komorki__PixelMapManager__
#define __komorki__PixelMapManager__

#include "cocos2d.h"
#include "PixelDescriptorProvider.h"
#include "PixelMapPartial.hpp"
#include "Statistic.hpp"
#include "AsyncPixelManager.h"

class PixelMapManager
{
public:
  PixelMapManager(komorki::PixelDescriptorProvider::Config* config, int mapSegmentSize = 100);
  ~PixelMapManager();
  void CreateMap(cocos2d::Node* superView);
  
  cocos2d::Size GetTotalMapSize() const;
  bool HightlightCellOnCursorPos(Vec2 cursorPosition, komorki::CellType type);
  void StopHightlighting();
  bool AddCreatureAtPosition(Vec2 cursorPosition, komorki::CellType type);
  void RemoveCreatureAtPostion(Vec2 cursorPosition);
  void CleanMap();
  void Reset();

  void UpdateAsync(float& updateTime);
  void UpdateWarp(float& updateTime, unsigned int numberOfUpdates);
  bool IsAvailable();
  
protected:
  
  enum OperationType
  {
    eOperationTypeAdd,
    eOperationTypeRemove,
    eOperationTypeKillAll
  };
  
  struct EditMapOperation
  {
    OperationType type;
    komorki::CellType cellType;
    komorki::Vec2 pos;
  };
  
  komorki::Vec2 PixelPosFromCursorPosition(Vec2 cursorPosition) const;
  void CleanUserData();
  void Update(float updateTime, float& outUpdateTime);

  int m_mapSegmentSize;
  
  Node* m_superView;
  
  komorki::PixelDescriptorProvider m_provider;
  komorki::PixelDescriptorProvider::Config m_config;
  std::vector<std::shared_ptr<PixelMapPartial> > m_maps;
  Sprite* m_bg;
  
  std::shared_ptr<komorki::AsyncPixelManager> m_manager;
  std::list<EditMapOperation> m_operationQueue;
  StatisticCounter<double> providerUpdateTime;
  StatisticCounter<double> mapsUpdateTime;
  StatisticCounter<unsigned long long> numberOfEntriesInUpdate;
};

#endif /* defined(__komorki__PixelMapManager__) */
