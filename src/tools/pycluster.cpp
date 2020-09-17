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

#ifndef CLUSTERING_PYCLUSTERING_H
#define CLUSTERING_PYCLUSTERING_H

#include <pybind11/pybind11.h>

#include <vector>

#include "pywrapper.h"

namespace py = pybind11;

PYBIND11_MODULE(pycomdensity, m) {
    m.doc() = "pycomdensity Package\n This package provides two closely related density based clustering Algorithms and a hierarchical clustering approach.";
    m.attr("version");

    m.def("volumescaled_common_nearest_neighbor",
          &volumescaled_common_nearest_neighbor,
          py::return_value_policy::automatic,
          "This function performs the vs-CNN clustering.\n"
          "\n"
          "Parameters\n"
          "-----------\n"
          "data: Iterable[Iterable[Number]]\n"
          "\tis the data to be clustered.\n"
          "cutoff: float\n"
          "\tis the cutoff radius for neighborhood calculation.\n"
          "similarity: int\n"
          "\tis the amount of mutual neighbors.\n"
          "Nkeep: int, optional\n"
          "\tis the minimum number of data points that a cluster must contain to be kept (default: 2).\n"
          "mutual: bool, optional (currently redundant option; False not possible at the moment)\n"
          "\trequires that the two considered points are mutual neighbors (default: True).\n"
          "\n"
          "RETURNS\n"
          "-------\n"
          "numpy.Array(numpy.Array(float)):\n"
          "\tThe clusters as arrays of data point indices. (np.Array(cluster(frame))\n"
          "\n",
          py::arg("data"),
          py::arg("cutoff"),
          py::arg("similarity"),
          py::arg("Nkeep") = 2,
          py::arg("mutual") = true);

    m.def("common_nearest_neighbor",
          &common_nearest_neighbor,
          py::return_value_policy::automatic,
          "This function performs the CNN clustering.\n"
          "\n"
          "Parameters\n"
          "-----------\n"
          "data: Iterable[Iterable[Number]]\n"
          "\tis the data to be clustered.\n"
          "cutoff: float\n"
          "\tis the cutoff radius for neighborhood calculation.\n"
          "similarity: int\n"
          "\tis the amount of mutual neighbors.\n"
          "Nkeep: int, optional\n"
          "\tis the minimum number of data points that a cluster must contain to be kept (default: 2).\n"
          "mutual: bool, optional (currently redundant option; False not possible at the moment)\n"
          "\trequires that the two considered points are mutual neighbors (default: True).\n"
          "\n"
          "RETURNS\n"
          "-------\n"
          "numpy.Array(numpy.Array(float)):\n"
          "\tThe clusters as arrays of data point indices. (np.Array(cluster(frame))\n"
          "\n",
          py::arg("data"),
          py::arg("cutoff"),
          py::arg("similarity"),
          py::arg("Nkeep") = 2,
          py::arg("mutual") = true);

    m.def("hierarchical_volumescaled_common_nearest_neighbor",
          &hierarchical_volumescaled_common_nearest_neighbor,
          py::return_value_policy::automatic,
          "This function applies the hierarchical approach for the vs-CNN clustering.\n"
          "\n"
          "Parameters\n"
          "-----------\n"
          "data: Iterable[Iterable[Number]]\n"
          "\tis the input data to be clustered.\n"
          "cutoff: float\n"
          "\tis the initial cutoff radius for neighborhood calculation.\n"
          "similarity: int\n"
          "\tis the amount of mutual neighbors.\n"
          "delta_free_energy: float\n"
          "\tis the free energy step between hierarchical tree levels.\n"
          "Nkeep: int, optional\n"
          "\tis the minimum number of data points that a cluster must contain to be kept (default: 2).\n"
          "Nsplit: int, optional\n"
          "\tis the maximum number of data points that a cluster must contain to be considered for splitting (default: 4)\n"
          "mutual: bool, optional (currently redundant option; False not possible at the moment)\n"
          "\trequires that the two considered points are mutual neighbors (default: True).\n"
          "\n"
          "RETURNS\n"
          "-------\n"
          "numpy.Array(numpy.Array(float)):\n"
          "\tThe clusters as arrays of data point indices. (np.Array(Cluster(frame))\n"
          "\n",
          py::arg("data"),
          py::arg("cutoff"),
          py::arg("similarity"),
          py::arg("delta_free_energy"),
          py::arg("Nkeep") = 2,
          py::arg("Nsplit") = 4,
          py::arg("mutual") = true);

    m.def("hierarchical_common_nearest_neighbor",
          &hierarchical_common_nearest_neighbor,
          py::return_value_policy::automatic,
          "This function applies the hierarchical approach for the CNN clustering.\n"
          "\n"
          "Parameters\n"
          "-----------\n"
          "data: Iterable[Iterable[Number]]\n"
          "\tis the input data to be clustered.\n"
          "cutoff: float\n"
          "\tis the initial cutoff radius for neighborhood calculation.\n"
          "similarity: int\n"
          "\tis the amount of mutual neighbors.\n"
          "delta_free_energy: float\n"
          "\tis the free energy step between hierarchical tree levels.\n"
          "Nkeep: int, optional\n"
          "\tis the minimum number of data points that a cluster must contain to be kept (default: 2).\n"
          "Nsplit: int, optional\n"
          "\tis the maximum number of data points that a cluster must contain to be considered for splitting (default: 4)\n"
          "mutual: bool, optional (currently redundant option; False not possible at the moment)\n"
          "\trequires that the two considered points are mutual neighbors (default: True).\n"
          "\n"
          "RETURNS\n"
          "-------\n"
          "numpy.Array(numpy.Array(float)):\n"
          "\tThe clusters as arrays of data point indices. (np.Array(Cluster(frame))\n"
          "\n",
          py::arg("data"),
          py::arg("cutoff"),
          py::arg("similarity"),
          py::arg("delta_free_energy"),
          py::arg("Nkeep") = 2,
          py::arg("Nsplit") = 4,
          py::arg("mutual") = true);

}

#endif //CLUSTERING_PYCLUSTERING_H
