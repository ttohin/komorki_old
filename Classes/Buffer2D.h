//
//  Buffer2D.h
//  ground
//
//  Created by ttohin on 05.09.15.
//
//

#ifndef __ground__Buffer2D__
#define __ground__Buffer2D__

#include <vector>



template <typename T, typename S = int>
class Buffer2D
{
public:
  
  Buffer2D(const S& _width, const S& _height) : width(_width), height(_height)
  {
    buff.reserve(width * height);
    buff.resize(width * height);
  }
  
  inline S GetWidth () const { return width; }
  inline S GetHeight () const { return height; }
  
  inline bool Set(const S& x, const S& y, const T& value)
  {
    if (!IsInside(x , y))
    {
      return false;
    }
    
    return SetInternal(x, y, value);
  }
  
  inline bool Get(const S& x, const S& y, T& value) const
  {
    if (!IsInside(x , y))
    {
      return false;
    }
    
    value = GetInternal(x, y);
    return true;
  }
  
  inline const T& Get(const S& x, const S& y, const T& defaultValue) const
  {
    if (!IsInside(x , y))
    {
      return defaultValue;
    }
    
    return GetInternal(x, y);
  }
  
  typedef std::function<void(const S& x, const S& y, const T& value)> OnValue;
  inline void ForEach(const OnValue& onValue) const
  {
    for (int i = 0; i < width; ++i)
    {
      for (int j = 0; j < height; ++j)
      {
        onValue(i, j, GetInternal(i, j));
      }
    }
  }
  
private:
  
  inline bool IsInside(const S& x, const S& y) const
  {
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
      return false;
    }
    
    return true;
  }
  
  inline bool SetInternal(const S& x, const S& y, const T& value)
  {
    buff[x + y * width] = value;
    return true;
  }
  
  inline const T& GetInternal(const S& x, const S& y) const
  {
    return buff[x + y * width];
  }
  
  std::vector<T> buff;
  S width;
  S height;
};

template <typename T>
using Buffer2DPtr = std::shared_ptr<Buffer2D<T>>;

#endif /* defined(__ground__Buffer2D__) */
