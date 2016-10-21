//
//  GenomsGenerator.hpp
//  Komorki
//
//  Created by Anton Simakov on 21/10/2016.
//
//

#ifndef GenomsGenerator_hpp
#define GenomsGenerator_hpp

#include <list>
#include "PixelMap.hpp"
#include "CellShapes.h"
#include "CellDescriptor.h"

namespace komorki
{
  class GenomsGenerator
  {
  public:
    
    typedef std::list<IShape::Ptr> ShapesList;
    
    GenomsGenerator();
    ShapesList Generate();
  private:
    void GenerateShape(PixelDescriptor* pd, IShape::Ptr& outShape, ShapeType& outShapeType);
    PixelMap m_internalMap;
  };
}

#endif /* GenomsGenerator_hpp */
