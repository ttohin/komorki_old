//
//  Genom.h
//  Komorki
//
//  Created 02.03.17.
//

#pragma once

#include "CellShapes.h"
#include "IShape.h"

namespace komorki
{
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
}

