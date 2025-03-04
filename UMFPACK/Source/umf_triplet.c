//------------------------------------------------------------------------------
// UMFPACK/Source/umf_triplet: convert triplet to CSC sparse matrix
//------------------------------------------------------------------------------

// UMFPACK, Copyright (c) 2005-2022, Timothy A. Davis, All Rights Reserved.
// SPDX-License-Identifier: GPL-2.0+

//------------------------------------------------------------------------------

/*
    Not user callable.  Converts triplet input to column-oriented form.
    Duplicate entries may exist (they are summed in the output).  The columns
    of the column-oriented form are in sorted order.  The input is not modified.
    Returns 1 if OK, 0 if an error occurred.

    Compiled into four different routines for each version (di, dl, zi, zl),
    for a total of 16 different routines.
*/

#include "umf_internal.h"
#include "umf_triplet.h"

#ifdef DO_MAP
#ifdef DO_VALUES
GLOBAL Int UMF_triplet_map_x
#else
GLOBAL Int UMF_triplet_map_nox
#endif
#else
#ifdef DO_VALUES
GLOBAL Int UMF_triplet_nomap_x
#else
GLOBAL Int UMF_triplet_nomap_nox
#endif
#endif
(
    Int n_row,
    Int n_col,
    Int nz,
    const Int Ti [ ],		/* size nz */
    const Int Tj [ ],		/* size nz */
    Int Ap [ ],			/* size n_col + 1 */
    Int Ai [ ],			/* size nz */
    Int Rp [ ],			/* size n_row + 1 */
    Int Rj [ ],			/* size nz */
    Int W [ ],			/* size max (n_row, n_col) */
    Int RowCount [ ]		/* size n_row */
#ifdef DO_VALUES
    , const double Tx [ ]	/* size nz */
    , double Ax [ ]		/* size nz */
    , double Rx [ ]		/* size nz */
#ifdef COMPLEX
    , const double Tz [ ]	/* size nz */
    , double Az [ ]		/* size nz */
    , double Rz [ ]		/* size nz */
#endif
#endif
#ifdef DO_MAP
    , Int Map [ ]		/* size nz */
    , Int Map2 [ ]		/* size nz */
#endif
)
{

    /* ---------------------------------------------------------------------- */
    /* local variables */
    /* ---------------------------------------------------------------------- */

    Int i, j, k, p, cp, p1, p2, pdest, pj ;
#ifdef DO_MAP
    Int duplicates ;
#endif
#ifdef DO_VALUES
#ifdef COMPLEX
    Int split = SPLIT (Tz) && SPLIT (Az) && SPLIT (Rz) ;
#endif
#endif

    /* ---------------------------------------------------------------------- */
    /* count the entries in each row (also counting duplicates) */
    /* ---------------------------------------------------------------------- */

    /* use W as workspace for row counts (including duplicates) */
    for (i = 0 ; i < n_row ; i++)
    {
	W [i] = 0 ;
    }

    for (k = 0 ; k < nz ; k++)
    {
	i = Ti [k] ;
	j = Tj [k] ;
	if (i < 0 || i >= n_row || j < 0 || j >= n_col)
	{
	    return (UMFPACK_ERROR_invalid_matrix) ;
	}
	W [i]++ ;
#ifndef NDEBUG
	DEBUG1 ((ID " triplet: "ID" "ID" ", k, i, j)) ;
#ifdef DO_VALUES
	{
	    Entry tt ;
	    ASSIGN (tt, Tx, Tz, k, split) ;
	    EDEBUG2 (tt) ;
	    DEBUG1 (("\n")) ;
	}
#endif
#endif
    }

    /* ---------------------------------------------------------------------- */
    /* compute the row pointers */
    /* ---------------------------------------------------------------------- */

    Rp [0] = 0 ;
    for (i = 0 ; i < n_row ; i++)
    {
	Rp [i+1] = Rp [i] + W [i] ;
	W [i] = Rp [i] ;
    }

    /* W is now equal to the row pointers */

    /* ---------------------------------------------------------------------- */
    /* construct the row form */
    /* ---------------------------------------------------------------------- */

    for (k = 0 ; k < nz ; k++)
    {
	p = W [Ti [k]]++ ;
#ifdef DO_MAP
	Map [k] = p ;
#endif
	Rj [p] = Tj [k] ;
#ifdef DO_VALUES
#ifdef COMPLEX
	if (split)
	{
	    Rx [p] = Tx [k] ;
	    Rz [p] = Tz [k] ;
	}
	else
	{
	    Rx [2*p  ] = Tx [2*k  ] ;
	    Rx [2*p+1] = Tx [2*k+1] ;
	}
#else
	Rx [p] = Tx [k] ;
#endif
#endif
    }

    /* Rp stays the same, but W [i] is advanced to the start of row i+1 */

#ifndef NDEBUG
    for (i = 0 ; i < n_row ; i++)
    {
	ASSERT (W [i] == Rp [i+1]) ;
    }
#ifdef DO_MAP
    for (k = 0 ; k < nz ; k++)
    {
	/* make sure that kth triplet is mapped correctly */
	p = Map [k] ;
	DEBUG1 (("First row map: Map ["ID"] = "ID"\n", k, p)) ;
	i = Ti [k] ;
	j = Tj [k] ;
	ASSERT (j == Rj [p]) ;
	ASSERT (Rp [i] <= p && p < Rp [i+1]) ;
    }
#endif
#endif

    /* ---------------------------------------------------------------------- */
    /* sum up duplicates */
    /* ---------------------------------------------------------------------- */

    /* use W [j] to hold position in Ri/Rx/Rz of a_ij, for row i [ */

    for (j = 0 ; j < n_col ; j++)
    {
	W [j] = EMPTY ;
    }

#ifdef DO_MAP
    duplicates = FALSE ;
#endif

    for (i = 0 ; i < n_row ; i++)
    {
	p1 = Rp [i] ;
	p2 = Rp [i+1] ;
	pdest = p1 ;
	/* At this point, W [j] < p1 holds true for all columns j, */
	/* because Ri/Rx/Rz is stored in row oriented order. */
#ifndef NDEBUG
	if (UMF_debug >= -2)
	{
	    for (j = 0 ; j < n_col ; j++)
	    {
		ASSERT (W [j] < p1) ;
	    }
	}
#endif
	for (p = p1 ; p < p2 ; p++)
	{
	    j = Rj [p] ;
	    ASSERT (j >= 0 && j < n_col) ;
	    pj = W [j] ;
	    if (pj >= p1)
	    {
		/* this column index, j, is already in row i, at position pj */
		ASSERT (pj < p) ;
		ASSERT (Rj [pj] == j) ;
#ifdef DO_MAP
		Map2 [p] = pj ;
		duplicates = TRUE ;
#endif
#ifdef DO_VALUES
		/* sum the entry */
#ifdef COMPLEX
		if (split)
		{
		    Rx [pj] += Rx [p] ;
		    Rz [pj] += Rz [p] ;
		}
		else
		{
		    Rx[2*pj  ] += Rx[2*p  ] ;
		    Rx[2*pj+1] += Rx[2*p+1] ;
		}
#else
		Rx [pj] += Rx [p] ;
#endif
#endif
	    }
	    else
	    {
		/* keep the entry */
		/* also keep track in W[j] of position of a_ij for case above */
		W [j] = pdest ;
#ifdef DO_MAP
		Map2 [p] = pdest ;
#endif
		/* no need to move the entry if pdest is equal to p */
		if (pdest != p)
		{
		    Rj [pdest] = j ;
#ifdef DO_VALUES
#ifdef COMPLEX
		    if (split)
		    {
			Rx [pdest] = Rx [p] ;
			Rz [pdest] = Rz [p] ;
		    }
		    else
		    {
			Rx [2*pdest  ] = Rx [2*p  ] ;
			Rx [2*pdest+1] = Rx [2*p+1] ;
		    }
#else
		    Rx [pdest] = Rx [p] ;
#endif
#endif
		}
		pdest++ ;
	    }
	}
	RowCount [i] = pdest - p1 ;
    }

    /* done using W for position of a_ij ] */

    /* ---------------------------------------------------------------------- */
    /* merge Map and Map2 into a single Map */
    /* ---------------------------------------------------------------------- */

#ifdef DO_MAP
    if (duplicates)
    {
	for (k = 0 ; k < nz ; k++)
	{
	    Map [k] = Map2 [Map [k]] ;
	}
    }
#ifndef NDEBUG
    else
    {
	/* no duplicates, so no need to recompute Map */
	for (k = 0 ; k < nz ; k++)
	{
	    ASSERT (Map2 [k] == k) ;
	}
    }
    for (k = 0 ; k < nz ; k++)
    {
	/* make sure that kth triplet is mapped correctly */
	p = Map [k] ;
	DEBUG1 (("Second row map: Map ["ID"] = "ID"\n", k, p)) ;
	i = Ti [k] ;
	j = Tj [k] ;
	ASSERT (j == Rj [p]) ;
	ASSERT (Rp [i] <= p && p < Rp [i+1]) ;
    }
#endif
#endif

    /* now the kth triplet maps to p = Map [k], and thus to Rj/Rx [p] */

    /* ---------------------------------------------------------------------- */
    /* count the entries in each column */
    /* ---------------------------------------------------------------------- */

    /* [ use W as work space for column counts of A */
    for (j = 0 ; j < n_col ; j++)
    {
	W [j] = 0 ;
    }

    for (i = 0 ; i < n_row ; i++)
    {
	for (p = Rp [i] ; p < Rp [i] + RowCount [i] ; p++)
	{
	    j = Rj [p] ;
	    ASSERT (j >= 0 && j < n_col) ;
	    W [j]++ ;
	}
    }

    /* ---------------------------------------------------------------------- */
    /* create the column pointers */
    /* ---------------------------------------------------------------------- */

    Ap [0] = 0 ;
    for (j = 0 ; j < n_col ; j++)
    {
	Ap [j+1] = Ap [j] + W [j] ;
    }
    /* done using W as workspace for column counts of A ] */

    for (j = 0 ; j < n_col ; j++)
    {
	W [j] = Ap [j] ;
    }

    /* ---------------------------------------------------------------------- */
    /* construct the column form */
    /* ---------------------------------------------------------------------- */

    for (i = 0 ; i < n_row ; i++)
    {
	for (p = Rp [i] ; p < Rp [i] + RowCount [i] ; p++)
	{
	    cp = W [Rj [p]]++ ;
#ifdef DO_MAP
	    Map2 [p] = cp ;
#endif
	    Ai [cp] = i ;
#ifdef DO_VALUES
#ifdef COMPLEX
	    if (split)
	    {
		Ax [cp] = Rx [p] ;
		Az [cp] = Rz [p] ;
	    }
	    else
	    {
		Ax [2*cp  ] = Rx [2*p  ] ;
		Ax [2*cp+1] = Rx [2*p+1] ;
	    }
#else
	    Ax [cp] = Rx [p] ;
#endif
#endif
	}
    }

    /* ---------------------------------------------------------------------- */
    /* merge Map and Map2 into a single Map */
    /* ---------------------------------------------------------------------- */

#ifdef DO_MAP
    for (k = 0 ; k < nz ; k++)
    {
	Map [k] = Map2 [Map [k]] ;
    }
#endif

    /* now the kth triplet maps to p = Map [k], and thus to Ai/Ax [p] */

#ifndef NDEBUG
    for (j = 0 ; j < n_col ; j++)
    {
	ASSERT (W [j] == Ap [j+1]) ;
    }

    UMF_dump_col_matrix (
#ifdef DO_VALUES
	Ax,
#ifdef COMPLEX
	Az,
#endif
#else
	(double *) NULL,
#ifdef COMPLEX
	(double *) NULL,
#endif
#endif
	Ai, Ap, n_row, n_col, nz) ;

#ifdef DO_MAP
    for (k = 0 ; k < nz ; k++)
    {
	/* make sure that kth triplet is mapped correctly */
	p = Map [k] ;
	DEBUG1 (("Col map: Map ["ID"] = "ID"\t", k, p)) ;
	i = Ti [k] ;
	j = Tj [k] ;
	ASSERT (i == Ai [p]) ;
	DEBUG1 (("   i "ID" j "ID" Ap[j] "ID" p "ID" Ap[j+1] "ID"\n",
		i, j, Ap [j], p, Ap [j+1])) ;
	ASSERT (Ap [j] <= p && p < Ap [j+1]) ;
    }
#endif
#endif

    return (UMFPACK_OK) ;
}
