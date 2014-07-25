#include "globals.h"
#include "Main.h"
#include "Misc.h"
//#include "MelkorDLL.h"
#include <ctime>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#ifdef _DEBUG
//#include "mpatrol.h"

#ifdef USE_CRT_MEMORY_CHECKING
// CRT debug macros
#define SET_CRT_DEBUG_FIELD(a)   _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define CLEAR_CRT_DEBUG_FIELD(a)   _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define CLEAR_CRT_DEBUG_FIELD(a)   ((void) 0)
#endif

#endif // _DEBUG

static struct option const long_options[] = 
{
	{"-debugmtdf",					NO_ARGUMENT,		OPT_DEBUGMTDF},
	{"-debuggo",					NO_ARGUMENT,		OPT_DEBUGGO},
	{"-disable-gametreesearch",		NO_ARGUMENT,		OPT_DISABLEGAMETREESEARCH},
	{"-useSuperKo",					NO_ARGUMENT,		OPT_USESUPERKO},
	{"-disable-superKo",			NO_ARGUMENT,		OPT_DISABLESUPERKO},
	{"-setupMTDFSearch",			REQUIRED_ARGUMENT,	OPT_SETUPMTDFSEARCH},
	{"-mode",						REQUIRED_ARGUMENT,	OPT_MODE},
	{"-network",					REQUIRED_ARGUMENT,	OPT_NETWORK},
	{"-colour",						REQUIRED_ARGUMENT,	OPT_COLOUR},
	{"-quiet",						NO_ARGUMENT,		OPT_QUIET},
	{"-help",						NO_ARGUMENT,		OPT_HELP},
	{"-verbose",					NO_ARGUMENT,		OPT_VERBOSE},
	{"-cheat",						NO_ARGUMENT,		OPT_CHEAT},
	{"-dontFillOwnEyes",			NO_ARGUMENT,		OPT_DONTFILLOWNEYES},
	{"-seed",						REQUIRED_ARGUMENT,	OPT_SEED},
	{"-logfile",					REQUIRED_ARGUMENT,	OPT_LOGFILE},
	{"-gtpScript",					REQUIRED_ARGUMENT,	OPT_GTPSCRIPT},
	{"-useGlobalMoveFunction",		NO_ARGUMENT,		OPT_USEGLOBALMOVEFUNCTION},
	{NULL,							0,					0}
};

int __cdecl main(int argc, char* argv[])
{
#ifdef _DEBUG

#ifdef USE_MEMORY_TRACKING 
#ifdef USE_CRT_MEMORY_CHECKING
#error Don't use _crt memory checking and our memory tracking at the same time!
#endif
#endif

#ifdef USE_CRT_MEMORY_CHECKING
	// this macro makes the _CrtDumpMemoryLeaks() get called automatically when the program exits
	SET_CRT_DEBUG_FIELD( _CRTDBG_LEAK_CHECK_DF );
	// This flag calls _CrtCheckMemory() at _every_ allocation and deallocation
//	SET_CRT_DEBUG_FIELD( _CRTDBG_CHECK_ALWAYS_DF );
	// This flag keeps freed memory blocks in the heap linked list, assigning them _FREE_BLOCK_ type
	// and filling them with bytes of 0xDD
//	SET_CRT_DEBUG_FIELD( _CRTDBG_DELAY_FREE_MEM_DF );
	// This flag includes _CRT_BLOCK_ memory block types in the leak detection and memory state comparisons
//	SET_CRT_DEBUG_FIELD( _CRTDBG_CHECK_CRT_DF );
//	_CrtSetBreakAlloc(16981);
#endif

#ifdef USE_MEMORY_TRACKING
	// uncomment these lines to use our own memory leak tracking routines
	atexit(DumpUnfreed);
#endif
#endif

	LogWriter log;

	int percent = 0;
	float p = 0.0f;
	string network = "";
	string gsSetupString = "";
	string mtdfSetupString = "";
	string temp;
	// command line options
	bool useSuperKo = Melkor::defaultUseSuperKo;
	bool useGlobalMoveFunction = Melkor::defaultUseGlobalMoveFunction;
	bool useGameTreeSearch = Melkor::defaultUseGameTreeSearch;
	bool useGTP = Melkor::defaultUseGTP;
	int computerColour = Melkor::defaultComputerColour;
	bool debuggo = Melkor::defaultDebuggo;
	bool verbose = Melkor::defaultVerbose;
	bool cheat = Melkor::defaultCheat;
	bool debugmtdf = false;

	bool dontFillOwnEyes = Go::defaultDontFillOwnEyes;

	bool testMode = false;

	string gtpScriptFilename("");

	// argv[0] is program name
	for(int i=1;i<argc;i++)
	{
		switch(getOption(argv, i))
		{
			case OPT_DEBUGGO:
				debuggo = true;
				break;
			case OPT_DEBUGMTDF:
				debugmtdf = true;
#ifdef MTDFSEARCH_LEAN_AND_MEAN
				log.println("Warning: -debugmtdf option will have no effect because this exe has been compiled");
				log.println("         with #define MTDFSEARCH_LEAN_AND_MEAN!");
#endif // MTDFSEARCH_LEAN_AND_MEAN
				break;
			case OPT_VERBOSE:
				verbose = true;
				break;
			case OPT_CHEAT:
				cheat = true;
				break;
			case OPT_SEED:
				long tempSeed;
				if(sscanf(argv[i+1], "%u", &tempSeed)==1)
				{
					if(tempSeed<0)
					{
						log.println("-seed argument requires a positive value or 0 to use the current time");
						log.println("");
						PrintOptions(log);
						return 0;
					}
					random_seed() = tempSeed;
					i++;
				}
				break;
			case OPT_DISABLEGAMETREESEARCH:
				useGameTreeSearch = false;
				break;
			case OPT_USESUPERKO:
				useSuperKo = true;
				break;
			case OPT_DISABLESUPERKO:
				useSuperKo = false;
				break;
			case OPT_SETUPMTDFSEARCH:
				mtdfSetupString = argv[i+1];
				i++;
				break;
			case OPT_MODE:
				if(!strcmp(argv[i+1], "gtp")) {
					useGTP = true;
					i++;
				}
				else if(!strcmp(argv[i+1], "unit_test")) {
					testMode = true;
					i++;
					break;
				}
				else {
					log.print("Unknown mode: ");
					log.println(argv[i+1]);
					log.println("");
					PrintOptions(log);
					return 0;
				}
				break;
			case OPT_COLOUR:
				if(!strcmp(argv[i+1], "b"))
				{
					computerColour = BLACK;
					i++;
				}
				else if(!strcmp(argv[i+1], "w"))
				{
					computerColour = WHITE;
					i++;
				}
				else
				{
					log.print("Unknown colour: ");
					log.println(argv[i+1]);
					log.println("");
					PrintOptions(log);
					return 0;
				}
				break;
			case OPT_NETWORK:
				network = argv[i+1];
				i++;
				break;
			case OPT_QUIET:
				LogWriter::forceStderr = true;
				break;
			case OPT_HELP:
				PrintOptions(log);
				return 0;
			case OPT_LOGFILE:
				LogWriter::startLogFile(string(argv[i+1]));
				i++;
				break;
			case OPT_DONTFILLOWNEYES:
				dontFillOwnEyes = true;
				break;
			case OPT_GTPSCRIPT:
				// Read in the filename
				gtpScriptFilename = argv[i+1];
				i++;
				break;
			case OPT_USEGLOBALMOVEFUNCTION:
				useGlobalMoveFunction = true;
				break;
			case OPT_UNKNOWN:
			default :
				// unrecognised option
				log.print("Unrecognised or incomplete option: ");
				log.println(argv[i]);
				log.println("");
				PrintOptions(log);
				return 0;
		}
	}

	Melkor melkor(random_seed(), &network, debugmtdf);

	string message;

	if(melkor.error)
	{
		message = "Error in Melkor object initialisation:\n";
		message+="Aborting...\n";
		LogWriter::printerr(message, "");
		exit(0);
	}

	char buffer[50];
	sprintf(buffer, "Seed = %d", random_seed());
	message = buffer;
	log.println(message);

	melkor.setUseSuperKo(useSuperKo);
	melkor.useGlobalMoveFunction = useGlobalMoveFunction;
	melkor.useGameTreeSearch = useGameTreeSearch;
	melkor.useGTP = useGTP;
	melkor.computerColour = computerColour;
	melkor.debuggo = debuggo;
	melkor.verbose = verbose;
	melkor.cheat = cheat;

	Go::dontFillOwnEyes = dontFillOwnEyes;

	// mtdf setup string overrides all individually specified parameters
	if(!melkor.mtdfSearch->setupMTDFSearch(&mtdfSetupString,melkor.getBoard())) {
		LogWriter::printerr("setupMTDFSearch string invalid...\nAborting...\n");
		exit(0);
	}

	// Test the DLL functions
//	DllSetBoardSize(19);
//	DllPlayMove(0, 0, BLACK);

	// handle GTP connection
	if(melkor.useGTP)
		melkor.playGTP(gtpScriptFilename);
	else if(testMode) {
		CppUnit::TextUi::TestRunner runner;
		runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
		runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );
		return runner.run() ? 0 : 1;
	}
	else 
		while(melkor.DoMainMenu(log));

	return 0;
}

int getOption(char* argv[], int i)
{
	// if argv[i] has unix style prefix, i.e. '--'
	// strip the first '-'
	int len = strlen(argv[i]);
	if(len>2 && argv[i][0]=='-' && argv[i][1]=='-') {
		string str(argv[i]);
		// get subtr from argv[i][1] to end
		sprintf(argv[i], "%s", str.substr(1).c_str());
	}

	// find the matching option and check
	// it has arguments if required
	int p=0;
	while(long_options[p].name!=NULL) {
		if(!strcmp(long_options[p].name, argv[i])) {
			// check required arguments are supplied
			if(long_options[p].has_arg==REQUIRED_ARGUMENT) {
				// if no arguments
				// return fail
				if(argv[i+1]==NULL)
					return OPT_UNKNOWN;
				return long_options[p].val;
			}
			else
				return long_options[p].val;
		}
		p++;
	}
	return OPT_UNKNOWN;
}

void PrintOptions(LogWriter& log)
{
	string title = " Melkor";
	title+=Melkor::getVersionString();
	log.println(title);
	log.println("----------------------");
	log.println("");
	log.println("Usage: melkor.exe");
	log.println("       [ -mode {gtp|unit_test} ]");
	log.println("       [ -network <filename> ]");
	log.println("       [ -colour {b|w} ]");
	log.println("       [ -dontFillOwnEyes ]");
	log.println("       [ -quiet ]");
	log.println("       [ -cheat ]");
	log.println("       [ -seed <seedValue> ]");
	log.println("       [ -help ]");
	log.println("\n  Setup options:\n");
	log.println("       [ -useGlobalMoveFunction (default) ]");
	log.println("       [ -disable-gametreesearch ]");
	log.println("       [ -useSuperKo | -disable-superKo ]");
	log.println("\n    Search specific options:\n");
	log.println("       [ -setupMTDFSearch { \"[time=n] [maxDepth=n] [useNChildren=n|n%] [prune=f]");
	log.println("              [goal=A(x,y)] [nextToPlay=[BLACK|WHITE]]\" }]");
	log.println("            where A is a character indicating goal type {C=CaptureGoal, L=LibertyCount,");
	log.println("            S=StoneCount, I=Influence (default)}");
	log.println("            and x,y are arguments to the goal.");
	log.println("");
	log.println("\n  Debugging options:");
	log.println("       [ -logfile <filename> ]");
	log.println("       [ -gtpScript <filename> ]");
	log.println("       [ -debuggo ]");
	log.println("       [ -debugmtdf ]");
	log.println("       [ -verbose ]");
	log.println("");
	log.println(" NOTE: -setupMTDFSearch will override any other command line search parameters");
	log.println("");
}