//
//  CellDescriptor.cpp
//  prsv
//
//  Created by Anton Simakov on 05.05.15.
//
//

#include "CellDescriptor.h"
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <sstream>

#define STRICT_CHECK(check) check

using namespace komorki;


CellDescriptor::CellDescriptor (GreatPixel* pd)
: m_skipFirstStep(false)
, m_age(0)
, m_new(false)
, userData(nullptr)
, parent(pd)
, m_updateId(0)
, m_m1(0)
, m_m2(0)
, m_m3(0)
, m_m4(0)
, m_volume(0)
{
  assert(pd);
  parent->m_cellDescriptor = this;
}

void CellDescriptor::Shape(const PerPixelFunc &op)
{
  m_shape->ForEach(op);
}

void CellDescriptor::ShapeRandom(const PerPixelFunc &op)
{
  m_shape->ForEach(op);
}

void CellDescriptor::Around(const PerPixelFunc& op)
{
  m_shape->Around(op);
}

void CellDescriptor::AroundRandom(const PerPixelFunc& op)
{
  m_shape->AroundRandom(op);
}

bool CellDescriptor::Check() 
{
  bool result = true;
  Shape([this, &result](GreatPixel* pd, bool& stop)
        {
          assert(pd);
          assert(pd->m_type == GreatPixel::CreatureType);
          assert (pd->m_cellDescriptor != nullptr);
          assert (pd->m_cellDescriptor->m_id == m_id);
          assert (pd->m_cellDescriptor == this);
        });
  return result;
}

void CellDescriptor::Finish()
{
  Shape([this](GreatPixel* pd, bool& stop){
    pd->m_type = GreatPixel::CreatureType;
    pd->m_cellDescriptor = this;
    pd->pushHandled = false;
  });
  
  m_shape->Apply(parent);
  m_volume = m_shape->Size();
}

void CellDescriptor::Move(GreatPixel* pd)
{
  CleanSpace();
  m_shape->SetPosition(pd);
  Shape([this](GreatPixel* pd, bool& stop)
        {
          assert(pd->m_cellDescriptor == nullptr);
          assert(pd->m_type == GreatPixel::Empty);
          pd->m_cellDescriptor = this;
          pd->m_type = GreatPixel::CreatureType;
        });
  parent = pd;
}

bool CellDescriptor::TestSpace(GreatPixel* pd)
{
  if (pd == nullptr) return false;
  
  bool empty = true;
  m_shape->SetPosition(pd);
  m_shape->ForEach([&empty, this](GreatPixel* pd, bool& stop)
                   {
                     if (pd == nullptr || pd->m_type != GreatPixel::Empty)
                     {
                       if (pd && pd->m_type == GreatPixel::CreatureType)
                       {
                         if (pd->m_cellDescriptor == this)
                         {
                           return;
                         }
                       }
                       
                       empty = false;
                       stop = true;
                     }
                   });
  m_shape->SetPosition(parent);
  return empty;
}

bool CellDescriptor::TestSpaceEmpty(GreatPixel* pd)
{
  bool empty = true;
  m_shape->SetPosition(pd);
  m_shape->ForEach([&empty, this](GreatPixel* pd, bool& stop)
                   {
                     if (pd == nullptr || pd->m_type != GreatPixel::Empty)
                     {
                       empty = false;
                       stop = true;
                     }
                   });
  m_shape->SetPosition(parent);
  return empty;
}

IShape* CellDescriptor::GetShape() const
{
  return m_shape.get();
}

bool CellDescriptor::Move(const std::vector<GreatPixel*>& removePixels,
                          const std::vector<GreatPixel*>& addPixels)
{
  PolymorphShape* shape = static_cast<PolymorphShape*>(m_shape.get());
  for (const auto& p : removePixels)
  {
    assert(p->m_cellDescriptor == this);
    p->m_cellDescriptor = nullptr;
    p->m_type = GreatPixel::Empty;
    shape->RemovePixel(p);
  }
  
  for (const auto& p : addPixels)
  {
    assert(p->m_cellDescriptor == nullptr);
    p->m_cellDescriptor = this;
    p->m_type = GreatPixel::CreatureType;
    shape->AddPixel(p);
    parent = p;
  }
  
  if (addPixels.empty()) // if no new pixels -
  {
    shape->ForEach([this](GreatPixel* pd, bool& stop)
                   {
                     parent = pd;
                     stop = true;
                   });
  }
  
  return true;
}

std::string CellDescriptor::GetAsciiArt() const
{
  const int size = 5;
  bool cornernKnown = false;
  Vec2 leftDownCornerPos;
  
  std::string result;
  result.reserve(size * size);
  
  unsigned long long symbolNum = (unsigned long long)this%26;
  char cellChar = 'a' + symbolNum;
  char cellCharBase = 'A' + symbolNum;
  
  for (int i = 0; i < size * 2; i++)
  {
    bool printLineNumber = true;
    
    for (int j = 0; j < size * 2; j++)
    {
      auto pd = parent->RecOffset(j - size, - (i - size));
      if (!pd) continue;
      
      if (printLineNumber)
      {
        std::ostringstream str;
        str << std::setfill('0') << std::setw(5) << pd->y << ":";
        result += str.str();
        printLineNumber = false;
      }
      
      if (!cornernKnown)
      {
        cornernKnown = true;
        leftDownCornerPos = Vec2(pd->x, pd->y);
      }
      
      
      if (pd == parent)
        result += pd->GetAscii(cellCharBase);
      else
        result += pd->GetAscii(cellChar);
    }
    
    result += "\n";
  }
  
  
  std::ostringstream str;
  str << std::setfill('0') << std::setw(5) << leftDownCornerPos.x << ":";
  result += str.str();
  
  for (int j = 0; j < size * 2; j++)
  {
    result += std::to_string(j);
  }
 
  return  result;
}

void CellDescriptor::PrintAsciiArt() const
{
  std::cout << GetAsciiArt() << std::endl;
}

//void CellDescriptor::StepFrom(GreatPixel* pd)
//{
//  STRICT_CHECK(
//               bool pdIsInCorner = true;
//               Around([pd, &pdIsInCorner](GreatPixel* cornerPd, bool& stop)
//                      {
//                        if (cornerPd == pd) pdIsInCorner = true;
//                      });
//               assert(pdIsInCorner);
//               );
//  Vec2 offset;
//  offset.x = parent->x > pd->x ? 1 : parent->x < pd->x ? -1 : 0;
//  offset.y = parent->y > pd->y ? 1 : parent->y < pd->y ? -1 : 0;
//  GreatPixel* newPos = parent->Offset(offset);
//  
//}


bool CellDescriptor::IsMyFood(CellDescriptor* cd) const
{
  return cd != this && cd->m_genom.m_groupId & m_genom.m_foodGroupId;
}


ShapeType CellDescriptor::GetShapeType() const
{
  return m_genom.m_shapeType;
}

void CellDescriptor::CleanSpace()
{
  m_shape->ForEach([&](GreatPixel* pd, bool& stop)
                   {
                     pd->m_cellDescriptor = nullptr;
                     pd->m_type = GreatPixel::Empty;
                   });
}

