#ifndef CELL_DESCRIPTOR_H
#define CELL_DESCRIPTOR_H

#include "PixelDescriptor.h"
#include "CellShapes.h"
#include <vector>

namespace komorki
{
  
class PixelDescriptor;

enum ShapeType
{
  eShapeTypeSinglePixel,
  eShapeTypePolymorph,
  eShapeTypeAmorph,
  eShapeTypeFixed,
  eShapeTypeRect
};
  
struct Genom
{
  typedef uint64_t GroupIdType;
  
  GroupIdType m_groupId = 0;
  GroupIdType m_dangerGroupId = 0;
  GroupIdType m_foodGroupId = 0;
  GroupIdType m_friendGroupId = 0;
  ShapeType m_shapeType;
  int m_health;
  int m_armor;
  int m_damage;
  int m_sleepTime;
  int m_lifeTime;
  IShape::Ptr m_shape;
  int m_lightFood;
  int m_passiveHealthIncome;
  int m_healthPerAttach;
  int m_volume = 1;
};
  
struct Transaction
{
  Transaction(): m_armor(0), m_health(0) {}
  int m_armor;
  int m_health;
};

class CellDescriptor
{
public:
  CellDescriptor (PixelDescriptor* pd);
 
  virtual void Shape(const PerPixelFunc& op);
  virtual void ShapeRandom(const PerPixelFunc& op);
  virtual void Around(const PerPixelFunc& op);
  virtual void AroundRandom(const PerPixelFunc& op);
  virtual bool Check();
  virtual void Finish();
  virtual void Move(PixelDescriptor* pd);
  virtual bool TestSpace(PixelDescriptor* pd);
  virtual bool TestSpaceEmpty(PixelDescriptor* pd);
  virtual IShape* GetShape() const;
  virtual bool Move(const std::vector<PixelDescriptor*>& removePixels,
                    const std::vector<PixelDescriptor*>& addPixels);
  virtual std::string GetAsciiArt() const;
  virtual void PrintAsciiArt() const;
  virtual bool IsMyFood(CellDescriptor* cd) const;
  virtual ShapeType GetShapeType() const;
  virtual void CleanSpace();
  
  virtual ~CellDescriptor () {};

  int m_id;
  int m_health;
  int m_armor;
  int m_age;
  bool m_skipFirstStep;
  bool m_new;
  int m_sleepCounter;
  unsigned int m_updateId;
  int m_m1;
  int m_m2;
  int m_m3;
  int m_m4;
  int m_volume;
 
  IShape::Ptr m_shape;
  void* userData;
  PixelDescriptor* parent;
  
  std::vector<Transaction> nextTurnTransaction;
  
  Genom m_genom;
};
  
} // namespace komorki;

#endif /* end of include guard: CELL_DESCRIPTOR_H */
