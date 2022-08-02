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
    template<currency::crypto quote>
    class candle {
        boost::posix_time::ptime opened_;
        price<quote> open_;
        price<quote> high_;
        price<quote> low_;
        price<quote> close_;

    public:
        candle(const boost::posix_time::ptime& opened, const price<quote>& open, const price<quote>& high,
                const price<quote>& low, const price<quote>& close)
                :opened_(opened), open_(open), high_(high), low_(low), close_(close)
        {
            if (high<=low)
                throw std::invalid_argument("High price has to be lower or equal to low price");
        }

        candle(const boost::posix_time::ptime& opened, double open, double high, double low, double close)
                :candle{opened, price<quote>{open}, price<quote>{high}, price<quote>{low}, price<quote>{close}} { }

        // mean high and low
        static price<quote> hl2(const candle& candle)
        {
            return price<quote>{mean(candle.high_, candle.low_)};
        }

        // mean open, high, low, close
        static price<quote> hlc3(const candle& candle)
        {
            return price<quote>{mean(candle.high_, candle.low_, candle.close_)};
        }

        // mean open, high, low, close
        static price<quote> ohlc4(const candle& candle)
        {
            return price<quote>{mean(candle.open_, candle.high_, candle.low_, candle.close_)};
        }

        boost::posix_time::ptime opened() const
        {
            return opened_;
        }

        price<quote> open() const
        {
            return open_;
        }

        price<quote> high() const
        {
            return high_;
        }

        price<quote> low() const
        {
            return low_;
        }

        price<quote> close() const
        {
            return close_;
        }
    };
}

#endif //EMASTRATEGY_CANDLE_HPP
