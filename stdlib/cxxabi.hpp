#ifndef CXXABI_HPP
#define CXXABI_HPP

#define ATEXIT_MAX_FUNCS 32

extern "C" {

typedef int uarch_t;

struct atexit_func_entry_t {
    void (*destructor_func)(void *);
    void *obj_ptr;
    void *dso_handle;
};

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso);
void __cxa_finalize(void *f);

__extension__ typedef int __guard __attribute__((mode(__DI__)));

int __cxa_guard_acquire (__guard *);
void __cxa_guard_release (__guard *);
void __cxa_guard_abort (__guard *);

}


#endif // CXXABI_HPP
