//
//  b2Utilites.h
//  testBox2DiOS
//
//  Created by ttohin on 05.11.12.
//
//


#include <stdio.h>
#include <stdlib.h>

#ifndef Komorki_Utilites_h
#define Komorki_Utilites_h

#define RANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)
#define RANDOM_0_1() ((float)rand()/RAND_MAX)

#define PTM_RATIO 32

#define cEqualFloats(f1, f2, epsilon) ( fabs( (f1) - (f2) ) <= epsilon )
#define cEqualPoints(p1, p2, epsilon) (cEqualFloats(p1.x, p2.x, epsilon) && cEqualFloats(p1.y, p2.y, epsilon))
#define cBetween(x, a, b) (cEqualFloats(((b - a) - (x - a)), 0.f, 0.2f))

#define cRandEps(x, eps) (RANDOM_MINUS1_1()*eps + x)
#define cRandAB(a, b) (RANDOM_0_1()*(b - a) + a)

inline int cRandABInt(int a, int b)
{
  if (a == b)
  {
    return a;
  }
  
  if (b < a)
  {
    std::swap(a, b);
  }
  
  return rand()%abs(b - a) + a;
}




#define cBoolRandPercent(percent) (RANDOM_0_1() <= percent)
#define cRandPlusMinus() (rand()%2 == 0 ? -1 : 1)


inline int cRandAorB(int a, int b, float percent = 0.5f)
{
  return cBoolRandPercent(percent) ? a : b;
}

#define cccb2(cgp) (CGPointMake(cgp.x * PTM_RATIO, cgp.y  * PTM_RATIO))

#define KOMORKI_DEBUG_UI
#ifdef KOMORKI_DEBUG_UI
#define KOMORKI_LOG(...) cocos2d::log(__VA_ARGS__)
#else
#define KOMORKI_LOG(...) {}
#endif

#define LOG_UPDATES

#define DEBUG_PARTIAL_MAP
//#define DEBUG_VIEWPORT

#endif // Komorki_Utilites_h
