//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_TRADE_H
#define EMASTRATEGY_TRADE_H

#include <ostream>
#include "point.h"

namespace trading {
    class position {
        point entry_{};
        point exit_{};

        point validate_exit(const point& exit)
        {
            if (entry_.get_created()>=exit.get_created())
                throw std::invalid_argument("Exit time has higher than entry time");

            return exit;
        }

    protected:
        position() = default;

    public:
        const point& get_entry() const
        {
            return entry_;
        }
        const point& get_exit() const
        {
            return exit_;
        }

        void set_entry(const point& entry)
        {
            entry_ = entry;
        }
        void set_exit(const point& exit)
        {
            validate_exit(exit);
            exit_ = exit;
        }

        virtual ~position() = default;
    };

    class long_position : public position {
    public:
        long_position()
                :position() { }

        ~long_position() override = default;
    };

    class short_position : public position {
    public:
        short_position()
                :position() { }

        ~short_position() override = default;
    };
}

#endif //EMASTRATEGY_TRADE_H
