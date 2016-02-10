#ifndef SORT_HPP
#define SORT_HPP

#include <stddef.h>

namespace util {
    template<class T>
    struct remove_reference {
        typedef T type;
    };

    template<class T>
    struct remove_reference<T&>{
        typedef T type;
    };

    template<class T>
    struct remove_reference<T&&> {
        typedef T type;
    };

    template<class T>
    constexpr typename remove_reference<T>::type&& move(T&& t) {
        return static_cast<typename remove_reference<T>::type&&>(t);
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
}

#endif // SORT_HPP
