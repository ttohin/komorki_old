//
//  CellShapes.cpp
//  prsv
//
//  Created by Anton Simakov on 05.05.15.
//
//

#include "CellShapes.h"
#include "GreatPixel.h"
#include "Random.h"
#include <assert.h>

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

void SinglePixel::SetPosition(GreatPixel* pd)
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

IShape::Ptr SinglePixel::CopyWithBasePixel(GreatPixel* pd) const
{
  return std::make_shared<SinglePixel>(pd);
}

GreatPixel* SinglePixel::GetOpposite(GreatPixel* target) const
{
  return m_pd->GetOpposite(target);
}

void SinglePixel::Apply(GreatPixel* pd)
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

void BigCell::SetPosition(GreatPixel* pd)
{
  m_pd = pd;
}

GreatPixel* BigCell::GetOpposite(GreatPixel* target) const
{
  return nullptr;
}

void BigCell::Apply(GreatPixel* pd)
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

IShape::Ptr BigCell::CopyWithBasePixel(GreatPixel* pd) const
{
  return std::make_shared<BigCell>(pd);
}

//**************************************************************************************************
// RectShape
//**************************************************************************************************
RectShape::RectShape(GreatPixel* pd, const Vec2& origin, const Vec2& size)
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
  int totalSteps = 0;
  
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
      xDir = 0; yDir = -1; numberOfSteps = m_size.y + 1;
    }
    
    for (int i = 0; i < numberOfSteps; ++i)
    {
      totalSteps += 1;
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

void RectShape::SetPosition(GreatPixel* pd)
{
  m_pd = pd;
}

GreatPixel* RectShape::GetOpposite(GreatPixel* target) const
{
  return nullptr;
}

void RectShape::Apply(GreatPixel* pd)
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

IShape::Ptr RectShape::CopyWithBasePixel(GreatPixel* pd) const
{
  return std::make_shared<RectShape>(pd, m_pdOffset, m_size);
}

void RectShape::SetAABB(const Vec2& origin, const Vec2& size)
{
  m_pdOffset = origin;
  m_size = size;
}

Rect RectShape::GetRect() const
{
  return {m_pdOffset, m_size};
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
PolymorphShape::PolymorphShape(GreatPixel* pd, int numberOfPixels) : m_pd(pd)
{
  m_pd = pd;
  m_shape.push_back(pd);
  
  if (numberOfPixels > 1)
  {
    pd->Around([&](GreatPixel* pixel, bool& stop)
               {
                 m_shape.push_back(pixel);
                 if (m_shape.size() == numberOfPixels) stop = true;
               });

  }
}

PolymorphShape::PolymorphShape(GreatPixel* pd, const std::vector<GreatPixel*>& pixels)
{
  m_pd = pd;
  m_shape.insert(m_shape.end(), pixels.begin(), pixels.end());
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
    p->Around([&](GreatPixel* pixel, bool&)
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
  
  unsigned int size = m_shape.size();
  unsigned int start = cRandABInt(0, size - 1);
  
  
  for(unsigned int i = 0; i < size; i++)
  {
    unsigned int index = (i + start)%size;
    auto p = m_shape[index];
    
    p->AroundRandom([&](GreatPixel* pixel, bool& _stop)
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

void PolymorphShape::SetPosition(GreatPixel* pd)
{
  m_pd = pd;
}

GreatPixel* PolymorphShape::GetOpposite(GreatPixel* target) const
{
  return nullptr;
}

void PolymorphShape::Apply(GreatPixel* pd)
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

void PolymorphShape::RemovePixel(GreatPixel* pd)
{
  std::vector<GreatPixel*>::iterator it = std::find(m_shape.begin(), m_shape.end(), pd);
  bool isInShape = it != m_shape.end();
  assert(isInShape);
  m_shape.erase(it);
}

void PolymorphShape::AddPixel(GreatPixel* pd)
{
  std::vector<GreatPixel*>::iterator it = std::find(m_shape.begin(), m_shape.end(), pd);
  bool isInShape = it != m_shape.end();
  assert(!isInShape);
  m_shape.push_back(pd);
  pd->offsetX = pd->x - m_pd->x;
  pd->offsetY = pd->y - m_pd->y;
}

bool PolymorphShape::IsInShape(GreatPixel* pd)
{
  std::vector<GreatPixel*>::iterator it = std::find(m_shape.begin(), m_shape.end(), pd);
  return it != m_shape.end();
}


void PolymorphShape::SetPixels(GreatPixel* pd, const std::vector<GreatPixel*>& pixels)
{
  m_pd = pd;
  m_shape = pixels;
}

unsigned int PolymorphShape::Size() const
{
  return m_shape.size();
}

Rect PolymorphShape::GetAABB() const
{
  Rect result;
  for (auto& p : m_shape)
  {
    auto pos = p->GetPos();
    if (result.origin == Vec2())
    {
      result.origin = pos;
      result.size = {1, 1};
      continue;
    }
    
    if (result.Top() < pos.y)
    {
      result.size.y += pos.y - result.Top();
    }
    if (result.Bottom() > pos.y)
    {
      result.MoveBotton(pos.y - result.Bottom());
    }
    if (result.Right() < pos.x)
    {
      result.size.x += pos.x - result.Right();
    }
    if (result.Left() > pos.x)
    {
      result.MoveLeft(pos.x - result.Left());
    }
  }
  
  return result;
}

IShape::Ptr PolymorphShape::CopyWithBasePixel(GreatPixel* pd) const
{
  std::vector<Vec2> offsetsList;
  for (auto& p : m_shape)
  {
    offsetsList.emplace_back(p->x - m_pd->x, p->y - m_pd->y);
  }
  
  std::vector<GreatPixel*> listOfNewPixels;
  for (auto& offset : offsetsList)
  {
    auto targetPd = pd->RecOffset(offset.x, offset.y);
    if (targetPd == nullptr) return nullptr;
    
    listOfNewPixels.push_back(targetPd);
  }
  
  return std::make_shared<PolymorphShape>(pd, listOfNewPixels);
}

