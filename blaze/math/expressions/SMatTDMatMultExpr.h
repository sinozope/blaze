//=================================================================================================
/*!
//  \file blaze/math/expressions/SMatTDMatMultExpr.h
//  \brief Header file for the sparse matrix/transpose dense matrix multiplication expression
//
//  Copyright (C) 2012-2017 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_EXPRESSIONS_SMATTDMATMULTEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_SMATTDMATMULTEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/Aliases.h>
#include <blaze/math/constraints/ColumnMajorMatrix.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/MatMatMultExpr.h>
#include <blaze/math/constraints/RequiresEvaluation.h>
#include <blaze/math/constraints/RowMajorMatrix.h>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/Exception.h>
#include <blaze/math/expressions/Computation.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/expressions/MatMatMultExpr.h>
#include <blaze/math/functors/DeclDiag.h>
#include <blaze/math/functors/DeclHerm.h>
#include <blaze/math/functors/DeclLow.h>
#include <blaze/math/functors/DeclSym.h>
#include <blaze/math/functors/DeclUpp.h>
#include <blaze/math/functors/Noop.h>
#include <blaze/math/shims/Conjugate.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/shims/Serial.h>
#include <blaze/math/traits/MultTrait.h>
#include <blaze/math/typetraits/Columns.h>
#include <blaze/math/typetraits/IsAligned.h>
#include <blaze/math/typetraits/IsComputation.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsLower.h>
#include <blaze/math/typetraits/IsStrictlyLower.h>
#include <blaze/math/typetraits/IsStrictlyUpper.h>
#include <blaze/math/typetraits/IsSymmetric.h>
#include <blaze/math/typetraits/IsTriangular.h>
#include <blaze/math/typetraits/IsUniLower.h>
#include <blaze/math/typetraits/IsUniUpper.h>
#include <blaze/math/typetraits/IsUpper.h>
#include <blaze/math/typetraits/RequiresEvaluation.h>
#include <blaze/math/typetraits/Rows.h>
#include <blaze/system/Optimizations.h>
#include <blaze/system/Thresholds.h>
#include <blaze/util/algorithms/Max.h>
#include <blaze/util/algorithms/Min.h>
#include <blaze/util/Assert.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/FunctionTrace.h>
#include <blaze/util/IntegralConstant.h>
#include <blaze/util/InvalidType.h>
#include <blaze/util/mpl/And.h>
#include <blaze/util/mpl/Bool.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/mpl/Or.h>
#include <blaze/util/TrueType.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsBuiltin.h>
#include <blaze/util/typetraits/RemoveReference.h>


namespace blaze {

//=================================================================================================
//
//  CLASS SMATTDMATMULTEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for sparse matrix-transpose dense matrix multiplications.
// \ingroup dense_matrix_expression
//
// The SMatTDMatMultExpr class represents the compile time expression for multiplications between
// a row-major sparse matrix and a column-major dense matrix.
*/
template< typename MT1  // Type of the left-hand side sparse matrix
        , typename MT2  // Type of the right-hand side dense matrix
        , bool SF       // Symmetry flag
        , bool HF       // Hermitian flag
        , bool LF       // Lower flag
        , bool UF >     // Upper flag
class SMatTDMatMultExpr
   : public MatMatMultExpr< DenseMatrix< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF>, false > >
   , private Computation
{
 private:
   //**Type definitions****************************************************************************
   using RT1 = ResultType_<MT1>;     //!< Result type of the left-hand side sparse matrix expression.
   using RT2 = ResultType_<MT2>;     //!< Result type of the right-hand side dense matrix expression.
   using ET1 = ElementType_<RT1>;    //!< Element type of the left-hand side dense matrix expression.
   using ET2 = ElementType_<RT2>;    //!< Element type of the right-hand side sparse matrix expression.
   using CT1 = CompositeType_<MT1>;  //!< Composite type of the left-hand side sparse matrix expression.
   using CT2 = CompositeType_<MT2>;  //!< Composite type of the right-hand side dense matrix expression.
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the composite type of the left-hand side sparse matrix expression.
   enum : bool { evaluateLeft = IsComputation<MT1>::value || RequiresEvaluation<MT1>::value };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the composite type of the right-hand side dense matrix expression.
   enum : bool { evaluateRight = IsComputation<MT2>::value || RequiresEvaluation<MT2>::value };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switches for the kernel generation.
   enum : bool {
      SYM  = ( SF && !( HF || LF || UF )    ),  //!< Flag for symmetric matrices.
      HERM = ( HF && !( LF || UF )          ),  //!< Flag for Hermitian matrices.
      LOW  = ( LF || ( ( SF || HF ) && UF ) ),  //!< Flag for lower matrices.
      UPP  = ( UF || ( ( SF || HF ) && LF ) )   //!< Flag for upper matrices.
   };
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! The CanExploitSymmetry struct is a helper struct for the selection of the optimal
       evaluation strategy. In case the right-hand side dense matrix operand is symmetric,
       \a value is set to 1 and an optimized evaluation strategy is selected. Otherwise
       \a value is set to 0 and the default strategy is chosen. */
   template< typename T1, typename T2, typename T3 >
   struct CanExploitSymmetry {
      enum : bool { value = IsSymmetric<T3>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! The IsEvaluationRequired struct is a helper struct for the selection of the parallel
       evaluation strategy. In case either of the two matrix operands requires an intermediate
       evaluation, the nested \value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct IsEvaluationRequired {
      enum : bool { value = ( evaluateLeft || evaluateRight ) &&
                            CanExploitSymmetry<T1,T2,T3>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case the left-hand side matrix is not a diagonal and a loop-unrolled computation is
       feasible, the nested \value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseOptimizedKernel {
      enum : bool { value = useOptimizedKernels &&
                            !IsDiagonal<T3>::value &&
                            !IsResizable< ElementType_<T1> >::value &&
                            !IsResizable<ET1>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Type of the functor for forwarding an expression to another assign kernel.
   /*! In case a temporary matrix needs to be created, this functor is used to forward the
       resulting expression to another assign kernel. */
   using ForwardFunctor = IfTrue_< HERM
                                 , DeclHerm
                                 , IfTrue_< SYM
                                          , DeclSym
                                          , IfTrue_< LOW
                                                   , IfTrue_< UPP
                                                            , DeclDiag
                                                            , DeclLow >
                                                   , IfTrue_< UPP
                                                            , DeclUpp
                                                            , Noop > > > >;
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   //! Type of this SMatTDMatMultExpr instance.
   using This = SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF>;

   using ResultType    = MultTrait_<RT1,RT2>;         //!< Result type for expression template evaluations.
   using OppositeType  = OppositeType_<ResultType>;   //!< Result type with opposite storage order for expression template evaluations.
   using TransposeType = TransposeType_<ResultType>;  //!< Transpose type for expression template evaluations.
   using ElementType   = ElementType_<ResultType>;    //!< Resulting element type.
   using ReturnType    = const ElementType;           //!< Return type for expression template evaluations.
   using CompositeType = const ResultType;            //!< Data type for composite expression templates.

   //! Composite type of the left-hand side sparse matrix expression.
   using LeftOperand = If_< IsExpression<MT1>, const MT1, const MT1& >;

   //! Composite type of the right-hand side dense matrix expression.
   using RightOperand = If_< IsExpression<MT2>, const MT2, const MT2& >;

   //! Type for the assignment of the left-hand side sparse matrix operand.
   using LT = IfTrue_< evaluateLeft, const RT1, CT1 >;

   //! Type for the assignment of the right-hand side dense matrix operand.
   using RT = IfTrue_< evaluateRight, const RT2, CT2 >;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum : bool { simdEnabled = false };

   //! Compilation switch for the expression template assignment strategy.
   enum : bool { smpAssignable = !evaluateLeft  && MT1::smpAssignable &&
                                 !evaluateRight && MT2::smpAssignable };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the SMatTDMatMultExpr class.
   //
   // \param lhs The left-hand side sparse matrix operand of the multiplication expression.
   // \param rhs The right-hand side dense matrix operand of the multiplication expression.
   */
   explicit inline SMatTDMatMultExpr( const MT1& lhs, const MT2& rhs ) noexcept
      : lhs_( lhs )  // Left-hand side sparse matrix of the multiplication expression
      , rhs_( rhs )  // Right-hand side dense matrix of the multiplication expression
   {
      BLAZE_INTERNAL_ASSERT( lhs.columns() == rhs.rows(), "Invalid matrix sizes" );
   }
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   */
   inline ReturnType operator()( size_t i, size_t j ) const {
      BLAZE_INTERNAL_ASSERT( i < lhs_.rows()   , "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < rhs_.columns(), "Invalid column access index" );

      if( IsDiagonal<MT1>::value ) {
         return lhs_(i,i) * rhs_(i,j);
      }
      else if( IsDiagonal<MT2>::value ) {
         return lhs_(i,j) * rhs_(j,j);
      }
      else if( IsTriangular<MT1>::value || IsTriangular<MT2>::value ) {
         const size_t begin( ( IsUpper<MT1>::value )
                             ?( ( IsLower<MT2>::value )
                                ?( max( ( IsStrictlyUpper<MT1>::value ? i+1UL : i )
                                      , ( IsStrictlyLower<MT2>::value ? j+1UL : j ) ) )
                                :( IsStrictlyUpper<MT1>::value ? i+1UL : i ) )
                             :( ( IsLower<MT2>::value )
                                ?( IsStrictlyLower<MT2>::value ? j+1UL : j )
                                :( 0UL ) ) );
         const size_t end( ( IsLower<MT1>::value )
                           ?( ( IsUpper<MT2>::value )
                              ?( min( ( IsStrictlyLower<MT1>::value ? i : i+1UL )
                                    , ( IsStrictlyUpper<MT2>::value ? j : j+1UL ) ) )
                              :( IsStrictlyLower<MT1>::value ? i : i+1UL ) )
                           :( ( IsUpper<MT2>::value )
                              ?( IsStrictlyUpper<MT2>::value ? j : j+1UL )
                              :( lhs_.columns() ) ) );

         if( begin >= end ) return ElementType();

         const size_t n( end - begin );

         return subvector( row( lhs_, i ), begin, n ) * subvector( column( rhs_, j ), begin, n );
      }
      else {
         return row( lhs_, i ) * column( rhs_, j );
      }
   }
   //**********************************************************************************************

   //**At function*********************************************************************************
   /*!\brief Checked access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   // \exception std::out_of_range Invalid matrix access index.
   */
   inline ReturnType at( size_t i, size_t j ) const {
      if( i >= lhs_.rows() ) {
         BLAZE_THROW_OUT_OF_RANGE( "Invalid row access index" );
      }
      if( j >= rhs_.columns() ) {
         BLAZE_THROW_OUT_OF_RANGE( "Invalid column access index" );
      }
      return (*this)(i,j);
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const noexcept {
      return lhs_.rows();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const noexcept {
      return rhs_.columns();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side sparse matrix operand.
   //
   // \return The left-hand side sparse matrix operand.
   */
   inline LeftOperand leftOperand() const noexcept {
      return lhs_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side transpose dense matrix operand.
   //
   // \return The right-hand side transpose dense matrix operand.
   */
   inline RightOperand rightOperand() const noexcept {
      return rhs_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can alias with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case the expression can alias, \a false otherwise.
   */
   template< typename T >
   inline bool canAlias( const T* alias ) const noexcept {
      return ( lhs_.isAliased( alias ) || rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an alias effect is detected, \a false otherwise.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const noexcept {
      return ( lhs_.isAliased( alias ) || rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the operands of the expression are properly aligned in memory.
   //
   // \return \a true in case the operands are aligned, \a false if not.
   */
   inline bool isAligned() const noexcept {
      return rhs_.isAligned();
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can be used in SMP assignments.
   //
   // \return \a true in case the expression can be used in SMP assignments, \a false if not.
   */
   inline bool canSMPAssign() const noexcept {
      return ( rows() * columns() >= SMP_SMATTDMATMULT_THRESHOLD ) && !IsDiagonal<MT2>::value;
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  lhs_;  //!< Left-hand side sparse matrix of the multiplication expression.
   RightOperand rhs_;  //!< Right-hand side dense matrix of the multiplication expression.
   //**********************************************************************************************

   //**Assignment to dense matrices****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a sparse matrix-transpose dense matrix multiplication to a dense matrix
   //        (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a sparse matrix-transpose
   // dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline DisableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      assign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side sparse matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      SMatTDMatMultExpr::selectAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default assignment to dense matrices********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a sparse matrix-transpose dense matrix multiplication
   //        (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the default assignment kernel for the sparse matrix-transpose
   // dense matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline DisableIf_< UseOptimizedKernel<MT3,MT4,MT5> >
      selectAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using ConstIterator = ConstIterator_<MT4>;

      const size_t M( A.rows()    );
      const size_t N( B.columns() );

      BLAZE_INTERNAL_ASSERT( !( SYM || HERM || LOW || UPP ) || M == N, "Broken invariant detected" );

      {
         size_t j( 0UL );

         for( ; (j+4UL) <= N; j+=4UL ) {
            for( size_t i=( SYM || HERM || LOW ? j : 0UL ); i<( UPP ? j+4UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+4UL) : A.upperBound(i,j+4UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               if( element == end ) {
                  reset( C(i,j    ) );
                  reset( C(i,j+1UL) );
                  reset( C(i,j+2UL) );
                  reset( C(i,j+3UL) );
                  continue;
               }

               C(i,j    ) = element->value() * B(element->index(),j    );
               C(i,j+1UL) = element->value() * B(element->index(),j+1UL);
               C(i,j+2UL) = element->value() * B(element->index(),j+2UL);
               C(i,j+3UL) = element->value() * B(element->index(),j+3UL);
               ++element;
               for( ; element!=end; ++element ) {
                  C(i,j    ) += element->value() * B(element->index(),j    );
                  C(i,j+1UL) += element->value() * B(element->index(),j+1UL);
                  C(i,j+2UL) += element->value() * B(element->index(),j+2UL);
                  C(i,j+3UL) += element->value() * B(element->index(),j+3UL);
               }
            }
         }

         for( ; (j+2UL) <= N; j+=2UL ) {
            for( size_t i=( SYM || HERM || LOW ? j : 0UL ); i<( UPP ? j+2UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+2UL) : A.upperBound(i,j+2UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               if( element == end ) {
                  reset( C(i,j    ) );
                  reset( C(i,j+1UL) );
                  continue;
               }

               C(i,j    ) = element->value() * B(element->index(),j    );
               C(i,j+1UL) = element->value() * B(element->index(),j+1UL);
               ++element;
               for( ; element!=end; ++element ) {
                  C(i,j    ) += element->value() * B(element->index(),j    );
                  C(i,j+1UL) += element->value() * B(element->index(),j+1UL);
               }
            }
         }

         for( ; j<N; ++j ) {
            for( size_t i=( SYM || HERM || LOW ? j : 0UL ); i<( UPP ? j+1UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j) : A.upperBound(i,j) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               if( element == end ) {
                  reset( C(i,j) );
                  continue;
               }

               C(i,j) = element->value() * B(element->index(),j);
               ++element;
               for( ; element!=end; ++element ) {
                  C(i,j) += element->value() * B(element->index(),j);
               }
            }
         }
      }

      if( SYM || HERM ) {
         for( size_t j=1UL; j<N; ++j ) {
            for( size_t i=0UL; i<j; ++i ) {
               C(i,j) = HERM ? conj( C(j,i) ) : C(j,i);
            }
         }
      }
      else if( LOW && !UPP ) {
         for( size_t j=1UL; j<N; ++j ) {
            for( size_t i=0UL; i<j; ++i ) {
               reset( C(i,j) );
            }
         }
      }
      else if( !LOW && UPP ) {
         for( size_t i=1UL; i<M; ++i ) {
            for( size_t j=0UL; j<i; ++j ) {
               reset( C(i,j) );
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Optimized assignment to dense matrices******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Optimized assignment of a sparse matrix-transpose dense matrix multiplication
   //        (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the optimized assignment kernel for the sparse matrix-transpose
   // dense matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline EnableIf_< UseOptimizedKernel<MT3,MT4,MT5> >
      selectAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using ConstIterator = ConstIterator_<MT4>;

      const size_t M( A.rows()    );
      const size_t N( B.columns() );

      BLAZE_INTERNAL_ASSERT( !( SYM || HERM || LOW || UPP ) || M == N, "Broken invariant detected" );

      reset( C );

      {
         size_t j( 0UL );

         for( ; (j+4UL) <= N; j+=4UL ) {
            for( size_t i=( SYM || HERM || LOW ? j : 0UL ); i<( UPP ? j+4UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+4UL) : A.upperBound(i,j+4UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j    ) += v1 * B(i1,j    ) + v2 * B(i2,j    ) + v3 * B(i3,j    ) + v4 * B(i4,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL) + v2 * B(i2,j+1UL) + v3 * B(i3,j+1UL) + v4 * B(i4,j+1UL);
                  C(i,j+2UL) += v1 * B(i1,j+2UL) + v2 * B(i2,j+2UL) + v3 * B(i3,j+2UL) + v4 * B(i4,j+2UL);
                  C(i,j+3UL) += v1 * B(i1,j+3UL) + v2 * B(i2,j+3UL) + v3 * B(i3,j+3UL) + v4 * B(i4,j+3UL);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j    ) += v1 * B(i1,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL);
                  C(i,j+2UL) += v1 * B(i1,j+2UL);
                  C(i,j+3UL) += v1 * B(i1,j+3UL);
               }
            }
         }

         for( ; (j+2UL) <= N; j+=2UL ) {
            for( size_t i=( SYM || HERM || LOW ? j : 0UL ); i<( UPP ? j+2UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+2UL) : A.upperBound(i,j+2UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j    ) += v1 * B(i1,j    ) + v2 * B(i2,j    ) + v3 * B(i3,j    ) + v4 * B(i4,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL) + v2 * B(i2,j+1UL) + v3 * B(i3,j+1UL) + v4 * B(i4,j+1UL);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j    ) += v1 * B(i1,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL);
               }
            }
         }

         for( ; j<N; ++j ) {
            for( size_t i=( SYM || HERM || LOW ? j : 0UL ); i<( UPP ? j+1UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j) : A.upperBound(i,j) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j) += v1 * B(i1,j) + v2 * B(i2,j) + v3 * B(i3,j) + v4 * B(i4,j);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j) += v1 * B(i1,j);
               }
            }
         }
      }

      if( SYM || HERM ) {
         for( size_t j=1UL; j<N; ++j ) {
            for( size_t i=0UL; i<j; ++i ) {
               C(i,j) = HERM ? conj( C(j,i) ) : C(j,i);
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to sparse matrices***************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a sparse matrix-transpose dense matrix multiplication to a sparse matrix
   //        (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a sparse matrix-transpose
   // dense matrix multiplication expression to a sparse matrix.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline DisableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      assign( SparseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      using TmpType = IfTrue_< SO, OppositeType, ResultType >;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TmpType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      const TmpType tmp( serial( rhs ) );
      assign( ~lhs, fwd( tmp ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring assignment********************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring assignment of a sparse matrix-transpose dense matrix multiplication
   //        (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the symmetry-based restructuring assignment of a sparse matrix-
   // transpose dense matrix multiplication expression. Due to the explicit application of the
   // SFINAE principle this function can only be selected by the compiler in case the symmetry
   // of either of the two matrix operands can be exploited.
   */
   template< typename MT  // Type of the target matrix
           , bool SO >    // Storage order of the target matrix
   friend inline EnableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      assign( Matrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      assign( ~lhs, fwd( rhs.lhs_ * trans( rhs.rhs_ ) ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense matrices*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a sparse matrix-transpose dense matrix multiplication
   //        to a dense matrix (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a sparse matrix-
   // transpose dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline DisableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      addAssign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side sparse matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      SMatTDMatMultExpr::selectAddAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default addition assignment to dense matrices***********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default addition assignment of a sparse matrix-transpose dense matrix multiplication
   //        (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the default addition assignment kernel for the sparse matrix-
   // transpose dense matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline DisableIf_< UseOptimizedKernel<MT3,MT4,MT5> >
      selectAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using ConstIterator = ConstIterator_<MT4>;

      const size_t M( A.rows()    );
      const size_t N( B.columns() );

      BLAZE_INTERNAL_ASSERT( !( LOW || UPP ) || M == N, "Broken invariant detected" );

      {
         size_t j( 0UL );

         for( ; (j+4UL) <= N; j+=4UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+4UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+4UL) : A.upperBound(i,j+4UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               for( ; element!=end; ++element ) {
                  C(i,j    ) += element->value() * B(element->index(),j    );
                  C(i,j+1UL) += element->value() * B(element->index(),j+1UL);
                  C(i,j+2UL) += element->value() * B(element->index(),j+2UL);
                  C(i,j+3UL) += element->value() * B(element->index(),j+3UL);
               }
            }
         }

         for( ; (j+2UL) <= N; j+=2UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+2UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+2UL) : A.upperBound(i,j+2UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               for( ; element!=end; ++element ) {
                  C(i,j    ) += element->value() * B(element->index(),j    );
                  C(i,j+1UL) += element->value() * B(element->index(),j+1UL);
               }
            }
         }

         for( ; j<N; ++j ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+1UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j) : A.upperBound(i,j) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               for( ; element!=end; ++element ) {
                  C(i,j) += element->value() * B(element->index(),j);
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Optimized addition assignment to dense matrices*********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Optimized addition assignment of a sparse matrix-transpose dense matrix multiplication
   //        (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the optimized addition assignment kernel for the sparse matrix-
   // transpose dense matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline EnableIf_< UseOptimizedKernel<MT3,MT4,MT5> >
      selectAddAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using ConstIterator = ConstIterator_<MT4>;

      const size_t M( A.rows()    );
      const size_t N( B.columns() );

      BLAZE_INTERNAL_ASSERT( !( LOW || UPP ) || M == N, "Broken invariant detected" );

      {
         size_t j( 0UL );

         for( ; (j+4UL) <= N; j+=4UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+4UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+4UL) : A.upperBound(i,j+4UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j    ) += v1 * B(i1,j    ) + v2 * B(i2,j    ) + v3 * B(i3,j    ) + v4 * B(i4,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL) + v2 * B(i2,j+1UL) + v3 * B(i3,j+1UL) + v4 * B(i4,j+1UL);
                  C(i,j+2UL) += v1 * B(i1,j+2UL) + v2 * B(i2,j+2UL) + v3 * B(i3,j+2UL) + v4 * B(i4,j+2UL);
                  C(i,j+3UL) += v1 * B(i1,j+3UL) + v2 * B(i2,j+3UL) + v3 * B(i3,j+3UL) + v4 * B(i4,j+3UL);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j    ) += v1 * B(i1,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL);
                  C(i,j+2UL) += v1 * B(i1,j+2UL);
                  C(i,j+3UL) += v1 * B(i1,j+3UL);
               }
            }
         }

         for( ; (j+2UL) <= N; j+=2UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+2UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+2UL) : A.upperBound(i,j+2UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j    ) += v1 * B(i1,j    ) + v2 * B(i2,j    ) + v3 * B(i3,j    ) + v4 * B(i4,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL) + v2 * B(i2,j+1UL) + v3 * B(i3,j+1UL) + v4 * B(i4,j+1UL);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j    ) += v1 * B(i1,j    );
                  C(i,j+1UL) += v1 * B(i1,j+1UL);
               }
            }
         }

         for( ; j<N; ++j ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+1UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j) : A.upperBound(i,j) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j) += v1 * B(i1,j) + v2 * B(i2,j) + v3 * B(i3,j) + v4 * B(i4,j);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j) += v1 * B(i1,j);
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring addition assignment***********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring addition assignment of a sparse matrix-transpose dense matrix
   //        multiplication (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the symmetry-based restructuring addition assignment of a sparse
   // matrix-transpose dense matrix multiplication expression. Due to the explicit application
   // of the SFINAE principle this function can only be selected by the compiler in case the
   // symmetry of either of the two matrix operands can be exploited.
   */
   template< typename MT  // Type of the target matrix
           , bool SO >    // Storage order of the target matrix
   friend inline EnableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      addAssign( Matrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      addAssign( ~lhs, fwd( rhs.lhs_ * trans( rhs.rhs_ ) ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to sparse matrices******************************************************
   // No special implementation for the addition assignment to sparse matrices.
   //**********************************************************************************************

   //**Subtraction assignment to dense matrices****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a sparse matrix-transpose dense matrix multiplication
   //        to a dense matrix (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a sparse matrix-
   // transpose dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline DisableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      subAssign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side sparse matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      SMatTDMatMultExpr::selectSubAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default subtraction assignment to dense matrices********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default subtraction assignment of a sparse matrix-transpose dense matrix multiplication
   //        (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the default subtraction assignment kernel for the sparse matrix-
   // transpose dense matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline DisableIf_< UseOptimizedKernel<MT3,MT4,MT5> >
      selectSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using ConstIterator = ConstIterator_<MT4>;

      const size_t M( A.rows()    );
      const size_t N( B.columns() );

      BLAZE_INTERNAL_ASSERT( !( LOW || UPP ) || M == N, "Broken invariant detected" );

      {
         size_t j( 0UL );

         for( ; (j+4UL) <= N; j+=4UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+4UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+4UL) : A.upperBound(i,j+4UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               for( ; element!=end; ++element ) {
                  C(i,j    ) -= element->value() * B(element->index(),j    );
                  C(i,j+1UL) -= element->value() * B(element->index(),j+1UL);
                  C(i,j+2UL) -= element->value() * B(element->index(),j+2UL);
                  C(i,j+3UL) -= element->value() * B(element->index(),j+3UL);
               }
            }
         }

         for( ; (j+2UL) <= N; j+=2UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+2UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+2UL) : A.upperBound(i,j+2UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               for( ; element!=end; ++element ) {
                  C(i,j    ) -= element->value() * B(element->index(),j    );
                  C(i,j+1UL) -= element->value() * B(element->index(),j+1UL);
               }
            }
         }

         for( ; j<N; ++j ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+1UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j) : A.upperBound(i,j) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               for( ; element!=end; ++element ) {
                  C(i,j) -= element->value() * B(element->index(),j);
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Optimized subtraction assignment to dense matrices******************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Optimized subtraction assignment of a sparse matrix-transpose dense matrix
   //        multiplication (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side multiplication operand.
   // \param B The right-hand side multiplication operand.
   // \return void
   //
   // This function implements the optimized subtraction assignment kernel for the sparse matrix-
   // transpose dense matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline EnableIf_< UseOptimizedKernel<MT3,MT4,MT5> >
      selectSubAssignKernel( MT3& C, const MT4& A, const MT5& B )
   {
      using ConstIterator = ConstIterator_<MT4>;

      const size_t M( A.rows()    );
      const size_t N( B.columns() );

      BLAZE_INTERNAL_ASSERT( !( LOW || UPP ) || M == N, "Broken invariant detected" );

      {
         size_t j( 0UL );

         for( ; (j+4UL) <= N; j+=4UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+4UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+4UL) : A.upperBound(i,j+4UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j    ) -= v1 * B(i1,j    ) + v2 * B(i2,j    ) + v3 * B(i3,j    ) + v4 * B(i4,j    );
                  C(i,j+1UL) -= v1 * B(i1,j+1UL) + v2 * B(i2,j+1UL) + v3 * B(i3,j+1UL) + v4 * B(i4,j+1UL);
                  C(i,j+2UL) -= v1 * B(i1,j+2UL) + v2 * B(i2,j+2UL) + v3 * B(i3,j+2UL) + v4 * B(i4,j+2UL);
                  C(i,j+3UL) -= v1 * B(i1,j+3UL) + v2 * B(i2,j+3UL) + v3 * B(i3,j+3UL) + v4 * B(i4,j+3UL);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j    ) -= v1 * B(i1,j    );
                  C(i,j+1UL) -= v1 * B(i1,j+1UL);
                  C(i,j+2UL) -= v1 * B(i1,j+2UL);
                  C(i,j+3UL) -= v1 * B(i1,j+3UL);
               }
            }
         }

         for( ; (j+2UL) <= N; j+=2UL ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+2UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j+2UL) : A.upperBound(i,j+2UL) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j    ) -= v1 * B(i1,j    ) + v2 * B(i2,j    ) + v3 * B(i3,j    ) + v4 * B(i4,j    );
                  C(i,j+1UL) -= v1 * B(i1,j+1UL) + v2 * B(i2,j+1UL) + v3 * B(i3,j+1UL) + v4 * B(i4,j+1UL);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j    ) -= v1 * B(i1,j    );
                  C(i,j+1UL) -= v1 * B(i1,j+1UL);
               }
            }
         }

         for( ; j<N; ++j ) {
            for( size_t i=( LOW ? j : 0UL ); i<( UPP ? j+1UL : M ); ++i )
            {
               const ConstIterator end( ( IsUpper<MT5>::value )
                                        ?( IsStrictlyUpper<MT5>::value ? A.lowerBound(i,j) : A.upperBound(i,j) )
                                        :( A.end(i) ) );
               ConstIterator element( ( IsLower<MT5>::value )
                                      ?( IsStrictlyLower<MT5>::value ? A.upperBound(i,j) : A.lowerBound(i,j) )
                                      :( A.begin(i) ) );

               const size_t nonzeros( end - element );
               const size_t kpos( nonzeros & size_t(-4) );
               BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

               for( size_t k=0UL; k<kpos; k+=4UL )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );
                  ++element;
                  const size_t i2( element->index() );
                  const ET1    v2( element->value() );
                  ++element;
                  const size_t i3( element->index() );
                  const ET1    v3( element->value() );
                  ++element;
                  const size_t i4( element->index() );
                  const ET1    v4( element->value() );
                  ++element;

                  BLAZE_INTERNAL_ASSERT( i1 < i2 && i2 < i3 && i3 < i4, "Invalid sparse matrix index detected" );

                  C(i,j) -= v1 * B(i1,j) + v2 * B(i2,j) + v3 * B(i3,j) + v4 * B(i4,j);
               }

               for( ; element!=end; ++element )
               {
                  const size_t i1( element->index() );
                  const ET1    v1( element->value() );

                  C(i,j) -= v1 * B(i1,j);
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring subtraction assignment********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring subtraction assignment of a sparse matrix-transpose dense matrix
   //        multiplication (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the symmetry-based restructuring subtraction assignment of a sparse
   // matrix-transpose dense matrix multiplication expression. Due to the explicit application of
   // the SFINAE principle this function can only be selected by the compiler in case the symmetry
   // of either of the two matrix operands can be exploited.
   */
   template< typename MT  // Type of the target matrix
           , bool SO >    // Storage order of the target matrix
   friend inline EnableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      subAssign( Matrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      subAssign( ~lhs, fwd( rhs.lhs_ * trans( rhs.rhs_ ) ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to sparse matrices***************************************************
   // No special implementation for the subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**Schur product assignment to dense matrices**************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Schur product assignment of a sparse matrix-transpose dense matrix multiplication
   //        to a dense matrix (\f$ A\circ=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression for the Schur product.
   // \return void
   //
   // This function implements the performance optimized Schur product assignment of a sparse
   // matrix-transpose dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline void schurAssign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ResultType tmp( serial( rhs ) );
      schurAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Schur product assignment to sparse matrices*************************************************
   // No special implementation for the Schur product assignment to sparse matrices.
   //**********************************************************************************************

   //**Multiplication assignment to dense matrices*************************************************
   // No special implementation for the multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**Multiplication assignment to sparse matrices************************************************
   // No special implementation for the multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP assignment to dense matrices************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a sparse matrix-transpose dense matrix multiplication to a dense
   //        matrix (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a sparse matrix-transpose
   // dense matrix multiplication expression to a dense matrix. Due to the explicit application of
   // the SFINAE principle this function can only be selected by the compiler in case either of the
   // two matrix operands requires an intermediate evaluation and no symmetry can be exploited.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< IsEvaluationRequired<MT,MT1,MT2> >
      smpAssign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side sparse matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpAssign( ~lhs, A * B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP assignment to sparse matrices***********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a sparse matrix-transpose dense matrix multiplication to a sparse
   //        matrix (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a sparse matrix-transpose
   // dense matrix multiplication expression to a sparse matrix. Due to the explicit application of
   // the SFINAE principle this function can only be selected by the compiler in case either of the
   // two matrix operands requires an intermediate evaluation and no symmetry can be exploited.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline EnableIf_< IsEvaluationRequired<MT,MT1,MT2> >
      smpAssign( SparseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      using TmpType = IfTrue_< SO, OppositeType, ResultType >;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( TmpType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      const TmpType tmp( rhs );
      smpAssign( ~lhs, fwd( tmp ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring SMP assignment****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring SMP assignment of a sparse matrix-transpose dense matrix multiplication
   //        (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the symmetry-based restructuring SMP assignment of a sparse matrix-
   // transpose dense matrix multiplication expression. Due to the explicit application of the
   // SFINAE principle this function can only be selected by the compiler in case the symmetry of
   // either of the two matrix operands can be exploited.
   */
   template< typename MT  // Type of the target matrix
           , bool SO >    // Storage order of the target matrix
   friend inline EnableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      smpAssign( Matrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      smpAssign( ~lhs, fwd( rhs.lhs_ * trans( rhs.rhs_ ) ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to dense matrices***************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP addition assignment of a sparse matrix-transpose dense matrix multiplication
   //        to a dense matrix (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized SMP addition assignment of a sparse
   // matrix-transpose dense matrix multiplication expression to a dense matrix. Due to the
   // explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case either of the two matrix operands requires an intermediate evaluation
   // and no symmetry can be exploited.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< IsEvaluationRequired<MT,MT1,MT2> >
      smpAddAssign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side sparse matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpAddAssign( ~lhs, A * B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring SMP addition assignment*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring SMP addition assignment of a sparse matrix-transpose dense matrix
   //        multiplication (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the symmetry-based restructuring SMP addition assignment of
   // a sparse matrix-transpose dense matrix multiplication expression. Due to the explicit
   // application of the SFINAE principle this function can only be selected by the compiler
   // in case the symmetry of either of the two matrix operands can be exploited.
   */
   template< typename MT  // Type of the target matrix
           , bool SO >    // Storage order of the target matrix
   friend inline EnableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      smpAddAssign( Matrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      smpAddAssign( ~lhs, fwd( rhs.lhs_ * trans( rhs.rhs_ ) ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to sparse matrices**************************************************
   // No special implementation for the SMP addition assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP subtraction assignment to dense matrices************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP subtraction assignment of a sparse matrix-transpose dense matrix multiplication
   //        to a dense matrix (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized SMP subtraction assignment of a sparse
   // matrix-transpose dense matrix multiplication expression to a dense matrix. Due to the
   // explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case either of the two matrix operands requires an intermediate evaluation
   // and no symmetry can be exploited.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline EnableIf_< IsEvaluationRequired<MT,MT1,MT2> >
      smpSubAssign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side sparse matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side dense matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpSubAssign( ~lhs, A * B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring SMP subtraction assignment****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring SMP subtraction assignment of a sparse matrix-transpose dense matrix
   //        multiplication (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the symmetry-based restructuring SMP subtraction assignment of
   // sparse matrix-transpose dense matrix multiplication expression. Due to the explicit a
   // application of the SFINAE principle this function can only be selected by the compiler
   // in case the symmetry of either of the two matrix operands can be exploited.
   */
   template< typename MT  // Type of the target matrix
           , bool SO >    // Storage order of the target matrix
   friend inline EnableIf_< CanExploitSymmetry<MT,MT1,MT2> >
      smpSubAssign( Matrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ForwardFunctor fwd;

      smpSubAssign( ~lhs, fwd( rhs.lhs_ * trans( rhs.rhs_ ) ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP subtraction assignment to sparse matrices***********************************************
   // No special implementation for the SMP subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP Schur product assignment to dense matrices**********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP Schur product assignment of a sparse matrix-transpose dense matrix multiplication
   //        to a dense matrix (\f$ A\circ=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression for the Schur product.
   // \return void
   //
   // This function implements the performance optimized SMP Schur product assignment of a
   // sparse matrix-transpose dense matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline void smpSchurAssign( DenseMatrix<MT,SO>& lhs, const SMatTDMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const ResultType tmp( rhs );
      smpSchurAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP Schur product assignment to sparse matrices*********************************************
   // No special implementation for the SMP Schur product assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP multiplication assignment to dense matrices*********************************************
   // No special implementation for the SMP multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**SMP multiplication assignment to sparse matrices********************************************
   // No special implementation for the SMP multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_FORM_VALID_MATMATMULTEXPR( MT1, MT2 );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL BINARY ARITHMETIC OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Multiplication operator for the multiplication of a row-major sparse matrix and a
//        column-major dense matrix (\f$ A=B*C \f$).
// \ingroup dense_matrix
//
// \param lhs The left-hand side sparse matrix for the multiplication.
// \param rhs The right-hand side dense matrix for the multiplication.
// \return The resulting matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// This operator represents the multiplication of a row-major sparse matrix and a column-major
// dense matrix:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::CompressedMatrix<double,rowMajor> A;
   blaze::DynamicMatrix<double,columnMajor> B;
   blaze::DynamicMatrix<double,rowMajor> C;
   // ... Resizing and initialization
   C = A * B;
   \endcode

// The operator returns an expression representing a dense matrix of the higher-order element
// type of the two involved matrix element types \a MT1::ElementType and \a MT2::ElementType.
// Both matrix types \a MT1 and \a MT2 as well as the two element types \a MT1::ElementType
// and \a MT2::ElementType have to be supported by the MultTrait class template.\n
// In case the current sizes of the two given matrices don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
inline decltype(auto)
   operator*( const SparseMatrix<MT1,false>& lhs, const DenseMatrix<MT2,true>& rhs )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).columns() != (~rhs).rows() ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Matrix sizes do not match" );
   }

   using ReturnType = const SMatTDMatMultExpr<MT1,MT2,false,false,false,false>;
   return ReturnType( ~lhs, ~rhs );
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Declares the given non-symmetric matrix multiplication expression as symmetric.
// \ingroup dense_matrix
//
// \param dm The input matrix multiplication expression.
// \return The redeclared dense matrix multiplication expression.
// \exception std::invalid_argument Invalid symmetric matrix specification.
//
// The \a declsym function declares the given non-symmetric matrix multiplication expression
// \a dm as symmetric. The function returns an expression representing the operation. In case
// the given expression does not represent a square matrix, a \a std::invalid_argument exception
// is thrown.\n
// The following example demonstrates the use of the \a declsym function:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::CompressedMatrix<double,rowMajor> A;
   blaze::DynamicMatrix<double,columnMajor> B;
   blaze::DynamicMatrix<double,rowMajor> C;
   // ... Resizing and initialization
   C = declsym( A * B );
   \endcode
*/
template< typename MT1  // Type of the left-hand side dense matrix
        , typename MT2  // Type of the right-hand side dense matrix
        , bool SF       // Symmetry flag
        , bool HF       // Hermitian flag
        , bool LF       // Lower flag
        , bool UF >     // Upper flag
inline decltype(auto) declsym( const SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF>& dm )
{
   BLAZE_FUNCTION_TRACE;

   if( !isSquare( dm ) ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid symmetric matrix specification" );
   }

   using ReturnType = const SMatTDMatMultExpr<MT1,MT2,true,HF,LF,UF>;
   return ReturnType( dm.leftOperand(), dm.rightOperand() );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Declares the given non-Hermitian matrix multiplication expression as Hermitian.
// \ingroup dense_matrix
//
// \param dm The input matrix multiplication expression.
// \return The redeclared dense matrix multiplication expression.
// \exception std::invalid_argument Invalid Hermitian matrix specification.
//
// The \a declherm function declares the given non-Hermitian matrix multiplication expression
// \a dm as Hermitian. The function returns an expression representing the operation. In case
// the given expression does not represent a square matrix, a \a std::invalid_argument exception
// is thrown.\n
// The following example demonstrates the use of the \a declherm function:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::CompressedMatrix<double,rowMajor> A;
   blaze::DynamicMatrix<double,columnMajor> B;
   blaze::DynamicMatrix<double,rowMajor> C;
   // ... Resizing and initialization
   C = declherm( A * B );
   \endcode
*/
template< typename MT1  // Type of the left-hand side dense matrix
        , typename MT2  // Type of the right-hand side dense matrix
        , bool SF       // Symmetry flag
        , bool HF       // Hermitian flag
        , bool LF       // Lower flag
        , bool UF >     // Upper flag
inline decltype(auto) declherm( const SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF>& dm )
{
   BLAZE_FUNCTION_TRACE;

   if( !isSquare( dm ) ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid Hermitian matrix specification" );
   }

   using ReturnType = const SMatTDMatMultExpr<MT1,MT2,SF,true,LF,UF>;
   return ReturnType( dm.leftOperand(), dm.rightOperand() );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Declares the given non-lower matrix multiplication expression as lower.
// \ingroup dense_matrix
//
// \param dm The input matrix multiplication expression.
// \return The redeclared dense matrix multiplication expression.
// \exception std::invalid_argument Invalid lower matrix specification.
//
// The \a decllow function declares the given non-lower matrix multiplication expression
// \a dm as lower. The function returns an expression representing the operation. In case
// the given expression does not represent a square matrix, a \a std::invalid_argument
// exception is thrown.\n
// The following example demonstrates the use of the \a decllow function:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::CompressedMatrix<double,rowMajor> A;
   blaze::DynamicMatrix<double,columnMajor> B;
   blaze::DynamicMatrix<double,rowMajor> C;
   // ... Resizing and initialization
   C = decllow( A * B );
   \endcode
*/
template< typename MT1  // Type of the left-hand side dense matrix
        , typename MT2  // Type of the right-hand side dense matrix
        , bool SF       // Symmetry flag
        , bool HF       // Hermitian flag
        , bool LF       // Lower flag
        , bool UF >     // Upper flag
inline decltype(auto) decllow( const SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF>& dm )
{
   BLAZE_FUNCTION_TRACE;

   if( !isSquare( dm ) ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid lower matrix specification" );
   }

   using ReturnType = const SMatTDMatMultExpr<MT1,MT2,SF,HF,true,UF>;
   return ReturnType( dm.leftOperand(), dm.rightOperand() );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Declares the given non-upper matrix multiplication expression as upper.
// \ingroup dense_matrix
//
// \param dm The input matrix multiplication expression.
// \return The redeclared dense matrix multiplication expression.
// \exception std::invalid_argument Invalid upper matrix specification.
//
// The \a declupp function declares the given non-upper matrix multiplication expression
// \a dm as upper. The function returns an expression representing the operation. In case
// the given expression does not represent a square matrix, a \a std::invalid_argument
// exception is thrown.\n
// The following example demonstrates the use of the \a declupp function:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::CompressedMatrix<double,rowMajor> A;
   blaze::DynamicMatrix<double,columnMajor> B;
   blaze::DynamicMatrix<double,rowMajor> C;
   // ... Resizing and initialization
   C = declupp( A * B );
   \endcode
*/
template< typename MT1  // Type of the left-hand side dense matrix
        , typename MT2  // Type of the right-hand side dense matrix
        , bool SF       // Symmetry flag
        , bool HF       // Hermitian flag
        , bool LF       // Lower flag
        , bool UF >     // Upper flag
inline decltype(auto) declupp( const SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF>& dm )
{
   BLAZE_FUNCTION_TRACE;

   if( !isSquare( dm ) ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid upper matrix specification" );
   }

   using ReturnType = const SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,true>;
   return ReturnType( dm.leftOperand(), dm.rightOperand() );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Declares the given non-diagonal matrix multiplication expression as diagonal.
// \ingroup dense_matrix
//
// \param dm The input matrix multiplication expression.
// \return The redeclared dense matrix multiplication expression.
// \exception std::invalid_argument Invalid diagonal matrix specification.
//
// The \a decldiag function declares the given non-diagonal matrix multiplication expression
// \a dm as diagonal. The function returns an expression representing the operation. In case
// the given expression does not represent a square matrix, a \a std::invalid_argument exception
// is thrown.\n
// The following example demonstrates the use of the \a decldiag function:

   \code
   using blaze::rowMajor;
   using blaze::columnMajor;

   blaze::CompressedMatrix<double,rowMajor> A;
   blaze::DynamicMatrix<double,columnMajor> B;
   blaze::DynamicMatrix<double,rowMajor> C;
   // ... Resizing and initialization
   C = decldiag( A * B );
   \endcode
*/
template< typename MT1  // Type of the left-hand side dense matrix
        , typename MT2  // Type of the right-hand side dense matrix
        , bool SF       // Symmetry flag
        , bool HF       // Hermitian flag
        , bool LF       // Lower flag
        , bool UF >     // Upper flag
inline decltype(auto) decldiag( const SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF>& dm )
{
   BLAZE_FUNCTION_TRACE;

   if( !isSquare( dm ) ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Invalid diagonal matrix specification" );
   }

   using ReturnType = const SMatTDMatMultExpr<MT1,MT2,SF,HF,true,true>;
   return ReturnType( dm.leftOperand(), dm.rightOperand() );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ROWS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct Rows< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public Rows<MT1>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  COLUMNS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct Columns< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public Columns<MT2>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISALIGNED SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsAligned< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< IsAligned<MT2>::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSYMMETRIC SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsSymmetric< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< Or< Bool<SF>
                            , And< Bool<HF>
                                 , IsBuiltin< ElementType_< SMatTDMatMultExpr<MT1,MT2,false,true,false,false> > > >
                            , And< Bool<LF>, Bool<UF> > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISHERMITIAN SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool LF, bool UF >
struct IsHermitian< SMatTDMatMultExpr<MT1,MT2,SF,true,LF,UF> >
   : public TrueType
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISLOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsLower< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< Or< Bool<LF>
                            , And< IsLower<MT1>, IsLower<MT2> >
                            , And< Or< Bool<SF>, Bool<HF> >
                                 , IsUpper<MT1>, IsUpper<MT2> > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUNILOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsUniLower< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< Or< And< IsUniLower<MT1>, IsUniLower<MT2> >
                            , And< Or< Bool<SF>, Bool<HF> >
                                 , IsUniUpper<MT1>, IsUniUpper<MT2> > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSTRICTLYLOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsStrictlyLower< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< Or< And< IsStrictlyLower<MT1>, IsLower<MT2> >
                            , And< IsStrictlyLower<MT2>, IsLower<MT1> >
                            , And< Or< Bool<SF>, Bool<HF> >
                                 , Or< And< IsStrictlyUpper<MT1>, IsUpper<MT2> >
                                     , And< IsStrictlyUpper<MT2>, IsUpper<MT1> > > > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsUpper< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< Or< Bool<UF>
                            , And< IsUpper<MT1>, IsUpper<MT2> >
                            , And< Or< Bool<SF>, Bool<HF> >
                                 , IsLower<MT1>, IsLower<MT2> > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUNIUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsUniUpper< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< Or< And< IsUniUpper<MT1>, IsUniUpper<MT2> >
                            , And< Or< Bool<SF>, Bool<HF> >
                                 , IsUniLower<MT1>, IsUniLower<MT2> > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSTRICTLYUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool SF, bool HF, bool LF, bool UF >
struct IsStrictlyUpper< SMatTDMatMultExpr<MT1,MT2,SF,HF,LF,UF> >
   : public BoolConstant< Or< And< IsStrictlyUpper<MT1>, IsUpper<MT2> >
                            , And< IsStrictlyUpper<MT2>, IsUpper<MT1> >
                            , And< Or< Bool<SF>, Bool<HF> >
                                 , Or< And< IsStrictlyLower<MT1>, IsLower<MT2> >
                                     , And< IsStrictlyLower<MT2>, IsLower<MT1> > > > >::value >
{};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
