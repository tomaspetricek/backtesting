#include <iostream>
#include <vector>
#include <memory>

#include "exceptions.h"
#include "indicator.h"
#include "strategy.h"
#include "read.h"
#include "position.h"

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
    // create strategy
    indicator::ema short_ema{5};
    indicator::ema middle_ema{21};
    indicator::ema long_ema{63};
    std::shared_ptr<strategy::long_triple_ema> strategy = std::make_shared<strategy::long_triple_ema>(short_ema,
            middle_ema, long_ema);

    // read candles
    std::filesystem::path candle_csv("../eth-usdt-1-hour.csv");
    std::chrono::seconds period = std::chrono::hours(1);
    std::vector<candle> candles = read_candles(candle_csv, ';', period);

    std::shared_ptr<long_order> order;
    std::shared_ptr<long_position> pos;
    std::vector<std::shared_ptr<long_position>> closed;

    double pos_size = 100;

    // use strategy
    for (const auto& candle : candles) {
        order = dynamic_pointer_cast<long_order>((*strategy)(candle.get_close()));

        if (order) {
            auto point = trading::point(candle.get_close(), candle.get_created());

            // open position
            if (order->action()==action::buy) {
                pos = std::make_shared<long_position>(point, pos_size);
            }
            // close position
            else if (order->action()==action::sell) {
                pos->set_exit(point);
                closed.push_back(pos);
                std::cout << pos->get_percent_gain() << std::endl;
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