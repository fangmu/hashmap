#include "util.h"
#include "murmurhash3.h"
#include <string.h>

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

uint32_t DefaultHashFun(const std::string& str) {
  char buf[32]; 
  uint32_t result;
  MurmurHash3_x86_32(str.c_str(), str.size(), 0, buf);
  memcpy(&result, buf, sizeof (uint32_t));
  return result;
}
  
} /* util */
} /* fm */
