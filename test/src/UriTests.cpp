/**
 * @file UriTests.cpp
 * 
 * This module contains the unit tests of the Uri::Uri class.
 * 
 */

#include <gtest/gtest.h>
#include <stddef.h>
#include <Uri/Uri.h>


TEST(UriTests, ParseFromStringNoScheme) {
    Uri::Uri uri;
    
    ASSERT_TRUE(uri.ParseFromString("foo/bar"));
    ASSERT_EQ("", uri.GetScheme());
    ASSERT_EQ(
        (std::vector<std::string>{
            "foo",
            "bar",
        }),
        uri.GetPath()
    );
}

TEST(UriTests, ParseFromStringUrl) {
    Uri::Uri uri;
    
    ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar"));
    ASSERT_EQ("http", uri.GetScheme());
    ASSERT_EQ("www.example.com", uri.GetHost());
    ASSERT_EQ(
        (std::vector<std::string>{
            "",
            "foo",
            "bar",
        }),
        uri.GetPath()
    );
}

TEST(UriTests, ParseFromStringUrnDefaultPathDelimiter) {
    Uri::Uri uri;

    ASSERT_TRUE(uri.ParseFromString("urn:book:fantasy:Hobbit"));
    ASSERT_EQ("urn", uri.GetScheme());
    ASSERT_EQ("", uri.GetHost());
    ASSERT_EQ(
        (std::vector<std::string>{
            "book:fantasy:Hobbit",
        }),
        uri.GetPath()
    );
}

TEST(UriTests, ParseFromStringPathCornerCases) {
    struct TestVector {
        std::string pathIn;
        std::vector<std::string> pathOut;
    };

    const std::vector<TestVector> testVectors{
        {"", {}},
        {"/", {""}},
        {"/foo", {"", "foo"}},
        {"foo/", {"foo", ""}},
    };

    size_t index = 0;
    for (const auto& testVector : testVectors) {
        Uri::Uri uri;
    
        ASSERT_TRUE(uri.ParseFromString(testVector.pathIn)) << index;
        ASSERT_EQ(testVector.pathOut, uri.GetPath()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringHasAPortNumber) {
    Uri::Uri uri;
    
    ASSERT_TRUE(uri.ParseFromString("http://www.example.com:8080/foo/bar"));
    ASSERT_EQ("www.example.com", uri.GetHost());
    ASSERT_TRUE(uri.HasPort());
    ASSERT_EQ(8080, uri.GetPort());
}

TEST(UriTests, ParseFromStringDoesNotHasAPortNumber) {
    Uri::Uri uri;
    
    ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar"));
    ASSERT_EQ("www.example.com", uri.GetHost());
    ASSERT_FALSE(uri.HasPort());
}

TEST(UriTests, ParseFromStringTwiceFirstWithPortNumberThenWithout) {
    Uri::Uri uri;
    
    ASSERT_TRUE(uri.ParseFromString("http://www.example.com:8080/foo/bar"));
    ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar"));
    ASSERT_EQ("www.example.com", uri.GetHost());
    ASSERT_FALSE(uri.HasPort());
}

TEST(UriTests, ParseFromStringBadPortNumber) {
    Uri::Uri uri;
    
    ASSERT_FALSE(uri.ParseFromString("http://www.example.com:spam/foo/bar"));
    ASSERT_FALSE(uri.ParseFromString("http://www.example.com:8080spam/foo/bar"));
    ASSERT_FALSE(uri.ParseFromString("http://www.example.com:spam8080/foo/bar"));
}

TEST(UriTests, ParseFromStringLargestValidPortNumber) {
    Uri::Uri uri;
    
    ASSERT_TRUE(uri.ParseFromString("http://www.example.com:6553/foo/bar"));
    ASSERT_TRUE(uri.HasPort());
    ASSERT_EQ(6553, uri.GetPort());
}

TEST(UriTests, ParseFromStringBadPortNumberTooBig) {
    Uri::Uri uri;
    
    ASSERT_FALSE(uri.ParseFromString("http://www.example.com:65536/foo/bar"));
}

TEST(UriTests, ParseFromStringBadPortNumberNegative) {
    Uri::Uri uri;
    
    ASSERT_FALSE(uri.ParseFromString("http://www.example.com:-1234/foo/bar"));
}

TEST(UriTests, ParseFromStringEndsAfterAuthority) {
    Uri::Uri uri;
    
    ASSERT_TRUE(uri.ParseFromString("http://example.com"));
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativeReferences) {
    struct TestVector {
        std::string uriString;
        bool isRelativeReference;
    };

    const std::vector<TestVector> testVectors{
        {"http://example.com/", false},
        {"http://example.com", false},
        {"/", true}, // absolute-path reference
        {"//example.com", true}, // network-path reference
        {"/foo", true}, // absolute-path reference
        {"foo/", true}, // relative-path reference
    };

    size_t index = 0;
    for (const auto& testVector : testVectors) {
        Uri::Uri uri;
    
        ASSERT_TRUE(uri.ParseFromString(testVector.uriString)) << index;
        ASSERT_EQ(testVector.isRelativeReference, uri.IsRelativeReference()) << index;
        ++index;
    }
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativePaths) {
    struct TestVector {
        std::string uriString;
        bool isRelativePath;
    };

    const std::vector<TestVector> testVectors{
        {"http://example.com/", false},
        {"http://example.com", true},
        {"/", false}, // absolute-path reference
        {"//example.com", true}, // network-path reference
        {"/foo", false}, // absolute-path reference
        {"foo/", true}, // relative-path reference
        /*
         * This is only a valid test vector if we understand
         * correctly that an empty string IS a valid
         * "relative reference" URI with an empty path.
         */
        {"", true},
    };

    size_t index = 0;
    for (const auto& testVector : testVectors) {
        Uri::Uri uri;
    
        ASSERT_TRUE(uri.ParseFromString(testVector.uriString)) << index;
        ASSERT_EQ(testVector.isRelativePath, uri.ContainsRelativePath()) << index;
        ++index;
    }
}