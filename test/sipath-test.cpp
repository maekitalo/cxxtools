#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <cxxtools/jsondeserializer.h>
#include <sstream>

namespace
{
    cxxtools::SerializationInfo getExampleStore()
    {
        std::istringstream json(R"JSON(
        { "store": {
            "book": [ 
              { "category": "reference",
                "author": "Nigel Rees",
                "title": "Sayings of the Century",
                "price": 8.95
              },
              { "category": "fiction",
                "author": "Evelyn Waugh",
                "title": "Sword of Honour",
                "price": 12.99
              },
              { "category": "fiction",
                "author": "Herman Melville",
                "title": "Moby Dick",
                "isbn": "0-553-21311-3",
                "price": 8.99
              },
              { "category": "fiction",
                "author": "J. R. R. Tolkien",
                "title": "The Lord of the Rings",
                "isbn": "0-395-19395-8",
                "price": 22.99
              }
            ],
            "bicycle": {
              "color": "red",
              "addon": "rack",
              "addon": "light",
              "price": 719.95
            }
          }
        }
        )JSON");

        cxxtools::JsonDeserializer deserializer(json);
        return deserializer.si();
    }
}

class SiPathTest : public cxxtools::unit::TestSuite
{
    cxxtools::SerializationInfo exampleStore;

public:
    SiPathTest()
        : cxxtools::unit::TestSuite("sipath"),
          exampleStore(getExampleStore())
    {
        registerMethod("root", *this, &SiPathTest::root);
        registerMethod("member", *this, &SiPathTest::member);
        registerMethod("memberN", *this, &SiPathTest::memberN);
        registerMethod("allMembers", *this, &SiPathTest::allMembers);
        registerMethod("array", *this, &SiPathTest::array);
        registerMethod("size", *this, &SiPathTest::size);
        registerMethod("count", *this, &SiPathTest::count);
        registerMethod("type", *this, &SiPathTest::type);
        registerMethod("isnull", *this, &SiPathTest::isnull);
    }

    void root()
    {
        auto si = exampleStore.path("$");

        CXXTOOLS_UNIT_ASSERT(si.findMember("store"));
        CXXTOOLS_UNIT_ASSERT(si.getMember("store").findMember("book"));
    }

    void member()
    {
        {
            auto si = exampleStore.path("$.store");
            CXXTOOLS_UNIT_ASSERT(si.findMember("book"));
        }

        {
            auto si = exampleStore.path("store.bicycle");
            CXXTOOLS_UNIT_ASSERT(si.findMember("color"));
        }

        {
            auto si = exampleStore.path("\"store\".'bicycle'");
            CXXTOOLS_UNIT_ASSERT(si.findMember("price"));
        }
    }

    void memberN()
    {
        std::string addon;

        exampleStore.path("store.bicycle.addon{0}") >>= addon;
        CXXTOOLS_UNIT_ASSERT_EQUALS(addon, "rack");

        exampleStore.path("store.bicycle.addon{1}") >>= addon;
        CXXTOOLS_UNIT_ASSERT_EQUALS(addon, "light");
    }

    void allMembers()
    {
        std::vector<std::string> addons;

        exampleStore.path("store.bicycle.addon{}") >>= addons;
        CXXTOOLS_UNIT_ASSERT_EQUALS(addons.size(), 2);
        CXXTOOLS_UNIT_ASSERT_EQUALS(addons[0], "rack");
        CXXTOOLS_UNIT_ASSERT_EQUALS(addons[1], "light");
    }

    void array()
    {
        std::string title;
        exampleStore.path("store.book[2].title") >>= title;
        CXXTOOLS_UNIT_ASSERT_EQUALS(title, "Moby Dick");

        double price;
        exampleStore.path("[0][1][3]") >>= price;
        CXXTOOLS_UNIT_ASSERT_EQUALS(price, 719.95);
    }

    void size()
    {
        unsigned size = 0;
        exampleStore.path("store.book::size") >>= size;
        CXXTOOLS_UNIT_ASSERT_EQUALS(size, 4);

        size = 2;
        exampleStore.path("store.book[0]::size") >>= size;
        CXXTOOLS_UNIT_ASSERT_EQUALS(size, 4);
    }

    void count()
    {
        unsigned count = 0;
        exampleStore.path("store.bicycle.addon::count") >>= count;
        CXXTOOLS_UNIT_ASSERT_EQUALS(count, 2);

        count = 0;
        exampleStore.path("store.bicycle.color::count") >>= count;
        CXXTOOLS_UNIT_ASSERT_EQUALS(count, 1);
    }

    void type()
    {
        {
            cxxtools::SerializationInfo si;
            si.setTypeName("some type");
            std::string type;
            si.path("::type") >>= type;
            CXXTOOLS_UNIT_ASSERT_EQUALS(type, "some type");
        }

        {
            cxxtools::SerializationInfo si;
            si.addMember("foo").setTypeName("foo type");
            std::string type;
            si.path("foo::type") >>= type;
            CXXTOOLS_UNIT_ASSERT_EQUALS(type, "foo type");

            type.clear();
            si.path("[0]::type") >>= type;
            CXXTOOLS_UNIT_ASSERT_EQUALS(type, "foo type");
        }
    }

    void isnull()
    {
        cxxtools::SerializationInfo si;
        bool isnull = false;
        si.path("::isnull") >>= isnull;
        CXXTOOLS_UNIT_ASSERT(isnull);

        si <<= 42;
        si.path("::isnull") >>= isnull;
        CXXTOOLS_UNIT_ASSERT(!isnull);
    }
};

cxxtools::unit::RegisterTest<SiPathTest> register_SiPathTest;
