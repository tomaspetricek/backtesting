//
// Created by Tomáš Petříček on 05.04.2023.
//

#ifndef BACKTESTING_INT_RANGE_HPP
#define BACKTESTING_INT_RANGE_HPP

#include <trading/validate.hpp>

namespace trading {
    class int_range {
    protected:
        int from_, to_, step_, min_, max_;
        std::size_t n_vals_;

        int_range(int from, int to, int step)
                :from_{from}, to_{to}, step_(step)
        {
            validate_int_range(from, to, step);
            int positive_step = std::abs(this->step_);
            n_vals_ = (std::abs(to-from)+positive_step)/positive_step;
            std::tie(min_, max_) = make_tuple(std::minmax(from, to));
        }

    public:
        using value_type = int;

        int step() const
        {
            return step_;
        }

        int from() const
        {
            return from_;
        }

        int to() const
        {
            return to_;
        }

        int min() const
        {
            return min_;
        }

        int max() const
        {
            return max_;
        }

        std::size_t value_count() const
        {
            return n_vals_;
        }
    };
}

#endif //BACKTESTING_INT_RANGE_HPP
