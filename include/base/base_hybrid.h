/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BASE_BASE_HYBRID_H_
#define INCLUDE_BASE_BASE_HYBRID_H_

#include <libk2tree_basic.h>
#include <utils/bitarray.h>
#include <utils/utils.h>
#include <utils/array_queue.h>
#include <BitSequence.h>  // libcds
#include <cstdlib>
#include <queue>
#include <memory>


namespace libk2tree {
using utils::BitArray;
using cds_static::BitSequence;
using cds_static::BitSequenceRG;
using std::ifstream;
using std::ofstream;
using std::queue;
using std::shared_ptr;
using utils::Ceil;
using utils::LoadValue;
using utils::SaveValue;
using utils::ArrayQueue;

struct Frame {
  uint p, q;
  uint z;
};

struct RangeFrame {
  uint p1, p2, q1, q2;
  uint dp, dq;
  uint z;
};



struct DirectImpl;
struct InverseImpl;

/**
 * Base implementation for k2trees with an hybrid aproach.
 * It provides all funcionality to represent and 
 * traverse the internal nodes, but delegates the responsability to explore
 * the leaf level.
 * The template parameter specifies the concrete class implementing 
 * LeafBits, RangeLeafBits and RangeLeafBits
 */
template<class Hybrid>
class base_hybrid {
 public:
  /**
   * Destructor
   */
  ~base_hybrid() {
    delete [] acum_rank_;
    delete [] offset_;
  }

  /**
   * Returns the number of objects in the original relation
   */
  inline uint cnt() const {
    return cnt_;
  }

  /**
   * Checks if exist a link from object p to q.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(uint p, uint q) const {
    uint N, div_level;
    uint z;
    int k;
    N = size_;
    z = 0;
    for (int level = 0; level < height_ - 1; ++level) {
      if (level > 0 && !T_->access(z))
        return false;

      k = GetK(level);
      div_level = N/k;

      z = Child(z, level, k);
      z += p/div_level*k + q/div_level;

      N = div_level, p %= div_level, q %= div_level;
    }
    if (!T_->access(z))
      return false;

    div_level = N/kl_;
    int child = p/div_level*kl_ + q/div_level;
    return CheckLeafChild(z, child);
  }
  /**
   * Iterates over all links in the given row.
   *
   * @param p Row in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object related to p.
   * The function expect a parameter of type uint.
   */
  template<class Function>
  void DirectLinks(uint p, Function fun) const {
    Neighbors<Function, DirectImpl>(p, fun);
  }

  /**
   * Iterates over all links in the given column.
   *
   * @param q Column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each 
   * object related to q.
   * The function expect a parameter of type uint.
   */
  template<class Function>
  void InverseLinks(uint q, Function fun) const {
    Neighbors<Function, InverseImpl>(q, fun);
  }

  /**
   * Iterates over all links in the specified submatrix.
   *
   * @param p1 Starting row in the matrix.
   * @param p2 Ending row in the matrix.
   * @param q1 Starting column in the matrix.
   * @param q2 Ending column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each 
   * pair of objects. The function expect two parameters of type uint
   */
  template<class Function>
  void RangeQuery(uint p1, uint p2, uint q1, uint q2, Function fun) const {
    assert(p1 <= p2 && q1 <= q2);

    uint div_level;
    uint div_p1, rem_p1, div_p2, rem_p2;
    uint div_q1, rem_q1, div_q2, rem_q2;
    uint dp, dq;
    // queue<RangeFrame> range_queue;
    range_queue.clear();

    range_queue.push({p1, p2, q1, q2, 0, 0, 0});
    uint N = size_;
    int level;
    for (level = 0; level < height_-1; ++level) {
      int k = GetK(level);
      div_level = N/k;

      uint cnt_level = range_queue.size();
      for (uint q = 0; q < cnt_level; ++q) {
        const RangeFrame &f = range_queue.front();
        uint first = Child(f.z, level, k);

        div_p1 = f.p1/div_level, rem_p1= f.p1%div_level;
        div_p2 = f.p2/div_level, rem_p2 = f.p2%div_level;
        for (uint i = div_p1; i <= div_p2; ++i) {
          uint z = first + k*i;
          dp = f.dp + div_level*i;
          p1 = i == div_p1 ? rem_p1 : 0;
          p2 = i == div_p2 ? rem_p2 : div_level - 1;

          div_q1 = f.q1/div_level, rem_q1 = f.q1%div_level;
          div_q2 = f.q2/div_level, rem_q2 = f.q2%div_level;
          for (uint j = div_q1; j <= div_q2; ++j) {
            dq = f.dq + div_level*j;
            q1 = j == div_q1 ? rem_q1 : 0;
            q2 = j == div_q2 ? rem_q2 : div_level-1;
            if (T_->access(z+j))
              range_queue.push({p1, p2, q1, q2, dp, dq, z + j});
          }
        }
        range_queue.pop();
      }
      N = div_level;
    }

    div_level = N/kl_;
    uint cnt_level = range_queue.size();
    for (uint q = 0; q < cnt_level; ++q) {
      const RangeFrame &f = range_queue.front();
      RangeLeafBits(f, div_level, fun);
      range_queue.pop();
    }
  }

  /**
   * Returns the child of the specified node
   *
   * @param z Position in T of the node.
   * @param level Level of the node.
   * @param k Level arity.
   * @param i Child number between 0 and k*k.
   *
   * @return Position in T representing the child.
   */
  inline uint Child(uint z, int level, int k, int i = 0) const {
    assert(level < height_);
    // child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
    z = z > 0 ? (T_->rank1(z-1) - acum_rank_[level-1])*k*k : 0;
    return z + offset_[level+1] + i;
  }

  /**
   * Gets the value of k in the given level.
   *
   * @param level
   *
   * @return Arity of the given level.
   */
  inline int GetK(int level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kl_;
  }

  /*
   * Get size in bytes.
   */
  size_t GetSize() const {
    size_t size;
    size = 5*sizeof(int);
    size += 2*sizeof(uint);
    size += sizeof(uint*) + height_*sizeof(uint);
    size += sizeof(uint*) + (height_+1)*sizeof(uint);
    size += T_->getSize() + sizeof(shared_ptr<BitSequence>);
    return size;
  }


 protected:
  // Arity of the first part.
  int k1_;
  // Arity of the second part.
  int k2_;
  // Arity of the level height-1.
  int kl_;
  // Last level with arity k1.
  int max_level_k1_;
  // Height of the tree.
  int height_;
  // Number of object.
  uint cnt_;
  // Size of the expanded matrix.
  uint size_;
  // Accumulated rank for each level.
  uint *acum_rank_;
  // Accumulated number of nodes util each level, inclusive.
  uint *offset_;
  // Bit array with rank capability containing internal nodes.
  shared_ptr<BitSequence> T_;

  static ArrayQueue<RangeFrame> range_queue;
  static ArrayQueue<Frame> neighbors_queue;


  base_hybrid(shared_ptr<BitSequence> T,
               int k1, int k2, int kl, int max_level_k1, int height,
               uint cnt, uint size)
      : k1_(k1),
        k2_(k2),
        kl_(kl),
        max_level_k1_(max_level_k1),
        height_(height),
        cnt_(cnt),
        size_(size),
        acum_rank_(new uint[height-1]),
        offset_(new uint[height+1]),
        T_(T) {
    acum_rank_[0] = 0;
    offset_[0] = offset_[1] = 0;
    offset_[2] = k1*k1;
    // To find the children of a node we need the accumulate rank
    // until the previous level. The last level with children is
    // height -1, so we only need acum_rank_ until level height - 2.
    // We need the offset until level height (leaf level)
    for (int level = 1; level <= height - 2; ++level) {
      int k = level <= max_level_k1? k1 : k2;
      acum_rank_[level] = T_->rank1(offset_[level+1]-1);
      offset_[level+2] = (acum_rank_[level]-acum_rank_[level-1])*k*k;
      offset_[level+2] += offset_[level+1];
    }
  }

  base_hybrid(const BitArray<uint> &T,
               int k1, int k2, int kl, int max_level_k1, int height,
               uint cnt, uint size)
      : base_hybrid(
            shared_ptr<BitSequence>(
                new BitSequenceRG(
                    const_cast<uint*>(T.GetRawData()),
                    T.length(),
                    20)),
            k1, k2, kl, max_level_k1, height, cnt, size) {}

  explicit base_hybrid(ifstream *in)
      : k1_(LoadValue<int>(in)),
        k2_(LoadValue<int>(in)),
        kl_(LoadValue<int>(in)),
        max_level_k1_(LoadValue<int>(in)),
        height_(LoadValue<int>(in)),
        cnt_(LoadValue<uint>(in)),
        size_(LoadValue<uint>(in)),
        acum_rank_(LoadValue<uint>(in, height_-1)),
        offset_(LoadValue<uint>(in, height_+1)),
        T_(BitSequence::load(*in)) {}

  void Save(ofstream *out) const {
    SaveValue(out, k1_);
    SaveValue(out, k2_);
    SaveValue(out, kl_);
    SaveValue(out, max_level_k1_);
    SaveValue(out, height_);
    SaveValue(out, cnt_);
    SaveValue(out, size_);
    SaveValue(out, acum_rank_, height_-1);
    SaveValue(out, offset_, height_+1);
    T_->save(*out);
  }


  template<class Function, class Impl>
  void LeafBits(const Frame &f, uint div_level, Function fun) const {
    static_cast<const Hybrid&>(*this).
    template LeafBits<Function, Impl>(f, div_level, fun);
  }
  template<class Function>
  void RangeLeafBits(const RangeFrame &f, uint div_level, Function fun) const {
    static_cast<const Hybrid&>(*this).
    template RangeLeafBits<Function>(f, div_level, fun);
  }


  /**
   * Checks a child of the given node in the leaf level. This functionality
   * is delegated and must be implemented by a concrete hybrid k2tree.
   *
   * @param z Position in T of the node.
   * @param child Child number between 0 and kL * kL.
   *
   * @return true if the child is 1 and false otherwise.
   */
  bool CheckLeafChild(uint z, int child) const {
    return static_cast<const Hybrid&>(*this).CheckLeafChild(z, child);
  }


  /*
   * Template implementation for DirectLinks and InverseLinks
   */
  template<class Function, class Impl>
  void Neighbors(uint object, Function fun) const {
    uint div_level;
    uint cnt_level;
    int k, level;
    // queue<Frame> neighbors_queue;
    neighbors_queue.clear();

    neighbors_queue.push(Impl::FirstFrame(object));
    uint N = size_;
    for (level = 0; level < height_ - 1; ++level) {
      k = GetK(level);
      div_level = N/k;

      cnt_level = neighbors_queue.size();
      for (uint i = 0; i < cnt_level; ++i) {
        const Frame &f = neighbors_queue.front();
        uint z = Child(f.z, level, k) + Impl::Offset(f, k, div_level);
        for (int j  = 0; j < k; ++j) {
          if (T_->access(z))
            neighbors_queue.push(Impl::NextFrame(f.p, f.q, z, j, div_level));
          z = Impl::NextChild(z, k);
        }
        neighbors_queue.pop();
      }
      N = div_level;
    }

    div_level = N/kl_;
    cnt_level = neighbors_queue.size();
    for (uint i = 0; i < cnt_level; ++i) {
      Frame &f = neighbors_queue.front();
      LeafBits<Function, Impl>(f, div_level, fun);
      neighbors_queue.pop();
    }
  }
};

struct DirectImpl {
  inline static Frame FirstFrame(uint p) {
    return {p, 0, 0};
  }
  inline static uint NextChild(uint z,  int) {
    return z  + 1;
  }
  inline static Frame NextFrame(uint p, uint q, uint z, int j, uint div_level) {
    return {p % div_level, q + div_level*j, z};
  }
  inline static uint Offset(const Frame &f, int k, uint div_level) {
    return f.p/div_level*k;
  }

  inline static uint Output(const Frame &f) {
    return f.q;
  }
};


struct InverseImpl {
  inline static Frame FirstFrame(uint q) {
    return {0, q, 0};
  }
  inline static uint NextChild(uint z, int k) {
    return z + k;
  }
  inline static Frame NextFrame(uint p, uint q, uint z, int j, uint div_level) {
    return {p + div_level*j, q % div_level, z};
  }
  inline static uint Offset(const Frame &f, int, uint div_level) {
    return f.q/div_level;
  }
  inline static uint Output(const Frame &f) {
    return f.p;
  }
};


template<class Hybrid>
ArrayQueue<RangeFrame> base_hybrid<Hybrid>::range_queue;

template<class Hybrid>
ArrayQueue<Frame> base_hybrid<Hybrid>::neighbors_queue;
}  // namespace libk2tree

#endif  // INCLUDE_BASE_BASE_HYBRID_H_
