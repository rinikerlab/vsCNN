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

#include "clustering.h"
#include "vs_cnn.h"
#include "cnn.h"
#include "discretization.h"
#include "tools/utility.h"

#include "tools/io.h"

#include "wrapper.h"

namespace Clustering {

    namespace Wrapper {

        // USER INTERFACE API
        void clustering(vector<vector<unsigned int> > &clusters,
                        vector<clstep> &leaves,
                        ArgParse &args) {
            // Input arguments from terminal flags
            const auto datafile = args.flag<std::string>("-dfile");
            const auto clusterfile = args.flag<std::string>("-cfile");
            const auto ntrajs = args.flag<unsigned int>("-ntrajs");
            const auto ndims = args.flag<unsigned int>("-ndims");
            const auto slice = args.flag<unsigned int>("-slice");
            const auto cut = args.flag<float>("-cut");
            const auto sim = args.flag<unsigned int>("-sim");
            const auto Nkeep = args.flag<int>("-Nkeep");
            const auto mutual = args.flag<bool>("mutual");

            // Obtain data
            vector<vector<float>> data;
            vector<unsigned int> shapes;
            map<unsigned int, unsigned int> frames;
            vector<unsigned int> traj_shapes(3);
            unsigned int total_frames = 0;
            total_frames = get_tICs(data, frames, shapes, traj_shapes, datafile, ntrajs, ndims, slice);
            std::cout << "TOTAL # FRAMES " << total_frames << std::endl;

            try {
                clusters = read_clusters(leaves, clusterfile);
            } catch (...) {
                // Obtain neighbor lists
                Neighbors neighbor_lists;
                Neighbors second_neighbor_lists;
                nns::neighbors(neighbor_lists, data, cut, sim);

                // Obtain clusters
                Core::simptr similarity;
                similarity = args.flag<bool>("-CNN") ? CommonNearestNeighbor::similarity : CommonDensity::similarity;
                clusters = Clustering::Core::algorithm(similarity,
                                                       data,
                                                       neighbor_lists,
                                                       second_neighbor_lists,
                                                       cut,
                                                       sim,
                                                       Nkeep,
                                                       mutual);
                leaves.resize(clusters.size(), clstep(0, cut, sim));

                // Write to file
                std::string ofile = clusterfile;
                if (fexists(ofile)) { ofile = backup_file(clusterfile); }
                write_clusters(ofile, clusters, leaves);
            }

            // Some debug printing
            float total = 0;
            float all = static_cast<float>(total_frames);
            cout << " CLUSTERING RESULTS " << endl;
            cout << "Cut: " << cut << "\tSim: " << sim << endl;
            cout << std::setprecision(2);
            for (size_t idx = 0; idx < clusters.size(); idx++) {
                float clsize = static_cast<float>(clusters[idx].size());
                cout << idx << "\t" << clusters[idx].size() << "\t" << 100.0f * clsize / all << "%" << endl;
                total += clsize;
            }
            cout << "Remaining noise is : " << 100.0f * (1.0 - total / all) << "%\n" << endl;
        }

        void scan(vector<vector<unsigned int> > &clusters,
                  vector<clstep> &leaves,
                  ArgParse &args) {
            // Input arguments from terminal flags
            const auto datafile = args.flag<std::string>("-dfile");
            const auto clusterfile = args.flag<std::string>("-cfile");
            const auto ntrajs = args.flag<unsigned int>("-ntrajs");
            const auto ndims = args.flag<unsigned int>("-ndims");
            const auto slice = args.flag<unsigned int>("-slice");
            const auto cut = args.flag<float>("-cut");
            const auto delta_cut = args.flag<float>("-dcut");
            const auto sim = args.flag<unsigned int>("-sim");
            const auto delta_sim = args.flag<unsigned int>("-dsim");
            const auto nsteps = args.flag<unsigned int>("-nsteps");
            const auto Nkeep = args.flag<int>("-Nkeep");
            const auto relmax = args.flag<float>("-relmax");
            const auto mutual = args.flag<bool>("mutual");

            // Obtain tICs
            vector<vector<float>> tICs;
            vector<unsigned int> shapes;
            map<unsigned int, unsigned int> frames;
            vector<unsigned int> traj_shapes(3);
            unsigned int total_frames = 0;
            total_frames = get_tICs(tICs, frames, shapes, traj_shapes, datafile, ntrajs, ndims, slice);
            std::cout << "TOTAL # FRAMES " << total_frames << std::endl;

            // Get a plan
            vector<clstep> plan;
            plan = Clustering::Utility::hierarchy(nsteps, cut, delta_cut, sim, delta_sim);
            auto maxsz = static_cast<unsigned int>(std::roundf(relmax * static_cast<float>(total_frames)));

            for (auto clstep : plan) {
                // Obtain neighbor lists
                Neighbors neighbor_lists;
                Neighbors second_neighbor_lists;
                nns::neighbors(neighbor_lists, tICs, clstep.cut, clstep.sim);
                if (neighbor_lists.size() < 2) continue;
                // Obtain clusters
                Core::simptr similarity;
                similarity = args.flag<bool>("-CNN") ? CommonNearestNeighbor::similarity : CommonDensity::similarity;
                vector<vector<unsigned int> > scan_clusters;
                scan_clusters = Clustering::Core::algorithm(similarity,
                                                            tICs,
                                                            neighbor_lists,
                                                            second_neighbor_lists,
                                                            clstep.cut,
                                                            clstep.sim,
                                                            Nkeep,
                                                            mutual);
                // Some debug printing
                float total = 0;
                auto all = static_cast<float>(total_frames);
                cout << " CLUSTERING RESULTS " << endl;
                cout << "Cut: " << clstep.cut << "\tSim: " << clstep.sim << endl;
                cout << std::setprecision(2);
                for (size_t idx = 0; idx < scan_clusters.size(); idx++) {
                    auto clsize = static_cast<float>(scan_clusters[idx].size());
                    cout << idx << "\t" << scan_clusters[idx].size() << "\t" << 100.0f * clsize / all << "%" << endl;
                    total += clsize;
                }
                cout << "Remaining noise is : " << 100.0f * (1.0 - total / all) << "%\n" << endl;
                // Break if the the clustered points is larger or equal to maxsz.
                unsigned int amount_clustered(0);
                if (!scan_clusters.empty())
                    for (auto &cluster : scan_clusters)
                        amount_clustered += cluster.size();
                if (amount_clustered >= maxsz) {
                    clusters = scan_clusters;
                    leaves.resize(clusters.size(), clstep);
                    break;
                }
            }

            // Write to file
            std::string ofile = clusterfile;
            if (fexists(ofile)) { ofile = backup_file(clusterfile); }
            write_clusters(ofile, clusters, leaves);
        }

        void hierarchical_clustering(vector<vector<unsigned int> > &clusters,
                                     vector<clstep> &leaves,
                                     ArgParse &args) {
            // Input arguments from terminal flags
            const auto datafile = args.flag<std::string>("-dfile");
            const auto clusterfile = args.flag<std::string>("-cfile");
            const auto hierarchicfile = args.flag<std::string>("-hfile");
            const auto ntrajs = args.flag<unsigned int>("-ntrajs");
            const auto ndims = args.flag<unsigned int>("-ndims");
            const auto slice = args.flag<unsigned int>("-slice");
            const auto cut = args.flag<float>("-cut");
            const auto sim = args.flag<unsigned int>("-sim");
            const auto delta_fe = args.flag<float>("-dfe");
            const auto Nkeep = args.flag<int>("-Nkeep");
            const auto Nsplit = args.flag<int>("-Nsplit");
            const auto mutual = args.flag<bool>("mutual");

            // Obtain tICs
            vector<vector<float>> tICs;
            vector<unsigned int> shapes;
            map<unsigned int, unsigned int> frames;
            vector<unsigned int> traj_shapes(3);
            unsigned int total_frames = 0;
            total_frames = get_tICs(tICs, frames, shapes, traj_shapes, datafile, ntrajs, ndims, slice);
            std::cout << "TOTAL # FRAMES " << total_frames << std::endl;

            try {
                clusters = read_clusters(leaves, hierarchicfile);
            } catch (...) {
                // Cluster hierarchically
                Core::simptr similarity;
                similarity = args.flag<bool>("-CNN") ? CommonNearestNeighbor::similarity : CommonDensity::similarity;
                clstep init_step(0, cut, sim);
                leaves = Clustering::hierarchical_clustering(similarity,
                                                             clusters,
                                                             tICs,
                                                             init_step,
                                                             delta_fe,
                                                             traj_shapes[2],
                                                             Nkeep,
                                                             Nsplit,
                                                             mutual);

                // Write to file
                std::string ofile = hierarchicfile;
                if (fexists(ofile)) { ofile = backup_file(hierarchicfile); }
                write_clusters(ofile, clusters, leaves);
            }

            float total = 0;
            float all = static_cast<float>(total_frames);
            cout << " HIERARCHICAL CLUSTERING RESULTS " << endl;
            cout << std::setprecision(2);
            for (size_t idx = 0; idx < clusters.size(); idx++) {
                float clsize = static_cast<float>(clusters[idx].size());
                cout << idx << "\t" << clusters[idx].size() << "\t" << 100.0f * clsize / all << "%";
                cout << "\tat Step: " << leaves[idx].step;
                cout << "\twith Cut: " << leaves[idx].cut;
                cout << "\tand Sim: " << leaves[idx].sim << endl;
                total += clsize;
            }
            cout << "Remaining noise is : " << 100.0f * (1.0 - total / all) << "%\n" << endl;
        }

        void
        mapping(vector<vector<unsigned int> > &clusters,
                vector<clstep> &leaves,
                ArgParse &args) {
            // Input arguments from terminal flags
            std::string datafile = args.flag<std::string>("-dfile");
            std::string mappingfile = args.flag<std::string>("-mfile");
            unsigned int ntrajs = args.flag<unsigned int>("-ntrajs");
            unsigned int ndims = args.flag<unsigned int>("-ndims");
            unsigned int slice = args.flag<unsigned int>("-slice");

            // Obtain full tICs
            vector<vector<float>> full_tICs;
            vector<unsigned int> full_shapes;
            map<unsigned int, unsigned int> full_frames;
            vector<unsigned int> full_traj_shapes(3);
            unsigned int full_total_frames = 0;
            full_total_frames = get_tICs(full_tICs, full_frames, full_shapes, full_traj_shapes, datafile, ntrajs, ndims,
                                         0);
            full_frames.clear();

            try {
                clusters = read_clusters(leaves, mappingfile);
            } catch (...) {
                // Obtain reduced tICs
                vector<vector<float>> reduced_tICs;
                vector<unsigned int> reduced_shapes;
                map<unsigned int, unsigned int> reduced_frames;
                vector<unsigned int> reduced_traj_shapes(3);
                get_tICs(reduced_tICs, reduced_frames, reduced_shapes, reduced_traj_shapes, datafile, ntrajs, ndims,
                         slice);

                // Map frames to existing clusters
                Clustering::cluster_mapping(clusters, full_tICs, reduced_tICs, reduced_frames, leaves, slice);

                // Write to file
                std::string ofile = mappingfile;
                if (fexists(ofile)) { ofile = backup_file(mappingfile); }
                write_clusters(ofile, clusters, leaves);
            }

            float total = 0;
            float all = static_cast<float>(full_total_frames);
            cout << " MAPPING RESULTS " << endl;
            cout << std::setprecision(2);
            for (size_t idx = 0; idx < clusters.size(); idx++) {
                float clsize = static_cast<float>(clusters[idx].size());
                cout << idx << "\t" << clusters[idx].size() << "\t" << 100.0f * clsize / all << "%";
                cout << "\tat Step: " << leaves[idx].step;
                cout << "\twith Cut: " << leaves[idx].cut;
                cout << "\tand Sim: " << leaves[idx].sim << endl;
                total += clsize;
            }
            cout << "Remaining noise is : " << 100.0f * (1.0 - total / all) << "%\n" << endl;
        }

        void
        dtrajs(vector<vector<unsigned int> > &clusters,
               ArgParse &args) {
            // Input arguments from terminal flags
            std::string dtrajfile = args.flag<std::string>("-tfile");
            std::string datafile = args.flag<std::string>("-dfile");
            unsigned int ntrajs = args.flag<unsigned int>("-ntrajs");
            unsigned int ndims = args.flag<unsigned int>("-ndims");
            unsigned int slice = args.flag<unsigned int>("-slice");
            bool do_mapping = args.flag<bool>("mapping");

            // Decide whether data is still sliced or mapped
            unsigned int final_slice = slice;
            if (do_mapping) { final_slice = 1; }

            // Obtain shapes of inital trajectories
            vector<vector<float>> tICs;
            vector<unsigned int> shapes;
            map<unsigned int, unsigned int> frames;
            vector<unsigned int> traj_shapes(3);
            get_tICs(tICs, frames, shapes, traj_shapes, datafile, ntrajs, ndims, final_slice);

            // Obtain the discrete trajectories
            vector<vector<int> > dtrajs;
            dtrajs = Clustering::Discretization::dtrajs(clusters, shapes);

            // Write to file
            write_dtrajs(dtrajfile, dtrajs);
        }

    }
}
