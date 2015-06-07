#ifndef PIXELDESCRIPTOR_H
#define PIXELDESCRIPTOR_H

#include "Common.h"
#include "PhysicalDescriptor.h"

namespace komorki
{
  class CellDescriptor;
  
  enum CellType {
    eCellTypeUnknown = 0x0000,
    eCellTypeGreen = 0x0001,
    eCellTypeSalad = eCellTypeGreen << 1,
    eCellTypeHunter = eCellTypeSalad << 1,
    eCellTypeImprovedSalad = eCellTypeHunter << 1,
    eCellTypeBigBlue = eCellTypeImprovedSalad << 1,
    eCellTypeNumberOfTypes = 5
  };
  
class PixelDescriptor;
  

class PixelDescriptor
{
public:

  enum Type
  {
    CreatureType = 0,
    TerrainType,
    Empty
  };

  PixelDescriptor* lt;
  PixelDescriptor* lc;
  PixelDescriptor* lb;
  PixelDescriptor* ct;
  PixelDescriptor* cb;
  PixelDescriptor* rt;
  PixelDescriptor* rc;
  PixelDescriptor* rb;
  PixelDescriptor** directions[8];
  CellDescriptor* m_cellDescriptor;
  Type m_type;
  PhysicalDescriptor m_physicalDesc;
  int x;
  int y;
 
  void Around(const PerPixelFunc& op);
  void AroundRandom(const PerPixelFunc& op);
  PixelDescriptor* GetOpposite(PixelDescriptor* pd) const;
  PixelDescriptor* Offset(const Vec2& offset) const;
  PixelDescriptor* Offset(PixelPos x, PixelPos y) const;
  bool Offset(PixelDescriptor* pd, Vec2& offset) const;
  void SetDirection(PixelPos x, PixelPos y, PixelDescriptor* pd);
  
  PixelDescriptor(int _x, int _y);
  virtual ~PixelDescriptor () {};
};
  
} // namespace komorki

#endif/* end of include guard: PIXELDESCRIPTOR_H */

