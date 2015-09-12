//
//  Common.h
//  prsv
//
//  Created by Anton Simakov on 09.05.15.
//
//

#ifndef prsv_Common_h
#define prsv_Common_h

namespace komorki
{
  class PixelDescriptor;
  
  typedef std::function<void(PixelDescriptor* pixel, bool& stop)> PerPixelFunc;
  
  typedef int PixelPos;

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
    inline Vec2 operator-(const Vec2& pos) const {return Vec2(x - pos.x, y - pos.y);}
    inline Vec2 operator+(const Vec2& pos) const {return Vec2(x + pos.x, y + pos.y);}
    inline Vec2 operator-() const {return Vec2(-x, -y);}
    bool In(const Rect& rect) const;
    
    std::string Description()const { return std::to_string(x) + "," + std::to_string(y); }
  };;
  
  struct Rect
  {
    Vec2 origin;
    Vec2 size;
    Rect Extract(const Rect& rect) const;
    bool In(const Rect& rect) const;
    
    bool operator==(const Rect& rect) const;
    bool operator!=(const Rect& rect) const;
    inline PixelPos Top() const { return origin.y + size.y; }
    inline PixelPos Right() const { return origin.x + size.x; }
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
