//
//  Common.cpp
//  prsv
//
//  Created by ttohin on 12.08.15.
//
//

#include "Common.h"

using namespace komorki;

bool Vec2::In(const komorki::Rect &rect) const
{
  return !(x < rect.Left() || x >= rect.Right()) && !(y < rect.Bottom() || y >= rect.Top());
}

Rect Rect::Extract(const Rect &other) const
{
  Rect result;
  result.origin.x = 0;
  result.origin.y = 0;
  result.size.x = 0;
  result.size.y = 0;
  
  int offset = other.origin.x - origin.x;
  if (offset >= 0)
  {
    if (origin.x + size.x >= other.origin.x)
    {
      result.origin.x = other.origin.x;
      result.size.x = std::min(origin.x + size.x - other.origin.x, other.size.x);
    }
    else if (origin.x + size.x < other.origin.x)
    {
      result.origin.x = 0;
      result.size.x = 0;
    }
    else
    {
      assert(0);
    }
  }
  else if (offset < 0)
  {
    if (other.origin.x + other.size.x > origin.x)
    {
      result.origin.x = origin.x;
      result.size.x = std::min(other.origin.x + other.size.x - origin.x, size.x);
    }
    else if (other.origin.x + other.size.x <= origin.x)
    {
      result.origin.x = 0;
      result.size.x = 0;
    }
    else
    {
      assert(0);
    }
  }
  else
  {
    assert(0);
  }
  
  if (result.size.x == 0)
  {
    return result;
  }
  
  offset = other.origin.y - origin.y;
  if (offset >= 0)
  {
    if (origin.y + size.y >= other.origin.y)
    {
      result.origin.y = other.origin.y;
      result.size.y = std::min(origin.y + size.y - other.origin.y, other.size.y);
    }
    else if (origin.y + size.y < other.origin.y)
    {
      result.origin.y = 0;
      result.size.y = 0;
    }
    else
    {
      assert(0);
    }
  }
  else if (offset < 0)
  {
    if (other.origin.y + other.size.y > origin.y)
    {
      result.origin.y = origin.y;
      result.size.y = std::min(other.origin.y + other.size.y - origin.y, size.y);
    }
    else if (other.origin.y + other.size.y <= origin.y)
    {
      result.origin.y = 0;
      result.size.y = 0;
    }
    else
    {
      assert(0);
    }
  }
  else
  {
    assert(0);
  }
  
  if (result.size.y == 0)
  {
    result.size.x = 0;
    result.origin.x = 0;
  }
  
  return result;
}

bool Rect::In(const Rect& other) const
{
  return origin.x >= other.origin.x
  && origin.x + size.x <= other.origin.x + other.size.x
  && origin.y >= other.origin.y
  && origin.y + size.y <= other.origin.y + other.size.y;
}

bool Rect::operator==(const Rect& rect) const
{
  return origin.x == rect.origin.x &&
  origin.y == rect.origin.y &&
  size.x == rect.size.x &&
  size.y == rect.size.y;
}

bool Rect::operator!=(const Rect& rect) const
{
  return !(*this == rect);
}