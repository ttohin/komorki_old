//
//  Common.h
//  prsv
//
//  Created by Anton Simakov on 09.05.15.
//
//

#ifndef prsv_Common_h
#define prsv_Common_h

#include <functional>
#include <string>
#include <vector>

namespace komorki
{
  class GreatPixel;
  
  typedef std::function<void(GreatPixel* pixel, bool& stop)> PerPixelFunc;
  
  typedef int32_t PixelPos;

  struct Rect;
  
  struct Vec2
  {
    PixelPos x;
    PixelPos y;
    inline Vec2():x(0),y(0){}
    inline Vec2(PixelPos _x, PixelPos _y):x(_x),y(_y){}
    inline Vec2& operator=(const Vec2& pos)  {x = pos.x; y = pos.y; return *this; }
    inline bool operator==(const Vec2& pos) const {return x == pos.x && y == pos.y;}
    inline bool operator!=(const Vec2& pos) const {return !(*this==pos);}
    inline void operator+=(const Vec2& pos) {x += pos.x; y += pos.y;}
    inline Vec2 operator-(const Vec2& pos) const {return Vec2(x - pos.x, y - pos.y);}
    inline Vec2 operator+(const Vec2& pos) const {return Vec2(x + pos.x, y + pos.y);}
    inline Vec2 operator-() const {return Vec2(-x, -y);}
    inline void operator*=(const int& value) {x *= value; y *= value;}
    inline Vec2 operator/(const int& value) const { return Vec2(x / value,y / value); }
    bool In(const Rect& rect) const;
    Vec2 Normalize() const;
    
    std::string Description()const { return std::to_string(x) + "," + std::to_string(y); }
  };
  
  struct Vec2Hasher
  {
    std::size_t operator()(const Vec2& k) const
    {
      PixelPos v1 = k.x << 16;
      PixelPos v2 = v1 + k.y;
      
      auto r = std::hash<PixelPos>()(v2);
      return r;
    }
  };
  
  typedef const Vec2& Vec2ConstRef;
  
  struct Rect
  {
    Vec2 origin;
    Vec2 size;
    
    Rect(){}
    Rect(Vec2 _origin, Vec2 _size) : origin(_origin), size(_size) {}
    Rect(PixelPos x, PixelPos y, PixelPos width, PixelPos height) : origin(x, y), size(width, height) {}
    
    Rect Extract(const Rect& rect) const;
    bool In(const Rect& rect) const;
    
    bool operator==(const Rect& rect) const;
    bool operator!=(const Rect& rect) const;
    inline PixelPos Top() const { return origin.y + size.y - 1; }
    inline PixelPos Right() const { return origin.x + size.x - 1; }
    inline PixelPos Bottom() const { return origin.y; }
    inline void MoveBotton(const PixelPos& bottomOffset)
    {
      origin.y += bottomOffset;
      size.y -= bottomOffset;
    }
    inline PixelPos Left() const { return origin.x; }
    inline void MoveLeft(const PixelPos& leftOffset)
    {
      origin.x += leftOffset;
      size.x -= leftOffset;
    }
  };
  
  bool SplitRectOnChunks(const Rect& rect, const Rect& existingRect, const PixelPos chunkSize, std::vector<Rect>& result);
  

}

#endif
