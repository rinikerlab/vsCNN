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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <vector>

#include <iomanip> // std::setprecision

#include "utility.h"
#include "../core.h"
#include "../clustering.h"
#include "../vs_cnn.h"
#include "../cnn.h"

namespace py = pybind11;

py::array
pyclustering(Clustering::Core::Similarity similarity,
             vector<vector<float>> &data,
             const float cut,
             const int sim,
             const int Nkeep,
             const bool mutual) {
    //checking input
    if (data.size() == 0)
        throw std::invalid_argument("The input data is empty.");
    else
        for (auto point : data)
            if (point.size() == 0)
                throw std::invalid_argument("The input data structure has an empty point.");
    if (cut <= 0)
        throw std::invalid_argument("Cutoff radius must be larger than 0.");
    if (sim < 2)
        throw std::invalid_argument("Similarity parameter must be at least 2.");
    if (Nkeep < 2 || Nkeep > data.size())
        throw std::invalid_argument("N_keep must be a value between 2 and the size of the data");

    vector<vector<unsigned int>> clusters;
    clusters = Clustering::clustering(similarity,
                                      data,
                                      cut,
                                      sim,
                                      Nkeep,
                                      mutual);

    // Some Clustering Result printing
    const unsigned int total_frames = data.size();
    std::ostringstream msg;
    msg << "CLUSTERING RESULTS \n";
    msg << "-------------------\n";
    msg << "Total # Data Points: " << total_frames << "\n";
    msg << "Cut: " << cut << "\tSim: " << sim << "\n\n";
    msg << "\tClusters\tsize\tratio\n";
    int clusterID = 0;
    float total = 0;
    float all_points = static_cast<float>(total_frames);
    for (auto cluster: clusters) {
        float clsize = static_cast<float>(cluster.size());
        msg << "\t" << clusterID << "\t" << cluster.size() << "\t" << 100.0f * (clsize / all_points) << "%\n";
        total += clsize;
        clusterID++;
    }
    msg << "Remaining noise is : " << (100.0f * (1 - (total / all_points))) << "%\n\n";
    py::print(msg.str());

    // Deep casting - Messy?!
    std::vector<py::array> tmpClusters;
    for (auto cluster: clusters) {
        py::array pCluster = py::cast(cluster);
        tmpClusters.push_back(pCluster);
    }
    py::array returnClusters = py::cast(tmpClusters);

    return returnClusters;
}

pybind11::array
volumescaled_common_nearest_neighbor(vector<vector<float>> data,
                                     const float cut,
                                     const int sim,
                                     const int Nkeep,
                                     const bool mutual) {
    return pyclustering(Clustering::CommonDensity::similarity,
                        data,
                        cut,
                        sim,
                        Nkeep,
                        mutual);
}

pybind11::array
common_nearest_neighbor(vector<vector<float>> data,
                        float cut,
                        int sim,
                        int Nkeep,
                        bool mutual) {
    return pyclustering(Clustering::CommonNearestNeighbor::similarity,
                        data,
                        cut,
                        sim,
                        Nkeep,
                        mutual);
}

pybind11::array
hierarchical_clustering(Clustering::Core::Similarity similarity,
                        vector<vector<float>> &data,
                        const float cut,
                        const int sim,
                        const float delta_fe,
                        const unsigned int Nkeep,
                        const unsigned int Nsplit,
                        const bool mutual) {
    //checking input
    if (data.size() == 0)
        throw std::invalid_argument("The input data is empty.");
    else
        for (auto point : data)
            if (point.size() == 0)
                throw std::invalid_argument("The input data structure has an empty point.");
    if (cut <= 0)
        throw std::invalid_argument("Cutoff radius must be larger than 0.");
    if (sim < 2)
        throw std::invalid_argument("Similarity parameter must be at least 2.");
    if (Nkeep < 2 || Nkeep > data.size())
        throw std::invalid_argument("N_keep must be a value between 2 and the size of the data");

    vector<vector<unsigned int> > clusters;
    clusters = Clustering::clustering(similarity,
                                      data,
                                      cut,
                                      sim,
                                      Nkeep,
                                      mutual);

    // Cluster hierarchically
    clstep init_step(0, cut, sim);
    vector<clstep> leaves;
    leaves = Clustering::hierarchical_clustering(similarity,
                                                 clusters,
                                                 data,
                                                 init_step,
                                                 delta_fe,
                                                 data[0].size(),
                                                 Nkeep,
                                                 Nsplit,
                                                 mutual);

    float total = 0;
    float all = static_cast<float>(data.size());
    std::ostringstream msg;
    msg << " HIERARCHICAL CLUSTERING RESULTS " << endl;
    msg << std::setprecision(2);
    for (size_t idx = 0; idx < clusters.size(); idx++) {
        float clsize = static_cast<float>(clusters[idx].size());
        msg << idx << "\t" << clusters[idx].size() << "\t" << 100.0f * clsize / all << "%";
        msg << "\tat Step: " << leaves[idx].step;
        msg << "\twith Cut: " << leaves[idx].cut;
        msg << "\tand Sim: " << leaves[idx].sim << endl;
        total += clsize;
    }
    msg << "Remaining noise is : " << 100.0f * (1.0 - total / all) << "%\n" << endl;
    py::print(msg.str());

    // Deep casting - Messy?!
    std::vector<py::array> tmpClusters;
    for (auto cluster: clusters) {
        py::array pCluster = py::cast(cluster);
        tmpClusters.push_back(pCluster);
    }
    py::array returnClusters = py::cast(tmpClusters);
    return returnClusters;
}

pybind11::array
hierarchical_volumescaled_common_nearest_neighbor(vector<vector<float> > data,
                                                  const float cut,
                                                  const int sim,
                                                  const float delta_fe,
                                                  const unsigned int Nkeep,
                                                  const unsigned int Nsplit,
                                                  const bool mutual) {
    return hierarchical_clustering(Clustering::CommonDensity::similarity,
                                   data,
                                   cut,
                                   sim,
                                   delta_fe,
                                   Nkeep,
                                   Nsplit,
                                   mutual);
}

pybind11::array
hierarchical_common_nearest_neighbor(vector<vector<float> > data,
                                     const float cut,
                                     const int sim,
                                     const float delta_fe,
                                     const unsigned int Nkeep,
                                     const unsigned int Nsplit,
                                     const bool mutual) {
    return hierarchical_clustering(Clustering::CommonNearestNeighbor::similarity,
                                   data,
                                   cut,
                                   sim,
                                   delta_fe,
                                   Nkeep,
                                   Nsplit,
                                   mutual);
}
