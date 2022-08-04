//
// Created by Tomáš Petříček on 25.06.2022.
//

#ifndef EMASTRATEGY_EXCEPTION_HPP
#define EMASTRATEGY_EXCEPTION_HPP

#include <exception>

namespace trading {
    class not_ready : public std::logic_error {
    public:
        explicit not_ready(const std::string& msg)
                :std::logic_error{msg} { }

        ~not_ready() noexcept override = default;

        const char* what() const _NOEXCEPT override
        {
            return logic_error::what();
        }
    };

    class bad_formatting : public std::runtime_error {
    public:
        explicit bad_formatting(const std::string& string)
                :runtime_error(string) { }

        ~bad_formatting() noexcept override = default;

        const char* what() const _NOEXCEPT override
        {
            return runtime_error::what();
        }
    };

    // source: https://github.com/mariusbancila/moneycpp/blob/master/include/money.h
    class currency_mismatch_error : public std::runtime_error {
    public:
        explicit currency_mismatch_error()
                :runtime_error("Currencies do not match") { }

        ~currency_mismatch_error() noexcept override = default;

        const char* what() const _NOEXCEPT override
        {
            return runtime_error::what();
        }
    };

    void print_exception(const std::exception& e, int level = 0)
    {
        std::cerr << std::string(level, '\t') << "exception: " << e.what() << '\n';

        try {
            std::rethrow_if_nested(e);
        }
        catch (const std::exception& nested_exception) {
            print_exception(nested_exception, level+1);
        }
        catch (...) { }
    }
}

#endif //EMASTRATEGY_EXCEPTION_HPP
