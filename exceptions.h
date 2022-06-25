//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_EXCEPTIONS_H
#define EMASTRATEGY_EXCEPTIONS_H

#include <exception>

class not_ready : public std::logic_error {
public:
    explicit not_ready(const std::string& msg) : std::logic_error{msg} {}
};

#endif //EMASTRATEGY_EXCEPTIONS_H
