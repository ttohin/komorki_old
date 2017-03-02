//
//  CellShapesHelper.cpp
//  prsv
//
//  Created by Anton Simakov on 05.07.16.
//
//

#include "CellShapesHelper.h"
#include "Random.h"
#include <iostream>
#include <assert.h>
#include <limits>
#include <cmath>

namespace komorki
{
  
Vec2 RandVec(const Vec2& source)
  {
    Vec2 r;
    r.x = cRandABInt(-1, 1);
    r.y = cRandABInt(-1, 1);
    
    if (r == -source || r == Vec2())
    {
      r = source;
    }
    return r;
  }
  
bool PushShapePixel(CellDescriptor* cd,
                    GreatPixel* pd,
                    const Vec2& offset,
                    MorphingInternal& changes)
  {
    if (changes.size() > 100) {
      assert(0);
    }
    
    if (pd->m_type == GreatPixel::Empty)
    {
      return true;
    }
    
    auto targetPd = pd->Offset(offset);
    if (targetPd->m_type == GreatPixel::Empty
        || (targetPd->m_type == GreatPixel::CreatureType
            && targetPd->m_cellDescriptor == cd
            && !targetPd->pushHandled))
    {
      MorphInternal m;
      m.pd = pd;
      m.offset = offset;
      pd->pushHandled = true;
      changes.push_back(m);
      
      return PushShapePixel(cd, targetPd, RandVec(offset), changes);
    }
    else
    {
      targetPd = nullptr;
      pd->AroundRandom([&](GreatPixel* localPd, bool& stop)
                              {
                                if (localPd->m_type == GreatPixel::Empty ||
                                    (localPd->m_type == GreatPixel::CreatureType
                                     && localPd->m_cellDescriptor == cd
                                     && !localPd->pushHandled))
                                {
                                  targetPd = localPd;
                                  stop = true;
                                }
                              });
      
      if (targetPd == nullptr)
      {
        return false;
      }
      
      MorphInternal m;
      m.pd = pd;
      pd->pushHandled = true;
      bool offsetResult = pd->Offset(targetPd, m.offset);
      assert(offsetResult);
      changes.push_back(m);
      
      return PushShapePixel(cd, targetPd, RandVec(offset), changes);
    }
    
    return true;
  }

bool MoveCellShape(CellDescriptor* cd,
                     const MorphingInternal& sourceChanges,
                            const PixelWorld::PixelMap& map, // ugly hack
                     Morphing& outMorphing,
                     Movement& outMovement)
{
  assert(sourceChanges.size() == 1);
  
  MorphingInternal intermidiateChanges;
  Vec2 baseOffset;
  GreatPixel* newBasePd = nullptr;
  for (const auto& change : sourceChanges) {
    if (change.pd == cd->parent)
    {
      baseOffset = change.offset;
      newBasePd = change.pd->Offset(change.offset);
      outMovement.destinationDesc = newBasePd;
      break;
    }
  }

  MorphingInternal intermidiateChanges2;
  for (const auto& change : sourceChanges)
  {
    if (!PushShapePixel(cd, change.pd, change.offset, intermidiateChanges2))
    {
      return false;
    }
  }
  
  PolymorphShape* shape = static_cast<PolymorphShape*>(cd->m_shape.get());
  MorphingInternal intermidiateChanges3;
  
  shape->ForEach([&](GreatPixel* pd, bool& stop)
  {
    Morph m;
    m.dir = Morph::Move;
    m.pos = Vec2(pd->x, pd->y);
    for (auto & c : intermidiateChanges2)
    {
      if (c.pd == pd)
      {
        m.delta += c.offset;
      }
    }
    
    if (m.delta.x != 0 || m.delta.y != 0) {
      outMorphing.vec.push_back(m);
    }
    
  });
  
  
  if (newBasePd)
  {
    shape->SetPosition(newBasePd);
    cd->parent = newBasePd;
  }
  
  unsigned int volume = cd->m_volume;
  GreatPixel::Vec pdStack;
  
  for (const auto& change : intermidiateChanges2)
  {
    GreatPixel* newPd = change.pd->Offset(change.offset);
    
    GreatPixel::Vec::iterator it = std::find(pdStack.begin(), pdStack.end(), change.pd);
    if (it == pdStack.end())
    {
      shape->RemovePixel(change.pd);
      RemovePixelFromCell(cd, change.pd);
    }
    else
    {
      pdStack.erase(it);
    }

    if (!shape->IsInShape(newPd))
    {
      shape->AddPixel(newPd);
    }
    else
    {
      pdStack.push_back(newPd);
    }
  }
  
//  std::cout << "intermidiateChanges2:" << std::endl;
//  for (auto& c : intermidiateChanges2)
//  {
//    std::cout << c.pd->x << "," <<c.pd->y << " -> " << c.offset.Description() << std::endl;
//  }
  
  assert(pdStack.empty());
  assert(volume == shape->Size());
  
  cd->Finish();
  
//  cd->PrintAsciiArt();
  
  assert(volume == cd->m_volume);
  
  for (auto & m : outMorphing.vec)
  {
    auto pd = map[m.pos.x + m.delta.x][m.pos.y + m.delta.y];
    assert(pd->m_cellDescriptor == cd);
    assert(pd->m_type == GreatPixel::CreatureType);
  }
  
  return true;
}
  
  bool AddPixelToCell(CellDescriptor* cd, GreatPixel* pd)
  {
    pd->m_type = GreatPixel::CreatureType;
    pd->m_cellDescriptor = cd;
    return true;
  }
  
  bool RemovePixelFromCell(CellDescriptor* cd, GreatPixel* pd)
  {
    pd->m_type = GreatPixel::Empty;
    pd->m_cellDescriptor = nullptr;
    return true;
  }
  
  void GetMaxMinDistantPixel(Vec2 pos, const IShape* shape, DistanceMeasureResult& result)
  {
    GreatPixel* minDistPd = nullptr;
    float minDist = std::numeric_limits<float>::max();
    GreatPixel* maxDistPd = nullptr;
    float maxDist = 0.f;
    shape->ForEach([&](GreatPixel* pixel, bool& stop)
              {
                float dist = std::powf(pos.x - pixel->x, 2) + std::powf(pos.y - pixel->y, 2);
                if (dist <= minDist)
                {
                  minDist = dist;
                  minDistPd = pixel;
                  result.minDist = pos - pixel->GetPos();
                }
                if (dist >= maxDist)
                {
                  maxDist = dist;
                  maxDistPd = pixel;
                  result.maxDist = pos + pixel->GetPos();
                }
                
                if (Vec2(pixel->x, pixel->y) == pos)
                {
                  result.targetIsInShape = true;
                }
              });
    
    result.maxPd = maxDistPd;
    result.minPd = minDistPd;
  }
  
  
  bool FreePixelHasMyParts(CellDescriptor* cd, GreatPixel* testPd, GreatPixel* ignorePd)
  {
    if (!testPd->IsEmpty())
    {
      return false;
    }
    
    bool result = false;
    
    testPd->Around([&](GreatPixel* pd, bool& stop){
      if (pd->m_cellDescriptor == cd && pd != ignorePd)
      {
        result = true;
        stop = true;
        return;
      }
    });
    
    return result;
  }
  
  bool FindNextCellPixel(CellDescriptor* cd,
                         GreatPixel* pd,
                         GreatPixel* include,
                         GreatPixel* except,
                         GreatPixel::Vec& pdStack)
  {
    if (except == pd)
    {
      return true;
    }
    
    GreatPixel::Vec::iterator it = std::find(pdStack.begin(), pdStack.end(), pd);
    if (it != pdStack.end())
    {
      return true;
    }
    else
    {
      pdStack.push_back(pd);
    }
    
    pd->Around([&](GreatPixel* targetPd, bool& stop) {
      
      if (include == targetPd)
      {
        FindNextCellPixel(cd, targetPd, include, except, pdStack);
      }
      else if (targetPd->m_cellDescriptor == cd)
      {
        FindNextCellPixel(cd, targetPd, include, except, pdStack);
      }
      
    });
    
    return true;
  }
  
  bool WillCauseTheGap(CellDescriptor* cd, GreatPixel* pd, const Vec2& offset)
  {
    GreatPixel::Vec pdStack;
    GreatPixel* basePd = cd->parent;
    if (pd == basePd)
    {
      basePd = pd->Offset(offset);
    }
    
    FindNextCellPixel(cd, basePd, pd->Offset(offset), pd, pdStack);
    
    if (pd->Offset(offset)->m_cellDescriptor == cd)
    {
      pdStack.push_back(pd);
    }
    
    assert(!pdStack.empty());
    assert(pdStack.size() <= cd->m_volume);
    
    if (pdStack.size() < cd->m_volume)
    {
      return true;
    }
    return false;
  }
  
  bool WillCauseTheGap(CellDescriptor* cd, GreatPixel* missingPd)
  {
    PolymorphShape* shape = static_cast<PolymorphShape*>(cd->GetShape());
    
    GreatPixel* pixelInShape = nullptr;
    
    shape->ForEach([&](GreatPixel* pd, bool& stop){
      if (pd != missingPd)
      {
        pixelInShape = pd;
        stop = true;
      }
    });
    
    assert(pixelInShape);
    
    GreatPixel::Vec pdStack;
    FindNextCellPixel(cd, pixelInShape, nullptr, missingPd, pdStack);
  
    return pdStack.size() != (shape->Size() - 1);
  }

}

