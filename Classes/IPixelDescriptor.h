#ifndef IPIXELDESCRIPTOR_H
#define IPIXELDESCRIPTOR_H

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
  int m_character;
};

#endif/* end of include guard: IPIXELDESCRIPTOR_H */

