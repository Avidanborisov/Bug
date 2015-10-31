#ifndef MATH_HPP
#define MATH_HPP

class Math {
public:
    template<class Unsigned>
    constexpr static Unsigned divRoundUp(Unsigned dividend, Unsigned divisor) {
        return (dividend - 1) / divisor + 1;
    }

    template<class Unsigned>
    constexpr static Unsigned roundDown(Unsigned num, Unsigned multiple) {
        return num / multiple * multiple;
    }

    template<class Unsigned>
    constexpr static Unsigned roundUp(Unsigned num, Unsigned multiple) {
        return roundDown(num + multiple - 1, multiple);
    }
};

#endif // MATH_HPP
