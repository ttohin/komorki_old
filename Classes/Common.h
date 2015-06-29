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
  
  struct Vec2
  {
    PixelPos x;
    PixelPos y;
    Vec2():x(0),y(0){}
    Vec2(PixelPos _x, PixelPos _y):x(_x),y(_y){}
    Vec2& operator=(const Vec2& pos)  {x = pos.x; y = pos.y; return *this; }
    bool operator==(const Vec2& pos) const {return x == pos.x && y == pos.y;}
    bool operator!=(const Vec2& pos) const {return !(*this==pos);}
    Vec2 operator-(const Vec2& pos) const {return Vec2(x - pos.x, y - pos.y);}
  };
}

#endif
