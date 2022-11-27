// Program generates an inclusion file with SIMD variants of "convolve"
// functions, for CDSPHBUpsampler.h and CDSPHBDownsampler.h.

#include <stdio.h>
#define IS_UPSAMPLE 0

void printScalar( const int n )
{
#if IS_UPSAMPLE
	printf( "op[ 1 ] = flt[ 0 ] * ( rp[ 1 ] + rp[ 0 ])" );
#else // IS_UPSAMPLE
	printf( "op[ 0 ] = rp1[ 0 ] + flt[ 0 ] * ( rp[ 1 ] + rp[ 0 ])" );
#endif // IS_UPSAMPLE

	int i;

	for( i = 1; i < n; i++ )
	{
		printf( "\n\t+ flt[ %i ] * ( rp[ %i ] + rp[ %i ])",
			i, i + 1, -i );
	}

	printf( ";\n" );
}

void printSSE( const int n )
{
	if( n == 1 )
	{
		printScalar( 1 );
		return;
	}

	printf( "__m128d v1, v2, m1, s1;\n" );

	int i;

	for( i = 1; i < n; i += 2 )
	{
		if( i == 3 )
		{
			printf( "__m128d v3, v4, m3, s3;\n" );
		}

		const int k1 = ( i & 2 ) + 1;
		const int k2 = k1 + 1;

		printf( "v%i = _mm_loadu_pd( rp - %i ); ", k2, i );
		printf( "v%i = _mm_loadu_pd( rp + %i );\n", k1, i );
		printf( "m%i = _mm_mul_pd( _mm_load_pd( flt + %i ),\n", k1, i - 1 );
		printf( "\t_mm_add_pd( v%i, _mm_shuffle_pd( v%i, v%i, 1 )));\n",
			k1, k2, k2 );

		if( i == 1 )
		{
			printf( "s1 = m1;\n" );
		}
		else
		if( i == 3 )
		{
			printf( "s3 = m3;\n" );
		}
		else
		{
			printf( "s%i = _mm_add_pd( s%i, m%i );\n", k1, k1, k1 );
		}
	}

	if( i > 3 )
	{
		printf( "s1 = _mm_add_pd( s1, s3 );\n" );
	}

#if IS_UPSAMPLE
	printf( "_mm_storel_pd( op + 1, _mm_add_pd( s1, "
		"_mm_shuffle_pd( s1, s1, 1 )));\n" );

	if(( n & 1 ) != 0 )
	{
		printf( "op[ 1 ] += flt[ %i ] * ( rp[ %i ] + rp[ -%i ]);\n",
			n - 1, n, n - 1 );
	}
#else // IS_UPSAMPLE
	printf( "_mm_storel_pd( op, _mm_add_pd( s1, "
		"_mm_shuffle_pd( s1, s1, 1 )));\n" );

	if(( n & 1 ) != 0 )
	{
		printf( "op[ 0 ] += rp1[ 0 ] + flt[ %i ] * ( rp[ %i ] + rp[ -%i ]);\n",
			n - 1, n, n - 1 );
	}
	else
	{
		printf( "op[ 0 ] += rp1[ 0 ];\n" );
	}
#endif // IS_UPSAMPLE
}

void printNEON( const int n )
{
	if( n == 1 )
	{
		printScalar( 1 );
		return;
	}

	printf( "float64x2_t v1, v2, s1;\n" );
	printf( "s1 = vdupq_n_f64( 0.0 );\n" );

	int i;

	for( i = 1; i < n; i += 2 )
	{
		if( i == 3 )
		{
			printf( "float64x2_t v3, v4, s3;\n" );
			printf( "s3 = vdupq_n_f64( 0.0 );\n" );
		}

		const int k1 = ( i & 2 ) + 1;
		const int k2 = k1 + 1;

		printf( "v%i = vld1q_f64( rp - %i ); ", k2, i );
		printf( "v%i = vld1q_f64( rp + %i );\n", k1, i );
		printf( "s%i = vmlaq_f64( s%i, vld1q_f64( flt + %i ),\n",
			k1, k1, i - 1 );

		printf( "\tvaddq_f64( v%i, vextq_f64( v%i, v%i, 1 )));\n",
			k1, k2, k2 );
	}

	if( i > 3 )
	{
		printf( "s1 = vaddq_f64( s1, s3 );\n" );
	}

#if IS_UPSAMPLE
	if(( n & 1 ) != 0 )
	{
		printf( "op[ 1 ] = vaddvq_f64( s1 ) + "
			"flt[ %i ] * ( rp[ %i ] + rp[ -%i ]);\n", n - 1, n, n - 1 );
	}
	else
	{
		printf( "op[ 1 ] = vaddvq_f64( s1 );\n" );
	}
#else // IS_UPSAMPLE
	if(( n & 1 ) != 0 )
	{
		printf( "op[ 0 ] = vaddvq_f64( s1 ) + rp1[ 0 ] + "
			"flt[ %i ] * ( rp[ %i ] + rp[ -%i ]);\n", n - 1, n, n - 1 );
	}
	else
	{
		printf( "op[ 0 ] = vaddvq_f64( s1 ) + rp1[ 0 ];\n" );
	}
#endif // IS_UPSAMPLE
}

int main()
{
	int i;

	printf( "// Auto-generated by `genhbc`, do not edit!\n\n" );
	printf( "#if defined( R8B_SSE2 )\n\n" );

	for( i = 1; i <= 14; i++ )
	{
		printf( "R8BHBC1( convolve%i )\n", i );
		printSSE( i );
		printf( "R8BHBC2\n\n" );
	}

	printf( "#elif defined( R8B_NEON )\n\n" );

	for( i = 1; i <= 14; i++ )
	{
		printf( "R8BHBC1( convolve%i )\n", i );
		printNEON( i );
		printf( "R8BHBC2\n\n" );
	}

	printf( "#else // SIMD\n\n" );

	for( i = 1; i <= 14; i++ )
	{
		printf( "R8BHBC1( convolve%i )\n", i );
		printScalar( i );
		printf( "R8BHBC2\n\n" );
	}

	printf( "#endif // SIMD\n" );
}
