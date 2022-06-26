//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_TRADE_H
#define EMASTRATEGY_TRADE_H

#include <ostream>
#include "point.h"

namespace trading {
    class position {
        side side_;
        point entry_;
        point exit_;

    public:
        position() = default;

        const side& get_side() const
        {
            return side_;
        }
        const point& get_entry() const
        {
            return entry_;
        }
        const point& get_exit() const
        {
            return exit_;
        }

        void set_side(const side& side)
        {
            side_ = side;
        }

        void set_entry(const point& entry)
        {
            entry_ = entry;
        }
        void set_exit(const point& exit)
        {
            exit_ = exit;
        }
    };
}

#endif //EMASTRATEGY_TRADE_H
