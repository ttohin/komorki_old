//
//  GenomsGenerator.cpp
//  Komorki
//
//  Created by Anton Simakov on 21/10/2016.
//
//

#include "GenomsGenerator.hpp"
#include "b2Utilites.h"

namespace komorki
{
  void GenomsGenerator::GenerateShape(PixelDescriptor* pd, IShape::Ptr& outShape, ShapeType& outShapeType)
  {
    std::shared_ptr<IShape> shape;
    
    unsigned int numberOfShapes = 2;
    unsigned int shapeIndex = cRandABInt(0, numberOfShapes);
    shapeIndex = 3;
    if (0 == shapeIndex)
    {
      shape = std::make_shared<SinglePixel>(pd);
      outShapeType = ShapeType::eShapeTypeSinglePixel;
    }
    else if (1 == shapeIndex)
    {
      unsigned int w = cRandABInt(2, 4);
      unsigned int h = cRandABInt(2, 4);
      shape = std::make_shared<RectShape>(pd, Vec2(0, 0), Vec2(w, h));
      outShapeType = ShapeType::eShapeTypeRect;
    }
    else if (2 == shapeIndex)
    {
      unsigned int numberOfPixels = cRandABInt(10, 12);
      shape = std::make_shared<PolymorphShape>(pd, numberOfPixels);
      outShapeType = ShapeType::eShapeTypeAmorph;
    }
    else if (3 == shapeIndex)
    {
      PixelDescriptor::Vec pixels;
      pixels.push_back(pd->Offset(cRandAorB(-1, 1), 0));
      pixels.push_back(pd->Offset(0, cRandAorB(-1, 1)));
      pixels.push_back(pd);
      shape = std::make_shared<PolymorphShape>(pd, pixels);
      outShapeType = ShapeType::eShapeTypeFixed;
    }
    else if (4 == shapeIndex)
    {
      unsigned int numberOfPixels = cRandABInt(2, 4);
      shape = std::make_shared<PolymorphShape>(pd, numberOfPixels);
      outShapeType = ShapeType::eShapeTypePolymorph;
    }
    else
    {
      assert(false);
    }
    
    outShape = shape;
  }
  
  GenomsGenerator::GenomsGenerator()
  : m_internalMap({100, 100})
  {
  }
  
  GenomsGenerator::ShapesList GenomsGenerator::Generate()
  {
    ShapesList result;
    IShape::Ptr shape;
    ShapeType shapeType;
    GenerateShape(m_internalMap.GetDescriptor(5, 5), shape, shapeType);
    
    result.push_back(shape);
    
    return result;
  }
}
