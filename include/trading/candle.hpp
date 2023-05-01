//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef BACKTESTING_CANDLE_HPP
#define BACKTESTING_CANDLE_HPP

#include <boost/date_time/posix_time/posix_time.hpp>
#include <trading/types.hpp>
#include <trading/pack.hpp>
#include <ostream>

namespace trading {
    class candle {
        std::time_t opened_{0};
        static constexpr price_t min_price{0};
        price_t open_{min_price}, high_{min_price}, low_{min_price}, close_{min_price};

    public:
        candle() = default;

        constexpr candle(std::time_t opened, price_t open, price_t high, price_t low, price_t close)
                :opened_(opened), open_(open), high_(high), low_(low), close_(close)
        {
            if (high<low)
                throw std::invalid_argument("High price has to be higher or equal to low price");
            if (open<low || open>high)
                throw std::invalid_argument("Open price has to be between or equal to low and high price");
            if (close<low || close>high)
                throw std::invalid_argument("Close price has to be between or equal to low and high price");
        }

        // mean high and low
        struct hl2 {
            price_t operator()(const candle& candle) const
            {
                return mean<price_t>(candle.high_, candle.low_);
            }

            static constexpr std::string_view name = "hl2";
        };

        // mean open, high, low, close
        struct hlc3 {
            price_t operator()(const candle& candle) const
            {
                return mean<price_t>(candle.high_, candle.low_, candle.close_);
            }

            static constexpr std::string_view name = "hlc3";
        };

        // mean open, high, low, close
        struct ohlc4 {
            price_t operator()(const candle& candle) const
            {
                return mean<price_t>(candle.open_, candle.high_, candle.low_, candle.close_);
            }

            static constexpr std::string_view name = "ohlc4";
        };

        bool risen() const
        {
            return close_<open_;
        }

        bool operator==(const candle& rhs) const
        {
            return opened_==rhs.opened_ &&
                    open_==rhs.open_ &&
                    high_==rhs.high_ &&
                    low_==rhs.low_ &&
                    close_==rhs.close_;
        }

        bool operator!=(const candle& rhs) const
        {
            return !(rhs==*this);
        }

        friend std::ostream& operator<<(std::ostream& os, const candle& candle)
        {
            os << "opened: " << candle.opened_ << " open: " << candle.open_ << " high: " << candle.high_ << " low: "
               << candle.low_ << " close: " << candle.close_;
            return os;
        }

        std::time_t opened() const
        {
            return opened_;
        }

        price_t open() const
        {
            return open_;
        }

        price_t high() const
        {
            return high_;
        }

        price_t low() const
        {
            return low_;
        }

        price_t close() const
        {
            return close_;
        }
    };
}

#endif //BACKTESTING_CANDLE_HPP
