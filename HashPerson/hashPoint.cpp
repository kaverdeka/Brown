#include "test_runner.h"
#include <limits>
#include <random>
#include <unordered_set>

using namespace std;

using CoordType = int;

struct Point3D {
    CoordType x;
    CoordType y;
    CoordType z;

    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct Hasher {
    size_t operator() (const Point3D& point) const {
        size_t r1 = _hasher(point.x);
        size_t r2 = _hasher(point.y);
        size_t r3 = _hasher(point.z);
        return  x * x * r1 + x * r2 + r3;
    }
    hash<CoordType> _hasher;
    static constexpr size_t x = 2'946'901;
};

void TestSmoke() {
    vector<Point3D> points = {
            {1, 2, 3},
            {0, 2, 3},
            {1, 0, 3},
            {1, 2, 0},
    };

    unordered_set<Point3D, Hasher> point_set;
    for (const auto& point : points) {
        point_set.insert(point);
    }

    ASSERT_EQUAL(points.size(), point_set.size());
    for (const auto& point : points) {
        ASSERT_EQUAL(point_set.count(point), static_cast<size_t>(1));
    }
}

void TestPurity() {
    Point3D point = {1, 2, 3};
    Hasher hasher;

    auto hash = hasher(point);
    for (size_t t = 0; t < 100; ++t) {
        ASSERT_EQUAL(hasher(point), hash);
    }
};

void TestX() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    bool component_matters = false;
    for (size_t t = 0; t < 1000; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        Point3D other_point = point;
        other_point.x = dist(gen);

        component_matters |= (hasher(point) != hasher(other_point));
    }

    ASSERT(component_matters);
}

void TestY() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    bool component_matters = false;
    for (size_t t = 0; t < 1000; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        Point3D other_point = point;
        other_point.y = dist(gen);

        component_matters |= (hasher(point) != hasher(other_point));
    }

    ASSERT(component_matters);
}

void TestZ() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    bool component_matters = false;
    for (size_t t = 0; t < 1000; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        Point3D other_point = point;
        other_point.z = dist(gen);

        component_matters |= (hasher(point) != hasher(other_point));
    }

    ASSERT(component_matters);
}

void TestDistribution() {
    auto seed = 42;
    mt19937 gen(seed);
    uniform_int_distribution<CoordType> dist(
            numeric_limits<CoordType>::min(),
            numeric_limits<CoordType>::max()
    );

    Hasher hasher;

    const size_t num_buckets = 2053;

    const size_t perfect_bucket_size = 50;
    const size_t num_points = num_buckets * perfect_bucket_size;
    vector<size_t> buckets(num_buckets);
    for (size_t t = 0; t < num_points; ++t) {
        Point3D point = {dist(gen), dist(gen), dist(gen)};
        ++buckets[hasher(point) % num_buckets];
    }

    double pearson_stat = 0;
    for (auto bucket_count : buckets) {
        size_t count_diff = bucket_count - perfect_bucket_size;
        pearson_stat +=
                count_diff * count_diff / static_cast<double>(perfect_bucket_size);
    }

    const double critical_value = 2158.4981036918693;
    ASSERT(pearson_stat < critical_value);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSmoke);
    RUN_TEST(tr, TestPurity);
    RUN_TEST(tr, TestX);
    RUN_TEST(tr, TestY);
    RUN_TEST(tr, TestZ);
    RUN_TEST(tr, TestDistribution);

    return 0;
}