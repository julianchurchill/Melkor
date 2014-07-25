#ifndef GLOBALS_H
#define GLOBALS_H

/** @file
 *  This file contains many defines and enums used throughout the %Morgoth project.
 */

#ifdef __AFX_H__
// Warning: Don't use our own memory routines and MFC memory leak handling at the same time!
/** Define the MFC debugging version of new if we're using MFC. */
#define new DEBUG_NEW
#else // __AFX_H__

#ifdef _DEBUG

// define one of these for memory tracking and output 
// when the program terminates
//#define USE_MEMORY_TRACKING
#define USE_CRT_MEMORY_CHECKING

#ifdef USE_MEMORY_TRACKING
// these headers need to be included before our memoryTracking
// new and delete replacements, so that compiler errors aren't generated
// when <new> and <memory> are included later (they contain definitions of new and delete
// which would be corrupted by the #define new DEBUG_NEW macro)
#include <new>
#include <memory>
#include <fstream>
#include "memoryTracking.h"
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#endif // USE_MEMORY_TRACKING

#ifdef USE_CRT_MEMORY_CHECKING
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // USE_CRT_MEMORY_CHECKING

#endif // _DEBUG

#endif // __AFX_H__

/** @todo Define the line below to highlight where we are with the design and coding
  *  of the move suggester feature. */
// Experimental
//#define USE_MOVE_SUGGESTERS

/** Key type used to store nodes and boardstructs in hash tables etc... */
#define BOARDSTRUCT_KEY_TYPE int

/** Define the transposition table data structure used by MTDFSearch. */
#define TRANSPOSITION_TABLE hash_set<Node>

/** Define this for Test::MTDFSearchTest() to print an english readable summary. */
#define PRINT_TEST_SUMMARY
/** Define to identify current running hardware, used by Test::MTDFSearchTest(). */
#define TEST_SUMMARY_HARDWARE_STR "AMD Athlon 1.67Ghz, 768Mb RAM, Windows 2000"

/** @todo Document the rest of this file... */

// The 2 defines below are currently unused.
#define RECTANGULAR_BOUNDARY
#define POLYGON_BOUNDARY

// define this to show up all the #error messages
// associated with our implementation of radial basis function
// neural networks
//#define RBF_ERRORS

// see CNBPNTrainer::trainPattern() and CN::getAnswer()
#define USE_CN_OUTPUT_SIGMOID_ACTIVATION_FUNCTION

// This is for CNBPNTrainer class
// if defined then training the hidden nodes
// will only adapt the winning node
//#define NO_NEIGHBOURHOOD
// This is for CNBPNTrainer class
// and makes the neighbourhood size reduce
// as we train on more patterns
#define LINEARLY_DEGRADING_NEIGHBOURHOOD_SIZE
// For CNBPNTrainer class, gives reducing learning rate
// during hidden node competitive training
#define LINEARLY_DEGRADING_LEARNING_RATE

// undefine this for some probable speed up
// see Misc::isLegal()...
#define INCLUDE_TIMING_CODE

// Used by Trainnet.exe to determine BPNTrainerInterface
// class to use
enum TRAINERTYPE {
	BPNTRAINER,
	CNTRAINER,
	RPROPTRAINER,
	TDTRAINER
};

// Used by the BPN classes to select input field shape
enum INPUTFIELDSHAPE {
	IFS_SQUARE,
	IFS_DIAMOND
};

// neural network weight initialisation defaults
#define WEIGHTMIN -0.3f
#define WEIGHTMAX  0.3f

#define OFFBOARD					-1
#define EMPTY						0
#define WHITE						1
#define BLACK						2
#define GOOD_BLACK_MOVE				3
#define GOOD_WHITE_MOVE				4
#define BAD_BLACK_MOVE				5
#define BAD_WHITE_MOVE				6
#define GOOD_BLACK_MOVE_AND_TOP_TEN	7
#define GOOD_WHITE_MOVE_AND_TOP_TEN	8

// life and death status defines
// note: Alive must be positive and DEAD negative for MTD(f) to work properly
// value of URGENT does not currently matter as long as it is different to the others
#define URGENT					1002
#define UNCONDITIONALLY_ALIVE		1001
// ALIVE must be guaranteed to be greater than the maximum possible liberties
// of a string on a 19x19 board
#define ALIVE						1000
// We consider a ladder to have safely escaped if it achieves 3 or more liberties
#define LADDER_ESCAPED              3
#define UNKNOWN						0
#define DEAD						-1
#define UNCONDITIONALLY_DEAD		-2
// NOSTATUS must be the lowest status score
#define NOSTATUS                    -1000

// some defines for user modification to a board
// within a GUI for instance NeuralGo
#define BLACKMOVE					10
#define WHITEMOVE					11
#define EMPTYSTONE					12
#define BLACKSTONE					13
#define WHITESTONE					14

// SGF markers
#define SGF_TRIANGLE				15
#define SGF_CROSS					16
#define SGF_CIRCLE					17
#define SGF_ARROW					18
#define SGF_LINE					19
#define SGF_SELECTED				20
#define SGF_SQUARE					21

#define READ						0
#define WRITE						1
#define WRITESINGLE					2

#define LIBERTYCOUNTEVAL			0
#define STONECOUNTEVAL				1
#define EVALNETWORKEVAL				2
#define MOVEFINDEREVAL				3
#define CAPTURESTRINGEVAL			4
#define INFLUENCEEVAL				5
//#define MAKELIFEEVAL				6

/*
// NOTE: These are old, we use Morgoth
// as a thread object now, instead of
// GTPFunctions, see MorgothGUI project
#define GTP_THREAD_SUCCESS			1
#define GTP_THREAD_FAILURE			2
// for thread messages to GTP objects
// so they know what we want them to do
// Set GtpGameComms->gtpRequest to one of
// these values
#define GTP_END_THREAD				1
#define GTP_RUN						2
#define GTP_GETNAME					3
#define GTP_SETBOARDSIZE			4
#define GTP_LOADSGF					5
#define GTP_NEXTSGF					6
#define GTP_PREVIOUSSGF				7
#define GTP_GTPQUIT					8
#define GTP_HELP					9
#define GTP_SUGGESTBLACKMOVE		10
#define GTP_SUGGESTWHITEMOVE		11
#define GTP_SCOREALLPOINTS			12
#define GTP_LOADNETWORK				13
#define GTP_QUITPROGRAM				14
#define GTP_GENMOVEBLACK			15
#define GTP_GENMOVEWHITE			16
#define GTP_SETID					17
#define GTP_UNDO					18
#define GTP_PLAYBLACK				19
#define GTP_PLAYWHITE				20
#define GTP_SETKOMI					21
#define GTP_SETHANDICAP				22
*/

#define MORGOTH_THREAD_SUCCESS		1
#define MORGOTH_THREAD_FAILURE		2

#define MORGOTH_END_THREAD			1
#define MORGOTH_GETNAME				3
#define MORGOTH_SETBOARDSIZE		4
#define MORGOTH_LOADSGF				5
#define MORGOTH_NEXTSGF				6
#define MORGOTH_PREVIOUSSGF			7
#define MORGOTH_GTPQUIT				8
#define MORGOTH_HELP				9
#define MORGOTH_SUGGESTBLACKMOVE	10
#define MORGOTH_SUGGESTWHITEMOVE	11
#define MORGOTH_SCOREALLPOINTS		12
#define MORGOTH_LOADNETWORK			13
#define MORGOTH_QUITPROGRAM			14
#define MORGOTH_GENMOVEBLACK		15
#define MORGOTH_GENMOVEWHITE		16
#define MORGOTH_SETID				17
#define MORGOTH_UNDO				18
#define MORGOTH_PLAYBLACK			19
#define MORGOTH_PLAYWHITE			20
#define MORGOTH_SETKOMI				21
#define MORGOTH_SETHANDICAP			22


#define BEGIN_NSEW_DIR				0
#define END_NSEW_DIR				3
#define NORTH						0
#define SOUTH						1
#define EAST						2
#define WEST						3

#define MAX_BOARD_SIZE				19

#endif