#ifndef VIENNACL_GENERATOR_SYMBOLIC_TYPES_HPP
#define VIENNACL_GENERATOR_SYMBOLIC_TYPES_HPP

/* =========================================================================
   Copyright (c) 2010-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                  ViennaCL - The Vienna Computing Library
                            -----------------

   Project Head:    Karl Rupp                   rupp@iue.tuwien.ac.at

   (A list of authors and contributors can be found in the PDF manual)

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */



#include "viennacl/generator/symbolic_types_base.hpp"
#include "viennacl/generator/operators.hpp"
#include "viennacl/meta/result_of.hpp"

namespace viennacl
{
  namespace generator
  {

  typedef std::map<void const *, shared_infos_t> shared_infos_map_t;
  typedef std::map<kernel_argument*,void const *,deref_less> temporaries_map_t;

      template<class T> struct repr_of;
      template<> struct repr_of<float>{ static const std::string value(){ return "f"; } };
      template<> struct repr_of<double>{ static const std::string value(){ return "d"; } };
      template<> struct repr_of<int>{ static const std::string value(){ return "i"; } };
      template<> struct repr_of<unsigned int>{ static const std::string value(){ return "ui"; } };

      template<class LHS, class OP, class RHS>
      class binary_vector_expression : public binary_vector_expression_infos_base{
      public:
          typedef typename LHS::ScalarType ScalarType;
          binary_vector_expression(LHS const & lhs, RHS const & rhs) :binary_vector_expression_infos_base( new LHS(lhs),new OP(),new RHS(rhs)){ }
      };



      template<class LHS, class RHS, class OP_REDUCE>
      class binary_vector_expression<LHS,reduce_type<OP_REDUCE>,RHS> : public matvec_prod_infos_base{
      public:
          typedef typename LHS::ScalarType ScalarType;
          binary_vector_expression(LHS const & lhs, RHS const & rhs) : matvec_prod_infos_base(new LHS(lhs), new reduce_type<OP_REDUCE>(), new RHS(rhs)){ }
      };


      template<class LHS, class OP, class RHS>
      class binary_scalar_expression : public binary_scalar_expression_infos_base{
      public:
          typedef typename LHS::ScalarType ScalarType;
          typedef LHS Lhs;
          typedef RHS Rhs;
          binary_scalar_expression(LHS const & lhs, RHS const & rhs) :binary_scalar_expression_infos_base( new LHS(lhs),new OP(),new RHS(rhs)){ }
      };



      template<class LHS, class OP, class RHS>
      class binary_matrix_expression : public binary_matrix_expression_infos_base{
      public:
          typedef typename LHS::ScalarType ScalarType;
          typedef LHS Lhs;
          typedef RHS Rhs;
          binary_matrix_expression(LHS const & lhs, RHS const & rhs) :binary_matrix_expression_infos_base( new LHS(lhs),new OP(),new RHS(rhs)){ }
      };

      template<class LHS, class RHS, class OP_REDUCE>
      class binary_matrix_expression<LHS,reduce_type<OP_REDUCE>,RHS> : public matmat_prod_infos_base{
      public:
          typedef typename LHS::ScalarType ScalarType;
          binary_matrix_expression(LHS const & lhs, RHS const & rhs) : matmat_prod_infos_base(new LHS(lhs), new reduce_type<OP_REDUCE>(), new RHS(rhs)){ }
      private:

      };

      template<class SUB, class OP>
      class unary_vector_expression : public unary_vector_expression_infos_base{
      public:
          typedef typename SUB::ScalarType ScalarType;
          unary_vector_expression(SUB const & sub) :unary_vector_expression_infos_base(new SUB(sub), new OP()){ }
      };

      template<class SUB, class OP>
      class unary_scalar_expression : public unary_scalar_expression_infos_base{
      public:
          typedef typename SUB::ScalarType ScalarType;
          unary_scalar_expression(SUB const & sub) :unary_scalar_expression_infos_base(new SUB(sub), new OP()){ }
      };

      template<class LHS, class OP_REDUCE, class RHS>
      class binary_scalar_expression<LHS, reduce_type<OP_REDUCE>, RHS > : public inner_product_infos_base{
          typedef typename LHS::ScalarType ScalarType;
      public:
           binary_scalar_expression(LHS const & lhs, RHS const & rhs):  inner_product_infos_base(new LHS(lhs), new OP_REDUCE, new RHS(rhs)), tmp_(1024){ }
           void enqueue(unsigned int & arg, viennacl::ocl::kernel & k) const{ k.arg(arg++,tmp_.handle().opencl_handle()); }
           void bind(std::map<void const *, shared_infos_t>  & shared_infos, std::map<kernel_argument*,void const *,deref_less> & temporaries_map){
               temporaries_map.insert(std::make_pair(this,handle())).first;
               infos_= &shared_infos.insert(std::make_pair(handle(),shared_infos_t(shared_infos.size(),print_type<ScalarType>::value(),sizeof(ScalarType)))).first->second;
               lhs_->bind(shared_infos,temporaries_map);
               rhs_->bind(shared_infos,temporaries_map);
           }
           void const * handle() const{ return static_cast<void const *>(&tmp_.handle()); }
      private:
          viennacl::vector<ScalarType> tmp_;
      };

      template<class SUB, class OP>
      class unary_matrix_expression : public unary_matrix_expression_infos_base{
      public:
          typedef typename SUB::ScalarType ScalarType;
          unary_matrix_expression(SUB const & sub) :unary_matrix_expression_infos_base(new SUB(sub), new OP()){ }
      };






    /**
    * @brief Symbolic scalar type. Will be passed by value.
    *
    * @tparam SCALARTYPE The Scalartype of the scalar in the generated code
    */
    template <typename SCALARTYPE>
    class cpu_symbolic_scalar : public cpu_scal_infos_base
    {
    public:
        typedef SCALARTYPE ScalarType;
        cpu_symbolic_scalar(ScalarType const & val) :  val_(val){}
        void enqueue(unsigned int & n_arg, viennacl::ocl::kernel & k) const{ k.arg(n_arg++,typename viennacl::result_of::cl_type<SCALARTYPE>::type(val_)); }
        std::string repr() const{ return "vscal"+repr_of<SCALARTYPE>::value(); }
        void bind(std::map<void const *, shared_infos_t>  & shared_infos
                  ,std::map<kernel_argument*,void const *,deref_less> & temporaries_map){
            infos_= &shared_infos.insert(std::make_pair(handle(),shared_infos_t(shared_infos.size(),print_type<ScalarType>::value(),sizeof(ScalarType)))).first->second;
        }
        void const * handle() const { return static_cast<void const *>(&val_); }
        std::string generate(unsigned int i, int vector_element = -1) const{  return infos_->name; }
    private:
        ScalarType val_;
    };

    template<int N>
    class symbolic_constant : public infos_base{
    public:
        virtual std::string generate(unsigned int i, int vector_element = -1) const { return to_string(N); }
        virtual std::string repr() const { return "cst"+to_string(N); }
        virtual std::string simplified_repr() const { return repr(); }
        virtual void access_index(unsigned int i, std::string const & ind0, std::string const & ind1){ }
        void fetch(unsigned int i, kernel_generation_stream & kss){ }
        void write_back(unsigned int i, kernel_generation_stream & kss){ }
        virtual void bind(std::map<void const *, shared_infos_t> & , std::map<kernel_argument*,void const *,deref_less> &){ }
        virtual void get_kernel_arguments(std::map<kernel_argument const *, std::string, deref_less> & args) const { }
    };

    /**
    * @brief Symbolic scalar type. Will be passed by pointer.
    *
    * @tparam ID The argument ID of the scalar in the generated code
    * @tparam SCALARTYPE The SCALARTYPE of the scalar in the generated code
    */
    template <typename SCALARTYPE>
    class gpu_symbolic_scalar : public gpu_scal_infos_base
    {
      private:
        typedef gpu_symbolic_scalar<SCALARTYPE> self_type;

      public:
        typedef viennacl::scalar<SCALARTYPE> vcl_t;
        typedef SCALARTYPE ScalarType;
        gpu_symbolic_scalar(vcl_t const & vcl_scal) : vcl_scal_(vcl_scal){ }
        void const * handle() const{ return static_cast<void const *>(&vcl_scal_.handle()); }
        void enqueue(unsigned int & n_arg, viennacl::ocl::kernel & k) const{  k.arg(n_arg++,vcl_scal_); }
        void bind(std::map<void const *, shared_infos_t>  & shared_infos, std::map<kernel_argument*,void const *,deref_less> & temporaries_map){
            infos_= &shared_infos.insert(std::make_pair(handle(),shared_infos_t(shared_infos.size(),print_type<ScalarType>::value(),sizeof(ScalarType)))).first->second;
        }
        std::string repr() const{ return "pscal"+repr_of<SCALARTYPE>::value(); }
    private:
        vcl_t const & vcl_scal_;
    };


    /**
      * @brief Symbolic vector type
      *
      * @tparam SCALARTYPE The Scalartype of the vector in the generated code
      * @tparam ALIGNMENT The Alignment of the vector in the generated code
      */
      template <typename SCALARTYPE>
      class symbolic_vector : public vec_infos_base{
        private:
          typedef symbolic_vector<SCALARTYPE> self_type;
        public:
          typedef viennacl::vector<SCALARTYPE> vcl_vec_t;
          typedef SCALARTYPE ScalarType;
          symbolic_vector(vcl_vec_t const & vcl_vec) : vcl_vec_(vcl_vec){ }
          virtual void const * handle() const{ return static_cast<void const *>(&vcl_vec_.handle()); }
          void enqueue(unsigned int & n_arg, viennacl::ocl::kernel & k) const{
              k.arg(n_arg++,vcl_vec_);
              k.arg(n_arg++,cl_uint(vcl_vec_.internal_size()/infos_->alignment));
          }
          void bind(std::map<void const *, shared_infos_t>  & shared_infos, std::map<kernel_argument*,void const *,deref_less> & temporaries_map){
              infos_= &shared_infos.insert(std::make_pair((void const *)&vcl_vec_.handle(),shared_infos_t(shared_infos.size(),print_type<ScalarType>::value(),sizeof(ScalarType)))).first->second;
          }
          std::string repr() const{
              return "vec"+repr_of<SCALARTYPE>::value();
          }
          size_t real_size() const{ return vcl_vec_.size(); }
        private:
          vcl_vec_t const & vcl_vec_;
      };


      /**
      * @brief Symbolic matrix type
      *
      * @tparam SCALARTYPE The Scalartype of the matrix in the generated code
      * @tparam F The Layout of the matrix in the generated code
      * @tparam ALIGNMENT The Alignment of the matrix in the generated code
      */
      template<class VCL_MATRIX>
      class symbolic_matrix : public mat_infos_base
      {
          typedef symbolic_matrix<VCL_MATRIX> self_type;

        public:
          typedef typename VCL_MATRIX::value_type::value_type ScalarType;
          symbolic_matrix(VCL_MATRIX const & vcl_mat) : mat_infos_base(are_same_type<typename VCL_MATRIX::orientation_category,viennacl::row_major_tag>::value), vcl_mat_(vcl_mat){ }
          size_t real_size1() const{ return vcl_mat_.size1(); }
          size_t real_size2() const{ return vcl_mat_.size2(); }
          void enqueue(unsigned int & n_arg, viennacl::ocl::kernel & k) const{
              cl_uint size1_arg = cl_uint(vcl_mat_.internal_size1());
              cl_uint size2_arg = cl_uint(vcl_mat_.internal_size2());

              if(is_rowmajor_) size2_arg /= infos_->alignment;
              else size1_arg /= infos_->alignment;

              k.arg(n_arg++,vcl_mat_);
              k.arg(n_arg++,cl_uint(0));
              k.arg(n_arg++,cl_uint(1));
              k.arg(n_arg++,cl_uint(0));
              k.arg(n_arg++,cl_uint(1));
              k.arg(n_arg++,size1_arg);
              k.arg(n_arg++,size2_arg);
          }
          void bind(std::map<void const *, shared_infos_t>  & shared_infos, std::map<kernel_argument*,void const *,deref_less> & temporaries_map){
              infos_= &shared_infos.insert(std::make_pair(handle(),shared_infos_t(shared_infos.size(),print_type<ScalarType>::value(),sizeof(ScalarType)))).first->second;
          }
          void const * handle() const{ return static_cast<void const *>(&vcl_mat_.handle()); }
          std::string repr() const{
              return "mat"+repr_of<typename VCL_MATRIX::value_type::value_type>::value()+(is_rowmajor_?'R':'C');
          }
      private:
          VCL_MATRIX const & vcl_mat_;
      };


      template<class SUB>
      class unary_matrix_expression<SUB, replicate_type> : public unary_matrix_expression_infos_base{
      private:
          template<class ScalarType>
          void access_index_impl(symbolic_vector<ScalarType>* sub, unsigned int i, std::string const & ind0, std::string const & ind1){
              std::string new_ind0 = ind0;
              std::string new_ind1 = "0";
              if(m_>1) new_ind0+="%" + sub->size();
              sub->access_index(i,new_ind0,new_ind1);
          }

          template<class MatrixType>
          void access_index_impl(symbolic_matrix<MatrixType> * sub, unsigned int i, std::string const & ind0, std::string const & ind1){
              std::string new_ind0 = ind0;
              std::string new_ind1 = ind1;
              if(m_>1) new_ind0 += "%" + sub->internal_size1();
              if(k_>1) new_ind1 += "%" + sub->internal_size2();
              sub->access_index(i,new_ind0,new_ind1);
          }
      public:
          typedef typename SUB::ScalarType ScalarType;
          unary_matrix_expression(SUB const & sub,unsigned int m,unsigned int k) :unary_matrix_expression_infos_base(new SUB(sub), new replicate_type())
                                                                                    , m_(m)
                                                                                    , k_(k){ }
          void access_index(unsigned int i, std::string const & ind0, std::string const & ind1){
              access_index_impl(dynamic_cast<SUB *>(sub_.get()),i,ind0,ind1);
          }
      private:
          unsigned int m_;
          unsigned int k_;
      };

      template<class SCALARTYPE>
      class unary_vector_expression<symbolic_vector<SCALARTYPE>, shift_type> : public unary_vector_expression_infos_base{
      private:
          std::string access_buffer(unsigned int i) const {
              std::string x = "(int)"+casted_sub_->get_access_index(i) + " + " + k_.name();
              std::string min = "0";
              std::string max = "(int)"+casted_sub_->size() + "-1";
              std::string ind = "min(max("+x+","+min+"),"+max+")";
              return casted_sub_->name() + '[' + ind + ']';
          }
      public:
          typedef SCALARTYPE ScalarType;
          unary_vector_expression(symbolic_vector<SCALARTYPE> const & sub,  unsigned int k) : unary_vector_expression_infos_base(new symbolic_vector<SCALARTYPE>(sub), new shift_type()), k_(k){
              casted_sub_ = dynamic_cast<vec_infos_base*>(sub_.get());
          }

          void bind(std::map<void const *, shared_infos_t>  & shared_infos, std::map<kernel_argument*,void const *,deref_less> & temporaries_map){
              unary_vector_expression_infos_base::bind(shared_infos,temporaries_map);
              k_.bind(shared_infos,temporaries_map);

          }

          void fetch(unsigned int i, kernel_generation_stream & kss){
              if(private_values_[i].empty()){
                  std::string val = casted_sub_->name() + "_shift_" + k_.name() + "_" + to_string(i);
                  std::string aligned_scalartype = casted_sub_->scalartype();
                  if(casted_sub_->alignment() > 1) aligned_scalartype += to_string(casted_sub_->alignment());
                  kss << aligned_scalartype << " " << val << " = " << access_buffer(i) << ";" << std::endl;
                  private_values_[i] = val;
              }
          }

          void write_back(unsigned int i, kernel_generation_stream & kss){ private_values_[i].clear(); }

          std::string generate(unsigned int i, int vector_element = -1) const {
              std::string res;
              std::map<unsigned int, std::string>::const_iterator it = private_values_.find(i);
              if(it==private_values_.end()) res = access_buffer(i);
              else res = it->second;
              if(vector_element >= 0 && casted_sub_->alignment() > 1) res += ".s" + to_string(vector_element);
              return res;
          }

          void get_kernel_arguments(std::map<kernel_argument const *, std::string, deref_less> & args) const{
              k_.get_kernel_arguments(args);
              casted_sub_->get_kernel_arguments(args);
          }
      private:
          cpu_symbolic_scalar<int> k_;
          std::map<unsigned int, std::string> private_values_;
          vec_infos_base* casted_sub_;
      };



  } // namespace generator
} // namespace viennacl


#endif
