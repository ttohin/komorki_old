//
//  CellShapesHelper.h
//  prsv
//
//  Created by Anton Simakov on 05.07.16.
//
//

#ifndef __prsv__CellShapesHelper__
#define __prsv__CellShapesHelper__

#include <list>
#include "CellShapes.h"
#include "PixelWorld.h"
#include "CellDescriptor.h"


namespace komorki
{
  struct MorphInternal
  {
    GreatPixel* pd;
    Vec2 offset;
  };
  
  struct DistanceMeasureResult
  {
    Vec2 maxDist;
    Vec2 minDist;
    GreatPixel* minPd = nullptr;
    GreatPixel* maxPd = nullptr;
    bool targetIsInShape = false;
  };
  
  typedef std::list<MorphInternal> MorphingInternal;
  
  bool MoveCellShape(CellDescriptor* cd,
                            const MorphingInternal& sourceChanges,
                            const PixelWorld::PixelMap& map, // ugly hack
                            Morphing& outMorphing,
                            Movement& outMovement);
  bool AddPixelToCell(CellDescriptor* cd, GreatPixel* pd);
  bool RemovePixelFromCell(CellDescriptor* cd, GreatPixel* pd);
  
  void GetMaxMinDistantPixel(Vec2 pos, const IShape* shape, DistanceMeasureResult& result);
  bool FreePixelHasMyParts(CellDescriptor* cd, GreatPixel* testPd, GreatPixel* ignorePd);
  
  bool WillCauseTheGap(CellDescriptor* cd, GreatPixel* pd, const Vec2& offset);
  bool WillCauseTheGap(CellDescriptor* cd, GreatPixel* missingPd);
}

#endif /* defined(__prsv__CellShapesHelper__) */
