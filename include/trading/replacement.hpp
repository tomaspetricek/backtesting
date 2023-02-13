//
// Created by Tomáš Petříček on 13.02.2023.
//

#ifndef BACKTESTING_REPLACEMENT_HPP
#define BACKTESTING_REPLACEMENT_HPP

#include <vector>

namespace trading {
    class en_block_replacement {

        template<class Individual>
        std::vector<Individual> operator()(std::vector<Individual>&&, std::vector<Individual>&& children)
        {
            return children;
        }
    };
}

#endif //BACKTESTING_REPLACEMENT_HPP