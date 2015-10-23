#ifndef SORT_HPP
#define SORT_HPP

#include <stddef.h>

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

#endif // SORT_HPP
