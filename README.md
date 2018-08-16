# Growing Self-Organizing Array (GSOA)

GSOA repository provides sources of the unsupervised learning based method for solving routing problems, e.g., variants of the Traveling Salesman Problem (TSP). The method has been introduced in the article [faigl18gsoa].

...
@article{faigl18gsoa,
  author    = {Jan Faigl},
  title     = {{GSOA:} Growing Self-Organizing Array - Unsupervised learning for
               the Close-Enough Traveling Salesman Problem and other routing problems},
  journal   = {Neurocomputing},
  volume    = {312},
  pages     = {120--134},
  year      = {2018},
  url       = {https://doi.org/10.1016/j.neucom.2018.05.079},
  doi       = {10.1016/j.neucom.2018.05.079},
}
...

Two implementations are currently provided:
* gsoa-etsp  - for solving Euclidean TSP (ETSP)
* gsoa-cetsp - for solving Close Enough TSP (CETSP)
that are described in the above referenced article.

Besides, the sources include problem instances for both TSP (based on the TSPLIB - http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsplib.html) and CETSP instances proposed by W.K. Mennel ( https://drum.lib.umd.edu/handle/1903/9822 ).

The implementation use visualization and algorithm support provided by the CRL ( https://github.com/comrob/crl ) which is submerged into the repository tree.

All the sources should be compiled into the build directory using cmake by calling ./install-cmake_build.sh which creates two binaries in build directory:
* tgsoa-etsp
* tgsoa-ctsp
that have accompanied configurations files tgsoa-etsp.cfg and tgsoa-cetsp.cfg. Note all the parameters can be specified also from the command line arguments.



