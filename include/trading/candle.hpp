//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef EMASTRATEGY_CANDLE_HPP
#define EMASTRATEGY_CANDLE_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

#include <trading/price_t.hpp>
#include <trading/pack.hpp>
#include <trading/currency.hpp>

namespace trading {
    class candle {
        boost::posix_time::ptime opened_;
        price_t open_;
        price_t high_;
        price_t low_;
        price_t close_;

    public:
        explicit candle(const boost::posix_time::ptime& opened, double open, double high,
                double low, double close)
                :opened_(opened), open_(price_t{open}), high_(price_t{high}),
                 low_(price_t{low}), close_(price_t{close})
        {
            if (high<=low)
                throw std::invalid_argument("High price has to be lower or equal to low price");
        }

        // mean high and low
        static price_t hl2(const candle& candle)
        {
            return price_t{mean(value_of(candle.high_), value_of(candle.low_))};
        }

        // mean open, high, low, close
        static price_t hlc3(const candle& candle)
        {
            return price_t{mean(value_of(candle.high_), value_of(candle.low_))};
        }

        // mean open, high, low, close
        static price_t ohlc4(const candle& candle)
        {
            return price_t{mean(value_of(candle.open_), value_of(candle.high_), value_of(candle.low_),
                    value_of(candle.close_))};
        }

        boost::posix_time::ptime opened() const
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

#endif //EMASTRATEGY_CANDLE_HPP
