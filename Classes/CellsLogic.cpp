//
//  CreaturesLogic.cpp
//  komorki
//
//  Created by ttohin on 21.03.15.
//
//

#include "CellsLogic.h"
#include "b2Utilites.h"
#include "PixelDescriptor.h"
#include "CellDescriptor.h"

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
int CountTypeAroundPosition(komorki::Vec2 pos, const PixelDescriptorProvider::PixelMap& map, const CellType& character)
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
      if(targetPd->m_type == PixelDescriptor::CreatureType &&
         (targetPd->m_cellDescriptor->m_character & character))
        counter += 1;
    }
  }
  
  return counter;
}

//**************************************************************************************************
void Attack(CellDescriptor* me,
            CellDescriptor* food,
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
  
  action.SetValueFlag(true);
  action.value.delta = delta;
}
  
//**************************************************************************************************
void ProcessHunterCreature(CellDescriptor* pd,
                           Vec2 pos,
                           PixelDescriptorProvider::Config::CellConfig* config,
                           const PixelDescriptorProvider::PixelMap& map, // ugly hack
                           Optional<Movement>& movement,
                           Optional<Action>& action)
{
  
}
  
//**************************************************************************************************
void ProcessBigBlue(CellDescriptor* pd,
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
  
  static std::vector<PixelDescriptor*> food;
  food.clear();
  food.reserve(20);
  
  static std::vector<PixelDescriptor*> empty;
  empty.clear();
  empty.reserve(20);
  
  static std::vector<PixelDescriptor*> enemies;
  enemies.clear();
  enemies.reserve(20);
  
  pd->AroundRandom([pd](PixelDescriptor* targetPd, bool& stop) {
    if(targetPd->m_type == PixelDescriptor::CreatureType)
    {
      if (targetPd->m_cellDescriptor == pd)
        return;
      
      Vec2 offset;
      offset.x = pd->parent->x > targetPd->x ? -1 : pd->parent->x < targetPd->x ? 1 : 0;
      offset.y = pd->parent->y > targetPd->y ? -1 : pd->parent->y < targetPd->y ? 1 : 0;
      
      if (targetPd->m_cellDescriptor->m_character & pd->m_foodMask)
      {
        food.push_back(targetPd);
      }
      if (targetPd->m_cellDescriptor->m_character & pd->m_dangerMask)
      {
        enemies.push_back(targetPd);
      }
    }
  });
  
  if (food.size() >= 5 || enemies.size() > 0)
  {
    for (auto enemy : food)
    {
      Transaction enemyInput;
      enemyInput.m_armor = -config->attack;
      enemy->m_cellDescriptor->nextTurnTransaction.push_back(enemyInput);
      
      Transaction myInput;
      myInput.m_health = config->healthPerAttack;
      pd->nextTurnTransaction.push_back(myInput);
    }
    
    action.SetValueFlag(true);
    action.value.delta = {0, 0};
    return;
  }
  
  pd->parent->AroundRandom([pd](PixelDescriptor* targetPd, bool& stop){
    if (pd->TestSpace(targetPd))
    {
      empty.push_back(targetPd);
    }
  });
  
  if (empty.size() != 0)
  {
    pd->Move(empty[0]);
    movement.SetValueFlag(true);
    movement.value.destinationDesc = empty[0];
    movement.value.duration = pd->m_sleepCounter;
  }
}

//**************************************************************************************************
void ProcessImprovedSalad(CellDescriptor* pd,
                          Vec2 pos,
                          PixelDescriptorProvider::Config::CellConfig* config,
                          const PixelDescriptorProvider::PixelMap& map, // ugly hack
                          Optional<Movement>& movement,
                          Optional<Action>& action)
{
  int friendsCount = 0;
  
  PixelDescriptor* frientPd = nullptr;
  PixelDescriptor* emptyBlock = nullptr;
  pd->AroundRandom([pd, &movement, &frientPd, &emptyBlock, &action, &friendsCount, config](PixelDescriptor* targetPd, bool& stop) {
    if(targetPd->m_type == PixelDescriptor::CreatureType)
    {
      if (targetPd->m_cellDescriptor == pd)
        return;
      
      Vec2 offset;
      offset.x = pd->parent->x > targetPd->x ? -1 : pd->parent->x < targetPd->x ? 1 : 0;
      offset.y = pd->parent->y > targetPd->y ? -1 : pd->parent->y < targetPd->y ? 1 : 0;
      if (targetPd->m_cellDescriptor->m_character & pd->m_dangerMask)
      {
        PixelDescriptor* newPos = pd->parent->Offset(-offset.x, -offset.y);
        
        if (pd->TestSpace(newPos))
        {
          pd->Move(newPos);
          movement.SetValueFlag(true);
          movement.value.destinationDesc = newPos;
        }
        
        stop = true;
        return;
      }
      
      if (targetPd->m_cellDescriptor->m_character & pd->m_foodMask)
      {
        Attack(pd, targetPd->m_cellDescriptor, offset, config, action);
        stop = true;
        return;
      }
      
      if (targetPd->m_cellDescriptor->m_character & pd->m_friendMask)
      {
        friendsCount += 1;
        frientPd = targetPd;
      }
    }
    else if (emptyBlock == nullptr)
    {
      if (pd->TestSpace(targetPd))
        emptyBlock = targetPd;
    }
  });
  
  if (action.isSet || movement.isSet) return;
  
  if (pd->m_sleepCounter != 0)
  {
    pd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    pd->m_sleepCounter = pd->m_sleepTime;
  }
  
  if (friendsCount >= 2)
  {
    pd->m_sleepCounter = pd->m_sleepTime = 10;
    return;
  }
  
  pd->m_sleepCounter = pd->m_sleepTime = 0;
  
  if (emptyBlock != nullptr)
  {
    pd->Move(emptyBlock);
    movement.SetValueFlag(true);
    movement.value.destinationDesc = emptyBlock;
  }
}
  
  //**************************************************************************************************
//  void ProcessImprovedSalad(CellDescriptor* pd,
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
//        if(targetPd->m_type != PixelDescriptor::Empty)
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
//      movement.SetValueFlag(true);
//      movement.value.source = pos;
//      movement.value.destination = maxPos;
//      
//    }
//    else if (saladCount > 2 && minPos != pos)
//    {
//      movement.SetValueFlag(true);
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
//      if(map[dest.x][dest.y]->m_type != PixelDescriptor::Empty)
//        return;
//      
//      movement.SetValueFlag(true);
//      movement.value.source = pos;
//      movement.value.destination = dest;
//    }
//  }

//**************************************************************************************************
void ProcessGreenCell(CellDescriptor* pd,
                      Vec2 pos,
                      PixelDescriptorProvider::Config::CellConfig* config,
                      const PixelDescriptorProvider::PixelMap& map, // ugly hack
                      Optional<Movement>& movement,
                      Optional<Action>& action)
{
  static int repeatStepNumber = 3;
  
  if (pd->m_sleepCounter != 0)
  {
    pd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    pd->m_sleepCounter = pd->m_sleepTime;
  }
  
  PixelDescriptor* frientPd = nullptr;
  pd->AroundRandom([pd, &movement, &frientPd, &action, config](PixelDescriptor* targetPd, bool& stop) {
    if(targetPd && targetPd->m_type == PixelDescriptor::CreatureType)
    {
      if (targetPd->m_cellDescriptor == pd)
        return;
      
      Vec2 offset;
      offset.x = pd->parent->x > targetPd->x ? -1 : pd->parent->x < targetPd->x ? 1 : 0;
      offset.y = pd->parent->y > targetPd->y ? -1 : pd->parent->y < targetPd->y ? 1 : 0;
      if (targetPd->m_cellDescriptor->m_character & pd->m_dangerMask)
      {
        PixelDescriptor* newPos = pd->parent->Offset(-offset.x, -offset.y);
        if (pd->TestSpace(newPos))
        {
          pd->m_m1 = -offset.x;
          pd->m_m2 = -offset.y;
          pd->m_m3 = cRandABInt(1, repeatStepNumber);
          pd->Move(newPos);
          movement.SetValueFlag(true);
          movement.value.destinationDesc = newPos;
          movement.value.duration = pd->m_sleepCounter;
        }
        
        stop = true;
        return;
      }
      
      if (targetPd->m_cellDescriptor->m_character & pd->m_foodMask)
      {
        Attack(pd, targetPd->m_cellDescriptor, offset, config, action);
        stop = true;
        return;
      }
      
      if (targetPd->m_cellDescriptor->m_character & pd->m_friendMask)
      {
        frientPd = targetPd;
      }
    }
  });
  
  if (action.isSet || movement.isSet) return;

  if (frientPd != nullptr)
  {
    Vec2 offset;
    offset.x = pd->parent->x > frientPd->x ? 1 : pd->parent->x < frientPd->x ? -1 : 0;
    offset.y = pd->parent->y > frientPd->y ? 1 : pd->parent->y < frientPd->y ? -1 : 0;
    PixelDescriptor* newPos = pd->parent->Offset(offset.x, offset.y);
    
    if (pd->TestSpace(newPos))
    {
      pd->m_m1 = offset.x;
      pd->m_m2 = offset.y;
      pd->m_m3 = cRandABInt(1, repeatStepNumber);
      pd->Move(newPos);
      movement.SetValueFlag(true);
      movement.value.destinationDesc = newPos;
      movement.value.duration = pd->m_sleepCounter;
      return;
    }
  }
  
  PixelDescriptor* emptyBlock = nullptr;
  if (pd->m_m3 != 0)
  {
    PixelDescriptor* newPos = pd->parent->Offset(pd->m_m1, pd->m_m2);
    if (pd->TestSpace(newPos))
    {
      emptyBlock = newPos;
      pd->m_m3 -= 1;
    }
    else
    {
      pd->m_m3 = 0;
    }
  }
  
  if (emptyBlock == nullptr)
  {
    pd->parent->AroundRandom([pd, &emptyBlock](PixelDescriptor* targetPd, bool& stop){
      if (pd->TestSpace(targetPd))
      {
        emptyBlock = targetPd;
        stop = true;
      }
    });
    
    if (pd->m_m3 == 0 && emptyBlock != nullptr)
    {
      pd->m_m3 = cRandABInt(1, repeatStepNumber);
    }
  }

  if (emptyBlock != nullptr)
  {
    Vec2 offset;
    assert(pd->parent->Offset(emptyBlock, offset));
    pd->m_m1 = offset.x;
    pd->m_m2 = offset.y;
    pd->Move(emptyBlock);
    movement.SetValueFlag(true);
    movement.value.destinationDesc = emptyBlock;
    movement.value.duration = pd->m_sleepCounter;
  }
}

//**************************************************************************************************
void ProcessCell(CellDescriptor* d,
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
    ProcessGreenCell(d, pos, cellConfig, map, m, a);
  }
  else if (d->m_character == eCellTypeSalad)
  {
    ProcessGreenCell(d, pos, cellConfig, map, m, a);
  }
  else if (d->m_character == eCellTypeImprovedSalad)
  {
    ProcessImprovedSalad(d, pos, cellConfig, map, m, a);
  }
  else if (d->m_character == eCellTypeBigBlue)
  {
    ProcessBigBlue(d, pos, cellConfig, map, m, a);
  }
}

} // namespace komorki;

