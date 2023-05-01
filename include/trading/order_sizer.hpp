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
#include <trading/validate.hpp>

namespace trading {
    template<std::size_t n_sizes>
    class order_sizer {
        std::array<double, n_sizes> cum_sizes_;
        index_t curr_idx{0};

        auto compute_cumulative_sizes(const std::array<fraction_t, n_sizes>& sizes)
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
                :cum_sizes_(compute_cumulative_sizes(validate_sizes(fracs))) { }

        order_sizer() = default;
    };
}

#endif //BACKTESTING_ORDER_SIZER_HPP
