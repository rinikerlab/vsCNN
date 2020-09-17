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

#include <iostream>

#include <set>
#include <numeric> // std::accumulate

#include <omp.h>
#include <parallel/algorithm>

#include "clustering.h"

namespace Clustering {

    // INTERFACE CLUSTERING
    vector<vector<unsigned int> >
    clustering(Clustering::Core::Similarity similarity,
               vector<vector<float> > &data,
               const float cut,
               const unsigned int sim,
               const int Nkeep,
               const bool mutual) {
        // Obtain neighbor lists
        Neighbors neighbor_lists;
        Neighbors second_neighbor_lists;
        nns::neighbors(neighbor_lists, data, cut, sim);

        // Obtain clusters
        return Clustering::Core::algorithm(similarity,
                                           data,
                                           neighbor_lists,
                                           second_neighbor_lists,
                                           cut,
                                           sim,
                                           Nkeep,
                                           mutual);
    }

    // INTERFACE HIERARCHICAL CLUSTERING
    vector<clstep>
    hierarchical_clustering(Clustering::Core::Similarity similarity,
                            vector<vector<unsigned int> > &clusters,
                            vector<vector<float> > &data,
                            const clstep init_step,
                            const float delta_fe,
                            const unsigned int ndims,
                            const unsigned int Nkeep,
                            const unsigned int Nsplit,
                            const bool mutual) {
        vector<clstep> leaves(clusters.size(), init_step);
        const float bfactor(std::exp(-delta_fe / ndims));

//#ifdef ENABLE_DEBUG_MACRO
        auto total_frames = static_cast<float>(data.size());
        cout << " HIERARCHICAL FREE ENERGY PLAN " << endl;
        cout << "\tSTEP\tFE\tCUT\tSIM " << endl;
//#endif
        clstep prev_step = init_step;
        clstep step = init_step;
        bool enough_neighbor_lists = true;
        while (enough_neighbor_lists) {
            // Initialize break criteria.
            vector<size_t> nghbrlst_szs(clusters.size(), 0);
            // Initialize data for hierarchical level.
            vector<vector<vector<unsigned int> > > hierarchic_clusters(clusters.size());
            for (unsigned int cluster_idx = 0; cluster_idx < clusters.size(); cluster_idx++) {
                if (clusters[cluster_idx].size() > Nsplit) {
                    Neighbors neighbors_ij;
                    Neighbors second_neighbors_ij;
                    nns::neighbors_from_cluster(neighbors_ij,
                                                clusters[cluster_idx],
                                                data,
                                                step.cut,
                                                step.sim);
                    nghbrlst_szs[cluster_idx] = neighbors_ij.size();

                    vector<vector<unsigned int> > new_clusters;
                    new_clusters = Clustering::Core::algorithm(similarity,
                                                               data,
                                                               neighbors_ij,
                                                               second_neighbors_ij,
                                                               step.cut,
                                                               step.sim,
                                                               Nkeep,
                                                               mutual);

                    // if number of clusters does not increase take the initial cluster
                    if (new_clusters.empty())
                        new_clusters.resize(1, clusters[cluster_idx]);
                    else if (new_clusters.size() == 1)
                        new_clusters[0] = clusters[cluster_idx];

                    for (const auto &new_cluster : new_clusters)
                        hierarchic_clusters[cluster_idx].push_back(new_cluster);
                } else {
                    hierarchic_clusters[cluster_idx].push_back(clusters[cluster_idx]);
                }
            }

            // sorting after parallel loop
            vector<vector<unsigned int> > output;
            for (unsigned int cluster_idx = 0; cluster_idx < hierarchic_clusters.size(); cluster_idx++)
                for (auto new_clusters : hierarchic_clusters[cluster_idx])
                    output.push_back(new_clusters);

            for (int cluster_idx = (hierarchic_clusters.size() - 1); cluster_idx >= 0; cluster_idx--) {
                size_t number = hierarchic_clusters[cluster_idx].size() - 1;
                if (number > 0) {
                    leaves.at(cluster_idx) = step;
                    auto leaves_it = leaves.begin() + cluster_idx;
                    leaves.insert(leaves_it, number, step);
                }
            }
            clusters = output;

            // Number of clustered points
            unsigned int clustered_points(0);
            for (auto &cluster : clusters)
                clustered_points += cluster.size();

//#ifdef ENABLE_DEBUG_MACRO
            // Print a little bit of hierarchical tree info
            std::cout << "STEP " << step.step << " CUT " << step.cut << " SIM " << step.sim << " #clustered "
                      << clustered_points << " (" << 100.0f * static_cast<float>(clustered_points) / total_frames
                      << "%) | Clusters: " << clusters.size();
            for (unsigned int cluster_idx = 0; cluster_idx < hierarchic_clusters.size(); cluster_idx++)
                for (auto const &new_cluster : hierarchic_clusters[cluster_idx])
                    cout << " " << cluster_idx << ":" << new_cluster.size();
            std::cout << std::endl;
//#endif

            // Setting cutoff for next hierarchical level
            prev_step = step;
            step.cut = step.cut * bfactor;
            step.step++;

            // Stopping criteria
            enough_neighbor_lists = std::any_of(nghbrlst_szs.begin(),
                                                nghbrlst_szs.end(),
                                                [Nkeep](size_t count) { return count > 2 * Nkeep; });
        }
//#ifdef ENABLE_DEBUG_MACRO
        std::cout << "Total # frames " << total_frames << std::endl;
//#endif

        return leaves;
    }

    // USER INTERFACE MAPPING
    // Maps the data which was not used in a initial clustering step onto the exiting cluster
    vector<vector<unsigned int> > cluster_mapping(vector<vector<unsigned int> > &clusters,
                                                  vector<vector<float> > &full_data,
                                                  vector<vector<float> > &reduced_data,
                                                  map<unsigned int, unsigned int> &frames,
                                                  vector<clstep> &leaves,
                                                  const unsigned int slice) {

        vector<map<unsigned int, float> > similarity_maps(full_data.size());

        // extract frame ID without slice
        std::set<unsigned int> true_frames;
        for (auto const &frame : frames)
            true_frames.insert(frame.second);

        for (unsigned int cluster_idx = 0; cluster_idx < clusters.size(); cluster_idx++) {

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(cluster_idx, similarity_maps, clusters, full_data, reduced_data, true_frames, leaves)
#endif
            for (size_t frame = 0; frame < full_data.size(); ++frame) {
                if (true_frames.find(frame) != true_frames.end()) { continue; }

                // Obtain single neighbor list of unclustered frame and
                // continue the loop if it is empty, i.e., it was smaller than similarity.
                Neighbors neighbors_from_frame;
                nns::neighbors_from_frame(neighbors_from_frame,
                                          frame,
                                          full_data[frame],
                                          reduced_data,
                                          leaves[cluster_idx].cut,
                                          leaves[cluster_idx].sim);

                if (neighbors_from_frame.count(frame) == 0) { continue; }

                vector<unsigned int> shared_neighbors;
                Clustering::Core::intersection(shared_neighbors,
                                               clusters[cluster_idx],
                                               neighbors_from_frame[frame]);

                if (shared_neighbors.size() >= leaves[cluster_idx].sim) {
                    float sim_f = static_cast<float>(shared_neighbors.size());
                    float size_f = static_cast<float>(clusters[cluster_idx].size());
                    similarity_maps[frame][cluster_idx] = sim_f / size_f;
                }
            }
        }

        map<unsigned int, int> clustered;
        for (size_t frame = 0; frame < full_data.size(); ++frame) {
            if (!(similarity_maps[frame].empty())) {
                auto it = __gnu_parallel::max_element(similarity_maps[frame].begin(),
                                                      similarity_maps[frame].end());
                clustered[frame] = it->first;
            }
        }

        // Current frames and clusters are scales down by slice;
        // so scale it up to push the mapped points into the clusters
        // in the preceding loop.
        for (size_t i = 0; i < clusters.size(); ++i)
            for (size_t j = 0; j < clusters[i].size(); ++j)
                clusters[i][j] = frames[clusters[i][j]];

        for (auto &point : clustered)
            clusters[point.second].push_back(point.first);

        return clusters;
    }

}
