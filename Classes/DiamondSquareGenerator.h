//
//  DiamondSquareGeneratore.h
//  prsv
//
//  Created by ttohin on 16.08.15.
//
//

#ifndef __prsv__DiamondSquareGenerator__
#define __prsv__DiamondSquareGenerator__

#include <vector>
#include "Buffer2D.h"

namespace komorki
{
  class DiamondSquareGenerator
  {
  public:
    
    typedef std::function<void(int x, int y, float& value)> OnValue;
    typedef std::function<void(int x, int y, float& value, float& diff)> OnDiff;
    
    DiamondSquareGenerator(int width, int height, float roughness, float randDelta, bool enableOverflow);
    bool GetValue(int x, int y, float& value) const;
    void Generate(const OnValue& onValue = nullptr);
    void ForEach(const OnValue& onValue) ;
    void Step(const OnDiff& onDiff = nullptr, int value = 1, float roughness = 0.1);
    void Add(DiamondSquareGenerator* other, const OnDiff& onDiff);
    void Multiply(DiamondSquareGenerator* other, const OnDiff& onDiff);
    void Add(float diff);
    void Multiply(float value);
    Buffer2DPtr<float> GetBuffer(int x, int y, int width, int height) const;
    
  private:
    float Displace(float value, int blockSize, float roughness) const;
    bool SetValue(int x, int y, float value);
    float GetValue(int x, int y) const;
    void Square(int x, int y, int xBlockSize, int yBlockSize);
    void Diamond(int x, int y, int xBlockSize, int yBlockSize);
    void SquareDiamond(int x, int y, int xBlockSize, int yBlockSize);
    
    std::vector<float> m_data;
    float m_roughness;
    int m_width;
    int m_height;
    float m_randDelta;
    bool m_overflow;
  };
}

#endif /* defined(__prsv__DiamondSquareGenerator__) */
