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

#include "tools/ArgParse.h"

#include "clustering.h"
#include "wrapper.h"

#include <limits>

using namespace std;

int main(int argc, char *argv[]) {

    ArgParse &args(*new ArgParse(argc, argv));

    args.flag<bool>("-CNN", false);
    const auto cut(args.flag<float>("-cut", std::numeric_limits<float>::max()));
    const auto sim(args.flag<unsigned int>("-sim", 0));
    const auto nsteps(args.flag<unsigned int>("-nsteps", 0));
    const auto delta_cut(args.flag<float>("-dcut", 0.0));
    const auto delta_sim(args.flag<unsigned int>("-dsim", 0));
    const auto Nkeep(args.flag<int>("-Nkeep", 0));
    const auto Nsplit(args.flag<int>("-Nsplit", 0));
    const auto relmax(args.flag<float>("-relmax", 0.0));
    const auto ntrajs(args.flag<unsigned int>("-ntrajs", std::numeric_limits<unsigned int>::max()));
    const auto ndims(args.flag<unsigned int>("-ndims", std::numeric_limits<unsigned int>::max()));
    const auto slice(args.flag<unsigned int>("-slice", 1));
    const auto do_clustering(args.flag<bool>("clustering", false));
    const auto do_hierarchic(args.flag<bool>("hierarchic", false));
    const auto do_scan(args.flag<bool>("scan", false));
    const auto mutual(args.flag<bool>("mutual", true));
    const auto do_mapping(args.flag<bool>("mapping", false));
    const auto do_dtrajs(args.flag<bool>("dtrajs", false));
    const auto overwrite(args.flag<bool>("--overwrite", true));
    const auto datafile(args.flag<string>("-dfile", "data.npy"));
    const auto clusterfile(args.flag<string>("-cfile", "clusters.npy"));
    const auto hierarchicfile(args.flag<string>("-hfile", "hclusters.npy"));
    const auto mappingfile(args.flag<string>("-mfile", "mclusters.npy"));
    const auto dtrajfile(args.flag<string>("-tfile", "dtrajs.npy"));
    const auto help(args.flag<bool>("--help", false));
    const auto delta_fe(args.flag<float>("-dfe", 0.25));

    if (help) {
        std::cout << " HIEARCHICAL DENSITY-BASED CLUSTERING " << std::endl;
        std::cout << "Use via `comdensity MODES OPTIONS`." << std::endl;
        std::cout << "For instance, run `comdensity clustering -dfile input.npy -cut 0.5 -sim 2`." << std::endl;
        std::cout << "MODES: clustering | scan | hierarchic | mapping | dtrajs" << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "-cut\tHyperspherical cutoff radius R (default: " << cut << ")" << std::endl;
        std::cout << "-sim\tSimilarity N (default: " << sim << ")" << std::endl;
        std::cout << "-dcut\tStep size for increasing R, must be negative (default: " << delta_cut << ")" << std::endl;
        std::cout << "-nsteps\tNumber of steps to increase R during scanning funcionality (default: " << nsteps << ")"
                  << std::endl;
        std::cout << "-slice\tSlice of input data (default: " << slice << ")" << std::endl;
        std::cout << "-ndims\tNumber of dimensions of input data (default: " << ndims << ")" << std::endl;
        std::cout << std::endl;
        std::cout << "I/O FILES" << std::endl;
        std::cout
                << "All I/O uses python npy-files. Don't forget the .npy extension when parsing, i.e., `-dfile mydata.npy`."
                << std::endl;
        std::cout << "-dfile\tInput data file in npy-format (default: data.npy)" << std::endl;
        std::cout << "\tShould have a three dimensional shape with 1 X #NumberofFrames X #Dimensions\n";
        std::cout << "\twhile the trajectories should be concatenated. Also a file that has the same name.\n";
        std::cout << "\tand a `-shape.npy` extension is required that lists the individual trajectory lengths.\n";
        std::cout << "\tWhen using pyemma the following snippet could help.\n";
        std::cout << "\t```\n";
        std::cout << "\t\tshape = []\n";
        std::cout << "\t\tfor traj in data:\n";
        std::cout << "\t\t  shape.append(len(traj))\n";
        std::cout << "\t\tcatted = np.concatenate(data)\n";
        std::cout << "\t\toutput = np.array([catted], dtype='float32')\n";
        std::cout << "\t\tnp.save(ticafile, output)\n";
        std::cout << "\t\tnp.save(shapefile, shape)\n";
        std::cout << "\t```\n";
        std::cout << "-cfile\tInput or output cluster file." << std::endl;
        std::cout << "-hfile\tInput or output cluster file after hierarchical clustering." << std::endl;
        std::cout << "-mfile\tInput or output of mapped cluster file." << std::endl;
        std::cout << "-tfile\tDiscrete trajectories output." << std::endl;
        std::cout
                << "\tIf output files are parsed, for instance, `-cfile clusters.npy`, another file called `clusters-shape.npy` is written."
                << std::endl;
        std::cout
                << "\tThis shape file gives the number of elements in each cluster that are organized in one dimension of `clusters.npy`."
                << std::endl;
        std::cout << "\tExample\n";
        std::cout << "\tcluster.npy contains a list of frame indices [1 2 3 5 8 13 21 34 55 89]\n";
        std::cout << "\tcluster-shape.npy contains a list of cluster sizes [5 3 2]\n";
        std::cout << "\tThus, the three clusters you have are \n";
        std::cout << "\t [1 2 3 5 8], [13 21 34], and [55 89].\n";
        std::cout
                << "\tNoise points such as the frame indices 0, 4, 6, 7, 9, 10 etc. are not contained in the cluster files.\n";
        std::cout
                << "\tIn the case of a discrete trajectory the shape file carries the trajectory lengths of the trajectories\n";
        std::cout << "\tthat are concatenated in the output trajectory file." << std::endl;
    }

    vector<vector<unsigned int> > clusters;
    vector<clstep> leaves;

    if (do_scan) {
        Clustering::Wrapper::scan(clusters, leaves, args);
    }

    if (do_clustering || do_hierarchic) {
        Clustering::Wrapper::clustering(clusters, leaves, args);
        if (do_hierarchic) {
            Clustering::Wrapper::hierarchical_clustering(clusters, leaves, args);
        }
        if (do_mapping) {
            Clustering::Wrapper::mapping(clusters, leaves, args);
        }
    }

    if (do_dtrajs && (do_clustering || do_hierarchic || do_mapping)) {
        Clustering::Wrapper::dtrajs(clusters, args);
    }

    delete &args;

    return 0;
}
