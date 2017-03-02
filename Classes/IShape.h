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
    virtual void SetPosition(PixelDescriptor* pd) = 0;
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const = 0;
    virtual void Apply(PixelDescriptor* pd) = 0; // applay the shape to corresponding pixel descriptors
    virtual unsigned int Size() const = 0;
    virtual Rect GetAABB() const { return Rect(); }
    virtual IShape::Ptr CopyWithBasePixel(PixelDescriptor* pd) const { return nullptr; }
    virtual ~IShape(){}
  };
}

