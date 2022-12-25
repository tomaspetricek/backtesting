//
// Created by Tomáš Petříček on 25.12.2022.
//

#ifndef BACKTESTING_ACTION_HPP
#define BACKTESTING_ACTION_HPP

namespace trading {
    enum class action {
        none = 0,
        opened = 1,
        closed = 2
    };
}

#endif //BACKTESTING_ACTION_HPP
