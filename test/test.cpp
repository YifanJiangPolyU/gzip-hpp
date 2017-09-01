#include <gzip.hpp>

#include <iostream>
#include <cassert>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <limits>

TEST_CASE("test version") {
    REQUIRE(GZIP_VERSION_STRING == std::string("1.0.0"));
}

TEST_CASE("successful compress - string") {
    std::string data = "hello";
    std::string value = gzip::compress(data);

    REQUIRE(value.size() > data.size());
}

TEST_CASE("successful compress - pointer (debug throws int overflow") {
    gzip::MAX_SIZE_BEFORE_COMPRESS = 5000000000;

    std::string data = "hello hello hello hello";
    const char * pointer = data.data();
    // numeric_limit useful for integer conversion
    unsigned int i = std::numeric_limits<unsigned int>::max();
    // turn int i into a long, so we can add to it safely without overflow
    unsigned long l = static_cast<unsigned long>(i) + 1; 
    
#ifdef DEBUG
    CHECK_THROWS_WITH(gzip::compress(pointer, l), Catch::Contains("size arg is too large to fit into unsigned int type"));
#else 
    std::string value = gzip::compress(pointer, l);
    REQUIRE(value.size() < data.size());
#endif
}

TEST_CASE("fail compress - throws max size limit") {
    gzip::MAX_SIZE_BEFORE_COMPRESS = 2000000000;
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();

    unsigned long l = 2000000001; 

    CHECK_THROWS_WITH(gzip::compress(pointer, l), Catch::Contains("size may use more memory than intended when decompressing"));

}

TEST_CASE("successful decompress") {
    std::string data = "hello hello hello hello";
    std::string compressed_data = gzip::compress(data);
    std::string value = gzip::decompress(compressed_data);

    REQUIRE(value.size() == data.size());
}

TEST_CASE("successful decompress - pointer (debug throws int overflow)") {
    gzip::MAX_SIZE_BEFORE_COMPRESS = 5000000000;
    std::string data = "hello hello hello hello";
    const char * pointer = data.data();
    std::string compressed_data = gzip::compress(pointer, data.size());
    const char * compressed_pointer = compressed_data.data();

    // numeric_limit useful for integer conversion
    unsigned int i = std::numeric_limits<unsigned int>::max();
    // turn int i into a long, so we can add to it safely without overflow
    unsigned long l = static_cast<unsigned long>(i) + 1; 

#ifdef DEBUG
    std::clog << "throws!!!";
    CHECK_THROWS(gzip::decompress(compressed_pointer, l));
#else 
    std::clog << "about to decompress RELEASE mode";
    std::string value = gzip::decompress(compressed_pointer, l);
    REQUIRE(value.size() == data.size());
#endif

}


TEST_CASE("invalid decompression")
{
    std::string data("this is a string that should be compressed data");
    // data is not compressed but we will try to decompress it

    CHECK_THROWS(gzip::decompress(data));
}

TEST_CASE("round trip compression - gzip")
{
    std::string data("this is a sentence that will be compressed into something");
    // Do we plan to create a bool method?
    CHECK(!gzip::is_compressed(data));
    
    int strategy;

    SECTION("strategy - invalid compression")
    {
        strategy = 99;
        int level = Z_DEFAULT_COMPRESSION;

        CHECK_THROWS(gzip::compress(data, level, strategy));
    }

    SECTION("compression level - invalid")
    {
        strategy = Z_DEFAULT_STRATEGY;
        int level = 99;

        CHECK_THROWS(gzip::compress(data, level, strategy));
    }

    SECTION("strategy - default")
    {
        strategy = Z_DEFAULT_STRATEGY;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(data, level, strategy);
                CHECK(gzip::is_compressed(compressed_data));
                std::string new_data = gzip::decompress(compressed_data);
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - filtered")
    {
        strategy = Z_FILTERED;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(data, level, strategy);
                CHECK(gzip::is_compressed(compressed_data));
                std::string new_data = gzip::decompress(compressed_data);
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - huffman only")
    {
        strategy = Z_HUFFMAN_ONLY;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(data, level, strategy);
                CHECK(gzip::is_compressed(compressed_data));
                std::string new_data = gzip::decompress(compressed_data);
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - rle")
    {
        strategy = Z_RLE;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(data, level, strategy);
                CHECK(gzip::is_compressed(compressed_data));
                std::string new_data = gzip::decompress(compressed_data);
                CHECK(data == new_data);
            }
        }
    }

    SECTION("strategy - fixed")
    {
        strategy = Z_FIXED;

        SECTION("no compression")
        {
            int level = Z_NO_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("default compression level")
        {
            int level = Z_DEFAULT_COMPRESSION;
            std::string compressed_data = gzip::compress(data, level, strategy);
            CHECK(gzip::is_compressed(compressed_data));
            std::string new_data = gzip::decompress(compressed_data);
            CHECK(data == new_data);
        }

        SECTION("compression level -- min to max")
        {
            for (int level = Z_BEST_SPEED; level <= Z_BEST_COMPRESSION; ++level)
            {
                std::string compressed_data = gzip::compress(data, level, strategy);
                CHECK(gzip::is_compressed(compressed_data));
                std::string new_data = gzip::decompress(compressed_data);
                CHECK(data == new_data);
            }
        }
    }
}