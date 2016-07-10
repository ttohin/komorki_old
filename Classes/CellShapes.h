//
//  CellShapes.h
//  prsv
//
//  Created by Anton Simakov on 05.05.15.
//
//

#ifndef __prsv__CellShapes__
#define __prsv__CellShapes__

#include "Common.h"
#include <vector>

namespace komorki
{
  class PixelDescriptor;
  
  class IShape
  {
  public:
    virtual void ForEach(const PerPixelFunc& op) const = 0;
    virtual void ForEachRandom(const PerPixelFunc& op) const = 0;
    virtual void Around(const PerPixelFunc& op) const = 0;
    virtual void AroundRandom(const PerPixelFunc& op) const = 0;
    virtual void SetPosition(PixelDescriptor* pd) = 0;
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const = 0;
    virtual void Apply(PixelDescriptor* pd) = 0; // applay the shape to corresponding pixel descriptors
    virtual unsigned int Size() const = 0;
    virtual Rect GetAABB() const { return Rect(); }
    virtual ~IShape(){}
  };
  
  class SinglePixel : public IShape
  {
  public:
    SinglePixel(PixelDescriptor* pd) : m_pd(pd) {}
    virtual ~SinglePixel(){}
    virtual void ForEach(const PerPixelFunc& op) const;
    virtual void ForEachRandom(const PerPixelFunc& op) const;
    virtual void Around(const PerPixelFunc& op) const;
    virtual void AroundRandom(const PerPixelFunc& op) const;
    virtual void SetPosition(PixelDescriptor* pd);
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
  private:
    PixelDescriptor* m_pd;
  };
  
  class BigCell : public IShape
  {
  public:
    BigCell(PixelDescriptor* pd) : m_pd(pd) {}
    virtual ~BigCell(){}
    virtual void ForEach(const PerPixelFunc& op) const;
    virtual void ForEachRandom(const PerPixelFunc& op) const;
    virtual void Around(const PerPixelFunc& op) const;
    virtual void AroundRandom(const PerPixelFunc& op) const;
    virtual void SetPosition(PixelDescriptor* pd);
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
  private:
    PixelDescriptor* m_pd;
  };
  
  class RectShape : public IShape
  {
  public:
    RectShape(PixelDescriptor* pd, const Vec2& origin, const Vec2& size);
    virtual ~RectShape(){}
    virtual void ForEach(const PerPixelFunc& op) const;
    virtual void ForEachRandom(const PerPixelFunc& op) const;
    virtual void Around(const PerPixelFunc& op) const;
    virtual void AroundRandom(const PerPixelFunc& op) const;
    virtual void SetPosition(PixelDescriptor* pd);
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    
    //public
    virtual void SetAABB(const Vec2& origin, const Vec2& size);
    
    
  private:
    PixelDescriptor* m_pd;
    Vec2 m_size;
    Vec2 m_pdOffset;
  };
  
  class PolymorphShape : public IShape
  {
  public:
    PolymorphShape(PixelDescriptor* pd, int numberOfPixels);
    virtual ~PolymorphShape(){}
    
    // IShape
    
    virtual void ForEach(const PerPixelFunc& op) const;
    virtual void ForEachRandom(const PerPixelFunc& op) const;
    virtual void Around(const PerPixelFunc& op) const;
    virtual void AroundRandom(const PerPixelFunc& op) const;
    virtual void SetPosition(PixelDescriptor* pd);
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    
    // public
    virtual void RemovePixel(PixelDescriptor* source);
    virtual void AddPixel(PixelDescriptor* source);
    virtual bool IsInShape(PixelDescriptor* pd);
    
  private:
    PixelDescriptor* m_pd;
    std::vector<PixelDescriptor*> m_shape;
  };
}

#endif /* defined(__prsv__CellShapes__) */
