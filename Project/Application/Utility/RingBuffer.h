#pragma once
#include <vector>
#include <memory>
#include <stdexcept>

/// @brief 固定容量のリングバッファクラス
template <typename T>
class RingBuffer {
public:
    /// @brief 指定した容量でリングバッファを初期化
    explicit RingBuffer(size_t capacity)
        : capacity_(capacity), buffer_(std::make_unique<T[]>(capacity)), head_(0), size_(0) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
    }
    /// @brief アイテムを追加、容量超過時は古いデータを上書き
    void Push(const T& item) {
        buffer_[head_] = item;
        head_ = (head_ + 1) % capacity_;
        if (size_ < capacity_) {
            ++size_;
        }
    }
    /// @brief 指定したインデックスのアイテムを取得
    T& Get(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        size_t realIndex = (head_ + capacity_ - size_ + index) % capacity_;
        return buffer_[realIndex];
    }
    /// @brief 指定したインデックスのアイテムを取得（const版）
    const T& Get(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        size_t realIndex = (head_ + capacity_ - size_ + index) % capacity_;
        return buffer_[realIndex];
    }

    /// @brief 現在のサイズ、容量、満杯判定、クリア
    size_t Size() const {
        return size_;
    }
    /// @brief バッファの容量を取得
    size_t Capacity() const {
        return capacity_;
    }
    /// @brief バッファが満杯かどうかを判定
    bool IsFull() const {
        return size_ == capacity_;
    }
    /// @brief バッファをクリア
    void Clear() {
        head_ = 0;
        size_ = 0;
    }

    /// @brief 現在のヘッドインデックスを取得
    size_t GetHeadIndex() const {
        return head_;
    }
    /// @brief 内部バッファへのポインタを取得
    T* GetBufferPointer() const {
        return buffer_.get();
    }
    /// 次の挿入位置を取得
    size_t GetNextInsertIndex() const {
        return head_;
    }

private:
    size_t capacity_;
    std::unique_ptr<T[]> buffer_;
    size_t head_;
    size_t size_;
};