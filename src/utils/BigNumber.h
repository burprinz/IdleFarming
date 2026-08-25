#pragma once
//
// Created by luisb on 21.06.2026.
//

#include <complex>
#include <format>

class BigNumber {
public:

    double base;
    int exponent;

    BigNumber() {
        this->base = 0.0;
        normalize();
    }

    BigNumber(const BigNumber& b) {
        this->base = b.base;
        this->exponent = b.exponent;
        normalize();
    }

    BigNumber(double value) {
        this->base = value;
        this->exponent = 0;
        normalize();
    }

    BigNumber(double base, int exponent) {
        this->base = base;
        this->exponent = exponent;
        normalize();
    }

    BigNumber& operator=(const BigNumber& b) {
        this->base = b.base;
        this->exponent = b.exponent;
        normalize();
        return *this;
    }

    BigNumber& operator+=(const BigNumber& b) {
        if (this->base == 0) {
            this->base = b.base;
            this->exponent = b.exponent;
            normalize();
            return *this;
        }
        if (b.base == 0) {
            return *this;
        }

        if (this->exponent > b.exponent) {
            int d = this->exponent - b.exponent;
            this->base = this->base + b.base / std::pow(10, d);
        } else {
            int d = b.exponent - this->exponent;
            this->base = b.base + this->base / std::pow(10, d);
            this->exponent = b.exponent;
        }
        this->normalize();
        return *this;
    }



    std::string toString() {
        if (exponent < 4) {
            double val = base * pow(10, exponent);
            return std::format("{:.0f}", val);
        }
        return std::format("{:.2f}e{}", base, exponent);
    }

    void normalize() {
        if (base == 0.0) {
            exponent = 0;
            return;
        }

        bool negative = base < 0.0;
        double absBase = std::abs(base);

        int shift = static_cast<int>(std::floor(std::log10(absBase)));

        base = absBase / std::pow(10.0, shift);
        exponent += shift;

        if (base >= 10.0) {
            base /= 10.0;
            exponent += 1;
        } else if (base < 1.0) {
            base *= 10.0;
            exponent -= 1;
        }

        if (negative) {
            base = -base;
        }
    }

    float percentageOf(const BigNumber& large) const {
        if (large.base == 0.0) {
            return 0.0f;
        }

        float baseRatio = static_cast<float>(this->base / large.base);
        int expDiff = this->exponent - large.exponent;
        return baseRatio * std::pow(10.0, expDiff);
    }

private:



};

static double EPSILON = 1e-9;


inline BigNumber operator+(const BigNumber& a, const BigNumber& b) {
    if (a.base == 0) {
        return BigNumber(b);
    }
    if (b.base == 0) {
        return BigNumber(a);
    }

    BigNumber result;
    if (a.exponent > b.exponent) {
        int d = a.exponent - b.exponent;
        result.base = a.base + b.base / std::pow(10, d);
        result.exponent = a.exponent;
    } else {
        int d = b.exponent - a.exponent;
        result.base = b.base + a.base / std::pow(10, d);
        result.exponent = b.exponent;
    }
    result.normalize();
    return result;
}

inline BigNumber operator-(const BigNumber& a, const BigNumber& b) {
    if (b.base == 0) {
        return BigNumber(a);
    }
    if (a.base == 0) {
        return BigNumber(-b.base, b.exponent);
    }

    BigNumber result;
    if (a.exponent >= b.exponent) {
        int d = a.exponent - b.exponent;
        result.base = a.base - b.base / std::pow(10, d);
        result.exponent = a.exponent;
    } else {
        int d = b.exponent - a.exponent;
        result.base = a.base / std::pow(10, d) - b.base;
        result.exponent = b.exponent;
    }
    result.normalize();
    return result;
}

inline BigNumber operator/(const BigNumber& a, const BigNumber& b) {
    BigNumber result;
    result.base = a.base / b.base;
    result.exponent = a.exponent - b.exponent;
    result.normalize();
    return result;
}

inline BigNumber operator*(const BigNumber& a, const BigNumber& b) {
    BigNumber result;
    result.base = a.base * b.base;
    result.exponent = a.exponent + b.exponent;
    result.normalize();
    return result;
}

inline bool operator==(const BigNumber& a, const BigNumber& b) {
    return (a.exponent == b.exponent) && (std::abs(a.base - b.base) < EPSILON);
}

inline bool operator<(const BigNumber& a, const BigNumber& b) {
    if (a.exponent != b.exponent) {
        return a.exponent < b.exponent;
    }
    return a.base < b.base;
}

inline bool operator>(const BigNumber& a, const BigNumber& b) {
    return b < a;
}

inline bool operator<=(const BigNumber& a, const BigNumber& b) {
    return !(b < a);
}

inline bool operator>=(const BigNumber& a, const BigNumber& b) {
    return !(a < b);
}

inline bool operator!=(const BigNumber& a, const BigNumber& b) {
    return !(a==b);
}
