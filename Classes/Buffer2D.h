//
//  Buffer2D.h
//  ground
//
//  Created by ttohin on 05.09.15.
//
//

#pragma once

#include <vector>
#include <functional>
#include <memory>

namespace komorki
{
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
    
    inline const T& GetWithDefault(const S& x, const S& y, const T& defaultValue) const
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
    
    inline bool SubSet(const S& _x, const S& _y, Buffer2D<T>& bufferOut) const
    {
      if (!IsInside(_x, _y) || !IsInside(_x + bufferOut.GetWidth() - 1, _y + bufferOut.GetHeight() - 1)) return false;
      
      for (int i = 0; i < bufferOut.GetWidth(); ++i)
      {
        for (int j = 0; j < bufferOut.GetHeight(); ++j)
        {
          bufferOut.Set(i, j, GetInternal(i + _x, j + _y));
        }
      }
      return true;
    }
    
    inline void Scale(unsigned int scale, Buffer2D<T>& bufferOut) const
    {
      Buffer2D<T> result(width * scale, height * scale);
      for (int i = 0; i < width; ++i)
      {
        for (int j = 0; j < height; ++j)
        {
          T value = GetInternal(i, j);
          
          for (int iScale = 0; iScale < scale; ++iScale)
          {
            for (int jScale = 0; jScale < scale; ++jScale)
            {
              bufferOut.Set(i * scale + iScale, j * scale + jScale, value);
            }
          }
        }
      }
    }
    
    inline void Fill(const T& value)
    {
      for (int i = 0; i < width; ++i)
      {
        for (int j = 0; j < height; ++j)
        {
          SetInternal(i, j, value);
        }
      }
    }
    
    inline std::string Description() const
    {
      std::stringstream ss;
      ss << "[" << width << ", "<< height << "]" << std::endl;
      for (int j = height - 1; j >= 0; --j)
      {
        for (int i = 0; i < width; ++i)
        {
          ss << GetInternal(i, j);
        }
        ss << std::endl;
      }
      
      return ss.str();
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
    
    inline T GetInternal(const S& x, const S& y) const
    {
      return buff[x + y * width];
    }
    
    std::vector<T> buff;
    S width;
    S height;
  };
  
  template <typename T>
  using Buffer2DPtr = std::shared_ptr<Buffer2D<T>>;
}


