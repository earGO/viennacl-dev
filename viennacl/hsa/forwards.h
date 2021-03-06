#ifndef VIENNACL_HSA_FORWARDS_H_
#define VIENNACL_HSA_FORWARDS_H_

/* =========================================================================
   Copyright (c) 2010-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.
   Portions of this software are copyright by UChicago Argonne, LLC.

                            -----------------
                  ViennaCL - The Vienna Computing Library
                            -----------------

   Project Head:    Karl Rupp                   rupp@iue.tuwien.ac.at

   (A list of authors and contributors can be found in the PDF manual)

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */

/** @file viennacl/hsa/forwards.h
    @brief This file provides the forward declarations for the OpenCL layer of ViennaCL
*/

#define VIENNACL_OCL_MAX_DEVICE_NUM  8

#include <stddef.h>

namespace viennacl
{
  namespace hsa
  {
    //device type tags (cf. OpenCL standard)
    /** @brief A tag identifying OpenCL devices as GPUs. */
    struct gpu_tag {};
    /** @brief A tag identifying OpenCL devices as CPUs. */
    struct cpu_tag {};


    class kernel;
    class device;
    class command_queue;
    class context;
    class program;

    template<class OCL_TYPE>
    class handle;

    template<typename KernelType>
    void enqueue(KernelType & k, viennacl::hsa::command_queue const & queue);

    inline viennacl::hsa::context & current_context();
    inline viennacl::hsa::device const & current_device();
  }
} //namespace viennacl

#endif

/*@}*/
