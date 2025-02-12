#include <cassert>
#include <cstdlib>
 
template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;
 
    explicit ArrayPtr(size_t size) {
        (size == 0) ? raw_ptr_ = nullptr : raw_ptr_ = new Type[size];
    }
 
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        if (raw_ptr) raw_ptr_ = raw_ptr;
    }
 
    ArrayPtr(const ArrayPtr&) = delete;
    ArrayPtr& operator=(const ArrayPtr&) = delete;
 
    ArrayPtr(ArrayPtr&& other)
    {
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = nullptr;
    }
 
    ~ArrayPtr() {
        delete[] raw_ptr_;
    }
 
    [[nodiscard]] Type* Release() noexcept {
        Type* currentarr = raw_ptr_;
        raw_ptr_ = nullptr;
        return currentarr;
    }
 
    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }
 
    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }
 
    explicit operator bool() const {
        return (raw_ptr_) ? true : false;
    }
 
    Type* Get() const noexcept {
        return raw_ptr_;
    }
 
    void swap(ArrayPtr& other) noexcept {
        Type* temp = other.raw_ptr_;
        other.raw_ptr_ = raw_ptr_;
        raw_ptr_ = temp;
    }
 
private:
    Type* raw_ptr_ = nullptr;
};
