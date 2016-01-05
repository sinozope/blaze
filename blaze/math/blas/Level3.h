//=================================================================================================
/*!
//  \file blaze/math/blas/Level3.h
//  \brief Header file for BLAS level 3 functions
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
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

#ifndef _BLAZE_MATH_BLAS_LEVEL3_H_
#define _BLAZE_MATH_BLAS_LEVEL3_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <boost/cast.hpp>
#include <blaze/math/constraints/BlasCompatible.h>
#include <blaze/math/constraints/Computation.h>
#include <blaze/math/constraints/ConstDataAccess.h>
#include <blaze/math/constraints/MutableDataAccess.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/math/typetraits/IsSymmetric.h>
#include <blaze/system/BLAS.h>
#include <blaze/system/Inline.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Complex.h>


namespace blaze {

//=================================================================================================
//
//  BLAS LEVEL 3 FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\name BLAS level 3 functions */
//@{
#if BLAZE_BLAS_MODE

BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, float alpha, const float* A, int lda,
                               const float* B, int ldb, float beta, float* C, int ldc );

BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, double alpha, const double* A, int lda,
                               const double* B, int ldb, double beta, float* C, int ldc );

BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, complex<float> alpha, const complex<float>* A,
                               int lda, const complex<float>* B, int ldb, complex<float> beta,
                               float* C, int ldc );

BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, complex<double> alpha, const complex<double>* A,
                               int lda, const complex<double>* B, int ldb, complex<double> beta,
                               float* C, int ldc );

template< typename MT1, bool SO1, typename MT2, bool SO2, typename MT3, bool SO3, typename ST >
BLAZE_ALWAYS_INLINE void gemm( DenseMatrix<MT1,SO1>& C, const DenseMatrix<MT2,SO2>& A,
                               const DenseMatrix<MT3,SO3>& B, ST alpha, ST beta );

BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               float alpha, const float* A, int lda, float* B, int ldb );

BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               double alpha, const double* A, int lda, double* B, int ldb );

BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               complex<float> alpha, const complex<float>* A, int lda,
                               complex<float>* B, int ldb );

BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               complex<double> alpha, const complex<double>* A, int lda,
                               complex<double>* B, int ldb );

template< typename MT1, bool SO1, typename MT2, bool SO2, typename ST >
BLAZE_ALWAYS_INLINE void trmm( DenseMatrix<MT1,SO1>& B, const DenseMatrix<MT2,SO2>& A,
                               CBLAS_SIDE side, CBLAS_UPLO uplo, ST alpha );

#endif
//@}
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a dense matrix/dense matrix multiplication with single precision
//        matrices (\f$ C=\alpha*A*B+\beta*C \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param transB Specifies whether to transpose matrix \a B (\a CblasNoTrans or \a CblasTrans).
// \param M The number of rows of matrix \a A and \a C \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B and \a C \f$[0..\infty)\f$.
// \param K The number of columns of matrix \a A and rows in matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$.
// \param A Pointer to the first element of matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \param beta The scaling factor for \f$ C \f$.
// \param C Pointer to the first element of matrix \a C.
// \param ldc The total number of elements between two rows/columns of matrix \a C \f$[0..\infty)\f$.
// \return void
//
// This function performs the dense matrix/dense matrix multiplication for single precision
// matrices based on the BLAS cblas_sgemm() function.
*/
BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, float alpha, const float* A, int lda,
                               const float* B, int ldb, float beta, float* C, int ldc )
{
   cblas_sgemm( order, transA, transB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a dense matrix/dense matrix multiplication with double precision
//        matrices (\f$ C=\alpha*A*B+\beta*C \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param transB Specifies whether to transpose matrix \a B (\a CblasNoTrans or \a CblasTrans).
// \param M The number of rows of matrix \a A and \a C \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B and \a C \f$[0..\infty)\f$.
// \param K The number of columns of matrix \a A and rows in matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$.
// \param A Pointer to the first element of matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \param beta The scaling factor for \f$ C \f$.
// \param C Pointer to the first element of matrix \a C.
// \param ldc The total number of elements between two rows/columns of matrix \a C \f$[0..\infty)\f$.
// \return void
//
// This function performs the dense matrix/dense matrix multiplication for double precision
// matrices based on the BLAS cblas_dgemm() function.
*/
BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, double alpha, const double* A, int lda,
                               const double* B, int ldb, double beta, double* C, int ldc )
{
   cblas_dgemm( order, transA, transB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a dense matrix/dense matrix multiplication with single precision
//        matrices (\f$ C=\alpha*A*B+\beta*C \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param transB Specifies whether to transpose matrix \a B (\a CblasNoTrans or \a CblasTrans).
// \param M The number of rows of matrix \a A and \a C \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B and \a C \f$[0..\infty)\f$.
// \param K The number of columns of matrix \a A and rows in matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$.
// \param A Pointer to the first element of matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \param beta The scaling factor for \f$ C \f$.
// \param C Pointer to the first element of matrix \a C.
// \param ldc The total number of elements between two rows/columns of matrix \a C \f$[0..\infty)\f$.
// \return void
//
// This function performs the dense matrix/dense matrix multiplication for single precision
// complex matrices based on the BLAS cblas_cgemm() function.
*/
BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, complex<float> alpha, const complex<float>* A,
                               int lda, const complex<float>* B, int ldb, complex<float> beta,
                               complex<float>* C, int ldc )
{
   cblas_cgemm( order, transA, transB, M, N, K, &alpha, A, lda, B, ldb, &beta, C, ldc );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a dense matrix/dense matrix multiplication with double precision
//        matrices (\f$ C=\alpha*A*B+\beta*C \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param transB Specifies whether to transpose matrix \a B (\a CblasNoTrans or \a CblasTrans).
// \param M The number of rows of matrix \a A and \a C \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B and \a C \f$[0..\infty)\f$.
// \param K The number of columns of matrix \a A and rows in matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$.
// \param A Pointer to the first element of matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \param beta The scaling factor for \f$ C \f$.
// \param C Pointer to the first element of matrix \a C.
// \param ldc The total number of elements between two rows/columns of matrix \a C \f$[0..\infty)\f$.
// \return void
//
// This function performs the dense matrix/dense matrix multiplication for double precision
// complex matrices based on the BLAS cblas_zgemm() function.
*/
BLAZE_ALWAYS_INLINE void gemm( CBLAS_ORDER order, CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                               int M, int N, int K, complex<double> alpha, const complex<double>* A,
                               int lda, const complex<double>* B, int ldb, complex<double> beta,
                               complex<double>* C, int ldc )
{
   cblas_zgemm( order, transA, transB, M, N, K, &alpha, A, lda, B, ldb, &beta, C, ldc );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a dense matrix/dense matrix multiplication (\f$ C=\alpha*A*B+\beta*C \f$).
// \ingroup math
//
// \param C The target left-hand side dense matrix.
// \param A The left-hand side multiplication operand.
// \param B The right-hand side multiplication operand.
// \param alpha The scaling factor for \f$ A*B \f$.
// \param beta The scaling factor for \f$ C \f$.
// \return void
//
// This function performs the dense matrix/dense matrix multiplication based on the BLAS
// gemm() functions. Note that the function only works for matrices with \c float, \c double,
// \c complex<float>, and \c complex<double> element type. The attempt to call the function
// with matrices of any other element type results in a compile time error.
*/
template< typename MT1   // Type of the left-hand side target matrix
        , bool SO1       // Storage order of the left-hand side target matrix
        , typename MT2   // Type of the left-hand side matrix operand
        , bool SO2       // Storage order of the left-hand side matrix operand
        , typename MT3   // Type of the right-hand side matrix operand
        , bool SO3       // Storage order of the right-hand side matrix operand
        , typename ST >  // Type of the scalar factors
BLAZE_ALWAYS_INLINE void gemm( DenseMatrix<MT1,SO1>& C, const DenseMatrix<MT2,SO2>& A,
                               const DenseMatrix<MT3,SO3>& B, ST alpha, ST beta )
{
   using boost::numeric_cast;

   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE( MT3 );

   BLAZE_CONSTRAINT_MUST_HAVE_MUTABLE_DATA_ACCESS( MT1 );
   BLAZE_CONSTRAINT_MUST_HAVE_CONST_DATA_ACCESS  ( MT2 );
   BLAZE_CONSTRAINT_MUST_HAVE_CONST_DATA_ACCESS  ( MT3 );

   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( typename MT1::ElementType );
   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( typename MT2::ElementType );
   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( typename MT3::ElementType );

   const int M  ( numeric_cast<int>( (~A).rows() )    );
   const int N  ( numeric_cast<int>( (~B).columns() ) );
   const int K  ( numeric_cast<int>( (~A).columns() ) );
   const int lda( numeric_cast<int>( (~A).spacing() ) );
   const int ldb( numeric_cast<int>( (~B).spacing() ) );
   const int ldc( numeric_cast<int>( (~C).spacing() ) );

   gemm( ( IsRowMajorMatrix<MT1>::value )?( CblasRowMajor ):( CblasColMajor ),
         ( SO1 == SO2 )?( CblasNoTrans ):( CblasTrans ),
         ( SO1 == SO3 )?( CblasNoTrans ):( CblasTrans ),
         M, N, K, alpha, (~A).data(), lda, (~B).data(), ldb, beta, (~C).data(), ldc );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a triangular dense matrix/dense matrix multiplication with single
//        precision matrices (\f$ B=\alpha*A*B \f$ or \f$ B=\alpha*B*A \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param side \a CblasLeft to compute \f$ B=\alpha*A*B \f$, \a CblasRight to compute \f$ B=\alpha*B*A \f$.
// \param uplo \a CblasLower to use the lower triangle from \a A, \a CblasUpper to use the upper triangle.
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param diag Specifies whether \a A is unitriangular (\a CblasNonUnit or \a CblasUnit).
// \param M The number of rows of matrix \a B \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$ or \f$ B*A \f$.
// \param A Pointer to the first element of the triangular matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \return void
//
// This function performs the scaling and multiplication of a triangular matrix by a matrix
// based on the cblas_strmm() function. Note that matrix \a A is expected to be a square matrix.
*/
BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               float alpha, const float* A, int lda, float* B, int ldb )
{
   cblas_strmm( order, side, uplo, transA, diag, M, N, alpha, A, lda, B, ldb );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a triangular dense matrix/dense matrix multiplication with double
//        precision matrices (\f$ B=\alpha*A*B \f$ or \f$ B=\alpha*B*A \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param side \a CblasLeft to compute \f$ B=\alpha*A*B \f$, \a CblasRight to compute \f$ B=\alpha*B*A \f$.
// \param uplo \a CblasLower to use the lower triangle from \a A, \a CblasUpper to use the upper triangle.
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param diag Specifies whether \a A is unitriangular (\a CblasNonUnit or \a CblasUnit).
// \param M The number of rows of matrix \a B \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$ or \f$ B*A \f$.
// \param A Pointer to the first element of the triangular matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \return void
//
// This function performs the scaling and multiplication of a triangular matrix by a matrix
// based on the cblas_dtrmm() function. Note that matrix \a A is expected to be a square matrix.
*/
BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               double alpha, const double* A, int lda, double* B, int ldb )
{
   cblas_dtrmm( order, side, uplo, transA, diag, M, N, alpha, A, lda, B, ldb );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a triangular dense matrix/dense matrix multiplication with single
//        precision complex matrices (\f$ B=\alpha*A*B \f$ or \f$ B=\alpha*B*A \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param side \a CblasLeft to compute \f$ B=\alpha*A*B \f$, \a CblasRight to compute \f$ B=\alpha*B*A \f$.
// \param uplo \a CblasLower to use the lower triangle from \a A, \a CblasUpper to use the upper triangle.
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param diag Specifies whether \a A is unitriangular (\a CblasNonUnit or \a CblasUnit).
// \param M The number of rows of matrix \a B \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$ or \f$ B*A \f$.
// \param A Pointer to the first element of the triangular matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \return void
//
// This function performs the scaling and multiplication of a triangular matrix by a matrix
// based on the cblas_ctrmm() function. Note that matrix \a A is expected to be a square matrix.
*/
BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               complex<float> alpha, const complex<float>* A, int lda,
                               complex<float>* B, int ldb )
{
   cblas_ctrmm( order, side, uplo, transA, diag, M, N, &alpha, A, lda, B, ldb );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a triangular dense matrix/dense matrix multiplication with double
//        precision complex matrices (\f$ B=\alpha*A*B \f$ or \f$ B=\alpha*B*A \f$).
// \ingroup math
//
// \param order Specifies the storage order of matrix \a A (\a CblasColMajor or \a CblasColMajor).
// \param side \a CblasLeft to compute \f$ B=\alpha*A*B \f$, \a CblasRight to compute \f$ B=\alpha*B*A \f$.
// \param uplo \a CblasLower to use the lower triangle from \a A, \a CblasUpper to use the upper triangle.
// \param transA Specifies whether to transpose matrix \a A (\a CblasNoTrans or \a CblasTrans).
// \param diag Specifies whether \a A is unitriangular (\a CblasNonUnit or \a CblasUnit).
// \param M The number of rows of matrix \a B \f$[0..\infty)\f$.
// \param N The number of columns of matrix \a B \f$[0..\infty)\f$.
// \param alpha The scaling factor for \f$ A*B \f$ or \f$ B*A \f$.
// \param A Pointer to the first element of the triangular matrix \a A.
// \param lda The total number of elements between two rows/columns of matrix \a A \f$[0..\infty)\f$.
// \param B Pointer to the first element of matrix \a B.
// \param ldb The total number of elements between two rows/columns of matrix \a B \f$[0..\infty)\f$.
// \return void
//
// This function performs the scaling and multiplication of a triangular matrix by a matrix
// based on the cblas_ztrmm() function. Note that matrix \a A is expected to be a square matrix.
*/
BLAZE_ALWAYS_INLINE void trmm( CBLAS_ORDER order, CBLAS_SIDE side, CBLAS_UPLO uplo,
                               CBLAS_TRANSPOSE transA, CBLAS_DIAG diag, int M, int N,
                               complex<double> alpha, const complex<double>* A, int lda,
                               complex<double>* B, int ldb )
{
   cblas_ztrmm( order, side, uplo, transA, diag, M, N, &alpha, A, lda, B, ldb );
}
#endif
//*************************************************************************************************


//*************************************************************************************************
#if BLAZE_BLAS_MODE
/*!\brief BLAS kernel for a triangular dense matrix/dense matrix multiplication with single
//        precision matrices (\f$ B=\alpha*A*B \f$ or \f$ B=\alpha*B*A \f$).
// \ingroup math
//
// \param B The target dense matrix.
// \param A The dense matrix multiplication operand.
// \param side \a CblasLeft to compute \f$ B=\alpha*A*B \f$, \a CblasRight to compute \f$ B=\alpha*B*A \f$.
// \param uplo \a CblasLower to use the lower triangle from \a A, \a CblasUpper to use the upper triangle.
// \param alpha The scaling factor for \f$ A*B \f$ or \f$ B*A \f$.
// \return void
//
// This function performs the scaling and multiplication of a triangular matrix by a matrix
// based on the cblas_strmm() function. Note that the function only works for matrices with
// \c float element type. The attempt to call the function with matrices of any other element
// type results in a compile time error. Also, matrix \a A is expected to be a square matrix.
*/
template< typename MT1   // Type of the left-hand side target matrix
        , bool SO1       // Storage order of the left-hand side target matrix
        , typename MT2   // Type of the left-hand side matrix operand
        , bool SO2       // Storage order of the left-hand side matrix operand
        , typename ST >  // Type of the scalar factors
BLAZE_ALWAYS_INLINE void trmm( DenseMatrix<MT1,SO1>& B, const DenseMatrix<MT2,SO2>& A,
                               CBLAS_SIDE side, CBLAS_UPLO uplo, ST alpha )
{
   using boost::numeric_cast;

   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE( MT2 );

   BLAZE_CONSTRAINT_MUST_HAVE_MUTABLE_DATA_ACCESS( MT1 );
   BLAZE_CONSTRAINT_MUST_HAVE_CONST_DATA_ACCESS  ( MT2 );

   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( typename MT1::ElementType );
   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( typename MT2::ElementType );

   BLAZE_INTERNAL_ASSERT( (~A).rows() == (~A).columns(), "Non-square triangular matrix detected" );
   BLAZE_INTERNAL_ASSERT( side == CblasLeft  || side == CblasRight, "Invalid side argument detected" );
   BLAZE_INTERNAL_ASSERT( uplo == CblasLower || uplo == CblasUpper, "Invalid uplo argument detected" );

   const int M  ( numeric_cast<int>( (~B).rows() )    );
   const int N  ( numeric_cast<int>( (~B).columns() ) );
   const int lda( numeric_cast<int>( (~A).spacing() ) );
   const int ldb( numeric_cast<int>( (~B).spacing() ) );

   trmm( ( IsRowMajorMatrix<MT1>::value )?( CblasRowMajor ):( CblasColMajor ),
         side,
         ( SO1 == SO2 )?( uplo ):( ( uplo == CblasLower )?( CblasUpper ):( CblasLower ) ),
         ( SO1 == SO2 )?( CblasNoTrans ):( CblasTrans ),
         CblasNonUnit,
         M, N, alpha, (~A).data(), lda, (~B).data(), ldb );
}
#endif
//*************************************************************************************************

} // namespace blaze

#endif
