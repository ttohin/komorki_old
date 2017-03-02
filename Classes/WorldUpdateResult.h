//
//  WorldUpdateResult.h
//  Komorki
//
//  Created 02.03.17.
//

#pragma once

#include <list>
#include "Common.h"


namespace komorki
{
  class CellDescriptor;
  class GreatPixel;
  class Config;
  
  struct AddCreature
  {
    GreatPixel* destinationDesc = nullptr;
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
  };
  
  struct DeleteCreature
  {
    std::shared_ptr<CellDescriptor> cellDescriptor = nullptr;
  };
  
  struct Movement
  {
    int duration = 0;
    GreatPixel* destinationDesc = nullptr;
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
  
  struct WorldUpateDiff
  {
    void* userData = nullptr;
    GreatPixel* desc = nullptr;
    komorki::Optional<komorki::Action> action;
    komorki::Optional<komorki::Movement> movement;
    komorki::Optional<komorki::AddCreature> addCreature;
    komorki::Optional<komorki::DeleteCreature> deleteCreature;
    komorki::Optional<komorki::Morphing> morph;
    komorki::Optional<komorki::ChangeRect> changeRect;
    WorldUpateDiff(CellDescriptor* desc);
  };
  
  typedef std::list<WorldUpateDiff> WorldUpdateList;
  
}

