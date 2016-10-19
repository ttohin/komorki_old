#ifndef IPIXELDESCRIPTORPROVIDER_H_69TPCKTG
#define IPIXELDESCRIPTORPROVIDER_H_69TPCKTG

#include "PixelDescriptor.h"
#include <list>
#include "Common.h"
#include "CellDescriptor.h"
#include "TerrainAnalizer.h"

namespace komorki
{
  class CellDescriptor;
  class PixelDescriptor;
  class Config;

  struct AddCreature
  {
    PixelDescriptor* destinationDesc = nullptr;
  };
  
  struct Action
  {
    Vec2 pos;
    Vec2 delta = Vec2(0.f, 0.f);
  };
  
  struct ChangeRect
  {
    Rect rect;
  };
  
  struct Morph
  {
    enum MorphDir
    {
      Move,
      Inside,
      Outside
    };
    Vec2 delta = Vec2(0.f, 0.f);
    Vec2 pos = Vec2(0.f, 0.f);
    MorphDir dir;
  };
  
  struct Morphing
  {
    typedef std::vector<Morph> Vec;
    Vec vec;
    bool proccessed = false;
//    Morphing() {}
//    Morphing(const Morphing& m) {}
  };
  
  struct DeleteCreature
  {
    std::shared_ptr<CellDescriptor> cellDescriptor = nullptr;
  };
  
  struct Movement
  {
    int duration = 0;
    PixelDescriptor* destinationDesc = nullptr;
  };
  
  template<typename T>
  class Optional
  {
  public:
    T value;
    bool isSet;
   
    Optional():isSet(false) {}
    Optional(const T& v):value(v), isSet(true) {}
    
    bool operator ==(bool b) const
    {
      return b == isSet;
    }
    
    bool operator==(T v) const
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
  virtual TerrainAnalizer::Result GetTerrain() const = 0;
  virtual komorki::Vec2 GetSize() const = 0;
  
  virtual void InitWithConfig(Config* config) = 0;

  struct UpdateResult
  {
    void* userData = nullptr;
    PixelDescriptor* desc = nullptr;
    komorki::Optional<komorki::Action> action;
    komorki::Optional<komorki::Movement> movement;
    komorki::Optional<komorki::AddCreature> addCreature;
    komorki::Optional<komorki::DeleteCreature> deleteCreature;
    komorki::Optional<komorki::Morphing> morph;
    komorki::Optional<komorki::ChangeRect> changeRect;
    UpdateResult(CellDescriptor* desc);
  };

  virtual void Update(bool passUpdateResult, std::list<UpdateResult>&) = 0;
  virtual ~IPixelDescriptorProvider () {};
};

}

#endif /* end of include guard: IPIXELDESCRIPTORPROVIDER_H_69TPCKTG */
