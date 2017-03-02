#ifndef GreatPixel_H
#define GreatPixel_H

#include "Common.h"
#include "PhysicalDescriptor.h"

namespace komorki
{
  class CellDescriptor;

  class GreatPixel
  {
  public:
    
    typedef std::vector<GreatPixel*> Vec;
    
    enum Type
    {
      CreatureType = 0,
      TerrainType,
      Empty
    };
    
    GreatPixel* lt;
    GreatPixel* lc;
    GreatPixel* lb;
    GreatPixel* ct;
    GreatPixel* cb;
    GreatPixel* rt;
    GreatPixel* rc;
    GreatPixel* rb;
    GreatPixel** directions[8];
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
    GreatPixel* GetOpposite(GreatPixel* pd) const;
    GreatPixel* Offset(const Vec2& offset) const;
    GreatPixel* Offset(PixelPos x, PixelPos y) const;
    GreatPixel* RecOffset(PixelPos x, PixelPos y);
    bool Offset(GreatPixel* pd, Vec2& offset) const;
    void SetDirection(PixelPos x, PixelPos y, GreatPixel* pd);
    std::string GetAscii() const;
    std::string GetAscii(char cellSymbol) const;
    int GetDirectionIndex(const Vec2& dir) const;
    GreatPixel* GetPixelByDirectionalindex(int index) const;
    
    inline bool IsEmpty() const { return m_type == Empty; }
    inline bool IsCell(CellDescriptor* cd) const { return m_cellDescriptor == cd; }
    inline Vec2 GetPos() const { return {x, y}; }
    
    GreatPixel(int _x, int _y);
    virtual ~GreatPixel () {};
  };
  
} // namespace komorki

#endif/* end of include guard: GreatPixel_H */

