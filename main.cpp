#include <iostream>
#include <vector>
#include <memory>

#include "exceptions.h"
#include "ema.h"
#include "triple_ema.h"
#include "read.h"
#include "position.h"
#include "currency.h"
#include "brute_force.h"

using namespace trading;
using namespace currency;
using namespace strategy;

void run()
{
    // create currency pair
    pair<crypto> pair{crypto::BTC, crypto::USDT};

    // read candles
    std::filesystem::path candle_csv("../data/btc-usdt-30-min.csv");
    std::chrono::seconds period = std::chrono::minutes(30);
    std::vector<candle> candles;

    try {
        candles = read_candles(candle_csv, ';', period);
    }
    catch (...) {
        std::throw_with_nested(std::runtime_error("Cannot read candles"));
    }

    if (candles.empty())
        throw std::runtime_error("No candles read");

    int min_period{1};
    int max_period{20};
    assert(max_period>=3);

    double pos_size{100};

    // prepare variables
    std::shared_ptr<strategy::long_triple_ema> strategy;
    std::optional<action>action;
    std::shared_ptr<long_position<crypto>> pos;
    std::vector<std::shared_ptr<long_position<crypto>>> closed;

    // use brute force
    for (int short_period{min_period}; short_period<max_period-1; short_period++) {
        for (int middle_period{short_period+1}; middle_period<max_period; middle_period++) {
            for (int long_period{middle_period+1}; long_period<max_period+1; long_period++) {
                std::cout << short_period << ", " << middle_period << ", " << long_period << std::endl;

                // create strategy
                try {
                    indicator::ema short_ema{short_period};
                    indicator::ema middle_ema{middle_period};
                    indicator::ema long_ema{long_period};
                    strategy = std::make_shared<strategy::long_triple_ema>(short_ema, middle_ema, long_ema);
                }
                catch (...) {
                    std::throw_with_nested(std::runtime_error("Cannot create strategy"));
                }

                // collect positions
                for (const auto& candle : candles) {
                    action = (*strategy)(candle.get_close());

                    if (action) {
                        auto point = trading::point(candle.get_close(), candle.get_created());

                        // open position
                        if (action==action::buy) {
                            pos = std::make_shared<long_position<crypto>>(point, pos_size, pair);
                        }
                            // close position
                        else if (action==action::sell) {
                            pos->set_exit(point);
                            closed.push_back(pos);
                        }
                    }
                }
            }
        }
    }
    std::cout << "done" << std::endl;

    // use optimizer
    optimizer::brute_force<int, int, int, int> optim;
    std::function<int(int, int, int)> func = [](int min_period, int middle_period, int long_period) {
        std::cout << "min period: " << min_period << std::endl
                  << "middle period: " << middle_period << std::endl
                  << "long period: " << long_period << std::endl;
        return 0;
    };
    range<int> min_periods{1, 10};
    range<int> middle_periods{2, 10};
    range<int> long_periods{3, 10};
    optim(min_periods, middle_periods, long_periods, func);

    // use range
    for (int i : min_periods)
        std::cout << i << " ";
    std::cout << std::endl;
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