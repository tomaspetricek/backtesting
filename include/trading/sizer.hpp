//
// Created by Tomáš Petříček on 14.09.2022.
//

#ifndef BACKTESTING_SIZER_HPP
#define BACKTESTING_SIZER_HPP

#include <array>
#include <trading/types.hpp>
#include <trading/utils.hpp>

namespace trading {
    template<std::size_t n_fracs>
    class sizer {
        std::array<fraction_t, n_fracs> cum_sizes_;
        index_t curr_idx{0};

        auto calc_cumulative_sizes(const std::array<fraction_t, n_fracs>& sizes)
        {
            std::array<fraction_t, n_fracs> cum_sizes;
            cum_sizes[0] = sizes[0];
            fraction_t sum_prev{sizes[0]};

            for (index_t i{1}; i<n_fracs; i++) {
                cum_sizes[i] = sizes[i]/(fraction_t{1}-sum_prev);
                sum_prev += sizes[i];
            }

            return cum_sizes;
        }

        template<std::size_t size>
        std::array<fraction_t, size> validate_sizes(const std::array<fraction_t, size>& sizes)
        {
            fraction_t sum{0};

            for (index_t i{0}; i<size; i++) {
                if (sizes[i]<=fraction_t{0} || sizes[i]>fraction_t{1})
                    throw std::invalid_argument("Size has to be in interval (0, 1]");
                sum += sizes[i];
            }

            if (fraction_t{1}!=sum)
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
            assert(curr_idx<n_fracs);
            return boost::rational_cast<amount_t>(cum_sizes_[curr_idx++])*rest;
        }

        explicit sizer(const std::array<fraction_t, n_fracs>& fracs)
                :cum_sizes_(calc_cumulative_sizes(validate_sizes(fracs))) { }

        sizer() = default;
    };
}

#endif //BACKTESTING_SIZER_HPP
