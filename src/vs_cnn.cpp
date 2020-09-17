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

#include "core.h"      // Clustering::Core::intersection
#include "geometry.h"  // geometry::regularized_intersection_volume

#include "vs_cnn.h"

namespace Clustering {

    namespace CommonDensity {

        float calc_distance(const vector<float> &vec1, const vector<float> &vec2) {
            const unsigned int ndims(vec1.size());
            float dist(0.0);
#pragma omp simd reduction(+:dist)
            for (unsigned int k = 0; k < ndims; k++) {
                float d(vec1[k] - vec2[k]);
                dist += (d * d);
            }
            return std::sqrt(dist);
        }

        ////////////// CORE UTILITY ///////////////
        bool similarity(vector<vector<float> > &data,
                        const Neighbors &neighbors_ij,
                        const unsigned int refpoint,
                        const unsigned int point,
                        const float cut,
                        const unsigned int sim) {
            vector<unsigned int> shared_neighbors;
            Clustering::Core::intersection(shared_neighbors,
                                           neighbors_ij.at(refpoint),
                                           neighbors_ij.at(point));
            float distance = calc_distance(data[refpoint], data[point]);
            double ivolume = Geometry::regularized_intersection_volume(distance,
                                                                       cut,
                                                                       data[refpoint].size());
            // plus two because of self-contained points
            double density = static_cast<double>(shared_neighbors.size() + 2) / ivolume;
            double simdensity = static_cast<double>(sim); // TODO: Here also plus 2?
            return (density >= simdensity);
        }

    } // end namespace CommonDensity
} // end namespace Clustering
