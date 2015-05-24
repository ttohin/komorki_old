//
//  CellDescriptor.cpp
//  prsv
//
//  Created by Anton Simakov on 05.05.15.
//
//

#include "CellDescriptor.h"

#define STRICT_CHECK(check) check

using namespace komorki;


CellDescriptor::CellDescriptor (PixelDescriptor* pd)
: m_skipFirstStep(false)
, m_age(0)
, m_sleepTime(0)
, m_new(false)
, userData(nullptr)
, parent(pd)
, m_updateId(0)
, m_m1(0)
, m_m2(0)
, m_m3(0)
, m_m4(0)
{
  assert(pd);
  parent->m_cellDescriptor = this;
}

void CellDescriptor::Shape(const PerPixelFunc &op)
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
  Shape([this, &result](PixelDescriptor* pd, bool& stop)
        {
          assert(pd);
          assert(pd->m_type == PixelDescriptor::CreatureType);
          assert (pd->m_cellDescriptor != nullptr);
          assert (pd->m_cellDescriptor->m_id == m_id);
          assert (pd->m_cellDescriptor == this);
        });
  return result;
}

void CellDescriptor::Finish()
{
  Shape([this](PixelDescriptor* pd, bool& stop){
    pd->m_type = PixelDescriptor::CreatureType;
    pd->m_cellDescriptor = this;
  });
}

void CellDescriptor::Move(PixelDescriptor* pd)
{
  Shape([](PixelDescriptor* pd, bool& stop)
        {
          pd->m_cellDescriptor = nullptr;
          pd->m_type = PixelDescriptor::Empty;
        });
  m_shape->SetPosition(pd);
  Shape([this](PixelDescriptor* pd, bool& stop)
        {
          assert(pd->m_cellDescriptor == nullptr);
          assert(pd->m_type == PixelDescriptor::Empty);
          pd->m_cellDescriptor = this;
          pd->m_type = PixelDescriptor::CreatureType;
        });
  parent = pd;
}

bool CellDescriptor::TestSpace(PixelDescriptor* pd)
{
  if (pd == nullptr) return false;
  
  bool empty = true;
  m_shape->SetPosition(pd);
  m_shape->ForEach([&empty, this](PixelDescriptor* pd, bool& stop)
                   {
                     if (pd == nullptr || pd->m_type != PixelDescriptor::Empty)
                     {
                       if (pd && pd->m_type == PixelDescriptor::CreatureType)
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

bool CellDescriptor::TestSpaceEmpty(PixelDescriptor* pd)
{
  bool empty = true;
  m_shape->SetPosition(pd);
  m_shape->ForEach([&empty, this](PixelDescriptor* pd, bool& stop)
                   {
                     if (pd == nullptr || pd->m_type != PixelDescriptor::Empty)
                     {
                       empty = false;
                       stop = true;
                     }
                   });
  m_shape->SetPosition(parent);
  return empty;
}

//void CellDescriptor::StepFrom(PixelDescriptor* pd)
//{
//  STRICT_CHECK(
//               bool pdIsInCorner = true;
//               Around([pd, &pdIsInCorner](PixelDescriptor* cornerPd, bool& stop)
//                      {
//                        if (cornerPd == pd) pdIsInCorner = true;
//                      });
//               assert(pdIsInCorner);
//               );
//  Vec2 offset;
//  offset.x = parent->x > pd->x ? 1 : parent->x < pd->x ? -1 : 0;
//  offset.y = parent->y > pd->y ? 1 : parent->y < pd->y ? -1 : 0;
//  PixelDescriptor* newPos = parent->Offset(offset);
//  
//}


