#pragma once
 
#include <cassert>
#include <initializer_list>
#include <vector>
#include <exception>
#include <algorithm>
#include <iostream>
#include <utility>
#include "array_ptr.h"
 
class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve)
        : capacity_(capacity_to_reserve) {}
 
    size_t GetCapacity() const {
        return capacity_;
    }
    
private:
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
};
 
template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
    SimpleVector() noexcept = default;
    
    explicit SimpleVector(size_t size) : SimpleVector(size,Type()) { 
    } 
     
    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size), simple_v_(size) { 
        std::fill(begin(), end(), value); 
    } 
     
    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()), simple_v_(init.size()) 
    { 
        std::copy(init.begin(),init.end(),begin());
    } 
    
    SimpleVector(const SimpleVector& other): size_(other.size_), capacity_(other.capacity_), simple_v_(other.size_){
        std::copy(other.begin(), other.end(), begin());
    }
   
    SimpleVector(SimpleVector&& other)
    {
        swap(other);
    }

    SimpleVector& operator=(const SimpleVector& other) { //копирующее присвоение
        if(this != &other){
            SimpleVector copy{ other };
            swap(copy);
        }
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& other)
    { //присвоение с перемещением 
        if(this != &other) {
            ArrayPtr<Type> simple_copy(other.size_);
            std::move(other.begin(), other.end(), simple_copy.Get());
            simple_v_.swap(simple_copy);
            size_ = other.size_;
            capacity_ = other.capacity_;
        }
        return *this;
        //я до конца так и не понял, что нужно переделать в данном операторе, создади копию, переместили
    }
    
    SimpleVector(ReserveProxyObj new_capacity)
    {
        Reserve(new_capacity.GetCapacity());
    }
    
    void Reserve(size_t new_capacity){
        if(new_capacity > capacity_){
            auto new_vec = ArrayPtr<Type>(new_capacity);
            for (size_t i = 0; i < capacity_; ++i){
                new_vec[i] = simple_v_[i];
            }
            simple_v_.swap(new_vec);
            capacity_ = new_capacity;
        }
    }
    
    ~SimpleVector() {}
    
    size_t GetSize() const noexcept {
        return size_;
    }
    
    size_t GetCapacity() const noexcept {
        return capacity_;
    }
    
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }
    
    Type& operator[](size_t index) noexcept {
        return simple_v_[index];
    }
    
    const Type& operator[](size_t index) const noexcept {
        return simple_v_[index];
    }
    
    Type& At(size_t index) {
        if (index >= size_){
            throw std::out_of_range("out of range");
        }
        return simple_v_[index];
    }
    
    const Type& At(size_t index) const {
        if (index > size_){
            throw std::out_of_range("out of range");
        }
        return simple_v_[index];
    }
    
    void Clear() noexcept {
        size_ = 0;
    }
    
    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            auto new_array = ArrayPtr<Type>(new_size);
            for (size_t i = 0; i < size_; ++i) {
                new_array[i] = std::move(simple_v_[i]);
            }
            simple_v_.swap(new_array);
            capacity_ = new_size;
        }
        for (size_t i = size_; i < new_size; ++i) {
            simple_v_[i] = Type();
        }
        size_ = new_size;
    }
    
    Iterator begin() noexcept {
        return simple_v_.Get();
    }
    
    Iterator end() noexcept {
        return simple_v_.Get() + size_;
    }
    
    ConstIterator begin() const noexcept {
        return simple_v_.Get();
    }
    
    ConstIterator end() const noexcept {
        return simple_v_.Get() + size_;
    }
    
    ConstIterator cbegin() const noexcept {
        return simple_v_.Get();
    }
    
    ConstIterator cend() const noexcept {
        return simple_v_.Get() + size_;
    }
    
    void PushBack(Type&& item) { 
       if(size_ == capacity_) 
       { 
            auto new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            auto new_vector = ArrayPtr<Type>(new_capacity); 
            for (size_t i = 0; i < size_; ++i) 
            { 
                new_vector[i] = std::move(simple_v_[i]); 
            } 
            simple_v_.swap(new_vector); 
            capacity_ = new_capacity; 
       } 
       simple_v_[size_] = std::move(item); 
       ++size_; 
    } 
    
    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t index = pos - begin();
        if(index > capacity_) {
            std::out_of_range("exit of out_of_range");
        }
        if(size_ == capacity_) {
            auto new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            auto new_vector = ArrayPtr<Type>(new_capacity);
            std::move(begin(), begin()+index, new_vector.Get());
            new_vector[index] = std::move(value);
            std::move(begin()+index, end(), new_vector.Get()+index+1);
            simple_v_.swap(new_vector);
            capacity_ = new_capacity;
        } else {
            std::move_backward(begin()+index, end(), end()+1);
            simple_v_[index] = std::move(value);
        }
        ++size_;
        return Iterator(simple_v_.Get() + index);
    }
    
    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }
    
    Iterator Erase(ConstIterator pos) {
        auto index = pos-begin();
        
        if(size_) {
            auto new_vector = ArrayPtr<Type>(capacity_);
            std::move(begin(), begin()+index, new_vector.Get());
            
            std::move(begin() + index+1, end(), new_vector.Get()+index);
            --size_;
            simple_v_.swap(new_vector);
        }
        return Iterator(begin() + index);
    }
    
    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        simple_v_.swap(other.simple_v_);
    }
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> simple_v_;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs == lhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
