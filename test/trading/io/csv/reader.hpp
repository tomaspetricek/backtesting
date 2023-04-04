//
// Created by Tomáš Petříček on 04.04.2023.
//

#ifndef BACKTESTING_TEST_IO_CSV_READER_HPP
#define BACKTESTING_TEST_IO_CSV_READER_HPP

#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <array>
#include <string>
#include <trading/io/csv/reader.hpp>

BOOST_AUTO_TEST_SUITE(io_csv_reader_test)
    BOOST_AUTO_TEST_CASE(constructor_exception_test)
    {
        BOOST_REQUIRE_THROW(trading::io::csv::reader<2>{{"does-not-exist.csv"}}, std::invalid_argument);
    }
    std::filesystem::path test_files_dir{"../../test/data/in/csv"};

    BOOST_AUTO_TEST_CASE(read_incorrect_column_count_test)
    {
        constexpr std::size_t col_count{3};
        using reader_type = trading::io::csv::reader<col_count+1>;
        reader_type reader{{test_files_dir/"header-only.csv"}};
        std::array<std::string, reader_type::column_count> header;
        BOOST_REQUIRE_THROW(reader.read_header(header), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(read_header_test)
    {
        constexpr std::size_t col_count{3};
        using reader_type = trading::io::csv::reader<col_count>;
        reader_type reader{{test_files_dir/"header-only.csv"}};
        std::array<std::string, reader_type::column_count> actual_header;
        BOOST_REQUIRE(reader.read_header(actual_header));
        std::array<std::string, reader_type::column_count> expect_header{"season", "gender", "height"};
        BOOST_TEST(actual_header==expect_header);
    }

    BOOST_AUTO_TEST_CASE(read_empty_file_test)
    {
        constexpr std::size_t col_count{1};
        using reader_type = trading::io::csv::reader<col_count>;
        reader_type reader{{test_files_dir/"empty.csv"}};
        std::array<std::string, reader_type::column_count> header;
        BOOST_REQUIRE(!reader.read_header(header));
        int a;
        BOOST_REQUIRE(!reader.read_row(a));
    }

    BOOST_AUTO_TEST_CASE(read_row_incorrect_type_test)
    {
        constexpr std::size_t col_count{3};
        using reader_type = trading::io::csv::reader<col_count>;
        reader_type reader{{test_files_dir/"body-only.csv"}};
        int a, b, c;
        BOOST_REQUIRE_THROW(reader.read_row(a, b, c), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(read_row_test)
    {
        constexpr std::size_t col_count{3};
        using reader_type = trading::io::csv::reader<col_count>;
        reader_type reader{{test_files_dir/"body-only.csv"}};

        // read 1st row
        int season;
        std::string gender;
        float height;
        BOOST_REQUIRE(reader.read_row(season, gender, height));
        BOOST_REQUIRE_EQUAL(season, 1);
        BOOST_REQUIRE_EQUAL(gender, std::string{"male"});
        BOOST_REQUIRE_EQUAL(height, 1.75);

        // read 2nd row
        BOOST_REQUIRE(reader.read_row(season, gender, height));
        BOOST_REQUIRE_EQUAL(season, 3);
        BOOST_REQUIRE_EQUAL(gender, std::string{"female"});
        BOOST_REQUIRE_CLOSE(height, 1.6, 0.001);
    }

    BOOST_AUTO_TEST_CASE(use_incorrect_delimiter_test)
    {
        constexpr std::size_t col_count{3};
        using reader_type = trading::io::csv::reader<col_count>;
        reader_type reader{{test_files_dir/"body-only.csv"}, ';'};
        std::array<std::string, col_count> header;
        BOOST_REQUIRE_THROW(reader.read_header(header), std::runtime_error);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //BACKTESTING_TEST_IO_CSV_READER_HPP
