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
#include <type_traits>
#include <utility>

namespace libk2tree {
namespace utils {

/**
 * Queue implemented with a fixed size array.
 */
template<class T, size_t Capacity = 10485760>
class ArrayQueue {
 public:
  explicit ArrayQueue()
      : data_(reinterpret_cast<T*>(::operator new (sizeof(T) * Capacity))),
        start_(0),
        end_(0) {}

  template<typename... Args>
  void emplace_back(Args... args) {
    new (&data_[end_]) T(std::forward<Args...>(args...));
    ++end_;
  }

  /**
   * Add value to the end of the queue
   */
  void push(const T &val) {
    new (&data_[end_]) T(val);
    ++end_;
  }

  /**
   * Add value to the end of the queue
   */
  void push(T &&val) {
    new (&data_[end_]) T(val);
    ++end_;
  }

  /**
   * Returns the value in the front of the queue.
   *
   * @return Const reference to the value.
   */
  const T &front() const {
    return data_[start_];
  }

  /**
   * Returns the value in the front of the queue.
   *
   * @return Reference to the value.
   */
  T &front() {
    return const_cast<T&>(static_cast<const ArrayQueue&>(*this).front());
  }


  /**
   * Removes element at front.
   */
  void pop() {
    ++start_;
  }

  void clear() {
    start_ = end_ = 0;
  }

  size_t size() const {
    return end_ - start_;
  }

  ~ArrayQueue() {
    delete data_;
  }

 private:
  /** Array with the elements */
  T *data_;
  /** Position of the first element */
  size_t start_;
  /** Position of the last element */
  size_t end_;
};


}  // namespace utils
}  // namespace libk2tree
#endif  // INCLUDE_UTILS_ARRAY_QUEUE_H_
