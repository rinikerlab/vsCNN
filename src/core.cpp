/*

MIT License

Copyright (c) 2020, R. Gregor Weiß, Benjamin Ries

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#include <set>
#include <numeric>

#include <omp.h>
#include <parallel/algorithm>

#include "tools/utility.h"

#include "core.h"

namespace Clustering {

    namespace Core {

        ////////////// CORE UTILITY ///////////////
        void intersection(vector<unsigned int> &out,
                          const vector<unsigned> &list1,
                          const vector<unsigned> &list2) {
            __gnu_parallel::set_intersection(list1.begin(), list1.end(),
                                             list2.begin(), list2.end(),
                                             std::back_inserter(out));
        }

        ////////////// CORE UTILITY ///////////////
        void
        similarity_unclustered(Similarity similarity,
                               vector<vector<float> > &data,
                               map<unsigned int, int> &clustered,
                               vector<vector<unsigned int> > &clusters,
                               const Neighbors &neighbors_ij,
                               const vector<unsigned int> &input,
                               const unsigned int refpoint,
                               const float cut,
                               const unsigned int sim) {
            vector<unsigned int> cluster(1, refpoint);
            unsigned int cluster_idx(clusters.size());

            unsigned int refpoint_is_clustered = 0;
#pragma omp critical
            refpoint_is_clustered = clustered.count(refpoint);

            if (refpoint_is_clustered == 0) {
#ifdef _OPENMP
#pragma omp parallel for default(none) shared(similarity, data, clustered, cluster, neighbors_ij, input, cluster_idx)
#endif
                for (size_t i = 0; i < input.size(); i++) {
                    const unsigned int point = input.at(i);
                    if (clustered.count(point) == 0) {
                        if (point != refpoint && neighbors_ij.count(point) == 1) {
                            bool similar = similarity(data,
                                                      neighbors_ij,
                                                      refpoint,
                                                      point,
                                                      cut,
                                                      sim);
#pragma omp critical
                            if (similar) {
                                clustered[point] = cluster_idx;
                                cluster.push_back(point);
                            }
                        }
                    }
                }
                // Only add this cluster if at least two points was added
                if (cluster.size() > 1) {
                    clustered[refpoint] = cluster_idx;
                    clusters.push_back(cluster);
                }
            } else {
                cout << "intersect_unclustered:" << endl;
                cout << "If you read this message please report." << endl;
                cout << refpoint << " " << refpoint_is_clustered << endl;
            }
        }

        void similarity_clustered(Similarity similarity,
                                  vector<vector<float> > &data,
                                  map<unsigned int, int> &clustered,
                                  vector<vector<unsigned int> > &clusters,
                                  const Neighbors &neighbors_ij,
                                  const vector<unsigned int> &input,
                                  const unsigned int refpoint,
                                  const float cut,
                                  const unsigned int sim) {
            unsigned int cluster_idx = clustered[refpoint];

            unsigned int refpoint_is_clustered = 1;
#pragma omp critical
            refpoint_is_clustered = clustered.count(refpoint);

            if (clustered.count(refpoint) == 1) {
                for (auto const &point : input) {
                    if (clustered.count(point) == 0) {
                        if (point != refpoint && neighbors_ij.count(point) == 1) {
                            bool similar = similarity(data,
                                                      neighbors_ij,
                                                      refpoint,
                                                      point,
                                                      cut,
                                                      sim);
#pragma omp critical
                            if (clustered.count(point) == 0) {
                                if (similar) {
                                    clusters[cluster_idx].push_back(point);
                                    clustered[point] = cluster_idx;
                                }
                            }
                        }
                    }
                }
            } else {
                cout << "intersect_clustered:" << endl;
                cout << "If you read this message please report." << endl;
                cout << refpoint << " " << refpoint_is_clustered << endl;
            }
        }

        // USER INTERFACE CLUSTERING
        // Clusters the data
        vector<vector<unsigned int> >
        algorithm(Similarity similarity,
                  vector<vector<float> > &data,
                  Neighbors &neighbors_ij,
                  Neighbors &second_neighbors_ij,
                  const float cut,
                  const unsigned int sim,
                  const unsigned int Nkeep,
                  const bool mutual) {
            map<unsigned int, int> clustered;
            vector<vector<unsigned int> > clusters;

            // sort by size of neighbor lists
            vector<std::pair<unsigned int, unsigned int> > neighbor_list_sizes;
            for (auto const &list : neighbors_ij) {
                const unsigned int refpoint = list.first;
                unsigned int neighbor_list_size = neighbors_ij[refpoint].size();
                if (!mutual && second_neighbors_ij.count(refpoint) == 1) {
                    neighbor_list_size += second_neighbors_ij.size();
                }
                neighbor_list_sizes.emplace_back(refpoint, neighbor_list_size);
            }
            // TODO : sorting can actually be removed. But for the sake of debugging we keep it right now.
            sort(neighbor_list_sizes.begin(), neighbor_list_sizes.end(),
                 [](const std::pair<unsigned int, unsigned int> &a,
                    const std::pair<unsigned int, unsigned int> &b) {
                     return a.second > b.second;
                 });

            // Goes through all points/frames/keys with neighbor lists
            unsigned prev_nof_clusters = clusters.size();
            for (auto const &it : neighbor_list_sizes) {
                const unsigned int refpoint = it.first;

                // Try seeding new cluster on current `refpoint`
                // exploiting its neighbor list and its second neighbor
                // list given `mutual` is turned off.
                if (clustered.count(refpoint) == 0) {
                    similarity_unclustered(similarity,
                                           data,
                                           clustered,
                                           clusters,
                                           neighbors_ij,
                                           neighbors_ij[refpoint],
                                           refpoint, cut,
                                           sim);
                    if (!mutual) {
                        if (clustered.count(refpoint) == 0) {
                            similarity_unclustered(similarity,
                                                   data,
                                                   clustered,
                                                   clusters,
                                                   neighbors_ij,
                                                   second_neighbors_ij[refpoint],
                                                   refpoint,
                                                   cut,
                                                   sim);
                        } else if (clustered.count(refpoint) == 1) {
                            similarity_clustered(similarity,
                                                 data,
                                                 clustered,
                                                 clusters,
                                                 neighbors_ij,
                                                 second_neighbors_ij[refpoint],
                                                 refpoint, cut,
                                                 sim);
                        }
                    }

                    // Go to next neighbor list if the current `refpoint` did not generate a new cluster.
                    if (clusters.size() > prev_nof_clusters) {
                        prev_nof_clusters = clusters.size();
                        const unsigned int cluster_idx = prev_nof_clusters - 1;

                        // Try adding points until no new points are added to the cluster
                        vector<unsigned int> to_consider(clusters[cluster_idx]);
                        __gnu_parallel::sort(to_consider.begin(), to_consider.end());
                        while (to_consider.size() > 0) {
                            vector<unsigned int> prev_cluster(clusters[cluster_idx]);
                            __gnu_parallel::sort(prev_cluster.begin(), prev_cluster.end());
#ifdef _OPENMP
#pragma omp parallel for default(none) shared(similarity, data, clustered, clusters, neighbors_ij, second_neighbors_ij, to_consider)
#endif
                            for (unsigned int frame = 0; frame < to_consider.size(); frame++) {
                                const unsigned int clpoint = to_consider[frame];
                                if (neighbors_ij.count(clpoint) == 1) {
                                    similarity_clustered(similarity,
                                                         data,
                                                         clustered,
                                                         clusters,
                                                         neighbors_ij,
                                                         neighbors_ij[clpoint],
                                                         clpoint,
                                                         cut,
                                                         sim);
                                }
                                if (!mutual && second_neighbors_ij.count(clpoint) == 1) {
                                    similarity_clustered(similarity,
                                                         data,
                                                         clustered,
                                                         clusters,
                                                         neighbors_ij,
                                                         second_neighbors_ij[clpoint],
                                                         clpoint,
                                                         cut,
                                                         sim);
                                }
                            }
                            vector<unsigned int> current_cluster(clusters[cluster_idx]);
                            __gnu_parallel::sort(current_cluster.begin(), current_cluster.end());

                            to_consider.clear();
                            __gnu_parallel::set_difference(current_cluster.begin(),
                                                           current_cluster.end(),
                                                           prev_cluster.begin(),
                                                           prev_cluster.end(),
                                                           std::back_inserter(to_consider));
                        }
                    }
                }
            }

            Clustering::Utility::sortNclean(clusters, Nkeep);

            return clusters;
        }

    } // end of namespace CommonNearestNeighbor
} // endo of namespace Clustering
