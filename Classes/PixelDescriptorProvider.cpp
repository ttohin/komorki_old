#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include <math.h>
#include "CellsLogic.h"

#define COLLECT_POPULATION_METRICS 0

namespace komorki
{

int mapWidth = 0;
int mapHeight = 0;
  
int nextId = 0;

PixelDescriptorProvider::Config::Config()
{
  this->terrainSize = 8;
  this->mapWidth = 150;
  this->mapHeight = 100;

  this->green.health = 301;
  this->green.sleepTime = 1;
  this->green.attack = 14;
  this->green.passiveHealthChunkMin = -1;
  this->green.passiveHealthChunkMax = -1;
  this->green.armor = 10;
  this->green.lifeTime = 830;
  this->green.percentOfMutations = 0.0;
  this->green.healthPerAttack = 90;
  
  this->orange.health = 1400;
  this->orange.sleepTime = 0;
  this->orange.attack = 15;
  this->orange.passiveHealthChunkMin = -1;
  this->orange.passiveHealthChunkMax = -1;
  this->orange.armor = 1;
  this->orange.lifeTime = 1530;
  this->orange.percentOfMutations = 0.0;
  this->orange.healthPerAttack = 66;
  
  this->salad.health = 301;
  this->salad.sleepTime = 30;
  this->salad.attack = 0;
  this->salad.passiveHealthChunkMin = -10;
  this->salad.passiveHealthChunkMax = 16;
  this->salad.armor = 21;
  this->salad.lifeTime = 830;
  this->salad.percentOfMutations = 0.0;
  this->salad.healthPerAttack = 0;
  
  this->cyan.health = 301;
  this->cyan.sleepTime = 0;
  this->cyan.attack = 0;
  this->cyan.passiveHealthChunkMin = -10;
  this->cyan.passiveHealthChunkMax = 14;
  this->cyan.armor = 30;
  this->cyan.lifeTime = 830;
  this->cyan.percentOfMutations = 0.0;
  this->cyan.healthPerAttack = 0;
  
  this->percentOfCreatures = 1.0;
  this->percentOfOrange = 0.05;
  this->percentOfGreen = 0.2;
  this->percentOfSalad = 0.4;
  this->percentOfCyan = 0.4;
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
  
void CreateCell(PixelDescriptor* pd, CellType creatureType, PixelDescriptorProvider::Config::CellConfig* cellConfig)
{
  if (pd == nullptr)
  {
    return;
  }
  
  pd->SetType(IPixelDescriptor::CreatureType);
  pd->m_character = creatureType;
  pd->m_id = nextId++;
  
  pd->m_health = pd->m_baseHealth = cellConfig->health;
  pd->m_armor = pd->m_baseArmor = cellConfig->armor;
  pd->m_damage = cellConfig->attack;
  pd->m_age = cRandABInt(0, 50);
  pd->m_sleepTime = cellConfig->sleepTime;
  pd->m_sleepCounter =  cRandABInt(0, pd->m_sleepTime);
  pd->m_lifeTime = cellConfig->lifeTime;
}

PixelDescriptorProvider::PixelPtr PixelDescriptorProvider::CreateCell(CellType creatureType)
{
  auto pd = std::make_shared<PixelDescriptor>(IPixelDescriptor::CreatureType);
  
  komorki::CreateCell(pd.get(), creatureType, m_config->ConfigForCell(creatureType));

  return pd;
}

void PixelDescriptorProvider::Init()
{
  nextId = 0;
  m_map.reserve(m_config->mapWidth);
  m_typeMap.reserve(m_config->mapWidth);
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    m_map.push_back(std::vector<PixelPtr>());
    m_map[i].reserve(m_config->mapHeight);
    m_typeMap.push_back(std::vector<IPixelDescriptor::Type>());
    m_typeMap[i].reserve(m_config->mapHeight);
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      IPixelDescriptor::Type type = cBoolRandPercent(m_config->percentOfCreatures) ? IPixelDescriptor::CreatureType : IPixelDescriptor::Empty;
      if (type == IPixelDescriptor::CreatureType)
      {
        CellType creatureType = eCellTypeEnd;
        if (cBoolRandPercent(m_config->percentOfGreen))
          creatureType = eCellTypeGreen;
        else if (cBoolRandPercent(m_config->percentOfOrange))
          creatureType = eCellTypeHunter;
        else if (cBoolRandPercent(m_config->percentOfSalad))
          creatureType = eCellTypeSalad;
        else if (cBoolRandPercent(m_config->percentOfCyan))
          creatureType = eCellTypeImprovedSalad;
        
        if (creatureType == eCellTypeEnd)
        {
          m_map[i].push_back(std::make_shared<PixelDescriptor>(IPixelDescriptor::Empty));
          continue;
        }

        auto pd = CreateCell(creatureType);
        
        m_map[i].push_back(pd);
      }
      else
      {
        m_map[i].push_back(std::make_shared<PixelDescriptor>(type));
      }
      m_typeMap[i].push_back(type);
    }
  }
  
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    int numberOfTerrains = cRandABInt(1, m_config->terrainSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[i][m_config->mapHeight - j - 1]->SetType(IPixelDescriptor::TerrainType);
    }
  }
  for (int i = 0; i < m_config->mapHeight; ++i)
  {
    int numberOfTerrains = cRandABInt(1, m_config->terrainSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[m_config->mapWidth - j - 1][i]->SetType(IPixelDescriptor::TerrainType);
    }
  }
  for (int i = 0; i < m_config->mapHeight; ++i)
  {
    int numberOfTerrains = cRandABInt(1, m_config->terrainSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[j][i]->SetType(IPixelDescriptor::TerrainType);
    }
  }
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    int numberOfTerrains = cRandABInt(1, m_config->terrainSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[i][j]->SetType(IPixelDescriptor::TerrainType);
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
  
  assert(0);
  return nullptr;
}

IPixelDescriptor* PixelDescriptorProvider::GetDescriptor(PixelPos x, PixelPos y) const
{
  return m_map[x][y].get();
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
      auto d = m_map[i][j].get();
      if (d->GetType() != IPixelDescriptor::CreatureType)
      {
        continue;
      }
      
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
        d->SetType(IPixelDescriptor::Empty);
        
        if (passUpdateResult)
        {
          UpdateResult r(d);
          r.source = Vec2(i, j);
          r.shouldDelete = true;
          result.push_back(r);
        }
        
        continue;
      }
      
      int maxHealth = d->m_baseHealth;
      if(d->m_health >= maxHealth * 2)
      {
        Vec2 newPdPos;
        
        PixelDescriptor* destinationDesc = nullptr;
        
        const int xStart = rand()%2 == 0 ? 1 : -1;
        const int yStart = rand()%2 == 0 ? 1 : -1;
        
        for (int di = xStart; di != -2*xStart; di += -xStart)
        {
          if (destinationDesc != nullptr)
            break;
          
          for (int dj = yStart; dj != -2*yStart; dj += -yStart)
          {
            if (destinationDesc != nullptr)
              break;
            
            int dx = i + di;
            int dy = j + dj;
            
            if(!CheckBounds(dx, dy))
              continue;
            
            auto targetPd = m_map[dx][dy];
            if(targetPd->GetType() == IPixelDescriptor::Empty)
            {
              newPdPos = Vec2(dx, dy);
              destinationDesc = targetPd.get();
            } // if(targetPd->GetType() == IPixelDescriptor::Empty)
          } // for (int dj = yStart; dj != -2*yStart; dj += -yStart)
        } // for (int di = xStart; di != -2*xStart; di += -xStart)
        
        if (destinationDesc != nullptr)
        {
          d->m_health = d->m_baseHealth;
          ProcessMutation(d, destinationDesc);
          
          if (passUpdateResult)
          {
            UpdateResult r(destinationDesc);
            r.source = Vec2(i, j);
            
            r.addCreature.SetValue(true);
            r.addCreature.value.to = newPdPos;
            r.addCreature.value.destinationDesc = destinationDesc;
            result.push_back(r);
          } // if (passUpdateResult)
        } // (destinationDesc != nullptr)
      } // if(d->m_health >= maxHealth * 2)
    }
  }
}

void PixelDescriptorProvider::ProcessMutation(PixelDescriptor* source, PixelDescriptor* destination)
{
  auto sourceConfig = m_config->ConfigForCell(source->m_character);
  
  destination->Copy(source);
  destination->SetType(IPixelDescriptor::CreatureType);
  destination->nextTurnTransaction.clear();
  destination->m_health = source->m_baseHealth;
  destination->m_id = nextId++;
  destination->m_skipFirstStep = true;
  destination->m_age = 0;
  
  if (cBoolRandPercent(sourceConfig->percentOfMutations))
  {
    CellType newType = (CellType)cRandABInt(eCellTypeStart, eCellTypeEnd);
    if (newType != destination->m_character)
    {
      auto destConfig = m_config->ConfigForCell(newType);
      komorki::CreateCell(destination, newType, destConfig);
    }
  }
}

void PixelDescriptorProvider::Update(bool passUpdateResult, std::list<IPixelDescriptorProvider::UpdateResult>& result)
{
  #if COLLECT_POPULATION_METRICS
    m_population.clear();
  #endif
  
  ProccessTransaction(passUpdateResult, result);

  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      m_typeMap[i][j] = line[j].get()->GetType();
    }
  }

  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    const std::vector<IPixelDescriptor::Type>& typeMapLine = m_typeMap[i];
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      auto oldPd = typeMapLine[j];
      
      if (oldPd == IPixelDescriptor::CreatureType)
      {
        auto d = line[j];
        
#if COLLECT_POPULATION_METRICS
        m_population[d->m_character]=m_population[d->m_character]+1;
#endif
        if (d->m_new)
        {
          d->m_new = false;
          
          if (passUpdateResult)
          {
            UpdateResult r(d.get());
            r.source = Vec2(i, j);
            r.addCreature.SetValue(true);
            r.addCreature.value.to = Vec2(i, j);
            r.addCreature.value.destinationDesc = d.get();
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

        komorki::ProcessCell(d.get(),
                        Vec2(i, j),
                        m_config,
                        m_map,
                        m,
                        a);
       
        if (m == false && a == false)
          continue;
        
        UpdateResult updateResult(d.get());
        updateResult.action = a;
        updateResult.movement = m;
        updateResult.source = Vec2(i, j);
        
        if(m == true)
        {
          auto destinationDesc = m_map[m.value.destination.x][m.value.destination.y];
          destinationDesc->Copy(d.get());
          d->SetType(IPixelDescriptor::Empty);
          d->nextTurnTransaction.clear();
          updateResult.movement.value.destinationDesc = destinationDesc.get();
        }

        if (passUpdateResult)
        {
          result.push_back(updateResult);
        }
      }
    }
  }
}
  
void PixelDescriptorProvider::SetCreatureType(const Vec2& pos, CellType type)
{
  if (!CheckBounds(pos.x, pos.y))
  {
    return;
  }
  
  {
  auto d = m_map[pos.x][pos.y];
  if (d->GetType() == IPixelDescriptor::Empty)
  {
    auto newPd = CreateCell(type);
    newPd->m_new = true;
    d->Copy(newPd.get());
  }
  }
  {
    auto d = m_map[pos.x + 1][pos.y + 1];
    if (d->GetType() == IPixelDescriptor::Empty)
    {
      auto newPd = CreateCell(type);
      newPd->m_new = true;
      d->Copy(newPd.get());
    }
  }
  {
    auto d = m_map[pos.x + 1][pos.y];
    if (d->GetType() == IPixelDescriptor::Empty)
    {
      auto newPd = CreateCell(type);
      newPd->m_new = true;
      d->Copy(newPd.get());
    }
  }
  {
    auto d = m_map[pos.x][pos.y + 1];
    if (d->GetType() == IPixelDescriptor::Empty)
    {
      auto newPd = CreateCell(type);
      newPd->m_new = true;
      d->Copy(newPd.get());
    }
  }
}
  
void PixelDescriptorProvider::KillAllCells()
{
  for (int i = 0; i < m_config->mapWidth; ++i)
  {
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < m_config->mapHeight; ++j)
    {
      auto d = line[j];
      if (d->GetType() == IPixelDescriptor::CreatureType)
      {
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
  
  auto d = m_map[pos.x][pos.y];
  if (d->GetType() != IPixelDescriptor::Empty)
  {
    Transaction t;
    t.m_health = INT32_MIN/2;
    d->nextTurnTransaction.push_back(t);
  }
}
  
}// namespace komorki;
