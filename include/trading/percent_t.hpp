//
// Created by Tomáš Petříček on 01.08.2022.
//

#ifndef EMASTRATEGY_PERCENT_T_HPP
#define EMASTRATEGY_PERCENT_T_HPP

#include <strong_type/strong_type.hpp>

namespace trading {
    using percent_t = strong::type<double, struct percent_tag, strong::ordered, strong::regular>;
}

#endif //EMASTRATEGY_PERCENT_T_HPP
