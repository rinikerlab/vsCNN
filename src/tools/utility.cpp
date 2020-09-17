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

#include <parallel/algorithm>

#include "utility.h"

namespace Clustering {

    namespace Utility {

        void sortNclean(vector<vector<unsigned int> > &clusters,
                        unsigned int Nkeep) {
            __gnu_parallel::sort(clusters.begin(), clusters.end(),
                                 [](const vector<unsigned int> &a, const vector<unsigned int> &b) {
                                     return a.size() > b.size();
                                 });
            clusters.erase(remove_if(clusters.begin(), clusters.end(),
                                     [Nkeep](const vector<unsigned int> &cluster) {
                                         return cluster.size() <= Nkeep;
                                     }),
                           clusters.end());
            clusters.shrink_to_fit();
        }

        ////////////// HIERARCHICAL CLUSTERING UTILITY ///////////////
        vector<clstep>
        hierarchy(const unsigned int nsteps,
                  const float cut,
                  const float delta_cut,
                  const unsigned int sim,
                  const unsigned int delta_sim) {
            vector<clstep> plan;
            for (unsigned int i = 0; i <= nsteps; i++) {
                float new_cut(cut - delta_cut * i);
                unsigned int new_sim(sim + delta_sim * i);
                clstep step(i, new_cut, new_sim);
                plan.push_back(step);
            }
            return plan;
        }

    }
}
