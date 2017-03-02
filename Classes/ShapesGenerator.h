//
//  ShapesGenerator.h
//  Komorki
//
//  Created by Anton Simakov on 21.10.16.
//
//

#ifndef __Komorki__ShapesGenerator__
#define __Komorki__ShapesGenerator__

#include <list>
#include "PixelMap.hpp"
#include "CellShapes.h"
#include "Genom.h"

namespace komorki
{
  class PixelDescriptor;
  
  class ShapesGenerator
  {
  public:
    
    struct ResultItem
    {
      IShape::Ptr shape;
      ShapeType type;
      PixelMap::Ptr map;
    };
    
    ShapesGenerator();
    ResultItem GenerateNext();
  private:
    void GenerateShape(PixelDescriptor* pd, IShape::Ptr& outShape, ShapeType& outShapeType);
    PixelMap::Ptr m_internalMap;
  };
}

#endif /* defined(__Komorki__ShapesGenerator__) */
