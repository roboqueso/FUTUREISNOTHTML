#ifndef __Util_h__
#define __Util_h__

#include "cinder/Vector.h"
#include <math.h>

class Util
{

public:

  static inline double ExponentialDecay(double exponent, double startTime, double curTime) {
    static const double e = 2.7182818;
    return pow(e, -exponent * (curTime - startTime));
  }

  static ci::Vec4f GenVec4(int numPoints, int pointIndex) {
    ci::Vec4f point;
    point[0] = static_cast<float>(pointIndex+1) / static_cast<float>(numPoints);
    for (int i=1; i<4; i++) {
      point[i] = static_cast<float>(radicalInverseFolded(pointIndex+1, i+1));
    }
    return point;
  }

  template <int windowSize>
  struct ExponentialSmoother {
    ExponentialSmoother() : avgValue(0) { }
    void Update(const double& data) {
      avgValue = (2*data + (windowSize - 1)*avgValue) / (windowSize + 1);
    }
    double avgValue;
  };

private:

  static inline double radicalInverseFolded(int n, int base) {
    double val = 0;
    double invBase = 1.0 / static_cast<double>(base);
    double invBi = invBase;
    int dI;
    int i = 0;
    while (val + base*invBi != val) {
      dI = (n+i) % base;
      val += static_cast<double>(dI) * invBi;
      n = static_cast<int>(n * invBase);
      invBi *= invBase;
      i++;
    }
    return val;
  }

};

#endif
