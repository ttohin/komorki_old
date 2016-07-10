//
//  PixelDescriptor.cpp
//  prsv
//
//  Created by Anton Simakov on 04.05.15.
//
//

#include "PixelDescriptor.h"
#include "b2Utilites.h"

using namespace komorki;

namespace Const
{
  const unsigned int numberOfDirections = 8;
}

PixelDescriptor::PixelDescriptor(int _x, int _y)
: lt (nullptr)
, lc (nullptr)
, lb (nullptr)
, ct (nullptr)
, cb (nullptr)
, rt (nullptr)
, rc (nullptr)
, rb (nullptr)
, directions{&lb, &cb, &rb, &lc, &rc, &lt, &ct, &rt}
, m_cellDescriptor(nullptr)
, m_type(PixelDescriptor::Empty)
, x(_x), y(_y)
, offsetX(0), offsetY(0)
, pushHandled(false)
{
  
}

void PixelDescriptor::Around(const PerPixelFunc& op)
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

void PixelDescriptor::AroundRandom(const PerPixelFunc& op)
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

PixelDescriptor* PixelDescriptor::GetOpposite(PixelDescriptor *target) const
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

PixelDescriptor* PixelDescriptor::Offset(const Vec2& offset) const
{
  return Offset(offset.x, offset.y);
}

PixelDescriptor* PixelDescriptor::Offset(PixelPos x, PixelPos y) const
{
  assert(!(x > 1) && !(x < -1));
  assert(!(y > 1) && !(y < -1));
  
  int index = x + 1 + (y + 1) * 3;
  if (index >= 4) index -= 1;
  return *directions[index];
}

PixelDescriptor* PixelDescriptor::RecOffset(PixelPos x, PixelPos y) 
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

bool PixelDescriptor::Offset(PixelDescriptor* target, Vec2& offset) const
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

void PixelDescriptor::SetDirection(PixelPos x, PixelPos y, PixelDescriptor* pd)
{
  assert(!(x > 1) && !(x < -1));
  assert(!(y > 1) && !(y < -1));
 
  int index = x + 1 + (y + 1) * 3;
  if (index >= 4) index -= 1;
  *directions[index] = pd;
}

std::string PixelDescriptor::GetAscii() const
{
  return GetAscii('a');
}

std::string PixelDescriptor::GetAscii(char cellSymbol) const
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
