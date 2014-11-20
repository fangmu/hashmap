#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <string>

namespace fm {
  
class HashMap {
  public:
    HashMap() {};
    virtual ~HashMap() {};

    virtual bool Add(const std::string& key, const std::string& value) = 0;
    virtual bool Get(const std::string& key, std::string& value) = 0;
    virtual bool Remove(const std::string& key) = 0;
};

} /* fm */

#endif /* end of include guard: __HASHMAP_H__ */
