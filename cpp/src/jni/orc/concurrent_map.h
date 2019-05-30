/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 */

#ifndef JNI_ID_TO_MODULE_MAP_H
#define JNI_ID_TO_MODULE_MAP_H

#include <memory>
#include <unordered_map>
#include <utility>

#include "arrow/util/macros.h"

namespace arrow {

template <typename HOLDER>
class concurrentMap {
 public:
  concurrentMap() : module_id_(kInitModuleId) {}

  jlong Insert(HOLDER holder) {
    mtx_.lock();
    jlong result = module_id_++;
    map_.insert(std::pair<jlong, HOLDER>(result, holder));
    mtx_.unlock();
    return result;
  }

  void Erase(jlong module_id) {
    mtx_.lock();
    map_.erase(module_id);
    mtx_.unlock();
  }

  HOLDER Lookup(jlong module_id) {
    HOLDER result = NULLPTR;
    try {
      result = map_.at(module_id);
    } catch (const std::out_of_range& e) {
    }
    if (result != NULLPTR) {
      return result;
    }
    mtx_.lock();
    try {
      result = map_.at(module_id);
    } catch (const std::out_of_range& e) {
    }
    mtx_.unlock();
    return result;
  }

  void Clear() {
    mtx_.lock();
    map_.clear();
    mtx_.unlock();
  }

 private:
  static const int kInitModuleId = 4;

  int64_t module_id_;
  std::mutex mtx_;
  // map from module ids returned to Java and module pointers
  std::unordered_map<jlong, HOLDER> map_;
};

}  // namespace arrow

#endif  // JNI_ID_TO_MODULE_MAP_H
