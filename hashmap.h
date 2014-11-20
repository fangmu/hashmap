#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <string>
#include <stdint.h>

namespace fm {
namespace map {
  
class HashMap {
  public:
    HashMap() {};
    virtual ~HashMap() {};

    virtual bool Put(const std::string& key, const std::string& value) = 0;
    virtual bool Get(const std::string& key, std::string& value) = 0;
    virtual bool Remove(const std::string& key) = 0;
    virtual uint32_t Size() const = 0;

    virtual std::string Dummy() = 0;
};

} /* map */
} /* fm */

#endif /* end of include guard: __HASHMAP_H__ */
