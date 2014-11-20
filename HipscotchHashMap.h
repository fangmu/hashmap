#ifndef __HIPSCOTCHHASHMAP_H__
#define __HIPSCOTCHHASHMAP_H__

#include "HashMap.h"

namespace fm {
namespace map {

class HipscotchHashMap : public HashMap {
  public:
    HipscotchHashMap();
    HipscotchHashMap(int32_t init_size);
    HipscotchHashMap(int32_t init_size, int32_t max_probe, int32_t );

    ~HipscotchHashMap();

  public:
    bool Add(const std::string& key, const std::string& value);
    bool Get(const std::string& key, std::string& value);
    bool Remove(const std::string& key);

  private:
    int32_t size_;
    int32_t count_;
};

} /* map */
} /* fm */

#endif /* end of include guard: __HIPSCOTCHHASHMAP_H__ */
