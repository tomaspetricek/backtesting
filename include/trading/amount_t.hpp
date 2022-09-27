//
// Created by Tomáš Petříček on 04.08.2022.
//

#ifndef BACKTESTING_AMOUNT_T_HPP
#define BACKTESTING_AMOUNT_T_HPP

#include <strong_type.hpp>

namespace trading {
    using amount_t = strong::type<double, struct amount_tag, strong::default_constructible,
            strong::equality, strong::ordered, strong::arithmetic, strong::ostreamable>;
}

#endif //BACKTESTING_AMOUNT_T_HPP
