#include "hipscotch_hashmap.h"
#include "robinhood_hashmap.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <set>

using namespace std;
using namespace fm::map;

string ToString(int num) {
  stringstream ss;
  ss << num;
  return ss.str();
}

int main(int argc, const char *argv[]) {
  HashMap* hash_map = new RobinhoodHashMap(4000);
  set<string> keys;
  string value_dummy;

  for (int i = 0; i < 3000; i++) {
    string key = "key" + ToString(rand());
    if (keys.find(key) != keys.end()) {
      continue;
    }
    keys.insert(key);
    if (hash_map->Get(key, value_dummy)) {
      cout << i << "Get error. Key: " << key << endl;
      cout << hash_map->Dummy();
      return -1;
    }

    if (!hash_map->Put(key, key+ToString(-i))) {
      cout << i << "Put error. Key: " << key << endl;
      cout << hash_map->Dummy();
      return -1;
    }

    if (!hash_map->Get(key, value_dummy)) {
      cout << i << "Second Get error. Key: " << key << endl;
      cout << hash_map->Dummy();
      return -1;
    }

    //if (!hash_map->Remove(key)) {
    //  cout << i << "Remove error. Key: " << key << endl;
    //  cout << hash_map->Dummy();
    //  return -1;
    //}

    //if (hash_map->Get(key, value_dummy)) {
    //  cout << i << "Get after Remove fail. Key: " << key << endl;
    //  cout << hash_map->Dummy();
    //  return -1;
    //}
  }

  int j = 0;
  for (std::set<std::string>::iterator it = keys.begin(); it != keys.end(); ++it, ++j) {
    if (!hash_map->Get(*it, value_dummy)) {
      cout << j << "Third Get value error. Key: " << *it << endl;
      cout << hash_map->Dummy();
      return -1;
    }
    if (!hash_map->Remove(*it)) {
      cout << j << "Remove error. Key: " << *it << endl;
      cout << hash_map->Dummy();
      return -1;
    }

    if (hash_map->Get(*it, value_dummy)) {
      cout << j << "Get after Remove success. Key: " << *it << endl;
      cout << hash_map->Dummy();
      return -1;
    }
  } 

  cout << hash_map->Dummy();

  return 0;
}
