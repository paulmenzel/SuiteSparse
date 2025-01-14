//------------------------------------------------------------------------------
// UMFPACK/Source/umfpack_transpose: transpose/permute a sparse matrix
//------------------------------------------------------------------------------

// UMFPACK, Copyright (c) 2005-2022, Timothy A. Davis, All Rights Reserved.
// SPDX-License-Identifier: GPL-2.0+

//------------------------------------------------------------------------------

/*
    User callable.  Computes a permuted transpose, R = (A (P,Q))' in MATLAB
    notation.  See umfpack.h for details.  A and R can be rectangular.
    The matrix A may be singular.
    The complex version can do transpose (') or array transpose (.').

    Dynamic memory usage: A single call to UMF_malloc is made, for a workspace
    of size max (n_row,n_col,1) * sizeof(Int).  This is then free'd on return,
    via UMF_free.
*/

#include "umf_internal.h"
#include "umf_transpose.h"
#include "umf_malloc.h"
#include "umf_free.h"

#ifndef NDEBUG
PRIVATE Int init_count ;
#endif

/* ========================================================================== */

GLOBAL int UMFPACK_transpose
(
    Int n_row,
    Int n_col,
    const Int Ap [ ],	/* size n_col+1 */
    const Int Ai [ ],	/* size nz = Ap [n_col] */
    const double Ax [ ], /* size nz, if present */
#ifdef COMPLEX
    const double Az [ ], /* size nz, if present */
#endif

    const Int P [ ],	/* P [k] = i means original row i is kth row in A(P,Q)*/
			/* P is identity if not present */
			/* size n_row, if present */

    const Int Q [ ],	/* Q [k] = j means original col j is kth col in A(P,Q)*/
			/* Q is identity if not present */
			/* size n_col, if present */

    Int Rp [ ],		/* size n_row+1 */
    Int Ri [ ],		/* size nz */
    double Rx [ ]	/* size nz, if present */
#ifdef COMPLEX
    , double Rz [ ]	/* size nz, if present */
    , int do_conjugate	/* if true, then to conjugate transpose */
			/* otherwise, do array transpose */
#endif
)
{

    /* ---------------------------------------------------------------------- */
    /* local variables */
    /* ---------------------------------------------------------------------- */

    Int *W, nn ;
    int status ;

#ifndef NDEBUG
    init_count = UMF_malloc_count ;
    UMF_dump_start ( ) ;
#endif

    /* ---------------------------------------------------------------------- */
    /* allocate workspace */
    /* ---------------------------------------------------------------------- */

    nn = MAX (n_row, n_col) ;
    nn = MAX (nn, 1) ;
    W = (Int *) UMF_malloc (nn, sizeof (Int)) ;
    if (!W)
    {
	DEBUGm4 (("out of memory: transpose work\n")) ;
	ASSERT (UMF_malloc_count == init_count) ;
	return (UMFPACK_ERROR_out_of_memory) ;
    }
    ASSERT (UMF_malloc_count == init_count + 1) ;

    /* ---------------------------------------------------------------------- */
    /* C = (A (P,Q))' or (A (P,Q)).' */
    /* ---------------------------------------------------------------------- */

    status = UMF_transpose (n_row, n_col, Ap, Ai, Ax, P, Q, n_col, Rp, Ri, Rx,
	W, TRUE
#ifdef COMPLEX
	, Az, Rz, do_conjugate
#endif
	) ;

    /* ---------------------------------------------------------------------- */
    /* free the workspace */
    /* ---------------------------------------------------------------------- */

    (void) UMF_free ((void *) W) ;
    ASSERT (UMF_malloc_count == init_count) ;

    return (status) ;
}
