#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include <string>

namespace fm {
namespace util {

uint32_t NearestOfTwoPower(uint32_t value);

uint32_t DefaultHashFun(const std::string& str);
  
} /* util */
} /* fm */

#endif /* end of include guard: __UTIL_H__ */
