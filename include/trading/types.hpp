//
// Created by Tomáš Petříček on 01.11.2022.
//

#ifndef BACKTESTING_TYPES_HPP
#define BACKTESTING_TYPES_HPP

#include <boost/rational.hpp>
#include <trading/fraction.hpp>

namespace trading {
    typedef float price_t, percent_t, amount_t;
    using fraction_t = fraction<std::size_t>;
    typedef std::size_t index_t;
    using percent = struct percent_tag;
    using amount = struct amount_tag;
}

#endif //BACKTESTING_TYPES_HPP
