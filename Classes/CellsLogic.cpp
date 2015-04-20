//
//  CreaturesLogic.cpp
//  komorki
//
//  Created by ttohin on 21.03.15.
//
//

#include "CellsLogic.h"
#include "b2Utilites.h"

namespace komorki
{
  
//**************************************************************************************************
bool CheckBounds(int x, int y)
{
  if(x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
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
      
      auto targetPd = map[dx][dy].get();
      if(targetPd->GetType() == IPixelDescriptor::CreatureType && targetPd->m_character == character)
        counter += 1;
    }
  }
  
  return counter;
}

//**************************************************************************************************
void Attack(PixelDescriptor* me,
            PixelDescriptor* food,
            Vec2 pos,
            Vec2 delta,
            PixelDescriptorProvider::Config::CellConfig* config,
            Optional<Action>& action)
{
  Transaction enemyInput;
  enemyInput.m_armor = -config->attack;
  food->nextTurnTransaction.push_back(enemyInput);
  
  Transaction myInput;
  myInput.m_health = config->healthPerAttack;
  me->nextTurnTransaction.push_back(myInput);
  
  action.SetValue(true);
  action.value.source = pos;
  action.value.delta = delta;
}
  
//**************************************************************************************************
void ProcessHunterCreature(PixelDescriptor* pd,
                           Vec2 pos,
                           PixelDescriptorProvider::Config::CellConfig* config,
                           const PixelDescriptorProvider::PixelMap& map, // ugly hack
                           Optional<Movement>& movement,
                           Optional<Action>& action)
{
  const int xStart = rand()%2 == 0 ? 1 : -1;
  const int yStart = rand()%2 == 0 ? 1 : -1;
  
  Vec2 otherHunterOffset;
  
  for (int i = xStart; i != -2*xStart; i += -xStart)
  {
    for (int j = yStart; j != -2*yStart; j += -yStart)
    {
      int dx = pos.x + i;
      int dy = pos.y + j;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy].get();
      if(targetPd->GetType() == IPixelDescriptor::CreatureType)
      {
        if (targetPd->m_id == pd->m_id)
          continue;
        
        if (targetPd->m_character == eCellTypeHunter)
        {
          otherHunterOffset = Vec2(i, j);
          continue;
        }
        
        if (targetPd->m_character == eCellTypeGreen)
        {
          Attack(pd, targetPd, pos, Vec2(i, j), config, action);
          return;
        }
      }
    }
  }
  
  auto dest = Vec2(pos.x - otherHunterOffset.x, pos.y - otherHunterOffset.y);
  if(map[dest.x][dest.y]->GetType() == IPixelDescriptor::Empty)
  {
    movement.SetValue(true);
    movement.value.source = pos;
    movement.value.destination = dest;
    return;
  }
  
  auto id = (int)roundf(CCRANDOM_MINUS1_1());
  auto jd = (int)roundf(CCRANDOM_MINUS1_1());
  
  if(!CheckBounds(pos.x + id, pos.y + jd))
    return;
  
  dest = Vec2(pos.x + id, pos.y + jd);
  if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
    return;
  
  movement.SetValue(true);
  movement.value.source = pos;
  movement.value.destination = dest;
}
  
//**************************************************************************************************
void ProcessSalad(PixelDescriptor* pd,
                  Vec2 pos,
                  PixelDescriptorProvider::Config::CellConfig* config,
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
  
  const int xStart = rand()%2 == 0 ? 1 : -1;
  const int yStart = rand()%2 == 0 ? 1 : -1;
  
  for (int di = xStart; di != -2*xStart; di += -xStart)
  {
    for (int dj = yStart; dj != -2*yStart; dj += -yStart)
    {
      if (di == 0 && dj == 0)
        continue;
      
      int dx = pos.x + di;
      int dy = pos.y + dj;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy].get();
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
void ProcessImprovedSalad(PixelDescriptor* pd,
                          Vec2 pos,
                          PixelDescriptorProvider::Config::CellConfig* config,
                          const PixelDescriptorProvider::PixelMap& map, // ugly hack
                          Optional<Movement>& movement,
                          Optional<Action>& action)
{
  int improvedSaladCount = 0;
  
  const int xStart = rand()%2 == 0 ? 1 : -1;
  const int yStart = rand()%2 == 0 ? 1 : -1;
  
  for (int di = xStart; di != -2*xStart; di += -xStart)
  {
    for (int dj = yStart; dj != -2*yStart; dj += -yStart)
    {
      if (di == 0 && dj == 0)
        continue;
      
      int dx = pos.x + di;
      int dy = pos.y + dj;
      if(!CheckBounds(dx, dy))
        continue;
      
      auto targetPd = map[dx][dy].get();
      if(targetPd->GetType() == IPixelDescriptor::CreatureType)
      {
        if (targetPd->m_id == pd->m_id)
          continue;
        
        if (targetPd->m_character == eCellTypeGreen)
        {
          auto dest = Vec2(pos.x - di, pos.y - dj);
          if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
            continue;
          
          movement.SetValue(true);
          movement.value.source = pos;
          movement.value.destination = dest;
          return;
        }
        
        if (targetPd->m_character == eCellTypeImprovedSalad)
        {
          ++improvedSaladCount;
        }
      }
    }
  }
  
  if (pd->m_sleepCounter != 0)
  {
    pd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    pd->m_sleepCounter = pd->m_sleepTime;
  }
  
  if (improvedSaladCount >= 2)
  {
    pd->m_sleepCounter = pd->m_sleepTime = 10;
    return;
  }
  
  pd->m_sleepCounter = pd->m_sleepTime = 0;
  
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
//  void ProcessImprovedSalad(PixelDescriptor* pd,
//                            Vec2 pos,
//                            const PixelDescriptorProvider::PixelMap& map, // ugly hack
//                            Optional<Movement>& movement,
//                            Optional<Action>& action)
//  {
//    if (pd->m_sleepCounter != 0)
//    {
//      pd->m_sleepCounter -= 1;
//      return;
//    }
//    else
//    {
//      pd->m_sleepCounter = pd->m_sleepTime;
//    }
//    
//    komorki::Vec2 minPos = pos;
//    komorki::Vec2 maxPos = pos;
//    
//    int saladCountMax = 0;
//    int saladCountMin = 0;
//    
//    int xStart = rand()%2 == 0 ? 1 : -1;
//    int yStart = rand()%2 == 0 ? 1 : -1;
//    
//    for (int di = xStart; di != -xStart; di += -xStart)
//    {
//      for (int dj = yStart; dj != -yStart; dj += -yStart)
//      {
//        if (di == 0 && dj == 0) {
//          continue;
//        }
//        
//        int dx = pos.x + di;
//        int dy = pos.y + dj;
//        if(!CheckBounds(dx, dy))
//          continue;
//        
//        auto targetPd = map[dx][dy].get();
//        if(targetPd->GetType() != IPixelDescriptor::Empty)
//          continue;
//        
//        komorki::Vec2 p = Vec2(dx, dy);
//        
//        int pc = CountTypeAroundPosition(p, map, eCellTypeImprovedSalad);
//        if (pc < saladCountMin) {
//          saladCountMin = pc;
//          minPos = p;
//        }
//        
//        if (pc > saladCountMax) {
//          saladCountMax = pc;
//          maxPos = p;
//        }
//      }
//    }
//    
//    int saladCount = CountTypeAroundPosition(pos, map, eCellTypeImprovedSalad);
//    if (saladCount <= 2 &&  maxPos != pos && saladCountMax > saladCount)
//    {
//      movement.SetValue(true);
//      movement.value.source = pos;
//      movement.value.destination = maxPos;
//      
//    }
//    else if (saladCount > 2 && minPos != pos)
//    {
//      movement.SetValue(true);
//      movement.value.source = pos;
//      movement.value.destination = minPos;
//    }
//    else
//    {
//      auto id = (int)roundf(CCRANDOM_MINUS1_1());
//      auto jd = (int)roundf(CCRANDOM_MINUS1_1());
//      
//      if(!CheckBounds(pos.x + id, pos.y + jd))
//        return;
//      
//      auto dest = Vec2(pos.x + id, pos.y + jd);
//      if(map[dest.x][dest.y]->GetType() != IPixelDescriptor::Empty)
//        return;
//      
//      movement.SetValue(true);
//      movement.value.source = pos;
//      movement.value.destination = dest;
//    }
//  }

//**************************************************************************************************
void ProcessGreenCell(PixelDescriptor* pd,
                      Vec2 pos,
                      PixelDescriptorProvider::Config::CellConfig* config,
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
      
      auto targetPd = map[dx][dy].get();
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
        
        if (targetPd->m_character == eCellTypeSalad ||
            targetPd->m_character == eCellTypeImprovedSalad)
        {
          Attack(pd, targetPd, pos, Vec2(i, j), config, action);
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
      
      auto targetPd = map[dx][dy].get();
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
                 PixelDescriptorProvider::Config* config,
                 const PixelDescriptorProvider::PixelMap& map, // ugly hack
                 Optional<Movement>& m,
                 Optional<Action>& a)
{
  PixelDescriptorProvider::Config::CellConfig* cellConfig = config->ConfigForCell(d->m_character);
  
  Transaction t;
  t.m_health = cRandABInt(cellConfig->passiveHealthChunkMin, cellConfig->passiveHealthChunkMax);
  d->nextTurnTransaction.push_back(t);
  
  if (d->m_character == eCellTypeGreen)
  {
    ProcessGreenCell(d, pos, cellConfig, map, m, a);
  }
  else if (d->m_character == eCellTypeHunter)
  {
    ProcessHunterCreature(d, pos, cellConfig, map, m, a);
  }
  else if (d->m_character == eCellTypeSalad)
  {
    ProcessSalad(d, pos, cellConfig, map, m, a);
  }
  else if (d->m_character == eCellTypeImprovedSalad)
  {
    ProcessImprovedSalad(d, pos, cellConfig, map, m, a);
  }
}

} // namespace komorki;

