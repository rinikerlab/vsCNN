//
// Created by gregor on 31.07.19.
//

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_tests

#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

#include <vector>
#include "../src/datatypes.h"
#include "../src/neighbors.h"

#include "../src/clustering.h"
#include "../src/core.h"
#include "../src/cnn.h"
#include "../src/vs_cnn.h"
#include "../src/geometry.h"

struct dataFixture {
    dataFixture() {
        nns::neighbors(shrt_neighbor_lists,
                       shrt,
                       fixed_cut,
                       0);

        nns::neighbors(mdm_neighbor_lists,
                       mdm,
                       fixed_cut,
                       0);

        nns::neighbors(lng_neighbor_lists,
                       lng,
                       fixed_cut,
                       0);
    };

    ~dataFixture() {};

    const float fixed_cut = 2.0 * std::sqrt(3);
    const unsigned int fixed_sim = 2;
    Neighbors shrt_neighbor_lists;
    Neighbors mdm_neighbor_lists;
    Neighbors lng_neighbor_lists;

    std::vector<std::vector<float>> shrt = {{1.,  2.,  3.},
                                            {2.,  3.,  4.},
                                            {3.,  4.,  5.},
                                            {4.,  5.,  6.},
                                            {5.,  6.,  7.},
                                            {6.,  7.,  8.},
                                            {7.,  8.,  9.},
                                            {91., 92., 93.},
                                            {92., 93., 94.},
                                            {93., 94., 95.},
                                            {94., 95., 96.},
                                            {95., 96., 97.},
                                            {96., 97., 98.},
                                            {97., 98., 99.}};

    std::vector<std::vector<float>> mdm = {{0.,  1.,  2.},
                                           {1.,  2.,  3.},
                                           {2.,  3.,  4.},
                                           {3.,  4.,  5.},
                                           {4.,  5.,  6.},
                                           {5.,  6.,  7.},
                                           {6.,  7.,  8.},
                                           {7.,  8.,  9.},
                                           {10,  11., 12.},
                                           {91., 92., 93.},
                                           {92., 93., 94.},
                                           {93., 94., 95.},
                                           {94., 95., 96.},
                                           {95., 96., 97.},
                                           {96., 97., 98.},
                                           {97., 98., 99.}};

    std::vector<std::vector<float>> lng = {{0.,  1.,  2.},
                                           {1.,  2.,  3.},
                                           {2.,  3.,  4.},
                                           {3.,  4.,  5.},
                                           {4.,  5.,  6.},
                                           {5.,  6.,  7.},
                                           {6.,  7.,  8.},
                                           {7.,  8.,  9.},
                                           {8.,  9.,  10.},
                                           {9.,  10., 11.},
                                           {20., 21., 22.},
                                           {21., 22., 23.},
                                           {22., 23., 24.},
                                           {23., 24., 25.},
                                           {24., 25., 26.},
                                           {25., 26., 27.},
                                           {26., 27., 28.},
                                           {27., 28., 29.},
                                           {28., 29., 30.},
                                           {29., 30., 31.},
                                           {91., 92., 93.},
                                           {92., 93., 94.},
                                           {93., 94., 95.},
                                           {94., 95., 96.},
                                           {95., 96., 97.},
                                           {96., 97., 98.},
                                           {97., 98., 99.}};
};

BOOST_FIXTURE_TEST_SUITE(NeighborsTestSuite, dataFixture)

    BOOST_AUTO_TEST_CASE(neighbors) {

        const float cut = std::sqrt(3);
        const unsigned int sim = 0;

        Neighbors neighbor_lists;
        nns::neighbors(neighbor_lists,
                       shrt,
                       cut,
                       sim);

        for (auto list : neighbor_lists) {
            if (list.first == 0 || list.first == 7) {
                BOOST_CHECK_EQUAL(list.second[0], list.first + 1);
            } else if (list.first == 6 || list.first == 13) {
                BOOST_CHECK_EQUAL(list.second[0], list.first - 1);
            } else {
                BOOST_CHECK_EQUAL(list.second[0], list.first - 1);
                BOOST_CHECK_EQUAL(list.second[1], list.first + 1);
            }
        }
    }

    BOOST_AUTO_TEST_CASE(neighbor_pruning) {

        const float cut = std::sqrt(3);
        const unsigned int sim = 1;

        Neighbors neighbor_lists;
        nns::neighbors(neighbor_lists,
                       lng,
                       cut,
                       sim);

        for (auto list : neighbor_lists) {
            BOOST_CHECK_EQUAL(list.second[0], list.first - 1);
            BOOST_CHECK_EQUAL(list.second[1], list.first + 1);
        }
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(CNNTestSuite, dataFixture)

    BOOST_AUTO_TEST_CASE(CNNsimilarity) {

        const float cut = 2.0f * std::sqrt(3);
        unsigned int sim = 0;
        Neighbors neighbor_lists;
        nns::neighbors(neighbor_lists,
                       shrt,
                       cut,
                       sim);
        sim = 2;

        BOOST_CHECK(!Clustering::CommonNearestNeighbor::similarity(shrt,
                                                                   neighbor_lists,
                                                                   0, 1,
                                                                   cut, sim));
        BOOST_CHECK(Clustering::CommonNearestNeighbor::similarity(shrt,
                                                                  neighbor_lists,
                                                                  2, 1,
                                                                  cut, sim));
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(vsCNNTestSuite, dataFixture)

    BOOST_AUTO_TEST_CASE(vsCNNdistance, *utf::tolerance(0.00001)) {
        BOOST_TEST(Clustering::CommonDensity::calc_distance(shrt[0], shrt[2]) == std::sqrt(12));
    }

    BOOST_AUTO_TEST_CASE(regularized_intersection_volume, *utf::tolerance(0.00001)) {
        BOOST_TEST(Geometry::regularized_intersection_volume(0.0f, std::sqrt(3.0f), 3) == 1.0f);
        BOOST_TEST(Geometry::regularized_intersection_volume(std::sqrt(3.0f), std::sqrt(3.0f), 3) == 0.3125f);
        BOOST_TEST(Geometry::regularized_intersection_volume(2.0f * std::sqrt(3.0f), std::sqrt(3.0f), 3) == 0.0f);
    }

    BOOST_AUTO_TEST_CASE(vsCNNsimilarity) {

        const float cut = 2.0f * std::sqrt(3);
        unsigned int sim = 0;
        Neighbors neighbor_lists;
        nns::neighbors(neighbor_lists,
                       shrt,
                       cut,
                       sim);
        sim = 2;

        // This is a different result to the CNN similarity and the reason
        // why the vsCNN clustering is more 'crisp'.
        BOOST_CHECK(Clustering::CommonDensity::similarity(shrt,
                                                          neighbor_lists,
                                                          0, 1,
                                                          cut, sim));
        BOOST_CHECK(Clustering::CommonDensity::similarity(shrt,
                                                          neighbor_lists,
                                                          2, 1,
                                                          cut, sim));
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(CoreTestSuite, dataFixture)

    BOOST_AUTO_TEST_CASE(intersection) {
        const vector<unsigned int> list1 = {0, 1, 2, 3};
        const vector<unsigned int> list2 = {2, 3, 4, 5};
        const vector<unsigned int> list3 = {4, 5, 6, 7};

        vector<unsigned int> out1(0);
        Clustering::Core::intersection(out1, list1, list2);

        vector<unsigned int> out2(0);
        Clustering::Core::intersection(out2, list1, list3);

        BOOST_CHECK(out1[0] == 2);
        BOOST_CHECK(out1[1] == 3);
        BOOST_CHECK(out2.empty());
    }


    BOOST_AUTO_TEST_CASE(similarity) {

        Clustering::Core::simptr similarity = Clustering::CommonNearestNeighbor::similarity;

        map<unsigned int, int> clustered;
        vector<vector<unsigned int> > clusters;

        unsigned int refpoint = 2;
        Clustering::Core::similarity_unclustered(similarity,
                                                 shrt,
                                                 clustered,
                                                 clusters,
                                                 shrt_neighbor_lists,
                                                 shrt_neighbor_lists[refpoint],
                                                 refpoint,
                                                 fixed_cut,
                                                 fixed_sim);

        refpoint = 11;
        Clustering::Core::similarity_unclustered(similarity,
                                                 shrt,
                                                 clustered,
                                                 clusters,
                                                 shrt_neighbor_lists,
                                                 shrt_neighbor_lists[refpoint],
                                                 refpoint,
                                                 fixed_cut,
                                                 fixed_sim);

        // Sort because of parallel loop in similarity_unclustered
        for (auto &cluster : clusters) {
            std::sort(cluster.begin(), cluster.end(),
                      [](const unsigned int &a, const unsigned int &b) { return a > b; });
        }

        BOOST_CHECK(clusters.size() == 2);
        BOOST_CHECK(clusters[0].size() == 3);
        BOOST_CHECK(clusters[1].size() == 3);
        BOOST_CHECK(clusters[0][0] == 3);
        BOOST_CHECK(clusters[0][1] == 2);
        BOOST_CHECK(clusters[0][2] == 1);
        BOOST_CHECK(clusters[1][0] == 12);
        BOOST_CHECK(clusters[1][1] == 11);
        BOOST_CHECK(clusters[1][2] == 10);

        refpoint = 3;
        Clustering::Core::similarity_clustered(similarity,
                                               shrt,
                                               clustered,
                                               clusters,
                                               shrt_neighbor_lists,
                                               shrt_neighbor_lists[refpoint],
                                               refpoint,
                                               fixed_cut,
                                               fixed_sim);

        refpoint = 10;
        Clustering::Core::similarity_clustered(similarity,
                                               shrt,
                                               clustered,
                                               clusters,
                                               shrt_neighbor_lists,
                                               shrt_neighbor_lists[refpoint],
                                               refpoint,
                                               fixed_cut,
                                               fixed_sim);

        // Sort because of parallel loop in similarity_unclustered
        for (auto &cluster : clusters) {
            std::sort(cluster.begin(), cluster.end(),
                      [](const unsigned int &a, const unsigned int &b) { return a > b; });
        }

        BOOST_CHECK(clusters.size() == 2);
        BOOST_CHECK(clusters[0].size() == 4);
        BOOST_CHECK(clusters[1].size() == 4);
        BOOST_CHECK(clusters[0][0] == 4);
        BOOST_CHECK(clusters[0][1] == 3);
        BOOST_CHECK(clusters[0][2] == 2);
        BOOST_CHECK(clusters[0][3] == 1);
        BOOST_CHECK(clusters[1][0] == 12);
        BOOST_CHECK(clusters[1][1] == 11);
        BOOST_CHECK(clusters[1][2] == 10);
        BOOST_CHECK(clusters[1][3] == 9);
    }

    BOOST_AUTO_TEST_CASE(CNNalgorithm) {

        const float cut = 5.0;
        const unsigned int sim = 2;

        Neighbors dummy_neighbors;
        vector<vector<unsigned int> > clusters;
        clusters = Clustering::Core::algorithm(Clustering::CommonNearestNeighbor::similarity,
                                               mdm,
                                               mdm_neighbor_lists,
                                               dummy_neighbors,
                                               cut,
                                               sim,
                                               0,
                                               true);

        for (auto &cluster : clusters) {
            std::sort(cluster.begin(), cluster.end(),
                      [](const unsigned int &a, const unsigned int &b) { return a > b; });
        }

        BOOST_CHECK_EQUAL(clusters.size(), 2);
        BOOST_CHECK_EQUAL(clusters[0].size(), 6);
        BOOST_CHECK_EQUAL(clusters[1].size(), 5);
        BOOST_CHECK_EQUAL(clusters[0][0], 6);
        BOOST_CHECK_EQUAL(clusters[0][1], 5);
        BOOST_CHECK_EQUAL(clusters[0][2], 4);
        BOOST_CHECK_EQUAL(clusters[0][3], 3);
        BOOST_CHECK_EQUAL(clusters[0][4], 2);
        BOOST_CHECK_EQUAL(clusters[0][5], 1);
        BOOST_CHECK_EQUAL(clusters[1][0], 14);
        BOOST_CHECK_EQUAL(clusters[1][1], 13);
        BOOST_CHECK_EQUAL(clusters[1][2], 12);
        BOOST_CHECK_EQUAL(clusters[1][3], 11);
        BOOST_CHECK_EQUAL(clusters[1][4], 10);

    }

    BOOST_AUTO_TEST_CASE(vsCNNalgorithm) {

        const float cut = 5.0;
        const unsigned int sim = 2;

        Neighbors dummy_neighbors;
        vector<vector<unsigned int> > clusters;
        clusters = Clustering::Core::algorithm(Clustering::CommonDensity::similarity,
                                               mdm,
                                               mdm_neighbor_lists,
                                               dummy_neighbors,
                                               cut,
                                               sim,
                                               0,
                                               true);

        for (auto &cluster : clusters) {
            std::sort(cluster.begin(), cluster.end(),
                      [](const unsigned int &a, const unsigned int &b) { return a > b; });
        }

        BOOST_CHECK_EQUAL(clusters.size(), 2);
        BOOST_CHECK_EQUAL(clusters[0].size(), 8);
        BOOST_CHECK_EQUAL(clusters[1].size(), 7);
        BOOST_CHECK_EQUAL(clusters[0][0], 7);
        BOOST_CHECK_EQUAL(clusters[0][1], 6);
        BOOST_CHECK_EQUAL(clusters[0][2], 5);
        BOOST_CHECK_EQUAL(clusters[0][3], 4);
        BOOST_CHECK_EQUAL(clusters[0][4], 3);
        BOOST_CHECK_EQUAL(clusters[0][5], 2);
        BOOST_CHECK_EQUAL(clusters[0][6], 1);
        BOOST_CHECK_EQUAL(clusters[0][7], 0);
        BOOST_CHECK_EQUAL(clusters[1][0], 15);
        BOOST_CHECK_EQUAL(clusters[1][1], 14);
        BOOST_CHECK_EQUAL(clusters[1][2], 13);
        BOOST_CHECK_EQUAL(clusters[1][3], 12);
        BOOST_CHECK_EQUAL(clusters[1][4], 11);
        BOOST_CHECK_EQUAL(clusters[1][5], 10);
        BOOST_CHECK_EQUAL(clusters[1][6], 9);
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ClusteringTestSuite, dataFixture)

    BOOST_AUTO_TEST_CASE(clustering_cnn) {

        const float cut = 5.0;
        const unsigned int sim = 2;

        vector<vector<unsigned int> > clusters;
        clusters = Clustering::clustering(Clustering::CommonNearestNeighbor::similarity,
                                          mdm,
                                          cut,
                                          sim,
                                          0,
                                          true);

        for (auto &cluster : clusters) {
            std::sort(cluster.begin(), cluster.end(),
                      [](const unsigned int &a, const unsigned int &b) { return a > b; });
        }

        BOOST_CHECK_EQUAL(clusters.size(), 2);
        BOOST_CHECK_EQUAL(clusters[0].size(), 6);
        BOOST_CHECK_EQUAL(clusters[1].size(), 5);
        BOOST_CHECK_EQUAL(clusters[0][0], 6);
        BOOST_CHECK_EQUAL(clusters[0][1], 5);
        BOOST_CHECK_EQUAL(clusters[0][2], 4);
        BOOST_CHECK_EQUAL(clusters[0][3], 3);
        BOOST_CHECK_EQUAL(clusters[0][4], 2);
        BOOST_CHECK_EQUAL(clusters[0][5], 1);
        BOOST_CHECK_EQUAL(clusters[1][0], 14);
        BOOST_CHECK_EQUAL(clusters[1][1], 13);
        BOOST_CHECK_EQUAL(clusters[1][2], 12);
        BOOST_CHECK_EQUAL(clusters[1][3], 11);
        BOOST_CHECK_EQUAL(clusters[1][4], 10);
    }

    BOOST_AUTO_TEST_CASE(clustering_vscnn) {

        const float cut = 5.0;
        const unsigned int sim = 2;

        vector<vector<unsigned int> > clusters;
        clusters = Clustering::clustering(Clustering::CommonDensity::similarity,
                                          mdm,
                                          cut,
                                          sim,
                                          0,
                                          true);

        for (auto &cluster : clusters) {
            std::sort(cluster.begin(), cluster.end(),
                      [](const unsigned int &a, const unsigned int &b) { return a > b; });
        }

        BOOST_CHECK_EQUAL(clusters.size(), 2);
        BOOST_CHECK_EQUAL(clusters[0].size(), 8);
        BOOST_CHECK_EQUAL(clusters[1].size(), 7);
        BOOST_CHECK_EQUAL(clusters[0][0], 7);
        BOOST_CHECK_EQUAL(clusters[0][1], 6);
        BOOST_CHECK_EQUAL(clusters[0][2], 5);
        BOOST_CHECK_EQUAL(clusters[0][3], 4);
        BOOST_CHECK_EQUAL(clusters[0][4], 3);
        BOOST_CHECK_EQUAL(clusters[0][5], 2);
        BOOST_CHECK_EQUAL(clusters[0][6], 1);
        BOOST_CHECK_EQUAL(clusters[0][7], 0);
        BOOST_CHECK_EQUAL(clusters[1][0], 15);
        BOOST_CHECK_EQUAL(clusters[1][1], 14);
        BOOST_CHECK_EQUAL(clusters[1][2], 13);
        BOOST_CHECK_EQUAL(clusters[1][3], 12);
        BOOST_CHECK_EQUAL(clusters[1][4], 11);
        BOOST_CHECK_EQUAL(clusters[1][5], 10);
        BOOST_CHECK_EQUAL(clusters[1][6], 9);
    }

BOOST_AUTO_TEST_SUITE_END()



