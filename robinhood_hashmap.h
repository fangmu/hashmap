#ifndef __ROBINHOOD_HASHMAP_H__
#define __ROBINHOOD_HASHMAP_H__

#include "hashmap.h"
#include "util.h"

namespace fm {
namespace map {

class RobinhoodHashMap : public HashMap {
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
      entry_ = NULL;
    }

    uint32_t hash_;
    Entry* entry_;
  };

  public:
    RobinhoodHashMap();
    RobinhoodHashMap(uint32_t init_num);
    RobinhoodHashMap(uint32_t init_num, uint32_t max_probe);

    ~RobinhoodHashMap();

  public:
    static const uint32_t DEFAULT_BUCKET_NUMBER = 64;
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
    bool PutBucket(Bucket& bucket, uint32_t cur_dib, uint32_t cur_idx);
    Entry* NewEntry(const std::string& key, const std::string& value);
    void ShiftEmptyBucketBack(uint32_t empty_idx);

  private:
    uint32_t bucket_num_;
    uint32_t max_probe_;
    uint32_t size_;
    Bucket* bucket_;
    HashFunPtr hash_fun_;
};
  
} /* map */
} /* fm */


#endif /* end of include guard: __ROBINHOOD_HASHMAP_H__ */
