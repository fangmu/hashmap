#include "HipscotchHashMap.h"

using namespace std;

namespace fm {

HipscotchHashMap::HipscotchHashMap() {
  size_ = 4;
  count_ = 0;
}

HipscotchHashMap::HipscotchHashMap(int32_t init_size) {
  size_ = init_size;
  count_ = 0;
}

HipscotchHashMap::~HipscotchHashMap() {

}

bool HipscotchHashMap::Add(const string& key, const string& value) {

}

bool HipscotchHashMap::Get(const string& key, string& value) {

}

bool HipscotchHashMap::Remove(const string& key) {

}

} /* fm */
