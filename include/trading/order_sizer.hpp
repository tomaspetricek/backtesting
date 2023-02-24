//
// Created by Tomáš Petříček on 14.09.2022.
//

#ifndef BACKTESTING_ORDER_SIZER_HPP
#define BACKTESTING_ORDER_SIZER_HPP

#include <array>
#include <trading/types.hpp>
#include <trading/utils.hpp>
#include <trading/convert.hpp>
#include <fmt/format.h>

namespace trading {
    template<std::size_t n_sizes>
    class order_sizer {
        std::array<double, n_sizes> cum_sizes_;
        index_t curr_idx{0};

        auto calc_cumulative_sizes(const std::array<fraction_t, n_sizes>& sizes)
        {
            std::array<double, n_sizes> cum_sizes;
            cum_sizes[0] = fraction_cast<double>(sizes[0]);
            double sum_prev{cum_sizes[0]};

            for (index_t i{1}; i<n_sizes-1; i++) {
                auto size = fraction_cast<double>(sizes[i]);
                cum_sizes[i] = size/(1.0-sum_prev);
                sum_prev += size;
            }

            cum_sizes[n_sizes-1] = 1.0;
            return cum_sizes;
        }

        template<std::size_t size>
        std::array<fraction_t, size> validate_sizes(const std::array<fraction_t, size>& sizes)
        {
            auto denom = sizes[0].denominator();
            fraction_t sum{0, denom};

            for (index_t i{0}; i<size; i++) {
                if (sizes[i]<=fraction_t{0, denom} || sizes[i]>fraction_t{denom, denom})
                    throw std::invalid_argument("Size has to be in interval (0, 1]");
                sum += sizes[i];
            }

            if (fraction_t{denom, denom}!=sum)
                throw std::invalid_argument("Sizes have to sum up to 1.0");

            return sizes;
        }

    public:
        void reset()
        {
            curr_idx = 0;
        }

        amount_t operator()(amount_t rest)
        {
            assert(curr_idx<n_sizes);
            return cum_sizes_[curr_idx++]*rest;
        }

        explicit order_sizer(const std::array<fraction_t, n_sizes>& fracs)
                :cum_sizes_(calc_cumulative_sizes(validate_sizes(fracs))) { }

        order_sizer() = default;
    };
}

#endif //BACKTESTING_ORDER_SIZER_HPP
