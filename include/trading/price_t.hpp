//
// Created by Tomáš Petříček on 03.07.2022.
//

#ifndef EMASTRATEGY_PRICE_T_HPP
#define EMASTRATEGY_PRICE_T_HPP

#include <trading/currency.hpp>
#include <trading/exception.hpp>

#include <strong_type/strong_type.hpp>

namespace trading {
    using price_t = strong::type<const double, struct price_tag, strong::equality, strong::ordered, strong::arithmetic>;
}

#endif //EMASTRATEGY_PRICE_T_HPP
