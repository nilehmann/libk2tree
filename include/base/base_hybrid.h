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
#include <utils/libremainder.h>
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
using utils::Ceil;
using utils::LoadValue;
using utils::SaveValue;
using utils::ArrayQueue;
using libremainder::Divider;

struct Frame {
  cnt_size p, q;
  size_t z;
};

struct RangeFrame {
  cnt_size p1, p2, q1, q2;
  cnt_size dp, dq;
  size_t z;
};



struct DirectImpl;
struct InverseImpl;

/**
 * Base implementation for <em>k<sup>2</sup></em>tree with an hybrid approach.
 * It provides all functionality to represent and traverse the internal nodes,
 * but delegates the responsibility to explore the leaf level.
 * The template parameter specifies a concrete class implementing 
 * CheckLeafChild, RangeLeafBits and RangeLeafBits
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
    delete [] div_level_;
  }

  /**
   * Returns the number of objects in the original relation
   * @return Number of objects in the relation.
   */
  inline cnt_size cnt() const {
    return cnt_;
  }

  /**
   * Returns the number of links (ones in the matrix).
   * @return Number of links.
   */
  inline size_t links() const {
    return links_;
  }

  /**
   * Checks if exist a link from object p to q.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   *
   * @return True if exists a link between the specified objects, false
   * otherwise.
   */
  bool CheckLink(cnt_size p, cnt_size q) const {
    Divider<cnt_size> div_level;
    size_t z;
    uint k;
    z = 0;
    for (uint level = 0; level < height_ - 1; ++level) {
      if (level > 0 && !T_->access(z))
        return false;

      k = GetK(level);
      div_level = div_level_[level];

      z = Child(z, level, k);
      z += p/div_level*k + q/div_level;

      p %= div_level, q %= div_level;
    }
    if (!T_->access(z))
      return false;

    div_level = div_level_[height_ - 1];
    uint child = (uint) (p/div_level*kL_ + q/div_level);
    return CheckLeafChild(z, child);
  }

  /**
   * Iterates over all links in the given row.
   *
   * @param p Row in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object q such that p is related to q.
   * The function expects a unique parameter of type cnt_size.
   */
  template<class Function>
  void DirectLinks(cnt_size p, Function fun) const {
    Links<Function, DirectImpl>(p, fun);
  }

  /**
   * Iterates over all links in the given column.
   *
   * @param q Column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each 
   * object p such that p is related to q.
   * The function expects a unique parameter of type cnt_size.
   */
  template<class Function>
  void InverseLinks(cnt_size q, Function fun) const {
    Links<Function, InverseImpl>(q, fun);
  }

  /**
   * Iterates over all links in the specified submatrix.
   *
   * @param p1 Starting row in the matrix.
   * @param p2 Ending row in the matrix.
   * @param q1 Starting column in the matrix.
   * @param q2 Ending column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each 
   * pair of objects (p,q) such that p is related to q and (p,q) lies inside
   * the specified submatrix. The function expects two parameters of type
   * cnt_size.
   */
  template<class Function>
  void RangeQuery(cnt_size p1, cnt_size p2,
                  cnt_size q1, cnt_size q2,
                  Function fun) const {
    assert(p1 <= p2 && q1 <= q2);

    Divider<cnt_size> div_level;
    cnt_size div_p1, rem_p1, div_p2, rem_p2;
    cnt_size div_q1, rem_q1, div_q2, rem_q2;
    cnt_size dp, dq;
    // queue<RangeFrame> range_queue;
    range_queue.clear();

    range_queue.push({p1, p2, q1, q2, 0, 0, 0});
    uint level;
    for (level = 0; level < height_-1; ++level) {
      uint k = GetK(level);
      div_level = div_level_[level];

      uint cnt_level = (uint) range_queue.size();
      for (uint q = 0; q < cnt_level; ++q) {
        const RangeFrame &f = range_queue.front();
        size_t first = Child(f.z, level, k);

        div_p1 = f.p1/div_level, rem_p1= f.p1%div_level;
        div_p2 = f.p2/div_level, rem_p2 = f.p2%div_level;
        for (cnt_size i = div_p1; i <= div_p2; ++i) {
          size_t z = first + k*i;
          dp = f.dp + (cnt_size) div_level*i;
          p1 = i == div_p1 ? rem_p1 : 0;
          p2 = i == div_p2 ? rem_p2 : (cnt_size) div_level - 1;

          div_q1 = f.q1/div_level, rem_q1 = f.q1%div_level;
          div_q2 = f.q2/div_level, rem_q2 = f.q2%div_level;
          for (cnt_size j = div_q1; j <= div_q2; ++j) {
            dq = f.dq + (cnt_size) div_level*j;
            q1 = j == div_q1 ? rem_q1 : 0;
            q2 = j == div_q2 ? rem_q2 : (cnt_size) div_level-1;
            if (T_->access(z+j))
              range_queue.push({p1, p2, q1, q2, dp, dq, z + j});
          }
        }
        range_queue.pop();
      }
    }

    div_level = div_level_[height_ - 1];
    uint cnt_level = (uint) range_queue.size();
    for (uint q = 0; q < cnt_level; ++q) {
      const RangeFrame &f = range_queue.front();
      RangeLeafBits(f, div_level, fun);
      range_queue.pop();
    }
  }

  /**
   * Returns the position of the i-th child of the specified node
   *
   * @param z Position in T of the node.
   * @param level Level of the node between 0 and height_ - 1.
   * @param k Level arity.
   * @param i Child number between 0 and k<sup>2</sup> - 1.
   *
   * @return Position in T representing the child.
   */
  inline size_t Child(size_t z, uint level, uint kl, uint i = 0) const {
    assert(level < height_);
    // child_l(x,i) = rank(T_l, z - 1)*k_l^2 + i (0 <= i < k_l^2);
    // child_l(x,i) = (rank(T, x -1) - rank_{l-1})*k_l^2 + offset_{l+1} + i
    z = z > 0 ? (T_->rank1(z-1) - acum_rank_[level-1])*kl*kl : 0;
    return z + offset_[level+1] + i;
  }

  /**
   * Gets the value of k in the given level.
   *
   * @param level
   *
   * @return Arity of the given level.
   */
  inline uint GetK(uint level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kL_;
  }

  /**
   * Returns the memory used by the structure.
   * 
   * @return Size in bytes.
   */
  size_t GetSize() const {
    size_t size;
    size = 5*sizeof(uint);
    size += 2*sizeof(cnt_size);
    size += sizeof(size_t);
    size += sizeof(size_t*) + height_*sizeof(size_t);
    size += sizeof(size_t*) + (height_+1)*sizeof(size_t);
    size += T_->getSize() + sizeof(shared_ptr<BitSequence>);
    return size;
  }


 protected:
  /** Arity of the first part. */
  uint k1_;
  /** Arity of the second part. */
  uint k2_;
  /** Arity of the level height-1. */
  uint kL_;
  /** Last level with arity k1. */
  uint max_level_k1_;
  /** Height of the tree. */
  uint height_;
  /** Number of object. */
  cnt_size cnt_;
  /** Size of the expanded matrix. */
  cnt_size size_;
  /** Number of links */
  size_t links_;
  /** Size of submatrices children of each level. */
  Divider<cnt_size> *div_level_;
  /** Accumulated rank for each level. */
  size_t *acum_rank_;
  /** Starting position in T of each level. */
  size_t *offset_;
  /** Bit array with rank capability containing internal nodes. */
  shared_ptr<BitSequence> T_;



  /** Queue to traverse the tree in a range query */
  static ArrayQueue<RangeFrame> range_queue;
  /** Queue to traverse the tree */
  static ArrayQueue<Frame> neighbors_queue;


  /**
   * Builds a tree with and hybrid approach using the specified data that
   * correctly represents an hybrid <em>k<sup>2</sup></em>-tree.
   *
   * @param T Bit array with rank capability storing the internal nodes.
   * This can be shared be multiple instances.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kL Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the original matrix.
   * @param size Size of the expanded matrix.
   */
  base_hybrid(shared_ptr<BitSequence> T,
              uint k1, uint k2, uint kL, uint max_level_k1, uint height,
              cnt_size cnt, cnt_size size, size_t links)
      : k1_(k1),
        k2_(k2),
        kL_(kL),
        max_level_k1_(max_level_k1),
        height_(height),
        cnt_(cnt),
        size_(size),
        links_(links),
        div_level_(new Divider<cnt_size>[height]),
        acum_rank_(new size_t[height-1]),
        offset_(new size_t[height+1]),
        T_(T) {
    // To find the children of a node we need the accumulated rank until the
    // previous level. The last level with children is  height - 1, so we only
    // need acum_rank_ until level height - 2.
    // We also need the offset of the next level, so we must have offset_ until
    // level height (leaf level).
    acum_rank_[0] = 0;
    offset_[0] = offset_[1] = 0;
    offset_[2] = k1*k1;
    for (uint level = 1; level <= height - 2; ++level) {
      uint k = GetK(level);
      acum_rank_[level] = T_->rank1(offset_[level+1]-1);
      offset_[level+2] = (acum_rank_[level]-acum_rank_[level-1])*k*k;
      offset_[level+2] += offset_[level+1];
    }

    // We need the size of the submatrices children until level height - 1
    div_level_[0] = size_/GetK(0);
    for (uint level = 1; level < height; ++level)
      div_level_[level] = (cnt_size) div_level_[level-1]/GetK(level);
  }

  base_hybrid(const BitArray<uint> &T,
              uint k1, uint k2, uint kl, uint max_level_k1, uint height,
              cnt_size cnt, cnt_size size, size_t links)
      : base_hybrid(
            shared_ptr<BitSequence>(
                new BitSequenceRG(
                    const_cast<uint*>(T.GetRawData()),
                    T.length(),
                    20)),
            k1, k2, kl, max_level_k1, height, cnt, size, links) {}

  explicit base_hybrid(ifstream *in)
      : k1_(LoadValue<uint>(in)),
        k2_(LoadValue<uint>(in)),
        kL_(LoadValue<uint>(in)),
        max_level_k1_(LoadValue<uint>(in)),
        height_(LoadValue<uint>(in)),
        cnt_(LoadValue<cnt_size>(in)),
        size_(LoadValue<cnt_size>(in)),
        links_(LoadValue<size_t>(in)),
        div_level_(LoadValue<Divider<cnt_size>>(in, height_)),
        acum_rank_(LoadValue<size_t>(in, height_-1)),
        offset_(LoadValue<size_t>(in, height_+1)),
        T_(BitSequence::load(*in)) {}

  /**
   * Save the information into a file.
   * @param out Output Stream
   */
  void Save(ofstream *out) const {
    SaveValue(out, k1_);
    SaveValue(out, k2_);
    SaveValue(out, kL_);
    SaveValue(out, max_level_k1_);
    SaveValue(out, height_);
    SaveValue(out, cnt_);
    SaveValue(out, size_);
    SaveValue(out, links_);
    SaveValue(out, div_level_, height_);
    SaveValue(out, acum_rank_, height_-1);
    SaveValue(out, offset_, height_+1);
    T_->save(*out);
  }


  template<class Function, class Impl>
  void LeafBits(const Frame &f, Divider<cnt_size> div_level,
                Function fun) const {
    static_cast<const Hybrid&>(*this).
    template LeafBits<Function, Impl>(f, div_level, fun);
  }
  template<class Function>
  void RangeLeafBits(const RangeFrame &f, Divider<cnt_size> div_level,
                     Function fun) const {
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
  bool CheckLeafChild(size_t z, uint child) const {
    return static_cast<const Hybrid&>(*this).CheckLeafChild(z, child);
  }


  /**
   * Template implementation for DirectLinks and InverseLinks
   *
   * @param object
   * @param fun 
   */
  template<class Function, class Impl>
  void Links(cnt_size object, Function fun) const {
    Divider<cnt_size> div_level;
    uint cnt_level;
    uint k, level;
    // queue<Frame> neighbors_queue;
    neighbors_queue.clear();

    neighbors_queue.push(Impl::FirstFrame(object));
    for (level = 0; level < height_ - 1; ++level) {
      k = GetK(level);
      div_level = div_level_[level];

      cnt_level = (uint) neighbors_queue.size();
      for (uint i = 0; i < cnt_level; ++i) {
        const Frame &f = neighbors_queue.front();
        size_t z = Child(f.z, level, k) + Impl::Offset(f, k, div_level);
        for (uint j = 0; j < k; ++j) {
          if (T_->access(z))
            neighbors_queue.push(Impl::NextFrame(f.p, f.q, z, j, div_level));
          z = Impl::NextChild(z, k);
        }
        neighbors_queue.pop();
      }
    }

    div_level = div_level_[height_ - 1];
    cnt_level = (uint) neighbors_queue.size();
    for (uint i = 0; i < cnt_level; ++i) {
      Frame &f = neighbors_queue.front();
      LeafBits<Function, Impl>(f, div_level, fun);
      neighbors_queue.pop();
    }
  }
};


struct DirectImpl {
  inline static Frame FirstFrame(cnt_size p) {
    return {p, 0, 0};
  }
  inline static size_t NextChild(size_t z,  uint) {
    return z + 1;
  }
  inline static Frame NextFrame(cnt_size p, cnt_size q,
                                size_t z, uint j, Divider<cnt_size> div_level) {
    return {p % div_level, q + (cnt_size) div_level*j, z};
  }
  inline static cnt_size Offset(const Frame &f, uint k,
                                Divider<cnt_size> div_level) {
    return f.p/div_level*k;
  }

  inline static cnt_size Output(const Frame &f) {
    return f.q;
  }
};


struct InverseImpl {
  inline static Frame FirstFrame(cnt_size q) {
    return {0, q, 0};
  }
  inline static size_t NextChild(size_t z, uint k) {
    return z + k;
  }
  inline static Frame NextFrame(cnt_size p, cnt_size q,
                                size_t z, uint j, Divider<cnt_size> div_level) {
    return {p + (cnt_size) div_level*j, q % div_level, z};
  }
  inline static cnt_size Offset(const Frame &f, uint,
                                Divider<cnt_size> div_level) {
    return f.q/div_level;
  }
  inline static cnt_size Output(const Frame &f) {
    return f.p;
  }
};

template<class Hybrid>
ArrayQueue<RangeFrame> base_hybrid<Hybrid>::range_queue;

template<class Hybrid>
ArrayQueue<Frame> base_hybrid<Hybrid>::neighbors_queue;
}  // namespace libk2tree

#endif  // INCLUDE_BASE_BASE_HYBRID_H_
