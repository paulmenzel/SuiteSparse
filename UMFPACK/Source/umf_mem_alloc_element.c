//------------------------------------------------------------------------------
// UMFPACK/Source/umf_mem_alloc_element: allocate element in Numeric->Memory
//------------------------------------------------------------------------------

// UMFPACK, Copyright (c) 2005-2022, Timothy A. Davis, All Rights Reserved.
// SPDX-License-Identifier: GPL-2.0+

//------------------------------------------------------------------------------

/* The UMF_mem_* routines manage the Numeric->Memory memory space. */

/* Allocate a nrows-by-ncols element, and initialize it. */
/* Returns the index into Numeric->Memory if successful, or 0 on failure. */

#include "umf_internal.h"
#include "umf_mem_alloc_element.h"
#include "umf_mem_alloc_tail_block.h"

GLOBAL Int UMF_mem_alloc_element
(
    NumericType *Numeric,
    Int nrows,
    Int ncols,
    Int **Rows,
    Int **Cols,
    Entry **C,
    Int *size,
    Element **epout
)
{

    Element *ep ;
    Unit *p ;
    Int i ;

    ASSERT (Numeric != (NumericType *) NULL) ;
    ASSERT (Numeric->Memory != (Unit *) NULL) ;

    *size = GET_ELEMENT_SIZE (nrows, ncols) ;
    if (INT_OVERFLOW (DGET_ELEMENT_SIZE (nrows, ncols) + 1))
    {
	/* :: allocate element, int overflow :: */
	return (0) ;	/* problem is too large */
    }

    i = UMF_mem_alloc_tail_block (Numeric, *size) ;
    (*size)++ ;
    if (!i)
    {
	DEBUG0 (("alloc element failed - out of memory\n")) ;
	return (0) ;	/* out of memory */
    }
    p = Numeric->Memory + i ;

    ep = (Element *) p ;

    DEBUG2 (("alloc_element done ("ID" x "ID"): p: "ID" i "ID"\n",
	nrows, ncols, (Int) (p-Numeric->Memory), i)) ;

    /* Element data structure, in order: */
    p += UNITS (Element, 1) ;		/* (1) Element header */
    *Cols = (Int *) p ;			/* (2) col [0..ncols-1] indices */
    *Rows = *Cols + ncols ;		/* (3) row [0..nrows-1] indices */
    p += UNITS (Int, ncols + nrows) ;
    *C = (Entry *) p ;			/* (4) C [0..nrows-1, 0..ncols-1] */

    ep->nrows = nrows ;		/* initialize the header information */
    ep->ncols = ncols ;
    ep->nrowsleft = nrows ;
    ep->ncolsleft = ncols ;
    ep->cdeg = 0 ;
    ep->rdeg = 0 ;
    ep->next = EMPTY ;

    DEBUG2 (("new block size: "ID" ", GET_BLOCK_SIZE (Numeric->Memory + i))) ;
    DEBUG2 (("Element size needed "ID"\n", GET_ELEMENT_SIZE (nrows, ncols))) ;

    *epout = ep ;

    /* return the offset into Numeric->Memory */
    return (i) ;
}
