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
  class GreatPixel;
  
  class SinglePixel : public IShape
  {
  public:
    SinglePixel(GreatPixel* pd) : m_pd(pd) {}
    virtual ~SinglePixel(){}
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(GreatPixel* pd) override;
    virtual GreatPixel* GetOpposite(GreatPixel* target) const override;
    virtual void Apply(GreatPixel* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(GreatPixel* pd) const override;
  private:
    GreatPixel* m_pd;
  };
  
  class BigCell : public IShape
  {
  public:
    BigCell(GreatPixel* pd) : m_pd(pd) {}
    virtual ~BigCell(){}
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(GreatPixel* pd) override;
    virtual GreatPixel* GetOpposite(GreatPixel* target) const override;
    virtual void Apply(GreatPixel* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(GreatPixel* pd) const override;
  private:
    GreatPixel* m_pd;
  };
  
  class RectShape : public IShape
  {
  public:
    RectShape(GreatPixel* pd, const Vec2& origin, const Vec2& size);
    virtual ~RectShape(){}
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(GreatPixel* pd) override;
    virtual GreatPixel* GetOpposite(GreatPixel* target) const override;
    virtual void Apply(GreatPixel* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(GreatPixel* pd) const override;
    
    //public
    virtual void SetAABB(const Vec2& origin, const Vec2& size);
    virtual Rect GetRect() const;
    
    
  private:
    GreatPixel* m_pd;
    Vec2 m_size;
    Vec2 m_pdOffset;
  };
  
  class PolymorphShape : public IShape
  {
  public:
    PolymorphShape(GreatPixel* pd, int numberOfPixels);
    PolymorphShape(GreatPixel* pd, const std::vector<GreatPixel*>& pixels);
    virtual ~PolymorphShape(){}
    
    // IShape
    
    virtual void ForEach(const PerPixelFunc& op) const override;
    virtual void ForEachRandom(const PerPixelFunc& op) const override;
    virtual void Around(const PerPixelFunc& op) const override;
    virtual void AroundRandom(const PerPixelFunc& op) const override;
    virtual void SetPosition(GreatPixel* pd) override;
    virtual GreatPixel* GetOpposite(GreatPixel* target) const override;
    virtual void Apply(GreatPixel* pd) override;
    virtual unsigned int Size() const override;
    virtual Rect GetAABB() const override;
    virtual IShape::Ptr CopyWithBasePixel(GreatPixel* pd) const override;
    
    // public
    virtual void RemovePixel(GreatPixel* source);
    virtual void AddPixel(GreatPixel* source);
    virtual bool IsInShape(GreatPixel* pd);
    virtual void SetPixels(GreatPixel* pd, const std::vector<GreatPixel*>& pixels);
    
  private:
    GreatPixel* m_pd;
    std::vector<GreatPixel*> m_shape;
  };
}

#endif /* defined(__prsv__CellShapes__) */
