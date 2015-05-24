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

namespace komorki
{
  class PixelDescriptor;
  
  class IShape
  {
  public:
    virtual void ForEach(const PerPixelFunc& op) const = 0;
    virtual void Around(const PerPixelFunc& op) const = 0;
    virtual void AroundRandom(const PerPixelFunc& op) const = 0;
    virtual void SetPosition(PixelDescriptor* pd) = 0;
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const = 0;
    virtual ~IShape(){}
  };
  
  class SingleCell : public IShape
  {
  public:
    SingleCell(PixelDescriptor* pd) : m_pd(pd) {}
    virtual ~SingleCell(){}
    virtual void ForEach(const PerPixelFunc& op) const;
    virtual void Around(const PerPixelFunc& op) const;
    virtual void AroundRandom(const PerPixelFunc& op) const;
    virtual void SetPosition(PixelDescriptor* pd);
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const;
  private:
    PixelDescriptor* m_pd;
  };
  
  class BigCell : public IShape
  {
  public:
    BigCell(PixelDescriptor* pd) : m_pd(pd) {}
    virtual ~BigCell(){}
    virtual void ForEach(const PerPixelFunc& op) const;
    virtual void Around(const PerPixelFunc& op) const;
    virtual void AroundRandom(const PerPixelFunc& op) const;
    virtual void SetPosition(PixelDescriptor* pd);
    virtual PixelDescriptor* GetOpposite(PixelDescriptor* target) const;
  private:
    PixelDescriptor* m_pd;
  };
}

#endif /* defined(__prsv__CellShapes__) */
