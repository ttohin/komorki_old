//
//  CreaturesLogic.cpp
//  komorki
//
//  Created by ttohin on 21.03.15.
//
//

#include "CellsLogic.h"
#include "Random.h"
#include "GreatPixel.h"
#include "CellDescriptor.h"
#include "CellShapesHelper.h"
#include <iostream>
#include <assert.h>

namespace komorki
{
  
void AroundInDirection(GreatPixel* cd, const Vec2& direction, const PerPixelFunc& func)
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
  
bool TestSpace(CellDescriptor* cd, IShape* shape)
{
  bool result = true;
  shape->ForEach([&](GreatPixel* pd, bool& stop)
                 {
                   if (pd->m_type == GreatPixel::Empty) return;
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
                 const PixelWorld::PixelMap& map, // ugly hack
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
    cd->Shape([](GreatPixel* pd, bool& stop){
      pd->m_cellDescriptor = nullptr;
      pd->m_type = GreatPixel::Empty;
    });
    
    RectShape* shape = static_cast<RectShape*>(cd->GetShape());
    shape->SetAABB(newRect.origin, newRect.size);
    
    changeRect.value.rect =  shape->GetAABB();
    changeRect.SetValueFlag(true);
    
    cd->Finish();
    
    cd->m_m4 = (cd->m_m4 + 1) % 2;
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
  
  AroundInDirection(cd->parent, normalizedTarget, [&](GreatPixel* targetPd, bool& stop)
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
  
  AroundInDirection(cd->parent, normalizedTarget, [&](GreatPixel* targetPd, bool& stop)
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
  
  cd->CleanSpace();
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
  
  AroundInDirection(cd->parent, normalizedTarget, [&](GreatPixel* targetPd, bool& stop)
                    {
                      Vec2 offset;
                      
                      if (!cd->parent->Offset(targetPd, offset)) {
                        assert(0);
                      }
                      
                      GreatPixel::Vec newShapePixels;
                      cd->Shape([&](GreatPixel* pd, bool& stop) {
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
  
  cd->Shape([&](GreatPixel* targetPd, bool& stop)
            {
              targetPd->m_cellDescriptor = nullptr;
              targetPd->m_type = GreatPixel::Empty;
            });
  
  Vec2 offset = m.value.destinationDesc->GetPos() - cd->parent->GetPos();
  
  GreatPixel::Vec newShapePixels;
  cd->Shape([&](GreatPixel* pd, bool& stop){
    newShapePixels.push_back(pd->Offset(offset));
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
  
  GreatPixel* toRemove = nullptr;
  
  distanceResult.maxPd->AroundRandom([&](GreatPixel* pd, bool& stop)
                                     {
                                       if (pd->m_cellDescriptor != cd || WillCauseTheGap(cd, distanceResult.maxPd))
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
                                       
                                       toRemove = distanceResult.maxPd;
                                       
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
  
  GreatPixel* toInsert = nullptr;
  
  bool performStepToTarget = cBoolRandPercent(0.3);
  if (!performStepToTarget)
  {
    cd->AroundRandom([&](GreatPixel* targetPd, bool& stop) {
      
      if (targetPd->IsEmpty())
      {
        targetPd->Around([&](GreatPixel* cellPd, bool& stop){
          
          if (cellPd->m_cellDescriptor == cd && cellPd != toRemove)
          {
            Morph m;
            m.dir = Morph::Outside;
            m.pos = targetPd->GetPos();
            targetPd->Offset(cellPd, m.delta);
            morph.value.vec.push_back(m);
            morph.SetValueFlag(true);
            
            toInsert = targetPd;
            
            stop = true;
          }
          
        });
        
        if (toInsert != nullptr) {
          stop = true;
        }
      }
      
    });
  }
  
  Vec2 normalizedMinDist = distanceResult.minDist.Normalize();
  
  if (toInsert == nullptr)
  {
    auto tagetPd = distanceResult.minPd->Offset(normalizedMinDist);
    assert(normalizedMinDist != Vec2());
    if (FreePixelHasMyParts(cd, tagetPd, distanceResult.minPd))
    {
      Morph m;
      m.dir = Morph::Outside;
      m.pos = tagetPd->GetPos();
      m.delta = -normalizedMinDist;
      morph.value.vec.push_back(m);
      morph.SetValueFlag(true);
      
      toInsert = tagetPd;
    }
  }
  
  if (toInsert == nullptr)
  {
    AroundInDirection(distanceResult.minPd, normalizedMinDist, [&](GreatPixel* targetPd, bool& stop)
                      {
                        if (!FreePixelHasMyParts(cd, targetPd, distanceResult.minPd) &&
                            !FreePixelHasMyParts(cd, targetPd, toRemove) )
                        {
                          return;
                        }
                        
                        Vec2 offset;
                        distanceResult.minPd->Offset(targetPd, offset);
                        
                        Morph m;
                        m.dir = Morph::Outside;
                        m.pos = targetPd->GetPos();
                        m.delta = -offset;
                        morph.value.vec.push_back(m);
                        morph.SetValueFlag(true);
                        
                        toInsert = targetPd;
                        
                        stop = true;
                      });
  }
  
  if (toInsert == nullptr)
  {
    distanceResult.minPd->AroundRandom([&](GreatPixel* pd, bool& stop)
                                       {
                                         if (!FreePixelHasMyParts(cd, pd, distanceResult.minPd) &&
                                             !FreePixelHasMyParts(cd, pd, toRemove) )
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
                                         
                                         toInsert = pd;
                                         
                                         stop = true;
                                       });
  }
  
  if (toInsert == nullptr || toRemove == nullptr)
  {
    cd->m_m1 = 0;
    cd->m_m2 = 0;
    cd->m_m3 = 0;
    morph.SetValueFlag(false);
    movement.SetValueFlag(false);
    return false;
  }
  
  assert(toInsert != nullptr && toRemove != nullptr);
  
  PolymorphShape* shape = static_cast<PolymorphShape*>(cd->GetShape());
  
  cd->CleanSpace();
  
  shape->AddPixel(toInsert);
  shape->RemovePixel(toRemove);
  
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
  
bool GrowAmorphCellIfNeeded(CellDescriptor* cd,
                            Optional<Morphing>& morph)
{
  if (cd->m_genom.m_shapeType == ShapeType::eShapeTypeAmorph
      && cd->m_genom.m_volume != cd->GetShape()->Size())
  {
    cd->AroundRandom([&](GreatPixel* targetPd, bool& stop) {
      
      Morph m;
      m.dir = Morph::Outside;
      m.pos = targetPd->GetPos();
      
      if (targetPd->IsEmpty())
      {
        targetPd->Around([&](GreatPixel* findCellTargetPd, bool& stop) {
          if (findCellTargetPd->m_cellDescriptor == cd)
          {
            stop = true;
            targetPd->Offset(findCellTargetPd, m.delta); // calculate offset for closer cell pixel
          }
        });
        
        targetPd->m_cellDescriptor = cd;
        
        PolymorphShape* shape = static_cast<PolymorphShape*>(cd->GetShape());
        shape->AddPixel(targetPd);
        targetPd->m_type = GreatPixel::CreatureType;
        
        morph.value.vec.push_back(m);
        morph.SetValueFlag(true);
        
        stop = true;
      }
    });
    
    if (morph == true)
    {
      return true;
    }
  }
  
  return false;
}
  
void ProcessUniversalCell(CellDescriptor* cd,
                         Vec2 pos,
                         const PixelWorld::PixelMap& map, // ugly hack
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
  
  if (GrowAmorphCellIfNeeded(cd, morph))
  {
    return;
  }
  
  // using memory to store destination (1, 2) and number of steps (2)
  if ((cd->parent->x == cd->m_m1 &&
       cd->parent->y == cd->m_m2) || cd->m_m1 == 0 || cd->m_m2 == 0 || cd->m_m3 == 0)
  {
    cd->m_m1 = cRandABInt(0, map.size());
    cd->m_m2 = cRandABInt(0, map.size());
    cd->m_m3 = 50;
  }
  
  cd->m_m3 -= 1;
  
  GreatPixel* enemyPd = nullptr;
  GreatPixel* frientPd = nullptr;
  GreatPixel* foodPd = nullptr;
  cd->AroundRandom([&](GreatPixel* targetPd, bool& stop) {
    if(targetPd && targetPd->m_type == GreatPixel::CreatureType)
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
                 const PixelWorld::PixelMap& map, // ugly hack
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
  
  h += d->m_genom.m_passiveHealthIncome;
  
  t.m_health = h;
  
  d->nextTurnTransaction.push_back(t);
  
  //d->PrintAsciiArt();
  
  ProcessUniversalCell(d,
                       pos,
                       map,
                       m,
                       a,
                       morph,
                       changeRect);
  
  //d->PrintAsciiArt();
}

} // namespace komorki;

