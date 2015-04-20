#ifndef IPIXELDESCRIPTORPROVIDER_H_69TPCKTG
#define IPIXELDESCRIPTORPROVIDER_H_69TPCKTG

#include "IPixelDescriptor.h"
#include <list>

namespace komorki
{
  typedef int PixelPos;
  
  struct Vec2
  {
    PixelPos x;
    PixelPos y;
    Vec2():x(0),y(0){}
    Vec2(PixelPos _x, PixelPos _y):x(_x),y(_y){}
    Vec2& operator=(const Vec2& pos) {x = pos.x; y = pos.y; return *this; }
    bool operator==(const Vec2& pos) {return x == pos.x && y == pos.y;}
    bool operator!=(const Vec2& pos) {return !(*this==pos);}
  };
  
  struct AddCreature
  {
    Vec2 to;
    IPixelDescriptor* destinationDesc;
  };
  
  struct Action
  {
    Vec2 source;
    Vec2 delta;
  };
  
  struct Movement
  {
    Vec2 source;
    Vec2 destination;
    IPixelDescriptor* destinationDesc;
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
    
    void SetValue(bool i_isSet)
    {
      isSet = i_isSet;
    }
    
  };

class IPixelDescriptorProvider
{
public:
  virtual IPixelDescriptor* GetDescriptor(komorki::PixelPos x, komorki::PixelPos y) const = 0;
  virtual komorki::Vec2 GetSize() const = 0;

  struct UpdateResult
  {
    void* userData;
    IPixelDescriptor* desc;
    komorki::Vec2 source;
    komorki::Optional<komorki::Action> action;
    komorki::Optional<komorki::Movement> movement;
    komorki::Optional<komorki::AddCreature> addCreature;
    bool shouldDelete;
    UpdateResult(IPixelDescriptor* idesc):
      shouldDelete(false), desc(idesc), userData(idesc->userData)
    {}
  };

  virtual void Update(bool passUpdateResult, std::list<UpdateResult>&) = 0;
  virtual ~IPixelDescriptorProvider () {};
};

}

#endif /* end of include guard: IPIXELDESCRIPTORPROVIDER_H_69TPCKTG */
