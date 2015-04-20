#ifndef PIXELDESCRIPTOR_H
#define PIXELDESCRIPTOR_H

#include "IPixelDescriptor.h"
#include <vector>

namespace komorki
{
  
struct Transaction
{
  Transaction(): m_armor(0), m_health(0) {}
  int m_armor;
  int m_health;
};

class PixelDescriptor : public IPixelDescriptor
{
public:
  PixelDescriptor (Type t)
  :m_temperature(0.0f)
  , m_type(t)
  , m_skipFirstStep(false)
  , m_age(0)
  , m_sleepTime(0)
  , m_new(false)
  {
    this->userData = nullptr;
  };
  virtual ~PixelDescriptor () {};

  virtual float GetTemperature() const { return m_temperature; };
  virtual void SetTemperature(float t) { m_temperature = t; };
  virtual Type GetType() const { return m_type; };
  virtual void SetType(Type t) { m_type = t; };
  void Copy(PixelDescriptor* d)
  {
    m_type = d->m_type;
    m_id = d->m_id;
    m_character = d->m_character;
    m_health = d->m_health;
    m_armor = d->m_armor;
    m_baseArmor = d->m_baseArmor;
    m_damage = d->m_damage;
    m_skipFirstStep = d->m_skipFirstStep;
    m_age = d->m_age;
    m_sleepTime = d->m_sleepTime;
    m_sleepCounter = d->m_sleepCounter;
    m_baseHealth = d->m_baseHealth;
    m_lifeTime = d->m_lifeTime;
    m_new = d->m_new;
    nextTurnTransaction = std::vector<Transaction>(d->nextTurnTransaction);
  }

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

  std::vector<Transaction> nextTurnTransaction;
private:
  float m_temperature;
  Type m_type;
};
  
} // namespace komorki;

#endif /* end of include guard: PIXELDESCRIPTOR_H */
