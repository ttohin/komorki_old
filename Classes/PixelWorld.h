#ifndef PixelWorld_H_69TPCKTG
#define PixelWorld_H_69TPCKTG

#include <vector>

#include "IPixelWorld.h"
#include "GreatPixel.h"
#include <memory>
#include <map>
#include "PixelWorldConfig.h"

namespace komorki
{
  class PixelWorld : public IPixelWorld
  {
  public:
    
    struct Group
    {
      Genom genom;
      unsigned int population = 0;
    };
    
    typedef std::map<Genom::GroupIdType, Group> GroupMap;
    
    PixelWorld();
    
    typedef std::shared_ptr<GreatPixel> PixelPtr;
    typedef std::vector<std::vector<PixelPtr> > PixelMap;
    
    void Init();
    virtual void Init(const PixelWorldConfig& config,
                      const GenomsGenerator::GenomsList& genoms);
    
    virtual void GenTerrain();
    virtual void GenLights();
    virtual void PopulateCells();
    
    virtual GreatPixel* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const;
    virtual TerrainAnalizer::Result GetTerrain() const;
    virtual komorki::Vec2 GetSize() const;
    virtual void Update(bool passUpdateResult, WorldUpdateList& result);
    virtual ~PixelWorld () {};
    CellDescriptor* ProcessMutation(CellDescriptor* source);
    void KillAllCells();
    void KillCellAtPostiion(const Vec2& pos);
    void ProccessTransaction(bool passUpdateResult, WorldUpdateList& result);
    bool CheckBounds(int x, int y);
    CellDescriptor* CreateRandomCell(GreatPixel* pd, Group& group);
    
  protected:
    PixelWorldConfig m_config;
    int CountTypeAroundPosition(komorki::Vec2 pos, int character);
    PixelMap m_map;
    unsigned int m_updateId;
    TerrainAnalizer::Result m_terrain;
    std::vector<std::vector<GreatPixel::Type> > m_typeMap;
    GroupMap m_groups;
    unsigned int m_numberOfLiveGroups = 0;
  };
  
}

#endif
