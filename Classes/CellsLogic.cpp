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
#include "CellShapesHelper.h"
#include <iostream>
#include <assert.h>

namespace komorki
{
  
//**************************************************************************************************
bool CheckBounds(int x, int y)
{
  if(x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
    return false;
  
  return true;
}
  
void AroundInDirection(PixelDescriptor* cd, const Vec2& direction, const PerPixelFunc& func)
{
  int directionIndex = cd->GetDirectionIndex(direction);
  bool stop = false;
  
  func(cd->GetPixelByDirectionalindex(directionIndex), stop);
  if (stop) return;
  
  int dirOffset = cRandPlusMinus();
  
  func(cd->GetPixelByDirectionalindex(directionIndex + dirOffset), stop);
  if (stop) return;
  func(cd->GetPixelByDirectionalindex(directionIndex - dirOffset), stop);
  if (stop) return;
}

////**************************************************************************************************
//int CountTypeAroundPosition(komorki::Vec2 pos, const PixelDescriptorProvider::PixelMap& map, const CellType& character)
//{
//  int counter = 0;
//  for (int i = -1; i <= 1; ++i)
//  {
//    for (int j = -1; j <= 1; ++j)
//    {
//      if (i == 0 && j == 0)
//        continue;
//      
//      int dx = pos.x + i;
//      int dy = pos.y + j;
//      if(!CheckBounds(dx, dy))
//        continue;
//      
//      auto targetPd = map[dx][dy].get();
//      if(targetPd->m_type == PixelDescriptor::CreatureType &&
//         (targetPd->m_cellDescriptor->m_character & character))
//        counter += 1;
//    }
//  }
//  
//  return counter;
//}
  
bool TestSpace(CellDescriptor* cd, IShape* shape)
{
  bool result = true;
  shape->ForEach([&](PixelDescriptor* pd, bool& stop)
                 {
                   if (pd->m_type == PixelDescriptor::Empty) return;
                   if (pd->m_cellDescriptor == cd) return;
                   
                   result = false;
                   stop = true;
                 });
  
  return result;
}

//**************************************************************************************************
void Attack(CellDescriptor* me,
            CellDescriptor* food,
            Vec2 delta,
            Optional<Action>& action)
{
  Transaction enemyInput;
  enemyInput.m_armor = -me->m_genom.m_damage;
  food->nextTurnTransaction.push_back(enemyInput);
  
  Transaction myInput;
  myInput.m_health = me->m_genom.m_healthPerAttach;
  me->nextTurnTransaction.push_back(myInput);
  
  action.SetValueFlag(true);
  action.value.delta = delta;
}
  
//**************************************************************************************************
void RectShapeChangeRect(CellDescriptor* cd,
                 Vec2 pos,
                 Config::CellConfig* config,
                 const PixelDescriptorProvider::PixelMap& map, // ugly hack
                 Optional<Movement>& movement,
                 Optional<Action>& action,
                 Optional<ChangeRect>& changeRect)
{
  Rect newRect;
  if (cd->m_m4 == 0)
  {
    newRect = {Vec2(0, 1), Vec2(1, 3)};
  }
  else if (cd->m_m4 == 1)
  {
    newRect = {Vec2(1, 0), Vec2(3, 1)};
  }
  
  RectShape testShape(cd->parent, newRect.origin, newRect.size);
  if (TestSpace(cd, &testShape))
  {
    cd->Shape([](PixelDescriptor* pd, bool& stop){
      pd->m_cellDescriptor = nullptr;
      pd->m_type = PixelDescriptor::Empty;
    });
    
    RectShape* shape = static_cast<RectShape*>(cd->GetShape());
    shape->SetAABB(newRect.origin, newRect.size);
    
    changeRect.value.rect =  shape->GetAABB();
    changeRect.SetValueFlag(true);
    
    cd->Finish();
    
    cd->m_m4 = (cd->m_m4 + 1) % 2;
  }
}
  
//**************************************************************************************************
void ProcessPink(CellDescriptor* cd,
                   Vec2 pos,
                   Config::CellConfig* config,
                   const PixelDescriptorProvider::PixelMap& map, // ugly hack
                   Optional<Movement>& movement,
                   Optional<Action>& action,
                   Optional<ChangeRect>& changeRect)
{
  if (cd->m_sleepCounter != 0)
  {
    cd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    cd->m_sleepCounter = cd->m_genom.m_sleepTime;
  }
  
  // using memory to store destination (1, 2) and number of steps (2)
  if ((cd->parent->x == cd->m_m1 &&
       cd->parent->y == cd->m_m2) || cd->m_m1 == 0 || cd->m_m2 == 0 || cd->m_m3 == 0)
  {
    cd->m_m1 = cRandABInt(0, map[0].size());
    cd->m_m2 = cRandABInt(0, map[0].size());
    cd->m_m3 = 50;
  }
  
  cd->m_m3 -= 1;
  
  cd->PrintAsciiArt();
  
  
  RectShape* shape = static_cast<RectShape*>(cd->GetShape());
  Rect currentRect = shape->GetRect();
  
  DistanceMeasureResult distanceResult;
  GetMaxMinDistantPixel(Vec2(cd->m_m1, cd->m_m2), cd->GetShape(), distanceResult);
  
  if (distanceResult.targetIsInShape)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    return;
  }
  
  Vec2 normalizedTarget = Vec2(cd->m_m1, cd->m_m2) - cd->parent->GetPos();
  normalizedTarget = normalizedTarget.Normalize();
  
  AroundInDirection(cd->parent, normalizedTarget, [&](PixelDescriptor* targetPd, bool& stop)
                                                    {
                                                      RectShape testShape(targetPd, currentRect.origin, currentRect.size);
                                                      
                                                      if (TestSpace(cd, &testShape))
                                                      {
                                                        movement.value.destinationDesc = targetPd;
                                                        movement.SetValueFlag(true);
                                                        stop = true;
                                                        return;
                                                      }
                                                    });
  
  if (movement.isSet == false)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    return;
  }
  
  cd->Shape([&](PixelDescriptor* targetPd, bool& stop)
            {
              targetPd->m_cellDescriptor = nullptr;
              targetPd->m_type = PixelDescriptor::Empty;
            });
  cd->GetShape()->SetPosition(movement.value.destinationDesc);
  cd->parent = movement.value.destinationDesc;
  cd->Finish();
  
  cd->PrintAsciiArt();
}
  
//**************************************************************************************************
void ProcessStaticWhite(CellDescriptor* cd,
                        Vec2 pos,
                        Config::CellConfig* config,
                        const PixelDescriptorProvider::PixelMap& map, // ugly hack
                        Optional<Movement>& movement,
                        Optional<Morphing>& morph,
                        Optional<Action>& action)
{
  if (cd->m_sleepCounter != 0)
  {
    cd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    cd->m_sleepCounter = cd->m_genom.m_sleepTime;
  }
  
  // using memory to store destination (1, 2) and number of steps (2)
  if ((cd->parent->x == cd->m_m1 &&
       cd->parent->y == cd->m_m2) || cd->m_m1 == 0 || cd->m_m2 == 0 || cd->m_m3 > 50)
  {
    cd->m_m1 = cRandABInt(0, map[0].size());
    cd->m_m2 = cRandABInt(0, map[0].size());
    cd->m_m3 = 0;
  }
  
  cd->m_m3 += 1;
  
  if (cd->m_m3 % 21 == 0)
  {
    action.value.pos = cd->parent->GetPos();
    action.value.delta = Vec2(-1, -1);
    action.SetValueFlag(true);
    return;
  }
  
  DistanceMeasureResult distanceResult;
  GetMaxMinDistantPixel(Vec2(cd->m_m1, cd->m_m2), cd->GetShape(), distanceResult);
  
  if (distanceResult.targetIsInShape)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    return;
  }
  
  Vec2 normalizedTarget = Vec2(cd->m_m1, cd->m_m2) - cd->parent->GetPos();
  normalizedTarget = normalizedTarget.Normalize();
  
  AroundInDirection(cd->parent, normalizedTarget, [&](PixelDescriptor* targetPd, bool& stop)
                    {
                      Vec2 offset;
                      
                      if (!cd->parent->Offset(targetPd, offset)) {
                        assert(0);
                      }
                      
                      PixelDescriptor::Vec newShapePixels;
                      cd->Shape([&](PixelDescriptor* pd, bool& stop) {
                        newShapePixels.push_back(pd->Offset(offset));
                      });
                      
                      PolymorphShape testShape(targetPd, newShapePixels);
                      
                      if (TestSpace(cd, &testShape))
                      {
                        movement.value.destinationDesc = targetPd;
                        movement.SetValueFlag(true);
                        stop = true;
                        return;
                      }
                    });
  
  if (movement.isSet == false)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    return;
  }
  
  cd->Shape([&](PixelDescriptor* targetPd, bool& stop)
            {
              targetPd->m_cellDescriptor = nullptr;
              targetPd->m_type = PixelDescriptor::Empty;
            });
  
  Vec2 offset = movement.value.destinationDesc->GetPos() - cd->parent->GetPos();
  
  PixelDescriptor::Vec newShapePixels;
  cd->Shape([&](PixelDescriptor* pd, bool& stop){
    newShapePixels.push_back(pd->Offset(offset));
    
    Morph m;
    m.dir = Morph::Move;
    m.pos = pd->GetPos();
    m.delta = offset;
    morph.value.vec.push_back(m);
    morph.SetValueFlag(true);
  });
  
  PolymorphShape* shape = static_cast<PolymorphShape*>(cd->GetShape());
  shape->SetPixels(movement.value.destinationDesc, newShapePixels);
  cd->GetShape()->SetPosition(movement.value.destinationDesc);
  cd->parent = movement.value.destinationDesc;
  cd->Finish();
  
  cd->PrintAsciiArt();
}
  
//**************************************************************************************************
void ProcessYellow(CellDescriptor* cd,
                  Vec2 pos,
                  Config::CellConfig* config,
                  const PixelDescriptorProvider::PixelMap& map, // ugly hack
                  Optional<Movement>& movement,
                  Optional<Morphing>& morph,
                  Optional<Action>& action)
{
  if (cd->m_sleepCounter != 0)
  {
    cd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    cd->m_sleepCounter = cd->m_genom.m_sleepTime;
  }
  
  // using memory to store destination (1, 2) and number of steps (2)
  if ((cd->parent->x == cd->m_m1 &&
       cd->parent->y == cd->m_m2) || cd->m_m1 == 0 || cd->m_m2 == 0 || cd->m_m3 > 50)
  {
    cd->m_m1 = cRandABInt(0, map[0].size());
    cd->m_m2 = cRandABInt(0, map[0].size());
    cd->m_m3 = 0;
  }
  
  cd->m_m3 += 1;
  
  cd->PrintAsciiArt();
  
  DistanceMeasureResult distanceResult;
  
  GetMaxMinDistantPixel(Vec2(cd->m_m1, cd->m_m2), cd->GetShape(), distanceResult);
  
  if (distanceResult.targetIsInShape)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    return;
  }
  
  std::vector<PixelDescriptor*> toRemove;
  
  distanceResult.maxPd->AroundRandom([&](PixelDescriptor* pd, bool& stop)
                                     {
                                       if (pd->m_cellDescriptor != cd)
                                       {
                                         return;
                                       }
                                       
                                       Morph m;
                                       m.dir = Morph::Inside;
                                       m.pos = Vec2(distanceResult.maxPd->x, distanceResult.maxPd->y);
                                       distanceResult.maxPd->Offset(pd, m.delta);
                                       morph.value.vec.push_back(m);
                                       morph.SetValueFlag(true);
                                       
                                       if (distanceResult.maxPd == cd->parent)
                                       {
                                         movement.value.destinationDesc = pd;
                                         movement.SetValueFlag(true);
                                       }
                                       
                                       toRemove.push_back(distanceResult.maxPd);
                                       
                                       stop = true;
                                       
                                     });
  
  if (morph.value.vec.size() == 0)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    morph.SetValueFlag(false);
    movement.SetValueFlag(false);
    return;
  }
  
  Vec2 normalizedMinDist = distanceResult.minDist.Normalize();
  
  std::vector<PixelDescriptor*> toInsert;
  
  {
    auto tagetPd = distanceResult.minPd->Offset(normalizedMinDist);
    if (FreePixelHasMyParts(cd, tagetPd, distanceResult.minPd))
    {
      Morph m;
      m.dir = Morph::Outside;
      m.pos = tagetPd->GetPos();
      m.delta = -normalizedMinDist;
      morph.value.vec.push_back(m);
      morph.SetValueFlag(true);
      
      toInsert.push_back(tagetPd);
    }
  }
  
  if (toInsert.size() == 0)
  {
    distanceResult.minPd->AroundRandom([&](PixelDescriptor* pd, bool& stop)
                                       {
                                         if (!FreePixelHasMyParts(cd, pd, distanceResult.minPd))
                                         {
                                           return;
                                         }
                                         
                                         Vec2 offset;
                                         distanceResult.minPd->Offset(pd, offset);
                                         
                                         Morph m;
                                         m.dir = Morph::Outside;
                                         m.pos = pd->GetPos();
                                         m.delta = -offset;
                                         morph.value.vec.push_back(m);
                                         morph.SetValueFlag(true);
                                         
                                         toInsert.push_back(pd);
                                         
                                         stop = true;
                                       });
  }
  
  
  if (toInsert.size() == 0 || toRemove.size() == 0)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    morph.SetValueFlag(false);
    movement.SetValueFlag(false);
    return;
  }
  
  PolymorphShape* shape = static_cast<PolymorphShape*>(cd->GetShape());
  

  for (auto& pd : toInsert)
  {
    shape->AddPixel(pd);
    pd->m_cellDescriptor = cd;
    pd->m_type = PixelDescriptor::CreatureType;
  }
  
  for (auto& pd : toRemove)
  {
    shape->RemovePixel(pd);
    pd->m_cellDescriptor = nullptr;
    pd->m_type = PixelDescriptor::Empty;
  }
 
  if (movement.isSet)
  {
    cd->Move(movement.value.destinationDesc);
  }
  
  cd->Finish();
  
  std::cout << "intermidiateChanges2:" << std::endl;
  
  for (auto & m : morph.value.vec)
  {
    std::cout << m.pos.Description() << "->" << m.delta.Description() << std::endl;
  }
  
  cd->PrintAsciiArt();
}
  
//**************************************************************************************************
void ProcessWhite(CellDescriptor* pd,
                  Vec2 pos,
                  Config::CellConfig* config,
                  const PixelDescriptorProvider::PixelMap& map, // ugly hack
                  Optional<Movement>& movement,
                  Optional<Morphing>& morph,
                  Optional<Action>& action)
{
  ProcessStaticWhite(pd, pos, config, map, movement, morph, action);
  return;
  
  if (pd->m_sleepCounter != 0)
  {
    pd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    pd->m_sleepCounter = pd->m_genom.m_sleepTime;
  }
  
  pd->AroundRandom([&](PixelDescriptor* targetPd, bool& stop)
                   {
                     if (targetPd->m_cellDescriptor && pd->IsMyFood(targetPd->m_cellDescriptor))
                     {
                       Attack(pd, targetPd->m_cellDescriptor, {0, 0}, action);
                       stop = true;
                     }
                   });
  
  if (action.isSet)
  {
    return;
  }
 
  // using memory to store destination (1, 2) and number of steps (2)
  if ((pd->parent->x == pd->m_m1 &&
       pd->parent->y == pd->m_m2) || pd->m_m1 == 0 || pd->m_m2 == 0 || pd->m_m3 > 100)
  {
    pd->m_m1 = cRandABInt(0, map[0].size());
    pd->m_m2 = cRandABInt(0, map[0].size());
    pd->m_m3 = 0;
  }
  
  pd->m_m3 += 1;
  
  DistanceMeasureResult distanceResult;
  
  GetMaxMinDistantPixel(Vec2(pd->m_m1, pd->m_m2), pd->GetShape(), distanceResult);
  
  if (distanceResult.targetIsInShape)
  {
    pd->m_m1 = 0;
    pd->m_m2 = 0;
    pd->m_m3 = 0;
    return;
  }
  
  Vec2 maxDistnacePdOffset = Vec2(pd->m_m1, pd->m_m2) - Vec2(distanceResult.maxPd->x, distanceResult.maxPd->y);
  Vec2 minDistnacePdOffset = Vec2(pd->m_m1, pd->m_m2) - Vec2(distanceResult.minPd->x, distanceResult.minPd->y);

  MorphingInternal destinationMorphing;
  
  bool found = false;
  
  if (pd->m_m3%2 == 0)
  {
    auto targetPd = distanceResult.minPd->Offset(minDistnacePdOffset.Normalize());
    if (targetPd->m_type == PixelDescriptor::Empty ||
        targetPd->m_cellDescriptor == pd)
    {
      if (!WillCauseTheGap(pd, distanceResult.minPd, minDistnacePdOffset.Normalize()))
      {
        MorphInternal m;
        m.offset = minDistnacePdOffset.Normalize();
        m.pd = distanceResult.minPd;
        destinationMorphing.push_back(m);
        found = true;
      }
    }
  }
  else
  {
    auto targetPd = distanceResult.maxPd->Offset(maxDistnacePdOffset.Normalize());
    if (targetPd->m_type == PixelDescriptor::Empty ||
        targetPd->m_cellDescriptor == pd)
    {
      if (!WillCauseTheGap(pd, distanceResult.maxPd, maxDistnacePdOffset.Normalize()))
      {
        MorphInternal m;
        m.offset = maxDistnacePdOffset.Normalize();
        m.pd = distanceResult.maxPd;
        destinationMorphing.push_back(m);
        found = true;
      }
    }
  }
  
  if (!found)
  {
    pd->ShapeRandom([&](PixelDescriptor* pixel, bool& stop)
                    {
                      Vec2 offsetFromTarget = Vec2(pd->m_m1, pd->m_m2) - Vec2(pixel->x, pixel->y);
                      offsetFromTarget = offsetFromTarget.Normalize();
                      auto targetPd = pixel->Offset(offsetFromTarget);
                      
                      if (targetPd->m_type == PixelDescriptor::Empty ||
                          targetPd->m_cellDescriptor == pd)
                      {
                        if (WillCauseTheGap(pd, pixel, offsetFromTarget))
                        {
                          return;
                        }
                        
                        MorphInternal m;
                        m.offset = offsetFromTarget;
                        m.pd = pixel;
                        destinationMorphing.push_back(m);
                        stop = true;
                        found = true;
                      }
                    });
  }
  
  if (!found)
  {
    distanceResult.maxPd->AroundRandom([&](PixelDescriptor* pixel, bool& stop)
                                       {
                                         Vec2 offsetFromTarget;
                                         distanceResult.maxPd->Offset(pixel, offsetFromTarget);
                                         
                                         if (pixel->m_cellDescriptor == pd)
                                         {
                                           if (WillCauseTheGap(pd, distanceResult.maxPd, offsetFromTarget))
                                           {
                                             return;
                                           }
                                           
                                           MorphInternal m;
                                           m.offset = offsetFromTarget;
                                           m.pd = distanceResult.maxPd;
                                           destinationMorphing.push_back(m);
                                           stop = true;
                                           found = true;
                                         }
                                       });
  }
  
  if (!found)
  {
    pd->m_m1 = 0;
    pd->m_m2 = 0;
    pd->m_m3 = 0;
    return;
  }
  
  MoveCellShape(pd, destinationMorphing, map, morph.value, movement.value);
  if (morph.value.vec.size() == 0)
  {
    return;
  }
  
  morph.SetValueFlag(true);
  if (movement.value.destinationDesc) {
    movement.SetValueFlag(true);
  }
  
  if (pd->m_volume <= 5)
  {
    pd->AroundRandom([&](PixelDescriptor* pixel, bool& stop){
      if (pixel->m_type == PixelDescriptor::Empty)
      {
        stop = true;
        Morph m;
        m.dir = Morph::Move;
        m.pos = Vec2(pixel->x, pixel->y);
        morph.value.vec.push_back(m);
        
        PolymorphShape* shape = static_cast<PolymorphShape*>(pd->m_shape.get());
        shape->AddPixel(pixel);
        AddPixelToCell(pd, pixel);
        pd->m_volume += 1;
      }
    });
  }
}
  

bool SimplePixelStepTo(CellDescriptor* cd,
                       const Vec2& target,
                       Optional<Movement>& m)
{
  if (cd->parent->GetPos() == target)
  {
    return false;
  }
  
  Vec2 normalizedTarget = target - cd->parent->GetPos();
  normalizedTarget = normalizedTarget.Normalize();
  
  AroundInDirection(cd->parent, normalizedTarget, [&](PixelDescriptor* targetPd, bool& stop)
                    {
                      if (cd->TestSpace(targetPd))
                      {
                        m.value.destinationDesc = targetPd;
                        m.SetValueFlag(true);
                        cd->Move(targetPd);
                        stop = true;
                        return;
                      }
                    });
  
  if (m == true) return true;
  
  return false;
}
  
  
bool RectShapeStepTo(CellDescriptor* cd,
                     const Vec2& target,
                     Optional<Movement>& m)
{
  RectShape* shape = static_cast<RectShape*>(cd->GetShape());
  Rect currentRect = shape->GetRect();
  
  if (target.In(shape->GetAABB()))
  {
    return false;
  }
  
  Vec2 normalizedTarget = target - cd->parent->GetPos();
  normalizedTarget = normalizedTarget.Normalize();
  
  AroundInDirection(cd->parent, normalizedTarget, [&](PixelDescriptor* targetPd, bool& stop)
                    {
                      RectShape testShape(targetPd, currentRect.origin, currentRect.size);
                      
                      if (TestSpace(cd, &testShape))
                      {
                        m.value.destinationDesc = targetPd;
                        m.SetValueFlag(true);
                        stop = true;
                        return;
                      }
                    });
  
  if (m.isSet == false)
  {
    return false;
  }
  
  cd->Shape([&](PixelDescriptor* targetPd, bool& stop)
            {
              targetPd->m_cellDescriptor = nullptr;
              targetPd->m_type = PixelDescriptor::Empty;
            });
  cd->GetShape()->SetPosition(m.value.destinationDesc);
  cd->parent = m.value.destinationDesc;
  cd->Finish();
  
  return true;
}
  
bool FixShapeStepTo(CellDescriptor* cd,
                    const Vec2& target,
                    Optional<Movement>& m,
                    Optional<Morphing>& morph)
{
  Vec2 normalizedTarget = target - cd->parent->GetPos();
  normalizedTarget = normalizedTarget.Normalize();
  
  if (normalizedTarget == Vec2())
  {
    return false;
  }
  
  AroundInDirection(cd->parent, normalizedTarget, [&](PixelDescriptor* targetPd, bool& stop)
                    {
                      Vec2 offset;
                      
                      if (!cd->parent->Offset(targetPd, offset)) {
                        assert(0);
                      }
                      
                      PixelDescriptor::Vec newShapePixels;
                      cd->Shape([&](PixelDescriptor* pd, bool& stop) {
                        newShapePixels.push_back(pd->Offset(offset));
                      });
                      
                      PolymorphShape testShape(targetPd, newShapePixels);
                      
                      if (TestSpace(cd, &testShape))
                      {
                        m.value.destinationDesc = targetPd;
                        m.SetValueFlag(true);
                        stop = true;
                        return;
                      }
                    });
  
  if (m.isSet == false)
  {
    return false;
  }
  
  cd->Shape([&](PixelDescriptor* targetPd, bool& stop)
            {
              targetPd->m_cellDescriptor = nullptr;
              targetPd->m_type = PixelDescriptor::Empty;
            });
  
  Vec2 offset = m.value.destinationDesc->GetPos() - cd->parent->GetPos();
  
  PixelDescriptor::Vec newShapePixels;
  cd->Shape([&](PixelDescriptor* pd, bool& stop){
    newShapePixels.push_back(pd->Offset(offset));
    
    Morph m;
    m.dir = Morph::Move;
    m.pos = pd->GetPos();
    m.delta = offset;
    morph.value.vec.push_back(m);
    morph.SetValueFlag(true);
  });
  
  PolymorphShape* shape = static_cast<PolymorphShape*>(cd->GetShape());
  shape->SetPixels(m.value.destinationDesc, newShapePixels);
  cd->GetShape()->SetPosition(m.value.destinationDesc);
  cd->parent = m.value.destinationDesc;
  cd->Finish();
  
  return true;
}
  
  
bool AmorphShapeStepTo(CellDescriptor* cd,
                    const Vec2& target,
                    Optional<Movement>& movement,
                    Optional<Morphing>& morph)
{
  DistanceMeasureResult distanceResult;
  
  GetMaxMinDistantPixel(target, cd->GetShape(), distanceResult);
  
  if (distanceResult.targetIsInShape)
  {
    return false;
  }
  
  std::vector<PixelDescriptor*> toRemove;
  
  distanceResult.maxPd->AroundRandom([&](PixelDescriptor* pd, bool& stop)
                                     {
                                       if (pd->m_cellDescriptor != cd)
                                       {
                                         return;
                                       }
                                       
                                       Morph m;
                                       m.dir = Morph::Inside;
                                       m.pos = Vec2(distanceResult.maxPd->x, distanceResult.maxPd->y);
                                       distanceResult.maxPd->Offset(pd, m.delta);
                                       morph.value.vec.push_back(m);
                                       morph.SetValueFlag(true);
                                       
                                       if (distanceResult.maxPd == cd->parent)
                                       {
                                         movement.value.destinationDesc = pd;
                                         movement.SetValueFlag(true);
                                       }
                                       
                                       toRemove.push_back(distanceResult.maxPd);
                                       
                                       stop = true;
                                       
                                     });
  
  if (morph.value.vec.size() == 0)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    morph.SetValueFlag(false);
    movement.SetValueFlag(false);
    return false;
  }
  
  Vec2 normalizedMinDist = distanceResult.minDist.Normalize();
  
  std::vector<PixelDescriptor*> toInsert;
  
  {
    auto tagetPd = distanceResult.minPd->Offset(normalizedMinDist);
    if (FreePixelHasMyParts(cd, tagetPd, distanceResult.minPd))
    {
      Morph m;
      m.dir = Morph::Outside;
      m.pos = tagetPd->GetPos();
      m.delta = -normalizedMinDist;
      morph.value.vec.push_back(m);
      morph.SetValueFlag(true);
      
      toInsert.push_back(tagetPd);
    }
  }
  
  if (toInsert.size() == 0)
  {
    distanceResult.minPd->AroundRandom([&](PixelDescriptor* pd, bool& stop)
                                       {
                                         if (!FreePixelHasMyParts(cd, pd, distanceResult.minPd))
                                         {
                                           return;
                                         }
                                         
                                         Vec2 offset;
                                         distanceResult.minPd->Offset(pd, offset);
                                         
                                         Morph m;
                                         m.dir = Morph::Outside;
                                         m.pos = pd->GetPos();
                                         m.delta = -offset;
                                         morph.value.vec.push_back(m);
                                         morph.SetValueFlag(true);
                                         
                                         toInsert.push_back(pd);
                                         
                                         stop = true;
                                       });
  }
  
  
  if (toInsert.size() == 0 || toRemove.size() == 0)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    morph.SetValueFlag(false);
    movement.SetValueFlag(false);
    return false;
  }
  
  PolymorphShape* shape = static_cast<PolymorphShape*>(cd->GetShape());
  
  
  for (auto& pd : toInsert)
  {
    shape->AddPixel(pd);
    pd->m_cellDescriptor = cd;
    pd->m_type = PixelDescriptor::CreatureType;
  }
  
  for (auto& pd : toRemove)
  {
    shape->RemovePixel(pd);
    pd->m_cellDescriptor = nullptr;
    pd->m_type = PixelDescriptor::Empty;
  }
  
  if (movement.isSet)
  {
    cd->Move(movement.value.destinationDesc);
  }
  
  cd->Finish();
  
  return true;
}
  
  
bool StepTo(CellDescriptor* cd,
              const Vec2& targetPd,
              Optional<Movement>& m,
              Optional<Morphing>& morph)
{
  if (cd->GetShapeType() == ShapeType::eShapeTypeSinglePixel)
  {
    return SimplePixelStepTo(cd, targetPd, m);
  }
  else if (cd->GetShapeType() == ShapeType::eShapeTypeRect)
  {
    return RectShapeStepTo(cd, targetPd, m);
  }
  else if (cd->GetShapeType() == ShapeType::eShapeTypeFixed)
  {
    return FixShapeStepTo(cd, targetPd, m, morph);
  }
  else if (cd->GetShapeType() == ShapeType::eShapeTypeAmorph)
  {
    return AmorphShapeStepTo(cd, targetPd, m, morph);
  }
  else
  {
    assert(0);
  }
  
  return false;
}
  
void ProcessUniversalCell(CellDescriptor* cd,
                         Vec2 pos,
                         Config* config,
                         const PixelDescriptorProvider::PixelMap& map, // ugly hack
                         Optional<Movement>& movement,
                         Optional<Action>& action,
                         Optional<Morphing>& morph,
                         Optional<ChangeRect>& changeRect)
{
  if (cd->m_sleepCounter != 0)
  {
    cd->m_sleepCounter -= 1;
    return;
  }
  else
  {
    cd->m_sleepCounter = cd->m_genom.m_sleepTime;
  }
  
  // using memory to store destination (1, 2) and number of steps (2)
  if ((cd->parent->x == cd->m_m1 &&
       cd->parent->y == cd->m_m2) || cd->m_m1 == 0 || cd->m_m2 == 0 || cd->m_m3 == 0)
  {
    cd->m_m1 = cRandABInt(0, map[0].size());
    cd->m_m2 = cRandABInt(0, map[0].size());
    cd->m_m3 = 50;
  }
  
  cd->m_m3 -= 1;
  
  
  PixelDescriptor* enemyPd = nullptr;
  PixelDescriptor* frientPd = nullptr;
  PixelDescriptor* foodPd = nullptr;
  cd->AroundRandom([&](PixelDescriptor* targetPd, bool& stop) {
    if(targetPd && targetPd->m_type == PixelDescriptor::CreatureType)
    {
      assert(targetPd->m_cellDescriptor);
      
      if (targetPd->m_cellDescriptor == cd)
        return;
      
      if (targetPd->m_cellDescriptor->m_genom.m_groupId & cd->m_genom.m_dangerGroupId)
      {
        enemyPd = targetPd;
        stop = true;
        return;
      }
      
      if (cd->IsMyFood(targetPd->m_cellDescriptor))
      {
        foodPd = targetPd;
        return;
      }
      
      if (targetPd->m_cellDescriptor->m_genom.m_groupId & cd->m_genom.m_friendGroupId)
      {
        frientPd = targetPd;
        return;
      }
    }
  });
  
  if (enemyPd)
  {
    Vec2 offset = enemyPd->GetPos() - cd->parent->GetPos();
    offset *= -10;
    Vec2 targetPos = cd->parent->GetPos() + offset;
    cd->m_m1 = targetPos.x;
    cd->m_m2 = targetPos.y;
    cd->m_m3 = 3;
    if (StepTo(cd, targetPos, movement, morph))
    {
      return;
    }
  }
  
  if (foodPd)
  {
    Vec2 offset = foodPd->GetPos() - cd->parent->GetPos();
    cd->m_m1 = offset.x;
    cd->m_m2 = offset.y;
    cd->m_m3 = 3;
    Attack(cd, foodPd->m_cellDescriptor, offset.Normalize(), action);
    return;
  }
  
  if (frientPd)
  {
    Vec2 offset = frientPd->GetPos() - cd->parent->GetPos();
    offset *= -10;
    Vec2 targetPos = cd->parent->GetPos() + offset;
    cd->m_m1 = targetPos.x;
    cd->m_m2 = targetPos.y;
    cd->m_m3 = 3;
    
    if (StepTo(cd, targetPos, movement, morph))
    {
      return;
    }
  }
  
  if (cd->m_m1 != 0 && cd->m_m2 != 0)
  {
    Vec2 targetPos = Vec2(cd->m_m1, cd->m_m2);
    if (!StepTo(cd, targetPos, movement, morph))
    {
      cd->m_m1 = 0;
      cd->m_m2 = 0;
      cd->m_m3 = 0;
    }
    else
    {
      return;
    }
  }
}
  

//**************************************************************************************************
void ProcessCell(CellDescriptor* d,
                 Vec2 pos,
                 Config* config,
                 const PixelDescriptorProvider::PixelMap& map, // ugly hack
                 Optional<Movement>& m,
                 Optional<Action>& a,
                 Optional<Morphing>& morph,
                 Optional<ChangeRect>& changeRect)
{
  Transaction t;
  const PhysicalFloat& light = d->parent->m_physicalDesc.light;
  int h = d->m_genom.m_lightFood * light;
  if (h > 0)
  {
    h *= light;
  }
  t.m_health = h;
  d->nextTurnTransaction.push_back(t);
  
  ProcessUniversalCell(d,
                       pos,
                       nullptr,
                       map,
                       m,
                       a,
                       morph,
                       changeRect);

}

} // namespace komorki;

