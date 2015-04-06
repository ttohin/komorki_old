//
//  Statistic.h
//  komorki
//
//  Created by ttohin on 15.03.15.
//
//

#ifndef komorki_Statistic_h
#define komorki_Statistic_h

template <typename T>
class StatisticCounter {
public:
  T maxValue;
  T minValue;
  double avaregeValue;
  unsigned int number;
  
public:
  StatisticCounter():number(0) {}
  
  void AddValue(T value)
  {
    if(number == 0)
    {
      minValue = maxValue = value;
    }
    
    if (value < minValue) {
      minValue = value;
    }
    
    if (value > maxValue) {
      maxValue = value;
    }
    
    avaregeValue = ((avaregeValue * number) + value) / (number + 1);
  
    number += 1;
  }
};

#endif
