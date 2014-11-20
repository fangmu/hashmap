#ifndef __HIPSCOTCHHASHMAP_H__
#define __HIPSCOTCHHASHMAP_H__

#include "hashmap.h"
#include "util.h"

namespace fm {
namespace map {

class HipscotchHashMap : public HashMap {
  struct Entry {
    Entry() {
      key_size_ = 0;
      value_size_ = 0;
      data_ = NULL;
    }
    uint32_t key_size_;
    uint32_t value_size_;
    char* data_;
  };

  struct Bucket {
    Bucket() {
      bitmap_ = 0;
    }

    uint32_t bitmap_;
    uint32_t hash_;
    Entry entry_;
  };

  public:
    HipscotchHashMap();
    HipscotchHashMap(uint32_t init_num);
    HipscotchHashMap(uint32_t init_num, uint32_t scotch_size, uint32_t max_probe);

    ~HipscotchHashMap();

  public:
    static const uint32_t DEFAULT_BUCKET_NUMBER = 4;
    static const uint32_t DEFAULT_SCOTCH_SIZE = 32;
    static const uint32_t DEFAULT_MAX_PROBE = 64;

    typedef uint32_t (*HashFunPtr)(const std::string& str);

  public:
    bool Put(const std::string& key, const std::string& value);
    bool Get(const std::string& key, std::string& value);
    bool Remove(const std::string& key);

    uint32_t Size() const {return size_;}

    void SetHashFun(HashFunPtr hash_fun);

    std::string Dummy();

  private:
    bool FindAndSwap(const uint32_t init_index, uint32_t& distance);

  private:
    uint32_t bucket_num_;
    uint32_t scotch_size_;
    uint32_t scotch_mask_;
    uint32_t max_probe_;
    uint32_t size_;
    Bucket* bucket_;
    HashFunPtr hash_fun_;
};

} /* map */
} /* fm */

#endif /* end of include guard: __HIPSCOTCHHASHMAP_H__ */
