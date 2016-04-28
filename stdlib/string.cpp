#include "string.hpp"
#include "utility.hpp"
#include "memops.hpp"
#include "new.hpp"

String::String() : size_(0), storage() { }

String::String(const char* s) : String(s, length(s)) { }

String::String(const char* s, size_t size) : size(size), large(size >= SMALL_CAPACITY) {
    if (large) {
        storage.large.data = new char[size + 1];
        storage.large.capacity = size + 1;
    }

    memmove(data(), s, size);
    data()[size] = '\0';
}

String::String(const String& other) : String(other.data(), other.size) { }

String& String::operator=(const String& other) {
    if (this != &other) {
        size = other.size;

        if (capacity() < size) {
            if (!large) {
                storage.large.data = new char[size + 1];
                large = true;
            } else {
                delete[] storage.large.data;
                storage.large.data = new char[size + 1];
            }

            storage.large.capacity = size + 1;
        }

        memmove(data(), other.data(), size + 1);
    }

    return *this;
}

String::String(String&& other) : String() {
    swap(other);
}

String& String::operator=(String&& other) {
    this->~String();
    new (this) String();
    swap(other);
    return *this;
}

String::~String() {
    if (large)
        delete[] storage.large.data;
}

char* String::data() {
    return large ? storage.large.data : storage.small.data;
}

const char* String::data() const {
    return large ? storage.large.data : storage.small.data;
}

char& String::operator[](size_t i) {
    return data()[i];
}

const char& String::operator[](size_t i) const {
    return data()[i];
}

char& String::front() {
    return data()[0];
}

const char& String::front() const {
    return data()[0];
}

char& String::back() {
    return data()[size - 1];
}

const char& String::back() const {
    return data()[size - 1];
}

char* String::begin() {
    return data();
}

const char* String::begin() const {
    return data();
}

char* String::end() {
    return data() + size;
}

const char* String::end() const {
    return data() + size;
}

size_t String::length() const {
    return size;
}

size_t String::capacity() const {
    return large ? storage.large.capacity : SMALL_CAPACITY;
}

bool String::empty() const {
    return size == 0;
}

String& String::operator+=(const char* rhs) {
    concat(rhs, length(rhs));
    return *this;
}

String& String::operator+=(const String& rhs) {
    concat(rhs.data(), rhs.size);
    return *this;
}

String& String::operator+=(char c) {
    concat(&c, 1);
    return *this;
}

String operator+(const String& lhs, const String& rhs) {
    return String::concat(lhs.data(), lhs.size, rhs.data(), rhs.size);
}

String operator+(const String& lhs, const char* rhs) {
    return String::concat(lhs.data(), lhs.size, rhs, String::length(rhs));
}

String operator+(const char* lhs, const String& rhs) {
    return String::concat(lhs, String::length(lhs), rhs.data(), rhs.size);
}

String operator+(const String& lhs, char rhs) {
    return String::concat(lhs.data(), lhs.size, &rhs, 1);
}

String operator+(char lhs, const String& rhs) {
    return String::concat(&lhs, 1, rhs.data(), rhs.size);
}

int String::compare(const char* rhs, size_t len) const {
    return compare(data(), rhs, Util::min(size, len));
}

int String::compare(const char* rhs) const {
    return compare(data(), rhs);
}

int String::compare(const String& rhs, size_t len) const {
    return compare(rhs.data(), len);
}

int String::compare(const String& rhs) const {
    return compare(rhs.data(), rhs.size);
}

bool String::operator==(const String& rhs) const {
    return compare(rhs) == 0;
}

bool String::operator!=(const String& rhs) const {
    return compare(rhs) != 0;
}

bool String::operator<(const String& rhs) const {
    return compare(rhs) < 0;
}

bool String::operator<=(const String& rhs) const {
    return compare(rhs) <= 0;
}

bool String::operator>(const String& rhs) const {
    return compare(rhs) > 0;
}

bool String::operator>=(const String& rhs) const {
    return compare(rhs) >= 0;
}

bool operator==(const String& lhs, const char* rhs) {
    return lhs.compare(rhs) == 0;
}

bool operator!=(const String& lhs, const char* rhs) {
    return lhs.compare(rhs) != 0;
}

bool operator<(const String& lhs, const char* rhs) {
    return lhs.compare(rhs) < 0;
}

bool operator<=(const String& lhs, const char* rhs) {
    return lhs.compare(rhs) <= 0;
}

bool operator>(const String& lhs, const char* rhs) {
    return lhs.compare(rhs) > 0;
}

bool operator>=(const String& lhs, const char* rhs) {
    return lhs.compare(rhs) >= 0;
}

bool operator==(const char* lhs, const String& rhs) {
    return rhs == lhs;
}

bool operator!=(const char* lhs, const String& rhs) {
    return rhs != lhs;
}

bool operator<(const char* lhs, const String& rhs) {
    return rhs > lhs;
}

bool operator<=(const char* lhs, const String& rhs) {
    return rhs >= lhs;
}

bool operator>(const char* lhs, const String& rhs) {
    return rhs < lhs;
}

bool operator>=(const char* lhs, const String& rhs) {
    return rhs <= lhs;
}

void String::swap(String& other) {
    Util::swap(size_, other.size_);
    Util::swap(storage, other.storage);
}

void String::reserve(size_t newCapacity) {
    if (newCapacity <= capacity())
        return;

    newCapacity = Util::max(newCapacity, capacity() * 2);
    auto newData = new char[newCapacity];

    memmove(newData, data(), size + 1);

    if (!large) {
        storage.large.data = newData;
        large = true;
    } else {
        delete[] storage.large.data;
        storage.large.data = newData;
    }

    storage.large.capacity = newCapacity;
}

void String::clear() {
    size = 0;
    data()[0] = '\0';
}

size_t String::find(char c) const {
    for (size_t i = 0; i < size; ++i)
        if (data()[i] == c)
            return i;

    return npos;
}

size_t String::rfind(char c) const {
    for (size_t i = size; i-- > 0;)
        if (data()[i] == c)
            return i;

    return npos;
}

void String::concat(const char* s, size_t len) {
    reserve(size + len + 1);

    memmove(data() + size, s, len);
    size += len;

    data()[size] = '\0';
}

String String::concat(const char* s1, size_t len1, const char* s2, size_t len2) {
    String result;
    result.reserve(len1 + len2 + 1);

    memmove(result.data(), s1, len1);
    memmove(result.data() + len1, s2, len2);
    result.size += len1 + len2;

    result.data()[result.size] = '\0';
    return result;
}

size_t String::length(const char* str) {
    size_t count = 0;

    while (str[count] != '\0')
        ++count;

    return count;
}

int String::compare(const char* s1, const char* s2, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (s1[i] < s2[i])
            return -1;
        if (s1[i] > s2[i])
            return 1;
    }

    return 0;
}

int String::compare(const char* s1, const char* s2) {
    return compare(s1, s2, Util::min(length(s1), length(s2)));
}
