#ifndef STRING_HPP
#define STRING_HPP

#include "stddef.h"
#include "limits.h"

class String {
public:
    static constexpr size_t npos = -1;

    String();
    String(const char* s);
    String(const char* s, size_t size);

    String(const String& other);
    String& operator=(const String& other);

    String(String&& other);
    String& operator=(String&& other);

    ~String();

    char* data();
    const char* data() const;

    char& operator[](size_t i);
    const char& operator[](size_t i) const;

    char& front();
    const char& front() const;
    char& back();
    const char& back() const;

    char* begin();
    const char* begin() const;
    char* end();
    const char* end() const;

    size_t length() const;
    size_t capacity() const;
    bool empty() const;

    String& operator+=(const char* rhs);
    String& operator+=(const String& rhs);
    String& operator+=(char c);

    friend String operator+(const String& lhs, const String& rhs);
    friend String operator+(const String& lhs, const char* rhs);
    friend String operator+(const char* lhs, const String& rhs);
    friend String operator+(const String& lhs, char rhs);
    friend String operator+(char lhs, const String& rhs);

    int compare(const char* rhs, size_t len) const;
    int compare(const char* rhs) const;
    int compare(const String& rhs, size_t len) const;
    int compare(const String& rhs) const;

    bool operator==(const String& rhs) const;
    bool operator!=(const String& rhs) const;
    bool operator< (const String& rhs) const;
    bool operator<=(const String& rhs) const;
    bool operator> (const String& rhs) const;
    bool operator>=(const String& rhs) const;

    friend bool operator==(const String& lhs, const char* rhs);
    friend bool operator!=(const String& lhs, const char* rhs);
    friend bool operator< (const String& lhs, const char* rhs);
    friend bool operator<=(const String& lhs, const char* rhs);
    friend bool operator> (const String& lhs, const char* rhs);
    friend bool operator>=(const String& lhs, const char* rhs);

    friend bool operator==(const char* lhs, const String& rhs);
    friend bool operator!=(const char* lhs, const String& rhs);
    friend bool operator< (const char* lhs, const String& rhs);
    friend bool operator<=(const char* lhs, const String& rhs);
    friend bool operator> (const char* lhs, const String& rhs);
    friend bool operator>=(const char* lhs, const String& rhs);

    void swap(String& other);
    void reserve(size_t newCapacity);
    void clear();

    size_t find(char c) const;
    size_t rfind(char c) const;

    static size_t length(const char* str);
    static int compare(const char* s1, const char* s2, size_t length);
    static int compare(const char* s1, const char* s2);

private:
    void concat(const char* s, size_t len);
    static String concat(const char* s1, size_t len1, const char* s2, size_t len2);

    // reserve last bit of size to store 'large bit' (0 for SSO, 1 for heap data)
    union {
        struct {
            size_t size : sizeof(size_t) * CHAR_BIT - 1;
            bool large  : 1;
        };

        size_t size_; // the full size value including the large bit
    };

    union {
        struct {
            char* data;
            size_t capacity;
        } large;

        struct {
            char data[sizeof large];
        } small;
    } storage;

    static_assert(sizeof storage.large == sizeof storage.small, "Different sizes for small/large storages");
    static constexpr auto SMALL_CAPACITY = sizeof storage;
};

inline String operator""_s(const char* str, size_t len) {
    return String(str, len);
}

#endif // STRING_HPP
