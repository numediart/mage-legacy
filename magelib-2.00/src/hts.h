/* --------------------------------------------------------------------------------------------	*/
/*																								*/
/*	This file is part of MAGE / pHTS( the performative HMM-based speech synthesis system )		*/
/*																								*/
/*	MAGE / pHTS is free software: you can redistribute it and/or modify it under the terms		*/
/*	of the GNU General Public License as published by the Free Software Foundation, either		*/
/*	version 3 of the license, or any later version.												*/
/*																								*/
/*	MAGE / pHTS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;	*/	
/*	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	*/
/*	See the GNU General Public License for more details.										*/
/*																								*/	
/*	You should have received a copy of the GNU General Public License along with MAGE / pHTS.	*/ 
/*	If not, see http://www.gnu.org/licenses/													*/
/*																								*/
/*																								*/	
/*	Copyright 2011 University of Mons :															*/
/*																								*/	
/*			Numediart Institute for New Media Art( www.numediart.org )							*/
/*			Acapela Group ( www.acapela-group.com )												*/
/*																								*/
/*																								*/
/*	 Developed by :																				*/
/*																								*/
/*		Maria Astrinaki, Geoffrey Wilfart, Alexis Moinet, Nicolas d'Alessandro, Thierry Dutoit	*/
/*																								*/
/* --------------------------------------------------------------------------------------------	*/

#pragma once

#include "MathFunctions.h"
#include "HTS_engine.h"
#include "HTS_hidden.h"

#include <cstdlib>
#include <stdarg.h>

// --- HTS Engine

/* Usage: output usage */
void Usage( void );

/* Error: output error message */
void Error( const int error, char *message, ... );

/* GetNumInterp: get number of speakers for interpolation from argv */
int GetNumInterp( int argc, char **argv_search );

// -- HTS Labels

bool isdigit_string( char *str );

/* HTS_set_duration: set duration from state duration probability distribution */
double mHTS_set_duration( int *duration, double *mean, double *vari, int size, double frame_length );

// --- HTS PStream

/* HTS_finv: calculate 1.0/variance function */
double HTS_finv( const double x );

/* HTS_PStream_mlpg: generate sequence of speech parameter vector maximizing its output probability for given pdf sequence */
void HTS_PStream_mlpg( HTS_PStream * pst );

/* HTS_PStream_calc_wuw_and_wum: calcurate W'U^{-1}W and W'U^{-1}M */
void HTS_PStream_calc_wuw_and_wum( HTS_PStream * pst, const int m );

/* HTS_PStream_ldl_factorization: Factorize W'*U^{-1}*W to L*D*L'( L: lower triangular, D: diagonal )*/
void HTS_PStream_ldl_factorization( HTS_PStream * pst );

/* HTS_PStream_forward_substitution: forward subtitution for mlpg */
void HTS_PStream_forward_substitution( HTS_PStream * pst );

/* HTS_PStream_backward_substitution: backward subtitution for mlpg */
void HTS_PStream_backward_substitution( HTS_PStream * pst, const int m );

/* HTS_PStream_gv_parmgen: function for mlpg using GV */
void HTS_PStream_gv_parmgen( HTS_PStream * pst, const int m );

/* HTS_PStream_conv_gv: subfunction for mlpg using GV */
void HTS_PStream_conv_gv( HTS_PStream * pst, const int m );

/* HTS_PStream_calc_derivative: subfunction for mlpg using GV */
double HTS_PStream_calc_derivative( HTS_PStream * pst, const int m );

/* HTS_PStream_calc_gv: subfunction for mlpg using GV */
void HTS_PStream_calc_gv( HTS_PStream * pst, const int m, double *mean, double *vari );

