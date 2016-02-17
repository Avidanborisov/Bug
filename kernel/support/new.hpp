#ifndef NEW_HPP
#define NEW_HPP

// Placement new

inline void* operator new(size_t, void* p) {
    return p;
}

inline void* operator new[](size_t, void* p) {
    return p;
}

#endif // NEW_HPP
