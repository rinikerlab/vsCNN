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

#ifndef CNN_CLUSTERING_COMMONDENSITY_H
#define CNN_CLUSTERING_COMMONDENSITY_H

#include <vector>

#include "neighbors.h"

using namespace std;

namespace Clustering {

    namespace CommonDensity {

        float calc_distance(const vector<float> &vec1, const vector<float> &vec2);

        ////////////// CORE UTILITY ///////////////
        bool similarity(vector<vector<float> > &data,
                        const Neighbors &neighbors_ij,
                        const unsigned int refpoint,
                        const unsigned int point,
                        const float cut,
                        const unsigned int sim);

    } // end namespace CommonDensity
} // end namespace Clustering

#endif //CNN_CLUSTERING_COMMONDENSITY_H
