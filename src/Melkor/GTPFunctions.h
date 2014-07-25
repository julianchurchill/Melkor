#ifndef GTPFUNCTIONS_H
#define GTPFUNCTIONS_H

#include "SGFReader.h"
// for gtp_printf
#include <stdarg.h>
#include <string>

#define NUM_GTP_COMMANDS 45
#define GTP_BUFSIZE 1000
#define GTP_SUCCESS 0
#define GTP_FAILURE 1
#define GTP_OK 0
#define GTP_FATAL 1
#define GTP_QUIT -1

class GTPFunctions
{
	private:
		typedef int GTP_FN(char *s, int id);

		bool programRunning;
		int id;

		int gtp_boardsize;
		const void* melkor;
		// so we can automagically pass  when they do
		// since we don't know when to pass yet
		bool theyPassed;

		string lastOutput;

		const static char* gtpCommandList[];
	
	public:
	
		Move lastMove;

		string getLastOutput() { return lastOutput; };

		// this function should be run at the beginning of _every_ command function
		void gtp_pre_command();

		GTPFunctions(const void* m, bool useMelkorType);
		virtual ~GTPFunctions() {};

		GTP_FN gtp_all_legal;
		GTP_FN gtp_backsgf;
		GTP_FN gtp_capture_string;
		GTP_FN gtp_clear_board;
		GTP_FN gtp_clear_search_history;
		GTP_FN gtp_cputime;
		GTP_FN gtp_estimate_score;
		GTP_FN gtp_eval;
		GTP_FN gtp_final_score;
		GTP_FN gtp_final_status_list;
		GTP_FN gtp_fixed_handicap;
		GTP_FN gtp_genmove;
		GTP_FN gtp_genmove_black;
		GTP_FN gtp_genmove_white;
		GTP_FN gtp_get_random_seed;
		GTP_FN gtp_help;
		GTP_FN gtp_influence_map;
		GTP_FN gtp_is_legal;
		GTP_FN gtp_kgs_genmove_cleanup;
		GTP_FN gtp_known_command;
		GTP_FN gtp_liberty_influence_map;
		GTP_FN gtp_list_big_moves;
		GTP_FN gtp_list_commands;
		GTP_FN gtp_list_strings_status;
		GTP_FN gtp_loadsgf;
		GTP_FN gtp_loadnetwork;
		GTP_FN gtp_menu;
		GTP_FN gtp_mtdf_search;
		GTP_FN gtp_name;
		GTP_FN gtp_network_suggest_black;
		GTP_FN gtp_network_suggest_white;
		GTP_FN gtp_nextsgf;
		GTP_FN gtp_place_free_handicap;
		GTP_FN gtp_play;
		GTP_FN gtp_playblack;
		GTP_FN gtp_playempty;
		GTP_FN gtp_playwhite;
		GTP_FN gtp_print_search_settings;
		GTP_FN gtp_protocol_version;
		GTP_FN gtp_quit;
		GTP_FN gtp_reg_genmove;
		GTP_FN gtp_reset;
		GTP_FN gtp_resetMTDF;
		GTP_FN gtp_save_sgf;
		GTP_FN gtp_score_point;
		GTP_FN gtp_score_all_points_and_print_board;
		GTP_FN gtp_score_all_points;
		GTP_FN gtp_score_all_points_and_illegal;
		GTP_FN gtp_set_boardsize;
		GTP_FN gtp_set_black_name;
		GTP_FN gtp_set_goal;
		GTP_FN gtp_set_free_handicap;
		GTP_FN gtp_set_komi;
		GTP_FN gtp_set_pass_value;
		GTP_FN gtp_set_play_timed_game;
		GTP_FN gtp_set_pruning_settings;
		GTP_FN gtp_set_search_depth;
		GTP_FN gtp_set_time_per_move;
		GTP_FN gtp_set_useNChildren;
		GTP_FN gtp_set_white_name;
		GTP_FN gtp_showboard;
		GTP_FN gtp_string_data;
		GTP_FN gtp_suggest_black;
		GTP_FN gtp_suggest_white;
		GTP_FN gtp_time_left;
		GTP_FN gtp_time_settings;
		GTP_FN gtp_undo;
		GTP_FN gtp_update_strings_status;
		GTP_FN gtp_version;
	
		// helper functions
		int gtp_internal_score_all_points(char*s, int id, bool printBoard, bool onlyLegalMoves);
		int gtp_sgfstep(char *s, int id, int jump, SGFNode* nextNode);
		void gtp_internal_set_boardsize(int b) { gtp_boardsize = b; };
		void play_gtp(string gtpScriptFilename);
	
		// the main loop
		void gtp_main_loop(const char* filename = NULL);

		// taken straight from gtp.c (gnugo 3.0.0)
		void __cdecl gtp_printf(const char *format, ...);
		void gtp_printid(int id, int status);
		int gtp_finish_response(void);
		int __cdecl gtp_success(int id, const char *format, ...);
		int __cdecl gtp_failure(int id, const char *format, ...);
		void gtp_panic();
		int gtp_decode_colour(char *s, int *color);
		int gtp_decode_coord(char *s, int *j, int *i, int gtp_boardsize);
		int gtp_decode_move(char *s, int *j, int *i, int *color, int gtp_boardsize);
		void sort_moves(int n, int movei[], int movej[]);
		void gtp_print_vertices(int n, int movei[], int movej[], int gtp_boardsize);
		void gtp_internal_print_vertex(int j, int i, string& res, int gtp_boardsize);
		void gtp_print_vertex(int j, int i, int gtp_boardsize);
};

#endif
