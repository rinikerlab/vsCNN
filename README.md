# Hierarchical Density-Based Clustering

This repository provides the source code for the C++ package ```comdensity``` and its python interface 
```pycomdensity```, i.e. the implementation of the density-based clustering algorithms of the common nearest neighbor 
(CNN) type [[1, 2]](#1,#2). Besides the original CNN algorithm [[1]](#1), the software contains the volume-scaled CNN (vs-CNN) [[2]](#2)
adaptation, which improves an issue of overdiscretization. 

Furthermore, a free-energy based hierarchical clustering route [[2]](#2) is implemented to cluster
Boltzmann distributed data sets, for which a plain density-based clustering does generally not suffice. Thus,
the software is particularly designed for the time series discretization for Markov state modeling of
molecular dynamics simulations.

## Citation

If you are using `(py)comdensity` in scientific work, please cite [[2]](#2)

## Installation

### Prerequisites

Compilation requires at least
* gcc 4.9.x 
* cmake 3.3.x
* boost 1.55.0 (lowest tested version)

We encourage to try for toolchains with higher versions of of gcc, cmake, and boost to achieve
the best compiler and library optimizations.

### pip Install

To install the package using `pip`, simply change into the cloned directory, i.e., the location of the ```setup.py```,
and run 
```
pip install .
```

#### CMake
The `comdensity` binary and the `pycomdensity` python package can be compiled via the CMake project.
Typical CMake installation should work like this
```$xslt
mkdir build
cd build
cmake ..
make
make install
```
If you want to install the binary in a certain location on your system, specify the path using the
```-DCMAKE_INSTALL_PREFIX=/your/path/to/bin``` flag.

If you omit `make install`, the newly created `build` folder can be appended to the `PATH` variable of your `.bashrc` to 
make the binary called `comdensity` available. To make the ```pycomdensity``` available, include the `build`
folder to the `PYTHONPATH` variable of your `.bashrc`.
```$xslt
export PATH="/path/to/clustering/build:$PATH"
export PYTHONPATH="/path/to/clustering/build:$PYTHONPATH"
```

The build type can be specified as an ```OPTION``` via ```-DCMAKE_BUILD_TYPE=[Debug|Release]```.
For the production use case the ```Release``` build type is recommended 
because it enables the required compiler optimization levels.

For the python bindings CMake will detect the python version from the environment's python path. 
If you want to specify a particular python version use the ```-DPYBIND11_PYTHON_VERSION=[2.X|3.X]``` flag.

## Usage

### `pycomdensity`

Usage in python is also documented in the package such that you can find information through
```$xslt
import pycomdensity
help(pycomdensity)
```

The package contains two functions for plain clustering through vs-CNN or the original CNN.
Two more functions implement the free-energy based hierarchical approach for either density-based algorithm.
For instance, the hierarchical vs-CNN approach is documented like this

```$xslt
    hierarchical_volumescaled_common_nearest_neighbor(...) method of builtins.PyCapsule instance
        hierarchical_volumescaled_common_nearest_neighbor(data: List[List[float]], cutoff: float, similarity: int, delta_free_energy: float, min_size: int = 2, max_size: int = 4, mutual: bool = True) -> array
        
        This function applies the hierarchical approach for the vs-CNN clustering.
        
        Parameters
        -----------
        data: Iterable[Iterable[Number]]
                is the input data to be clustered.
        cutoff: float
                is the initial cutoff radius for neighborhood calculation.
        similarity: int
                is the amount of mutual neighbors.
        delta_free_energy: float
                is the free energy step between hierarchical tree levels.
        Nkeep: int, optional
                is the minimum number of data points that a cluster must contain to be kept (default: 2).
        Nsplit: int, optional
                is the maximum number of data points that a cluster must contain to be considered for splitting (default: 4)
        mutual: bool, optional (currently redundant option; False not possible at the moment)
                requires that the two considered points are mutual neighbors (default: True).
        
        RETURNS
        -------
        numpy.Array(numpy.Array(float)):
                The clusters as arrays of data point indices. (np.Array(Cluster(frame))
```

Note, that `data` has the format
`#datapoints x #datadimensionality`. Hence, individual trajectories are concatenated in `data`.
Also note, that slicing of the data has to be handled as pre-processing step.
(The post-processing of sorting the clusters into discretized trajectories
for MSM construction is also handled by the user at the moment.)

Moreover, the default value `mutual=True` cannot be changed at the moment.

### `comdensity`

For documentation of the binary type `comdensity --help` in the shell.

#### Clustering
Plain (non-hierarchical) density-based clustering is started via
```$xslt
   comdensity clustering [OPTIONS]
```
with following options

| Options | Description |
| ------------- |:----------------:|
| `-CNN` | falls back to CNN instead of default vs-CNN |
| Clustering options |
| `-cut` |  cutoff radius |
| `-sim` | similarity (mutual neighbor density) |
| `-Nkeep` | minimum cluster size to keep |
| I/O Files |
| `-dfile` | input data (npy-file, comes with a shape file) |
| `-cfile` | output clusters (npy-file, comes with a shape file) |
| Data reduction options |
| `-slice` | data points to skip (if you want to use less than available) |
| `-ntrajs` | number of trajectories (if you want to use less than available) |
| `-ndims` | number of dimensions from data | 

The input data file should have a three dimensional shape with 1 X #NumberofFrames X #Dimensions
while separate trajectories must be concatenated. Also a file that has the same name
with `-shape.npy` extension is required that lists the individual trajectory lengths.
The following snippet could help.
```
import numpy as np

inname = "TICA.npy"
data = np.load(inname, allow_pickle=True)

shape = []
for traj in data:
  shape.append(len(traj))
shape = np.array(shape, dtype='uint32')

catted = np.concatenate(data)
output = np.array([catted], dtype='float32')
print(np.shape(output))

outname = "cTICA.npy"
np.save(outname, output)
shapefile = "cTICA-shape.npy"
np.save(shapefile, shape)
```
#### Scan for Large Cutoff
Before the hierarchical clustering, one has to find a large cutoff for which a majority of the data is clustered. 
One can use
```$xslt
   comdensity scan [OPTIONS]
```
with following additional options

| Options | Description |
| ------------- |:----------------:|
| `-CNN` | falls back to CNN instead of default vs-CNN |
| Clustering options |
| `-cut` |  cutoff radius |
| `-sim` | similarity (mutual neighbor density) |
| `-dcut` | delta cutoff radius per step/layer (must be negative, for instance, -0.1) |
| `-nsteps` | number of times -cut is increased by -dcut |
| `-relmax` | target relative amount of clustered data, for instance, 0.99 |
| I/O Files |
| `-dfile` | input data (npy-file, comes with a shape file) |
| `-cfile` | input/output clusters (npy-file, comes with a shape file) |
| `-hfile` | output hierarchical clusters (npy-file, comes with a shape file) |
| Data reduction options |
| `-slice` | data points to skip (if you want to use less than available) |
| `-ntrajs` | number of trajectories (if you want to use less than available) |
| `-ndims` | number of dimensions from data | 

#### Hierarchical
The free-energy based hierarchical approach is enabled via
```$xslt
   comdensity hierarchic [OPTIONS]
```
with following additional options

| Options | Description |
| ------------- |:----------------:|
| `-CNN` | falls back to CNN instead of default vs-CNN |
| Clustering options |
| `-cut` | cutoff radius |
| `-sim` | similarity (mutual neighbor density) |
| `-dfe` | delta free energy per hierarchical step/layer |
| `-Nkeep` | minimal cluster size to keep |
| `-Nsplit` | maximal cluster size to split |
| I/O Files |
| `-dfile` | input data (npy-file, comes with a shape file) |
| `-cfile` | input/output clusters (npy-file, comes with a shape file) |
| `-hfile` | output hierarchical clusters (npy-file, comes with a shape file) |
| Data reduction options |
| `-slice` | frames to skip (if you want to use less than available) |
| `-ntrajs` | number of trajectories (if you want to use less than available) |
| `-ndims` | number of dimensions from data | 

#### Discretize

Besides the output of clustering results one get can obtain the discretized trajectories when specifying
```$xslt
   comdensity [clustering|hierarchic] dtrajs [OPTIONS]
```
with either production option `[clustering|hierarchical]`

| Options | Description |
| ------------- |:----------------:|
| I/O Files |
| `-dfile` | input data (npy-file, comes with a shape file) |
| `-cfile` | input clusters (npy-file, comes with a shape file) |
| `-hfile` | input hierarchical clusters (npy-file, comes with a shape file) |
| `-mfile` | input mapped clusters (npy-file, comes with a shape file) |
| `-tfile` | output discretized trajectories (one-dimensional) |
| Data reduction options |
| `-slice` | data slicing that was used during clustering |

## References
<a id="1">[1]</a> 
B. Keller,  X. Daura, W. van Gunsteren 
<em>Comparing Geometric and Kinetic Cluster Algorithms for Molecular Simulation Data</em>
J. Chem. Phys. 132, 074110 (2010)

<a id="2">[2]</a> 
R. G. Weiss, B. Schroeder, S. Wang, S. Riniker 
<em>Volume-Scaled Common Nearest Neighbor Clustering with Free-Energy Hierarchy</em>
J. Chem. Phys. XX, XXXX (2020)
