#ifndef EIGEN_HPP_
#define EIGEN_HPP_

#define EIGEN_MATRIXBASE_PLUGIN "eigen_matrix_plugin.hpp"

// For whatever reason, Eigen needs these headers (probably a mistake I made, but this works for now)
// #include <memory>
// #include <alloca.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmisleading-indentation" // PlainObjectBase.h:325 (b/c EIGEN_INITIALIZE_MATRICES_BY_NAN)
#include "Eigen/Core"
#include "unsupported/Eigen/CXX11/TensorSymmetry"
#pragma clang diagnostic pop

#endif // EIGEN_HPP_
