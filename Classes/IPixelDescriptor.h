#ifndef IPIXELDESCRIPTOR_H
#define IPIXELDESCRIPTOR_H

namespace komorki
{
  enum CellType {
    eCellTypeStart = 0,
    eCellTypeGreen = eCellTypeStart,
    eCellTypeSalad,
    eCellTypeHunter,
    eCellTypeImprovedSalad,
    eCellTypeEnd
  };

class IPixelDescriptor
{
public:

  enum Type
  {
    CreatureType = 0,
    TerrainType,
    Empty
  };

  virtual float GetTemperature() const = 0;
  virtual Type GetType() const = 0;
  virtual ~IPixelDescriptor () {};
  void* userData;
  CellType m_character;
};
  
} // namespace komorki

#endif/* end of include guard: IPIXELDESCRIPTOR_H */

