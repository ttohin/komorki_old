//
//  IShape.h
//  Komorki
//
//  Created 02.03.17.
//

#pragma once

#include "Common.h"
#include <memory>

namespace komorki
{
  class IShape
  {
  public:
    typedef std::shared_ptr<IShape> Ptr;
    virtual void ForEach(const PerPixelFunc& op) const = 0;
    virtual void ForEachRandom(const PerPixelFunc& op) const = 0;
    virtual void Around(const PerPixelFunc& op) const = 0;
    virtual void AroundRandom(const PerPixelFunc& op) const = 0;
    virtual void SetPosition(GreatPixel* pd) = 0;
    virtual GreatPixel* GetOpposite(GreatPixel* target) const = 0;
    virtual void Apply(GreatPixel* pd) = 0; // applay the shape to corresponding pixel descriptors
    virtual unsigned int Size() const = 0;
    virtual Rect GetAABB() const { return Rect(); }
    virtual IShape::Ptr CopyWithBasePixel(GreatPixel* pd) const { return nullptr; }
    virtual ~IShape(){}
  };
}

