# Computational Robotics Library (CRL)

CRL repository provides a set of helper function for developing algorithms. The main features include support for 
   * parameters of algorithms with binding to program options
   * results log and batch execution of the algorithm for specified number of trials
   * graphics output via cairo to save high quality output in pdf, svg, eps, and png as well.

The library depends on Boost, Log4cxx and graphics on the cairo library.

Standard makefiles are provided and cmake files as well. On most systems, it should be sufficient to just execut ./install.sh.
For the cmake, use ./install-cmake_build.sh to build the libraries into build directory.

