#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \ingroup attributes */

#include "detection.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup cuda_attributes CUDA
///
///   \ingroup attributes
///
/// \{

/** \def ein_host

      \brief portable `__host__` for CUDA

      \details

      indicates the function should be available on the host

    \def ein_device

      \brief portable `__device__` for CUDA

      \details

      indicates the function should be available on the device

    \def ein_hd

      \brief applies both \ref ein_host and \ref ein_device

      \details

      most functions in the library should be defined this way

      may eventually add similar annotations for sycl and the like

    \def ein_global

      \brief portable `__global__` for CUDA

      \details

      indicates the function is a global entry point for a compute kernel */

#ifdef __CUDACC__
#define ein_host __host__
#define ein_device __device__
#define ein_global __global__
#else
#define ein_host
#define ein_device
#define ein_global
#endif

#define ein_hd ein_host ein_device

/// \}
