//
//  GenomsGenerator.cpp
//  Komorki
//
//  Created by Anton Simakov on 21/10/2016.
//
//

#include "GenomsGenerator.hpp"
#include "b2Utilites.h"

namespace komorki
{
  namespace
  {
    int kMaxNumberOfGroups = 64;
  }
  
  Genom GenomsGenerator::GenerateGenom(Genom::GroupIdType groupId, const ShapesGenerator::ResultItem &shape) const
  {
    Genom g;
    g.m_groupId = groupId;
    
    for (int j = 0; j < kMaxNumberOfGroups; j++)
    {
      if (g.m_groupId == (1 << j)) {
        continue;
      }
      g.m_foodGroupId |= cRandAorB(0, 1) ? 1 << j : 0;
    }
    
    for (int j = 0; j < kMaxNumberOfGroups; j++)
    {
      if (g.m_groupId == (1 << j)) {
        continue;
      }
      g.m_dangerGroupId = cRandAorB(0, 1, 0.9) ? 1 << j : 0;
    }
    
    for (int j = 0; j < kMaxNumberOfGroups; j++)
    {
      if (g.m_groupId == (1 << j)) {
        continue;
      }
      g.m_friendGroupId |= cRandAorB(0, 1) ? 1 << j : 0;
    }
    
    g.m_health = cRandABInt(100, 1000);
    g.m_armor = cRandABInt(1, 100);
    g.m_sleepTime = cRandABInt(0, 10);
    g.m_lifeTime = cRandABInt(100, 1000);
    g.m_damage = cRandABInt(1, 50);
    g.m_lightFood = cRandABInt(1, 15);
    g.m_passiveHealthIncome = cRandABInt(0, 10);
    g.m_healthPerAttach = cRandABInt(10, 100);
    g.m_shape = shape.shape;
    g.m_shapeType = shape.type;
    
    return g;
  }
  
  bool GenomsGenerator::AddShape(const ShapesGenerator::ResultItem &shape, Genom::GroupIdType& outGroupId)
  {
    if (m_genomsList.size() == kMaxNumberOfGroups)
    {
      return false;
    }
    
    outGroupId = (Genom::GroupIdType)1 << m_genomsList.size();
    auto genom = GenerateGenom(outGroupId, shape);
    m_genomsList.push_back(genom);
    
    m_internalMap = shape.map;
    
    return true;
  }
}
