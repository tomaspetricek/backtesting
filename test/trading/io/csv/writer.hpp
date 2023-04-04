//
// Created by Tomáš Petříček on 04.04.2023.
//

#ifndef BACKTESTING_TEST_IO_CSV_WRITER_HPP
#define BACKTESTING_TEST_IO_CSV_WRITER_HPP

#include <vector>
#include <array>
#include <boost/test/unit_test.hpp>
#include <trading/io/csv/writer.hpp>
#include <trading/io/csv/reader.hpp>

BOOST_AUTO_TEST_SUITE(io_csv_writer_test)
    std::filesystem::path test_files_dir{"../../test/data/out/csv"};

    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        std::filesystem::path path{{test_files_dir/"does-not-exist.csv"}};
        std::filesystem::remove(path);
        BOOST_REQUIRE_NO_THROW(trading::io::csv::writer<2>{path});
    }

    BOOST_AUTO_TEST_CASE(usage_test)
    {
        struct person {
            int age;
            std::string name;
            double height;
        };
        std::vector<person> people{
                {18, "Tomas Edison", 1.75},
                {23, "Barack Obama", 1.6}
        };
        constexpr std::size_t n_cols = 3;
        std::filesystem::path path{{test_files_dir/"complete.csv"}};
        std::filesystem::remove(path);
        std::array<std::string, n_cols> expect_header{{"age", "name", "height"}}, actual_header;
        {
            trading::io::csv::writer<n_cols> writer{path};
            writer.write_header(expect_header);
            for (const auto& p: people)
                writer.write_row(p.age, p.name, p.height);
        }
        {
            trading::io::csv::reader<n_cols> reader{path};
            reader.read_header(actual_header);
            BOOST_TEST(actual_header==expect_header);
            person p;
            std::size_t i{0};

            while (reader.read_row(p.age, p.name, p.height)) {
                BOOST_REQUIRE(i!=people.size());
                BOOST_REQUIRE_EQUAL(p.age, people[i].age);
                BOOST_REQUIRE_EQUAL(p.name, people[i].name);
                BOOST_REQUIRE_EQUAL(p.height, people[i].height);
                i++;
            }
            BOOST_REQUIRE_EQUAL(i, people.size());
        }
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_IO_CSV_WRITER_HPP
