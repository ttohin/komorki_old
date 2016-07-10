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
    eCellTypeWhite = eCellTypeBigBlue << 1,
    eCellTypeYellow = eCellTypeWhite << 1,
    eCellTypePink = eCellTypeYellow << 1,
    eCellTypeNumberOfTypes = 8
  };
  
class PixelDescriptor;
  

class PixelDescriptor
{
public:
  
  typedef std::vector<PixelDescriptor*> Vec;

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
  bool pushHandled;
  
  // offset from root pixel in shapes
  int offsetX;
  int offsetY;
 
  void Around(const PerPixelFunc& op);
  void AroundRandom(const PerPixelFunc& op);
  PixelDescriptor* GetOpposite(PixelDescriptor* pd) const;
  PixelDescriptor* Offset(const Vec2& offset) const;
  PixelDescriptor* Offset(PixelPos x, PixelPos y) const;
  PixelDescriptor* RecOffset(PixelPos x, PixelPos y);
  bool Offset(PixelDescriptor* pd, Vec2& offset) const;
  void SetDirection(PixelPos x, PixelPos y, PixelDescriptor* pd);
  std::string GetAscii() const;
  std::string GetAscii(char cellSymbol) const;
  
  inline bool IsEmpty() const { return m_type == Empty; }
  inline bool IsCell(CellDescriptor* cd) const { return m_cellDescriptor == cd; }
  inline Vec2 GetPos() const { return {x, y}; }
  
  PixelDescriptor(int _x, int _y);
  virtual ~PixelDescriptor () {};
};
  
} // namespace komorki

#endif/* end of include guard: PIXELDESCRIPTOR_H */

