#ifndef MEASUREMENT_METRIC_HPP_
#define MEASUREMENT_METRIC_HPP_

#include <numeric>
#include <stdint.h>



class Metric {
public:
  Metric(Metric const&) = delete;
  uint8_t dLdv[3] = {0, 0, 0};  // Value (RGB)
  uint8_t dsqr    =  0;         // Distance, squared
  uint8_t vdir[2] = {0, 0};     // Direction as a fraction
};



#endif // MEASUREMENT_METRIC_HPP_
