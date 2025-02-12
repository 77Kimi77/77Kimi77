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
        if (this != &rhs)
        {
            SingleLinkedList currentlist(rhs);
            swap(currentlist);
        }
        return *this;    
    }
 
    // Обменивает содержимое списков за время O(1)
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
        return (size_ == 0) ? true : false;
    }
 
    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
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
    return (lhs < rhs) || (lhs == rhs);
}
 
template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
 
template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs > rhs) || (lhs == rhs);
}

void Test3() {
    {
        SingleLinkedList<int> list_1;
        list_1.PushFront(1);
        list_1.PushFront(2);
 
        SingleLinkedList<int> list_2;
        list_2.PushFront(1);
        list_2.PushFront(2);
        list_2.PushFront(3);
 
        SingleLinkedList<int> list_1_copy;
        list_1_copy.PushFront(1);
        list_1_copy.PushFront(2);
 
        SingleLinkedList<int> empty_list;
        SingleLinkedList<int> another_empty_list;
 
        assert(list_1 == list_1);
        assert(empty_list == empty_list);
 
        assert(list_1 == list_1_copy);
        assert(list_1 != list_2);
        assert(list_2 != list_1);
        assert(empty_list == another_empty_list);
    }

    {
        SingleLinkedList<int> first;
        first.PushFront(1);
        first.PushFront(2);
 
        SingleLinkedList<int> second;
        second.PushFront(10);
        second.PushFront(11);
        second.PushFront(15);
 
        const auto old_first_begin = first.begin();
        const auto old_second_begin = second.begin();
        const auto old_first_size = first.GetSize();
        const auto old_second_size = second.GetSize();
 
        first.swap(second);
 
        assert(second.begin() == old_first_begin);
        assert(first.begin() == old_second_begin);
        assert(second.GetSize() == old_first_size);
        assert(first.GetSize() == old_second_size);
 
        {
            using std::swap;

            swap(first, second);

            assert(first.begin() == old_first_begin);
            assert(second.begin() == old_second_begin);
            assert(first.GetSize() == old_first_size);
            assert(second.GetSize() == old_second_size);
        }
    }
 
    {
        SingleLinkedList<int> list{ 1, 2, 3, 4, 5 };
        assert(list.GetSize() == 5);
        assert(!list.IsEmpty());
        assert(std::equal(list.begin(), list.end(), std::begin({ 1, 2, 3, 4, 5 })));
    }
 
    {
        using IntList = SingleLinkedList<int>;
 
        assert((IntList{ 1, 2, 3 } < IntList{ 1, 2, 3, 1 }));
        assert((IntList{ 1, 2, 3 } <= IntList{ 1, 2, 3 }));
        assert((IntList{ 1, 2, 4 } > IntList{ 1, 2, 3 }));
        assert((IntList{ 1, 2, 3 } >= IntList{ 1, 2, 3 }));
    }
 
    {
        const SingleLinkedList<int> empty_list{};
        {
            auto list_copy(empty_list);
            assert(list_copy.IsEmpty());
        }
 
        SingleLinkedList<int> non_empty_list{ 1, 2, 3, 4 };
        {
            auto list_copy(non_empty_list);
 
            assert(non_empty_list.begin() != list_copy.begin());
            assert(list_copy == non_empty_list);
        }
    }
 
    {
        const SingleLinkedList<int> source_list{ 1, 2, 3, 4 };
 
        SingleLinkedList<int> receiver{ 5, 4, 3, 2, 1 };
        receiver = source_list;
        assert(receiver.begin() != source_list.begin());
        assert(receiver == source_list);
    }

    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
            : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
            : countdown_ptr(other.countdown_ptr)
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                }
                else {
                    --(*countdown_ptr);
                }
            }
        }
        
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        int* countdown_ptr = nullptr;
    };
 
    {
        SingleLinkedList<ThrowOnCopy> src_list;
        src_list.PushFront(ThrowOnCopy{});
        src_list.PushFront(ThrowOnCopy{});
        auto thrower = src_list.begin();
        src_list.PushFront(ThrowOnCopy{});
 
        int copy_counter = 0; 
        thrower->countdown_ptr = &copy_counter;
 
        SingleLinkedList<ThrowOnCopy> dst_list;
        dst_list.PushFront(ThrowOnCopy{});
        int dst_counter = 10;
        dst_list.begin()->countdown_ptr = &dst_counter;
        dst_list.PushFront(ThrowOnCopy{});
 
        try {
            dst_list = src_list;
            assert(false);
        }
        catch (const std::bad_alloc&) {
            assert(dst_list.GetSize() == 2);
            auto it = dst_list.begin();
            assert(it != dst_list.end());
            assert(it->countdown_ptr == nullptr);
            ++it;
            assert(it != dst_list.end());
            assert(it->countdown_ptr == &dst_counter);
            assert(dst_counter == 10);
        }
        catch (...) {
            assert(false);
        }
    }
}
 
int main() {
    Test3();
}
