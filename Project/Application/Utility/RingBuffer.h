#pragma once
#include <vector>
#include <memory>
#include <stdexcept>

template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity)
        : capacity_(capacity), buffer_(std::make_unique<T[]>(capacity)), head_(0), size_(0) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
    }
    void Push(const T& item) {
        buffer_[head_] = item;
        head_ = (head_ + 1) % capacity_;
        if (size_ < capacity_) {
            ++size_;
        }
    }
    T Get(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        size_t realIndex = (head_ + capacity_ - size_ + index) % capacity_;
        return buffer_[realIndex];
    }
    size_t Size() const {
        return size_;
    }
    size_t Capacity() const {
        return capacity_;
    }
    bool IsFull() const {
        return size_ == capacity_;
    }
    void Clear() {
        head_ = 0;
        size_ = 0;
    }
private:
    size_t capacity_;
    std::unique_ptr<T[]> buffer_;
    size_t head_;
    size_t size_;
};