/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/
#line 14 "cmdl/grammar.y"
#include "../expression/expression.h"
#line 15 "cmdl/grammar.y"
#include "../expression/comparison_expression.h"
#line 16 "cmdl/grammar.y"
#include "../expression/conditional_expression.h"
#line 17 "cmdl/grammar.y"
#include "../expression/exponentiation_expression.h"
#line 18 "cmdl/grammar.y"
#include "../expression/logical_expression.h"
#line 19 "cmdl/grammar.y"
#include "../expression/number_expression.h"
#line 20 "cmdl/grammar.y"
#include "../expression/product_expression.h"
#line 21 "cmdl/grammar.y"
#include "../expression/sum_expression.h"
#line 22 "cmdl/grammar.y"
#include "../expression/variable_expression.h"
#line 23 "cmdl/grammar.y"
#include "../expression/function_expression.h"
#line 25 "cmdl/grammar.y"
#include "symbols.h"
#line 26 "cmdl/grammar.y"
#include "parse_tree.h"
#line 27 "cmdl/grammar.y"
#include "parser.h"
#line 29 "cmdl/grammar.y"
#include  <assert.h>
#line 30 "cmdl/grammar.y"
using namespace expression;
#line 31 "cmdl/grammar.y"
using namespace cmdl;
#line 32 "cmdl/grammar.y"
#undef NDEBUG
#line 34 "cmdl/grammar.y"

	// Forward declaration parser functions.
	void internal_Parse(
		void *yyp,                   /* The parser */
		int yymajor,                 /* The major token code number */
		terminal_symbol* yyminor,       /* The value for the token */
		parse_tree* parse_tree               /* Optional %extra_argument parameter */
	);

	void *internal_ParseAlloc(void* (*mallocProc)(size_t));

	void internal_ParseFree(
		void *p,                    /* The parser to be deleted */
		void(*freeProc)(void*)     /* Function used to reclaim memory */
	);
	void internal_ParseTrace(FILE *TraceFILE, char *zTracePrompt);
	void cmdl::parser::initialize_internal()
	{
		uninitialize_internal();
		if (logFilePath_.empty())
		{
			logFile_ = nullptr;
		}
		else
		{
			fopen_s(&logFile_, logFilePath_.c_str(), "w");
			if(logFile_)
				internal_ParseTrace(logFile_, "cmdl_");
			else
				internal_ParseTrace(0, "cmdl_");
		}
		try
		{
			handle_ = internal_ParseAlloc(malloc);
		}
		catch (...)
		{
			throw std::exception("Could not allocate space for cmdl parser.");
		}
		if (!handle_)
		{
			throw std::exception("Could not create cmdl parser.");
		}
	}
	void cmdl::parser::uninitialize_internal()
	{
		if(!handle_)
			return;
		internal_ParseTrace(0, "cmdl_");
		internal_ParseFree(handle_, free); 
		handle_ = nullptr;
		if (logFile_)
			fclose(logFile_);
		logFile_ = nullptr;
	}

	void cmdl::parser::parse_token(int tokenID, cmdl::terminal_symbol* token, cmdl::parse_tree& parseTree)
	{
		if (!handle_)
		{
			throw std::exception("Parser handle invalid.");
		}
		try
		{
			internal_Parse(handle_, tokenID, token, &parseTree);
		}
		catch (const std::exception& ex)
		{
			throw ex;
		}
		catch (...)
		{
			throw std::exception("Unknown error");
		}
	}
#line 138 "cmdl/grammar.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    internal_ParseTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is internal_ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    internal_ParseARG_SDECL     A static variable declaration for the %extra_argument
**    internal_ParseARG_PDECL     A parameter declaration for the %extra_argument
**    internal_ParseARG_STORE     Code to store %extra_argument into yypParser
**    internal_ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 43
#define YYACTIONTYPE unsigned char
#define internal_ParseTOKENTYPE terminal_symbol*
typedef union {
  int yyinit;
  internal_ParseTOKENTYPE yy0;
  arguments* yy1;
  conjunction_expression* yy16;
  reaction_side* yy44;
  product_expression* yy63;
  reaction_component* yy66;
  disjunction_expression* yy72;
  conditional_expression* yy78;
  expression_base* yy81;
  sum_expression* yy83;
  int yy85;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define internal_ParseARG_SDECL parse_tree* parseTree;
#define internal_ParseARG_PDECL ,parse_tree* parseTree
#define internal_ParseARG_FETCH parse_tree* parseTree = yypParser->parseTree
#define internal_ParseARG_STORE yypParser->parseTree = parseTree
#define YYERRORSYMBOL 27
#define YYERRSYMDT yy85
#define YYNSTATE             87
#define YYNRULE              54
#define YY_MAX_SHIFT         86
#define YY_MIN_SHIFTREDUCE   108
#define YY_MAX_SHIFTREDUCE   161
#define YY_MIN_REDUCE        162
#define YY_MAX_REDUCE        215
#define YY_ERROR_ACTION      216
#define YY_ACCEPT_ACTION     217
#define YY_NO_ACTION         218
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The yy_shift_ofst[S]+X value is out of range, or
**    (2)  yy_lookahead[yy_shift_ofst[S]+X] is not equal to X, or
**    (3)  yy_shift_ofst[S] equal YY_SHIFT_USE_DFLT.
** (Implementation note: YY_SHIFT_USE_DFLT is chosen so that
** YY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (602)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    36,   30,   29,   27,   26,   25,   24,   23,   22,   12,
 /*    10 */    33,   11,   31,   28,   32,   31,   28,  217,    1,   35,
 /*    20 */   145,  154,   36,   30,   29,   27,   26,   25,   24,   23,
 /*    30 */    22,   34,   33,   32,   31,   28,   47,  152,  167,  171,
 /*    40 */   176,  181,  184,   70,  201,   75,  143,  144,   36,   30,
 /*    50 */    29,   27,   26,   25,   24,   23,   22,   34,   33,   32,
 /*    60 */    31,   28,   36,   30,   29,   27,   26,   25,   24,   23,
 /*    70 */    22,   34,   33,   32,   31,   28,   39,   28,  167,  171,
 /*    80 */   176,  181,  184,   71,  201,   78,    6,  142,   36,   30,
 /*    90 */    29,   27,   26,   25,   24,   23,   22,   34,   33,   32,
 /*   100 */    31,   28,   36,   30,   29,   27,   26,   25,   24,   23,
 /*   110 */    22,   34,   33,   32,   31,   28,  109,  164,   38,  164,
 /*   120 */   164,   35,  164,    5,   36,   30,   29,   27,   26,   25,
 /*   130 */    24,   23,   22,   34,   33,   32,   31,   28,    4,  164,
 /*   140 */   164,  164,  111,  164,  164,    3,   30,   29,   27,   26,
 /*   150 */    25,   24,   23,   22,   34,   33,   32,   31,   28,   36,
 /*   160 */    30,   29,   27,   26,   25,   24,   23,   22,   12,   33,
 /*   170 */    11,   31,   28,   36,   30,   29,   27,   26,   25,   24,
 /*   180 */    23,   22,   34,   33,   32,   31,   28,   36,   30,   29,
 /*   190 */    27,   26,   25,   24,   23,   22,  164,   33,   11,   31,
 /*   200 */    28,  211,  164,  164,  211,  211,  211,   47,  164,  167,
 /*   210 */   171,  176,  181,  184,   73,  201,   29,   27,   26,   25,
 /*   220 */    24,   23,   22,   34,   33,   32,   31,   28,   27,   26,
 /*   230 */    25,   24,   23,   22,   34,   33,   32,   31,   28,  216,
 /*   240 */   216,  216,  216,  216,  216,   34,   33,   32,   31,   28,
 /*   250 */    47,    5,  167,  171,  176,  181,  184,   72,  201,  203,
 /*   260 */   210,   32,   31,   28,    5,  155,  164,  164,  164,  203,
 /*   270 */   203,  164,   16,  203,  203,    7,   17,   65,  110,   37,
 /*   280 */   164,  164,  164,   51,    9,  167,  171,  176,  181,  184,
 /*   290 */     5,  202,   48,   74,  167,  171,  176,  181,  184,  205,
 /*   300 */     2,  164,  164,  164,  164,  164,  164,    6,  207,  205,
 /*   310 */   205,  164,   77,  205,  205,  164,    6,  164,  207,  207,
 /*   320 */    16,   76,  207,  207,   17,   63,  110,   37,  164,   16,
 /*   330 */   164,  164,    9,   17,   86,  110,   37,  164,  164,  164,
 /*   340 */   164,   10,   41,  164,  167,  171,  176,  181,  184,   16,
 /*   350 */   164,  164,  164,   17,   86,  110,   37,  164,  164,  164,
 /*   360 */    43,   13,  167,  171,  176,  181,  184,   46,  164,  167,
 /*   370 */   171,  176,  181,  184,  164,   40,  164,  167,  171,  176,
 /*   380 */   181,  184,   85,  164,  167,  171,  176,  181,  184,   62,
 /*   390 */   164,  167,  171,  176,  181,  184,   42,  164,  167,  171,
 /*   400 */   176,  181,  184,   66,  164,  167,  171,  176,  181,  184,
 /*   410 */   164,   79,  164,  167,  171,  176,  181,  184,   80,  164,
 /*   420 */   167,  171,  176,  181,  184,   81,  164,  167,  171,  176,
 /*   430 */   181,  184,   54,  164,  167,  171,  176,  181,  184,   52,
 /*   440 */   164,  167,  171,  176,  181,  184,  164,   82,  164,  167,
 /*   450 */   171,  176,  181,  184,   67,  164,  167,  171,  176,  181,
 /*   460 */   184,   55,  164,  167,  171,  176,  181,  184,   56,  164,
 /*   470 */   167,  171,  176,  181,  184,   57,  164,  167,  171,  176,
 /*   480 */   181,  184,  164,   58,  164,  167,  171,  176,  181,  184,
 /*   490 */    59,  164,  167,  171,  176,  181,  184,   60,  164,  167,
 /*   500 */   171,  176,  181,  184,   83,  164,  167,  171,  176,  181,
 /*   510 */   184,   61,  164,  167,  171,  176,  181,  184,  164,   53,
 /*   520 */   164,  167,  171,  176,  181,  184,   84,  164,  167,  171,
 /*   530 */   176,  181,  184,   68,  164,  167,  171,  176,  181,  184,
 /*   540 */    69,  164,  167,  171,  176,  181,  184,   50,  164,  167,
 /*   550 */   171,  176,  181,  184,  164,   44,  164,  167,  171,  176,
 /*   560 */   181,  184,   45,  164,  167,  171,  176,  181,  184,   49,
 /*   570 */   164,  167,  171,  176,  181,  184,  164,   16,  164,  164,
 /*   580 */   164,   17,   86,  110,   37,   16,  205,  164,  164,   17,
 /*   590 */    64,  110,   37,  164,    6,  164,  164,  164,    8,   77,
 /*   600 */   164,  205,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    10 */    12,   13,   14,   15,   13,   14,   15,   28,   29,   21,
 /*    20 */     1,   25,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    30 */    10,   11,   12,   13,   14,   15,   33,   25,   35,   36,
 /*    40 */    37,   38,   39,   40,   41,   25,    1,    1,    2,    3,
 /*    50 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*    60 */    14,   15,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    70 */    10,   11,   12,   13,   14,   15,   33,   15,   35,   36,
 /*    80 */    37,   38,   39,   40,   41,   25,   19,    1,    2,    3,
 /*    90 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   100 */    14,   15,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   110 */    10,   11,   12,   13,   14,   15,   20,   42,   22,   42,
 /*   120 */    42,   21,   42,   11,    2,    3,    4,    5,    6,    7,
 /*   130 */     8,    9,   10,   11,   12,   13,   14,   15,   26,   42,
 /*   140 */    42,   42,   20,   42,   42,    2,    3,    4,    5,    6,
 /*   150 */     7,    8,    9,   10,   11,   12,   13,   14,   15,    2,
 /*   160 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   170 */    13,   14,   15,    2,    3,    4,    5,    6,    7,    8,
 /*   180 */     9,   10,   11,   12,   13,   14,   15,    2,    3,    4,
 /*   190 */     5,    6,    7,    8,    9,   10,   42,   12,   13,   14,
 /*   200 */    15,   27,   42,   42,   30,   31,   32,   33,   42,   35,
 /*   210 */    36,   37,   38,   39,   40,   41,    4,    5,    6,    7,
 /*   220 */     8,    9,   10,   11,   12,   13,   14,   15,    5,    6,
 /*   230 */     7,    8,    9,   10,   11,   12,   13,   14,   15,    5,
 /*   240 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   250 */    33,   11,   35,   36,   37,   38,   39,   40,   41,   11,
 /*   260 */     0,   13,   14,   15,   11,   25,   42,   42,   42,   21,
 /*   270 */    22,   42,   12,   25,   26,   22,   16,   17,   18,   19,
 /*   280 */    42,   42,   42,   33,   24,   35,   36,   37,   38,   39,
 /*   290 */    11,   41,   33,   34,   35,   36,   37,   38,   39,   11,
 /*   300 */    21,   42,   42,   42,   42,   42,   42,   19,   11,   21,
 /*   310 */    22,   42,   24,   25,   26,   42,   19,   42,   21,   22,
 /*   320 */    12,   24,   25,   26,   16,   17,   18,   19,   42,   12,
 /*   330 */    42,   42,   24,   16,   17,   18,   19,   42,   42,   42,
 /*   340 */    42,   24,   33,   42,   35,   36,   37,   38,   39,   12,
 /*   350 */    42,   42,   42,   16,   17,   18,   19,   42,   42,   42,
 /*   360 */    33,   24,   35,   36,   37,   38,   39,   33,   42,   35,
 /*   370 */    36,   37,   38,   39,   42,   33,   42,   35,   36,   37,
 /*   380 */    38,   39,   33,   42,   35,   36,   37,   38,   39,   33,
 /*   390 */    42,   35,   36,   37,   38,   39,   33,   42,   35,   36,
 /*   400 */    37,   38,   39,   33,   42,   35,   36,   37,   38,   39,
 /*   410 */    42,   33,   42,   35,   36,   37,   38,   39,   33,   42,
 /*   420 */    35,   36,   37,   38,   39,   33,   42,   35,   36,   37,
 /*   430 */    38,   39,   33,   42,   35,   36,   37,   38,   39,   33,
 /*   440 */    42,   35,   36,   37,   38,   39,   42,   33,   42,   35,
 /*   450 */    36,   37,   38,   39,   33,   42,   35,   36,   37,   38,
 /*   460 */    39,   33,   42,   35,   36,   37,   38,   39,   33,   42,
 /*   470 */    35,   36,   37,   38,   39,   33,   42,   35,   36,   37,
 /*   480 */    38,   39,   42,   33,   42,   35,   36,   37,   38,   39,
 /*   490 */    33,   42,   35,   36,   37,   38,   39,   33,   42,   35,
 /*   500 */    36,   37,   38,   39,   33,   42,   35,   36,   37,   38,
 /*   510 */    39,   33,   42,   35,   36,   37,   38,   39,   42,   33,
 /*   520 */    42,   35,   36,   37,   38,   39,   33,   42,   35,   36,
 /*   530 */    37,   38,   39,   33,   42,   35,   36,   37,   38,   39,
 /*   540 */    33,   42,   35,   36,   37,   38,   39,   33,   42,   35,
 /*   550 */    36,   37,   38,   39,   42,   33,   42,   35,   36,   37,
 /*   560 */    38,   39,   33,   42,   35,   36,   37,   38,   39,   33,
 /*   570 */    42,   35,   36,   37,   38,   39,   42,   12,   42,   42,
 /*   580 */    42,   16,   17,   18,   19,   12,   11,   42,   42,   16,
 /*   590 */    17,   18,   19,   42,   19,   42,   42,   42,   23,   24,
 /*   600 */    42,   26,
};
#define YY_SHIFT_USE_DFLT (602)
#define YY_SHIFT_COUNT    (86)
#define YY_SHIFT_MIN      (-4)
#define YY_SHIFT_MAX      (575)
static const short yy_shift_ofst[] = {
 /*     0 */   602,  260,  308,  308,  308,  308,  565,  317,  337,  565,
 /*    10 */   565,  573,  565,  565,  565,  565,  565,  565,  565,  565,
 /*    20 */   565,  565,  565,  565,  565,  565,  565,  565,  565,  565,
 /*    30 */   565,  565,  565,  565,  565,  565,  565,  565,  565,   -2,
 /*    40 */    20,   46,   60,   86,  100,  122,  143,  157,  171,  171,
 /*    50 */   171,  185,  212,  212,  223,  234,  234,  234,  234,  234,
 /*    60 */   234,  223,  248,  288,  297,  575,    1,    1,    1,    1,
 /*    70 */   240,  279,  253,  112,   96,   19,   -4,   12,   45,   62,
 /*    80 */    62,   62,   62,   62,   62,   62,   67,
};
#define YY_REDUCE_USE_DFLT (-12)
#define YY_REDUCE_COUNT (38)
#define YY_REDUCE_MIN   (-11)
#define YY_REDUCE_MAX   (536)
static const short yy_reduce_ofst[] = {
 /*     0 */   -11,  174,    3,   43,  217,  250,  259,  309,  327,  334,
 /*    10 */   342,  349,  356,  363,  370,  378,  385,  392,  399,  406,
 /*    20 */   414,  421,  428,  435,  442,  450,  457,  464,  471,  478,
 /*    30 */   486,  493,  349,  500,  507,  514,  522,  529,  536,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   212,  200,  200,  200,  200,  216,  168,  216,  216,  216,
 /*    10 */   216,  216,  216,  216,  216,  216,  216,  216,  216,  216,
 /*    20 */   216,  216,  216,  216,  216,  216,  216,  216,  216,  216,
 /*    30 */   216,  216,  216,  216,  216,  216,  216,  216,  216,  216,
 /*    40 */   216,  216,  216,  216,  216,  216,  216,  216,  169,  170,
 /*    50 */   166,  204,  183,  182,  186,  195,  194,  193,  192,  191,
 /*    60 */   190,  185,  172,  162,  162,  162,  175,  174,  173,  172,
 /*    70 */   216,  216,  216,  216,  216,  216,  216,  216,  216,  180,
 /*    80 */   188,  187,  179,  189,  178,  177,  162,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  internal_ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
  yyStackEntry *yystackEnd;            /* Last entry in the stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void internal_ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "SEMICOLON",     "QUESTIONMARK",  "AND",         
  "OR",            "EQUAL",         "NOT_EQUAL",     "GREATER",     
  "GREATER_EQUAL",  "LESS",          "LESS_EQUAL",    "PLUS",        
  "MINUS",         "MULTIPLY",      "DIVIDE",        "EXP",         
  "NOT",           "IDENTIFIER",    "VALUE",         "LEFT_ROUND",  
  "RIGHT_ROUND",   "COLON",         "COMMA",         "ASSIGN",      
  "LEFT_SQUARE",   "RIGHT_SQUARE",  "ARROW",         "error",       
  "model",         "statements",    "statement",     "assignment",  
  "reaction",      "expression",    "arguments",     "comparison",  
  "sum",           "product",       "conjunction",   "disjunction", 
  "reactionSide",  "reactionComponent",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "expression ::= IDENTIFIER",
 /*   1 */ "expression ::= IDENTIFIER LEFT_ROUND arguments RIGHT_ROUND",
 /*   2 */ "expression ::= VALUE",
 /*   3 */ "expression ::= LEFT_ROUND expression RIGHT_ROUND",
 /*   4 */ "comparison ::= expression QUESTIONMARK expression COLON expression",
 /*   5 */ "expression ::= comparison",
 /*   6 */ "arguments ::=",
 /*   7 */ "arguments ::= expression",
 /*   8 */ "arguments ::= arguments COMMA expression",
 /*   9 */ "expression ::= sum",
 /*  10 */ "sum ::= expression PLUS expression",
 /*  11 */ "sum ::= expression MINUS expression",
 /*  12 */ "sum ::= sum PLUS expression",
 /*  13 */ "sum ::= sum MINUS expression",
 /*  14 */ "expression ::= product",
 /*  15 */ "product ::= expression MULTIPLY expression",
 /*  16 */ "product ::= expression DIVIDE expression",
 /*  17 */ "product ::= product MULTIPLY expression",
 /*  18 */ "product ::= product DIVIDE expression",
 /*  19 */ "expression ::= conjunction",
 /*  20 */ "conjunction ::= expression AND expression",
 /*  21 */ "conjunction ::= conjunction AND expression",
 /*  22 */ "expression ::= disjunction",
 /*  23 */ "disjunction ::= expression OR expression",
 /*  24 */ "disjunction ::= disjunction OR expression",
 /*  25 */ "expression ::= NOT expression",
 /*  26 */ "expression ::= MINUS expression",
 /*  27 */ "expression ::= expression EXP expression",
 /*  28 */ "expression ::= expression EQUAL expression",
 /*  29 */ "expression ::= expression NOT_EQUAL expression",
 /*  30 */ "expression ::= expression GREATER expression",
 /*  31 */ "expression ::= expression GREATER_EQUAL expression",
 /*  32 */ "expression ::= expression LESS expression",
 /*  33 */ "expression ::= expression LESS_EQUAL expression",
 /*  34 */ "assignment ::= IDENTIFIER ASSIGN expression SEMICOLON",
 /*  35 */ "assignment ::= IDENTIFIER ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON",
 /*  36 */ "reaction ::= reactionSide ARROW reactionSide COMMA expression SEMICOLON",
 /*  37 */ "reaction ::= reactionSide ARROW reactionSide COMMA LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON",
 /*  38 */ "reactionSide ::=",
 /*  39 */ "reactionSide ::= reactionComponent",
 /*  40 */ "reactionSide ::= reactionSide PLUS reactionComponent",
 /*  41 */ "reactionSide ::= expression PLUS expression",
 /*  42 */ "reactionSide ::= reactionSide PLUS expression",
 /*  43 */ "reactionComponent ::= IDENTIFIER",
 /*  44 */ "reactionComponent ::= IDENTIFIER LEFT_SQUARE RIGHT_SQUARE",
 /*  45 */ "reactionComponent ::= expression MULTIPLY IDENTIFIER",
 /*  46 */ "reactionComponent ::= expression MULTIPLY IDENTIFIER LEFT_SQUARE RIGHT_SQUARE",
 /*  47 */ "reactionComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionSide COLON reactionSide RIGHT_SQUARE",
 /*  48 */ "model ::= statements",
 /*  49 */ "statements ::= statements statement",
 /*  50 */ "statements ::=",
 /*  51 */ "statement ::= assignment",
 /*  52 */ "statement ::= reaction",
 /*  53 */ "statement ::= error",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;
  if( p->yystack==&p->yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->yystk0;
  }else{
    pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to internal_ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
void internal_ParseInit(void *yypParser){
  yyParser *pParser = (yyParser*)yypParser;
#ifdef YYTRACKMAXSTACKDEPTH
  pParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
  pParser->yytos = NULL;
  pParser->yystack = NULL;
  pParser->yystksz = 0;
  if( yyGrowStack(pParser) ){
    pParser->yystack = &pParser->yystk0;
    pParser->yystksz = 1;
  }
#endif
#ifndef YYNOERRORRECOVERY
  pParser->yyerrcnt = -1;
#endif
  pParser->yytos = pParser->yystack;
  pParser->yystack[0].stateno = 0;
  pParser->yystack[0].major = 0;
#if YYSTACKDEPTH>0
  pParser->yystackEnd = &pParser->yystack[YYSTACKDEPTH-1];
#endif
}

#ifndef internal_Parse_ENGINEALWAYSONSTACK
/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to internal_Parse and internal_ParseFree.
*/
void *internal_ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ) internal_ParseInit(pParser);
  return pParser;
}
#endif /* internal_Parse_ENGINEALWAYSONSTACK */


/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  internal_ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
      /* TERMINAL Destructor */
    case 1: /* SEMICOLON */
    case 2: /* QUESTIONMARK */
    case 3: /* AND */
    case 4: /* OR */
    case 5: /* EQUAL */
    case 6: /* NOT_EQUAL */
    case 7: /* GREATER */
    case 8: /* GREATER_EQUAL */
    case 9: /* LESS */
    case 10: /* LESS_EQUAL */
    case 11: /* PLUS */
    case 12: /* MINUS */
    case 13: /* MULTIPLY */
    case 14: /* DIVIDE */
    case 15: /* EXP */
    case 16: /* NOT */
    case 17: /* IDENTIFIER */
    case 18: /* VALUE */
    case 19: /* LEFT_ROUND */
    case 20: /* RIGHT_ROUND */
    case 21: /* COLON */
    case 22: /* COMMA */
    case 23: /* ASSIGN */
    case 24: /* LEFT_SQUARE */
    case 25: /* RIGHT_SQUARE */
    case 26: /* ARROW */
{
#line 8 "cmdl/grammar.y"

	delete (yypminor->yy0);
	(yypminor->yy0) = nullptr;

#line 797 "cmdl/grammar.c"
}
      break;
    case 33: /* expression */
{
#line 167 "cmdl/grammar.y"
 
	delete (yypminor->yy81);
	(yypminor->yy81) = nullptr;

#line 807 "cmdl/grammar.c"
}
      break;
    case 34: /* arguments */
{
#line 217 "cmdl/grammar.y"
 
	delete (yypminor->yy1);
	(yypminor->yy1) = nullptr;

#line 817 "cmdl/grammar.c"
}
      break;
    case 35: /* comparison */
{
#line 201 "cmdl/grammar.y"
 
	delete (yypminor->yy78);
	(yypminor->yy78) = nullptr;

#line 827 "cmdl/grammar.c"
}
      break;
    case 36: /* sum */
{
#line 238 "cmdl/grammar.y"
 
	delete (yypminor->yy83);
	(yypminor->yy83) = nullptr;

#line 837 "cmdl/grammar.c"
}
      break;
    case 37: /* product */
{
#line 267 "cmdl/grammar.y"
 
	delete (yypminor->yy63);
	(yypminor->yy63) = nullptr;

#line 847 "cmdl/grammar.c"
}
      break;
    case 38: /* conjunction */
{
#line 297 "cmdl/grammar.y"
 
	delete (yypminor->yy16);
	(yypminor->yy16) = nullptr;

#line 857 "cmdl/grammar.c"
}
      break;
    case 39: /* disjunction */
{
#line 317 "cmdl/grammar.y"
 
	delete (yypminor->yy72);
	(yypminor->yy72) = nullptr;

#line 867 "cmdl/grammar.c"
}
      break;
    case 40: /* reactionSide */
{
#line 426 "cmdl/grammar.y"
 
	delete (yypminor->yy44);
	(yypminor->yy44) = nullptr;

#line 877 "cmdl/grammar.c"
}
      break;
    case 41: /* reactionComponent */
{
#line 468 "cmdl/grammar.y"
 
	delete (yypminor->yy66);
	(yypminor->yy66) = nullptr;

#line 887 "cmdl/grammar.c"
}
      break;
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/*
** Clear all secondary memory allocations from the parser
*/
void internal_ParseFinalize(void *p){
  yyParser *pParser = (yyParser*)p;
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
}

#ifndef internal_Parse_ENGINEALWAYSONSTACK
/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void internal_ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  internal_ParseFinalize(p);
  (*freeProc)(p);
}
#endif /* internal_Parse_ENGINEALWAYSONSTACK */

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int internal_ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yytos->stateno;
 
  if( stateno>=YY_MIN_REDUCE ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
  do{
    i = yy_shift_ofst[stateno];
    assert( iLookAhead!=YYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead],
               yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    }else{
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   internal_ParseARG_FETCH;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
#line 5 "cmdl/grammar.y"
throw std::exception("Parser stack overflow while parsing cmdl file.");
#line 1068 "cmdl/grammar.c"
/******** End %stack_overflow code ********************************************/
   internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%sShift '%s', go to state %d\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%sShift '%s'\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major]);
    }
  }
}
#else
# define yyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  internal_ParseTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>yypParser->yystackEnd ){
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;       /* Symbol on the left-hand side of the rule */
  signed char nrhs;     /* Negative of the number of RHS symbols in the rule */
} yyRuleInfo[] = {
  { 33, -1 },
  { 33, -4 },
  { 33, -1 },
  { 33, -3 },
  { 35, -5 },
  { 33, -1 },
  { 34, 0 },
  { 34, -1 },
  { 34, -3 },
  { 33, -1 },
  { 36, -3 },
  { 36, -3 },
  { 36, -3 },
  { 36, -3 },
  { 33, -1 },
  { 37, -3 },
  { 37, -3 },
  { 37, -3 },
  { 37, -3 },
  { 33, -1 },
  { 38, -3 },
  { 38, -3 },
  { 33, -1 },
  { 39, -3 },
  { 39, -3 },
  { 33, -2 },
  { 33, -2 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 31, -4 },
  { 31, -6 },
  { 32, -6 },
  { 32, -8 },
  { 40, 0 },
  { 40, -1 },
  { 40, -3 },
  { 40, -3 },
  { 40, -3 },
  { 41, -1 },
  { 41, -3 },
  { 41, -3 },
  { 41, -5 },
  { 41, -7 },
  { 28, -1 },
  { 29, -2 },
  { 29, 0 },
  { 30, -1 },
  { 30, -1 },
  { 30, -1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno        /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  internal_ParseARG_FETCH;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s], go to state %d.\n", yyTracePrompt,
      yyRuleName[yyruleno], yymsp[yysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfo[yyruleno].nrhs==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yytos>=yypParser->yystackEnd ){
      yyStackOverflow(yypParser);
      return;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        return;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* expression ::= IDENTIFIER */
#line 171 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new variable_expression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1266 "cmdl/grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 1: /* expression ::= IDENTIFIER LEFT_ROUND arguments RIGHT_ROUND */
#line 176 "cmdl/grammar.y"
{
	auto func = new function_expression(*yymsp[-3].minor.yy0);
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yylhsminor.yy81 = nullptr;
	for(auto& argument : *yymsp[-1].minor.yy1)
	{
		func->push_back(std::move(argument));
	}
	delete yymsp[-1].minor.yy1;
	yymsp[-1].minor.yy1 = nullptr;
	yylhsminor.yy81 = func;
	func = nullptr;
}
#line 1285 "cmdl/grammar.c"
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,20,&yymsp[0].minor);
  yymsp[-3].minor.yy81 = yylhsminor.yy81;
        break;
      case 2: /* expression ::= VALUE */
#line 191 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new number_expression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1297 "cmdl/grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 3: /* expression ::= LEFT_ROUND expression RIGHT_ROUND */
{  yy_destructor(yypParser,19,&yymsp[-2].minor);
#line 196 "cmdl/grammar.y"
{
	yymsp[-2].minor.yy81 = yymsp[-1].minor.yy81;
}
#line 1306 "cmdl/grammar.c"
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
        break;
      case 4: /* comparison ::= expression QUESTIONMARK expression COLON expression */
#line 205 "cmdl/grammar.y"
{
	yylhsminor.yy78 = new conditional_expression(std::unique_ptr<expression_base>(yymsp[-4].minor.yy81), std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
	yymsp[-2].minor.yy81 = nullptr;
	yymsp[0].minor.yy81 = nullptr;
	yymsp[-4].minor.yy81 = nullptr;
}
#line 1318 "cmdl/grammar.c"
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-4].minor.yy78 = yylhsminor.yy78;
        break;
      case 5: /* expression ::= comparison */
#line 211 "cmdl/grammar.y"
{
	yylhsminor.yy81 = yymsp[0].minor.yy78;
}
#line 1328 "cmdl/grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 6: /* arguments ::= */
#line 221 "cmdl/grammar.y"
{
	yymsp[1].minor.yy1 = new arguments();
}
#line 1336 "cmdl/grammar.c"
        break;
      case 7: /* arguments ::= expression */
#line 224 "cmdl/grammar.y"
{
	yylhsminor.yy1 = new arguments();
	yylhsminor.yy1->push_back(typename arguments::value_type(yymsp[0].minor.yy81));
	yymsp[0].minor.yy81 = nullptr;
}
#line 1345 "cmdl/grammar.c"
  yymsp[0].minor.yy1 = yylhsminor.yy1;
        break;
      case 8: /* arguments ::= arguments COMMA expression */
#line 229 "cmdl/grammar.y"
{
	yylhsminor.yy1 = yymsp[-2].minor.yy1;
	yymsp[-2].minor.yy1 = nullptr;
	yylhsminor.yy1->push_back(typename arguments::value_type(yymsp[0].minor.yy81));
	yymsp[0].minor.yy81 = nullptr;
}
#line 1356 "cmdl/grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy1 = yylhsminor.yy1;
        break;
      case 9: /* expression ::= sum */
#line 242 "cmdl/grammar.y"
{
	yylhsminor.yy81 = yymsp[0].minor.yy83;
}
#line 1365 "cmdl/grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 10: /* sum ::= expression PLUS expression */
#line 245 "cmdl/grammar.y"
{
	yylhsminor.yy83 = new sum_expression();
	yylhsminor.yy83->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy81));
	yylhsminor.yy83->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1375 "cmdl/grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy83 = yylhsminor.yy83;
        break;
      case 11: /* sum ::= expression MINUS expression */
#line 250 "cmdl/grammar.y"
{
	yylhsminor.yy83 = new sum_expression();
	yylhsminor.yy83->push_back(false,  std::unique_ptr<expression_base>(yymsp[-2].minor.yy81));
	yylhsminor.yy83->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1386 "cmdl/grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy83 = yylhsminor.yy83;
        break;
      case 12: /* sum ::= sum PLUS expression */
#line 255 "cmdl/grammar.y"
{
	yylhsminor.yy83 = yymsp[-2].minor.yy83;
	yylhsminor.yy83->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1396 "cmdl/grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy83 = yylhsminor.yy83;
        break;
      case 13: /* sum ::= sum MINUS expression */
#line 259 "cmdl/grammar.y"
{
	yylhsminor.yy83 = yymsp[-2].minor.yy83;
	yylhsminor.yy83->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1406 "cmdl/grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy83 = yylhsminor.yy83;
        break;
      case 14: /* expression ::= product */
#line 271 "cmdl/grammar.y"
{
	yylhsminor.yy81 = yymsp[0].minor.yy63;
}
#line 1415 "cmdl/grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 15: /* product ::= expression MULTIPLY expression */
#line 274 "cmdl/grammar.y"
{
	yylhsminor.yy63 = new product_expression();
	yylhsminor.yy63->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy81));
	yylhsminor.yy63->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));

}
#line 1426 "cmdl/grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy63 = yylhsminor.yy63;
        break;
      case 16: /* product ::= expression DIVIDE expression */
#line 280 "cmdl/grammar.y"
{
	yylhsminor.yy63 = new product_expression();
	yylhsminor.yy63->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy81));
	yylhsminor.yy63->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));

}
#line 1438 "cmdl/grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy63 = yylhsminor.yy63;
        break;
      case 17: /* product ::= product MULTIPLY expression */
#line 286 "cmdl/grammar.y"
{
	yylhsminor.yy63 = yymsp[-2].minor.yy63;
	yylhsminor.yy63->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1448 "cmdl/grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy63 = yylhsminor.yy63;
        break;
      case 18: /* product ::= product DIVIDE expression */
#line 290 "cmdl/grammar.y"
{
	yylhsminor.yy63 = yymsp[-2].minor.yy63;
	yylhsminor.yy63->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1458 "cmdl/grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy63 = yylhsminor.yy63;
        break;
      case 19: /* expression ::= conjunction */
#line 301 "cmdl/grammar.y"
{
	yylhsminor.yy81 = yymsp[0].minor.yy16;
}
#line 1467 "cmdl/grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 20: /* conjunction ::= expression AND expression */
#line 304 "cmdl/grammar.y"
{
	yylhsminor.yy16 = new conjunction_expression();
	yylhsminor.yy16->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy81));
	yylhsminor.yy16->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));

}
#line 1478 "cmdl/grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 21: /* conjunction ::= conjunction AND expression */
#line 310 "cmdl/grammar.y"
{
	yylhsminor.yy16 = yymsp[-2].minor.yy16;
	yylhsminor.yy16->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1488 "cmdl/grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 22: /* expression ::= disjunction */
#line 321 "cmdl/grammar.y"
{
	yylhsminor.yy81 = yymsp[0].minor.yy72;
}
#line 1497 "cmdl/grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 23: /* disjunction ::= expression OR expression */
#line 324 "cmdl/grammar.y"
{
	yylhsminor.yy72 = new disjunction_expression();
	yylhsminor.yy72->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy81));
	yylhsminor.yy72->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));

}
#line 1508 "cmdl/grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy72 = yylhsminor.yy72;
        break;
      case 24: /* disjunction ::= disjunction OR expression */
#line 330 "cmdl/grammar.y"
{
	yylhsminor.yy72 = yymsp[-2].minor.yy72;
	yylhsminor.yy72->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1518 "cmdl/grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy72 = yylhsminor.yy72;
        break;
      case 25: /* expression ::= NOT expression */
{  yy_destructor(yypParser,16,&yymsp[-1].minor);
#line 336 "cmdl/grammar.y"
{
	yymsp[-1].minor.yy81 = new unary_not_expression(std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1528 "cmdl/grammar.c"
}
        break;
      case 26: /* expression ::= MINUS expression */
{  yy_destructor(yypParser,12,&yymsp[-1].minor);
#line 341 "cmdl/grammar.y"
{
	yymsp[-1].minor.yy81 = new unary_minus_expression(std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1537 "cmdl/grammar.c"
}
        break;
      case 27: /* expression ::= expression EXP expression */
#line 346 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new exponentiation_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81));
}
#line 1545 "cmdl/grammar.c"
  yy_destructor(yypParser,15,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 28: /* expression ::= expression EQUAL expression */
#line 352 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81), comparison_expression::type_equal);
}
#line 1554 "cmdl/grammar.c"
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 29: /* expression ::= expression NOT_EQUAL expression */
#line 355 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81), comparison_expression::type_not_equal);
}
#line 1563 "cmdl/grammar.c"
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 30: /* expression ::= expression GREATER expression */
#line 358 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81), comparison_expression::type_greater);
}
#line 1572 "cmdl/grammar.c"
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 31: /* expression ::= expression GREATER_EQUAL expression */
#line 361 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81), comparison_expression::type_greater_equal);
}
#line 1581 "cmdl/grammar.c"
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 32: /* expression ::= expression LESS expression */
#line 364 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81), comparison_expression::type_less);
}
#line 1590 "cmdl/grammar.c"
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 33: /* expression ::= expression LESS_EQUAL expression */
#line 367 "cmdl/grammar.y"
{
	yylhsminor.yy81 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy81), std::unique_ptr<expression_base>(yymsp[0].minor.yy81), comparison_expression::type_less_equal);
}
#line 1599 "cmdl/grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 34: /* assignment ::= IDENTIFIER ASSIGN expression SEMICOLON */
#line 372 "cmdl/grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-3].minor.yy0 and yymsp[-1].minor.yy81 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-1].minor.yy81);
	yymsp[-1].minor.yy81 = nullptr;

	parseTree->create_variable(std::move(name), parseTree->get_expression_value(e_temp.get()));
}
#line 1616 "cmdl/grammar.c"
  yy_destructor(yypParser,23,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 35: /* assignment ::= IDENTIFIER ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 384 "cmdl/grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy0 and yymsp[-2].minor.yy81 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-5].minor.yy0;
	delete yymsp[-5].minor.yy0;
	yymsp[-5].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-2].minor.yy81);
	yymsp[-2].minor.yy81 = nullptr;

	parseTree->create_variable(std::move(name), std::move(e_temp));
}
#line 1633 "cmdl/grammar.c"
  yy_destructor(yypParser,23,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-3].minor);
  yy_destructor(yypParser,25,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 36: /* reaction ::= reactionSide ARROW reactionSide COMMA expression SEMICOLON */
#line 398 "cmdl/grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy44, yymsp[-3].minor.yy44 and yymsp[-1].minor.yy81 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<reaction_side>(yymsp[-5].minor.yy44);
	auto products_temp = std::unique_ptr<reaction_side>(yymsp[-3].minor.yy44);
	auto e_temp = yymsp[-1].minor.yy81;
	yymsp[-5].minor.yy44 = nullptr;
	yymsp[-3].minor.yy44 = nullptr;
	yymsp[-1].minor.yy81 = nullptr;

	parseTree->create_reaction(std::move(reactants_temp), std::move(products_temp), parseTree->get_expression_value(e_temp));
	delete e_temp;
	e_temp = nullptr;
}
#line 1655 "cmdl/grammar.c"
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 37: /* reaction ::= reactionSide ARROW reactionSide COMMA LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 412 "cmdl/grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-7].minor.yy44, yymsp[-5].minor.yy44 and yymsp[-2].minor.yy81 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<reaction_side>(yymsp[-7].minor.yy44);
	auto products_temp = std::unique_ptr<reaction_side>(yymsp[-5].minor.yy44);
	auto e_temp = yymsp[-2].minor.yy81;
	yymsp[-7].minor.yy44 = nullptr;
	yymsp[-5].minor.yy44 = nullptr;
	yymsp[-2].minor.yy81 = nullptr;

	parseTree->create_reaction(std::move(reactants_temp), std::move(products_temp), std::unique_ptr<expression_base>(e_temp));
}
#line 1674 "cmdl/grammar.c"
  yy_destructor(yypParser,26,&yymsp[-6].minor);
  yy_destructor(yypParser,22,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-3].minor);
  yy_destructor(yypParser,25,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 38: /* reactionSide ::= */
#line 430 "cmdl/grammar.y"
{
	yymsp[1].minor.yy44 = new reaction_side();
}
#line 1686 "cmdl/grammar.c"
        break;
      case 39: /* reactionSide ::= reactionComponent */
#line 433 "cmdl/grammar.y"
{
	auto rc_temp = std::unique_ptr<reaction_component>(yymsp[0].minor.yy66);
	yymsp[0].minor.yy66 = nullptr;

	yylhsminor.yy44 = new reaction_side();
	yylhsminor.yy44->push_back(std::move(rc_temp));
}
#line 1697 "cmdl/grammar.c"
  yymsp[0].minor.yy44 = yylhsminor.yy44;
        break;
      case 40: /* reactionSide ::= reactionSide PLUS reactionComponent */
#line 440 "cmdl/grammar.y"
{
	yylhsminor.yy44 = yymsp[-2].minor.yy44;
	yymsp[-2].minor.yy44 = nullptr;
	auto rc_temp = std::unique_ptr<reaction_component>(yymsp[0].minor.yy66);
	yymsp[0].minor.yy66 = nullptr;

	yylhsminor.yy44->push_back(std::move(rc_temp));
}
#line 1710 "cmdl/grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy44 = yylhsminor.yy44;
        break;
      case 41: /* reactionSide ::= expression PLUS expression */
#line 449 "cmdl/grammar.y"
{
	delete(yymsp[-2].minor.yy81);
	yymsp[-2].minor.yy81=nullptr;
	delete(yymsp[0].minor.yy81);
	yymsp[0].minor.yy81=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1723 "cmdl/grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 42: /* reactionSide ::= reactionSide PLUS expression */
#line 459 "cmdl/grammar.y"
{
	delete(yymsp[0].minor.yy81);
	yymsp[0].minor.yy81=nullptr;
	delete(yymsp[-2].minor.yy44);
	yymsp[-2].minor.yy44=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1735 "cmdl/grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 43: /* reactionComponent ::= IDENTIFIER */
#line 472 "cmdl/grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	yylhsminor.yy66 = nullptr;

	yylhsminor.yy66 = new reaction_component(state, 1, false);
}
#line 1748 "cmdl/grammar.c"
  yymsp[0].minor.yy66 = yylhsminor.yy66;
        break;
      case 44: /* reactionComponent ::= IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 481 "cmdl/grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	yylhsminor.yy66 = nullptr;

	yylhsminor.yy66 = new reaction_component(state, 1, true);
}
#line 1761 "cmdl/grammar.c"
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
  yymsp[-2].minor.yy66 = yylhsminor.yy66;
        break;
      case 45: /* reactionComponent ::= expression MULTIPLY IDENTIFIER */
#line 490 "cmdl/grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-2].minor.yy81);
	yymsp[-2].minor.yy81 = nullptr;
	yylhsminor.yy66 = nullptr;

	auto stochiometry = parseTree->get_expression_value(e_temp.get());
	yylhsminor.yy66 = new reaction_component(state, stochiometry, false);
}
#line 1779 "cmdl/grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy66 = yylhsminor.yy66;
        break;
      case 46: /* reactionComponent ::= expression MULTIPLY IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 502 "cmdl/grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-4].minor.yy81);
	yymsp[-4].minor.yy81 = nullptr;
	yylhsminor.yy66 = nullptr;

	auto stochiometry = parseTree->get_expression_value(e_temp.get());
	yylhsminor.yy66 = new reaction_component(state, stochiometry, true);
}
#line 1796 "cmdl/grammar.c"
  yy_destructor(yypParser,13,&yymsp[-3].minor);
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
  yymsp[-4].minor.yy66 = yylhsminor.yy66;
        break;
      case 47: /* reactionComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionSide COLON reactionSide RIGHT_SQUARE */
{  yy_destructor(yypParser,24,&yymsp[-6].minor);
#line 514 "cmdl/grammar.y"
{
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-5].minor.yy81);
	yymsp[-5].minor.yy81 = nullptr;
	auto s1_temp = std::unique_ptr<reaction_side>(yymsp[-3].minor.yy44);
	auto s2_temp = std::unique_ptr<reaction_side>(yymsp[-1].minor.yy44);
	yymsp[-3].minor.yy44 = nullptr;
	yymsp[-1].minor.yy44 = nullptr;
	yymsp[-6].minor.yy66 = nullptr;

	identifier state = parseTree->create_choice(std::move(e_temp), std::move(s1_temp), std::move(s2_temp));
	yymsp[-6].minor.yy66 = new reaction_component(state, 1, false);
}
#line 1817 "cmdl/grammar.c"
  yy_destructor(yypParser,2,&yymsp[-4].minor);
  yy_destructor(yypParser,21,&yymsp[-2].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      default:
      /* (48) model ::= statements */ yytestcase(yyruleno==48);
      /* (49) statements ::= statements statement */ yytestcase(yyruleno==49);
      /* (50) statements ::= */ yytestcase(yyruleno==50);
      /* (51) statement ::= assignment (OPTIMIZED OUT) */ assert(yyruleno!=51);
      /* (52) statement ::= reaction (OPTIMIZED OUT) */ assert(yyruleno!=52);
      /* (53) statement ::= error (OPTIMIZED OUT) */ assert(yyruleno!=53);
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[yysize].stateno,(YYCODETYPE)yygoto);

  /* There are no SHIFTREDUCE actions on nonterminals because the table
  ** generator has simplified them to pure REDUCE actions. */
  assert( !(yyact>YY_MAX_SHIFT && yyact<=YY_MAX_SHIFTREDUCE) );

  /* It is not possible for a REDUCE to be followed by an error */
  assert( yyact!=YY_ERROR_ACTION );

  if( yyact==YY_ACCEPT_ACTION ){
    yypParser->yytos += yysize;
    yy_accept(yypParser);
  }else{
    yymsp += yysize+1;
    yypParser->yytos = yymsp;
    yymsp->stateno = (YYACTIONTYPE)yyact;
    yymsp->major = (YYCODETYPE)yygoto;
    yyTraceShift(yypParser, yyact);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  internal_ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
#line 4 "cmdl/grammar.y"
throw std::exception("Syntax error.");
#line 1876 "cmdl/grammar.c"
/************ End %parse_failure code *****************************************/
  internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  internal_ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  internal_ParseARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
/************ End %syntax_error code ******************************************/
  internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  internal_ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "internal_ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void internal_Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  internal_ParseTOKENTYPE yyminor       /* The value for the token */
  internal_ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  unsigned int yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  yypParser = (yyParser*)yyp;
  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  internal_ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput '%s'\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    }else if( yyact <= YY_MAX_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos >= yypParser->yystack
            && yymx != YYERRORSYMBOL
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos < yypParser->yystack || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}