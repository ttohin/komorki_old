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
#include "PixelDescriptorProvider.h"
#include "CellDescriptor.h"


namespace komorki
{
  struct MorphInternal
  {
    PixelDescriptor* pd;
    Vec2 offset;
  };
  
  struct DistanceMeasureResult
  {
    Vec2 maxDist;
    Vec2 minDist;
    PixelDescriptor* minPd = nullptr;
    PixelDescriptor* maxPd = nullptr;
    bool targetIsInShape = false;
  };
  
  typedef std::list<MorphInternal> MorphingInternal;
  
  bool MoveCellShape(CellDescriptor* cd,
                            const MorphingInternal& sourceChanges,
                            const PixelDescriptorProvider::PixelMap& map, // ugly hack
                            Morphing& outMorphing,
                            Movement& outMovement);
  bool AddPixelToCell(CellDescriptor* cd, PixelDescriptor* pd);
  bool RemovePixelFromCell(CellDescriptor* cd, PixelDescriptor* pd);
  
  void GetMaxMinDistantPixel(Vec2 pos, const IShape* shape, DistanceMeasureResult& result);
  bool FreePixelHasMyParts(CellDescriptor* cd, PixelDescriptor* testPd, PixelDescriptor* ignorePd);
  
  bool WillCauseTheGap(CellDescriptor* cd, PixelDescriptor* pd, const Vec2& offset);
}

#endif /* defined(__prsv__CellShapesHelper__) */
