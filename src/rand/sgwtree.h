/*
 * generate degree sequence from degree profile
 */
INT *gendegsequence(INT *N, INT size, gsl_rng *rgen) {
	INT i, j, p;	
	INT *out;

	// result will be stored in an array of integers
	out = (INT *) calloc(size, sizeof(INT));
	if(out == NULL) {
		// memory allocation error
		fprintf(stderr, "Memory allocation error in function gendegsequence\n");
		exit(-1);
	}

	// create initial array with sequential list of outdegrees
	for(i=0, p=0; i<size; i++)
		for(j=0; j<N[i]; j++, p++)
			out[p]=i;

	// shuffle 
	gsl_ran_shuffle (rgen, out, size, sizeof (INT));

	return out;
}

/*
 * Cyclically shifts a vector to make it a degree sequence of a tree
 */

int cycshift(INT *D, INT size) {
	INT i, j;
	INT indmin;
	long long sum, min;
	INT *tmp;

	for(i=0, sum=0, min=0, indmin=0; i<size; i++) {	
		sum += (long long) D[i] - 1;
		if(sum < min) {
			min = sum;
			indmin = i;
		}
	}

	if(indmin < size-1) {
		tmp = (INT *) calloc(size, sizeof(INT));
		if(tmp == NULL) {
			// memory allocation error
			fprintf(stderr, "Memory allocation error in function cycshift\n");
			exit(-1);
		}	
		
		// save reordered list in temporary array
		for(i=indmin+1,j=0; i<size; i++,j++)
			tmp[j] = D[i];

		for(i=0,j=size-indmin-1; i<indmin+1; i++,j++)	// initialize j only to avoid compiler warning
		//for(i=0; i<indmin+1; i++,j++)
			tmp[j] = D[i];

		// copy back
		for(i=0; i<size; i++)
			D[i] = tmp[i];

		// free memory
		free(tmp);
	}

	return 0;
}
				

