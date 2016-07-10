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

Vec2 Vec2::Normalize() const
{
  float max = std::max(std::abs(x), std::abs(y));
  if (max == 0.f)
  {
    return {0, 0};
  }
  
  auto res = Vec2(std::round((float)x/max), std::round((float)y/max));
  return res;
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


bool komorki::SplitRectOnChunks(const Rect& rect, const Rect& existingRect, const PixelPos chunkSize, std::vector<Rect>& result)

{
  if (rect.size.x == 0 || rect.size.y == 0)
    return false;
  
  uint mapSegmentSize = MIN(chunkSize, rect.size.x);
  mapSegmentSize = MIN(chunkSize, rect.size.y);
  
  int stepsX = rect.size.x/mapSegmentSize + 1;
  int stepsY = rect.size.y/mapSegmentSize + 1;
  
  for (int i = 0; i < stepsX; ++i)
  {
    for (int j = 0; j < stepsY; ++j)
    {
      int width = MIN(mapSegmentSize, rect.size.x - i*mapSegmentSize);
      int height = MIN(mapSegmentSize, rect.size.y - j*mapSegmentSize);
      
      if (width <= 0 || height <= 0)
      {
        continue;
      }
      
      Rect mapRect;
      mapRect.origin.x = rect.origin.x + i*mapSegmentSize;
      mapRect.origin.y = rect.origin.y + j*mapSegmentSize;
      mapRect.size.x = width;
      mapRect.size.y = height;
      
      if (!mapRect.In(existingRect))
      {
        result.push_back(mapRect);
      }
    }
  }
  
  return true;
}
