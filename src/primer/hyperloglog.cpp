#include "hyperloglog.h"

namespace bustub {

// HyperLogLog 的构造函数，初始化基数、前置位数和桶
// n_bits 是前置位数，决定了桶的数量
template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits)
    : cardinality_(0), b_(n_bits > 0 ? n_bits : 0), buckets_(std::vector<uint64_t>(n_bits > 0 ? 1 << n_bits : 1)) {}
        //使用初始化列表来初始化类的成员变量。
        //cardinality_(0) 初始化基数为0，
        //b_(n_bits > 0 ? n_bits : 0) 初始化前置位数为n_bits，如果n_bits小于等于0，则设置为0，
        //buckets_(std::vector<uint64_t>(n_bits > 0 ? 1 << n_bits : 1)) 初始化桶
        //为一个大小为2^n_bits的向量，如果n_bits小于等于0，则设置桶的大小为1。


// 计算给定哈希值的二进制文件。哈希值应转换为 64 位二进制流（否则测试可能会失败）。
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {   //hash_t 是size_t类型
  std::bitset<BITSET_CAPACITY> bits(hash);
  return bits;
}

// 计算哈希值的左侧第一个1的位置
template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  int i = BITSET_CAPACITY - 1 - b_;
  uint64_t ans = 1;
  while (i >= 0 && !bset.test(i)) {         //从左往右（最高位开始）i>0且哈希值不为1时，往右循环
    ans++;
    i--;        
  }
  return ans;                 //返回p值，记录到桶中
}

// 计算值并将其放入寄存器中
template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  hash_t hashcode = CalculateHash(val);
  std::bitset<BITSET_CAPACITY> bits = ComputeBinary(hashcode);
  uint64_t p = PositionOfLeftmostOne(bits);
  uint64_t index = (bits >> (BITSET_CAPACITY - b_)).to_ulong();
  buckets_[index] = std::max(buckets_[index], p);
}

// 计算基数
// 计算公式：E = α * m^2 / Σ(2^-Z_i
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  double sum = 0;
  uint64_t m = (1 << b_);
  for (uint64_t i = 0; i < m; i++) {
    // 不要对无符号数取负数，会变成一个很大的数！要先转换为有符号数后再取负数，要么就少用无符号数！
    sum += pow(2.0, -static_cast<double>(buckets_[i]));
  }
  cardinality_ = (CONSTANT * m * m) / sum;
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub