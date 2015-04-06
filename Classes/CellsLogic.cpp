//
//  CreaturesLogic.cpp
//  komorki
//
//  Created by ttohin on 21.03.15.
//
//

#include "CellsLogic.h"
#include "CellsParameters.h"
#include "b2Utilites.h"

namespace komorki
{
  
//**************************************************************************************************
bool CheckBounds(int x, int y)
{
  if(x < 0 || x >= kMapWidth || y < 0 || y >= kMapHeight)
    return false;
  
  return true;
}

//**************************************************************************************************
int CountTypeAroundPosition(komorki::Vec2 pos, const PixelDescriptorProvider::PixelMap& map, int character)
{
  int counter = 0;
  for (int i = -1; i <= 1; ++i)
  {
    for (int j = -1; j <= 1; ++j)
    {
      if (i == 0 && j == 0)
        continue;
      
      int dx = pos.x + i;
      int dy = pos.y + j;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy];
      if(targetPd->GetType() == IPixelDescriptor::CreatureType && targetPd->m_character == character)
        counter += 1;
    }
  }
  
  return counter;
}

//**************************************************************************************************
void ProcessHunterCreature(PixelDescriptor* pd,
                           Vec2 pos,
                           const PixelDescriptorProvider::PixelMap& map, // ugly hack
                           Optional<Movement>& movement,
                           Optional<Action>& action)
{
  for (int i = -1; i <= 1; ++i)
  {
    for (int j = -1; j <= 1; ++j)
    {
      int dx = pos.x + i;
      int dy = pos.y + j;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy];
      if(targetPd->GetType() == IPixelDescriptor::CreatureType)
      {
        if (targetPd->m_id == pd->m_id)
          continue;
        
        if (targetPd->m_character == eCellTypeSalat ||
            targetPd->m_character == eCellTypeHunter ||
            targetPd->m_character == eCellTypeImprovedSalat)
          continue;
        
        Transaction enemyInput;
        enemyInput.m_armor = -kHunterAttack;
        targetPd->nextTurnTransaction.push_back(enemyInput);
        
        Transaction myInput;
        myInput.m_health = kHunterHealthIncome;
        pd->nextTurnTransaction.push_back(myInput);
        
        action.SetValue(true);
        action.value.source = pos;
        action.value.delta = Vec2(i, j);
        return;
      }
    }
  }
  
  for (int i = -1; i <= 1; ++i)
  {
    for (int j = -1; j <= 1; ++j)
    {
      int dx = pos.x + i;
      int dy = pos.y + j;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy];
      if(targetPd->GetType() == IPixelDescriptor::CreatureType)
      {
        if (targetPd->m_id == pd->m_id)
          continue;
        
        if (targetPd->m_character == eCellTypeHunter)
        {
          auto dest = Vec2(pos.x - i, pos.y - j);
          if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
            continue;
          
          movement.SetValue(true);
          movement.value.source = pos;
          movement.value.destination = dest;
          return;
        }
      }
    }
  }
  
  auto id = (int)roundf(CCRANDOM_MINUS1_1());
  auto jd = (int)roundf(CCRANDOM_MINUS1_1());
  
  if(!CheckBounds(pos.x + id, pos.y + jd))
    return;
  
  auto dest = Vec2(pos.x + id, pos.y + jd);
  if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
    return;
  
  movement.SetValue(true);
  movement.value.source = pos;
  movement.value.destination = dest;
}
  
//**************************************************************************************************
void ProcessSalat(PixelDescriptor* pd,
                  Vec2 pos,
                  const PixelDescriptorProvider::PixelMap& map, // ugly hack
                  Optional<Movement>& movement,
                  Optional<Action>& action)
{
  if (pd->m_sleepCounter != 0)
  {
    pd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    pd->m_sleepCounter = pd->m_sleepTime;
  }
  
  
  std::vector<komorki::Vec2> emptyBlocks;
  
  int xStart = rand()%2 == 0 ? 1 : -1;
  int yStart = rand()%2 == 0 ? 1 : -1;
  
  for (int di = xStart; di != -xStart; di += -xStart)
  {
    for (int dj = yStart; dj != -yStart; dj += -yStart)
    {
      if (di == 0 && dj == 0)
        continue;
      
      int dx = pos.x + di;
      int dy = pos.y + dj;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy];
      if(targetPd->GetType() == IPixelDescriptor::Empty)
        emptyBlocks.push_back(Vec2(dx, dy));
    }
  }
  
  if (emptyBlocks.empty())
    return;
  
  int destIndex = cRandABInt(0, emptyBlocks.size());
  
  movement.SetValue(true);
  movement.value.source = pos;
  movement.value.destination = emptyBlocks[destIndex];
}

//**************************************************************************************************
void ProcessImprovedSalat(PixelDescriptor* pd,
                          Vec2 pos,
                          const PixelDescriptorProvider::PixelMap& map, // ugly hack
                          Optional<Movement>& movement,
                          Optional<Action>& action)
{
  if (pd->m_sleepCounter != 0)
  {
    pd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    pd->m_sleepCounter = pd->m_sleepTime;
  }
  
  komorki::Vec2 minPos = pos;
  komorki::Vec2 maxPos = pos;
  
  int salatCountMax = 0;
  int salatCountMin = 0;
  
  int xStart = rand()%2 == 0 ? 1 : -1;
  int yStart = rand()%2 == 0 ? 1 : -1;
  
  for (int di = xStart; di != -xStart; di += -xStart)
  {
    for (int dj = yStart; dj != -yStart; dj += -yStart)
    {
      if (di == 0 && dj == 0) {
        continue;
      }
      
      int dx = pos.x + di;
      int dy = pos.y + dj;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy];
      if(targetPd->GetType() != IPixelDescriptor::Empty)
        continue;
      
      komorki::Vec2 p = Vec2(dx, dy);
      
      int pc = CountTypeAroundPosition(p, map, eCellTypeImprovedSalat);
      if (pc < salatCountMin) {
        salatCountMin = pc;
        minPos = p;
      }
      
      if (pc > salatCountMax) {
        salatCountMax = pc;
        maxPos = p;
      }
    }
  }
  
  int salatCount = CountTypeAroundPosition(pos, map, eCellTypeImprovedSalat);
  if (salatCount <= 2 &&  maxPos != pos && salatCountMax > salatCount)
  {
    movement.SetValue(true);
    movement.value.source = pos;
    movement.value.destination = maxPos;
    
  }
  else if (salatCount > 2 && minPos != pos)
  {
    movement.SetValue(true);
    movement.value.source = pos;
    movement.value.destination = minPos;
  }
  else
  {
    auto id = (int)roundf(CCRANDOM_MINUS1_1());
    auto jd = (int)roundf(CCRANDOM_MINUS1_1());
    
    if(!CheckBounds(pos.x + id, pos.y + jd))
      return;
    
    auto dest = Vec2(pos.x + id, pos.y + jd);
    if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
      return;
    
    movement.SetValue(true);
    movement.value.source = pos;
    movement.value.destination = dest;
  }
}

//**************************************************************************************************
void ProcessGreenCell(PixelDescriptor* pd,
                      Vec2 pos,
                      const PixelDescriptorProvider::PixelMap& map, // ugly hack
                      Optional<Movement>& movement,
                      Optional<Action>& action)
{
  if (pd->m_sleepCounter != 0)
  {
    pd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    pd->m_sleepCounter = pd->m_sleepTime;
  }
  
  for (int i = -1; i <= 1; ++i)
  {
    for (int j = -1; j <= 1; ++j)
    {
      int dx = pos.x + i;
      int dy = pos.y + j;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy];
      if(targetPd->GetType() == IPixelDescriptor::CreatureType)
      {
        if (targetPd->m_id == pd->m_id)
          continue;
        
        if (targetPd->m_character == eCellTypeHunter)
        {
          auto dest = Vec2(pos.x - i, pos.y - j);
          if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
            continue;
          
          movement.SetValue(true);
          movement.value.source = pos;
          movement.value.destination = dest;
          return;
        }
        
        if (targetPd->m_character == eCellTypeSalat ||
            targetPd->m_character == eCellTypeImprovedSalat)
        {
          Transaction enemyInput;
          enemyInput.m_armor = -kGreenAttack;
          targetPd->nextTurnTransaction.push_back(enemyInput);
          
          Transaction myInput;
          myInput.m_health = kGreenHealthIncome;
          pd->nextTurnTransaction.push_back(myInput);
          
          action.SetValue(true);
          action.value.source = pos;
          action.value.delta = Vec2(i, j);
          return;
        }
      }
    }
  }
  
  for (int i = -1; i <= 1; ++i)
  {
    for (int j = -1; j <= 1; ++j)
    {
      int dx = pos.x + i;
      int dy = pos.y + j;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy];
      if(targetPd->GetType() == IPixelDescriptor::CreatureType)
      {
        if (targetPd->m_id == pd->m_id)
          continue;
        
        if (targetPd->m_character == eCellTypeGreen)
        {
          auto dest = Vec2(pos.x - i, pos.y - j);
          if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
            continue;
          
          movement.SetValue(true);
          movement.value.source = pos;
          movement.value.destination = dest;
          return;
        }
      }
    }
  }
  
  auto id = (int)roundf(CCRANDOM_MINUS1_1());
  auto jd = (int)roundf(CCRANDOM_MINUS1_1());
  
  if(!CheckBounds(pos.x + id, pos.y + jd))
    return;
  
  auto dest = Vec2(pos.x + id, pos.y + jd);
  if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
    return;
  
  movement.SetValue(true);
  movement.value.source = pos;
  movement.value.destination = dest;
}

//**************************************************************************************************
void ProcessCell(PixelDescriptor* d,
                 Vec2 pos,
                 const PixelDescriptorProvider::PixelMap& map, // ugly hack
                 Optional<Movement>& m,
                 Optional<Action>& a)
{
  if (d->m_character == eCellTypeGreen)
  {
    Transaction t;
    t.m_health = kHunterHungryDamage;
    d->nextTurnTransaction.push_back(t);
    ProcessGreenCell(d, pos, map, m, a);
  }
  else if (d->m_character == eCellTypeHunter)
  {
    Transaction t;
    t.m_health = kGreenHungryDamage;
    d->nextTurnTransaction.push_back(t);
    ProcessHunterCreature(d, pos, map, m, a);
  }
  else if (d->m_character == eCellTypeSalat)
  {
    Transaction t;
    t.m_health = cRandABInt(kSalatIncomeMin, kSalatIncomeMax);
    d->nextTurnTransaction.push_back(t);
    ProcessSalat(d, pos, map, m, a);
  }
  else if (d->m_character == eCellTypeImprovedSalat)
  {
    Transaction t;
    t.m_health = cRandABInt(kImprovedSalatIncomeMin, kImprovedSalatIncomeMax);
    d->nextTurnTransaction.push_back(t);
    ProcessImprovedSalat(d, pos, map, m, a);
  }
}

} // namespace komorki;

