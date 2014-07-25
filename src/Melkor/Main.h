#ifndef MAIN_H
#define MAIN_H

#include "Melkor.h"

// all possible options
enum {
	OPT_UNKNOWN=1,
	OPT_DEBUGMTDF,
	OPT_DEBUGGO,
	OPT_VERBOSE,
	OPT_DISABLEGAMETREESEARCH,
	OPT_USESUPERKO,
	OPT_DISABLESUPERKO,
	OPT_SETUPMTDFSEARCH,
	OPT_MODE,
	OPT_NETWORK,
	OPT_COLOUR,
	OPT_QUIET,
	OPT_CHEAT,
	OPT_SEED,
	OPT_LOGFILE,
	OPT_DONTFILLOWNEYES,
	OPT_GTPSCRIPT,
	OPT_USEGLOBALMOVEFUNCTION,
	OPT_HELP
};

enum {
	NO_ARGUMENT=1,
	REQUIRED_ARGUMENT,
	OPTIONAL_ARGUMENT
};

struct option
{
	const char *name;
	int has_arg;
	int val;
};

int getOption(char* argv[], int i);
void PrintOptions(LogWriter& log);

#endif