#ifndef CELL_DESCRIPTOR_H
#define CELL_DESCRIPTOR_H

#include "PixelDescriptor.h"
#include "CellShapes.h"
#include <vector>

namespace komorki
{
  
class PixelDescriptor;
  
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
  virtual void Around(const PerPixelFunc& op);
  virtual void AroundRandom(const PerPixelFunc& op);
  virtual bool Check();
  virtual void Finish();
  virtual void Move(PixelDescriptor* pd);
  virtual bool TestSpace(PixelDescriptor* pd);
  virtual bool TestSpaceEmpty(PixelDescriptor* pd);
  
  virtual ~CellDescriptor () {};

  int m_id;
  int m_baseHealth;
  int m_health;
  int m_armor;
  int m_baseArmor;
  int m_damage;
  int m_age;
  bool m_skipFirstStep;
  bool m_new;
  int m_sleepCounter;
  int m_sleepTime;
  int m_lifeTime;
  unsigned int m_updateId;
  int m_m1;
  int m_m2;
  int m_m3;
  int m_m4;
 
  std::shared_ptr<IShape> m_shape;
  CellType m_character;
  CellType m_dangerMask;
  CellType m_foodMask;
  CellType m_friendMask;
  void* userData;
  PixelDescriptor* parent;

  std::vector<Transaction> nextTurnTransaction;
};
  
} // namespace komorki;

#endif /* end of include guard: CELL_DESCRIPTOR_H */
