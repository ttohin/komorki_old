#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include <math.h>
#include "CellsParameters.h"
#include "CellsLogic.h"

#define COLLECT_POPULATION_METRICS 0

int kGroundSize = 8;
int kMapHeight = 500;
int kMapWidth = 500;

int kGreenHealth = 301;
int kHunterHealth = 1400;
int kBaseHealth = 200;

int kHunterHealthIncome = 55;
int kHunterAttack = 15;
int kHunterHungryDamage = -1;

int kGreenHealthIncome = 66;
int kGreenAttack = 14;
int kGreenHungryDamage = -1;
int kGreenSleepTime = 1;

float kPercentOfCreature = .1;
float kPercentOfHunters = 0.1;
float kPercentOfGreen = 0.5;
float kPercentOfSalat = 0.4;
int kSalatIncomeMin = -10;
int kSalatIncomeMax = 16;
int kImprovedSalatIncomeMin = -10;
int kImprovedSalatIncomeMax = 14;
int kImprovedSalatArmor = 30;

int kSalatArmor = 21;
int kSalatSleepTime = 30;

int kBaseArmor = 10;

int kMaxLifeTime = 830;
int kHunterLifeTime = 1530;
float kPercentOfMutations = 0.0;

namespace komorki
{

int nextId = 0;

PixelDescriptorProvider::Config::Config()
{
  this->terrainSize = kGroundSize;
  this->mapWidth = kMapWidth;
  this->mapHeight = kMapHeight;
  
  this->greenHealth = kGreenHealth;
  this->hunterHealth = kHunterHealth;
  this->baseHealth = kBaseHealth;
  this->hunterHealthIncome = kHunterHealthIncome;
  this->hunterAttack = kHunterAttack;
  this->greenHealthIncome = kGreenHealthIncome;
  this->greenAttack = kGreenAttack;
  this->salatArmor = kSalatArmor;
  this->maxLifeTime = kMaxLifeTime;
  
  this->hunterHungryDamage = kHunterHungryDamage;
  
  this->greenHugryDamage = kGreenHungryDamage;
  this->greenSleepTime = kGreenSleepTime;
  
  this->percentOfCreatures = kPercentOfCreature;
  this->percentOfHunters = kPercentOfHunters;
  this->percentOfGreen = kPercentOfGreen;
  this->percentOfSalat = kPercentOfSalat;
  
  this->salatIncomeMin = kSalatIncomeMin;
  this->salatIncomeMax = kSalatIncomeMax;
  this->imporvedSalatIncomeMin = kImprovedSalatIncomeMin;
  this->improvedSalatIncomeMax = kImprovedSalatIncomeMax;
  this->improvedSalatArmor = kImprovedSalatArmor;
  
  this->salatSleepTime = kSalatSleepTime;
  this->percentOfMutations = kPercentOfMutations;
  this->hunterLifeTime = kHunterLifeTime;
}

void PixelDescriptorProvider::InitWithConfig(PixelDescriptorProvider::Config *config)
{
  kGroundSize = config->terrainSize;
  kMapWidth = config->mapWidth;
  kMapHeight = config->mapHeight;
  
  kGreenHealth = config->greenHealth;
  kHunterHealth = config->hunterHealth;
  kBaseHealth = config->baseHealth;
  kHunterHealthIncome = config->hunterHealthIncome;
  kHunterAttack = config->hunterAttack;
  kGreenHealthIncome = config->greenHealthIncome;
  kGreenAttack = config->greenAttack;
  kSalatArmor = config->salatArmor;
  kMaxLifeTime = config->maxLifeTime;

  kHunterHungryDamage = config->hunterHungryDamage;
  
  kGreenHungryDamage = config->greenHugryDamage;
  kGreenSleepTime = config->greenSleepTime;
  
  kPercentOfCreature = config->percentOfCreatures;
  kPercentOfHunters = config->percentOfHunters;
  kPercentOfGreen = config->percentOfGreen;
  kPercentOfSalat = config->percentOfSalat;
  
  kSalatIncomeMin = config->salatIncomeMin;
  kSalatIncomeMax = config->salatIncomeMax;
  kImprovedSalatIncomeMin = config->imporvedSalatIncomeMin;
  kImprovedSalatIncomeMax = config->improvedSalatIncomeMax;
  kImprovedSalatArmor = config->improvedSalatArmor;
  
  kSalatSleepTime = config->salatSleepTime;
  kPercentOfMutations = config->percentOfMutations;
  kHunterLifeTime = config->hunterLifeTime;
  
  kBaseArmor = config->baseArmor;
  
  if (kGroundSize <= 0 || kGroundSize >= kMapHeight || kGroundSize >= kMapWidth)
    kGroundSize = 1;
  
  if (kMapHeight <= 3 || kMapHeight > 1000)
    kMapHeight = 3;
  
  if (kMapWidth <= 3 || kMapWidth > 1000)
    kMapWidth = 3;
  
  if (kMapHeight > 1000)
    kMapHeight = 1000;
  
  if (kMapWidth > 1000)
    kMapWidth = 1000;

  InitWithDefaults();
}
  
void CreateCell(PixelDescriptor* pd, CellType creatureType)
{
  if (pd == nullptr)
  {
    return;
  }
  
  pd->SetType(IPixelDescriptor::CreatureType);
  pd->m_character = creatureType;
  pd->m_id = nextId++;
  
  pd->m_health = pd->m_baseHealth = kBaseHealth;
  pd->m_armor = pd->m_baseArmor = kBaseArmor;
  pd->m_damage = 0;
  pd->m_age = cRandABInt(0, 50);
  pd->m_sleepCounter = 0;
  pd->m_lifeTime = kMaxLifeTime;
  
  if (creatureType == eCellTypeHunter)
  {
    pd->m_health = pd->m_baseHealth = kHunterHealth;
    pd->m_damage = kHunterAttack;
    pd->m_lifeTime = kHunterLifeTime;
  }
  if (creatureType == eCellTypeGreen)
  {
    pd->m_health = pd->m_baseHealth = kGreenHealth;
    pd->m_damage = kGreenAttack;
    pd->m_sleepCounter = pd->m_sleepTime = kGreenSleepTime;
  }
  if (creatureType == eCellTypeSalat)
  {
    pd->m_armor = pd->m_baseArmor = kSalatArmor;
    pd->m_sleepCounter = pd->m_sleepTime = kSalatSleepTime;
  }
  if (creatureType == eCellTypeImprovedSalat)
  {
    pd->m_armor = pd->m_baseArmor = kImprovedSalatArmor;
  }
}

PixelDescriptorProvider::PixelPtr PixelDescriptorProvider::CreateCell(CellType creatureType)
{
  auto pd = std::make_shared<PixelDescriptor>(IPixelDescriptor::CreatureType);
  komorki::CreateCell(pd.get(), creatureType);
  return pd;
}

void PixelDescriptorProvider::InitWithDefaults()
{
  nextId = 0;
  m_map.reserve(kMapWidth);
  m_typeMap.reserve(kMapWidth);
  for (int i = 0; i < kMapWidth; ++i)
  {
    m_map.push_back(std::vector<PixelPtr>());
    m_map[i].reserve(kMapHeight);
    m_typeMap.push_back(std::vector<IPixelDescriptor::Type>());
    m_typeMap[i].reserve(kMapHeight);
    for (int j = 0; j < kMapHeight; ++j)
    {
      IPixelDescriptor::Type type = cBoolRandPercent(kPercentOfCreature) ? IPixelDescriptor::CreatureType : IPixelDescriptor::Empty;
      if (type == IPixelDescriptor::CreatureType)
      {
        CellType creatureType = cBoolRandPercent(kPercentOfHunters) ? eCellTypeHunter :
        cBoolRandPercent(kPercentOfGreen) ? eCellTypeGreen :
        cBoolRandPercent(kPercentOfSalat) ? eCellTypeSalat : eCellTypeImprovedSalat;
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
  
  for (int i = 0; i < kMapWidth; ++i)
  {
    int numberOfTerrains = cRandABInt(1, kGroundSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[i][kMapHeight - j - 1]->SetType(IPixelDescriptor::TerrainType);
    }
  }
  for (int i = 0; i < kMapHeight; ++i)
  {
    int numberOfTerrains = cRandABInt(1, kGroundSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[kMapWidth - j - 1][i]->SetType(IPixelDescriptor::TerrainType);
    }
  }
  for (int i = 0; i < kMapHeight; ++i)
  {
    int numberOfTerrains = cRandABInt(1, kGroundSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[j][i]->SetType(IPixelDescriptor::TerrainType);
    }
  }
  for (int i = 0; i < kMapWidth; ++i)
  {
    int numberOfTerrains = cRandABInt(1, kGroundSize);
    for (int j = 0; j < numberOfTerrains; ++j)
    {
      m_map[i][j]->SetType(IPixelDescriptor::TerrainType);
    }
  }
}

IPixelDescriptor* PixelDescriptorProvider::GetDescriptor(PixelPos x, PixelPos y) const
{
  return m_map[x][y].get();
}

Vec2 PixelDescriptorProvider::GetSize() const
{
  return Vec2(kMapWidth, kMapHeight);
}

bool PixelDescriptorProvider::CheckBounds(int x, int y)
{
  if(x < 0 || x >= kMapWidth || y < 0 || y >= kMapHeight)
    return false;
  
  return true;
}

void PixelDescriptorProvider::ProccessTransaction(bool passUpdateResult, std::list<UpdateResult>& result)
{
  for (int i = 0; i < kMapWidth; ++i)
  {
    for (int j = 0; j < kMapHeight; ++j)
    {
      auto d = m_map[i][j].get();
      if (d->GetType() == IPixelDescriptor::CreatureType)
      {
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
              }
            }
          }

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
            }
          }
        }

        if (d->m_age > d->m_lifeTime)
        {
          if (cBoolRandPercent(1.0f - kPercentOfMutations))
            continue;
          
          d->m_age = 0;
          d->m_skipFirstStep = true;
          bool isHunter = cBoolRandPercent(0.1);
          d->m_character = isHunter ? eCellTypeHunter : cRandABInt(eCellTypeGreen, eCellTypeHunter);
          
          if (passUpdateResult)
          {
            UpdateResult r1(d);
            r1.source = Vec2(i, j);
            r1.shouldDelete = true;
            result.push_back(r1);
            
            UpdateResult r2(d);
            r2.source = Vec2(i, j);
            
            r2.addCreature.SetValue(true);
            r2.addCreature.value.to = Vec2(i, j);
            r2.addCreature.value.destinationDesc = d;
            result.push_back(r2);
          }
        }
      }
    }
  }
}

void PixelDescriptorProvider::ProcessMutation(PixelDescriptor* source, PixelDescriptor* destination)
{
  destination->Copy(source);
  destination->SetType(IPixelDescriptor::CreatureType);
  destination->nextTurnTransaction.clear();
  destination->m_health = source->m_baseHealth;
  destination->m_id = nextId++;
  destination->m_skipFirstStep = true;
  destination->m_age = 0;
  
  if (cBoolRandPercent(kPercentOfMutations))
  {
    CellType newType = (CellType)cRandABInt(eCellTypeStart, eCellTypeEnd);
    if (newType != destination->m_character)
    {
      komorki::CreateCell(destination, newType);
    }
  }
}

void PixelDescriptorProvider::Update(bool passUpdateResult, std::list<IPixelDescriptorProvider::UpdateResult>& result)
{
  #if COLLECT_POPULATION_METRICS
    m_population.clear();
  #endif
  
  ProccessTransaction(passUpdateResult, result);

  for (int i = 0; i < kMapWidth; ++i)
  {
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < kMapHeight; ++j)
    {
      m_typeMap[i][j] = line[j].get()->GetType();
    }
  }

  for (int i = 0; i < kMapWidth; ++i)
  {
    const std::vector<IPixelDescriptor::Type>& typeMapLine = m_typeMap[i];
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < kMapHeight; ++j)
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
  for (int i = 0; i < kMapWidth; ++i)
  {
    const std::vector<PixelPtr>& line = m_map[i];
    for (int j = 0; j < kMapHeight; ++j)
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
