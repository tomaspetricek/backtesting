//
// Created by Tomáš Petříček on 01.11.2022.
//

#ifndef BACKTESTING_TYPES_HPP
#define BACKTESTING_TYPES_HPP

#include <strong_type.hpp>
//#include <boost/rational.hpp>

namespace trading {
    using price_t = strong::type<double, struct price_tag, strong::equality, strong::ordered, strong::arithmetic, strong::ostreamable>;

    using percent_t = strong::type<double, struct percent_tag, strong::ordered, strong::regular, strong::arithmetic, strong::ostreamable>;

    using amount_t = strong::type<double, struct amount_tag, strong::default_constructible, strong::equality, strong::ordered, strong::arithmetic, strong::ostreamable>;

    using fraction_t = strong::type<double, struct fraction_tag, strong::ordered, strong::regular, strong::arithmetic, strong::ostreamable>;
    // using fraction_t = boost::rational<std::size_t>;

    typedef std::size_t index_t;
}

#endif //BACKTESTING_TYPES_HPP
