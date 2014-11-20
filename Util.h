#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

namespace fm {
namespace util {

uint32_t NearestOfTwoPower(uint32_t value) {
  if (value >= (UINT32_MAX >> 1)) {
    return 0;
  } 

  uint32_t result = 1;
  while (result < value) {
    result <<= 1;
  }

  return result;
}
  
} /* util */
} /* fm */

#endif /* end of include guard: __UTIL_H__ */

