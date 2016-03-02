#ifndef SORT_HPP
#define SORT_HPP

#include <stddef.h>
#include "support/new.hpp"
#include "assert.hpp"

namespace std { // initializer_list required to be part of std namespace for compiler integration
    template<class T>
    class initializer_list {
    public:
        constexpr initializer_list() : data_(nullptr), size_(0) { }

        constexpr size_t size() const {
            return size_;
        }

        constexpr const T* begin() const {
            return data_;
        }

        constexpr const T* end() const {
            return data_ + size_;
        }

    private:
        // Only the compiler is able to call the actual constructor using special {a,b,c,...} syntax
        constexpr initializer_list(const T* data, size_t size) : data_(data), size_(size) { }

        const T* data_;
        size_t size_;
    };

    template<class T>
    constexpr const T* begin(initializer_list<T> l) {
        return l.begin();
    }

    template<class T>
    constexpr const T* end(initializer_list<T> l) {
        return l.end();
    }
}

namespace Util {
    template<class T>
    using Initializer = std::initializer_list<T>;

    template<class T>
    struct RemoveReference {
        typedef T type;
    };

    template<class T>
    struct RemoveReference<T&>{
        typedef T type;
    };

    template<class T>
    struct RemoveReference<T&&> {
        typedef T type;
    };

    template<class T>
    constexpr typename RemoveReference<T>::type&& move(T&& t) {
        return static_cast<typename RemoveReference<T>::type&&>(t);
    }

    template<class T>
    constexpr T&& forward(typename RemoveReference<T>::type& t) {
        return static_cast<T&&>(t);
    }

    template<class T>
    constexpr T&& forward(typename RemoveReference<T>::type&& t) {
        return static_cast<T&&>(t);
    }

    template<class T>
    void swap(T& a, T& b) {
        T c(move(a));
        a = move(b);
        b = move(c);
    }

    template<class T, class Compare>
    void sort(T arr[], size_t n, Compare cmp) {
        // Insertion sort

        for (size_t i = 1; i < n; ++i) {
            size_t j = i;
            T elem = arr[j];

            while (j > 0 && cmp(elem, arr[j - 1])) {
                arr[j] = arr[j - 1];
                --j;
            }

            arr[j] = elem;
        }
    }

    template<class T, size_t N, class Compare>
    void sort(T (&arr)[N], Compare cmp) {
        sort(arr, N, cmp);
    }

    template<class T>
    constexpr const T& min(const T& a, const T& b) {
        return a < b ? a : b;
    }

    template<class T>
    constexpr const T& max(const T& a, const T& b) {
        return a > b ? a : b;
    }

    template<class InputIt, class OutputIt>
    OutputIt copy(OutputIt dest, InputIt begin, InputIt end) {
        while (begin != end) {
            *dest = *begin;
            ++dest;
            ++begin;
        }

        return dest;
    }

    template<class InputIt, class OutputIt>
    OutputIt copyBackwards(OutputIt destEnd, InputIt begin, InputIt end) {
        while (begin != end) {
            *--destEnd = *--end;
        }

        return destEnd;
    }

    template<class ForwardIt, class T>
    void fill(ForwardIt begin, ForwardIt end, const T& value) {
        while (begin != end) {
            *begin = value;
            ++begin;
        }
    }

    template<class T, class... Args>
    void construct(T* p, Args&&... args) {
        new (static_cast<void*>(p)) T(forward<Args>(args)...);
    }

    template<class ForwardIt, class... Args>
    void constructRange(ForwardIt begin, ForwardIt end, Args&&... args) {
        while (begin != end) {
            construct(begin, forward<Args>(args)...);
            ++begin;
        }
    }

    template<class InputIt, class OutputIt>
    OutputIt constructCopy(OutputIt dest, InputIt begin, InputIt end) {
        while (begin != end) {
            construct(dest, *begin);
            ++dest;
            ++begin;
        }

        return dest;
    }

    template<class T>
    void destroy(T* p) {
        p->~T();
    }

    template<class T>
    void destroy(T* begin, T* end) {
        while (begin != end) {
            begin->~T();
            ++begin;
        }
    }

    struct InputIteratorTag { };
    struct OutputIteratorTag { };
    struct ForwardIteratorTag : public InputIteratorTag { };
    struct BidirectionalIteratorTag : public ForwardIteratorTag { };
    struct RandomAccessIteratorTag: public BidirectionalIteratorTag { };

    template<class T>
    class IteratorTraits {
    public:
        using Category       = typename T::Category;
        using ValueType      = typename T::ValueType;
        using Pointer        = typename T::Pointer;
        using Reference      = typename T::Reference;
        using DifferenceType = typename T::DifferenceType;
    };

    template<class T>
    class IteratorTraits<T*> {
    public:
        using Category       = RandomAccessIteratorTag;
        using ValueType      = T;
        using Pointer        = T*;
        using Reference      = T&;
        using DifferenceType = ptrdiff_t;
    };

    template<class T>
    class IteratorTraits<const T*> {
    public:
        using Category       = RandomAccessIteratorTag;
        using ValueType      = T;
        using Pointer        = const T*;
        using Reference      = const T&;
        using DifferenceType = ptrdiff_t;
    };

    namespace Detail {
        template<class T>
        using Diff = typename IteratorTraits<T>::DifferenceType;

        template<class InputIt>
        Diff<InputIt> distance(InputIt begin, InputIt end, InputIteratorTag) {
            Diff<InputIt> dist = 0;

            while (begin != end) {
                ++begin;
                ++dist;
            }

            return dist;
        }

        template<class RandomAccessIt>
        Diff<RandomAccessIt> distance(RandomAccessIt begin, RandomAccessIt end, RandomAccessIteratorTag) {
            return end - begin;
        }

        template<class InputIt, class Distance>
        void advance(InputIt& it, Distance n, InputIteratorTag) {
            assert(n >= 0);
            while (n-- != 0)
                ++it;
        }

        template<class BidirectionalIt, class Distance>
        void advance(BidirectionalIt& it, Distance n, BidirectionalIteratorTag) {
            if (n > 0) {
                while (n-- != 0)
                    ++it;
            } else {
                while (n++ != 0)
                    --it;
            }
        }

        template<class RandomAccessIt, class Distance>
        void advance(RandomAccessIt& it, Distance n, RandomAccessIteratorTag) {
            it += n;
        }
    }

    template<class InputIt>
    auto distance(InputIt begin, InputIt end) {
        using Category = typename IteratorTraits<InputIt>::Category;
        return Detail::distance(begin, end, Category());
    }

    template<class InputIt, class Distance>
    void advance(InputIt& it, Distance n) {
        using Category = typename IteratorTraits<InputIt>::Category;
        return Detail::advance(it, n, Category());
    }

    template<class InputIt>
    InputIt next(InputIt it, typename IteratorTraits<InputIt>::DifferenceType n = 1) {
        advance(it, n);
        return it;
    }

    template<class BidirectionalIt>
    BidirectionalIt prev(BidirectionalIt it, typename IteratorTraits<BidirectionalIt>::DifferenceType n = 1) {
        advance(it, -n);
        return it;
    }

    template<class Iterator>
    class MoveIterator {
    public:
        using Category       = typename IteratorTraits<Iterator>::Category;
        using ValueType      = typename IteratorTraits<Iterator>::ValueType;
        using DifferenceType = typename IteratorTraits<Iterator>::DifferenceType;
        using Pointer        = Iterator;
        using Reference      = ValueType&&;

        MoveIterator() { }
        explicit MoveIterator(Iterator it) : it(it) { }

        template<class U>
        MoveIterator(const MoveIterator<U>& other) : it(other.it) { }

        template<class U>
        MoveIterator& operator=(const MoveIterator<U>& other) {
            it = other.it;
        }

        Iterator base() const {
            return it;
        }

        Reference operator*() const {
            return move(*it);
        }

        Pointer operator->() const {
            return it;
        }

        MoveIterator& operator++() {
            ++it;
            return *this;
        }

        MoveIterator& operator--() {
            --it;
            return *this;
        }

        MoveIterator& operator++(int) {
            MoveIterator temp = *this;
            ++it;
            return temp;
        }

        MoveIterator& operator--(int) {
            MoveIterator temp = *this;
            --it;
            return temp;
        }

        MoveIterator& operator+=(DifferenceType off) const {
            it += off;
            return *this;
        }

        MoveIterator& operator-=(DifferenceType off) const {
            it -= off;
            return *this;
        }

        MoveIterator operator+(DifferenceType off) const {
            return MoveIterator(it + off);
        }

        MoveIterator operator-(DifferenceType off) const {
            return MoveIterator(it - off);
        }

        Reference operator[](DifferenceType index) const {
            return move(it[index]);
        }

    private:
        Iterator it;
    };

    template<class Iterator>
    MoveIterator<Iterator> makeMoveIterator(Iterator it) {
        return MoveIterator<Iterator>(it);
    }

    template<class It>
    bool operator==(const MoveIterator<It>& lhs, const MoveIterator<It>& rhs) {
        return lhs.base() == rhs.base();
    }

    template<class L, class R>
    bool operator==(const MoveIterator<L>& lhs, const MoveIterator<R>& rhs) {
        return lhs.base() == rhs.base();
    }

    template<class It>
    bool operator!=(const MoveIterator<It>& lhs, const MoveIterator<It>& rhs) {
        return !(lhs == rhs);
    }

    template<class L, class R>
    bool operator!=(const MoveIterator<L>& lhs, const MoveIterator<R>& rhs) {
        return !(lhs == rhs);
    }

    template<class It>
    bool operator<(const MoveIterator<It>& lhs, const MoveIterator<It>& rhs) {
        return lhs.base() < rhs.base();
    }

    template<class L, class R>
    bool operator<(const MoveIterator<L>& lhs, const MoveIterator<R>& rhs) {
        return lhs.base() < rhs.base();
    }

    template<class It>
    bool operator<=(const MoveIterator<It>& lhs, const MoveIterator<It>& rhs) {
        return !(rhs < lhs);
    }

    template<class L, class R>
    bool operator<=(const MoveIterator<L>& lhs, const MoveIterator<R>& rhs) {
        return !(rhs < lhs);
    }

    template<class It>
    bool operator>(const MoveIterator<It>& lhs, const MoveIterator<It>& rhs) {
        return rhs < lhs;
    }

    template<class L, class R>
    bool operator>(const MoveIterator<L>& lhs, const MoveIterator<R>& rhs) {
        return rhs < lhs;
    }

    template<class It>
    bool operator>=(const MoveIterator<It>& lhs, const MoveIterator<It>& rhs) {
        return rhs <= lhs;
    }

    template<class L, class R>
    bool operator>=(const MoveIterator<L>& lhs, const MoveIterator<R>& rhs) {
        return rhs <= lhs;
    }

    template<class InputIt, class OutputIt>
    OutputIt move(OutputIt dest, InputIt begin, InputIt end) {
        return copy(dest, makeMoveIterator(begin), makeMoveIterator(end));
    }

    template<class InputIt, class OutputIt>
    OutputIt moveBackwards(OutputIt destEnd, InputIt begin, InputIt end) {
        return copyBackwards(destEnd, makeMoveIterator(begin), makeMoveIterator(end));
    }

    template<class InputIt, class OutputIt>
    OutputIt constructMove(OutputIt dest, InputIt begin, InputIt end) {
        return constructCopy(dest, makeMoveIterator(begin), makeMoveIterator(end));
    }
}

#endif // SORT_HPP
