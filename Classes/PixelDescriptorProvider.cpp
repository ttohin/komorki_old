#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include <math.h>
#include "CellsLogic.h"
#include "CellShapes.h"
#include "CellDescriptor.h"
#include "Map.h"
#include "DiamondSquareGenerator.h"

#define COLLECT_POPULATION_METRICS 1

namespace komorki
{

int mapWidth = 0;
int mapHeight = 0;
  
int nextId = 0;
  
IPixelDescriptorProvider::UpdateResult::UpdateResult(CellDescriptor* desc):
desc(desc->parent), userData(desc->userData)
{
//  assert(userData);
}

PixelDescriptorProvider::Config::Config()
{
//  this->terrainSize = 8;
//  this->mapWidth = 150;
//  this->mapHeight = 100;
//
//  this->green.health = 301;
//  this->green.sleepTime = 2;
//  this->green.attack = 14;
//  this->green.passiveHealthChunkMin = -1;
//  this->green.passiveHealthChunkMax = -1;
//  this->green.armor = 10;
//  this->green.lifeTime = 830;
//  this->green.percentOfMutations = 0.0;
//  this->green.healthPerAttack = 90;
//  this->green.food = (CellType)(eCellTypeImprovedSalad | eCellTypeSalad);
//  this->green.danger = (CellType)(eCellTypeHunter);
//  this->green.friends = eCellTypeGreen;
//  
//  this->orange.health = 1400;
//  this->orange.sleepTime = 0;
//  this->orange.attack = 15;
//  this->orange.passiveHealthChunkMin = -1;
//  this->orange.passiveHealthChunkMax = -1;
//  this->orange.armor = 300;
//  this->orange.lifeTime = 1530;
//  this->orange.percentOfMutations = 0.0;
//  this->orange.healthPerAttack = 66;
//  this->orange.food = (CellType)(eCellTypeGreen);
//  this->orange.danger = (CellType)(eCellTypeUnknown);
//  this->orange.friends = (CellType)(eCellTypeHunter);
//  
//  this->salad.health = 301;
//  this->salad.sleepTime = 30;
//  this->salad.attack = 100;
//  this->salad.passiveHealthChunkMin = -10;
//  this->salad.passiveHealthChunkMax = 16;
//  this->salad.armor = 21;
//  this->salad.lifeTime = 830;
//  this->salad.percentOfMutations = 0.0;
//  this->salad.healthPerAttack = 301;
//  this->salad.food = (CellType)(eCellTypeBigBlue);
//  this->salad.danger = (CellType)(eCellTypeUnknown);
//  this->salad.friends = (CellType)(eCellTypeUnknown);
//  
//  this->cyan.health = 301;
//  this->cyan.sleepTime = 0;
//  this->cyan.attack = 3;
//  this->cyan.passiveHealthChunkMin = -10;
//  this->cyan.passiveHealthChunkMax = 14;
//  this->cyan.armor = 30;
//  this->cyan.lifeTime = 830;
//  this->cyan.percentOfMutations = 0.0;
//  this->cyan.healthPerAttack = 40;
//  this->cyan.food = (CellType)(eCellTypeBigBlue);
//  this->cyan.danger = (CellType)(eCellTypeGreen);
//  this->cyan.friends = (CellType)(eCellTypeImprovedSalad);
//
//  this->blue.health = 301;
//  this->blue.sleepTime = 1;
//  this->blue.attack = 20;
//  this->blue.passiveHealthChunkMin = 0;
//  this->blue.passiveHealthChunkMax = 6;
//  this->blue.armor = 30;
//  this->blue.lifeTime = 830;
//  this->blue.percentOfMutations = 0.0;
//  this->blue.healthPerAttack = 150;
//  this->blue.food = (CellType)(eCellTypeHunter);
//  this->blue.danger = (CellType)(eCellTypeUnknown);
//  this->blue.friends = (CellType)(eCellTypeBigBlue);
// 
//  this->percentOfCreatures = 0.1;
//  this->percentOfOrange = 0.05;
//  this->percentOfGreen = 0.2;
//  this->percentOfSalad = 0.4;
//  this->percentOfCyan = 0.4;
//  this->percentOfBlue = 0.2;
  
  this->terrainSize = 8;
  this->mapWidth = 500;
  this->mapHeight = 500;
  
  this->green.health = 301;
  this->green.sleepTime = 2;
  this->green.attack = 18;
  this->green.passiveHealthChunkMin = -1;
  this->green.passiveHealthChunkMax = -1;
  this->green.armor = 10;
  this->green.lifeTime = 830;
  this->green.percentOfMutations = 0.0;
  this->green.healthPerAttack = 90;
  this->green.food = (CellType)(eCellTypeImprovedSalad | eCellTypeSalad);
  this->green.danger = (CellType)(eCellTypeHunter);
  this->green.friends = eCellTypeGreen;
  
  this->orange.health = 1400;
  this->orange.sleepTime = 0;
  this->orange.attack = 15;
  this->orange.passiveHealthChunkMin = -1;
  this->orange.passiveHealthChunkMax = -1;
  this->orange.armor = 300;
  this->orange.lifeTime = 9530;
  this->orange.percentOfMutations = 0.0;
  this->orange.healthPerAttack = 66;
  this->orange.food = (CellType)(eCellTypeGreen);
  this->orange.danger = (CellType)(eCellTypeUnknown);
  this->orange.friends = (CellType)(eCellTypeHunter);
  
  this->salad.health = 301;
  this->salad.sleepTime = 30;
  this->salad.attack = 100;
  this->salad.passiveHealthChunkMin = -10;
  this->salad.passiveHealthChunkMax = 16;
  this->salad.armor = 21;
  this->salad.lifeTime = 830;
  this->salad.percentOfMutations = 0.0;
  this->salad.healthPerAttack = 301;
  this->salad.food = (CellType)(eCellTypeUnknown);
  this->salad.danger = (CellType)(eCellTypeUnknown);
  this->salad.friends = (CellType)(eCellTypeUnknown);
  
  this->cyan.health = 301;
  this->cyan.sleepTime = 0;
  this->cyan.attack = 6;
  this->cyan.passiveHealthChunkMin = -10;
  this->cyan.passiveHealthChunkMax = 14;
  this->cyan.armor = 30;
  this->cyan.lifeTime = 830;
  this->cyan.percentOfMutations = 0.0;
  this->cyan.healthPerAttack = 140;
  this->cyan.food = (CellType)(eCellTypeBigBlue);
  this->cyan.danger = (CellType)(eCellTypeGreen);
  this->cyan.friends = (CellType)(eCellTypeImprovedSalad);
  
  this->blue.health = 501;
  this->blue.sleepTime = 5;
  this->blue.attack = 100;
  this->blue.passiveHealthChunkMin = -2;
  this->blue.passiveHealthChunkMax = 1;
  this->blue.armor = 110;
  this->blue.lifeTime = 9830;
  this->blue.percentOfMutations = 0.0;
  this->blue.healthPerAttack = 50;
  this->blue.food = (CellType)(eCellTypeHunter | eCellTypeGreen | eCellTypeImprovedSalad | eCellTypeSalad);
  this->blue.danger = (CellType)eCellTypeImprovedSalad;
  this->blue.friends = (CellType)(eCellTypeBigBlue);
  
  this->percentOfCreatures = 0.1;
  this->percentOfOrange = 0.05;
  this->percentOfGreen = 0.2;
  this->percentOfSalad = 0.4;
  this->percentOfCyan = 0.4;
  this->percentOfBlue = 0.2;
  
}

void PixelDescriptorProvider::InitWithConfig(Config* config)
{
  m_config = config;
  
  if (m_config->terrainSize <= 0 || m_config->terrainSize >= m_config->mapHeight || m_config->terrainSize >= m_config->mapWidth)
    m_config->terrainSize = 1;
  
  if ( m_config->mapHeight <= 3)
     m_config->mapHeight = 3;
  
  if ( m_config->mapWidth <= 3)
     m_config->mapWidth = 3;
  
  if (m_config->mapHeight > 1000)
    m_config->mapHeight = 1000;
  
  if (m_config->mapWidth > 1000)
    m_config->mapWidth = 1000;
  
  mapWidth = m_config->mapWidth;
  mapHeight = m_config->mapHeight;

  Init();
}
  
  
bool CreateCell(PixelDescriptor* pd, CellType creatureType, PixelDescriptorProvider::Config::CellConfig* cellConfig)
{
  if (pd == nullptr || cellConfig == nullptr)
  {
    return false;
  }
  
  std::shared_ptr<IShape> shape;
  if (creatureType == eCellTypeBigBlue)
  {
    shape = std::make_shared<BigCell>(pd);
  }
  else
  {
    shape = std::make_shared<SingleCell>(pd);
  }
 
  bool empty = true;
  shape->ForEach([&empty](PixelDescriptor* pixel, bool& stop){
    
    if (pixel == nullptr || pixel->m_type != PixelDescriptor::Empty)
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
    return false;
  }
  
  CellDescriptor* cd = new CellDescriptor(pd);
  cd->m_shape = shape;
 
  pd->m_type = PixelDescriptor::CreatureType;
  cd->m_character = creatureType;
  cd->m_id = nextId++;
  
  cd->m_health = cd->m_baseHealth = cellConfig->health;
  cd->m_armor = cd->m_baseArmor = cellConfig->armor;
  cd->m_damage = cellConfig->attack;
  cd->m_age = cRandABInt(0, 50);
  cd->m_sleepTime = cellConfig->sleepTime;
  cd->m_sleepCounter =  cRandABInt(0, cd->m_sleepTime);
  cd->m_lifeTime = cellConfig->lifeTime;
  cd->m_friendMask = cellConfig->friends;
  cd->m_foodMask = cellConfig->food;
  cd->m_dangerMask = cellConfig->danger;
  cd->m_skipFirstStep = true;
  
  cd->Finish();
  
  return true;
}

PixelDescriptorProvider::PixelPtr PixelDescriptorProvider::CreateCell(CellType creatureType, const komorki::Vec2& pos)
{
  return nullptr;
}

void PixelDescriptorProvider::Init()
{
  m_updateId = 0;
  nextId = 0;
  m_map.reserve(m_config->mapWidth);
  m_typeMap.reserve(m_config->mapWidth);
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    m_map.push_back(std::vector<PixelPtr>());
    m_map[i].reserve(m_config->mapHeight);
    m_typeMap.push_back(std::vector<PixelDescriptor::Type>());
    m_typeMap[i].reserve(m_config->mapHeight);
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      m_map[i].push_back(std::make_shared<PixelDescriptor>(i, j));
    }
  }
  
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      PixelDescriptor* pd = m_map[i][j].get();
      
      for (int di = -1; di <= 1; ++di)
      {
        for (int dj = -1; dj <= 1; ++dj)
        {
          if (i + di >= m_config->mapWidth || i + di < 0)
          {
            continue;
          }
          
          if (j + dj >= m_config->mapHeight || j + dj < 0)
          {
            continue;
          }
          
          if (di == 0 && dj == 0) {
            continue;
          }
          
          PixelDescriptor* aroundPd = m_map[i + di][j + dj].get();
          pd->SetDirection(di, dj, aroundPd);
        }
      }
    }
  }
  
  komorki::DiamondSquareGenerator gen(512, 512, 60.f, -0.5, false);
  gen.Generate(nullptr);
  
  auto buffer = gen.GetBuffer(0,0, m_config->mapWidth, m_config->mapHeight);
  auto analizer = std::shared_ptr<TerrainAnalizer>(new TerrainAnalizer(buffer));
  auto analizedBuffer = analizer->GetLevels();

  analizedBuffer->ForEach([&](const int& x, const int& y, const TerrainLevel& level)
                          {
                            if (x%2 != 0 && y%2 != 0) {
                              return ;
                            }
                            if (level >= TerrainLevel::Ground)
                            {
                              m_map[x/2][y/2]->m_type = PixelDescriptor::TerrainType;
                            }
                          });
  
  m_terrain = analizer->GetResult();
  
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    m_map[i][0]->m_type = (PixelDescriptor::TerrainType);
  }
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    m_map[i][m_config->mapHeight - 1]->m_type = (PixelDescriptor::TerrainType);
  }
  for (int i = 0; i < m_config->mapHeight; ++i)
  {
    m_map[0][i]->m_type = (PixelDescriptor::TerrainType);
  }
  for (int i = 0; i < m_config->mapHeight; ++i)
  {
    m_map[m_config->mapWidth - 1][i]->m_type = (PixelDescriptor::TerrainType);
  }

  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      PixelDescriptor* pd = m_map[i][j].get();
      PixelDescriptor::Type type = cBoolRandPercent(m_config->percentOfCreatures) ? PixelDescriptor::CreatureType : PixelDescriptor::Empty;
      if (type == PixelDescriptor::CreatureType)
      {
        CellType creatureType = eCellTypeUnknown;
        if (cBoolRandPercent(m_config->percentOfGreen))
          creatureType = eCellTypeGreen;
        else if (cBoolRandPercent(m_config->percentOfOrange))
          creatureType = eCellTypeHunter;
        else if (cBoolRandPercent(m_config->percentOfSalad))
          creatureType = eCellTypeSalad;
        else if (cBoolRandPercent(m_config->percentOfCyan))
          creatureType = eCellTypeImprovedSalad;
        else if (cBoolRandPercent(m_config->percentOfBlue))
          creatureType = eCellTypeBigBlue;
       
        static int count = 0;
        if (creatureType == eCellTypeUnknown)
        {
          continue;
        }
        
        if (count == 1) {
          continue;
        }
        
        if(komorki::CreateCell(pd, creatureType, m_config->ConfigForCell(creatureType)))
        {
//          count++;
        }
      }
    }
  }
}
  
PixelDescriptorProvider::Config::CellConfig* PixelDescriptorProvider::Config::ConfigForCell(CellType type)
{
  if (type == CellType::eCellTypeGreen)
  {
    return &green;
  }
  if (type == CellType::eCellTypeHunter)
  {
    return &orange;
  }
  if (type == CellType::eCellTypeSalad)
  {
    return &salad;
  }
  if (type == CellType::eCellTypeImprovedSalad)
  {
    return &cyan;
  }
  if (type == CellType::eCellTypeBigBlue)
  {
    return &blue;
  }
  
  assert(0);
  return nullptr;
}

PixelDescriptor* PixelDescriptorProvider::GetDescriptor(PixelPos x, PixelPos y) const
{
  if (x < 0 || x >= m_config->mapWidth)
  {
    return nullptr;
  }
  
  if (y < 0 || y >= m_config->mapHeight)
  {
    return nullptr;
  }
  
  return m_map[x][y].get();
}
  
TerrainAnalizer::Result PixelDescriptorProvider::GetTerrain() const
  {
    return m_terrain;
  }

Vec2 PixelDescriptorProvider::GetSize() const
{
  return Vec2(m_config->mapWidth, m_config->mapHeight);
}

bool PixelDescriptorProvider::CheckBounds(int x, int y)
{
  if(x < 0 || x >= m_config->mapWidth || y < 0 || y >= m_config->mapHeight)
    return false;
  
  return true;
}

void PixelDescriptorProvider::ProccessTransaction(bool passUpdateResult, std::list<UpdateResult>& result)
{
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      auto pd = m_map[i][j].get();
      if (pd->m_type != PixelDescriptor::CreatureType)
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
      
      bool shouldBeRemoved = d->m_age > d->m_lifeTime;
      
      if(d->m_armor <= 0 || d->m_health <= 0 || shouldBeRemoved)
      {
        d->Shape([](PixelDescriptor* pd, bool& stop){
          pd->m_cellDescriptor = nullptr;
          pd->m_type = PixelDescriptor::Empty;
        });
        
        if (passUpdateResult)
        {
          UpdateResult r(d);
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
      
      int maxHealth = d->m_baseHealth;
      if(d->m_health >= maxHealth * 2)
      {
        auto destinationDesc = ProcessMutation(d);
        if (destinationDesc)
        {
          d->m_health = d->m_baseHealth;
          
          if (passUpdateResult)
          {
            UpdateResult r(d);
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
  
CellType GetRanromCellType()
{
  int randIntType = cRandABInt(0, eCellTypeNumberOfTypes);
  if (randIntType == 0) return eCellTypeGreen;
  if (randIntType == 1) return eCellTypeSalad;
  if (randIntType == 2) return eCellTypeHunter;
  if (randIntType == 3) return eCellTypeImprovedSalad;
  if (randIntType == 4) return eCellTypeBigBlue;
  assert(0);
  return eCellTypeUnknown;
}

CellDescriptor* PixelDescriptorProvider::ProcessMutation(CellDescriptor* source)
{
  CellDescriptor* result = nullptr;
  
  auto sourceConfig = m_config->ConfigForCell(source->m_character);
  
  CellType newType = source->m_character;
  if (cBoolRandPercent(sourceConfig->percentOfMutations))
  {
    newType = GetRanromCellType();
  }
  
  auto destConfig = m_config->ConfigForCell(newType);
  
  source->AroundRandom([&result, newType, destConfig](PixelDescriptor* pd, bool& stop){
    
    if (komorki::CreateCell(pd, newType, destConfig)) {
      stop = true;
      result = pd->m_cellDescriptor;
    }
    
  });
  
  return result;
}

void PixelDescriptorProvider::Update(bool passUpdateResult, std::list<PixelDescriptorProvider::UpdateResult>& result)
{
//  if (m_updateId == 4)
//  {
//    return;
//  }
 
  #if COLLECT_POPULATION_METRICS
    m_population.clear();
  #endif
  
  ProccessTransaction(passUpdateResult, result);

  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      m_typeMap[i][j] = line[j].get()->m_type;
    }
  }

  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    const std::vector<PixelDescriptor::Type>& typeMapLine = m_typeMap[i];
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      auto oldPd = typeMapLine[j];
      
      if (oldPd == PixelDescriptor::CreatureType)
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
            UpdateResult r(d);
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

        d->Check();
        
        komorki::ProcessCell(d,
                        Vec2(i, j),
                        m_config,
                        m_map,
                        m,
                        a);
        
        d->Check();
       
        if (m == false && a == false)
          continue;
        
        UpdateResult updateResult(d);
        updateResult.desc = pd.get();
        updateResult.action = a;
        updateResult.movement = m;

        if (passUpdateResult)
        {
          result.push_back(updateResult);
        }
      }
    }
  }
  
  ++m_updateId;
}
  
void PixelDescriptorProvider::SetCreatureType(const Vec2& pos, CellType type)
{
//  if (!CheckBounds(pos.x, pos.y))
//  {
//    return;
//  }
//  
//  {
//  auto d = m_map[pos.x][pos.y];
//  if (d->m_type == PixelDescriptor::Empty)
//  {
//    auto newPd = CreateCell(type);
//    newPd->m_new = true;
//    d->Copy(newPd.get());
//  }
//  }
//  {
//    auto d = m_map[pos.x + 1][pos.y + 1];
//    if (d->m_type == PixelDescriptor::Empty)
//    {
//      auto newPd = CreateCell(type);
//      newPd->m_new = true;
//      d->Copy(newPd.get());
//    }
//  }
//  {
//    auto d = m_map[pos.x + 1][pos.y];
//    if (d->m_type == PixelDescriptor::Empty)
//    {
//      auto newPd = CreateCell(type);
//      newPd->m_new = true;
//      d->Copy(newPd.get());
//    }
//  }
//  {
//    auto d = m_map[pos.x][pos.y + 1];
//    if (d->m_type == PixelDescriptor::Empty)
//    {
//      auto newPd = CreateCell(type);
//      newPd->m_new = true;
//      d->Copy(newPd.get());
//    }
//  }
}
  
void PixelDescriptorProvider::KillAllCells()
{
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      auto pd = line[j];
      if (pd->m_type == PixelDescriptor::CreatureType)
      {
        auto d = pd->m_cellDescriptor;
        Transaction t;
        t.m_health = INT32_MIN/2;
        d->nextTurnTransaction.push_back(t);
      }
    }
  }
}
  
void PixelDescriptorProvider::KillCellAtPostiion(const Vec2& pos)
{
  if (!CheckBounds(pos.x, pos.y))
  {
    return;
  }
  
  auto pd = m_map[pos.x][pos.y];
  if (pd->m_type != PixelDescriptor::Empty)
  {
    auto d = pd->m_cellDescriptor;
    Transaction t;
    t.m_health = INT32_MIN/2;
    d->nextTurnTransaction.push_back(t);
  }
}
  
}// namespace komorki;
