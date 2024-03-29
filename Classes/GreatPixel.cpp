//
//  GreatPixel.cpp
//  prsv
//
//  Created by Anton Simakov on 04.05.15.
//
//

#include "GreatPixel.h"
#include "Random.h"
#include <assert.h>
#include <cstdlib>

using namespace komorki;

namespace Const
{
  const unsigned int numberOfDirections = 8;
}

GreatPixel::GreatPixel(int _x, int _y)
: lt (nullptr)
, lc (nullptr)
, lb (nullptr)
, ct (nullptr)
, cb (nullptr)
, rt (nullptr)
, rc (nullptr)
, rb (nullptr)
#ifndef _WIN32
 , directions{&lb, &cb, &rb, &rc, &rt, &ct, &lt, &lc}
#endif
, m_cellDescriptor(nullptr)
, m_type(GreatPixel::Empty)
, x(_x), y(_y)
, offsetX(0), offsetY(0)
, pushHandled(false)
{
#ifdef _WIN32
  directions[0] = &lb;
  directions[1] = &cb;
  directions[2] = &rb;
  directions[3] = &rc;
  directions[4] = &rt;
  directions[5] = &ct;
  directions[6] = &lt;
  directions[7] = &lc;
#endif
}

void GreatPixel::Around(const PerPixelFunc& op)
{
  bool stop = false;
  for (int i = 0; i < Const::numberOfDirections; ++i)
  {
    op(*directions[i], stop);
    if (stop)
    {
      break;
    }
  }
}

void GreatPixel::AroundRandom(const PerPixelFunc& op)
{
  const int start = cRandABInt(0, Const::numberOfDirections);
  bool stop = false;
  for (int i = start; (i - start) != Const::numberOfDirections; i += 1)
  {
    op(*directions[i%Const::numberOfDirections], stop);
    if (stop)
    {
      break;
    }
  }
}

GreatPixel* GreatPixel::GetOpposite(GreatPixel *target) const
{
  if (target == lt) return rb;
  if (target == lc) return rc;
  if (target == lb) return rt;
  if (target == ct) return cb;
  if (target == cb) return ct;
  if (target == rt) return lb;
  if (target == rc) return lc;
  if (target == rb) return lt;
  return nullptr;
}

GreatPixel* GreatPixel::Offset(const Vec2& offset) const
{
  return Offset(offset.x, offset.y);
}

GreatPixel* GreatPixel::Offset(PixelPos x, PixelPos y) const
{
  assert(!(x > 1) && !(x < -1));
  assert(!(y > 1) && !(y < -1));
  
  if (Vec2(-1, 1) == Vec2(x, y)) return lt;
  if (Vec2(-1, 0) == Vec2(x, y)) return lc;
  if (Vec2(-1, -1) == Vec2(x, y)) return lb;
  if (Vec2(0, 1) == Vec2(x, y)) return ct;
  if (Vec2(0, -1) == Vec2(x, y)) return cb;
  if (Vec2(1, 1) == Vec2(x, y)) return rt;
  if (Vec2(1, 0) == Vec2(x, y)) return rc;
  if (Vec2(1, -1) == Vec2(x, y)) return rb;
  
  return nullptr;
}

GreatPixel* GreatPixel::RecOffset(PixelPos x, PixelPos y) 
{
  if (x == 0 && y == 0) return this;
  PixelPos _x = x > 0 ? 1 : x < 0 ? -1 : x;
  PixelPos _y = y > 0 ? 1 : y < 0 ? -1 : y;
  
  auto pd = Offset(_x, _y);
//  if (pd == nullptr)
//  {
//    pd = Offset(0, _y);
//    if (pd) return pd->RecOffset(0, y - _y);
//  }
//  
//  if (pd == nullptr)
//  {
//    pd = Offset(_x, 0);
//    if (pd) return pd->RecOffset(x - _x, 0);
//  }
  
  if (pd == nullptr) return nullptr;
  
  return pd->RecOffset(x - _x, y - _y);
}

bool GreatPixel::Offset(GreatPixel* target, Vec2& offset) const
{
  if (target == lt) { offset = {-1, 1}; return true; }
  if (target == lc) { offset = {-1, 0}; return true; }
  if (target == lb) { offset = {-1, -1}; return true; }
  if (target == ct) { offset = {0, 1}; return true; }
  if (target == cb) { offset = {0, -1}; return true; }
  if (target == rt) { offset = {1, 1}; return true; };
  if (target == rc) { offset = {1, 0}; return true; }
  if (target == rb) { offset = {1, -1}; return true; }
  return false;
}

//bool GreatPixel::Offset(GreatPixel* target, Vec2& offset) const
//{
//  if (target == lt) { offset = {-1, 1}; return true; }
//  if (target == lc) { offset = {-1, 0}; return true; }
//  if (target == lb) { offset = {-1, -1}; return true; }
//  if (target == ct) { offset = {0, 1}; return true; }
//  if (target == cb) { offset = {0, -1}; return true; }
//  if (target == rt) { offset = {1, 1}; return true; };
//  if (target == rc) { offset = {1, 0}; return true; }
//  if (target == rb) { offset = {1, -1}; return true; }
//  return false;
//}

void GreatPixel::SetDirection(PixelPos x, PixelPos y, GreatPixel* pd)
{
  assert(!(x > 1) && !(x < -1));
  assert(!(y > 1) && !(y < -1));
  
  if (Vec2(-1, 1) == Vec2(x, y)) lt = pd;
  if (Vec2(-1, 0) == Vec2(x, y)) lc = pd;
  if (Vec2(-1, -1) == Vec2(x, y)) lb = pd;
  if (Vec2(0, 1) == Vec2(x, y)) ct = pd;
  if (Vec2(0, -1) == Vec2(x, y)) cb = pd;
  if (Vec2(1, 1) == Vec2(x, y)) rt = pd;
  if (Vec2(1, 0) == Vec2(x, y)) rc = pd;
  if (Vec2(1, -1) == Vec2(x, y)) rb = pd;
}

//void GreatPixel::SetDirection(PixelPos x, PixelPos y, GreatPixel* pd)
//{
//  assert(!(x > 1) && !(x < -1));
//  assert(!(y > 1) && !(y < -1));
//  
//  int index = x + 1 + (y + 1) * 3;
//  if (index >= 4) index -= 1;
//  *directions[index] = pd;
//}

std::string GreatPixel::GetAscii() const
{
  return GetAscii('a');
}

std::string GreatPixel::GetAscii(char cellSymbol) const
{
  switch (m_type)
  {
    case CreatureType:
      return std::string(&cellSymbol, 1);
    case TerrainType:
      return "#";
    case Empty:
      return ".";
  }
  return "?";
}

int GreatPixel::GetDirectionIndex(const Vec2& dir) const
{
  for (int i = 0; i < Const::numberOfDirections; ++i)
  {
    Vec2 testOffset;
    Offset(*directions[i], testOffset);
    if (testOffset == dir)
    {
      return i;
    }
  }
  
  assert(0);
  return -1;
}

GreatPixel* GreatPixel::GetPixelByDirectionalindex(int index) const
{
  if (index < 0)
  {
    index = Const::numberOfDirections - std::abs(index)%Const::numberOfDirections;
  }
  if (index >= Const::numberOfDirections)
  {
    index = index%Const::numberOfDirections;
  }
  
  assert(index >= 0);
  assert(index < Const::numberOfDirections);
  return *directions[index];
}
