/**
 *  \file src/CSR/MatTrisolve/upper_normal.c
 *  \brief Non-transposed sparse triangular solve implementation when
 *  the matrix is upper triangular.
 *
 *  \todo The case of sorted indices assumes not only that the
 *  indices are sorted, but also that there is a unique diagonal
 *  element. Do we need to fix this? The general ordering case
 *  makes no such assumption.
 */
#include <assert.h>

#include <oski/config.h> /* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/vecview.h>
#include <oski/CSR/format.h>
#include <oski/CSR/module.h>

#if IS_VAL_COMPLEX
/** Complex-valued, so do not use explicit 'register' keyword */
#define REGISTER
#else
/** Real-valued, so use explicit 'register' keyword */
#define REGISTER register
#endif

/**
 *  \brief Solves \f$T\cdot x = \alpha\cdot b\f$ for \f$x\f$, where:
 *    - \f$T\f$ is upper triangular,
 *    - the diagonal is implicit and equal to 1,
 *    - the column indices within a given row may be unsorted, and
 *    - \f$x\f$ is an array with general stride.
 */
static void
CSR_MatTrisolveUpper_Unitdiag_xsX(
	oski_index_t m, oski_index_t indbase,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, oski_value_t* restrict x, oski_index_t incx )
{
	oski_index_t i;
	for( i = m-1; i >= 0; i-- )
	{
		REGISTER oski_value_t _xi;
		oski_index_t k;

		VAL_MUL( _xi, x[i*incx], alpha );
		for( k = ptr[i]-indbase; k < ptr[i+1]-indbase; k++ )
		{
			oski_index_t j = ind[k] - indbase;
			VAL_MSUB( _xi, val[k], x[j*incx] );
		}
		VAL_ASSIGN( x[i*incx], _xi );
	}
}

/**
 *  \brief Solves \f$T\cdot x = \alpha\cdot b\f$ for \f$x\f$, where:
 *    - \f$T\f$ is upper triangular,
 *    - the diagonal is implicit and equal to 1,
 *    - the column indices within a given row may be unsorted, and
 *    - \f$x\f$ is an array with unit-stride.
 */
static void
CSR_MatTrisolveUpper_Unitdiag_xs1(
	oski_index_t m, oski_index_t indbase,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, oski_value_t* x )
{
	oski_index_t i;
	for( i = m-1; i >= 0; i-- )
	{
		REGISTER oski_value_t _xi;
		oski_index_t k;

		VAL_MUL( _xi, x[i], alpha );
		for( k = ptr[i]-indbase; k < ptr[i+1]-indbase; k++ )
		{
			oski_index_t j = ind[k] - indbase;
			VAL_MSUB( _xi, val[k], x[j] );
		}
		VAL_ASSIGN( x[i], _xi );
	}
}

/**
 *  \brief Solves \f$T\cdot x = \alpha\cdot b\f$ for \f$x\f$, where:
 *    - \f$T\f$ is upper triangular,
 *    - a full (all non-zero) diagonal exists,
 *    - the column indices within a given row are sorted in increasing
 *      order (i.e., the last element of each row is the diagonal
 *      non-zero value), and
 *    - \f$x\f$ is an array with general stride.
 */
static void
CSR_MatTrisolveUpper_Diag_Sorted_xsX( oski_index_t m, oski_index_t indbase,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, oski_value_t* restrict x, oski_index_t incx )
{
	oski_index_t i;
	for( i = m-1; i >= 0; i-- )
	{
		REGISTER oski_value_t _xi;
		oski_index_t k;
		oski_value_t _di;

		/* assert( ind[ptr[i]]-indbase == i ); */
		VAL_ASSIGN( _di, val[ptr[i]-indbase] );

		VAL_MUL( _xi, x[i*incx], alpha );
		for( k = ptr[i]+1-indbase; k < ptr[i+1]-indbase; k++ )
		{
			oski_index_t j = ind[k] - indbase;
			VAL_MSUB( _xi, val[k], x[j*incx] );
		}
		/* assert( ind[k]-indbase == i ); */
		VAL_DIVEQ( _xi, _di );
		VAL_ASSIGN( x[i*incx], _xi );
	}
}

/**
 *  \brief Solves \f$T\cdot x = \alpha\cdot b\f$ for \f$x\f$, where:
 *    - \f$T\f$ is upper triangular,
 *    - a full (all non-zero) diagonal exists,
 *    - the column indices within a given row are sorted in increasing
 *      order (i.e., the last element of each row is the diagonal
 *      non-zero value), and
 *    - \f$x\f$ is an array with unit-stride.
 */
static void
CSR_MatTrisolveUpper_Diag_Sorted_xs1( oski_index_t m, oski_index_t indbase,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, oski_value_t* x )
{
	oski_index_t i;
	for( i = m-1; i >= 0; i-- )
	{
		REGISTER oski_value_t _xi;
		oski_index_t k;
		oski_value_t _di;

		/* assert( ind[ptr[i]]-indbase == i ); */
		VAL_ASSIGN( _di, val[ptr[i]-indbase] );

		VAL_MUL( _xi, x[i], alpha );
		for( k = ptr[i]+1-indbase; k < ptr[i+1]-indbase; k++ )
		{
			oski_index_t j = ind[k] - indbase;
			VAL_MSUB( _xi, val[k], x[j] );
		}
		VAL_DIVEQ( _xi, _di );
		VAL_ASSIGN( x[i], _xi );
	}
}

/**
 *  \brief Solves \f$T\cdot x = \alpha\cdot b\f$ for \f$x\f$, where:
 *    - \f$T\f$ is upper triangular,
 *    - a full (all non-zero) diagonal exists,
 *    - the column indices within a given row may be unsorted, and
 *    - \f$x\f$ is an array with general stride.
 */
static void
CSR_MatTrisolveUpper_Diag_Unsorted_xsX( oski_index_t m, oski_index_t indbase,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, oski_value_t* restrict x, oski_index_t incx )
{
	oski_index_t i;
	for( i = m-1; i >= 0; i-- )
	{
		REGISTER oski_value_t _xi;
		oski_index_t k;
		oski_value_t _di; /* stores diagonal element */

		VAL_SET_ZERO( _di );
		VAL_MUL( _xi, x[i*incx], alpha );
		for( k = ptr[i]-indbase; k < ptr[i+1]-indbase; k++ )
		{
			oski_index_t j = ind[k] - indbase;
			if( j != i )
				VAL_MSUB( _xi, val[k], x[j*incx] );
			else
				VAL_INC( _di, val[k] );
		}
		VAL_DIVEQ( _xi, _di );
		VAL_ASSIGN( x[i*incx], _xi );
	}
}

/**
 *  \brief Solves \f$T\cdot x = \alpha\cdot b\f$ for \f$x\f$, where:
 *    - \f$T\f$ is upper triangular,
 *    - a full (all non-zero) diagonal exists,
 *    - the column indices within a given row may be unsorted, and
 *    - \f$x\f$ is an array with unit-stride.
 */
static void
CSR_MatTrisolveUpper_Diag_Unsorted_xs1( oski_index_t m, oski_index_t indbase,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, oski_value_t* x )
{
	oski_index_t i;
	for( i = m-1; i >= 0; i-- )
	{
		REGISTER oski_value_t _xi;
		oski_index_t k;
		oski_value_t _di; /* stores diagonal element */

		VAL_SET_ZERO( _di );
		VAL_MUL( _xi, x[i], alpha );
		for( k = ptr[i]-indbase; k < ptr[i+1]-indbase; k++ )
		{
			oski_index_t j = ind[k] - indbase;
			if( j != i )
				VAL_MSUB( _xi, val[k], x[j] );
			else
				VAL_INC( _di, val[k] );
		}
		VAL_DIVEQ( _xi, _di );
		VAL_ASSIGN( x[i], _xi );
	}
}

void
CSR_MatTrisolveUpper(
	oski_index_t m, oski_index_t indbase, int is_unit, int is_sorted,
	const oski_index_t* ptr, const oski_index_t* ind, const oski_value_t* val,
	oski_value_t alpha, oski_value_t* restrict x, oski_index_t num_vecs,
	oski_index_t rowinc, oski_index_t colinc )
{
	oski_index_t k;
	oski_value_t* xp;

	if( is_unit )
	{
		if( rowinc == 1 )
			for( k = 0, xp = x; k < num_vecs; k++, xp += colinc )
				CSR_MatTrisolveUpper_Unitdiag_xs1( m, indbase,
					ptr, ind, val, alpha, xp );
		else /* rowinc > 1 */
			for( k = 0, xp = x; k < num_vecs; k++, xp += colinc )
				CSR_MatTrisolveUpper_Unitdiag_xsX( m, indbase,
					ptr, ind, val, alpha, xp, rowinc );
	}
	else /* ! is_unit */
	{
		if( is_sorted )
		{
			if( rowinc == 1 )
				for( k = 0, xp = x; k < num_vecs; k++, xp += colinc )
					CSR_MatTrisolveUpper_Diag_Sorted_xs1( m, indbase,
						ptr, ind, val, alpha, xp );
			else /* rowinc != 1 */
				for( k = 0, xp = x; k < num_vecs; k++, xp += colinc )
					CSR_MatTrisolveUpper_Diag_Sorted_xsX( m, indbase,
						ptr, ind, val, alpha, xp, rowinc );
		}
		else /* ! is_sorted */
		{
			if( rowinc == 1 )
				for( k = 0, xp = x; k < num_vecs; k++, xp += colinc )
					CSR_MatTrisolveUpper_Diag_Unsorted_xs1( m, indbase,
						ptr, ind, val, alpha, xp );
			else /* rowinc != 1 */
				for( k = 0, xp = x; k < num_vecs; k++, xp += colinc )
					CSR_MatTrisolveUpper_Diag_Unsorted_xsX( m, indbase,
						ptr, ind, val, alpha, xp, rowinc );
		}
	}
}


/* eof */
