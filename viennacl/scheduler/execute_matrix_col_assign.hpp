#ifndef VIENNACL_SCHEDULER_EXECUTE_MATRIX_COL_ASSIGN_HPP
#define VIENNACL_SCHEDULER_EXECUTE_MATRIX_COL_ASSIGN_HPP

/* =========================================================================
   Copyright (c) 2010-2013, Institute for Microelectronics,
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


/** @file viennacl/scheduler/execute_matrix_col_assign.hpp
    @brief Deals with the execution of A = RHS; for a matrix A and any compatible right hand side expression RHS.
*/

#include "viennacl/forwards.h"
#include "viennacl/scheduler/forwards.h"
#include "viennacl/linalg/matrix_operations.hpp"

namespace viennacl
{
  namespace scheduler
  {
    /** @brief Deals with x = RHS where RHS is a vector expression */
    inline void execute_matrix_col_assign_composite(statement const & s, statement_node const & root_node)
    {
      statement::container_type const & expr = s.array();

      if (expr[1].op.type  == OPERATION_BINARY_ADD_TYPE)
      {
        if (expr[0].lhs.type == MATRIX_COL_FLOAT_TYPE
            && expr[1].lhs.type == MATRIX_COL_FLOAT_TYPE
            && expr[1].rhs.type == MATRIX_COL_FLOAT_TYPE)
        {
          viennacl::matrix_base<float, viennacl::column_major>       & A = *(expr[0].lhs.matrix_col_float);
          viennacl::matrix_base<float, viennacl::column_major> const & B = *(expr[1].lhs.matrix_col_float);
          viennacl::matrix_base<float, viennacl::column_major> const & C = *(expr[1].rhs.matrix_col_float);
          viennacl::linalg::ambm(A,
                                 B, 1.0, 1, false, false,
                                 C, 1.0, 1, false, false);
        }
        else if (expr[0].lhs.type == MATRIX_COL_DOUBLE_TYPE
                 && expr[1].lhs.type == MATRIX_COL_DOUBLE_TYPE
                 && expr[1].rhs.type == MATRIX_COL_DOUBLE_TYPE)
        {
          viennacl::matrix_base<double, viennacl::column_major>       & A = *(expr[0].lhs.matrix_col_double);
          viennacl::matrix_base<double, viennacl::column_major> const & B = *(expr[1].lhs.matrix_col_double);
          viennacl::matrix_base<double, viennacl::column_major> const & C = *(expr[1].rhs.matrix_col_double);
          viennacl::linalg::ambm(A,
                                 B, 1.0, 1, false, false,
                                 C, 1.0, 1, false, false);
        }
        else
          throw statement_not_supported_exception("Cannot deal with addition of column-major matrix");
      }
      else if (expr[1].op.type  == OPERATION_BINARY_SUB_TYPE)
      {
        if (expr[0].lhs.type == MATRIX_COL_FLOAT_TYPE
            && expr[1].lhs.type == MATRIX_COL_FLOAT_TYPE
            && expr[1].rhs.type == MATRIX_COL_FLOAT_TYPE)
        {
          viennacl::matrix_base<float, viennacl::column_major>       & A = *(expr[0].lhs.matrix_col_float);
          viennacl::matrix_base<float, viennacl::column_major> const & B = *(expr[1].lhs.matrix_col_float);
          viennacl::matrix_base<float, viennacl::column_major> const & C = *(expr[1].rhs.matrix_col_float);
          viennacl::linalg::ambm(A,
                                 B,  1.0, 1, false, false,
                                 C, -1.0, 1, false, false);
        }
        else if (expr[0].lhs.type == MATRIX_COL_DOUBLE_TYPE
                 && expr[1].lhs.type == MATRIX_COL_DOUBLE_TYPE
                 && expr[1].rhs.type == MATRIX_COL_DOUBLE_TYPE)
        {
          viennacl::matrix_base<double, viennacl::column_major>       & A = *(expr[0].lhs.matrix_col_double);
          viennacl::matrix_base<double, viennacl::column_major> const & B = *(expr[1].lhs.matrix_col_double);
          viennacl::matrix_base<double, viennacl::column_major> const & C = *(expr[1].rhs.matrix_col_double);
          viennacl::linalg::ambm(A,
                                 B,  1.0, 1, false, false,
                                 C, -1.0, 1, false, false);
        }
        else
          throw statement_not_supported_exception("Cannot deal with subtraction of column-major matrix");
      }
      else
        throw statement_not_supported_exception("Unsupported binary operator for column-major matrix operations");
    }

    /** @brief Deals with A = B  for a matrix B */
    inline void execute_matrix_col_assign_matrix(statement const & s, statement_node const & root_node)
    {
      typedef statement::container_type   StatementContainer;

      StatementContainer const & expr = s.array();

      if (root_node.lhs.type == MATRIX_COL_FLOAT_TYPE && root_node.rhs.type == MATRIX_COL_FLOAT_TYPE)
      {
        viennacl::matrix_base<float, viennacl::column_major>       & A = *(root_node.lhs.matrix_col_float);
        viennacl::matrix_base<float, viennacl::column_major> const & B = *(root_node.rhs.matrix_col_float);
        viennacl::linalg::am(A,
                             B, 1.0, 1, false, false);
      }
      else if (root_node.lhs.type == MATRIX_COL_DOUBLE_TYPE && root_node.rhs.type == MATRIX_COL_DOUBLE_TYPE)
      {
        viennacl::matrix_base<double, viennacl::column_major>       & A = *(root_node.lhs.matrix_col_double);
        viennacl::matrix_base<double, viennacl::column_major> const & B = *(root_node.rhs.matrix_col_double);
        viennacl::linalg::am(A,
                             B, 1.0, 1, false, false);
      }
      else
        throw statement_not_supported_exception("Unsupported assignment to column-major matrix");
    }

    /** @brief Generic dispatcher */
    inline void execute_matrix_col_assign(statement const & s, statement_node const & root_node)
    {
      switch (root_node.rhs.type_family)
      {
        case COMPOSITE_OPERATION_FAMILY:
          execute_matrix_col_assign_composite(s, root_node);
          break;
        case MATRIX_COL_TYPE_FAMILY:
          execute_matrix_col_assign_matrix(s, root_node);
          break;
        default:
          throw statement_not_supported_exception("Invalid rvalue encountered in column-major matrix assignment");
      }
    }


  }

} //namespace viennacl

#endif

