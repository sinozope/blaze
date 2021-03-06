//=================================================================================================
/*!
//  \file src/mathtest/dmatdmatmult/AliasingTest.cpp
//  \brief Source file for the dense matrix/dense matrix multiplication aliasing test
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


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <cstdlib>
#include <iostream>
#include <blazetest/mathtest/dmatdmatmult/AliasingTest.h>


namespace blazetest {

namespace mathtest {

namespace dmatdmatmult {

//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Constructor for the aliasing test class.
//
// \exception std::runtime_error Operation error detected.
*/
AliasingTest::AliasingTest()
   : dA3x4_ ( 3UL, 4UL )
   , dB4x3_ ( 4UL, 3UL )
   , dC3x3_ ( 3UL, 3UL )
   , dD3x3_ ( 3UL, 3UL )
   , dE3x3_ ( 3UL, 3UL )
   , tdA3x4_( 3UL, 4UL )
   , tdB4x3_( 4UL, 3UL )
   , tdC3x3_( 3UL, 3UL )
   , tdD3x3_( 3UL, 3UL )
   , tdE3x3_( 3UL, 3UL )
{
   testDMatDMatMult  ();
   testDMatTDMatMult ();
   testTDMatDMatMult ();
   testTDMatTDMatMult();
}
//*************************************************************************************************




//=================================================================================================
//
//  TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Test of the dense matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Error detected.
//
// This function performs aliasing tests for the dense matrix/dense matrix multiplication.
// In case an error is detected, a \a std::runtime_error exception is thrown.
*/
void AliasingTest::testDMatDMatMult()
{
   //=====================================================================================
   // Multiplication
   //=====================================================================================

   // Assignment to left-hand side operand
   {
      test_ = "DMatDMatMult - Assignment to left-hand side operand";

      initialize();

      result_ = dA3x4_ * dB4x3_;
      dA3x4_  = dA3x4_ * dB4x3_;

      checkResult( dA3x4_, result_ );
   }

   // Assignment to first operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Assignment to first operand of left-hand side compound";

      initialize();

      result_ = ( dA3x4_ * dB4x3_ ) * dC3x3_;
      dA3x4_  = ( dA3x4_ * dB4x3_ ) * dC3x3_;

      checkResult( dA3x4_, result_ );
   }

   // Assignment to second operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Assignment to second operand of left-hand side compound";

      initialize();

      result_ = ( dA3x4_ * dB4x3_ ) * dC3x3_;
      dB4x3_  = ( dA3x4_ * dB4x3_ ) * dC3x3_;

      checkResult( dB4x3_, result_ );
   }

   // Assignment to right-hand side operand
   {
      test_ = "DMatDMatMult - Assignment to right-hand side operand";

      initialize();

      result_ = dA3x4_ * dB4x3_;
      dB4x3_  = dA3x4_ * dB4x3_;

      checkResult( dB4x3_, result_ );
   }

   // Assignment to first operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Assignment to first operand of right-hand side compound";

      initialize();

      result_ = dC3x3_ * ( dA3x4_ * dB4x3_ );
      dA3x4_  = dC3x3_ * ( dA3x4_ * dB4x3_ );

      checkResult( dA3x4_, result_ );
   }

   // Assignment to second operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Assignment to second operand of right-hand side compound";

      initialize();

      result_ = dC3x3_ * ( dA3x4_ * dB4x3_ );
      dB4x3_  = dC3x3_ * ( dA3x4_ * dB4x3_ );

      checkResult( dB4x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with addition assignment
   //=====================================================================================

   // Addition assignment to left-hand side operand
   {
      test_ = "DMatDMatMult - Addition assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ += dC3x3_ * dD3x3_;
      dC3x3_  += dC3x3_ * dD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Addition assignment to first operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Addition assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ += ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dC3x3_  += ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Addition assignment to second operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Addition assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ += ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dD3x3_  += ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Addition assignment to right-hand side operand
   {
      test_ = "DMatDMatMult - Addition assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ += dC3x3_ * dD3x3_;
      dD3x3_  += dC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Addition assignment to first operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Addition assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ += dC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  += dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Addition assignment to second operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Addition assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ += dC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  += dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with subtraction assignment
   //=====================================================================================

   // Subtraction assignment to left-hand side operand
   {
      test_ = "DMatDMatMult - Subtraction assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ -= dC3x3_ * dD3x3_;
      dC3x3_  -= dC3x3_ * dD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Subtraction assignment to first operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Subtraction assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ -= ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dC3x3_  -= ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Subtraction assignment to second operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Subtraction assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ -= ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dD3x3_  -= ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Subtraction assignment to right-hand side operand
   {
      test_ = "DMatDMatMult - Subtraction assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ -= dC3x3_ * dD3x3_;
      dD3x3_  -= dC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Subtraction assignment to first operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Subtraction assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ -= dC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  -= dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Subtraction assignment to second operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Subtraction assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ -= dC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  -= dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with Schur product assignment
   //=====================================================================================

   // Schur product assignment to left-hand side operand
   {
      test_ = "DMatDMatMult - Schur product assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ %= dC3x3_ * dD3x3_;
      dC3x3_  %= dC3x3_ * dD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Schur product assignment to first operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Schur product assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ %= ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dC3x3_  %= ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Schur product assignment to second operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Schur product assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ %= ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dD3x3_  %= ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Schur product assignment to right-hand side operand
   {
      test_ = "DMatDMatMult - Schur product assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ %= dC3x3_ * dD3x3_;
      dD3x3_  %= dC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Schur product assignment to first operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Schur product assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ %= dC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  %= dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Schur product assignment to second operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Schur product assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ %= dC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  %= dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with multiplication assignment
   //=====================================================================================

   // Multiplication assignment to left-hand side operand
   {
      test_ = "DMatDMatMult - Multiplication assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ *= dC3x3_ * dD3x3_;
      dC3x3_  *= dC3x3_ * dD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Multiplication assignment to first operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Multiplication assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ *= ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dC3x3_  *= ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Multiplication assignment to second operand of left-hand side compound
   {
      test_ = "DMatDMatMult - Multiplication assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ *= ( dC3x3_ * dD3x3_ ) * dE3x3_;
      dD3x3_  *= ( dC3x3_ * dD3x3_ ) * dE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Multiplication assignment to right-hand side operand
   {
      test_ = "DMatDMatMult - Multiplication assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ *= dC3x3_ * dD3x3_;
      dD3x3_  *= dC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Multiplication assignment to first operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Multiplication assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ *= dC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  *= dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Multiplication assignment to second operand of right-hand side compound
   {
      test_ = "DMatDMatMult - Multiplication assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ *= dC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  *= dC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Test of the dense matrix/transpose dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Error detected.
//
// This function performs aliasing tests for the dense matrix/transpose dense matrix
// multiplication. In case an error is detected, a \a std::runtime_error exception is
// thrown.
*/
void AliasingTest::testDMatTDMatMult()
{
   //=====================================================================================
   // Multiplication
   //=====================================================================================

   // Assignment to left-hand side operand
   {
      test_ = "DMatTDMatMult - Assignment to left-hand side operand";

      initialize();

      result_ = dA3x4_ * tdB4x3_;
      dA3x4_  = dA3x4_ * tdB4x3_;

      checkResult( dA3x4_, result_ );
   }

   // Assignment to first operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Assignment to first operand of left-hand side compound";

      initialize();

      result_ = ( dA3x4_ * dB4x3_ ) * tdC3x3_;
      dA3x4_  = ( dA3x4_ * dB4x3_ ) * tdC3x3_;

      checkResult( dA3x4_, result_ );
   }

   // Assignment to second operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Assignment to second operand of left-hand side compound";

      initialize();

      result_ = ( dA3x4_ * dB4x3_ ) * tdC3x3_;
      dB4x3_  = ( dA3x4_ * dB4x3_ ) * tdC3x3_;

      checkResult( dB4x3_, result_ );
   }

   // Assignment to right-hand side operand
   {
      test_ = "DMatTDMatMult - Assignment to right-hand side operand";

      initialize();

      result_ = dA3x4_ * tdB4x3_;
      tdB4x3_ = dA3x4_ * tdB4x3_;

      checkResult( tdB4x3_, result_ );
   }

   // Assignment to first operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Assignment to first operand of right-hand side compound";

      initialize();

      result_ = dC3x3_ * ( tdA3x4_ * tdB4x3_ );
      tdA3x4_ = dC3x3_ * ( tdA3x4_ * tdB4x3_ );

      checkResult( tdA3x4_, result_ );
   }

   // Assignment to second operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Assignment to second operand of right-hand side compound";

      initialize();

      result_ = dC3x3_ * ( tdA3x4_ * tdB4x3_ );
      tdB4x3_ = dC3x3_ * ( tdA3x4_ * tdB4x3_ );

      checkResult( tdB4x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with addition assignment
   //=====================================================================================

   // Addition assignment to left-hand side operand
   {
      test_ = "DMatTDMatMult - Addition assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ += dC3x3_ * tdD3x3_;
      dC3x3_  += dC3x3_ * tdD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Addition assignment to first operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Addition assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ += ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dC3x3_  += ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Addition assignment to second operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Addition assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ += ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dD3x3_  += ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Addition assignment to right-hand side operand
   {
      test_ = "DMatTDMatMult - Addition assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ += dC3x3_ * tdD3x3_;
      tdD3x3_ += dC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Addition assignment to first operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Addition assignment to first operand of right-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ += dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ += dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Addition assignment to second operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Addition assignment to second operand of right-hand side compound";

      initialize();

      result_ =  tdE3x3_;
      result_ += dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ += dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with subtraction assignment
   //=====================================================================================

   // Subtraction assignment to left-hand side operand
   {
      test_ = "DMatTDMatMult - Subtraction assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ -= dC3x3_ * tdD3x3_;
      dC3x3_  -= dC3x3_ * tdD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Subtraction assignment to first operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Subtraction assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ -= ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dC3x3_  -= ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Subtraction assignment to second operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Subtraction assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ -= ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dD3x3_  -= ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Subtraction assignment to right-hand side operand
   {
      test_ = "DMatTDMatMult - Subtraction assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ -= dC3x3_ * tdD3x3_;
      tdD3x3_ -= dC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Subtraction assignment to first operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Subtraction assignment to first operand of right-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ -= dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ -= dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Subtraction assignment to second operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Subtraction assignment to second operand of right-hand side compound";

      initialize();

      result_ =  tdE3x3_;
      result_ -= dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ -= dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with Schur product assignment
   //=====================================================================================

   // Schur product assignment to left-hand side operand
   {
      test_ = "DMatTDMatMult - Schur product assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ %= dC3x3_ * tdD3x3_;
      dC3x3_  %= dC3x3_ * tdD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Schur product assignment to first operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Schur product assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ %= ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dC3x3_  %= ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Schur product assignment to second operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Schur product assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ %= ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dD3x3_  %= ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Schur product assignment to right-hand side operand
   {
      test_ = "DMatTDMatMult - Schur product assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ %= dC3x3_ * tdD3x3_;
      tdD3x3_ %= dC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Schur product assignment to first operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Schur product assignment to first operand of right-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ %= dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ %= dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Schur product assignment to second operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Schur product assignment to second operand of right-hand side compound";

      initialize();

      result_ =  tdE3x3_;
      result_ %= dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ %= dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with multiplication assignment
   //=====================================================================================

   // Multiplication assignment to left-hand side operand
   {
      test_ = "DMatTDMatMult - Multiplication assignment to left-hand side operand";

      initialize();

      result_ =  dC3x3_;
      result_ *= dC3x3_ * tdD3x3_;
      dC3x3_  *= dC3x3_ * tdD3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Multiplication assignment to first operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Multiplication assignment to first operand of left-hand side compound";

      initialize();

      result_ =  dC3x3_;
      result_ *= ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dC3x3_  *= ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dC3x3_, result_ );
   }

   // Multiplication assignment to second operand of left-hand side compound
   {
      test_ = "DMatTDMatMult - Multiplication assignment to second operand of left-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ *= ( dC3x3_ * dD3x3_ ) * tdE3x3_;
      dD3x3_  *= ( dC3x3_ * dD3x3_ ) * tdE3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Multiplication assignment to right-hand side operand
   {
      test_ = "DMatTDMatMult - Multiplication assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ *= dC3x3_ * tdD3x3_;
      tdD3x3_ *= dC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Multiplication assignment to first operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Multiplication assignment to first operand of right-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ *= dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ *= dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Multiplication assignment to second operand of right-hand side compound
   {
      test_ = "DMatTDMatMult - Multiplication assignment to second operand of right-hand side compound";

      initialize();

      result_ =  tdE3x3_;
      result_ *= dC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ *= dC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Test of the transpose dense matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Error detected.
//
// This function performs aliasing tests for the transpose dense matrix/dense matrix
// multiplication. In case an error is detected, a \a std::runtime_error exception is
// thrown.
*/
void AliasingTest::testTDMatDMatMult()
{
   //=====================================================================================
   // Multiplication
   //=====================================================================================

   // Assignment to left-hand side operand
   {
      test_ = "TDMatDMatMult - Assignment to left-hand side operand";

      initialize();

      result_ = tdA3x4_ * dB4x3_;
      tdA3x4_ = tdA3x4_ * dB4x3_;

      checkResult( tdA3x4_, result_ );
   }

   // Assignment to first operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Assignment to first operand of left-hand side compound";

      initialize();

      result_ = ( tdA3x4_ * tdB4x3_ ) * dC3x3_;
      tdA3x4_ = ( tdA3x4_ * tdB4x3_ ) * dC3x3_;

      checkResult( tdA3x4_, result_ );
   }

   // Assignment to second operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Assignment to second operand of left-hand side compound";

      initialize();

      result_ = ( tdA3x4_ * tdB4x3_ ) * dC3x3_;
      tdB4x3_ = ( tdA3x4_ * tdB4x3_ ) * dC3x3_;

      checkResult( tdB4x3_, result_ );
   }

   // Assignment to right-hand side operand
   {
      test_ = "TDMatDMatMult - Assignment to right-hand side operand";

      initialize();

      result_ = tdA3x4_ * dB4x3_;
      dB4x3_  = tdA3x4_ * dB4x3_;

      checkResult( dB4x3_, result_ );
   }

   // Assignment to first operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Assignment to first operand of right-hand side compound";

      initialize();

      result_ = tdC3x3_ * ( dA3x4_ * dB4x3_ );
      dA3x4_  = tdC3x3_ * ( dA3x4_ * dB4x3_ );

      checkResult( dA3x4_, result_ );
   }

   // Assignment to second operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Assignment to second operand of right-hand side compound";

      initialize();

      result_ = tdC3x3_ * ( dA3x4_ * dB4x3_ );
      dB4x3_  = tdC3x3_ * ( dA3x4_ * dB4x3_ );

      checkResult( dB4x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with addition assignment
   //=====================================================================================

   // Addition assignment to left-hand side operand
   {
      test_ = "TDMatDMatMult - Addition assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ += tdC3x3_ * dD3x3_;
      tdC3x3_ += tdC3x3_ * dD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Addition assignment to first operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Addition assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ += ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdC3x3_ += ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Addition assignment to second operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Addition assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ += ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdD3x3_ += ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Addition assignment to right-hand side operand
   {
      test_ = "TDMatDMatMult - Addition assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ += tdC3x3_ * dD3x3_;
      dD3x3_  += tdC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Addition assignment to first operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Addition assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ += tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  += tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Addition assignment to second operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Addition assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ += tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  += tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with subtraction assignment
   //=====================================================================================

   // Subtraction assignment to left-hand side operand
   {
      test_ = "TDMatDMatMult - Subtraction assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ -= tdC3x3_ * dD3x3_;
      tdC3x3_ -= tdC3x3_ * dD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Subtraction assignment to first operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Subtraction assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ -= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdC3x3_ -= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Subtraction assignment to second operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Subtraction assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ -= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdD3x3_ -= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Subtraction assignment to right-hand side operand
   {
      test_ = "TDMatDMatMult - Subtraction assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ -= tdC3x3_ * dD3x3_;
      dD3x3_  -= tdC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Subtraction assignment to first operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Subtraction assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ -= tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  -= tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Subtraction assignment to second operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Subtraction assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ -= tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  -= tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with Schur product assignment
   //=====================================================================================

   // Schur product assignment to left-hand side operand
   {
      test_ = "TDMatDMatMult - Schur product assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ %= tdC3x3_ * dD3x3_;
      tdC3x3_ %= tdC3x3_ * dD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Schur product assignment to first operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Schur product assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ %= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdC3x3_ %= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Schur product assignment to second operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Schur product assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ %= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdD3x3_ %= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Schur product assignment to right-hand side operand
   {
      test_ = "TDMatDMatMult - Schur product assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ %= tdC3x3_ * dD3x3_;
      dD3x3_  %= tdC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Schur product assignment to first operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Schur product assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ %= tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  %= tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Schur product assignment to second operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Schur product assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ %= tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  %= tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with multiplication assignment
   //=====================================================================================

   // Multiplication assignment to left-hand side operand
   {
      test_ = "TDMatDMatMult - Multiplication assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ *= tdC3x3_ * dD3x3_;
      tdC3x3_ *= tdC3x3_ * dD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Multiplication assignment to first operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Multiplication assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ *= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdC3x3_ *= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Multiplication assignment to second operand of left-hand side compound
   {
      test_ = "TDMatDMatMult - Multiplication assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ *= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;
      tdD3x3_ *= ( tdC3x3_ * tdD3x3_ ) * dE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Multiplication assignment to right-hand side operand
   {
      test_ = "TDMatDMatMult - Multiplication assignment to right-hand side operand";

      initialize();

      result_ =  dD3x3_;
      result_ *= tdC3x3_ * dD3x3_;
      dD3x3_  *= tdC3x3_ * dD3x3_;

      checkResult( dD3x3_, result_ );
   }

   // Multiplication assignment to first operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Multiplication assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ *= tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dD3x3_  *= tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dD3x3_, result_ );
   }

   // Multiplication assignment to second operand of right-hand side compound
   {
      test_ = "TDMatDMatMult - Multiplication assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ *= tdC3x3_ * ( dD3x3_ * dE3x3_ );
      dE3x3_  *= tdC3x3_ * ( dD3x3_ * dE3x3_ );

      checkResult( dE3x3_, result_ );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Test of the transpose dense matrix/transpose dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Error detected.
//
// This function performs aliasing tests for the transpose dense matrix/transpose dense
// matrix multiplication. In case an error is detected, a \a std::runtime_error exception
// is thrown.
*/
void AliasingTest::testTDMatTDMatMult()
{
   //=====================================================================================
   // Multiplication
   //=====================================================================================

   // Assignment to left-hand side operand
   {
      test_ = "TDMatTDMatMult - Assignment to left-hand side operand";

      initialize();

      result_ = tdA3x4_ * tdB4x3_;
      tdA3x4_ = tdA3x4_ * tdB4x3_;

      checkResult( tdA3x4_, result_ );
   }

   // Assignment to first operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Assignment to first operand of left-hand side compound";

      initialize();

      result_ = ( tdA3x4_ * tdB4x3_ ) * tdC3x3_;
      tdA3x4_ = ( tdA3x4_ * tdB4x3_ ) * tdC3x3_;

      checkResult( tdA3x4_, result_ );
   }

   // Assignment to second operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Assignment to second operand of left-hand side compound";

      initialize();

      result_ = ( tdA3x4_ * tdB4x3_ ) * tdC3x3_;
      tdB4x3_ = ( tdA3x4_ * tdB4x3_ ) * tdC3x3_;

      checkResult( tdB4x3_, result_ );
   }

   // Assignment to right-hand side operand
   {
      test_ = "TDMatTDMatMult - Assignment to right-hand side operand";

      initialize();

      result_ = tdA3x4_ * tdB4x3_;
      tdB4x3_ = tdA3x4_ * tdB4x3_;

      checkResult( tdB4x3_, result_ );
   }

   // Assignment to first operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Assignment to first operand of right-hand side compound";

      initialize();

      result_ = tdC3x3_ * ( tdA3x4_ * tdB4x3_ );
      tdA3x4_ = tdC3x3_ * ( tdA3x4_ * tdB4x3_ );

      checkResult( tdA3x4_, result_ );
   }

   // Assignment to second operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Assignment to second operand of right-hand side compound";

      initialize();

      result_ = tdC3x3_ * ( tdA3x4_ * tdB4x3_ );
      tdB4x3_ = tdC3x3_ * ( tdA3x4_ * tdB4x3_ );

      checkResult( tdB4x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with addition assignment
   //=====================================================================================

   // Addition assignment to left-hand side operand
   {
      test_ = "TDMatTDMatMult - Addition assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ += tdC3x3_ * tdD3x3_;
      tdC3x3_ += tdC3x3_ * tdD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Addition assignment to first operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Addition assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ += ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdC3x3_ += ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Addition assignment to second operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Addition assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ += ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdD3x3_ += ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Addition assignment to right-hand side operand
   {
      test_ = "TDMatTDMatMult - Addition assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ += tdC3x3_ * tdD3x3_;
      tdD3x3_ += tdC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Addition assignment to first operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Addition assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ += tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ += tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Addition assignment to second operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Addition assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ += tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ += tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with subtraction assignment
   //=====================================================================================

   // Subtraction assignment to left-hand side operand
   {
      test_ = "TDMatTDMatMult - Subtraction assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ -= tdC3x3_ * tdD3x3_;
      tdC3x3_ -= tdC3x3_ * tdD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Subtraction assignment to first operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Subtraction assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ -= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdC3x3_ -= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Subtraction assignment to second operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Subtraction assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ -= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdD3x3_ -= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Subtraction assignment to right-hand side operand
   {
      test_ = "TDMatTDMatMult - Subtraction assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ -= tdC3x3_ * tdD3x3_;
      tdD3x3_ -= tdC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Subtraction assignment to first operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Subtraction assignment to first operand of right-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ -= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ -= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Subtraction assignment to second operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Subtraction assignment to second operand of right-hand side compound";

      initialize();

      result_ =  tdE3x3_;
      result_ -= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ -= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with Schur product assignment
   //=====================================================================================

   // Schur product assignment to left-hand side operand
   {
      test_ = "TDMatTDMatMult - Schur product assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ %= tdC3x3_ * tdD3x3_;
      tdC3x3_ %= tdC3x3_ * tdD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Schur product assignment to first operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Schur product assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ %= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdC3x3_ %= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Schur product assignment to second operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Schur product assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ %= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdD3x3_ %= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Schur product assignment to right-hand side operand
   {
      test_ = "TDMatTDMatMult - Schur product assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ %= tdC3x3_ * tdD3x3_;
      tdD3x3_ %= tdC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Schur product assignment to first operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Schur product assignment to first operand of right-hand side compound";

      initialize();

      result_ =  dD3x3_;
      result_ %= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ %= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Schur product assignment to second operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Schur product assignment to second operand of right-hand side compound";

      initialize();

      result_ =  dE3x3_;
      result_ %= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ %= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }


   //=====================================================================================
   // Multiplication with multiplication assignment
   //=====================================================================================

   // Multiplication assignment to left-hand side operand
   {
      test_ = "TDMatTDMatMult - Multiplication assignment to left-hand side operand";

      initialize();

      result_ =  tdC3x3_;
      result_ *= tdC3x3_ * tdD3x3_;
      tdC3x3_ *= tdC3x3_ * tdD3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Multiplication assignment to first operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Multiplication assignment to first operand of left-hand side compound";

      initialize();

      result_ =  tdC3x3_;
      result_ *= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdC3x3_ *= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdC3x3_, result_ );
   }

   // Multiplication assignment to second operand of left-hand side compound
   {
      test_ = "TDMatTDMatMult - Multiplication assignment to second operand of left-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ *= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;
      tdD3x3_ *= ( tdC3x3_ * tdD3x3_ ) * tdE3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Multiplication assignment to right-hand side operand
   {
      test_ = "TDMatTDMatMult - Multiplication assignment to right-hand side operand";

      initialize();

      result_ =  tdD3x3_;
      result_ *= tdC3x3_ * tdD3x3_;
      tdD3x3_ *= tdC3x3_ * tdD3x3_;

      checkResult( tdD3x3_, result_ );
   }

   // Multiplication assignment to first operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Multiplication assignment to first operand of right-hand side compound";

      initialize();

      result_ =  tdD3x3_;
      result_ *= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdD3x3_ *= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdD3x3_, result_ );
   }

   // Multiplication assignment to second operand of right-hand side compound
   {
      test_ = "TDMatTDMatMult - Multiplication assignment to second operand of right-hand side compound";

      initialize();

      result_ =  tdE3x3_;
      result_ *= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );
      tdE3x3_ *= tdC3x3_ * ( tdD3x3_ * tdE3x3_ );

      checkResult( tdE3x3_, result_ );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Initialization of all member vectors and matrices.
//
// \return void
// \exception std::runtime_error Error detected.
//
// This function initializes all member vectors and matrices to specific predetermined values.
*/
void AliasingTest::initialize()
{
   // Initializing the first row-major dense matrix
   dA3x4_.resize( 3UL, 4UL, false );
   dA3x4_(0,0) = -1;
   dA3x4_(0,1) =  0;
   dA3x4_(0,2) = -2;
   dA3x4_(0,3) =  0;
   dA3x4_(1,0) =  0;
   dA3x4_(1,1) =  2;
   dA3x4_(1,2) = -3;
   dA3x4_(1,3) =  1;
   dA3x4_(2,0) =  0;
   dA3x4_(2,1) =  1;
   dA3x4_(2,2) =  2;
   dA3x4_(2,3) =  2;

   // Initializing the second row-major dense matrix
   dB4x3_.resize( 4UL, 3UL, false );
   dB4x3_(0,0) =  1;
   dB4x3_(0,1) =  0;
   dB4x3_(0,2) = -3;
   dB4x3_(1,0) =  0;
   dB4x3_(1,1) = -1;
   dB4x3_(1,2) =  0;
   dB4x3_(2,0) =  0;
   dB4x3_(2,1) =  2;
   dB4x3_(2,2) =  1;
   dB4x3_(3,0) =  2;
   dB4x3_(3,1) =  1;
   dB4x3_(3,2) = -2;

   // Initializing the third row-major dense matrix
   dC3x3_.resize( 3UL, 3UL, false );
   dC3x3_(0,0) =  1;
   dC3x3_(0,1) =  0;
   dC3x3_(0,2) =  2;
   dC3x3_(1,0) =  0;
   dC3x3_(1,1) =  3;
   dC3x3_(1,2) = -1;
   dC3x3_(2,0) = -1;
   dC3x3_(2,1) =  0;
   dC3x3_(2,2) =  2;

   // Initializing the fourth row-major dense matrix
   dD3x3_.resize( 3UL, 3UL, false );
   dD3x3_(0,0) =  0;
   dD3x3_(0,1) = -1;
   dD3x3_(0,2) =  0;
   dD3x3_(1,0) =  1;
   dD3x3_(1,1) = -2;
   dD3x3_(1,2) =  2;
   dD3x3_(2,0) =  0;
   dD3x3_(2,1) =  0;
   dD3x3_(2,2) = -3;

   // Initializing the fifth row-major dense matrix
   dE3x3_.resize( 3UL, 3UL, false );
   dE3x3_(0,0) =  2;
   dE3x3_(0,1) =  0;
   dE3x3_(0,2) =  0;
   dE3x3_(1,0) =  0;
   dE3x3_(1,1) =  1;
   dE3x3_(1,2) = -2;
   dE3x3_(2,0) =  1;
   dE3x3_(2,1) =  0;
   dE3x3_(2,2) =  0;

   // Initializing the first column-major dense matrix
   tdA3x4_.resize( 3UL, 4UL, false );
   tdA3x4_(0,0) = -1;
   tdA3x4_(0,1) =  0;
   tdA3x4_(0,2) = -2;
   tdA3x4_(0,3) =  0;
   tdA3x4_(1,0) =  0;
   tdA3x4_(1,1) =  2;
   tdA3x4_(1,2) = -3;
   tdA3x4_(1,3) =  1;
   tdA3x4_(2,0) =  0;
   tdA3x4_(2,1) =  1;
   tdA3x4_(2,2) =  2;
   tdA3x4_(2,3) =  2;

   // Initializing the second column-major dense matrix
   tdB4x3_.resize( 4UL, 3UL, false );
   tdB4x3_(0,0) =  1;
   tdB4x3_(0,1) =  0;
   tdB4x3_(0,2) = -3;
   tdB4x3_(1,0) =  0;
   tdB4x3_(1,1) = -1;
   tdB4x3_(1,2) =  0;
   tdB4x3_(2,0) =  0;
   tdB4x3_(2,1) =  2;
   tdB4x3_(2,2) =  1;
   tdB4x3_(3,0) =  2;
   tdB4x3_(3,1) =  1;
   tdB4x3_(3,2) = -2;

   // Initializing the third column-major dense matrix
   tdC3x3_.resize( 3UL, 3UL, false );
   tdC3x3_(0,0) =  1;
   tdC3x3_(0,1) =  0;
   tdC3x3_(0,2) =  2;
   tdC3x3_(1,0) =  0;
   tdC3x3_(1,1) =  3;
   tdC3x3_(1,2) = -1;
   tdC3x3_(2,0) = -1;
   tdC3x3_(2,1) =  0;
   tdC3x3_(2,2) =  2;

   // Initializing the fourth column-major dense matrix
   tdD3x3_.resize( 3UL, 3UL, false );
   tdD3x3_(0,0) =  0;
   tdD3x3_(0,1) = -1;
   tdD3x3_(0,2) =  0;
   tdD3x3_(1,0) =  1;
   tdD3x3_(1,1) = -2;
   tdD3x3_(1,2) =  2;
   tdD3x3_(2,0) =  0;
   tdD3x3_(2,1) =  0;
   tdD3x3_(2,2) = -3;

   // Initializing the fifth column-major dense matrix
   tdE3x3_.resize( 3UL, 3UL, false );
   tdE3x3_(0,0) =  2;
   tdE3x3_(0,1) =  0;
   tdE3x3_(0,2) =  0;
   tdE3x3_(1,0) =  0;
   tdE3x3_(1,1) =  1;
   tdE3x3_(1,2) = -2;
   tdE3x3_(2,0) =  1;
   tdE3x3_(2,1) =  0;
   tdE3x3_(2,2) =  0;
}
//*************************************************************************************************

} // namespace dmatdmatmult

} // namespace mathtest

} // namespace blazetest




//=================================================================================================
//
//  MAIN FUNCTION
//
//=================================================================================================

//*************************************************************************************************
int main()
{
   std::cout << "   Running aliasing test..." << std::endl;

   try
   {
      RUN_DMATDMATMULT_ALIASING_TEST;
   }
   catch( std::exception& ex ) {
      std::cerr << "\n\n ERROR DETECTED during aliasing test:\n"
                << ex.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
//*************************************************************************************************
