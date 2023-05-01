//
// Created by Tomáš Petříček on 07.07.2022.
//

#ifndef BACKTESTING_MA_HPP
#define BACKTESTING_MA_HPP

namespace trading {
    class ma {
        static std::size_t validate_period(size_t period)
        {
            if (period<min_period)
                throw std::invalid_argument("Period has to be greater than 1");

            return period;
        }

    protected:
        constexpr static size_t min_period{1};
        size_t period_{min_period};

    public:
        explicit ma(size_t period = min_period)
                :period_(validate_period(period)) { }

        std::size_t period() const
        {
            return period_;
        }
    };
}

#endif //BACKTESTING_MA_HPP
