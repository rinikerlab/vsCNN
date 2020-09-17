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

#ifndef CLUSTERING_CORE_H
#define CLUSTERING_CORE_H

#include <iostream>

#include <cstdlib>
#include <cmath>
#include <vector>
#include <utility>

#include "datatypes.h"
#include "neighbors.h"

using namespace std;

namespace Clustering {

    namespace Core {

        typedef bool Similarity(vector<vector<float> > &data,
                                const Neighbors &neighbors_ij,
                                const unsigned int refpoint,
                                const unsigned int point,
                                const float cut,
                                const unsigned int sim);

        typedef bool (*simptr)(vector<vector<float> > &data,
                               const Neighbors &neighbors_ij,
                               const unsigned int refpoint,
                               const unsigned int point,
                               const float cut,
                               const unsigned int sim);

        ////////////// CORE UTILITY ///////////////
        // Wrapper for the STL intersection algorithm
        void intersection(vector<unsigned int> &out,
                          const vector<unsigned> &list1,
                          const vector<unsigned> &list2);

        ////////////// CORE UTILITY ///////////////
        // CLUSTERING CORE FUNCTION
        // Intersects neighbor lists from points in `input` with the
        // NOT YET clustered `refpoint` to find the number of shared
        // neighbors, i.e., their similarity. Outputs the clustered
        // points and `refpoint` into a NEW cluster in `clusters`
        // and a NEW cluster label in`clustered`.
        void
        similarity_unclustered(Similarity similarity,
                               vector<vector<float> > &data,
                               map<unsigned int, int> &clustered,
                               vector<vector<unsigned int> > &clusters,
                               const Neighbors &neighbors_ij,
                               const vector<unsigned int> &input,
                               const unsigned int refpoint,
                               const float cut,
                               const unsigned int sim);

        // CLUSTERING CORE FUNCTION
        // Intersects neighbor lists from points in `input` with the
        // YET clustered `refpoint` to find the number of shared
        // neighbors, i.e., their similarity. Outputs the clustered
        // points and `refpoint` into the cluster of `refpoint` in
        // `clusters` and associated cluster label of `refpoint`
        // to the newly clustered point in`clustered`.
        void similarity_clustered(Similarity similarity,
                                  vector<vector<float> > &data,
                                  map<unsigned int, int> &clustered,
                                  vector<vector<unsigned int> > &clusters,
                                  const Neighbors &neighbors_ij,
                                  const vector<unsigned int> &input,
                                  const unsigned int refpoint,
                                  const float cut,
                                  const unsigned int sim);

        // USER INTERFACE CLUSTERING
        // Clusters the data
        vector<vector<unsigned int> >
        algorithm(Similarity similarity,
                  vector<vector<float> > &data,
                  Neighbors &neighbor_ij,
                  Neighbors &second_neighbor_ij,
                  const float cut,
                  const unsigned int sim,
                  const unsigned int Nkeep,
                  const bool mutual);

    } // end namespace Core
} // end namespace Clustering

#endif //CLUSTERING_CORE_H
