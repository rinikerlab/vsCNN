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

#ifndef CLUSTERING_NEIGHBORS_H
#define CLUSTERING_NEIGHBORS_H

#include "datatypes.h"

using namespace std;

namespace nns {

    // Directly obtain the neighbor lists of two
    // frames from tICs and stores it in `neighbors_i`
    // only if the distance is below the cutoff.
    void calc_neighbors(vector<unsigned int> &neighbors_i,  // :output: a neighbor list
                        const vector<vector<float>> &data,        // all input data
                        const vector<float> &ref_point,           // reference point of neighbor list
                        const unsigned int from,            // starting point in data
                        const unsigned int to,              // ending point in data
                        const float cutsquare);

    // Directly calculate neighbor lists if memory is an issue
    void neighbors(Neighbors &neighbors_ij,
                   const vector<vector<float> > &data,
                   const float cut,
                   const unsigned int sim);

    // Directly obtain the neighbor lists of two
    // frames from tICs and stores it in `neighbors_i`
    // only if the distance is below the cutoff.
    void calc_neighbors(vector<unsigned int> &neighbors_i,  // :output: a neighbor list
                        vector<unsigned int> &second_neighbors_i,
                        const vector<vector<float>> &data,        // all input data
                        const vector<float> &ref_point,           // reference point of neighbor list
                        const unsigned int from,            // starting point in data
                        const unsigned int to,              // ending point in data
                        const float cutsquare,
                        const bool mutual);

    // Directly calculate neighbor lists if memory is an issue
    void neighbors(Neighbors &neighbors_ij,
                   Neighbors &second_neighbors_ij,
                   const vector<vector<float> > &data,
                   const float cut,
                   const unsigned int sim,
                   const bool mutual);

    // Extend neighbor lists with increased cut
    void extend_neighbors(Neighbors &neighbors_ij,
                          const vector<vector<float> > &data,
                          const float cut,
                          const unsigned int sim);

    // Prune neighbor lists with decreased cut or increased sim than before
    void prune_neighbors(Neighbors &neighbors_ij,
                         const vector<vector<float> > &data,
                         const float cut,
                         const unsigned int sim);

    ////////////// MAPPING UTILITY ///////////////
    // Obtain neighbor list of one frame
    void neighbors_from_frame(Neighbors &neighbors_ij,
                              unsigned int frame,
                              vector<float> &ref_point,
                              vector<vector<float> > &data,
                              const float cut,
                              const unsigned int sim);

    // Obtain neighbor lists from all frames in cluster
    void neighbors_from_cluster(Neighbors &neighbors_ij,
                                vector<unsigned int> &cluster,
                                vector<vector<float> > &data,
                                const float cut,
                                const unsigned int sim);

    // Obtain neighbor lists from all frames in cluster
    void neighbors_from_cluster(Neighbors &neighbors_ij,
                                Neighbors &second_neighbors_ij,
                                vector<unsigned int> &cluster,
                                vector<vector<float> > &data,
                                const float cut,
                                const unsigned int sim,
                                const bool mutual);

}

#endif //CLUSTERING_NEIGHBORS_H
