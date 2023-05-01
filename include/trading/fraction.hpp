//
// Created by Tomáš Petříček on 08.02.2023.
//

#ifndef BACKTESTING_FRACTION_HPP
#define BACKTESTING_FRACTION_HPP

#include <boost/functional/hash.hpp>

// inspired by: https://www.boost.org/doc/libs/1_75_0/libs/rational/rational.html
#include <ostream>
namespace trading {
    template<class IntType>
    class fraction {
        IntType num_{0};
        IntType den_{1};

    public:
        constexpr fraction(IntType num, IntType den)
                :num_(num), den_(den) { }

        constexpr fraction(IntType num)
                :num_(num) { }

        constexpr fraction() = default;

        IntType denominator() const
        {
            return den_;
        }

        IntType numerator() const
        {
            return num_;
        }

        bool operator==(const fraction& rhs) const
        {
            return num_==rhs.num_;
        }

        bool operator!=(const fraction& rhs) const
        {
            return !(rhs==*this);
        }

        bool operator<(const fraction& rhs) const
        {
            return num_<rhs.num_;
        }

        bool operator>(const fraction& rhs) const
        {
            return rhs<*this;
        }

        bool operator<=(const fraction& rhs) const
        {
            return !(rhs<*this);
        }

        bool operator>=(const fraction& rhs) const
        {
            return !(*this<rhs);
        }

        constexpr fraction operator+(const fraction& rhs) const
        {
            return fraction{num_+rhs.num_, den_};
        }

        constexpr fraction operator-(const fraction& rhs) const
        {
            return fraction{num_-rhs.num_, den_};
        }

        constexpr fraction& operator-=(const fraction& rhs)
        {
            num_ -= rhs.num_;
            return *this;
        }

        constexpr fraction& operator+=(const fraction& f)
        {
            num_ += f.num_;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const fraction& f)
        {
            os << f.num_ << "/" << f.den_;
            return os;
        }

        friend std::size_t hash_value(const fraction& f)
        {
            std::size_t seed{0};
            boost::hash_combine(seed, f.num_);
            boost::hash_combine(seed, f.den_);
            return seed;
        }
    };

    template <typename T, typename IntType>
    inline T fraction_cast(const fraction<IntType>& src)
    {
        return static_cast<T>(src.numerator())/static_cast<T>(src.denominator());
    }
}

#endif //BACKTESTING_FRACTION_HPP
