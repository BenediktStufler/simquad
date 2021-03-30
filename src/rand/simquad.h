// half-edge datastructure
struct hedge {
	struct hedge *ne;	// next half-edge
	struct hedge *pr;	// previous half-edge
	struct hedge *op;	// opposite half-edge
};


struct stack {
        INT val;
        struct stack *pre;
};

void spush(struct stack **st, int val) {
        struct stack *mew;

        // create new box that contains val
        mew = (struct stack *) malloc(sizeof(struct stack));
        if(mew == NULL) {
                fprintf(stderr, "Memory allocation error in function spush.\n");
                exit(-1);
        }
        mew->val = val;

        // place box on stack
        mew->pre = *st;
        *st = mew;
}

int spop(struct stack **st) {
        struct stack *cur;
        int ret;

        cur = *st;
        ret = cur->val;
        *st = cur->pre;

        free(cur);
        return ret;
}





// produces an example half edge structure
// used for debugging and tests
struct hedge *tria_example() {
	struct hedge *ex;
	int i;
	int a,b;

	int ad[20][2] = {{0,1}, {2,37}, {3,4}, {5,6}, {7,36}, {8,13}, {9,10}, 
			{11,12}, {14,15}, {16,33}, {17,22}, {18,19}, {20,21},
			{23,28}, {24,25}, {26,27}, {29,30}, {31,32}, {34,35},
			{38,39}};

	// 42 is not a typo
	ex = (struct hedge *) calloc(42, sizeof(struct hedge));

	// set next and previous relation as if it were a cycle
	ex[0].pr = &ex[39];
	ex[0].ne = &ex[1];
	ex[39].ne = &ex[0];
	ex[39].pr = &ex[38];
	for(i=1; i<39; i++) {
		ex[i].ne = &ex[i+1];
		ex[i].pr = &ex[i-1];
	}

	// set the opposite relation
	for(i=0;i<20;i++) {
		a = ad[i][0];
		b = ad[i][1];
		ex[a].op = &ex[b];
		ex[b].op = &ex[a];
	}

	ex[40].pr = NULL;
	ex[40].ne = NULL;
	ex[40].op = NULL;
	ex[41].pr = NULL;
	ex[41].ne = NULL;
	ex[41].op = NULL;

	return ex;
}


// output half edge structure line by line
// used for debugging and tests
void debug_out_hedge(struct hedge *he, long int num) {
	for(long int i = 0; i<num; i++) {
		printf("%ld - (%ld, %ld, %ld)\n", i, he[i].ne-he, he[i].pr-he, he[i].op-he);
	}
}


// generate half-edge structure out of outdegree list
// has an extra space
struct hedge *hegen(INT *D, INT n) {
	struct hedge *ex;
	struct stack *uqueue;
	struct stack *mqueue;
	INT ptr;
	INT cur;
	INT partner;

	INT i;
	INT wrap;


	ex = (struct hedge *) calloc(4*n, sizeof(struct hedge));

	if(ex == NULL) {
		fprintf(stderr, "Memory allocation error in function hegen.\n");
		exit(-1);
	}

	wrap = 4*n-3;

	// set next and previous relation as if it were a cycle
	ex[0].pr = &ex[wrap];
	ex[0].ne = &ex[1];
	ex[wrap].ne = &ex[0];
	ex[wrap].pr = &ex[wrap-1];
	for(i=1; i<wrap; i++) {
		ex[i].ne = &ex[i+1];
		ex[i].pr = &ex[i-1];
	}



	/* set the opposite relation */
	// initialize queues 
	ptr = 1;
	cur = 0;	
	uqueue = NULL;
	mqueue = NULL;
	for(i=0;i<D[0];i++) {
		spush(&uqueue, 0);
		spush(&uqueue, 1);
	}	



	// iterate
	while(uqueue) {
		if(spop(&uqueue)) {
			// Up - We travel upwards in the contour exploration
			spush(&mqueue, cur);	// add half-edge label to queue
						// of labels that wait to be paired

			//printf("Queued to pairing list: %ld\n", cur); //DEBUG
			for(i=0;i<D[ptr];i++) {
				spush(&uqueue, 0);
				spush(&uqueue, 1);
			}	
			ptr++;

			cur++;
		} else {
			// Down - We travel downwards in the contour exploration
			
			// pair 
			partner = spop(&mqueue);
			//printf("Pair %ld with %ld\n", cur, partner); //DEBUG
			ex[cur].op = &ex[partner];
			ex[partner].op = &ex[cur];

			cur++;
		}
	}

	ex[4*n-2].pr = NULL;
	ex[4*n-2].ne = NULL;
	ex[4*n-2].op = NULL;
	ex[4*n-1].pr = NULL;
	ex[4*n-1].ne = NULL;
	ex[4*n-1].op = NULL;


	return ex;
}




// output half-edge structure
void outhedge(struct hedge *he, long int num) {
	FILE *stream;

	stream = stdout;

	fprintf(stream, "{");
	for(long int i = 0; i<num; i++) {
		fprintf(stream, "{");
		fprintf(stream, "%ld,", 1+(INT) (he[i].ne - he) );
		fprintf(stream, "%ld,", 1+(INT) (he[i].pr - he) );
		fprintf(stream, "%ld",  1+(INT) (he[i].op - he) );
		fprintf(stream, "}");
		if(i<num-1) fprintf(stream, ",");
	}
	fprintf(stream, "}");

}


void closure(struct hedge *ex, struct hedge *x, struct hedge *y, int swi) {
	struct hedge *sta;	
	struct hedge *cur;	
	struct hedge *tmp;	

	struct hedge *c1, *c2, *d1, *d2;

	// perform local closures
	sta = ex;
	cur = ex;
	do {
		if(cur->op == cur->ne) {		// current half-edge points to a leaf
			// the next half-edge always points back to an inner
			// vertex. hence we need to check if the three after that
			// point to inner vertices as well.
			if( !(cur->ne->ne->op == cur->ne->ne->ne || cur->ne->ne->ne->op == cur->ne->ne->ne->ne || cur->ne->ne->ne->ne->op == cur->ne->ne->ne->ne->ne) ) {
				// perform a local closure
				tmp = cur->ne->ne->ne->ne->ne;
				cur->ne->ne->ne->ne->ne = cur->ne;
				cur->ne->pr = cur->ne->ne->ne->ne;
				cur->ne = tmp;
				tmp->pr = cur;

				// update start pointer and current pointer
				sta = cur;
				cur = tmp;
			} else {
				cur = cur->ne;	// walk to next half-edge
			}
		} else {
			cur = cur->ne;	// walk to next half-edge
		}
	} while(cur != sta);
	
	
	// perform final closure
	x->op = y;
	y->op = x;

	// find C
	while( !(cur->pr==cur->op && cur->ne->ne->op==cur->ne->ne->ne) )	
		cur = cur->ne;
	c1 = cur;

	// find D
	cur = cur->ne;	
	while( !(cur->pr==cur->op && cur->ne->ne->op==cur->ne->ne->ne) )	
		cur = cur->ne;
	d1 = cur;

	// toss a fair coin flip on whether to interchange C and D
	if(swi) {
		tmp = c1;
		c1  = d1;
		d1 = tmp;
	}

	c2 = c1->ne->ne;
	d2 = d1->ne->ne;

	//DEBUG
	//printf("c1 = %ld\n", c1 - ex);
	//printf("c2 = %ld\n", c2 - ex);
	//printf("d1 = %ld\n", d1 - ex);
	//printf("d2 = %ld\n", d2 - ex);


	// connect things to A
	cur = c2->ne;
	while( cur != d1 ) {
		tmp = cur->ne->ne->ne->ne;
		cur->pr = tmp->pr;
		tmp->pr->ne = cur;

		cur = tmp;
	}	


	// connect things to B
	cur = d2->ne;
	while( cur != c1 ) {
		tmp = cur->ne->ne->ne->ne;
		cur->pr = tmp->pr;
		tmp->pr->ne = cur;

		cur = tmp;
	}	

	// connect x and y
	y->pr = c2;
	y->ne = c1;
	c2->ne = y;
	c1->pr = y;

	x->pr = d2;
	x->ne = d1;
	d1->pr = x;
	d2->ne = x;
}


// wrapper for closure function
void wclosure(struct hedge *ex, INT n, gsl_rng *rgen) {
	struct hedge *x;
	struct hedge *y;
       	int swi;

	x = &ex[4*n-2];
	y = &ex[4*n-1];

	swi = gsl_rng_uniform_int(rgen, 2);

	closure(ex,x,y, swi); 
}

// add one leaf at each vertex with uniform random locations
INT *enrich(INT *D, INT n, gsl_rng *rgen) {
	INT *out; 
	INT i;
	INT pin;
	INT pout;
	INT a;

	struct stack *uqueue;
	
	
	// result will be stored in an array of integers
        out = (INT *) calloc(2*n, sizeof(INT));
        if(out == NULL) {
                // memory allocation error
                fprintf(stderr, "Memory allocation error in function enrich.\n");
                exit(-1);
        }


	// initialize queues 
	pin = 1;	
	pout = 1;
	uqueue = NULL;

	// select 1 box out of D[0]+1 boxes
	a = gsl_rng_uniform_int(rgen, D[0]+1);
	out[0] = D[0]+1;
	for(i=0;i<D[0]+1;i++) {
		if(i!=a) {
			spush(&uqueue, 1);
		} else {
			spush(&uqueue, 0);
		}
	}	

	// iterate
	while(uqueue) {
		if(spop(&uqueue)) {
			// we add a non-leaf
			
			//add outdegree with 1 extra leaf
			out[pout] = D[pin]+1;

			// choose location of leaf
			a = gsl_rng_uniform_int(rgen, D[pin]+1);

			// queue the leaves and non-leaves
			for(i=0;i<D[pin]+1;i++) {
				if(i!=a) {
					spush(&uqueue, 1);
				} else {
					spush(&uqueue, 0);
				}
			}	
			pin++;
			pout++;
		} else {
			// we add a leaf
			out[pout] = 0;
			pout++;	
		}
	}


	return out;
}

/*
 * Simulate a simple quadrangulation with a given number of quadrangles
 */
int simquad(struct cmdarg *comarg, gsl_rng **rgens) {
	INT *degprofile;		// outdeg profile
	mpfr_t *xi;			// offspring law
	INT *D, *E;			// degree sequence
	INT n;				// size of tree
	struct hedge *map;
	INT i;


	//DEBUG
	//INT test[]={3,0,3,0,0,4,2,0,0,0,4,2,0,0,2,0,0,0,0,0,0};

	/* calculate size of tree */
	/*
	 * v - e + f = 2
	 * 4f = 2e
	 *
	 * Hence: e = 2f, v = f+2
	 *
	 * Also: v = n + 2, so n = f
	 *
	 */
	n = (INT) comarg->size;

	/* select offspring distribution */
	xi = xiquad(n);


	// add loops according to comarg->num
	printf("{");
	for(i=0; i<comarg->num; i++) {	
		if(i>0) printf(",");

		/* simulate balls in boxes model */
		degprofile = tbinb(n, n-1, xi, comarg->threads, rgens);

		/* generate degree sequence */
		D = gendegsequence(degprofile, n, rgens[0]);

		// clean-up degprofile	
		free(degprofile);

		/* cyclically shift sequence */
		cycshift(D, n);


		/* add one leaf at each vertex with uniform random locations */
		E = enrich(D, n, rgens[0]);

		// clean-up degree sequence
		free(D);

		/* convert to half-edge data structure with extra space for closure*/
		map = hegen(E, n);

		free(E);

		/* perform closure */
		wclosure(map, n, rgens[0]);

		//DEBUG
		debug_out_hedge(map, 4*n);
		exit(-1);

		/* output half edge*/
		outhedge(map, 4*n);


		free(map);
	}
	printf("}\n");



	// clean up offspring distribution
	if(xi != NULL) {
		for(i=0; i<n; i++)
			mpfr_clear(xi[i]);
		free(xi);
	}

	return 0;
}
