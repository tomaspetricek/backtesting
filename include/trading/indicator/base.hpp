//
// Created by Tomáš Petříček on 08.08.2022.
//

#ifndef EMASTRATEGY_BASE_HPP
#define EMASTRATEGY_BASE_HPP

namespace trading::indicator {
    class base {
        virtual base& operator()(double sample) = 0;

        virtual explicit operator double() const = 0;
    };
}

#endif //EMASTRATEGY_BASE_HPP
