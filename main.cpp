#include <iostream>
#include <vector>

#include "exceptions.h"
#include "indicator.h"
#include "strategy.h"
#include "position.h"
#include "order.h"
#include "read.h"

using namespace trading;

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

void run()
{
    indicator::ema short_ema{2};
    indicator::ema middle_ema{4};
    indicator::ema long_ema{6};
    strategy::triple_ema strategy{short_ema, middle_ema, long_ema};


    // read candles
    std::filesystem::path candle_csv("../eth-usdt-1-hour.csv");
    std::chrono::seconds period = std::chrono::hours(1);
    std::vector<candle> candles = read_candles(candle_csv, ';', period);

    std::optional<order>order;
    std::shared_ptr<position> pos;
    std::vector<std::shared_ptr<position>> closed;

    // use strategy
    for (const auto& candle : candles) {
        order = strategy(candle.get_close());

        if (order) {
            auto point = trading::point(candle.get_close(), candle.get_created());

            // open position
            if (order->side()==side::long_)
                pos = std::make_shared<long_position>();
            else if (order->side()==side::short_)
                pos = std::make_shared<short_position>();

            // close position
            if (order->action()==action::buy) {
                pos->set_entry(point);
            }
            else if (order->action()==action::sell) {
                pos->set_exit(point);
                closed.push_back(pos);
            }
        }
    }

    std::cout << "done" << std::endl;
}

int main()
{
    try {
        run();
    }
    catch (const std::exception& e) {
        print_exception(e);
    }

    return 0;
}