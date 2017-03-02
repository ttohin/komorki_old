//
//  CellShapes.h
//  prsv
//
//  Created by Anton Simakov on 05.05.15.
//
//

#ifndef __prsv__CellShapes__
#define __prsv__CellShapes__

#include "IShape.h"
#include "Common.h"
#include <vector>
#include <memory>

namespace komorki
{
  class PixelDescriptor;
  
  class SinglePixel : public IShape
  {
  public:
    SinglePixel(PixelDescriptor* pd) : m_pd(pd) {}
    virtual ~SinglePixel(){}
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(PixelDescriptor* pd) override;
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const override;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(PixelDescriptor* pd) const override;
  private:
    PixelDescriptor* m_pd;
  };
  
  class BigCell : public IShape
  {
  public:
    BigCell(PixelDescriptor* pd) : m_pd(pd) {}
    virtual ~BigCell(){}
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(PixelDescriptor* pd) override;
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const override;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(PixelDescriptor* pd) const override;
  private:
    PixelDescriptor* m_pd;
  };
  
  class RectShape : public IShape
  {
  public:
    RectShape(PixelDescriptor* pd, const Vec2& origin, const Vec2& size);
    virtual ~RectShape(){}
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(PixelDescriptor* pd) override;
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const override;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(PixelDescriptor* pd) const override;
    
    //public
    virtual void SetAABB(const Vec2& origin, const Vec2& size);
    virtual Rect GetRect() const;
    
    
  private:
    PixelDescriptor* m_pd;
    Vec2 m_size;
    Vec2 m_pdOffset;
  };
  
  class PolymorphShape : public IShape
  {
  public:
    PolymorphShape(PixelDescriptor* pd, int numberOfPixels);
    PolymorphShape(PixelDescriptor* pd, const std::vector<PixelDescriptor*>& pixels);
    virtual ~PolymorphShape(){}
    
    // IShape
    
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(PixelDescriptor* pd) override;
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const override;
    virtual void Apply(PixelDescriptor* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(PixelDescriptor* pd) const override;
    
    // public
    virtual void RemovePixel(PixelDescriptor* source);
    virtual void AddPixel(PixelDescriptor* source);
    virtual bool IsInShape(PixelDescriptor* pd);
    virtual void SetPixels(PixelDescriptor* pd, const std::vector<PixelDescriptor*>& pixels);
    
  private:
    PixelDescriptor* m_pd;
    std::vector<PixelDescriptor*> m_shape;
  };
}

#endif /* defined(__prsv__CellShapes__) */
