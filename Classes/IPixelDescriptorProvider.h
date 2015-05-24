#ifndef IPIXELDESCRIPTORPROVIDER_H_69TPCKTG
#define IPIXELDESCRIPTORPROVIDER_H_69TPCKTG

#include "PixelDescriptor.h"
#include <list>
#include "Common.h"
#include "CellDescriptor.h"

namespace komorki
{
  class CellDescriptor;
  class PixelDescriptor;

  
  struct AddCreature
  {
    PixelDescriptor* destinationDesc;
  };
  
  struct Action
  {
    Vec2 delta;
  };
  
  struct DeleteCreature
  {
    std::shared_ptr<CellDescriptor> cellDescriptor;
  };
  
  struct Movement
  {
    PixelDescriptor* destinationDesc;
  };
  
  template<typename T>
  class Optional
  {
  public:
    T value;
    bool isSet;
   
    Optional():isSet(false) {}
    Optional(const T& v):value(v), isSet(true) {}
    
    bool operator ==(bool b)
    {
      return b == isSet;
    }
    
    bool operator==(T v)
    {
      if (isSet) {
        return value == v;
      }
      else
      {
        return false;
      }
    }
    
    void SetValueFlag(bool i_isSet)
    {
      isSet = i_isSet;
    }
    
    void SetValue(T _v)
    {
      isSet = true;
      value = _v;
    }
    
  };

class IPixelDescriptorProvider
{
public:
  virtual PixelDescriptor* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const = 0;
  virtual komorki::Vec2 GetSize() const = 0;

  struct UpdateResult
  {
    void* userData;
    PixelDescriptor* desc;
    komorki::Optional<komorki::Action> action;
    komorki::Optional<komorki::Movement> movement;
    komorki::Optional<komorki::AddCreature> addCreature;
    komorki::Optional<komorki::DeleteCreature> deleteCreature;
    UpdateResult(CellDescriptor* desc);
  };

  virtual void Update(bool passUpdateResult, std::list<UpdateResult>&) = 0;
  virtual ~IPixelDescriptorProvider () {};
};

}

#endif /* end of include guard: IPIXELDESCRIPTORPROVIDER_H_69TPCKTG */
