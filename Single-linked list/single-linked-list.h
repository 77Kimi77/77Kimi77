#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <vector>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };
 
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;
 
        explicit BasicIterator(Node* node): node_(node) {
        }
 
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;
 
        BasicIterator() = default;
 
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }
 
        BasicIterator& operator=(const BasicIterator& rhs) = default;
 
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
 
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
 
        }
 
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
 
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }
 
        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;
        }
 
        BasicIterator operator++(int) noexcept {
            auto old_value(*this);
            ++(*this);
            return old_value;
        }
 
        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }
 
        [[nodiscard]] pointer operator->() const noexcept {
            return &node_->value;
        }
 
    private:
        Node* node_ = nullptr;
    };
 
public:
 
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
 
    SingleLinkedList() : head_(), size_(0) {};
 
    ~SingleLinkedList() { Clear(); };
 
    SingleLinkedList(std::initializer_list<Type> values)
    {
        SingleLinkedList currentlist;
        currentlist.Initialization(values.begin(), values.end());
        swap(currentlist);
    }
 
    SingleLinkedList(const SingleLinkedList& other)
    {
        SingleLinkedList currentlist;
        currentlist.Initialization(other.begin(), other.end());
        swap(currentlist);
    }
 
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if(rhs.IsEmpty()){
            Clear();
        }
        if (this != &rhs)
        {
            SingleLinkedList currentlist(rhs);
            swap(currentlist);
        }
        return *this;    
    }
 
    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }
 
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
 
    void Clear() noexcept {
        while (head_.next_node)
        {
            Node* new_node = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = new_node;
        }
        size_ = 0;
    }
 
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }
 
    [[nodiscard]] bool IsEmpty() const noexcept {
        return (size_ == 0);
    }
 
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;
 
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{ head_.next_node };
    }
 
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{ nullptr };
    }
 
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{ head_.next_node };
    }
 
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{ nullptr };
    }
 
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{ head_.next_node };
    }
 
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{ nullptr };
    }
    
    [[nodiscard]] Iterator before_begin() noexcept {
        // Реализуйте самостоятельно
        return Iterator{&head_};
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = pos.node_->next_node;
        pos.node_->next_node = new Node(value, new_node);
        ++size_;
        return Iterator{pos.node_->next_node};
    }

    void PopFront() noexcept {
        if(!IsEmpty()){
            Node* current_node = head_.next_node->next_node;
            delete head_.next_node;
            --size_;
            head_.next_node = current_node;
        }
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        if(!IsEmpty()){
            Node* eraser_node = pos.node_->next_node->next_node;
            delete pos.node_->next_node;
            --size_;
            pos.node_->next_node = eraser_node;
        }
        return Iterator{pos.node_->next_node};
    }
 
private:
    Node head_;
    size_t size_;

    template<typename iterator>
    void Initialization(iterator begin, iterator end)
    {
        Node* newnode = &head_;
        for (auto i = begin; i != end; ++i)
        {
            ++size_;
            newnode->next_node = new Node(*i, nullptr);
            newnode = newnode->next_node;
        }
    }
};
 
template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}
 
template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}
 
template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs > rhs);
}
 
template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
 
template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
