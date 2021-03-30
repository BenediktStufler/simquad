const char *argp_program_version =
"simquad 1.0";

const char *argp_program_bug_address =
"<benedikt.stufler@posteo.net>";


/* This structure is used by main to communicate with parse_opt. */
struct cmdarg
{
	int threads;			// number of threads we are going to launch
	unsigned long int seed;		// seed for the random generators
	INT size;			// target size of random quadrangulation 

	const gsl_rng_type *randgen;	// type of random generator

	unsigned int num;		// number of samples
	int Tnum;			// has value been set by the user?
};



/*
 * Functions that provide default values for parameters
 */


/*
 * seeding procedure Robert G. Brown
 * https://sourceware.org/legacy-ml/gsl-discuss/2004-q1/msg00071.html
 */
unsigned long int getseed(void) {
	FILE *devr;
	unsigned long int seed;
 	struct timeval tv;
 
	if ((devr = fopen("/dev/random","r")) == NULL) {
 		gettimeofday(&tv,0);
 		seed = tv.tv_sec + tv.tv_usec;
	} else {
   		fread(&seed,sizeof(seed),1,devr);
   		fclose(devr);
	}

	return seed;
}


/*
 * use number of cpu cores as default value for number of threads
 * adapted from code by Philip Willoughby
 * https://lists.gnu.org/archive/html/autoconf/2002-08/msg00126.html
 * 
 */
unsigned int getnumcores(void) {
	int nprocs_max = -1;
	
	#ifdef _WIN32
		#ifndef _SC_NPROCESSORS_ONLN
			SYSTEM_INFO info;
			GetSystemInfo(&info);
			#define sysconf(a) info.dwNumberOfProcessors
			#define _SC_NPROCESSORS_ONLN
		#endif
	#endif

	#ifdef _SC_NPROCESSORS_ONLN
		nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
		if (nprocs_max < 1) {
			fprintf(stderr, "Could not determine number of CPUs configured:\n%s\n. Falling back to default value 1.\n", strerror (errno));
			return 1;
		}
		return nprocs_max;
	#else
		fprintf(stderr, "Could not determine number of CPUs. Falling back to default value 1.\n");
  		return 1;
	#endif
}


/*
	 OPTIONS.   Field 1 in ARGP.
	 Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
	{"size",		's', "SIZE", 0, 	"Generate a uniform planted simple quadrangulation with s >= 6 faces,  s + 2 vertices, and 2s edges. Has to be greater than or equal to six."},
	{"num", 		'N', "NUM", 0, 		"Simulate NUM many samples."},
	{"threads", 	't', "THREADS", 0,	"Distribute the workload on THREADS many threads. The default value is the number of CPU cores."}, 
	{"randgen",  	'r', "RANDGEN", 0, 	"Use the pseudo random generator RANDGEN. Available options are taus2, gfsr4, mt19937, ranlux, ranlxs0, ranlxs1, ranlxs2, ranlxd1, ranlxd2, mrg, cmrg, ranlux389. The default is taus2."},
	{"seed", 		'S', "SEED", 0, "Specify the seed of the random generator in the first thread. Thread number k will receive SEED + k - 1 as seed. The default is to set SEED to the systems timestamp (in seconds)."},
	{0}
};


/*
	 PARSER. Field 2 in ARGP.
	 Order of parameters: KEY, ARG, STATE.
*/
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct cmdarg *arguments = state->input;
	const char *strgens[] = {"taus2", "gfsr4", "mt19937", "ranlux", "ranlxs0", "ranlxs1", "ranlxs2", "ranlxd1", "ranlxd2", "mrg", "cmrg", "ranlux389"};
	int num;

	switch (key) {
		case 't':
			// the number of threads we are going to launch
			arguments->threads = (int) strtoimax(arg, NULL, 10);
			if(arguments->threads <= 0) {
				fprintf(stderr, "Error: the number of threads must be positive.\n");
				exit(-1);
			}
			break;
		case 's':
			// size parameter for random tree
			arguments->size = (INT) strtoimax(arg, NULL, 10);
			if(arguments->size < 6) {
				fprintf(stderr, "Error: the SIZE command line argument must be at least six.\n");
				exit(-1);
			}
			break;
		case 'S':
			// seed for random generator
			arguments->seed = (unsigned int) strtoimax(arg, NULL, 10);
			break;
		case 'r':
			// select random generator
			for(num=0; num<12; num++) {
				if( strcmp(strgens[num],arg) == 0) {
					if(num==0) {
						arguments->randgen = gsl_rng_taus2;
						break;
					} else if (num==1) {
						arguments->randgen = gsl_rng_gfsr4;
						break;
					} else if (num==2) {
						arguments->randgen = gsl_rng_mt19937;
						break;
					} else if (num==3) {
						arguments->randgen = gsl_rng_ranlux;
						break;
					} else if (num==4) {
						arguments->randgen = gsl_rng_ranlxs0;
						break;
					} else if (num==5) {
						arguments->randgen = gsl_rng_ranlxs1;
						break;
					} else if (num==6) {
						arguments->randgen = gsl_rng_ranlxs2;
						break;
					} else if (num==7) {
						arguments->randgen = gsl_rng_ranlxd1;
						break;
					} else if (num==8) {
						arguments->randgen = gsl_rng_ranlxd2;
						break;
					} else if (num==9) {
						arguments->randgen = gsl_rng_mrg;
						break;
					} else if (num==10) {
						arguments->randgen = gsl_rng_cmrg;
						break;
					} else if (num==11) {
						arguments->randgen = gsl_rng_ranlux389;
						break;
					}
				}
			}
			if(num>11) {
				fprintf(stderr, "Error: Invalid value for random number generator.\n");
				exit(-1);
			}

			break;
		case 'N':
			// the number of samples
			arguments->num = (unsigned int) strtoimax(arg, NULL, 10);
			arguments->Tnum = 1;
			if( arguments->num <= 0 ) {
				fprintf(stderr, "Error: The --num parameter has to be a positive integer.\n");
				exit(-1);
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}


/*
	 ARGS_DOC. Field 3 in ARGP.
	 A description of the non-option command-line arguments
		 that we accept.
*/
static char args_doc[] = "";

/*
	DOC.	Field 4 in ARGP.
	Program documentation.
*/
static char doc[] = "simquad -- generate uniform simple quadrangulations";

/*
	 The ARGP structure itself.
*/
static struct argp argp = {options, parse_opt, args_doc, doc};



int getcmdargs(struct cmdarg *comarg, int argc, char **argv) {
	/* set command line arguments defaults */

	comarg->size = 20;
	comarg->num = 1;
	comarg->Tnum = 0;

	// the default random generator is the taus2 algorithm
	comarg->randgen = gsl_rng_taus2;

	// default value for seed of random generator
	comarg->seed = getseed();

	// default number of threads is the number of cpu cores
	comarg->threads = getnumcores();

	/* read command line arguments and perform some sanity checks*/
	argp_parse (&argp, argc, argv, 0, 0, comarg);

	return 0;
}

