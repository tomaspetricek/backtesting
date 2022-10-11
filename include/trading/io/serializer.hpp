//
// Created by Tomáš Petříček on 10.10.2022.
//

#ifndef BACKTESTING_SERIALIZER_HPP
#define BACKTESTING_SERIALIZER_HPP

#include <string>

class serializer {
    template<std::size_t n>
    void operator()(const std::string (& data)[n])
    {

    }
};

#endif //BACKTESTING_SERIALIZER_HPP
