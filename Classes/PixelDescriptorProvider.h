#ifndef PIXELDESCRIPTORPROVIDER_H_69TPCKTG
#define PIXELDESCRIPTORPROVIDER_H_69TPCKTG

#include <vector>

#include "IPixelDescriptorProvider.h"
#include "PixelDescriptor.h"
#include <memory>
#include <map>

namespace komorki
{
  extern int mapWidth;
  extern int mapHeight;
  
class PixelDescriptorProvider : public IPixelDescriptorProvider
{
public:
  
  class Config
  {
  public:
    
    struct CellConfig
    {
      int health;
      int sleepTime;
      int attack;
      int passiveHealthChunkMin;
      int passiveHealthChunkMax;
      int healthPerAttack;
      int armor;
      int lifeTime;
      float percentOfMutations;
      CellType danger;
      CellType food;
      CellType friends;
    };
    
    int terrainSize;
    int mapHeight;
    int mapWidth;
    
    float percentOfCreatures;
    float percentOfOrange;
    float percentOfGreen;
    float percentOfSalad;
    float percentOfCyan;
    float percentOfBlue;
    
    CellConfig orange;
    CellConfig green;
    CellConfig salad;
    CellConfig cyan;
    CellConfig blue;
    
    Config::CellConfig* ConfigForCell(CellType type);
    
    Config();
  };
  
  typedef std::shared_ptr<PixelDescriptor> PixelPtr;
  typedef std::vector<std::vector<PixelPtr> > PixelMap;
  
  void Init();
  void InitWithConfig(Config* config);
  
  virtual PixelDescriptor* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const;
  virtual komorki::Vec2 GetSize() const;
  virtual void Update(bool passUpdateResult, std::list<UpdateResult>& result);
  virtual ~PixelDescriptorProvider () {};
  CellDescriptor* ProcessMutation(CellDescriptor* source);
  void KillAllCells();
  void KillCellAtPostiion(const Vec2& pos);
  void ProccessTransaction(bool passUpdateResult, std::list<UpdateResult>& result);
  bool CheckBounds(int x, int y);
  void SetCreatureType(const Vec2& pos, CellType type);
  PixelPtr CreateCell(CellType type,  const komorki::Vec2& pos);
  
  std::map<int, int> m_population;
  Config* m_config;
  
private:
  int CountTypeAroundPosition(komorki::Vec2 pos, int character);
  PixelMap m_map;
  unsigned int m_updateId;
  std::vector<std::vector<PixelDescriptor::Type> > m_typeMap;
};
  
}

#endif
