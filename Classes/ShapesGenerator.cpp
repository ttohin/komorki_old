//
//  ShapesGenerator.cpp
//  Komorki
//
//  Created by Anton Simakov on 21.10.16.
//
//

#include "ShapesGenerator.h"
#include "Random.h"
#include "PixelDescriptor.h"

namespace komorki
{
  IShape::Ptr ContinousShape(PixelDescriptor* pd)
  {
    PixelDescriptor::Vec pixels;

    int numberOfPixels = 3;
    PixelDescriptor* currentPd = pd;
    pixels.push_back(pd);
    pd->m_type = PixelDescriptor::CreatureType;
    while (1)
    {
      bool foundEmpty = false;
      currentPd->AroundRandom([&](PixelDescriptor* p, bool& stop){
        if (p->IsEmpty())
        {
          p->m_type = PixelDescriptor::CreatureType;
          pixels.push_back(p);
          currentPd = p;
          stop = true;
          foundEmpty = true;
        }
      });
      
      if (foundEmpty == false || pixels.size() == numberOfPixels) break;
    }
    
    auto shape = std::make_shared<PolymorphShape>(pd, pixels);
    shape->ForEach([](PixelDescriptor* p, bool& stop){
      p->m_type = PixelDescriptor::Empty;
    });
    
    return shape;
  }
  
  IShape::Ptr CompactShape(PixelDescriptor* pd)
  {
    PixelDescriptor::Vec pixels;
    
    int numberOfPixels = cRandABInt(4, 8);
    pixels.push_back(pd);
    
    pd->AroundRandom([&](PixelDescriptor* p, bool& stop){
      if (p->IsEmpty() && cBoolRandPercent(0.6) && pixels.size() < numberOfPixels)

      {
        p->m_type = PixelDescriptor::CreatureType;
        pixels.push_back(p);
      }
    });
    
    int stepNumber = cRandABInt(1, 4);
    
    for (int i = 0; i < stepNumber; ++i)
    {
      int randomPixelIndex = cRandABInt(0, pixels.size());
      auto randomPixel = pixels[randomPixelIndex];
      randomPixel->AroundRandom([&](PixelDescriptor* p, bool& stop){
        if (p->IsEmpty() && cBoolRandPercent(0.6) && pixels.size() < numberOfPixels)
        {
          p->m_type = PixelDescriptor::CreatureType;
          pixels.push_back(p);
        }
      });
    }
    
    auto shape = std::make_shared<PolymorphShape>(pd, pixels);
    shape->ForEach([](PixelDescriptor* p, bool& stop){
      p->m_type = PixelDescriptor::Empty;
    });
    
    return shape;
  }
  
  void ShapesGenerator::GenerateShape(PixelDescriptor* pd, IShape::Ptr& outShape, ShapeType& outShapeType)
  {
    std::shared_ptr<IShape> shape;
    
    unsigned int numberOfShapes = 5;
    unsigned int shapeIndex = cRandABInt(0, numberOfShapes);
//    shapeIndex = 4;
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
      shape = ContinousShape(pd);
      outShapeType = ShapeType::eShapeTypeFixed;
    }
    else if (3 == shapeIndex)
    {
      shape = CompactShape(pd);
      outShapeType = ShapeType::eShapeTypeFixed;
    }
    else if (4 == shapeIndex)
    {
      shape = std::make_shared<PolymorphShape>(pd, 1);
      outShapeType = ShapeType::eShapeTypeAmorph;
    }
    else
    {
      assert(false);
    }
    
    outShape = shape;
  }
  
  ShapesGenerator::ShapesGenerator()
  : m_internalMap(new PixelMap(Vec2(100, 100)))
  {
  }
  
  ShapesGenerator::ResultItem ShapesGenerator::GenerateNext()
  {
    ResultItem result;
    GenerateShape(m_internalMap->GetDescriptor(50, 50), result.shape, result.type);
    result.map = m_internalMap;
    return result;
  }
}
