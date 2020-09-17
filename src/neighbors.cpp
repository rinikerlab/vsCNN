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

#include <omp.h>
#include <parallel/algorithm>

#include "neighbors.h"

namespace nns {

    void calc_neighbors(vector<unsigned int> &neighbors_i,
                        const vector<vector<float>> &data,
                        const vector<float> &ref_point,
                        const unsigned int from,
                        const unsigned int to,
                        const float cutsquare) {
        const unsigned int ndims(data[0].size());

        for (unsigned int j = from; j < to; ++j) {

            // Calculate distance
            float dist(0.0);
#pragma omp simd reduction(+:dist)
            for (unsigned int k = 0; k < ndims; ++k) {
                float d(ref_point[k] - data[j][k]);
                dist += (d * d);
            }

            // Only add frame to neighbor list if the
            // distance is smaller than the cut off
            if (dist <= cutsquare) {
                neighbors_i.push_back(j);
            }
        }
    }

    void neighbors(Neighbors &neighbors_ij,
                   const vector<vector<float> > &data,
                   const float cut,
                   const unsigned int sim) {

        const unsigned int num_frames(data.size());
        float cutsquare(cut * cut);

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(data, neighbors_ij, cutsquare)
#endif
        for (unsigned int i = 0; i < num_frames; ++i) {

            // Call calc_neighbors twice to prevent adding
            // the i'th frame to its own neighbor list
            vector<unsigned int> neighbors_i;
            calc_neighbors(neighbors_i, data, data[i], 0, i, cutsquare);
            calc_neighbors(neighbors_i, data, data[i], i + 1, num_frames, cutsquare);

            // Only add neighbor list to the std::map if it
            // comprises more than similarity + 1 neighbors
#pragma omp critical
            if (neighbors_i.size() >= sim + 1) { neighbors_ij[i] = neighbors_i; }
        }
    }

    void calc_neighbors(vector<unsigned int> &neighbors_i,
                        vector<unsigned int> &second_neighbors_i,
                        const vector<vector<float>> &data,
                        const vector<float> &ref_point,
                        const unsigned int from,
                        const unsigned int to,
                        const float cutsquare,
                        const bool mutual) {
        const unsigned int ndims(data[0].size());
        const float fourcutsquare(4.0f * cutsquare);

        for (unsigned int j = from; j < to; ++j) {

            // Calculate distance
            float dist(0.0);
#pragma omp simd reduction(+:dist)
            for (unsigned int k = 0; k < ndims; ++k) {
                float d(ref_point[k] - data[j][k]);
                dist += (d * d);
            }

            // Only add frame to neighbor list if the
            // distance is smaller than the cut off
            if (dist <= cutsquare) {
                neighbors_i.push_back(j);
            } else if (!mutual && dist > cutsquare && dist <= fourcutsquare) {
                second_neighbors_i.push_back(j);
            }
        }
    }

    void neighbors(Neighbors &neighbors_ij,
                   Neighbors &second_neighbors_ij,
                   const vector<vector<float> > &data,
                   const float cut,
                   const unsigned int sim,
                   const bool mutual) {

        const unsigned int num_frames(data.size());
        float cutsquare(cut * cut);

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(data, neighbors_ij, second_neighbors_ij, cutsquare)
#endif
        for (unsigned int i = 0; i < num_frames; ++i) {

            // Call calc_neighbors twice to prevent adding
            // the i'th frame to its own neighbor list
            vector<unsigned int> neighbors_i;
            vector<unsigned int> second_neighbors_i;
            calc_neighbors(neighbors_i, second_neighbors_i, data, data[i], 0, i, cutsquare, mutual);
            calc_neighbors(neighbors_i, second_neighbors_i, data, data[i], i + 1, num_frames, cutsquare, mutual);

            // Only add neighbor list to the std::map if it
            // comprises more than similarity + 1 neighbors
#pragma omp critical
            if (neighbors_i.size() >= sim + 1) {
                neighbors_ij[i] = neighbors_i;
                if (!mutual && second_neighbors_i.size() != 0) {
                    second_neighbors_ij[i] = second_neighbors_i;
                }
            }
        }
    }

    void add_neighbors(std::vector<unsigned int> &neighbors_i,
                       const std::vector<std::vector<float>> &data,
                       const vector<float> &ref_point,
                       const unsigned int from,
                       const unsigned int to,
                       const float cutsquare) {
        const unsigned int ndims(data[0].size());

        std::vector<unsigned int> to_consider(to-from);
        std::iota(to_consider.begin(), to_consider.end(), from);
        for (auto j : neighbors_i)
            to_consider.erase(std::remove(to_consider.begin(),
                                          to_consider.end(),
                                          j));

        //for (unsigned int j = from; j < to; ++j) {
        for (auto j : to_consider) {
            //if (__gnu_parallel::find(neighbors_i.begin(), neighbors_i.end(), j) == neighbors_i.end())
            //    continue;

            // Calculate distance
            float dist(0.0);
#pragma omp simd reduction(+:dist)
            for (unsigned int k = 0; k < ndims; ++k) {
                float d(ref_point[k] - data[j][k]);
                dist += (d * d);
            }

            // Only add frame to neighbor list if the
            // distance is smaller than the cut off
            if (dist <= cutsquare) {
                neighbors_i.push_back(j);
            }
        }
    }

    void extend_neighbors(Neighbors &neighbors_ij,
                          const vector<vector<float> > &data,
                          const float cut,
                          const unsigned int sim) {

        const unsigned int num_frames(data.size());
        float cutsquare(cut * cut);

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(data, neighbors_ij, cutsquare)
#endif
        for (unsigned int i = 0; i < num_frames; ++i) {

            // Call calc_neighbors twice to prevent adding
            // the i'th frame to its own neighbor list
            vector<unsigned int> neighbors_i;
            add_neighbors(neighbors_i, data, data[i], 0, i, cutsquare);
            add_neighbors(neighbors_i, data, data[i], i + 1, num_frames, cutsquare);

            // Only add neighbor list to the std::map if it
            // comprises more than similarity + 1 neighbors
#pragma omp critical
            if (neighbors_i.size() >= sim + 1) { neighbors_ij[i] = neighbors_i; }
        }
    }

    void trim_neighbors(std::vector<unsigned int> &neighbors_i,
                        const std::vector<std::vector<float>> &data,
                        const std::vector<float> &ref_point,
                        const float cutsquare) {
        const unsigned int ndims(data[0].size());

        std::vector<unsigned int> eraser;
        for (unsigned int point = 0; point < neighbors_i.size(); ++point) {
            unsigned int j = neighbors_i.at(point);

            // Calculate distance
            float dist(0.0);
#pragma omp simd reduction(+:dist)
            for (unsigned int k = 0; k < ndims; ++k) {
                float d(ref_point[k] - data[j][k]);
                dist += (d * d);
            }

            // Only add frame to neighbor list if the
            // distance is smaller than the cut off
#pragma omp critical
            if (dist > cutsquare) { eraser.push_back(j); }
        }

        for (auto j : eraser) {
            neighbors_i.erase(std::remove(neighbors_i.begin(),
                                          neighbors_i.end(),
                                          j));
        }
        neighbors_i.shrink_to_fit();
    }

    void prune_neighbors(Neighbors &neighbors_ij,
                         const vector<vector<float> > &data,
                         const float cut,
                         const unsigned int sim) {

        float cutsquare(cut * cut);

        std::vector<unsigned int> keys;
        for (auto neighbors_i = neighbors_ij.begin(); neighbors_i != neighbors_ij.end(); ++neighbors_i)
            keys.push_back(neighbors_i->first);

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(neighbors_ij, data, cutsquare, keys)
#endif
        for (size_t i = 0; i < keys.size(); ++i)
            trim_neighbors(neighbors_ij[keys[i]], data, data[keys[i]], cutsquare);

        for (unsigned int key : keys)
            if (neighbors_ij[key].size() < sim + 1)
                neighbors_ij.erase(key);
    }

    ////////////// MAPPING UTILITY ///////////////
    void neighbors_from_frame(Neighbors &neighbors_ij,
                              unsigned int frame,
                              vector<float> &ref_point,
                              vector<vector<float> > &data,
                              const float cut,
                              const unsigned int sim) {
        const unsigned int num_frames(data.size());
        float cutsquare(cut * cut);

        vector<unsigned int> neighbors_i;
        calc_neighbors(neighbors_i, data, ref_point, 0, num_frames, cutsquare);

        // Only add neighbor list to the std::map if it
        // comprises more than similarity + 1 neighbors
        if (neighbors_i.size() >= sim + 1) { neighbors_ij[frame] = neighbors_i; }
    }

    void neighbors_from_cluster(Neighbors &neighbors_ij,
                                vector<unsigned int> &cluster,
                                vector<vector<float> > &data,
                                const float cut,
                                const unsigned int sim) {
        const unsigned int num_frames(data.size());
        float cutsquare(cut * cut);

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(data, cluster, neighbors_ij, cutsquare)
#endif
        for (unsigned int i = 0; i < cluster.size(); i++) {
            unsigned int refpoint(cluster[i]);

            // Call calc_neighbors twice to prevent adding
            // the i'th frame to its own neighbor list
            vector<unsigned int> neighbors_i;
            vector<unsigned int> second_neighbors_i;
            calc_neighbors(neighbors_i,
                           data, data[refpoint],
                           0, refpoint,
                           cutsquare);
            calc_neighbors(neighbors_i,
                           data, data[refpoint],
                           refpoint + 1, num_frames,
                           cutsquare);

            // Only add neighbor list to the std::map if it
            // comprises more than similarity + 1 neighbors
#pragma omp critical
            if (neighbors_i.size() >= sim + 1) { neighbors_ij[refpoint] = neighbors_i; }
        }
    }

    void neighbors_from_cluster(Neighbors &neighbors_ij,
                                Neighbors &second_neighbors_ij,
                                vector<unsigned int> &cluster,
                                vector<vector<float> > &data,
                                const float cut,
                                const unsigned int sim,
                                const bool mutual) {
        const unsigned int num_frames(data.size());
        float cutsquare(cut * cut);

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(data, cluster, neighbors_ij, second_neighbors_ij, cutsquare)
#endif
        for (unsigned int i = 0; i < cluster.size(); i++) {
            unsigned int refpoint(cluster[i]);

            // Call calc_neighbors twice to prevent adding
            // the i'th frame to its own neighbor list
            vector<unsigned int> neighbors_i;
            vector<unsigned int> second_neighbors_i;
            calc_neighbors(neighbors_i,
                           second_neighbors_i,
                           data, data[refpoint],
                           0, refpoint,
                           cutsquare, mutual);
            calc_neighbors(neighbors_i,
                           second_neighbors_i,
                           data, data[refpoint],
                           refpoint + 1, num_frames,
                           cutsquare, mutual);

            // Only add neighbor list to the std::map if it
            // comprises more than similarity + 1 neighbors
#pragma omp critical
            if (neighbors_i.size() >= sim + 1) {
                neighbors_ij[refpoint] = neighbors_i;
                if (second_neighbors_i.size() != 0) {
                    second_neighbors_ij[i] = second_neighbors_i;
                }
            }
        }
    }

} /* end of namespace */