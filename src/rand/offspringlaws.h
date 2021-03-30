/*
 * We provide some offspring distributions for our simulations
 */

/*
 * Provides a quadrangulation type offspring law
 *		xi[i] = const * (i+1) * (1/3)**i
 */
mpfr_t *xiquad(INT n) {
	mpfr_t *xi;
	INT i;
	mpfr_t norm, mpi, con1, con2, base, fac1, fac2;

	// initialize (warning: mpfr sets to NaN, gmp sets to 0.0)
	mpfr_init2(norm, PREC);
	mpfr_init2(mpi, PREC);
	mpfr_init2(con1, PREC);
	mpfr_init2(con2, PREC);
	mpfr_init2(base, PREC);
	mpfr_init2(fac1, PREC);
	mpfr_init2(fac2, PREC);

	xi = (mpfr_t *) calloc(n, sizeof(mpfr_t));
	if(xi == NULL) {
		// memory allocation error
		fprintf(stderr, "Memory allocation error in function xitria\n");
		exit(-1);
	}
	for(i=0; i<n; i++)
		mpfr_init2(xi[i], PREC);

	/*
	for(i=0; i<n; i++) {
		xi[i] = (i+1) * (1/3)**i
	}
	*/
	mpfr_set_ui(con1, 1, MPFR_RNDN);
	mpfr_set_ui(con2, 3, MPFR_RNDN);
	mpfr_div(base, con1, con2, MPFR_RNDN);

	mpfr_set_ui(xi[0], 1, MPFR_RNDN);
	for(i=1; i<n; i++) {
		// stay realistic with precomputing
		if(i > 100000) {
			mpfr_set_ui(xi[i], 0, MPFR_RNDN);
		} else {
		mpfr_set_ui(mpi, i, MPFR_RNDN);
		mpfr_add(fac1, mpi, con1, MPFR_RNDN);
		mpfr_pow(fac2, base, mpi, MPFR_RNDN);
		mpfr_mul(xi[i], fac1, fac2, MPFR_RNDN); 
		}
	}


	// normalize 
	/*
	for(i=0, norm=0.0; i<n; i++)
		norm += xi[i];
	for(i=0; i<n; i++)
		xi[i] /= norm;
	*/

	mpfr_set_ui(norm, 0, MPFR_RNDN);	
	for(i=0; i<n; i++)
		mpfr_add(norm, norm, xi[i], MPFR_RNDN);

	for(i=0; i<n; i++)
		mpfr_div(xi[i], xi[i], norm, MPFR_RNDN);

	//DEBUG
	/*
	DOUBLE tmp;
	for(i=0; i<n; i++) {
		tmp = mpfr_get_ld(xi[i], MPFR_RNDN);
		printf("p[%" STR(FINT) "] = %" STR(FDOUBLE) "\n", i, tmp);
	}
	*/


	// clean up
	mpfr_clear(norm);
	mpfr_clear(mpi);
	mpfr_clear(con1);
	mpfr_clear(con2);
	mpfr_clear(base);
	mpfr_clear(fac1);
	mpfr_clear(fac2);


	return xi;
}



