#pragma once
#include <cassert>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <new>
#include <utility>
#include <iterator>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity) : buffer_(Allocate(capacity)), capacity_(capacity) 
    {}

    ~RawMemory() { 
        Deallocate(buffer_); 
    }

    RawMemory(const RawMemory&) = delete;
    RawMemory(RawMemory&& other) noexcept : buffer_(std::exchange(other.buffer_, nullptr))
        , capacity_(std::exchange(other.capacity_, 0)) 
    {}

    RawMemory& operator=(RawMemory&& rhs);
    RawMemory& operator=(const RawMemory& rhs) = delete;

    T* operator+(size_t offset) noexcept; 
    const T* operator+(size_t offset) const noexcept;
    const T& operator[](size_t index) const noexcept;
    T& operator[](size_t index) noexcept;

    void Swap(RawMemory& other) noexcept;
    const T* GetAddress() const noexcept;
    T* GetAddress() noexcept;
    size_t Capacity() const;

private:
    T* buffer_ = nullptr;
    size_t capacity_ = 0;

    static T* Allocate(size_t n) { 
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }
    static void Deallocate(T* buf) noexcept {
        operator delete(buf); 
    }
};


template <typename T>
class Vector {
public:

    using iterator = T*;
    using const_iterator = const T*;

    Vector() = default;

    explicit Vector(size_t size) : data_(size), size_(size) {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }

    Vector(const Vector& other) : data_(other.size_), size_(other.size_) {
        std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
    }

    Vector(Vector&& other) noexcept : data_(std::move(other.data_))
        , size_(std::exchange(other.size_, 0)) {}

    ~Vector() { std::destroy_n(data_.GetAddress(), size_); }

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

    size_t Size() const noexcept;
    size_t Capacity() const noexcept;
    void Swap(Vector& other) noexcept;
    void Reserve(size_t new_capacity);
    void Resize(size_t new_size);

    template <typename... Args>
    T& EmplaceBack(Args&&... args);

    void CopyOrMoveData(RawMemory<T>& new_data);

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args);

    template<typename ...Args>
    iterator RelocateEmplace(const_iterator pos, Args && ...args); //метод с релокацией

    template<typename ...Args>
    iterator NonRelocateEmplace(const_iterator pos, Args && ...args); //метод без релокации
    iterator Insert(const_iterator pos, const T& item);
    iterator Insert(const_iterator pos, T&& item);
    iterator Erase(const_iterator pos);

    template <typename... Args>
    T& PushBack(Args&&... args);
    void PopBack(); 

    Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other) noexcept;
    const T& operator[](size_t index) const noexcept;
    T& operator[](size_t index) noexcept;

private:
    RawMemory<T> data_;
    size_t size_ = 0;
};

/*
    все методы вынесены за пределы класса, 
    стало гораздо проще читать код и понимать, 
    какие методы есть у разных классов
*/
//методы для памяти

template<typename T>
inline RawMemory<T>& RawMemory<T>::operator=(RawMemory&& rhs) {

    if (this != &rhs) {
        buffer_ = std::move(rhs.buffer_);
        capacity_ = std::move(rhs.capacity_);
        rhs.buffer_ = nullptr;
        rhs.capacity_ = 0;
    }
    return *this;
}

template <typename T>
inline T* RawMemory<T>::operator+(size_t offset) noexcept {
    assert(offset <= capacity_); 
    return buffer_ + offset;
}

template <typename T>
inline const T* RawMemory<T>::operator+(size_t offset) const noexcept {
    return const_cast<RawMemory&>(*this) + offset;
}

template <typename T>
inline const T& RawMemory<T>::operator[](size_t index) const noexcept {
    return const_cast<RawMemory&>(*this)[index];
}

template <typename T>
inline T& RawMemory<T>::operator[](size_t index) noexcept {
    assert(index < capacity_); 
    return buffer_[index];
}

template <typename T>
void RawMemory<T>::Swap(RawMemory& other) noexcept {
    std::swap(buffer_, other.buffer_); 
    std::swap(capacity_, other.capacity_);
}

template <typename T>
const T* RawMemory<T>::GetAddress() const noexcept {
    return buffer_;
}

template <typename T>
T* RawMemory<T>::GetAddress() noexcept {
    return buffer_;
}

template <typename T>
size_t RawMemory<T>::Capacity() const {
    return capacity_;
}

// методы для вектора

template<typename T>
inline Vector<T>::iterator Vector<T>::begin() noexcept
{
    return data_.GetAddress();
}

template<typename T>
inline Vector<T>::iterator Vector<T>::end() noexcept {
    return size_ + data_.GetAddress();
}

template<typename T>
inline Vector<T>::const_iterator Vector<T>::cbegin() const noexcept {
    return data_.GetAddress();
}

template<typename T>
inline Vector<T>::const_iterator Vector<T>::cend() const noexcept {
    return size_ + data_.GetAddress();
}

template<typename T>
inline Vector<T>::const_iterator Vector<T>::begin() const noexcept {
    return cbegin();
}

template<typename T>
inline Vector<T>::const_iterator Vector<T>::end() const noexcept {
    return cend();
}

template <typename T>
size_t Vector<T>::Size() const noexcept {
    return size_;
}

template <typename T>
size_t Vector<T>::Capacity() const noexcept {
    return data_.Capacity();
}

template <typename T>
void Vector<T>::Swap(Vector& other) noexcept {
    data_.Swap(other.data_), std::swap(size_, other.size_);
}

template <typename T>
void Vector<T>::Reserve(size_t new_capacity) {

    if (new_capacity <= data_.Capacity()) {
        return;
    }

    RawMemory<T> new_data(new_capacity);

    if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
    }
    else {
        std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
    }

    std::destroy_n(data_.GetAddress(), size_);
    data_.Swap(new_data);
}

template <typename T>
void Vector<T>::Resize(size_t new_size) {

    if (new_size < size_) {
        std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);

    }
    else {
        if (new_size > data_.Capacity()) {
            const size_t new_capacity = std::max(data_.Capacity() * 2, new_size);

            Reserve(new_capacity);
        }
        std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
    }
    size_ = new_size;
}

template <typename T>
template <typename... Args>
T& Vector<T>::PushBack(Args&&... args) {
    return EmplaceBack(std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
T& Vector<T>::EmplaceBack(Args&&... args) {

    if (data_.Capacity() <= size_) {

        RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
        new (new_data.GetAddress() + size_) T(std::forward<Args>(args)...);
        CopyOrMoveData(new_data);
    }
    else {
        new (data_.GetAddress() + size_) T(std::forward<Args>(args)...);
    }

    return data_[size_++];
}

template <typename T>
void Vector<T>::CopyOrMoveData(RawMemory<T>& new_data) {
    if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
    }
    else {
        std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
    }

    std::destroy_n(data_.GetAddress(), size_);
    data_.Swap(new_data);
}

template <typename T>
template <typename... Args>
typename Vector<T>::iterator Vector<T>::Emplace(const_iterator pos, Args&&... args) {
    assert(pos >= begin() && pos <= end());
    int position = pos - begin();

    if (data_.Capacity() <= size_) {
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            return RelocateEmplace(pos, std::forward<Args>(args)...);
        }
        else {
            return NonRelocateEmplace(pos, std::forward<Args>(args)...);
        }
    }
    else {
        try {
            if (pos != end()) {
                T new_s(std::forward<Args>(args)...);
                new (end()) T(std::forward<T>(data_[size_ - 1]));

                std::move_backward(begin() + position, end() - 1, end());
                *(begin() + position) = std::forward<T>(new_s);
            }
            else {
                new (end()) T(std::forward<Args>(args)...);
            }
        }
        catch (...) {
            operator delete(end());
            throw;
        }
    }

    size_++;
    return begin() + position;
}

template <typename T>
template <typename... Args>
typename Vector<T>::iterator Vector<T>::RelocateEmplace(const_iterator pos, Args&&... args) {
    assert(pos >= begin() && pos <= end());
    int position = pos - begin();

    if (data_.Capacity() <= size_) {

        RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);

        new (new_data.GetAddress() + position) T(std::forward<Args>(args)...);

        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data_.GetAddress(), position, new_data.GetAddress());
            std::uninitialized_move_n(data_.GetAddress() + position, size_ - position, new_data.GetAddress() + position + 1);
        }
        else {
            std::uninitialized_copy_n(data_.GetAddress(), position, new_data.GetAddress());
            std::uninitialized_copy_n(data_.GetAddress() + position, size_ - position, new_data.GetAddress() + position + 1);
        }

        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }
    else {

        try {

            if (pos != end()) {

                T new_s(std::forward<Args>(args)...);
                new (end()) T(std::forward<T>(data_[size_ - 1]));

                std::move_backward(begin() + position, end() - 1, end());
                *(begin() + position) = std::forward<T>(new_s);
            }
            else {
                new (end()) T(std::forward<Args>(args)...);
            }
        }
        catch (...) {
            operator delete(end());
            throw;
        }
    }

    size_++;
    return begin() + position;
}

template <typename T>
template <typename... Args>
typename Vector<T>::iterator Vector<T>::NonRelocateEmplace(const_iterator pos, Args&&... args) {
    assert(pos >= begin() && pos <= end());
    int position = pos - begin();

    if (data_.Capacity() <= size_) {

        RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);

        new (new_data.GetAddress() + position) T(std::forward<Args>(args)...);
        std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());

        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }
    else {

        try {

            if (pos != end()) {

                T new_s(std::forward<Args>(args)...);
                new (end()) T(std::forward<T>(data_[size_ - 1]));

                std::move_backward(begin() + position, end() - 1, end());
                *(begin() + position) = std::forward<T>(new_s);
            }
            else {
                new (end()) T(std::forward<Args>(args)...);
            }
        }
        catch (...) {
            operator delete(end());
            throw;
        }
    }

    size_++;
    return begin() + position;
}

template <typename T>
typename Vector<T>::iterator Vector<T>::Insert(const_iterator pos, const T& item) {
    return Emplace(pos, item); 
}

template <typename T>
typename Vector<T>::iterator Vector<T>::Insert(const_iterator pos, T&& item) {
    return Emplace(pos, std::move(item)); 
}

template <typename T>
typename Vector<T>::iterator Vector<T>::Erase(const_iterator pos) {

    assert(pos >= begin() && pos <= end());
    int position = pos - begin();

    std::move(begin() + position + 1, end(), begin() + position);
    std::destroy_at(end() - 1);
    --size_;

    return (begin() + position);
}

template <typename T>
void Vector<T>::PopBack() {
    assert(size_);
    std::destroy_at(data_.GetAddress() + size_ - 1);
    --size_;
}

template<typename T>
inline Vector<T>& Vector<T>::operator=(const Vector& other)
{
    if (this != &other) {

        if (other.size_ <= data_.Capacity()) {
            size_t min_size = std::min(size_, other.size_); // использование std::min для нахождения размера
            std::copy_n(other.data_.GetAddress(), min_size, data_.GetAddress()); //копирование перед циклом проверки
            if (size_ <= other.size_) {
                std::uninitialized_copy_n(other.data_.GetAddress() + size_,
                    other.size_ - size_,
                    data_.GetAddress() + size_);
            }
            else {
                std::destroy_n(data_.GetAddress() + other.size_,
                    size_ - other.size_);
            }

            size_ = other.size_;

        }
        else {
            Vector other_copy(other);
            Swap(other_copy);
        }
    }
    return *this;
}

template<typename T>
inline Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
    Swap(other);
    return *this;
}

template<typename T>
inline const T& Vector<T>::operator[](size_t index) const noexcept {
    return const_cast<Vector&>(*this)[index];
}

template<typename T>
inline T& Vector<T>::operator[](size_t index) noexcept {
    assert(index < size_);
    return data_[index];
}

