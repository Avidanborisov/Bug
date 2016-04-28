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

}


#endif // CXXABI_HPP
