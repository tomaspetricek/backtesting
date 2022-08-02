//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_POSITION_HPP
#define EMASTRATEGY_POSITION_HPP

#include <ostream>

#include <boost/date_time/posix_time/ptime.hpp>

#include <trading/currency.hpp>
#include <trading/price.hpp>
#include <trading/currency.hpp>
#include <trading/amount.hpp>

namespace trading {
    template<currency::crypto buy, currency::crypto sell, currency::crypto price_curr>
    class position {
    protected:
        amount<buy> bought_{0};
        amount<sell> sold_{0};
        price<price_curr> price_;
        boost::posix_time::ptime created_;

    public:
        position() = default;

        explicit position(const amount<sell>& sold, const price<price_curr>& price, const boost::posix_time::ptime& created)
                :sold_(sold), price_(price), created_(created) {}

        const price<price_curr>& price() const
        {
            return price_;
        }
        
        boost::posix_time::ptime created() const
        {
            return created_;
        }

        const amount<buy>& bought() const
        {
            return bought_;
        }

        const amount<sell>& sold() const
        {
            return sold_;
        }
    };
}

#endif //EMASTRATEGY_POSITION_HPP
