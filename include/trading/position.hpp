//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_POSITION_HPP
#define EMASTRATEGY_POSITION_HPP

#include <ostream>

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/currency.hpp>
#include <trading/price_t.hpp>
#include <trading/currency.hpp>
#include <trading/amount_t.hpp>

namespace trading {
    class position {
    protected:
        amount_t sold_;
        amount_t bought_;
        price_t price_;
        boost::posix_time::ptime created_;

    protected:
        explicit position(amount_t sold, const amount_t bought, const price_t& price,
                const boost::posix_time::ptime& created)
                :sold_(sold), bought_(bought), price_(price), created_(created) { }

    public:
        const price_t& price() const
        {
            return price_;
        }

        boost::posix_time::ptime created() const
        {
            return created_;
        }

        amount_t sold() const
        {
            return sold_;
        }
        amount_t bought() const
        {
            return bought_;
        }
    };
}

#endif //EMASTRATEGY_POSITION_HPP
