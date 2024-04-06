#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <algor.h>


class testAlgor : public algor 
{
public:
    double testAbs(double x)
    {
        return abs(x);
    }
    double testSqtr(double x)
    {
        return sqrt(x);
    }
    int testMin(int a, int b)
    {
        return min(a, b);
    }
    int testMax(int a, int b)
    {
        return max(a, b);
    }
    void swap(unsigned int &a, unsigned int &b)
    {
        return swap(a, b);
    }
};


TEST_CASE("Min Max")
{
    testAlgor ta;

    SUBCASE("Min")
    {
        CHECK(ta.testMin(1, 2) == 1);
        CHECK(ta.testMin(2, 1) == 1);
        CHECK(ta.testMin(0, 0) == 0);
        CHECK(ta.testMin(-1, 0) == -1);
        CHECK(ta.testMin(0, -1) == -1);
        CHECK(ta.testMin(10, 20) == 10);
        CHECK(ta.testMin(20, 10) == 10);
        CHECK(ta.testMin(12345, 67890) == 12345);
        CHECK(ta.testMin(67890, 12345) == 12345);
        CHECK(ta.testMin(INT_MAX, INT_MIN) == INT_MIN);
    }

    SUBCASE("Max")
    {
        CHECK(ta.testMin(1, 2) == 2);
        CHECK(ta.testMin(2, 1) == 2);
        CHECK(ta.testMin(0, 0) == 0);
        CHECK(ta.testMin(-1, 0) == 0);
        CHECK(ta.testMin(0, -1) == -1);
        CHECK(ta.testMin(10, 20) == 20);
        CHECK(ta.testMin(20, 10) == 20);
        CHECK(ta.testMin(12345, 67890) == 67890);
        CHECK(ta.testMin(67890, 12345) == 67890);
        CHECK(ta.testMin(INT_MAX, INT_MIN) == INT_MAX);
    }

    
}

