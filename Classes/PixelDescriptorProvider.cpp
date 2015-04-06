#include "PixelDescriptorProvider.h"
#include "b2Utilites.h"
#include <math.h>
#include "CellsParameters.h"
#include "CellsLogic.h"

#define COLLECT_POPULATION_METRICS 0

int kGroundSize = 8;
int kMapHeight = 100;
int kMapWidth = 150;

int kGreenHealth = 301;
int kHunterHealth = 555;
int kBaseHealth = 200;

int kHunterHealthIncome = 22;
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
int ImprovedPlanArmor = 30;

int kSalatArmor = 21;
int kSalatSleepTime = 30;

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
  this->basehealth = kBaseHealth;
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
  this->improvedSalatArmor = ImprovedPlanArmor;
  
  this->salatSleepTime = kSalatSleepTime;
  this->percentOfMMutations = kPercentOfMutations;
  this->hunterLifeTime = kHunterLifeTime;
}

void PixelDescriptorProvider::InitWithConfig(PixelDescriptorProvider::Config *config)
{
  kGroundSize = config->terrainSize;
  kMapWidth = config->mapWidth;
  kMapHeight = config->mapHeight;
  
  kGreenHealth = config->greenHealth;
  kHunterHealth = config->hunterHealth;
  kBaseHealth = config->basehealth;
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
  ImprovedPlanArmor = config->improvedSalatArmor;
  
  kSalatSleepTime = config->salatSleepTime;
  kPercentOfMutations = config->percentOfMMutations;
  kHunterLifeTime = config->hunterLifeTime;
  
  InitWithDefaults();
}

PixelDescriptorProvider::PixelPtr PixelDescriptorProvider::CreateCell(CellType creatureType)
{
  auto pd = std::make_shared<PixelDescriptor>(IPixelDescriptor::CreatureType);
  pd->m_character = creatureType;
  pd->m_id = nextId++;
  pd->m_health = pd->m_baseHealth = creatureType == eCellTypeGreen ? kGreenHealth : creatureType == eCellTypeHunter ? kHunterHealth : kBaseHealth;
  pd->m_armor = pd->m_baseArmor = creatureType == eCellTypeSalat ? kSalatArmor : creatureType == eCellTypeImprovedSalat ? ImprovedPlanArmor : 10;
  pd->m_damage = creatureType == eCellTypeHunter ? kHunterAttack : creatureType == eCellTypeGreen ? kGreenAttack : 0;
  pd->m_age = cRandABInt(0, 50);
  pd->m_sleepCounter = pd->m_sleepTime = creatureType == eCellTypeSalat ? kSalatSleepTime : creatureType == eCellTypeGreen ? kGreenSleepTime : 0;
  pd->m_lifeTime = creatureType == eCellTypeHunter ? kHunterLifeTime : kMaxLifeTime;
  
  return pd;
}

void PixelDescriptorProvider::InitWithDefaults()
{
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
      auto d = m_map[i][j];
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
            UpdateResult r(d.get());
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
          bool foundNewPos = false;
          
          std::shared_ptr<PixelDescriptor> destinationDesc;
          
          int xStart = rand()%2 == 0 ? 1 : -1;
          int yStart = rand()%2 == 0 ? 1 : -1;
          
          for (int di = xStart; di != -xStart; di += -xStart)
          {
            if (foundNewPos)
              break;
            
            for (int dj = yStart; dj != -yStart; dj += -yStart)
            {
              if (foundNewPos)
                break;
              
              int dx = i + di;
              int dy = j + dj;

              if(!CheckBounds(dx, dy))
                continue;

              auto targetPd = m_map[dx][dy];
              if(targetPd->GetType() == IPixelDescriptor::Empty)
              {
                foundNewPos = true;
                newPdPos = Vec2(dx, dy);
                destinationDesc = targetPd;
              }
            }
          }

          if (foundNewPos)
          {
            d->m_health = d->m_baseHealth;
            PixelDescriptor newPd = ProcessMutation(d.get());
            
            destinationDesc->Copy(&newPd);
            
            if (passUpdateResult)
            {
              UpdateResult r(destinationDesc.get());
              r.source = Vec2(i, j);
              
              r.addCreature.SetValue(true);
              r.addCreature.value.to = newPdPos;
              r.addCreature.value.destinationDesc = destinationDesc.get();
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
            UpdateResult r1(d.get());
            r1.source = Vec2(i, j);
            r1.shouldDelete = true;
            result.push_back(r1);
            
            UpdateResult r2(d.get());
            r2.source = Vec2(i, j);
            
            r2.addCreature.SetValue(true);
            r2.addCreature.value.to = Vec2(i, j);
            r2.addCreature.value.destinationDesc = d.get();
            result.push_back(r2);
          }
        }
      }
    }
  }
}

PixelDescriptor PixelDescriptorProvider::ProcessMutation(PixelDescriptor* d)
{
  PixelDescriptor newPd(IPixelDescriptor::CreatureType);
  newPd.Copy(d);
  newPd.nextTurnTransaction.clear();
  newPd.m_health = newPd.m_baseHealth = cRandEps(d->m_baseHealth, 0);
  
  if (newPd.m_health <= 0)
  {
    newPd.m_health = newPd.m_baseHealth = 10;
  }
  
  newPd.m_id = nextId++;
  newPd.m_skipFirstStep = true;
  newPd.m_sleepCounter = newPd.m_sleepTime = cRandEps(d->m_sleepTime, 0);
  newPd.m_armor = newPd.m_baseArmor = cRandEps(d->m_baseArmor, 0);
  if (newPd.m_armor <= 0)
  {
    newPd.m_armor = newPd.m_baseArmor = 1;
  }
  
  newPd.m_damage = cRandEps(d->m_damage, 0);
  if (newPd.m_damage <= 0)
  {
    newPd.m_damage = newPd.m_damage = 1;
  }
  
  newPd.m_age = 0;
  
  return newPd;
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
      m_typeMap[i][j] = line[j]->GetType();
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
  
  auto d = m_map[pos.x][pos.y];
  if (d->GetType() == IPixelDescriptor::Empty)
  {
    auto newPd = CreateCell(type);
    newPd->m_new = true;
    d->Copy(newPd.get());
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
