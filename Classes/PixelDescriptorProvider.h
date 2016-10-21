#ifndef PIXELDESCRIPTORPROVIDER_H_69TPCKTG
#define PIXELDESCRIPTORPROVIDER_H_69TPCKTG

#include <vector>

#include "IPixelDescriptorProvider.h"
#include "PixelDescriptor.h"
#include <memory>
#include <map>
#include "PixelProviderConfig.h"

namespace komorki
{
  extern int mapWidth;
  extern int mapHeight;
  
class PixelDescriptorProvider : public IPixelDescriptorProvider
{
public:
  
  struct Group
  {
    Genom genom;
    unsigned int population = 0;
  };
  
  typedef std::map<uint64_t, Group> GroupMap;
  
  PixelDescriptorProvider();
  
  typedef std::shared_ptr<PixelDescriptor> PixelPtr;
  typedef std::vector<std::vector<PixelPtr> > PixelMap;
  
  void Init();
  virtual void InitWithConfig(Config* config, const GenomsGenerator::GenomsList& genoms) override;
  
  virtual void GenTerrain();
  virtual void GenLights();
  virtual void PopulateCells();
  
  virtual PixelDescriptor* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const;
  virtual TerrainAnalizer::Result GetTerrain() const;
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
  
protected:
  Config* m_config;
  int CountTypeAroundPosition(komorki::Vec2 pos, int character);
  PixelMap m_map;
  unsigned int m_updateId;
  TerrainAnalizer::Result m_terrain;
  std::vector<std::vector<PixelDescriptor::Type> > m_typeMap;
  GroupMap m_groups;
  GenomsGenerator::GenomsList m_genoms;
};
  
}

#endif
