//
//  CellShapes.cpp
//  prsv
//
//  Created by Anton Simakov on 05.05.15.
//
//

#include "CellShapes.h"
#include "PixelDescriptor.h"
#include "b2Utilites.h"

using namespace komorki;

void SinglePixel::ForEach(const PerPixelFunc& op) const
{
  bool stop = false;
  op(m_pd, stop);
}

void SinglePixel::ForEachRandom(const PerPixelFunc& op) const
{
  bool stop = false;
  op(m_pd, stop);
}

void SinglePixel::Around(const PerPixelFunc& op) const
{
  m_pd->Around(op);
}

void SinglePixel::AroundRandom(const PerPixelFunc& op) const
{
  m_pd->AroundRandom(op);
}

void SinglePixel::SetPosition(PixelDescriptor* pd)
{
  m_pd = pd;
}

unsigned int SinglePixel::Size() const
{
  return 1;
}

Rect SinglePixel::GetAABB() const
{
  return {m_pd->GetPos(), {1, 1}};
}

PixelDescriptor* SinglePixel::GetOpposite(PixelDescriptor* target) const
{
  return m_pd->GetOpposite(target);
}

void SinglePixel::Apply(PixelDescriptor* pd)
{
  m_pd->offsetX = 0;
  m_pd->offsetY = 0;
}

void BigCell::ForEach(const PerPixelFunc& op) const
{
  bool stop = false;
  if (false == stop) op(m_pd, stop);
  if (false == stop) op(m_pd->rc, stop);
  if (false == stop) op(m_pd->rt, stop);
  if (false == stop) op(m_pd->ct, stop);
}

void BigCell::ForEachRandom(const PerPixelFunc& op) const
{
  ForEach(op);
}

void BigCell::Around(const PerPixelFunc& op) const
{
  bool stop = false;
  if (false == stop) op(m_pd->lc, stop);
  if (false == stop) op(m_pd->lb, stop);
  if (false == stop) op(m_pd->cb, stop);
  if (false == stop) op(m_pd->rc->cb, stop);
  if (false == stop) op(m_pd->rc->rb, stop);
  if (false == stop) op(m_pd->rc->rc, stop);
  if (false == stop) op(m_pd->rt->rt, stop);
  if (false == stop) op(m_pd->rt->rc, stop);
  if (false == stop) op(m_pd->rt->ct, stop);
  if (false == stop) op(m_pd->ct->ct, stop);
  if (false == stop) op(m_pd->ct->lt, stop);
  if (false == stop) op(m_pd->ct->lc, stop);
}

void BigCell::AroundRandom(const PerPixelFunc& op) const
{
  const int start = cRandABInt(0, 12);
  const int dir = rand()%2 ? 1 : 1;
  bool stop = false;
  for (int ri = start; (ri - dir*start) != 12; ri += dir)
  {
    int i = ri%12;
    if (i == 0) op(m_pd->lc, stop); if (stop) break;
    if (i == 1) op(m_pd->lb, stop); if (stop) break;
    if (i == 2) op(m_pd->cb, stop); if (stop) break;
    if (i == 3) op(m_pd->rc->cb, stop); if (stop) break;
    if (i == 4) op(m_pd->rc->rb, stop); if (stop) break;
    if (i == 5) op(m_pd->rc->rc, stop); if (stop) break;
    if (i == 6) op(m_pd->rt->rt, stop); if (stop) break;
    if (i == 7) op(m_pd->rt->rc, stop); if (stop) break;
    if (i == 8) op(m_pd->rt->ct, stop); if (stop) break;
    if (i == 9) op(m_pd->ct->ct, stop); if (stop) break;
    if (i == 10) op(m_pd->ct->lt, stop); if (stop) break;
    if (i == 11) op(m_pd->ct->lc, stop); if (stop) break;
  }
}

void BigCell::SetPosition(PixelDescriptor* pd)
{
  m_pd = pd;
}

PixelDescriptor* BigCell::GetOpposite(PixelDescriptor* target) const
{
  return nullptr;
}

void BigCell::Apply(PixelDescriptor* pd)
{
  m_pd->offsetX = 0;
  m_pd->offsetY = 0;
}

unsigned int BigCell::Size() const
{
  return 4;
}

Rect BigCell::GetAABB() const
{
  return {m_pd->GetPos(), {2, 2}};
}

//**************************************************************************************************
// RectShape
//**************************************************************************************************
RectShape::RectShape(PixelDescriptor* pd, const Vec2& origin, const Vec2& size)
: m_pd(pd)
, m_size(size)
, m_pdOffset(origin)
{
}

void RectShape::ForEach(const PerPixelFunc& op) const
{
  bool stop = false;
  int dir = 1;
  auto leftDownPd = m_pd->RecOffset(-m_pdOffset.x, -m_pdOffset.y);
  
  for (int x = 0; x < m_size.x; ++x)
  {
    for (int y = 0; y < m_size.y; ++y)
    {
      op(leftDownPd, stop);
      if (stop) return;
      if (y != m_size.y - 1) leftDownPd = leftDownPd->Offset(0, dir);
    }
    
    leftDownPd = leftDownPd->Offset(1, 0);
    dir *= -1;
  }
}

void RectShape::ForEachRandom(const PerPixelFunc& op) const
{
  ForEach(op);
}

void RectShape::Around(const PerPixelFunc& op) const
{
  bool stop = false;
  auto leftDownPd = m_pd->RecOffset(-m_pdOffset.x - 1, -m_pdOffset.y - 1);
  
  int iteration = 0;
  
  while (iteration != 4)
  {
    int numberOfSteps = 0;
    int xDir;
    int yDir;
    
    if (iteration == 0)
    {
      xDir = 1; yDir = 0; numberOfSteps = m_size.x + 1;
    }
    if (iteration == 1)
    {
      xDir = 0; yDir = 1; numberOfSteps = m_size.y + 1;
    }
    if (iteration == 2)
    {
      xDir = -1; yDir = 0; numberOfSteps = m_size.x + 1;
    }
    if (iteration == 3)
    {
      xDir = 0; yDir = -1; numberOfSteps = m_size.y;
    }
    
    for (int i = 0; i < numberOfSteps; ++i)
    {
      op(leftDownPd, stop);
      if (stop) return;
      leftDownPd = leftDownPd->Offset(xDir, yDir);
    }
    
    iteration += 1;
  }
}

void RectShape::AroundRandom(const PerPixelFunc& op) const
{
  Around(op);
}

void RectShape::SetPosition(PixelDescriptor* pd)
{
  m_pd = pd;
}

PixelDescriptor* RectShape::GetOpposite(PixelDescriptor* target) const
{
  return nullptr;
}

void RectShape::Apply(PixelDescriptor* pd)
{
  m_pd->offsetX = 0;
  m_pd->offsetY = 0;
}

unsigned int RectShape::Size() const
{
  return m_size.x * m_size.y;
}

Rect RectShape::GetAABB() const
{
  Vec2 origin = m_pd->GetPos() - m_pdOffset;
  return {origin, m_size};
}

void RectShape::SetAABB(const Vec2& origin, const Vec2& size)
{
  m_pdOffset = origin;
  m_size = size;
}


//***************************************************************
void PolymorphShape::ForEach(const PerPixelFunc& op) const
{
  bool stop = false;
  for(const auto& p : m_shape)
  {
    op(p, stop);
    if (stop)
    {
      break;
    }
  }
}

//***************************************************************
PolymorphShape::PolymorphShape(PixelDescriptor* pd, int numberOfPixels) : m_pd(pd)
{
  m_shape.push_back(pd);
  
  if (numberOfPixels > 1)
  {
    pd->Around([&](PixelDescriptor* pixel, bool& stop)
               {
                 m_shape.push_back(pixel);
                 if (m_shape.size() == numberOfPixels) stop = true;
               });

  }
  
}

void PolymorphShape::ForEachRandom(const PerPixelFunc& op) const
{
  ForEachRandom(op);
}

void PolymorphShape::Around(const PerPixelFunc& op) const
{
  bool stop = false;
  
  for(const auto& p : m_shape)
  {
    p->Around([&](PixelDescriptor* pixel, bool&)
              {
                if (std::find(m_shape.begin(), m_shape.end(), pixel) == m_shape.end())
                {
                  op(pixel, stop);
                  if (stop)
                  {
                    return;
                  }
                }
              });
  }
}

void PolymorphShape::AroundRandom(const PerPixelFunc& op) const
{
  bool stop = false;
  
  for(const auto& p : m_shape)
  {
    p->AroundRandom([&](PixelDescriptor* pixel, bool& _stop)
              {
                if (std::find(m_shape.begin(), m_shape.end(), pixel) == m_shape.end())
                {
                  op(pixel, _stop);
                  stop = _stop;
                }
              });
    if (stop)
    {
      break;
    }
  }
}

void PolymorphShape::SetPosition(PixelDescriptor* pd)
{
  m_pd = pd;
}

PixelDescriptor* PolymorphShape::GetOpposite(PixelDescriptor* target) const
{
  return nullptr;
}

void PolymorphShape::Apply(PixelDescriptor* pd)
{
  m_pd = pd;
  m_pd->offsetX = 0;
  m_pd->offsetY = 0;
  for (auto& p : m_shape)
  {
    p->offsetX = p->x - m_pd->x;
    p->offsetY = p->y - m_pd->y;
  }
}

void PolymorphShape::RemovePixel(PixelDescriptor* pd)
{
  std::vector<PixelDescriptor*>::iterator it = std::find(m_shape.begin(), m_shape.end(), pd);
  bool isInShape = it != m_shape.end();
  assert(isInShape);
  m_shape.erase(it);
}

void PolymorphShape::AddPixel(PixelDescriptor* pd)
{
  std::vector<PixelDescriptor*>::iterator it = std::find(m_shape.begin(), m_shape.end(), pd);
  bool isInShape = it != m_shape.end();
  assert(!isInShape);
  m_shape.push_back(pd);
  pd->offsetX = pd->x - m_pd->x;
  pd->offsetY = pd->y - m_pd->y;
}

bool PolymorphShape::IsInShape(PixelDescriptor* pd)
{
  std::vector<PixelDescriptor*>::iterator it = std::find(m_shape.begin(), m_shape.end(), pd);
  return it != m_shape.end();
}

unsigned int PolymorphShape::Size() const
{
  return m_shape.size();
}


Rect PolymorphShape::GetAABB() const
{
  return Rect();
}

