#include "PixelWorld.h"
#include "Random.h"
#include <math.h>
#include <assert.h>
#include "CellsLogic.h"
#include "CellShapes.h"
#include "CellDescriptor.h"
#include "Map.h"
#include "DiamondSquareGenerator.h"

#define COLLECT_POPULATION_METRICS 0

namespace komorki
{
  int nextId = 0;
  int kMaxNumberOfGroups = 64;
  int kInitialNumberOfGroups = 32;

  PixelWorld::PixelWorld()
  {
    
  }
  
  void PixelWorld::Init(const PixelWorldConfig& config,
                        const GenomsGenerator::GenomsList& genoms)
  {
    m_config = config;
    for (auto& g : genoms)
    {
      Group group;
      group.genom = g;
      m_groups[g.m_groupId] = group;
    }
    
    if ( m_config.height <= 3)
      m_config.height = 3;
    
    if ( m_config.width <= 3)
      m_config.width = 3;
    
    if (m_config.height > 1000)
      m_config.height = 1000;
    
    if (m_config.width > 1000)
      m_config.width = 1000;
    
    Init();
  }
  
  void GenerateShape(GreatPixel* pd, IShape::Ptr& outShape, ShapeType& outShapeType)
  {
    std::shared_ptr<IShape> shape;
    
    unsigned int numberOfShapes = 2;
    unsigned int shapeIndex = cRandABInt(0, numberOfShapes);
    shapeIndex = 3;
    if (0 == shapeIndex)
    {
      shape = std::make_shared<SinglePixel>(pd);
      outShapeType = ShapeType::eShapeTypeSinglePixel;
    }
    else if (1 == shapeIndex)
    {
      unsigned int w = cRandABInt(2, 4);
      unsigned int h = cRandABInt(2, 4);
      shape = std::make_shared<RectShape>(pd, Vec2(0, 0), Vec2(w, h));
      outShapeType = ShapeType::eShapeTypeRect;
    }
    else if (2 == shapeIndex)
    {
      unsigned int numberOfPixels = cRandABInt(10, 12);
      shape = std::make_shared<PolymorphShape>(pd, numberOfPixels);
      outShapeType = ShapeType::eShapeTypeAmorph;
    }
    else if (3 == shapeIndex)
    {
      GreatPixel::Vec pixels;
      pixels.push_back(pd->Offset(-1, 0));
      pixels.push_back(pd->Offset(0, 1));
      pixels.push_back(pd->Offset(0, -1));
      pixels.push_back(pd->Offset(1, 0));
      pixels.push_back(pd);
      shape = std::make_shared<PolymorphShape>(pd, pixels);
      outShapeType = ShapeType::eShapeTypeFixed;
    }
    else if (4 == shapeIndex)
    {
      unsigned int numberOfPixels = cRandABInt(2, 4);
      shape = std::make_shared<PolymorphShape>(pd, numberOfPixels);
      outShapeType = ShapeType::eShapeTypePolymorph;
    }
    
    else
    {
      assert(false);
    }
    
    outShape = shape;
  }
  
  CellDescriptor* PixelWorld::CreateRandomCell(GreatPixel* pd, Group& group)
  {
    assert(pd);
    
    
    bool empty = true;
    auto shape = group.genom.m_shape->CopyWithBasePixel(pd);
    if (shape == nullptr) {
      return nullptr;
    }
    
    shape->ForEach([&empty](GreatPixel* pixel, bool& stop){
      
      if (pixel == nullptr || pixel->m_type != GreatPixel::Empty)
      {
        empty = false;
        stop = true;
      }
      else
      {
        assert(pixel->m_cellDescriptor == nullptr);
      }
    });
    
    if (empty == false)
    {
      return nullptr;
    }
    
    CellDescriptor* cd = new CellDescriptor(pd);
    cd->m_shape = shape;
    
    pd->m_type = GreatPixel::CreatureType;
    cd->m_id = nextId++;
    cd->m_genom = group.genom;
    
    cd->m_health = group.genom.m_health;
    cd->m_armor = group.genom.m_armor;
    cd->m_age = cRandABInt(0, 50);
    cd->m_sleepCounter =  cRandABInt(0, group.genom.m_sleepTime);
    cd->m_skipFirstStep = true;
    
    cd->Finish();
    
    if (group.population == 0) {
      m_numberOfLiveGroups += 1;
    }
    group.population += 1;
    
    return cd;
  }
  
  void PixelWorld::Init()
  {
    m_updateId = 0;
    nextId = 0;
    m_map.reserve(m_config.width);
    m_typeMap.reserve(m_config.width);
                      
    for (int i = 0; i < m_config.width; ++i)
    {
      m_map.push_back(std::vector<PixelPtr>());
      m_map[i].reserve(m_config.height);
      m_typeMap.push_back(std::vector<GreatPixel::Type>());
      m_typeMap[i].reserve(m_config.height);
      for (int j = 0; j < m_config.height; ++j)
      {
        m_map[i].push_back(std::make_shared<GreatPixel>(i, j));
      }
    }
    
    for (int i = 0; i < m_config.width; ++i)
    {
      for (int j = 0; j < m_config.height; ++j)
      {
        GreatPixel* pd = m_map[i][j].get();
        
        for (int di = -1; di <= 1; ++di)
        {
          for (int dj = -1; dj <= 1; ++dj)
          {
            if (i + di >= m_config.width || i + di < 0)
            {
              continue;
            }
            
            if (j + dj >= m_config.height || j + dj < 0)
            {
              continue;
            }
            
            if (di == 0 && dj == 0) {
              continue;
            }
            
            GreatPixel* aroundPd = m_map[i + di][j + dj].get();
            pd->SetDirection(di, dj, aroundPd);
          }
        }
      }
    }
    
    GenTerrain();
    GenLights();
    
    for (int i = 0; i < m_config.width; ++i)
    {
      m_map[i][0]->m_type = (GreatPixel::TerrainType);
    }
    for (int i = 0; i < m_config.width; ++i)
    {
      m_map[i][m_config.height - 1]->m_type = (GreatPixel::TerrainType);
    }
    for (int i = 0; i < m_config.height; ++i)
    {
      m_map[0][i]->m_type = (GreatPixel::TerrainType);
    }
    for (int i = 0; i < m_config.height; ++i)
    {
      m_map[m_config.width - 1][i]->m_type = (GreatPixel::TerrainType);
    }
    
    PopulateCells();
  }
  
  Genom GenerateGenom(uint64_t groupId, PixelWorld::PixelMap& map)
  {
    Genom g;
    g.m_groupId = groupId;
    
    for (int j = 0; j < kMaxNumberOfGroups; j++)
    {
      if (g.m_groupId == (1 << j)) {
        continue;
      }
      g.m_foodGroupId |= cRandAorB(0, 1) ? 1 << j : 0;
    }
    
    for (int j = 0; j < kMaxNumberOfGroups; j++)
    {
      if (g.m_groupId == (1 << j)) {
        continue;
      }
      g.m_dangerGroupId = cRandAorB(0, 1, 0.9) ? 1 << j : 0;
    }
    
    for (int j = 0; j < kMaxNumberOfGroups; j++)
    {
      if (g.m_groupId == (1 << j)) {
        continue;
      }
      g.m_friendGroupId |= cRandAorB(0, 1) ? 1 << j : 0;
    }
    
    g.m_health = cRandABInt(100, 1000);
    g.m_armor = cRandABInt(1, 100);
    g.m_sleepTime = cRandABInt(0, 10);
    g.m_lifeTime = cRandABInt(100, 1000);
    g.m_damage = cRandABInt(1, 50);
    g.m_lightFood = cRandABInt(1, 15);
    g.m_passiveHealthIncome = cRandABInt(0, 10);
    g.m_healthPerAttach = cRandABInt(10, 100);
    GenerateShape(map[20][20].get(), g.m_shape, g.m_shapeType);
    
    return g;
  }
  
  std::vector<Genom> GenerateGenoms(PixelWorld::PixelMap& map)
  {
    std::vector<Genom> result;
    for (int i = 0; i < kInitialNumberOfGroups; ++i)
    {
      Genom g = GenerateGenom(1 << i, map);
      result.push_back(g);
    }
    
    return result;
  }
  
  void PixelWorld::PopulateCells()
  {
    std::vector<GroupMap::key_type> groups;
    for (const auto& it : m_groups)
    {
      groups.push_back(it.first);
    }
    
    for (int i = 0; i < m_config.width; ++i)
    {
      for (int j = 0; j < m_config.height; ++j)
      {
        GreatPixel* pd = m_map[i][j].get();
        if (cBoolRandPercent(m_config.percentOfCellsOnStartup))
        {
          int groupIndex = cRandABInt(0, groups.size());
          auto groupId = groups[groupIndex];
          
          static int count = 1;
          //        if (count == 0)
          //        {
          //          continue;
          //        }
          
          if (CreateRandomCell(pd, m_groups[groupId]))
          {
            count--;
          }
        }
      }
    }
  }
  
  void PixelWorld::GenTerrain()
  {
    komorki::DiamondSquareGenerator gen(512, 512, 60.f, -0.5, false);
    gen.Generate(nullptr);
    
    komorki::DiamondSquareGenerator gen1(512, 512, 20.f, -0.5, false);
    gen1.Generate(nullptr);
    
    gen.Multiply(&gen1, nullptr);
    
    auto buffer = gen.GetBuffer(0,0, m_config.width, m_config.height);
    auto analizer = std::shared_ptr<TerrainAnalizer>(new TerrainAnalizer(buffer));
    auto analizedBuffer = analizer->GetLevels();
    
    analizedBuffer->ForEach([&](const int& x, const int& y, const TerrainLevel& level)
                            {
                              if (x%2 != 0 && y%2 != 0) {
                                return ;
                              }
                              if (level >= TerrainLevel::Ground)
                              {
                                m_map[x/2][y/2]->m_type = GreatPixel::TerrainType;
                              }
                            });
    
    m_terrain = analizer->GetResult();
  }
  
  void PixelWorld::GenLights()
  {
    komorki::DiamondSquareGenerator gen1(512, 512, 10.f, -0.5, false);
    gen1.Generate(nullptr);
    auto buffer = gen1.GetBuffer(0,0, m_config.width, m_config.height);
    buffer->ForEach([&](const int& x, const int& y, const float& level)
                    {
                      float resultlevel = 0.5 + level * 0.5f;
                      float heightLevel = ((float)y/(float)m_config.height);
                      resultlevel = resultlevel * 0.8 + heightLevel * 0.2;
                      m_map[x][y]->m_physicalDesc.light = resultlevel;
                    });
  }
  
  GreatPixel* PixelWorld::GetDescriptor(PixelPos x, PixelPos y) const
  {
    if (x < 0 || x >= m_config.width)
    {
      return nullptr;
    }
    
    if (y < 0 || y >= m_config.height)
    {
      return nullptr;
    }
    
    return m_map[x][y].get();
  }
  
  TerrainAnalizer::Result PixelWorld::GetTerrain() const
  {
    return m_terrain;
  }
  
  Vec2 PixelWorld::GetSize() const
  {
    return Vec2(m_config.width, m_config.height);
  }
  
  bool PixelWorld::CheckBounds(int x, int y)
  {
    if(x < 0 || x >= m_config.width || y < 0 || y >= m_config.height)
      return false;
    
    return true;
  }
  
  void PixelWorld::ProccessTransaction(bool passUpdateResult, WorldUpdateList& result)
  {
    for (int i = 0; i < m_config.width; ++i)
    {
      for (int j = 0; j < m_config.height; ++j)
      {
        auto pd = m_map[i][j].get();
        if (pd->m_type != GreatPixel::CreatureType)
        {
          continue;
        }
        
        assert(pd->m_cellDescriptor);
        auto d = pd->m_cellDescriptor;
        d->m_age += 1;
        
        for (auto i : d->nextTurnTransaction)
        {
          d->m_armor += i.m_armor;
          d->m_health += i.m_health;
        }
        
        d->nextTurnTransaction.clear();
        
        bool shouldBeRemoved = d->m_age > d->m_genom.m_lifeTime;
        
        if(d->m_armor <= 0 || d->m_health <= 0 || shouldBeRemoved)
        {
          d->Shape([](GreatPixel* pd, bool& stop){
            pd->m_cellDescriptor = nullptr;
            pd->m_type = GreatPixel::Empty;
          });
          
          if (m_groups[d->m_genom.m_groupId].population > 0)
          {
            m_groups[d->m_genom.m_groupId].population -= 1;
          }
          else if (m_groups[d->m_genom.m_groupId].population == 0)
          {
            assert(0);
          }
          
          if (m_groups[d->m_genom.m_groupId].population == 0)
          {
            m_numberOfLiveGroups -= 1;
            GenomsGenerator::UpdateGenomState(m_groups[d->m_genom.m_groupId].genom);
          }
          
          if (passUpdateResult)
          {
            WorldUpateDiff r(d);
            r.deleteCreature.SetValueFlag(true);
            r.deleteCreature.value.cellDescriptor = std::shared_ptr<CellDescriptor>(d);
            result.push_back(r);
          }
          else
          {
            delete d;
          }
          
          continue;
        }
        
        int maxHealth = d->m_genom.m_health;
        if(d->m_health >= maxHealth * 2)
        {
          auto destinationDesc = ProcessMutation(d);
          if (destinationDesc)
          {
            d->m_health = d->m_genom.m_health;
            
            if (passUpdateResult)
            {
              WorldUpateDiff r(d);
              r.userData = nullptr;
              
              r.addCreature.SetValueFlag(true);
              r.addCreature.value.destinationDesc = destinationDesc->parent;
              result.push_back(r);
            } // if (passUpdateResult)
          } // (destinationDesc != nullptr)
          
        } // if(d->m_health >= maxHealth * 2)
      }
    }
  }
  
  CellDescriptor* PixelWorld::ProcessMutation(CellDescriptor* source)
  {
    CellDescriptor* result = nullptr;
    
    if (false && cBoolRandPercent(0.001))
    {
      for (auto& g : m_groups)
      {
        if (g.second.population != 0)
        {
          continue;
        }
        
        source->AroundRandom([&](GreatPixel* pd, bool& stop){
          if (nullptr != CreateRandomCell(pd, g.second)) {
            stop = true;
            result = pd->m_cellDescriptor;
          }
        });
        
        if (result)
        {
          return result;
        }
      }
    }
    
    source->AroundRandom([&](GreatPixel* pd, bool& stop){
      
      if (nullptr != CreateRandomCell(pd, m_groups[source->m_genom.m_groupId])) {
        stop = true;
        result = pd->m_cellDescriptor;
      }
      
    });
    
    return result;
  }
  
  void PixelWorld::Update(bool passUpdateResult, WorldUpdateList& result)
  {
    //  return;
    //  if (m_updateId == 0)
    //  {
    //    return;
    //  }
    
#if COLLECT_POPULATION_METRICS
    m_population.clear();
#endif
    
    ProccessTransaction(passUpdateResult, result);
    
    for (int i = 0; i < m_config.width; ++i)
    {
      const std::vector<PixelPtr>& line = m_map[i];
      for (int j = 0; j < m_config.height; ++j)
      {
        m_typeMap[i][j] = line[j].get()->m_type;
      }
    }
    
    for (int i = 0; i < m_config.width; ++i)
    {
      const std::vector<GreatPixel::Type>& typeMapLine = m_typeMap[i];
      const std::vector<PixelPtr>& line = m_map[i];
      for (int j = 0; j < m_config.height; ++j)
      {
        auto oldPd = typeMapLine[j];
        
        if (oldPd == GreatPixel::CreatureType)
        {
          auto pd = line[j];
          
          auto d = pd->m_cellDescriptor;
          if (d == nullptr || d->parent != pd.get())
          {
            continue;
          }
          
          if (d->m_updateId == m_updateId)
          {
            continue;
          }
          
#if COLLECT_POPULATION_METRICS
          m_population[d->m_character]=m_population[d->m_character]+1;
#endif
          
          d->m_updateId = m_updateId;
          
          if (d && d->m_new)
          {
            d->m_new = false;
            
            if (passUpdateResult)
            {
              WorldUpateDiff r(d);
              r.addCreature.SetValueFlag(true);
              r.addCreature.value.destinationDesc = pd.get();
              result.push_back(r);
            }
            
            continue;
          }
          
          if (d->m_skipFirstStep)
          {
            d->m_skipFirstStep = false;
            continue;
          }
          
          Optional<Action> a;
          Optional<Movement> m;
          Optional<Morphing> morph;
          Optional<ChangeRect> changeRect;
          
          d->Check();
          
          komorki::ProcessCell(d,
                               Vec2(i, j),
                               m_map,
                               m,
                               a,
                               morph,
                               changeRect);
          
          d->Check();
          
          if (m == false && a == false && morph == false && changeRect == false)
            continue;
          
          WorldUpateDiff updateResult(d);
          updateResult.desc = pd.get();
          updateResult.action = a;
          updateResult.movement = m;
          updateResult.morph = morph;
          updateResult.changeRect = changeRect;
          
          if (passUpdateResult)
          {
            result.push_back(updateResult);
          }
        }
      }
    }
    
    for (int i = 0; i < m_config.width; ++i)
    {
      const std::vector<PixelPtr>& line = m_map[i];
      for (int j = 0; j < m_config.height; ++j)
      {
        auto pd = line[j];
        
        if (pd->m_type == GreatPixel::CreatureType)
        {
          auto d = pd->m_cellDescriptor;
          
          d->Shape([d](GreatPixel* cellPd, bool& stop){
            assert(cellPd->m_cellDescriptor == d);
          });
        }
      }
    }
    
    ++m_updateId;
  }

  void PixelWorld::KillAllCells()
  {
    for (int i = 0; i < m_config.width; ++i)
    {
      const std::vector<PixelPtr>& line = m_map[i];
      for (int j = 0; j < m_config.height; ++j)
      {
        auto pd = line[j];
        if (pd->m_type == GreatPixel::CreatureType)
        {
          auto d = pd->m_cellDescriptor;
          Transaction t;
          t.m_health = INT32_MIN/2;
          d->nextTurnTransaction.push_back(t);
        }
      }
    }
  }
  
  void PixelWorld::KillCellAtPostiion(const Vec2& pos)
  {
    if (!CheckBounds(pos.x, pos.y))
    {
      return;
    }
    
    auto pd = m_map[pos.x][pos.y];
    if (pd->m_type != GreatPixel::Empty)
    {
      auto d = pd->m_cellDescriptor;
      Transaction t;
      t.m_health = INT32_MIN/2;
      d->nextTurnTransaction.push_back(t);
    }
  }
  
}// namespace komorki;
