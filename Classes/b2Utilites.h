//
//  b2Utilites.h
//  testBox2DiOS
//
//  Created by ttohin on 05.11.12.
//
//


#include <stdio.h>
#include <stdlib.h>

#define RANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)
#define RANDOM_0_1() ((float)rand()/RAND_MAX)

#ifndef testBox2DiOS_b2Utilites_h
#define testBox2DiOS_b2Utilites_h

#define PTM_RATIO 32

#define cEqualFloats(f1, f2, epsilon) ( fabs( (f1) - (f2) ) <= epsilon )
#define cEqualPoints(p1, p2, epsilon) (cEqualFloats(p1.x, p2.x, epsilon) && cEqualFloats(p1.y, p2.y, epsilon))
#define cBetween(x, a, b) (cEqualFloats(((b - a) - (x - a)), 0.f, 0.2f))

#define cRandEps(x, eps) (CCRANDOM_MINUS1_1()*eps + x)
#define cRandAB(a, b) (CCRANDOM_0_1()*(b - a) + a)
#define cRandABInt(a, b) (rand()%(b - a) + a)

#define cBoolRandPercent(percent) (CCRANDOM_0_1() <= percent)

#define cccb2(cgp) (CGPointMake(cgp.x * PTM_RATIO, cgp.y  * PTM_RATIO))

#endif
