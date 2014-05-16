/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_UTILS_ARRAY_QUEUE_H_
#define INCLUDE_UTILS_ARRAY_QUEUE_H_

#include <libk2tree_basic.h>

namespace libk2tree {
namespace utils {

template<class T>
class ArrayQueue {
 public:
  explicit ArrayQueue(uint capacity = 10485760)
      : data_(reinterpret_cast<T*>(::operator new (sizeof(T) * capacity))),
        start_(0),
        end_(0) {}

  template<typename... Args>
  void emplace_back(Args... args) {
    new (&data_[end_]) T(args...);
    ++end_;
  }

  void push(const T &val) {
    new (&data_[end_]) T(val);
    ++end_;
  }
  void push(T &&val) {
    new (&data_[end_]) T(val);
    ++end_;
  }

  T &front() {
    return data_[start_];
  }

  void pop() {
    ++start_;
  }

  void clear() {
    start_ = end_ = 0;
  }

  uint size() {
    return end_ - start_;
  }

  ~ArrayQueue() {
    delete data_;
  }

 private:
  T *data_;
  uint start_;
  uint end_;
};


}  // namespace utils
}  // namespace libk2tree
#endif  // INCLUDE_UTILS_ARRAY_QUEUE_H_
