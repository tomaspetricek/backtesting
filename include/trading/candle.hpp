//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef EMASTRATEGY_CANDLE_HPP
#define EMASTRATEGY_CANDLE_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

#include <trading/price.hpp>
#include <trading/pack.hpp>
#include <trading/currency.hpp>

namespace trading {
    class candle {
        boost::posix_time::ptime opened_;
        price open_;
        price high_;
        price low_;
        price close_;

    public:
        explicit candle(const boost::posix_time::ptime& opened, double open, double high,
                double low, double close)
                :opened_(opened), open_(price{open}), high_(price{high}),
                 low_(price{low}), close_(price{close})
        {
            if (high<=low)
                throw std::invalid_argument("High price has to be lower or equal to low price");
        }

        // mean high and low
        static price hl2(const candle& candle)
        {
            return price{mean(candle.high_, candle.low_)};
        }

        // mean open, high, low, close
        static price hlc3(const candle& candle)
        {
            return price{mean(candle.high_, candle.low_)};
        }

        // mean open, high, low, close
        static price ohlc4(const candle& candle)
        {
            return price{mean(candle.open_, candle.high_, candle.low_, candle.close_)};
        }

        boost::posix_time::ptime opened() const
        {
            return opened_;
        }

        price open() const
        {
            return open_;
        }

        price high() const
        {
            return high_;
        }

        price low() const
        {
            return low_;
        }

        price close() const
        {
            return close_;
        }
    };
}

#endif //EMASTRATEGY_CANDLE_HPP
