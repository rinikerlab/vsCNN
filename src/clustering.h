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

#ifndef CNN_CLUSTERING_H
#define CNN_CLUSTERING_H

#include <vector>

#include "datatypes.h" // clstep, Neighbors
#include "core.h"      // Clustering::Core::Similarity

using namespace std;

namespace Clustering {

    vector<vector<unsigned int> >
    clustering(Clustering::Core::Similarity similarity,
               vector<vector<float> > &data,
               const float cut,
               const unsigned int sim,
               const int Nkeep,
               const bool mutual);

    // USER INTERFACE HIERARCHICAL CLUSTERING
    // Hierarchical clustering
    vector<clstep>
    hierarchical_clustering(Clustering::Core::Similarity similarity,
                            vector<vector<unsigned int> > &clusters,
                            vector<vector<float> > &data,
                            const clstep init_step,
                            const float delta_fe,
                            const unsigned int ndims,
                            const unsigned int Nkeep,
                            const unsigned int Nsplit,
                            const bool mutual);

    // USER INTERFACE MAPPING
    // Maps the data which was not used in a initial clustering step onto the exiting clusters
    vector<vector<unsigned int> > cluster_mapping(vector<vector<unsigned int> > &clusters,
                                                  vector<vector<float> > &full_data,
                                                  vector<vector<float> > &reduced_data,
                                                  map<unsigned int, unsigned int> &frames,
                                                  vector<clstep> &leaves,
                                                  const unsigned int slice);

};

#endif //CNN_CLUSTERING_H
