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

#include "io.h"

bool fexists(const string &filename) {
    ifstream filestream(filename);
    bool ret = filestream.good();
    filestream.close();
    return ret;
}

std::string backup_file(const std::string outfile) {
    std::string ofile = outfile;
    int lastslash = outfile.find_last_of('/');
    std::string path = outfile.substr(0, lastslash + 1);
    std::string file = outfile.substr(lastslash + 1);

    int lastdot = file.find_last_of('.');
    std::string name = file.substr(0, lastdot + 1);
    std::string type = file.substr(lastdot);

    int i = 0;
    ofile = outfile;
    while (fexists(ofile)) {
        ofile = path + name;
        ofile += std::to_string(i) + file.substr(lastdot);
        i++;
    }
    return ofile;
}

vector<vector<float>> read_tICs(vector<unsigned int> &traj_shapes,
                                const string &filename,
                                const unsigned int ntrajs,
                                const unsigned int ndims,
                                const unsigned int slice) {
    vector<float> tica;
    vector<unsigned long> tica_shape;
    npy::LoadArrayFromNumpy(filename, tica_shape, tica);

    traj_shapes[0] = tica_shape[0]; // number of trajs
    traj_shapes[1] = tica_shape[1]; // number of frames
    traj_shapes[2] = tica_shape[2]; // tIC vector dimensionality
    if (ntrajs < traj_shapes[0])
        traj_shapes[0] = ntrajs;
    if (slice > 1)
        traj_shapes[1] /= slice;
    if (ndims < tica_shape[2])
        traj_shapes[2] = ndims;

    vector<vector<float>> tICs(traj_shapes[0] * traj_shapes[1], vector<float>(tica_shape[2], 0.0));

    unsigned int dim = tica_shape[2];
    for (unsigned int frame = 0; frame < traj_shapes[0] * traj_shapes[1]; frame++) {
        for (unsigned component = 0; component < tica_shape[2]; component++) {
            int tica_position = frame * slice * dim + component;
            tICs[frame][component] = tica[tica_position];
        }
    }

    if (ndims < tica_shape[2])
        for (unsigned int frame = 0; frame < traj_shapes[0] * traj_shapes[1]; frame++)
            tICs[frame].resize(ndims);

    return tICs;
}

std::vector<std::vector<float>> read_tICs(std::vector<unsigned int> &traj_shapes,
                                          std::map<unsigned int, unsigned int> &frames,
                                          std::vector<unsigned int> &shapes,
                                          const std::string &filename,
                                          const unsigned int ntrajs,
                                          const unsigned int ndims,
                                          const unsigned int slice) {
    // Obtain trajectory shape information
    unsigned int lastdot = filename.find_last_of('.');
    string sfilename = filename.substr(0, lastdot) + "-shape.npy";
    vector<unsigned long> shapes_shape;
    npy::LoadArrayFromNumpy(sfilename, shapes_shape, shapes);

    // Obtain tICA
    vector<float> tica;
    vector<unsigned long> tica_shape;
    npy::LoadArrayFromNumpy(filename, tica_shape, tica);

    traj_shapes[0] = shapes.size(); // number of trajs
    if (ntrajs < traj_shapes[0])
        traj_shapes[0] = ntrajs;
    traj_shapes[1] = tica_shape[1]; // number of frames
    vector<unsigned int> sliced_shapes(shapes);
    if (slice > 1) {
        traj_shapes[1] = 0;
        for (int i = 0; i < shapes.size(); ++i) {
            unsigned int sliced_shape = ((int) shapes[i] / slice);// + 1);
            traj_shapes[1] += sliced_shape;
            sliced_shapes[i] = sliced_shape;
        }
    }
    traj_shapes[2] = tica_shape[2]; // tIC vector dimensionality
    if (ndims < tica_shape[2])
        traj_shapes[2] = ndims;

    vector<vector<float>> tICs(traj_shapes[1], vector<float>(tica_shape[2], 0.0));

    unsigned int dim = tica_shape[2];
    unsigned int traj_begin = 0;
    unsigned int sliced_traj_begin = 0;
    for (unsigned int traj = 0; traj < traj_shapes[0]; ++traj) {

        unsigned int shape = shapes[traj];
        unsigned int sliced_shape = sliced_shapes[traj];
        for (unsigned int frame = 0; frame < sliced_shape; ++frame) {

            for (unsigned int component = 0; component < tica_shape[2]; ++component) {
                unsigned int tica_position = (traj_begin + frame * slice) * dim + component;
                tICs[sliced_traj_begin + frame][component] = tica[tica_position];
            }
            frames[sliced_traj_begin + frame] = traj_begin + frame * slice;
        }
        traj_begin += shape;
        sliced_traj_begin += sliced_shape;
    }
    shapes = sliced_shapes;

    if (ndims < tica_shape[2])
        for (unsigned int frame = 0; frame < traj_shapes[1]; frame++)
            tICs[frame].resize(ndims);

    return tICs;
}

unsigned int get_tICs(vector<vector<float> > &tICs,
                      std::map<unsigned int, unsigned int> &frames,
                      vector<unsigned int> &shapes,
                      vector<unsigned int> &traj_shapes,
                      const string &filename,
                      const unsigned int ntrajs,
                      const unsigned int ndims,
                      const unsigned int slice) {
    unsigned int total_frames = 0;
    if (fexists(filename)) {
        if (fexists(filename.substr(0, filename.find_last_of('.')) + "-shape.npy")) {
            tICs = read_tICs(traj_shapes, frames, shapes, filename, ntrajs, ndims, slice);
            for (auto shape : shapes)
                total_frames += shape;
        } else {
            tICs = read_tICs(traj_shapes, filename, ntrajs, ndims, slice);
            total_frames = traj_shapes[0] * traj_shapes[1];
        }
    } else {
        std::cerr << "Input data file does not exist. Did you forget .npy extension?" << std::endl;
        exit(EXIT_FAILURE);
    }

    return total_frames;
}

void write_clusters(const string &cfilename,
                    const vector<vector<unsigned int>> &clusters,
                    const vector<clstep> &leaves) {

    vector<unsigned int> clusters_accum;
    vector<unsigned int> shapes_accum;
    vector<unsigned int> steps_accum;
    vector<float> cuts_accum;
    vector<unsigned int> sims_accum;

    // Accumulate vectors
    for (auto &cluster : clusters) {
        shapes_accum.push_back(cluster.size());
        clusters_accum.insert(clusters_accum.end(), cluster.begin(), cluster.end());
    }
    for (auto &leaf : leaves) {
        steps_accum.push_back(leaf.step);
        cuts_accum.push_back(leaf.cut);
        sims_accum.push_back(leaf.sim);
    }

    unsigned int n_dims = 1;

    unsigned int lastdot = cfilename.find_last_of('.');
    std::string name = cfilename.substr(0, lastdot);
    std::string type = cfilename.substr(lastdot);

    std::string hcfilename = cfilename;
    std::string hsfilename = name + "-shape" + type;
    std::string llfilename = name + "-leaves" + type;
    std::string lcfilename = name + "-leaves-cut" + type;
    std::string lsfilename = name + "-leaves-sim" + type;

    const long unsigned shape_clusters[] = {clusters_accum.size()};
    const long unsigned shape_shapes[] = {shapes_accum.size()};
    const long unsigned shape_steps[] = {steps_accum.size()};
    const long unsigned shape_cuts[] = {cuts_accum.size()};
    const long unsigned shape_sims[] = {sims_accum.size()};

    npy::SaveArrayAsNumpy<unsigned int>(hcfilename, false, n_dims, shape_clusters, clusters_accum);
    npy::SaveArrayAsNumpy<unsigned int>(hsfilename, false, n_dims, shape_shapes, shapes_accum);
    npy::SaveArrayAsNumpy<unsigned int>(llfilename, false, n_dims, shape_steps, steps_accum);
    npy::SaveArrayAsNumpy<float>(lcfilename, false, n_dims, shape_cuts, cuts_accum);
    npy::SaveArrayAsNumpy<unsigned int>(lsfilename, false, n_dims, shape_sims, sims_accum);
}

vector<vector<unsigned int>> read_clusters(vector<clstep> &leaves,
                                           const string &cfilename) {

    leaves.resize(0);

    unsigned int lastdot = cfilename.find_last_of('.');
    std::string hcfilename = cfilename.substr(0, lastdot) + ".npy";
    std::string hsfilename = cfilename.substr(0, lastdot) + "-shape.npy";
    std::string llfilename = cfilename.substr(0, lastdot) + "-leaves.npy";
    std::string lcfilename = cfilename.substr(0, lastdot) + "-leaves-cut.npy";
    std::string lsfilename = cfilename.substr(0, lastdot) + "-leaves-sim.npy";

    vector<unsigned int> clusters_accum;
    vector<unsigned int> shapes_accum;
    vector<unsigned int> steps_accum;
    vector<float> cuts_accum;
    vector<unsigned int> sims_accum;

    vector<unsigned long> shape_clusters;
    vector<unsigned long> shape_shapes;
    vector<unsigned long> shape_steps;
    vector<unsigned long> shape_cuts;
    vector<unsigned long> shape_sims;

    npy::LoadArrayFromNumpy(hcfilename, shape_clusters, clusters_accum);
    npy::LoadArrayFromNumpy(hsfilename, shape_shapes, shapes_accum);
    npy::LoadArrayFromNumpy(llfilename, shape_steps, steps_accum);
    npy::LoadArrayFromNumpy(lcfilename, shape_cuts, cuts_accum);
    npy::LoadArrayFromNumpy(lsfilename, shape_sims, sims_accum);

    vector<vector<unsigned int>> clusters(0);
    unsigned long i = 0;
    unsigned long cn = 0;
    while (i < shape_clusters[0]) {
        vector<unsigned int> cluster(0);
        for (unsigned int j = 0; j < shapes_accum[cn]; j++) {
            cluster.push_back(clusters_accum[i]);
            i++;
        };

        clusters.push_back(cluster);
        cn++;
    }

    for (unsigned int j = 0; j < cuts_accum.size(); j++)
        leaves.emplace_back(clstep(steps_accum[j], cuts_accum[j], sims_accum[j]));

    return clusters;
}

void write_dtrajs(const string &dtraj_filename, vector<vector<int> > &dtrajs) {

    // Accumulate vectors
    vector<int> dtrajs_accum;
    vector<unsigned int> shapes_accum;
    for (auto const &dtraj : dtrajs) {
        shapes_accum.push_back(dtraj.size());
        dtrajs_accum.insert(dtrajs_accum.end(), dtraj.begin(), dtraj.end());
    }

    unsigned int n_dims = 1;

    unsigned int lastdot = dtraj_filename.find_last_of('.');
    std::string name = dtraj_filename.substr(0, lastdot);
    std::string sfilename = name + "-shape.npy";

    const long unsigned shape_clusters[] = {dtrajs_accum.size()};
    const long unsigned shape_shapes[] = {shapes_accum.size()};
    npy::SaveArrayAsNumpy<int>(dtraj_filename, false, n_dims, shape_clusters, dtrajs_accum);
    npy::SaveArrayAsNumpy<unsigned int>(sfilename, false, n_dims, shape_shapes, shapes_accum);
}

