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

#ifndef CNN_IO_H
#define CNN_IO_H

#include <vector>
#include <string>

#include "external/npy.h"

#include "../datatypes.h"

using namespace std;

bool fexists(const string &filename);

std::string backup_file(const std::string outfile);

vector<vector<float>> read_tICs(vector<unsigned int> &traj_shapes,
                                const string &filename,
                                const unsigned int ntrajs,
                                const unsigned int ndims,
                                const unsigned int slice);

vector<vector<float>> read_tICs(vector<unsigned int> &traj_shapes,
                                std::map<unsigned int, unsigned int> &frames,
                                vector<unsigned int> &shapes,
                                const string &filename,
                                const unsigned int ntrajs,
                                const unsigned int ndims,
                                const unsigned int slice);

unsigned int get_tICs(vector<vector<float> > &tICs,
                      std::map<unsigned int, unsigned int> &frames,
                      vector<unsigned int> &shapes,
                      vector<unsigned int> &traj_shapes,
                      const string &filename,
                      const unsigned int ntrajs,
                      const unsigned int ndims,
                      const unsigned int slice);

void write_clusters(const string &cfilename,
                    const vector<vector<unsigned int> > &clusters,
                    const vector<clstep> &leaves);

vector<vector<unsigned int>> read_clusters(vector<clstep> &leaves,
                                           const string &cfilename);

void write_dtrajs(const string &dtraj_filename,
                  vector<vector<int> > &dtrajs);

template<class T>
void write_to_npy(const string &filename,
                  const vector<T> &data) {
    unsigned int n_dims = 1;
    const long unsigned shape[] = {data.size()};
    npy::SaveArrayAsNumpy<T>(filename, false, n_dims, shape, data);
}

#endif //CNN_IO_H
