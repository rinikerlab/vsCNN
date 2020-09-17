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

#include "discretization.h"

namespace Clustering {

    namespace Discretization {

        // USER INTERFACE DISCRETIZATION
        // Retrieves the discretized tajectories
        vector<vector<int> > dtrajs(const vector<vector<unsigned int> > &clusters,
                                    const vector<unsigned int> &traj_shapes) {

            vector<vector<int> > dtrajs;
            for (auto shape : traj_shapes)
                dtrajs.push_back(vector<int>(shape, -1));

            for (int i = 0; i < clusters.size(); i++) {
                int cn = i; // cluster number
                for (auto idx : clusters[i]) {

                    unsigned int concat = 0;
                    unsigned int traj_idx = 0;
                    while (concat <= idx && traj_idx < traj_shapes.size()) {
                        concat += traj_shapes[traj_idx];
                        ++traj_idx;
                    }
                    --traj_idx;
                    concat -= traj_shapes[traj_idx];

                    unsigned int frame_idx = idx - concat;
                    dtrajs[traj_idx][frame_idx] = cn;
                }
            }

            return dtrajs;
        }

    }
}
