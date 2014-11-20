#include "hipscotch_hashmap.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <bitset>

using namespace std;

namespace fm {
namespace map {
  
HipscotchHashMap::HipscotchHashMap() :
    bucket_num_(DEFAULT_BUCKET_NUMBER), scotch_size_(DEFAULT_SCOTCH_SIZE),
    max_probe_(DEFAULT_MAX_PROBE), size_(0) {
  scotch_mask_ = ~((uint64_t)(~0) << scotch_size_);
  bucket_ = new Bucket[bucket_num_];
  hash_fun_ = fm::util::DefaultHashFun;
}

HipscotchHashMap::HipscotchHashMap(uint32_t init_num) :
    scotch_size_(DEFAULT_SCOTCH_SIZE), max_probe_(DEFAULT_MAX_PROBE), size_(0) {
  uint32_t adapt_num = fm::util::NearestOfTwoPower(init_num);
  if (adapt_num == 0) {
    adapt_num = DEFAULT_BUCKET_NUMBER;
  }
  bucket_num_ = adapt_num;
  scotch_mask_ = ~((uint64_t)(~0) << scotch_size_);
  bucket_ = new Bucket[bucket_num_];
  hash_fun_ = fm::util::DefaultHashFun;
}
    
HipscotchHashMap::HipscotchHashMap(uint32_t init_num, uint32_t scotch_size, uint32_t max_probe) :
    max_probe_(max_probe), size_(0) {
  uint32_t adapt_num = fm::util::NearestOfTwoPower(init_num);
  if (adapt_num == 0) {
    adapt_num = DEFAULT_BUCKET_NUMBER;
  }
  bucket_num_ = adapt_num;

  if (scotch_size > (sizeof (uint32_t)*8)) {
    scotch_size = (sizeof (uint32_t) * 8);
  }
  scotch_size_ = scotch_size;
  scotch_mask_ = ~((uint64_t)(~0) << scotch_size_);

  bucket_ = new Bucket[bucket_num_];
  hash_fun_ = fm::util::DefaultHashFun;
}

HipscotchHashMap::~HipscotchHashMap() {
  if (bucket_ != NULL) {
    for (uint32_t i = 0; i < bucket_num_; ++i) {
      if (bucket_->entry_.data_ == NULL) {
        delete[] bucket_->entry_.data_;
      }
    }
    delete[] bucket_;
    bucket_ = NULL;
  }
}

bool HipscotchHashMap::Get(const string& key, string& value) {
  uint32_t hash = hash_fun_(key);
  Bucket& init_bucket = bucket_[hash % bucket_num_];

  if ((init_bucket.bitmap_ & scotch_mask_) == 0) {
    return false;
  }

  for (uint32_t i = 0; i < scotch_size_; ++i) {
    if ((init_bucket.bitmap_ & (1 << i)) == 0) {
      continue;
    }

    Bucket& current_bucket = bucket_[(hash + i) % bucket_num_];
    if (current_bucket.hash_ == hash &&
        current_bucket.entry_.key_size_ == key.size() &&
        strncmp(current_bucket.entry_.data_, key.c_str(), key.size()) == 0) {

      value.assign(current_bucket.entry_.data_ + current_bucket.entry_.key_size_, 
                   current_bucket.entry_.value_size_);
      return true;
    }
  }
  return false;
}

bool HipscotchHashMap::Put(const string& key, const string& value) {
  uint32_t hash = hash_fun_(key);
  uint32_t init_index = hash % bucket_num_;

  if (bucket_[init_index].entry_.data_ == NULL) {
    bucket_[init_index].entry_.key_size_ = key.size();
    bucket_[init_index].entry_.value_size_ = value.size();
    bucket_[init_index].entry_.data_ = new char[key.size() + value.size()];
    strncpy(bucket_[init_index].entry_.data_,              key.c_str(), key.size());
    strncpy(bucket_[init_index].entry_.data_ + key.size(), value.c_str(), value.size());
    bucket_[init_index].hash_ = hash;
    bucket_[init_index].bitmap_ |= 1;
    size_++;
    return true;
  }

  bool found = false;
  uint32_t distance = 1;
  for (; distance < max_probe_; ++distance) {
    Bucket& bucket = bucket_[(init_index + distance) % bucket_num_]; 
    if (bucket.entry_.data_ == NULL) {
      found = true;    
      break;
    }
  }

  if (found) {
    do {
      if (distance < scotch_size_) {
        uint32_t insert_index = (init_index + distance) % bucket_num_;
        bucket_[insert_index].entry_.key_size_ = key.size();
        bucket_[insert_index].entry_.value_size_ = value.size();
        bucket_[insert_index].entry_.data_ = new char[key.size() + value.size()];
        strncpy(bucket_[insert_index].entry_.data_,              key.c_str(),   key.size());
        strncpy(bucket_[insert_index].entry_.data_ + key.size(), value.c_str(), value.size());
        bucket_[insert_index].hash_ = hash;
        bucket_[init_index].bitmap_ |= 1 << distance; 
        size_++;
        return true;
      }
    } while (FindAndSwap(init_index, distance));
  }

  return false;
}

bool HipscotchHashMap::FindAndSwap(const uint32_t init_index, uint32_t& distance) {
  
  for (uint32_t try_distance = distance - scotch_size_ + 1 ; try_distance < distance; ++try_distance) {
    Bucket& try_bucket = bucket_[(init_index + try_distance) % bucket_num_];
    if ((try_bucket.bitmap_ & scotch_mask_) == 0) {
      continue;
    }

    bool found = false;
    uint32_t swap_distance = 0;
    for (uint32_t j = 0; j < (scotch_size_ - 1) && (try_distance+j) < distance; ++j) {
      if ((try_bucket.bitmap_ & (1 << j)) != 0) {
        found = true;
        swap_distance = j;
        break;
      }
    }

    if (found) {
      uint32_t new_distance = try_distance + swap_distance;
      Bucket& swap_bucket = bucket_[(init_index + new_distance) % bucket_num_];
      Bucket& empty_bucket = bucket_[(init_index + distance) % bucket_num_];

      Entry tmp = swap_bucket.entry_;

      swap_bucket.entry_ = empty_bucket.entry_;

      empty_bucket.entry_ = tmp;
      empty_bucket.hash_ = swap_bucket.hash_;

      try_bucket.bitmap_ &=  (~(1 << swap_distance));
      try_bucket.bitmap_ |=  ((1 << (distance - try_distance)));
      distance = new_distance ;
      return true;
    }
  } 
  return false;
}

bool HipscotchHashMap::Remove(const string& key) {
  uint32_t hash = hash_fun_(key);
  Bucket& init_bucket = bucket_[hash % bucket_num_];

  if ((init_bucket.bitmap_ & scotch_mask_) == 0) {
    return false;
  }

  for (uint32_t i = 0; i < scotch_size_; ++i) {
    if ((init_bucket.bitmap_ & (1 << i)) == 0) {
      continue;
    }
    Bucket& current_bucket = bucket_[(hash + i) % bucket_num_];
    if (current_bucket.hash_ == hash && current_bucket.entry_.data_ != NULL &&
        current_bucket.entry_.key_size_ == key.size() &&
        strncmp(current_bucket.entry_.data_, key.c_str(), key.size()) == 0) {

      current_bucket.entry_.key_size_ = 0;
      current_bucket.entry_.value_size_ = 0;
      delete[] current_bucket.entry_.data_;
      init_bucket.bitmap_ &= (~(1 << i));
      size_--;

      return true;
    }
  }
  return false;
}

string HipscotchHashMap::Dummy() {
  stringstream ss;  
  for (uint32_t i = 0; i < bucket_num_; ++i) {
    if (bucket_[i].entry_.data_ != NULL) {
      bitset<32> bitmap(bucket_[i].bitmap_);
      ss << i << " bucket."
        << " hash: " << (bucket_[i].hash_ % bucket_num_)
        << " bitmap:" << bitmap 
        << " key: " << string(bucket_[i].entry_.data_, bucket_[i].entry_.key_size_) 
        << " value: " << string(bucket_[i].entry_.data_+bucket_[i].entry_.key_size_, bucket_[i].entry_.value_size_)
        << endl;
    }
  }
  return ss.str();
}

} /* map */
} /* fm */
