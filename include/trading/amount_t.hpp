//
// Created by Tomáš Petříček on 04.08.2022.
//

#ifndef EMASTRATEGY_AMOUNT_T_HPP
#define EMASTRATEGY_AMOUNT_T_HPP

#include <strong_type.hpp>

namespace trading {
    using amount_t = strong::type<double, struct amount_tag, strong::equality, strong::ordered, strong::arithmetic>;
}

#endif //EMASTRATEGY_AMOUNT_T_HPP
