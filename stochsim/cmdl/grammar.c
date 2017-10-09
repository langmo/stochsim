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
#define YYNOCODE 39
#define YYACTIONTYPE unsigned char
#define internal_ParseTOKENTYPE terminal_symbol*
typedef union {
  int yyinit;
  internal_ParseTOKENTYPE yy0;
  conjunction_expression* yy4;
  reaction_component* yy30;
  reaction_side* yy40;
  sum_expression* yy43;
  product_expression* yy47;
  expression_base* yy53;
  disjunction_expression* yy72;
  int yy77;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define internal_ParseARG_SDECL parse_tree* parseTree;
#define internal_ParseARG_PDECL ,parse_tree* parseTree
#define internal_ParseARG_FETCH parse_tree* parseTree = yypParser->parseTree
#define internal_ParseARG_STORE yypParser->parseTree = parseTree
#define YYERRORSYMBOL 25
#define YYERRSYMDT yy77
#define YYNSTATE             68
#define YYNRULE              47
#define YY_MAX_SHIFT         67
#define YY_MIN_SHIFTREDUCE   87
#define YY_MAX_SHIFTREDUCE   133
#define YY_MIN_REDUCE        134
#define YY_MAX_REDUCE        180
#define YY_ERROR_ACTION      181
#define YY_ACCEPT_ACTION     182
#define YY_NO_ACTION         183
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
#define YY_ACTTAB_COUNT (374)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    24,   23,   21,   20,   19,   18,   17,   16,   29,   27,
 /*    10 */    26,   25,   22,   37,  137,  142,  147,  150,   55,  167,
 /*    20 */    57,  118,  117,   24,   23,   21,   20,   19,   18,   17,
 /*    30 */    16,   29,   27,   26,   25,   22,   24,   23,   21,   20,
 /*    40 */    19,   18,   17,   16,   29,   27,   26,   25,   22,   36,
 /*    50 */   137,  142,  147,  150,  127,  168,   60,  125,  115,   24,
 /*    60 */    23,   21,   20,   19,   18,   17,   16,   29,   27,   26,
 /*    70 */    25,   22,   24,   23,   21,   20,   19,   18,   17,   16,
 /*    80 */    29,   27,   26,   25,   22,   26,   25,   22,  116,   89,
 /*    90 */   182,    1,   24,   23,   21,   20,   19,   18,   17,   16,
 /*   100 */    22,   27,    7,   25,   22,   23,   21,   20,   19,   18,
 /*   110 */    17,   16,   29,   27,   26,   25,   22,  136,  176,  136,
 /*   120 */   136,  176,  176,  176,   37,  137,  142,  147,  150,   56,
 /*   130 */   167,   21,   20,   19,   18,   17,   16,   29,   27,   26,
 /*   140 */    25,   22,  181,  181,  181,  181,  181,  181,   29,   27,
 /*   150 */    26,   25,   22,  175,  136,   11,  136,  136,  136,   12,
 /*   160 */    87,   88,   30,  136,   11,    6,  136,  136,   12,   50,
 /*   170 */    88,   30,   11,  136,  136,  136,   12,   87,   88,   30,
 /*   180 */    11,  136,    8,  136,   12,   48,   88,   30,    3,  136,
 /*   190 */   136,   32,  137,  142,  147,  150,   34,  137,  142,  147,
 /*   200 */   150,  136,    4,   11,    3,  136,  136,   12,   87,   88,
 /*   210 */    30,   31,  137,  142,  147,  150,  136,    2,  136,   11,
 /*   220 */   136,  136,  136,   12,   49,   88,   30,   67,  137,  142,
 /*   230 */   147,  150,   33,  137,  142,  147,  150,  136,  136,   52,
 /*   240 */   137,  142,  147,  150,   61,  137,  142,  147,  150,  136,
 /*   250 */   136,   62,  137,  142,  147,  150,   63,  137,  142,  147,
 /*   260 */   150,   40,  137,  142,  147,  150,   38,  137,  142,  147,
 /*   270 */   150,   64,  137,  142,  147,  150,   41,  137,  142,  147,
 /*   280 */   150,  136,  136,   42,  137,  142,  147,  150,   43,  137,
 /*   290 */   142,  147,  150,   44,  137,  142,  147,  150,   45,  137,
 /*   300 */   142,  147,  150,   46,  137,  142,  147,  150,   65,  137,
 /*   310 */   142,  147,  150,  136,  136,   47,  137,  142,  147,  150,
 /*   320 */    39,  137,  142,  147,  150,   66,  137,  142,  147,  150,
 /*   330 */    53,  137,  142,  147,  150,   54,  137,  142,  147,  150,
 /*   340 */    51,  137,  142,  147,  150,  171,  136,   35,  137,  142,
 /*   350 */   147,  150,  173,  136,  136,  136,   59,  136,  171,  171,
 /*   360 */   171,  136,  136,   58,  136,  173,  173,  136,  136,  136,
 /*   370 */     5,   59,  136,  171,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    10 */    12,   13,   14,   31,   32,   33,   34,   35,   36,   37,
 /*    20 */    22,    1,    1,    2,    3,    4,    5,    6,    7,    8,
 /*    30 */     9,   10,   11,   12,   13,   14,    2,    3,    4,    5,
 /*    40 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   31,
 /*    50 */    32,   33,   34,   35,   22,   37,   22,   22,    1,    2,
 /*    60 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*    70 */    13,   14,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    80 */    10,   11,   12,   13,   14,   12,   13,   14,    1,   19,
 /*    90 */    26,   27,    2,    3,    4,    5,    6,    7,    8,    9,
 /*   100 */    14,   11,   12,   13,   14,    3,    4,    5,    6,    7,
 /*   110 */     8,    9,   10,   11,   12,   13,   14,   38,   25,   38,
 /*   120 */    38,   28,   29,   30,   31,   32,   33,   34,   35,   36,
 /*   130 */    37,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   140 */    13,   14,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   150 */    12,   13,   14,    0,   38,   11,   38,   38,   38,   15,
 /*   160 */    16,   17,   18,   38,   11,   21,   38,   38,   15,   16,
 /*   170 */    17,   18,   11,   38,   38,   38,   15,   16,   17,   18,
 /*   180 */    11,   38,   21,   38,   15,   16,   17,   18,   10,   38,
 /*   190 */    38,   31,   32,   33,   34,   35,   31,   32,   33,   34,
 /*   200 */    35,   38,   24,   11,   10,   38,   38,   15,   16,   17,
 /*   210 */    18,   31,   32,   33,   34,   35,   38,   23,   38,   11,
 /*   220 */    38,   38,   38,   15,   16,   17,   18,   31,   32,   33,
 /*   230 */    34,   35,   31,   32,   33,   34,   35,   38,   38,   31,
 /*   240 */    32,   33,   34,   35,   31,   32,   33,   34,   35,   38,
 /*   250 */    38,   31,   32,   33,   34,   35,   31,   32,   33,   34,
 /*   260 */    35,   31,   32,   33,   34,   35,   31,   32,   33,   34,
 /*   270 */    35,   31,   32,   33,   34,   35,   31,   32,   33,   34,
 /*   280 */    35,   38,   38,   31,   32,   33,   34,   35,   31,   32,
 /*   290 */    33,   34,   35,   31,   32,   33,   34,   35,   31,   32,
 /*   300 */    33,   34,   35,   31,   32,   33,   34,   35,   31,   32,
 /*   310 */    33,   34,   35,   38,   38,   31,   32,   33,   34,   35,
 /*   320 */    31,   32,   33,   34,   35,   31,   32,   33,   34,   35,
 /*   330 */    31,   32,   33,   34,   35,   31,   32,   33,   34,   35,
 /*   340 */    31,   32,   33,   34,   35,   10,   38,   31,   32,   33,
 /*   350 */    34,   35,   10,   38,   38,   38,   21,   38,   23,   24,
 /*   360 */    10,   38,   38,   21,   38,   23,   24,   38,   38,   38,
 /*   370 */    20,   21,   38,   23,
};
#define YY_SHIFT_USE_DFLT (374)
#define YY_SHIFT_COUNT    (67)
#define YY_SHIFT_MIN      (-2)
#define YY_SHIFT_MAX      (350)
static const short yy_shift_ofst[] = {
 /*     0 */   374,  153,  169,  169,  144,  161,  192,  208,  192,  192,
 /*    10 */   192,  192,  192,  192,  192,  192,  192,  192,  192,  192,
 /*    20 */   192,  192,  192,  192,  192,  192,  192,  192,  192,  192,
 /*    30 */   192,   -2,   21,   34,   57,   70,   90,   90,  102,  102,
 /*    40 */   127,  138,  138,  138,  138,  138,  138,  127,  335,  342,
 /*    50 */   350,   73,   73,   73,   73,  178,  194,   20,   32,   35,
 /*    60 */    87,   86,   86,   86,   86,   86,   86,   86,
};
#define YY_REDUCE_USE_DFLT (-19)
#define YY_REDUCE_COUNT (30)
#define YY_REDUCE_MIN   (-18)
#define YY_REDUCE_MAX   (316)
static const short yy_reduce_ofst[] = {
 /*     0 */    64,   93,  -18,   18,  160,  165,  180,  196,  201,  208,
 /*    10 */   213,  220,  225,  230,  235,  240,  245,  252,  257,  262,
 /*    20 */   267,  272,  277,  284,  289,  294,  196,  299,  304,  309,
 /*    30 */   316,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   177,  166,  166,  181,  181,  181,  181,  181,  181,  181,
 /*    10 */   181,  181,  181,  181,  181,  181,  181,  181,  181,  181,
 /*    20 */   181,  181,  181,  181,  181,  181,  181,  181,  181,  181,
 /*    30 */   181,  181,  181,  181,  181,  181,  170,  169,  149,  148,
 /*    40 */   152,  161,  160,  159,  158,  157,  156,  151,  134,  134,
 /*    50 */   134,  138,  141,  139,  140,  181,  181,  181,  181,  181,
 /*    60 */   181,  146,  154,  153,  145,  155,  144,  143,
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
  "$",             "SEMICOLON",     "AND",           "OR",          
  "EQUAL",         "NOT_EQUAL",     "GREATER",       "GREATER_EQUAL",
  "LESS",          "LESS_EQUAL",    "PLUS",          "MINUS",       
  "MULTIPLY",      "DIVIDE",        "EXP",           "NOT",         
  "IDENTIFIER",    "VALUE",         "LEFT_ROUND",    "RIGHT_ROUND", 
  "ASSIGN",        "LEFT_SQUARE",   "RIGHT_SQUARE",  "ARROW",       
  "COMMA",         "error",         "model",         "statements",  
  "statement",     "assignment",    "reaction",      "expression",  
  "sum",           "product",       "conjunction",   "disjunction", 
  "reactionSide",  "reactionComponent",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "expression ::= IDENTIFIER",
 /*   1 */ "expression ::= VALUE",
 /*   2 */ "expression ::= LEFT_ROUND expression RIGHT_ROUND",
 /*   3 */ "expression ::= sum",
 /*   4 */ "sum ::= expression PLUS expression",
 /*   5 */ "sum ::= expression MINUS expression",
 /*   6 */ "sum ::= sum PLUS expression",
 /*   7 */ "sum ::= sum MINUS expression",
 /*   8 */ "expression ::= product",
 /*   9 */ "product ::= expression MULTIPLY expression",
 /*  10 */ "product ::= expression DIVIDE expression",
 /*  11 */ "product ::= product MULTIPLY expression",
 /*  12 */ "product ::= product DIVIDE expression",
 /*  13 */ "expression ::= conjunction",
 /*  14 */ "conjunction ::= expression AND expression",
 /*  15 */ "conjunction ::= conjunction AND expression",
 /*  16 */ "expression ::= disjunction",
 /*  17 */ "disjunction ::= expression OR expression",
 /*  18 */ "disjunction ::= disjunction OR expression",
 /*  19 */ "expression ::= NOT expression",
 /*  20 */ "expression ::= MINUS expression",
 /*  21 */ "expression ::= expression EXP expression",
 /*  22 */ "expression ::= expression EQUAL expression",
 /*  23 */ "expression ::= expression NOT_EQUAL expression",
 /*  24 */ "expression ::= expression GREATER expression",
 /*  25 */ "expression ::= expression GREATER_EQUAL expression",
 /*  26 */ "expression ::= expression LESS expression",
 /*  27 */ "expression ::= expression LESS_EQUAL expression",
 /*  28 */ "assignment ::= IDENTIFIER ASSIGN expression SEMICOLON",
 /*  29 */ "assignment ::= IDENTIFIER ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON",
 /*  30 */ "reaction ::= reactionSide ARROW reactionSide COMMA expression SEMICOLON",
 /*  31 */ "reaction ::= reactionSide ARROW reactionSide COMMA LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON",
 /*  32 */ "reactionSide ::=",
 /*  33 */ "reactionSide ::= reactionComponent",
 /*  34 */ "reactionSide ::= reactionSide PLUS reactionComponent",
 /*  35 */ "reactionSide ::= expression",
 /*  36 */ "reactionSide ::= reactionSide PLUS expression",
 /*  37 */ "reactionComponent ::= IDENTIFIER",
 /*  38 */ "reactionComponent ::= IDENTIFIER LEFT_SQUARE RIGHT_SQUARE",
 /*  39 */ "reactionComponent ::= expression MULTIPLY IDENTIFIER",
 /*  40 */ "reactionComponent ::= expression MULTIPLY IDENTIFIER LEFT_SQUARE RIGHT_SQUARE",
 /*  41 */ "model ::= statements",
 /*  42 */ "statements ::= statements statement",
 /*  43 */ "statements ::=",
 /*  44 */ "statement ::= assignment",
 /*  45 */ "statement ::= reaction",
 /*  46 */ "statement ::= error",
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
    case 2: /* AND */
    case 3: /* OR */
    case 4: /* EQUAL */
    case 5: /* NOT_EQUAL */
    case 6: /* GREATER */
    case 7: /* GREATER_EQUAL */
    case 8: /* LESS */
    case 9: /* LESS_EQUAL */
    case 10: /* PLUS */
    case 11: /* MINUS */
    case 12: /* MULTIPLY */
    case 13: /* DIVIDE */
    case 14: /* EXP */
    case 15: /* NOT */
    case 16: /* IDENTIFIER */
    case 17: /* VALUE */
    case 18: /* LEFT_ROUND */
    case 19: /* RIGHT_ROUND */
    case 20: /* ASSIGN */
    case 21: /* LEFT_SQUARE */
    case 22: /* RIGHT_SQUARE */
    case 23: /* ARROW */
    case 24: /* COMMA */
{
#line 8 "cmdl/grammar.y"

	delete (yypminor->yy0);
	(yypminor->yy0) = nullptr;

#line 735 "cmdl/grammar.c"
}
      break;
    case 31: /* expression */
{
#line 166 "cmdl/grammar.y"
 
	delete (yypminor->yy53);
	(yypminor->yy53) = nullptr;

#line 745 "cmdl/grammar.c"
}
      break;
    case 32: /* sum */
{
#line 187 "cmdl/grammar.y"
 
	delete (yypminor->yy43);
	(yypminor->yy43) = nullptr;

#line 755 "cmdl/grammar.c"
}
      break;
    case 33: /* product */
{
#line 216 "cmdl/grammar.y"
 
	delete (yypminor->yy47);
	(yypminor->yy47) = nullptr;

#line 765 "cmdl/grammar.c"
}
      break;
    case 34: /* conjunction */
{
#line 246 "cmdl/grammar.y"
 
	delete (yypminor->yy4);
	(yypminor->yy4) = nullptr;

#line 775 "cmdl/grammar.c"
}
      break;
    case 35: /* disjunction */
{
#line 266 "cmdl/grammar.y"
 
	delete (yypminor->yy72);
	(yypminor->yy72) = nullptr;

#line 785 "cmdl/grammar.c"
}
      break;
    case 36: /* reactionSide */
{
#line 374 "cmdl/grammar.y"
 
	delete (yypminor->yy40);
	(yypminor->yy40) = nullptr;

#line 795 "cmdl/grammar.c"
}
      break;
    case 37: /* reactionComponent */
{
#line 411 "cmdl/grammar.y"
 
	delete (yypminor->yy30);
	(yypminor->yy30) = nullptr;

#line 805 "cmdl/grammar.c"
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
#line 986 "cmdl/grammar.c"
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
  { 31, -1 },
  { 31, -1 },
  { 31, -3 },
  { 31, -1 },
  { 32, -3 },
  { 32, -3 },
  { 32, -3 },
  { 32, -3 },
  { 31, -1 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 31, -1 },
  { 34, -3 },
  { 34, -3 },
  { 31, -1 },
  { 35, -3 },
  { 35, -3 },
  { 31, -2 },
  { 31, -2 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 29, -4 },
  { 29, -6 },
  { 30, -6 },
  { 30, -8 },
  { 36, 0 },
  { 36, -1 },
  { 36, -3 },
  { 36, -1 },
  { 36, -3 },
  { 37, -1 },
  { 37, -3 },
  { 37, -3 },
  { 37, -5 },
  { 26, -1 },
  { 27, -2 },
  { 27, 0 },
  { 28, -1 },
  { 28, -1 },
  { 28, -1 },
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
#line 170 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new variable_expression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1177 "cmdl/grammar.c"
  yymsp[0].minor.yy53 = yylhsminor.yy53;
        break;
      case 1: /* expression ::= VALUE */
#line 175 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new number_expression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1187 "cmdl/grammar.c"
  yymsp[0].minor.yy53 = yylhsminor.yy53;
        break;
      case 2: /* expression ::= LEFT_ROUND expression RIGHT_ROUND */
{  yy_destructor(yypParser,18,&yymsp[-2].minor);
#line 180 "cmdl/grammar.y"
{
	yymsp[-2].minor.yy53 = yymsp[-1].minor.yy53;
}
#line 1196 "cmdl/grammar.c"
  yy_destructor(yypParser,19,&yymsp[0].minor);
}
        break;
      case 3: /* expression ::= sum */
#line 191 "cmdl/grammar.y"
{
	yylhsminor.yy53 = yymsp[0].minor.yy43;
}
#line 1205 "cmdl/grammar.c"
  yymsp[0].minor.yy53 = yylhsminor.yy53;
        break;
      case 4: /* sum ::= expression PLUS expression */
#line 194 "cmdl/grammar.y"
{
	yylhsminor.yy43 = new sum_expression();
	yylhsminor.yy43->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy53));
	yylhsminor.yy43->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1215 "cmdl/grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy43 = yylhsminor.yy43;
        break;
      case 5: /* sum ::= expression MINUS expression */
#line 199 "cmdl/grammar.y"
{
	yylhsminor.yy43 = new sum_expression();
	yylhsminor.yy43->push_back(false,  std::unique_ptr<expression_base>(yymsp[-2].minor.yy53));
	yylhsminor.yy43->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1226 "cmdl/grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy43 = yylhsminor.yy43;
        break;
      case 6: /* sum ::= sum PLUS expression */
#line 204 "cmdl/grammar.y"
{
	yylhsminor.yy43 = yymsp[-2].minor.yy43;
	yylhsminor.yy43->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1236 "cmdl/grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy43 = yylhsminor.yy43;
        break;
      case 7: /* sum ::= sum MINUS expression */
#line 208 "cmdl/grammar.y"
{
	yylhsminor.yy43 = yymsp[-2].minor.yy43;
	yylhsminor.yy43->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1246 "cmdl/grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy43 = yylhsminor.yy43;
        break;
      case 8: /* expression ::= product */
#line 220 "cmdl/grammar.y"
{
	yylhsminor.yy53 = yymsp[0].minor.yy47;
}
#line 1255 "cmdl/grammar.c"
  yymsp[0].minor.yy53 = yylhsminor.yy53;
        break;
      case 9: /* product ::= expression MULTIPLY expression */
#line 223 "cmdl/grammar.y"
{
	yylhsminor.yy47 = new product_expression();
	yylhsminor.yy47->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy53));
	yylhsminor.yy47->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));

}
#line 1266 "cmdl/grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy47 = yylhsminor.yy47;
        break;
      case 10: /* product ::= expression DIVIDE expression */
#line 229 "cmdl/grammar.y"
{
	yylhsminor.yy47 = new product_expression();
	yylhsminor.yy47->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy53));
	yylhsminor.yy47->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));

}
#line 1278 "cmdl/grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy47 = yylhsminor.yy47;
        break;
      case 11: /* product ::= product MULTIPLY expression */
#line 235 "cmdl/grammar.y"
{
	yylhsminor.yy47 = yymsp[-2].minor.yy47;
	yylhsminor.yy47->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1288 "cmdl/grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy47 = yylhsminor.yy47;
        break;
      case 12: /* product ::= product DIVIDE expression */
#line 239 "cmdl/grammar.y"
{
	yylhsminor.yy47 = yymsp[-2].minor.yy47;
	yylhsminor.yy47->push_back(true, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1298 "cmdl/grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy47 = yylhsminor.yy47;
        break;
      case 13: /* expression ::= conjunction */
#line 250 "cmdl/grammar.y"
{
	yylhsminor.yy53 = yymsp[0].minor.yy4;
}
#line 1307 "cmdl/grammar.c"
  yymsp[0].minor.yy53 = yylhsminor.yy53;
        break;
      case 14: /* conjunction ::= expression AND expression */
#line 253 "cmdl/grammar.y"
{
	yylhsminor.yy4 = new conjunction_expression();
	yylhsminor.yy4->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy53));
	yylhsminor.yy4->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));

}
#line 1318 "cmdl/grammar.c"
  yy_destructor(yypParser,2,&yymsp[-1].minor);
  yymsp[-2].minor.yy4 = yylhsminor.yy4;
        break;
      case 15: /* conjunction ::= conjunction AND expression */
#line 259 "cmdl/grammar.y"
{
	yylhsminor.yy4 = yymsp[-2].minor.yy4;
	yylhsminor.yy4->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1328 "cmdl/grammar.c"
  yy_destructor(yypParser,2,&yymsp[-1].minor);
  yymsp[-2].minor.yy4 = yylhsminor.yy4;
        break;
      case 16: /* expression ::= disjunction */
#line 270 "cmdl/grammar.y"
{
	yylhsminor.yy53 = yymsp[0].minor.yy72;
}
#line 1337 "cmdl/grammar.c"
  yymsp[0].minor.yy53 = yylhsminor.yy53;
        break;
      case 17: /* disjunction ::= expression OR expression */
#line 273 "cmdl/grammar.y"
{
	yylhsminor.yy72 = new disjunction_expression();
	yylhsminor.yy72->push_back(false, std::unique_ptr<expression_base>(yymsp[-2].minor.yy53));
	yylhsminor.yy72->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));

}
#line 1348 "cmdl/grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy72 = yylhsminor.yy72;
        break;
      case 18: /* disjunction ::= disjunction OR expression */
#line 279 "cmdl/grammar.y"
{
	yylhsminor.yy72 = yymsp[-2].minor.yy72;
	yylhsminor.yy72->push_back(false, std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1358 "cmdl/grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy72 = yylhsminor.yy72;
        break;
      case 19: /* expression ::= NOT expression */
{  yy_destructor(yypParser,15,&yymsp[-1].minor);
#line 285 "cmdl/grammar.y"
{
	yymsp[-1].minor.yy53 = new unary_not_expression(std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1368 "cmdl/grammar.c"
}
        break;
      case 20: /* expression ::= MINUS expression */
{  yy_destructor(yypParser,11,&yymsp[-1].minor);
#line 290 "cmdl/grammar.y"
{
	yymsp[-1].minor.yy53 = new unary_minus_expression(std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1377 "cmdl/grammar.c"
}
        break;
      case 21: /* expression ::= expression EXP expression */
#line 295 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new exponentiation_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy53), std::unique_ptr<expression_base>(yymsp[0].minor.yy53));
}
#line 1385 "cmdl/grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy53 = yylhsminor.yy53;
        break;
      case 22: /* expression ::= expression EQUAL expression */
#line 301 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy53), std::unique_ptr<expression_base>(yymsp[0].minor.yy53), comparison_expression::type_equal);
}
#line 1394 "cmdl/grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy53 = yylhsminor.yy53;
        break;
      case 23: /* expression ::= expression NOT_EQUAL expression */
#line 304 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy53), std::unique_ptr<expression_base>(yymsp[0].minor.yy53), comparison_expression::type_not_equal);
}
#line 1403 "cmdl/grammar.c"
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy53 = yylhsminor.yy53;
        break;
      case 24: /* expression ::= expression GREATER expression */
#line 307 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy53), std::unique_ptr<expression_base>(yymsp[0].minor.yy53), comparison_expression::type_greater);
}
#line 1412 "cmdl/grammar.c"
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy53 = yylhsminor.yy53;
        break;
      case 25: /* expression ::= expression GREATER_EQUAL expression */
#line 310 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy53), std::unique_ptr<expression_base>(yymsp[0].minor.yy53), comparison_expression::type_greater_equal);
}
#line 1421 "cmdl/grammar.c"
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy53 = yylhsminor.yy53;
        break;
      case 26: /* expression ::= expression LESS expression */
#line 313 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy53), std::unique_ptr<expression_base>(yymsp[0].minor.yy53), comparison_expression::type_less);
}
#line 1430 "cmdl/grammar.c"
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy53 = yylhsminor.yy53;
        break;
      case 27: /* expression ::= expression LESS_EQUAL expression */
#line 316 "cmdl/grammar.y"
{
	yylhsminor.yy53 = new comparison_expression(std::unique_ptr<expression_base>(yymsp[-2].minor.yy53), std::unique_ptr<expression_base>(yymsp[0].minor.yy53), comparison_expression::type_less_equal);
}
#line 1439 "cmdl/grammar.c"
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy53 = yylhsminor.yy53;
        break;
      case 28: /* assignment ::= IDENTIFIER ASSIGN expression SEMICOLON */
#line 321 "cmdl/grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-3].minor.yy0 and yymsp[-1].minor.yy53 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-1].minor.yy53);
	yymsp[-1].minor.yy53 = nullptr;

	parseTree->create_variable(std::move(name), parseTree->get_expression_value(e_temp.get()));
}
#line 1456 "cmdl/grammar.c"
  yy_destructor(yypParser,20,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 29: /* assignment ::= IDENTIFIER ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 332 "cmdl/grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy0 and yymsp[-2].minor.yy53 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-5].minor.yy0;
	delete yymsp[-5].minor.yy0;
	yymsp[-5].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-2].minor.yy53);
	yymsp[-2].minor.yy53 = nullptr;

	parseTree->create_variable(std::move(name), std::move(e_temp));
}
#line 1473 "cmdl/grammar.c"
  yy_destructor(yypParser,20,&yymsp[-4].minor);
  yy_destructor(yypParser,21,&yymsp[-3].minor);
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 30: /* reaction ::= reactionSide ARROW reactionSide COMMA expression SEMICOLON */
#line 346 "cmdl/grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy40, yymsp[-3].minor.yy40 and yymsp[-1].minor.yy53 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<reaction_side>(yymsp[-5].minor.yy40);
	auto products_temp = std::unique_ptr<reaction_side>(yymsp[-3].minor.yy40);
	auto e_temp = yymsp[-1].minor.yy53;
	yymsp[-5].minor.yy40 = nullptr;
	yymsp[-3].minor.yy40 = nullptr;
	yymsp[-1].minor.yy53 = nullptr;

	parseTree->create_reaction(std::move(reactants_temp), std::move(products_temp), parseTree->get_expression_value(e_temp));
	delete e_temp;
	e_temp = nullptr;
}
#line 1495 "cmdl/grammar.c"
  yy_destructor(yypParser,23,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 31: /* reaction ::= reactionSide ARROW reactionSide COMMA LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 360 "cmdl/grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-7].minor.yy40, yymsp[-5].minor.yy40 and yymsp[-2].minor.yy53 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<reaction_side>(yymsp[-7].minor.yy40);
	auto products_temp = std::unique_ptr<reaction_side>(yymsp[-5].minor.yy40);
	auto e_temp = yymsp[-2].minor.yy53;
	yymsp[-7].minor.yy40 = nullptr;
	yymsp[-5].minor.yy40 = nullptr;
	yymsp[-2].minor.yy53 = nullptr;

	parseTree->create_reaction(std::move(reactants_temp), std::move(products_temp), std::unique_ptr<expression_base>(e_temp));
}
#line 1514 "cmdl/grammar.c"
  yy_destructor(yypParser,23,&yymsp[-6].minor);
  yy_destructor(yypParser,24,&yymsp[-4].minor);
  yy_destructor(yypParser,21,&yymsp[-3].minor);
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 32: /* reactionSide ::= */
#line 378 "cmdl/grammar.y"
{
	yymsp[1].minor.yy40 = new reaction_side();
}
#line 1526 "cmdl/grammar.c"
        break;
      case 33: /* reactionSide ::= reactionComponent */
#line 381 "cmdl/grammar.y"
{
	auto rc_temp = std::unique_ptr<reaction_component>(yymsp[0].minor.yy30);
	yymsp[0].minor.yy30 = nullptr;

	yylhsminor.yy40 = new reaction_side();
	yylhsminor.yy40->push_back(std::move(rc_temp));
}
#line 1537 "cmdl/grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 34: /* reactionSide ::= reactionSide PLUS reactionComponent */
#line 388 "cmdl/grammar.y"
{
	yylhsminor.yy40 = yymsp[-2].minor.yy40;
	yymsp[-2].minor.yy40 = nullptr;
	auto rc_temp = std::unique_ptr<reaction_component>(yymsp[0].minor.yy30);
	yymsp[0].minor.yy30 = nullptr;

	yylhsminor.yy40->push_back(std::move(rc_temp));
}
#line 1550 "cmdl/grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 35: /* reactionSide ::= expression */
#line 397 "cmdl/grammar.y"
{
	delete(yymsp[0].minor.yy53);
	yymsp[0].minor.yy53=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1561 "cmdl/grammar.c"
        break;
      case 36: /* reactionSide ::= reactionSide PLUS expression */
#line 402 "cmdl/grammar.y"
{
	delete(yymsp[0].minor.yy53);
	yymsp[0].minor.yy53=nullptr;
	delete(yymsp[-2].minor.yy40);
	yymsp[-2].minor.yy40=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1572 "cmdl/grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
        break;
      case 37: /* reactionComponent ::= IDENTIFIER */
#line 415 "cmdl/grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	yylhsminor.yy30 = nullptr;

	yylhsminor.yy30 = new reaction_component(state, 1, false);
}
#line 1585 "cmdl/grammar.c"
  yymsp[0].minor.yy30 = yylhsminor.yy30;
        break;
      case 38: /* reactionComponent ::= IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 424 "cmdl/grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	yylhsminor.yy30 = nullptr;

	yylhsminor.yy30 = new reaction_component(state, 1, true);
}
#line 1598 "cmdl/grammar.c"
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yy_destructor(yypParser,22,&yymsp[0].minor);
  yymsp[-2].minor.yy30 = yylhsminor.yy30;
        break;
      case 39: /* reactionComponent ::= expression MULTIPLY IDENTIFIER */
#line 433 "cmdl/grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-2].minor.yy53);
	yymsp[-2].minor.yy53 = nullptr;
	yylhsminor.yy30 = nullptr;

	auto stochiometry = parseTree->get_expression_value(e_temp.get());
	yylhsminor.yy30 = new reaction_component(state, stochiometry, false);
}
#line 1616 "cmdl/grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy30 = yylhsminor.yy30;
        break;
      case 40: /* reactionComponent ::= expression MULTIPLY IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 445 "cmdl/grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<expression_base>(yymsp[-4].minor.yy53);
	yymsp[-4].minor.yy53 = nullptr;
	yylhsminor.yy30 = nullptr;

	auto stochiometry = parseTree->get_expression_value(e_temp.get());
	yylhsminor.yy30 = new reaction_component(state, stochiometry, true);
}
#line 1633 "cmdl/grammar.c"
  yy_destructor(yypParser,12,&yymsp[-3].minor);
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yy_destructor(yypParser,22,&yymsp[0].minor);
  yymsp[-4].minor.yy30 = yylhsminor.yy30;
        break;
      default:
      /* (41) model ::= statements */ yytestcase(yyruleno==41);
      /* (42) statements ::= statements statement */ yytestcase(yyruleno==42);
      /* (43) statements ::= */ yytestcase(yyruleno==43);
      /* (44) statement ::= assignment (OPTIMIZED OUT) */ assert(yyruleno!=44);
      /* (45) statement ::= reaction (OPTIMIZED OUT) */ assert(yyruleno!=45);
      /* (46) statement ::= error (OPTIMIZED OUT) */ assert(yyruleno!=46);
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
#line 1692 "cmdl/grammar.c"
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