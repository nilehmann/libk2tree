/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <bits/basic_k2tree.h>

namespace libk2tree {

template<class _Size>
basic_k2tree<_Size>::basic_k2tree(const BitArray<unsigned int, _Size> &T,
                                 const BitArray<unsigned int, _Size> &L,
                                 int k1, int k2, int kl, int max_level_k1,
                                 int height, _Size cnt, _Size size) :
    T_(new BitSequenceRG(T.GetRawData(), T.length(), 20)),
    L_(L),
    k1_(k1),
    k2_(k2),
    kl_(kl),
    max_level_k1_(max_level_k1),
    height_(height),
    cnt_(cnt),
    size_(size),
    acum_rank_(new _Size[height]),
    offset_(new _Size[height+1]) {
  acum_rank_[0] = offset_[0] = 0;
  offset_[1] = k1*k1;
  for (int level = 1; level < height; ++level) {
    int k = level <= max_level_k1? k1 : k2;
    acum_rank_[level] = T_->rank1(offset_[level]-1);
    offset_[level+1] = (acum_rank_[level]-acum_rank_[level-1])*k*k;
    offset_[level+1] += offset_[level];
  }
}
template<class _Size>
basic_k2tree<_Size>::basic_k2tree(ifstream *in) :
    T_(BitSequence::load(*in)),
    L_(in),
    k1_(LoadValue<int>(in)),
    k2_(LoadValue<int>(in)),
    kl_(LoadValue<int>(in)),
    max_level_k1_(LoadValue<int>(in)),
    height_(LoadValue<int>(in)),
    cnt_(LoadValue<_Size>(in)),
    size_(LoadValue<_Size>(in)),
    acum_rank_(LoadValue<_Size>(in, height_)),
    offset_(LoadValue<_Size>(in, height_+1)) {}

template<class _Size>
basic_k2tree<_Size>::~basic_k2tree() {
  delete [] acum_rank_;
  delete [] offset_;
  delete T_;
}

template<class _Size>
bool basic_k2tree<_Size>::CheckLink(_Size p, _Size q) const {
  _Size N, div_level;
  size_t z;
  int k;

  N = size_;
  z = 0;
  for (int level = 0; level < height_; ++level) {
    k = GetK(level);

    div_level = N/k;
    if (level > 0 && T_->access(z))
      z = GetFirstChild(z, level, k);
    else if (level > 0)
      return false;

    int child = p/div_level*k + q/div_level;
    z += child;

    N /= k, p %= div_level, q %= div_level;
  }
  return L_.GetBit(z - T_->getLength());
}


template<class _Size>
void basic_k2tree<_Size>::Save(ofstream *out) const {
  T_->save(*out);
  L_.Save(out);
  SaveValue(out, k1_);
  SaveValue(out, k2_);
  SaveValue(out, kl_);
  SaveValue(out, max_level_k1_);
  SaveValue(out, height_);
  SaveValue(out, cnt_);
  SaveValue(out, size_);
  SaveValue(out, acum_rank_, height_);
  SaveValue(out, offset_, height_+1);
}

template<class _Size>
bool basic_k2tree<_Size>::operator==(const basic_k2tree &rhs) const {
  if (T_->getLength() != rhs.T_->getLength()) return false;
  for (size_t i = 0; i < T_->getLength(); ++i)
    if (T_->access(i) != rhs.T_->access(i)) return false;

  if (L_.length() != rhs.L_.length()) return false;
  for (size_t i = 0; i < L_.length(); ++i)
    if (L_.GetBit(i) != rhs.L_.GetBit(i)) return false;

  if (height_ != rhs.height_) return false;

  for (int i = 0; i < height_; ++i)
    if (acum_rank_[i] != acum_rank_[i]) return false;

  for (int i = 0; i <= height_; ++i)
    if (offset_[i] != offset_[i]) return false;

  return k1_ == rhs.k1_ && k2_ == rhs.k2_ && kl_ == rhs.kl_ &&
         max_level_k1_ == rhs.max_level_k1_ && size_ == rhs.size_;
}

template class basic_k2tree<unsigned int>;
template class basic_k2tree<size_t>;

}  // namespace libk2tree

