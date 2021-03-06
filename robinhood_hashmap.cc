#include "robinhood_hashmap.h"
#include <iostream>
#include <sstream>
#include <string.h>

using namespace std;

namespace fm {
namespace map {
  
RobinhoodHashMap::RobinhoodHashMap() :
    bucket_num_(DEFAULT_BUCKET_NUMBER), max_probe_(DEFAULT_MAX_PROBE), size_(0) { 
  bucket_ = new Bucket[bucket_num_];
  hash_fun_ = fm::util::DefaultHashFun;
}

RobinhoodHashMap::RobinhoodHashMap(uint32_t init_num) :
    max_probe_(DEFAULT_MAX_PROBE), size_(0) {
  uint32_t adapt_num = fm::util::NearestOfTwoPower(init_num);
  if (adapt_num == 0) {
    adapt_num = DEFAULT_BUCKET_NUMBER;
  }
  bucket_num_ = adapt_num;
  bucket_ = new Bucket[bucket_num_];
  hash_fun_ = fm::util::DefaultHashFun;
}

RobinhoodHashMap::RobinhoodHashMap(uint32_t init_num, uint32_t max_probe) :
    max_probe_(max_probe), size_(0) {
  uint32_t adapt_num = fm::util::NearestOfTwoPower(init_num);
  if (adapt_num == 0) {
    adapt_num = DEFAULT_BUCKET_NUMBER;
  }
  bucket_num_ = adapt_num;
  bucket_ = new Bucket[bucket_num_];
  hash_fun_ = fm::util::DefaultHashFun;
}

RobinhoodHashMap::~RobinhoodHashMap() {
  if (bucket_ != NULL) {
    for (uint32_t i = 0; i < bucket_num_; ++i) {
      if (bucket_[i].entry_ != NULL) {
        if (bucket_[i].entry_->data_ != NULL) {
          delete[] bucket_[i].entry_->data_;
        }
        delete bucket_[i].entry_;
      }
    }
    delete[] bucket_;
    bucket_ = NULL;
    bucket_num_ = 0;
  } 
}

bool RobinhoodHashMap::Get(const string& key, string& value) {
  uint32_t hash = hash_fun_(key);

  for (uint32_t i = 0; i< max_probe_; ++i) {
    uint32_t cur_idx = (hash + i) % bucket_num_;
    if (bucket_[cur_idx].entry_ == NULL) {
      return false;
    }

    uint32_t cur_init = bucket_[cur_idx].hash_ % bucket_num_;
    uint32_t cur_dib = (cur_idx >= cur_init) ? (cur_idx - cur_init) : (cur_idx + bucket_num_ - cur_init);
    if (cur_dib < i) {
      return false;
    }

    if (bucket_[cur_idx].hash_ == hash &&
        bucket_[cur_idx].entry_->key_size_ == key.size() &&
        memcmp(bucket_[cur_idx].entry_->data_, key.c_str(), key.size()) == 0) {
      value.assign(bucket_[cur_idx].entry_->data_+key.size(), bucket_[cur_idx].entry_->value_size_);
      return true;
    }
  }
  return false;
}

bool RobinhoodHashMap::Put(const string& key, const string& value) {
  uint32_t hash = hash_fun_(key);

  for (uint32_t i = 0; i < max_probe_; ++i) {
    uint32_t cur_idx = (hash + i) % bucket_num_;
    if (bucket_[cur_idx].entry_ == NULL) {
      Entry* entry = NewEntry(key, value);
      bucket_[cur_idx].entry_ = entry;
      bucket_[cur_idx].hash_ = hash;
      return true;
    }

    uint32_t cur_init = bucket_[cur_idx].hash_ % bucket_num_;
    uint32_t cur_dib = (cur_idx >= cur_init) ? (cur_idx - cur_init) : (cur_idx + bucket_num_ - cur_init);
    if (cur_dib < i) {
      Entry* entry = NewEntry(key, value);
      Bucket tmp = bucket_[cur_idx];
      bucket_[cur_idx].hash_ = hash;
      bucket_[cur_idx].entry_ = entry;

      return PutBucket(tmp, cur_dib, cur_idx);
    }
  }
  return false;
}

RobinhoodHashMap::Entry* RobinhoodHashMap::NewEntry(const string& key, const string& value) {
  Entry* entry = new Entry();
  entry->key_size_ = key.size();
  entry->value_size_ = value.size();
  entry->data_ = new char[key.size() + value.size()];
  memcpy(entry->data_,            key.c_str(),   key.size());
  memcpy(entry->data_+key.size(), value.c_str(), value.size());
  return entry;
}

bool RobinhoodHashMap::PutBucket(Bucket& bucket, uint32_t cur_dib, uint32_t cur_idx) {

  for (uint32_t i = 1; i <= max_probe_; ++i) {
    uint32_t try_idx =  (cur_idx + i) % bucket_num_; 
    if (bucket_[try_idx].entry_ == NULL) {
      bucket_[try_idx] = bucket; 
      return true;
    }

    uint32_t try_init = bucket_[try_idx].hash_ % bucket_num_;
    uint32_t try_dib = (try_idx >= try_init) ? (try_idx - try_init) : (try_idx + bucket_num_ - try_init);
    if (try_dib < (cur_dib + i)) {
      Bucket tmp = bucket_[try_idx];
      bucket_[try_idx] = bucket;
      return PutBucket(tmp, try_dib, try_idx);
    }
  }
  return false;
}

bool RobinhoodHashMap::Remove(const string& key) {
  uint32_t hash = hash_fun_(key);
  for (uint32_t i = 0; i < max_probe_; ++i) {
    uint32_t cur_idx = (hash + i) % bucket_num_;
    if (bucket_[cur_idx].entry_ == NULL) {
      return false;
    }

    if (bucket_[cur_idx].hash_ == hash &&
        bucket_[cur_idx].entry_->key_size_ == key.size() &&
        memcmp(bucket_[cur_idx].entry_->data_, key.c_str(), key.size()) == 0) {
      delete[] bucket_[cur_idx].entry_->data_; 
      delete bucket_[cur_idx].entry_;
      bucket_[cur_idx].entry_ = NULL;

      ShiftEmptyBucketBack(cur_idx);
      return true;
    }


    uint32_t cur_init = bucket_[cur_idx].hash_ % bucket_num_;
    uint32_t cur_dib = (cur_idx >= cur_init) ? (cur_idx - cur_init) : (cur_idx + bucket_num_ - cur_init);

    if (cur_dib < i) {
      return false;
    }
  }
  return false;
}

void RobinhoodHashMap::ShiftEmptyBucketBack(uint32_t empty_idx) {
  for (uint32_t i = 1; i < bucket_num_; ++i) {
    uint32_t next_idx = (empty_idx + 1) % bucket_num_;
    if (bucket_[next_idx].entry_ == NULL) {
      return;
    }

    uint32_t next_init = bucket_[next_idx].hash_ % bucket_num_;
    if (next_idx == next_init) {
      return;
    }

    bucket_[empty_idx] = bucket_[next_idx];
    bucket_[next_idx].entry_ = NULL;
    empty_idx = next_idx;
  }
}

string RobinhoodHashMap::Dummy() {
  stringstream ss;  
  for (uint32_t i = 0; i < bucket_num_; ++i) {
    if (bucket_[i].entry_ != NULL) {
      ss << i << " bucket."
        << " hash: " << (bucket_[i].hash_ % bucket_num_)
        << " dib: " << (i - (bucket_[i].hash_ % bucket_num_))
        << " key: " << string(bucket_[i].entry_->data_, bucket_[i].entry_->key_size_) 
        << " value: " << string(bucket_[i].entry_->data_+bucket_[i].entry_->key_size_, bucket_[i].entry_->value_size_)
        << endl;
    }
  }
  return ss.str();
}

} /* map */
} /* fm */
