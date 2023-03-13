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
#line 14 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "expression_common.h"
#line 15 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "ComparisonExpression.h"
#line 16 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "ConditionalExpression.h"
#line 17 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "ExponentiationExpression.h"
#line 18 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "ConjunctionExpression.h"
#line 19 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "DisjunctionExpression.h"
#line 20 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "UnaryNotExpression.h"
#line 21 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "NumberExpression.h"
#line 22 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "ProductExpression.h"
#line 23 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "SumExpression.h"
#line 24 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "UnaryMinusExpression.h"
#line 25 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "VariableExpression.h"
#line 26 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "FunctionExpression.h"
#line 28 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "expression_symbols.h"
#line 29 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "ExpressionParseTree.h"
#line 30 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include "ExpressionParser.h"
#line 32 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
#include  <assert.h>
#line 33 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
using namespace expression;
#line 64 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
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
**    expression_ParseTOKENTYPE     is the data type used for minor type for terminal
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
**                       which is expression_ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    expression_ParseARG_SDECL     A static variable declaration for the %extra_argument
**    expression_ParseARG_PDECL     A parameter declaration for the %extra_argument
**    expression_ParseARG_STORE     Code to store %extra_argument into yypParser
**    expression_ParseARG_FETCH     Code to extract %extra_argument from yypParser
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
#define YYNOCODE 33
#define YYACTIONTYPE unsigned char
#define expression_ParseTOKENTYPE TerminalSymbol*
typedef union {
  int yyinit;
  expression_ParseTOKENTYPE yy0;
  SumExpression* yy8;
  ConjunctionExpression* yy13;
  DisjunctionExpression* yy14;
  IExpression* yy16;
  ConditionalExpression* yy19;
  ProductExpression* yy20;
  FunctionArguments* yy43;
  int yy65;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define expression_ParseARG_SDECL ExpressionParseTree* parseTree;
#define expression_ParseARG_PDECL ,ExpressionParseTree* parseTree
#define expression_ParseARG_FETCH ExpressionParseTree* parseTree = yypParser->parseTree
#define expression_ParseARG_STORE yypParser->parseTree = parseTree
#define YYERRORSYMBOL 23
#define YYERRSYMDT yy65
#define YYNSTATE             56
#define YYNRULE              36
#define YY_MAX_SHIFT         55
#define YY_MIN_SHIFTREDUCE   65
#define YY_MAX_SHIFTREDUCE   100
#define YY_MIN_REDUCE        101
#define YY_MAX_REDUCE        136
#define YY_ERROR_ACTION      137
#define YY_ACCEPT_ACTION     138
#define YY_NO_ACTION         139
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
#define YY_ACTTAB_COUNT (332)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   135,   16,   24,   18,   17,   15,   14,   13,   12,   11,
 /*    10 */    10,   22,   21,   20,   19,   16,   24,   18,   17,   15,
 /*    20 */    14,   13,   12,   11,   10,   22,   21,   20,   19,   16,
 /*    30 */    20,   19,   16,  103,    1,   23,  103,  103,   24,   18,
 /*    40 */    17,   15,   14,   13,   12,   11,   10,   22,   21,   20,
 /*    50 */    19,   16,   66,  103,   26,  103,   68,  103,  103,   24,
 /*    60 */    18,   17,   15,   14,   13,   12,   11,   10,   22,   21,
 /*    70 */    20,   19,   16,   17,   15,   14,   13,   12,   11,   10,
 /*    80 */    22,   21,   20,   19,   16,   15,   14,   13,   12,   11,
 /*    90 */    10,   22,   21,   20,   19,   16,  137,  137,  137,  137,
 /*   100 */   137,  137,   22,   21,   20,   19,   16,  136,   27,  103,
 /*   110 */   106,  110,  115,  120,  123,  138,  136,   30,   47,  106,
 /*   120 */   110,  115,  120,  123,  136,   43,  103,  106,  110,  115,
 /*   130 */   120,  123,  136,   48,  103,  106,  110,  115,  120,  123,
 /*   140 */   136,   49,  103,  106,  110,  115,  120,  123,  136,   50,
 /*   150 */   103,  106,  110,  115,  120,  123,  136,   35,  103,  106,
 /*   160 */   110,  115,  120,  123,  136,   33,  103,  106,  110,  115,
 /*   170 */   120,  123,  136,   51,  103,  106,  110,  115,  120,  123,
 /*   180 */   136,   44,  103,  106,  110,  115,  120,  123,  136,   36,
 /*   190 */   103,  106,  110,  115,  120,  123,  136,   37,  103,  106,
 /*   200 */   110,  115,  120,  123,  136,   38,  103,  106,  110,  115,
 /*   210 */   120,  123,  136,   39,  103,  106,  110,  115,  120,  123,
 /*   220 */   136,   40,  103,  106,  110,  115,  120,  123,  136,   41,
 /*   230 */   103,  106,  110,  115,  120,  123,  136,   52,  103,  106,
 /*   240 */   110,  115,  120,  123,  136,   42,  103,  106,  110,  115,
 /*   250 */   120,  123,  136,   34,  103,  106,  110,  115,  120,  123,
 /*   260 */   136,   53,  103,  106,  110,  115,  120,  123,  136,   54,
 /*   270 */   103,  106,  110,  115,  120,  123,  136,   45,  103,  106,
 /*   280 */   110,  115,  120,  123,  136,   46,  103,  106,  110,  115,
 /*   290 */   120,  123,  136,   32,  103,  106,  110,  115,  120,  123,
 /*   300 */   136,   28,  103,  106,  110,  115,  120,  123,  136,   29,
 /*   310 */   103,  106,  110,  115,  120,  123,  136,   31,  103,  106,
 /*   320 */   110,  115,  120,  123,    4,  103,  103,  103,    5,   55,
 /*   330 */    67,   25,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     0,   15,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    10 */    10,   11,   12,   13,   14,   15,    2,    3,    4,    5,
 /*    20 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*    30 */    13,   14,   15,   32,   19,   21,   32,   32,    2,    3,
 /*    40 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*    50 */    14,   15,   20,   32,   22,   32,   20,   32,   32,    2,
 /*    60 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*    70 */    13,   14,   15,    4,    5,    6,    7,    8,    9,   10,
 /*    80 */    11,   12,   13,   14,   15,    5,    6,    7,    8,    9,
 /*    90 */    10,   11,   12,   13,   14,   15,    5,    6,    7,    8,
 /*   100 */     9,   10,   11,   12,   13,   14,   15,   23,   24,   32,
 /*   110 */    26,   27,   28,   29,   30,   31,   23,   24,   25,   26,
 /*   120 */    27,   28,   29,   30,   23,   24,   32,   26,   27,   28,
 /*   130 */    29,   30,   23,   24,   32,   26,   27,   28,   29,   30,
 /*   140 */    23,   24,   32,   26,   27,   28,   29,   30,   23,   24,
 /*   150 */    32,   26,   27,   28,   29,   30,   23,   24,   32,   26,
 /*   160 */    27,   28,   29,   30,   23,   24,   32,   26,   27,   28,
 /*   170 */    29,   30,   23,   24,   32,   26,   27,   28,   29,   30,
 /*   180 */    23,   24,   32,   26,   27,   28,   29,   30,   23,   24,
 /*   190 */    32,   26,   27,   28,   29,   30,   23,   24,   32,   26,
 /*   200 */    27,   28,   29,   30,   23,   24,   32,   26,   27,   28,
 /*   210 */    29,   30,   23,   24,   32,   26,   27,   28,   29,   30,
 /*   220 */    23,   24,   32,   26,   27,   28,   29,   30,   23,   24,
 /*   230 */    32,   26,   27,   28,   29,   30,   23,   24,   32,   26,
 /*   240 */    27,   28,   29,   30,   23,   24,   32,   26,   27,   28,
 /*   250 */    29,   30,   23,   24,   32,   26,   27,   28,   29,   30,
 /*   260 */    23,   24,   32,   26,   27,   28,   29,   30,   23,   24,
 /*   270 */    32,   26,   27,   28,   29,   30,   23,   24,   32,   26,
 /*   280 */    27,   28,   29,   30,   23,   24,   32,   26,   27,   28,
 /*   290 */    29,   30,   23,   24,   32,   26,   27,   28,   29,   30,
 /*   300 */    23,   24,   32,   26,   27,   28,   29,   30,   23,   24,
 /*   310 */    32,   26,   27,   28,   29,   30,   23,   24,   32,   26,
 /*   320 */    27,   28,   29,   30,   12,   32,   32,   32,   16,   17,
 /*   330 */    18,   19,
};
#define YY_SHIFT_USE_DFLT (332)
#define YY_SHIFT_COUNT    (55)
#define YY_SHIFT_MIN      (-14)
#define YY_SHIFT_MAX      (312)
static const short yy_shift_ofst[] = {
 /*     0 */   312,  312,  312,  312,  312,  312,  312,  312,  312,  312,
 /*    10 */   312,  312,  312,  312,  312,  312,  312,  312,  312,  312,
 /*    20 */   312,  312,  312,  312,  312,  312,  312,    0,   14,   36,
 /*    30 */    57,   57,   57,   69,   69,   80,   91,   91,   91,   91,
 /*    40 */    91,   91,   80,   17,   17,   17,   17,   32,  -14,  -14,
 /*    50 */   -14,  -14,  -14,  -14,  -14,   15,
};
#define YY_REDUCE_USE_DFLT (-1)
#define YY_REDUCE_COUNT (26)
#define YY_REDUCE_MIN   (0)
#define YY_REDUCE_MAX   (293)
static const short yy_reduce_ofst[] = {
 /*     0 */    84,   93,  101,  109,  117,  125,  133,  141,  149,  157,
 /*    10 */   165,  173,  181,  189,  197,  205,  213,  221,  229,  237,
 /*    20 */   245,  253,  261,  269,  277,  285,  293,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   137,  107,  137,  137,  137,  137,  137,  137,  137,  137,
 /*    10 */   137,  137,  137,  137,  137,  137,  137,  137,  137,  137,
 /*    20 */   137,  137,  137,  137,  137,  137,  137,  137,  137,  137,
 /*    30 */   108,  109,  105,  122,  121,  125,  134,  133,  132,  131,
 /*    40 */   130,  129,  124,  114,  113,  112,  111,  137,  119,  127,
 /*    50 */   126,  118,  128,  117,  116,  101,
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
  expression_ParseARG_SDECL                /* A place to hold %extra_argument */
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
void expression_ParseTrace(FILE *TraceFILE, char *zTracePrompt){
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
  "RIGHT_ROUND",   "COLON",         "COMMA",         "error",       
  "expression",    "arguments",     "comparison",    "sum",         
  "product",       "conjunction",   "disjunction",   "result",      
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
 /*  34 */ "result ::= expression",
 /*  35 */ "expression ::= error",
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
** second argument to expression_ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
void expression_ParseInit(void *yypParser){
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

#ifndef expression_Parse_ENGINEALWAYSONSTACK
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
** to expression_Parse and expression_ParseFree.
*/
void *expression_ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ) expression_ParseInit(pParser);
  return pParser;
}
#endif /* expression_Parse_ENGINEALWAYSONSTACK */


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
  expression_ParseARG_FETCH;
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
{
#line 8 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"

	delete (yypminor->yy0);
	(yypminor->yy0) = nullptr;

#line 635 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
      break;
    case 24: /* expression */
{
#line 82 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
 
	delete (yypminor->yy16);
	(yypminor->yy16) = nullptr;

#line 645 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
      break;
    case 25: /* arguments */
{
#line 132 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
 
	delete (yypminor->yy43);
	(yypminor->yy43) = nullptr;

#line 655 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
      break;
    case 26: /* comparison */
{
#line 116 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
 
	delete (yypminor->yy19);
	(yypminor->yy19) = nullptr;

#line 665 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
      break;
    case 27: /* sum */
{
#line 153 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
 
	delete (yypminor->yy8);
	(yypminor->yy8) = nullptr;

#line 675 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
      break;
    case 28: /* product */
{
#line 182 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
 
	delete (yypminor->yy20);
	(yypminor->yy20) = nullptr;

#line 685 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
      break;
    case 29: /* conjunction */
{
#line 212 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
 
	delete (yypminor->yy13);
	(yypminor->yy13) = nullptr;

#line 695 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
      break;
    case 30: /* disjunction */
{
#line 232 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
 
	delete (yypminor->yy14);
	(yypminor->yy14) = nullptr;

#line 705 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
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
void expression_ParseFinalize(void *p){
  yyParser *pParser = (yyParser*)p;
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
}

#ifndef expression_Parse_ENGINEALWAYSONSTACK
/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void expression_ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  expression_ParseFinalize(p);
  (*freeProc)(p);
}
#endif /* expression_Parse_ENGINEALWAYSONSTACK */

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int expression_ParseStackPeak(void *p){
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
   expression_ParseARG_FETCH;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
#line 5 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
throw std::runtime_error("Parser stack overflow while parsing expression.");
#line 886 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
/******** End %stack_overflow code ********************************************/
   expression_ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
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
  expression_ParseTOKENTYPE yyMinor        /* The minor token to shift in */
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
  { 24, -1 },
  { 24, -4 },
  { 24, -1 },
  { 24, -3 },
  { 26, -5 },
  { 24, -1 },
  { 25, 0 },
  { 25, -1 },
  { 25, -3 },
  { 24, -1 },
  { 27, -3 },
  { 27, -3 },
  { 27, -3 },
  { 27, -3 },
  { 24, -1 },
  { 28, -3 },
  { 28, -3 },
  { 28, -3 },
  { 28, -3 },
  { 24, -1 },
  { 29, -3 },
  { 29, -3 },
  { 24, -1 },
  { 30, -3 },
  { 30, -3 },
  { 24, -2 },
  { 24, -2 },
  { 24, -3 },
  { 24, -3 },
  { 24, -3 },
  { 24, -3 },
  { 24, -3 },
  { 24, -3 },
  { 24, -3 },
  { 31, -1 },
  { 24, -1 },
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
  expression_ParseARG_FETCH;
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
#line 86 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new VariableExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1066 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy16 = yylhsminor.yy16;
        break;
      case 1: /* expression ::= IDENTIFIER LEFT_ROUND arguments RIGHT_ROUND */
#line 91 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	auto func = new FunctionExpression(*yymsp[-3].minor.yy0);
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yylhsminor.yy16 = nullptr;
	for(auto& argument : *yymsp[-1].minor.yy43)
	{
		func->PushBack(std::move(argument));
	}
	delete yymsp[-1].minor.yy43;
	yymsp[-1].minor.yy43 = nullptr;
	yylhsminor.yy16 = func;
	func = nullptr;
}
#line 1085 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,20,&yymsp[0].minor);
  yymsp[-3].minor.yy16 = yylhsminor.yy16;
        break;
      case 2: /* expression ::= VALUE */
#line 106 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new NumberExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1097 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy16 = yylhsminor.yy16;
        break;
      case 3: /* expression ::= LEFT_ROUND expression RIGHT_ROUND */
{  yy_destructor(yypParser,19,&yymsp[-2].minor);
#line 111 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yymsp[-2].minor.yy16 = yymsp[-1].minor.yy16;
}
#line 1106 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
        break;
      case 4: /* comparison ::= expression QUESTIONMARK expression COLON expression */
#line 120 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy19 = new ConditionalExpression(std::unique_ptr<IExpression>(yymsp[-4].minor.yy16), std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
	yymsp[-2].minor.yy16 = nullptr;
	yymsp[0].minor.yy16 = nullptr;
	yymsp[-4].minor.yy16 = nullptr;
}
#line 1118 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-4].minor.yy19 = yylhsminor.yy19;
        break;
      case 5: /* expression ::= comparison */
#line 126 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = yymsp[0].minor.yy19;
}
#line 1128 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy16 = yylhsminor.yy16;
        break;
      case 6: /* arguments ::= */
#line 136 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yymsp[1].minor.yy43 = new FunctionArguments();
}
#line 1136 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
        break;
      case 7: /* arguments ::= expression */
#line 139 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy43 = new FunctionArguments();
	yylhsminor.yy43->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy16));
	yymsp[0].minor.yy16 = nullptr;
}
#line 1145 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy43 = yylhsminor.yy43;
        break;
      case 8: /* arguments ::= arguments COMMA expression */
#line 144 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy43 = yymsp[-2].minor.yy43;
	yymsp[-2].minor.yy43 = nullptr;
	yylhsminor.yy43->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy16));
	yymsp[0].minor.yy16 = nullptr;
}
#line 1156 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy43 = yylhsminor.yy43;
        break;
      case 9: /* expression ::= sum */
#line 157 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = yymsp[0].minor.yy8;
}
#line 1165 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy16 = yylhsminor.yy16;
        break;
      case 10: /* sum ::= expression PLUS expression */
#line 160 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy8 = new SumExpression();
	yylhsminor.yy8->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy16));
	yylhsminor.yy8->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1175 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy8 = yylhsminor.yy8;
        break;
      case 11: /* sum ::= expression MINUS expression */
#line 165 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy8 = new SumExpression();
	yylhsminor.yy8->PushBack(false,  std::unique_ptr<IExpression>(yymsp[-2].minor.yy16));
	yylhsminor.yy8->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1186 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy8 = yylhsminor.yy8;
        break;
      case 12: /* sum ::= sum PLUS expression */
#line 170 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy8 = yymsp[-2].minor.yy8;
	yylhsminor.yy8->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1196 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy8 = yylhsminor.yy8;
        break;
      case 13: /* sum ::= sum MINUS expression */
#line 174 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy8 = yymsp[-2].minor.yy8;
	yylhsminor.yy8->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1206 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy8 = yylhsminor.yy8;
        break;
      case 14: /* expression ::= product */
#line 186 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = yymsp[0].minor.yy20;
}
#line 1215 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy16 = yylhsminor.yy16;
        break;
      case 15: /* product ::= expression MULTIPLY expression */
#line 189 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy20 = new ProductExpression();
	yylhsminor.yy20->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy16));
	yylhsminor.yy20->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));

}
#line 1226 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy20 = yylhsminor.yy20;
        break;
      case 16: /* product ::= expression DIVIDE expression */
#line 195 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy20 = new ProductExpression();
	yylhsminor.yy20->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy16));
	yylhsminor.yy20->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));

}
#line 1238 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy20 = yylhsminor.yy20;
        break;
      case 17: /* product ::= product MULTIPLY expression */
#line 201 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy20 = yymsp[-2].minor.yy20;
	yylhsminor.yy20->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1248 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy20 = yylhsminor.yy20;
        break;
      case 18: /* product ::= product DIVIDE expression */
#line 205 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy20 = yymsp[-2].minor.yy20;
	yylhsminor.yy20->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1258 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy20 = yylhsminor.yy20;
        break;
      case 19: /* expression ::= conjunction */
#line 216 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = yymsp[0].minor.yy13;
}
#line 1267 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy16 = yylhsminor.yy16;
        break;
      case 20: /* conjunction ::= expression AND expression */
#line 219 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy13 = new ConjunctionExpression();
	yylhsminor.yy13->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy16));
	yylhsminor.yy13->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));

}
#line 1278 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy13 = yylhsminor.yy13;
        break;
      case 21: /* conjunction ::= conjunction AND expression */
#line 225 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy13 = yymsp[-2].minor.yy13;
	yylhsminor.yy13->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1288 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy13 = yylhsminor.yy13;
        break;
      case 22: /* expression ::= disjunction */
#line 236 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = yymsp[0].minor.yy14;
}
#line 1297 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yymsp[0].minor.yy16 = yylhsminor.yy16;
        break;
      case 23: /* disjunction ::= expression OR expression */
#line 239 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy14 = new DisjunctionExpression();
	yylhsminor.yy14->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy16));
	yylhsminor.yy14->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));

}
#line 1308 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy14 = yylhsminor.yy14;
        break;
      case 24: /* disjunction ::= disjunction OR expression */
#line 245 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy14 = yymsp[-2].minor.yy14;
	yylhsminor.yy14->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1318 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy14 = yylhsminor.yy14;
        break;
      case 25: /* expression ::= NOT expression */
{  yy_destructor(yypParser,16,&yymsp[-1].minor);
#line 251 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yymsp[-1].minor.yy16 = new UnaryNotExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1328 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
        break;
      case 26: /* expression ::= MINUS expression */
{  yy_destructor(yypParser,12,&yymsp[-1].minor);
#line 256 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yymsp[-1].minor.yy16 = new UnaryMinusExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1337 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
}
        break;
      case 27: /* expression ::= expression EXP expression */
#line 261 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new ExponentiationExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
}
#line 1345 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,15,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 28: /* expression ::= expression EQUAL expression */
#line 267 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16), ComparisonExpression::type_equal);
}
#line 1354 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 29: /* expression ::= expression NOT_EQUAL expression */
#line 270 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16), ComparisonExpression::type_not_equal);
}
#line 1363 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 30: /* expression ::= expression GREATER expression */
#line 273 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16), ComparisonExpression::type_greater);
}
#line 1372 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 31: /* expression ::= expression GREATER_EQUAL expression */
#line 276 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16), ComparisonExpression::type_greater_equal);
}
#line 1381 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 32: /* expression ::= expression LESS expression */
#line 279 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16), ComparisonExpression::type_less);
}
#line 1390 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 33: /* expression ::= expression LESS_EQUAL expression */
#line 282 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	yylhsminor.yy16 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy16), std::unique_ptr<IExpression>(yymsp[0].minor.yy16), ComparisonExpression::type_less_equal);
}
#line 1399 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy16 = yylhsminor.yy16;
        break;
      case 34: /* result ::= expression */
#line 293 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	parseTree->SetResult(std::unique_ptr<IExpression>(yymsp[0].minor.yy16));
	yymsp[0].minor.yy16 = nullptr;
}
#line 1409 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
        break;
      case 35: /* expression ::= error */
#line 298 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
{
	throw std::runtime_error("Syntax error.");
}
#line 1416 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
        break;
      default:
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
  expression_ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
#line 4 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.y"
throw std::runtime_error("Syntax error while parsing expression.");
#line 1465 "/Users/antoineloussouarn/Desktop/stochsim/lib/expression/expression_grammar.c"
/************ End %parse_failure code *****************************************/
  expression_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  expression_ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  expression_ParseARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
/************ End %syntax_error code ******************************************/
  expression_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  expression_ParseARG_FETCH;
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
  expression_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "expression_ParseAlloc" which describes the current state of the parser.
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
void expression_Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  expression_ParseTOKENTYPE yyminor       /* The value for the token */
  expression_ParseARG_PDECL               /* Optional %extra_argument parameter */
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
  expression_ParseARG_STORE;

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