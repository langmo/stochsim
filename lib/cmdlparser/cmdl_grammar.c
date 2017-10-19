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
#line 14 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "expression_common.h"
#line 15 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "ComparisonExpression.h"
#line 16 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "ConditionalExpression.h"
#line 17 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "ExponentiationExpression.h"
#line 18 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "ConjunctionExpression.h"
#line 19 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "DisjunctionExpression.h"
#line 20 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "UnaryNotExpression.h"
#line 21 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "NumberExpression.h"
#line 22 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "ProductExpression.h"
#line 23 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "SumExpression.h"
#line 24 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "UnaryMinusExpression.h"
#line 25 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "VariableExpression.h"
#line 26 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "FunctionExpression.h"
#line 28 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "cmdl_symbols.h"
#line 29 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "CmdlParseTree.h"
#line 30 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include "CmdlParser.h"
#line 32 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
#include  <assert.h>
#line 33 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
using namespace expression;
#line 34 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
using namespace cmdlparser;
#line 66 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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
**    cmdl_internal_ParseTOKENTYPE     is the data type used for minor type for terminal
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
**                       which is cmdl_internal_ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    cmdl_internal_ParseARG_SDECL     A static variable declaration for the %extra_argument
**    cmdl_internal_ParseARG_PDECL     A parameter declaration for the %extra_argument
**    cmdl_internal_ParseARG_STORE     Code to store %extra_argument into yypParser
**    cmdl_internal_ParseARG_FETCH     Code to extract %extra_argument from yypParser
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
#define YYNOCODE 48
#define YYACTIONTYPE unsigned short int
#define cmdl_internal_ParseTOKENTYPE TerminalSymbol*
typedef union {
  int yyinit;
  cmdl_internal_ParseTOKENTYPE yy0;
  ReactionSpecifiers* yy5;
  ReactionSide* yy7;
  ConjunctionExpression* yy29;
  IExpression* yy40;
  ReactionSpecifier* yy44;
  ConditionalExpression* yy49;
  SumExpression* yy60;
  FunctionArguments* yy61;
  ReactionComponent* yy63;
  DisjunctionExpression* yy75;
  ProductExpression* yy84;
  int yy95;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define cmdl_internal_ParseARG_SDECL CmdlParseTree* parseTree;
#define cmdl_internal_ParseARG_PDECL ,CmdlParseTree* parseTree
#define cmdl_internal_ParseARG_FETCH CmdlParseTree* parseTree = yypParser->parseTree
#define cmdl_internal_ParseARG_STORE yypParser->parseTree = parseTree
#define YYERRORSYMBOL 29
#define YYERRSYMDT yy95
#define YYNSTATE             101
#define YYNRULE              62
#define YY_MAX_SHIFT         100
#define YY_MIN_SHIFTREDUCE   128
#define YY_MAX_SHIFTREDUCE   189
#define YY_MIN_REDUCE        190
#define YY_MAX_REDUCE        251
#define YY_ERROR_ACTION      252
#define YY_ACCEPT_ACTION     253
#define YY_NO_ACTION         254
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
#define YY_ACTTAB_COUNT (686)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    41,   35,   34,   32,   31,   30,   29,   28,   27,   17,
 /*    10 */    38,   16,   36,   33,   37,   36,   33,  253,    1,   40,
 /*    20 */   181,   86,   41,   35,   34,   32,   31,   30,   29,   28,
 /*    30 */    27,   39,   38,   37,   36,   33,   51,  182,  195,  199,
 /*    40 */   204,  209,  212,   88,  163,  170,   76,  228,   41,   35,
 /*    50 */    34,   32,   31,   30,   29,   28,   27,   39,   38,   37,
 /*    60 */    36,   33,   54,   85,  195,  199,  204,  209,  212,  179,
 /*    70 */    11,   92,   15,  162,   41,   35,   34,   32,   31,   30,
 /*    80 */    29,   28,   27,   39,   38,   37,   36,   33,   41,   35,
 /*    90 */    34,   32,   31,   30,   29,   28,   27,   39,   38,   37,
 /*   100 */    36,   33,  129,  164,   43,  165,  177,   40,   33,   11,
 /*   110 */    41,   35,   34,   32,   31,   30,   29,   28,   27,   39,
 /*   120 */    38,   37,   36,   33,   10,  192,   10,  192,  131,  192,
 /*   130 */   192,    5,   35,   34,   32,   31,   30,   29,   28,   27,
 /*   140 */    39,   38,   37,   36,   33,   41,   35,   34,   32,   31,
 /*   150 */    30,   29,   28,   27,   39,   38,   37,   36,   33,   41,
 /*   160 */    35,   34,   32,   31,   30,   29,   28,   27,   17,   38,
 /*   170 */    16,   36,   33,  246,   53,    9,  195,  199,  204,  209,
 /*   180 */   212,  246,  246,   78,  192,  192,  234,  246,  192,  180,
 /*   190 */   246,    9,    9,   41,   35,   34,   32,   31,   30,   29,
 /*   200 */    28,   27,    2,   38,   16,   36,   33,    3,   34,   32,
 /*   210 */    31,   30,   29,   28,   27,   39,   38,   37,   36,   33,
 /*   220 */    32,   31,   30,   29,   28,   27,   39,   38,   37,   36,
 /*   230 */    33,  252,  252,  252,  252,  252,  252,   39,   38,   37,
 /*   240 */    36,   33,  245,   53,  192,  195,  199,  204,  209,  212,
 /*   250 */     9,    9,   77,  192,   21,  234,  192,    9,   22,   71,
 /*   260 */   130,   42,    6,  192,  192,    7,   13,    4,  192,   89,
 /*   270 */    87,   53,  192,  195,  199,  204,  209,  212,  192,  192,
 /*   280 */    79,  192,  192,  234,  192,   44,  192,  195,  199,  204,
 /*   290 */   209,  212,  192,  192,   80,  192,  192,  234,  192,  192,
 /*   300 */    51,  192,  195,  199,  204,  209,  212,  192,  192,  192,
 /*   310 */    81,  228,  192,  192,   53,  192,  195,  199,  204,  209,
 /*   320 */   212,  192,  192,   83,  192,   53,  234,  195,  199,  204,
 /*   330 */   209,  212,  192,  192,   84,  192,  192,  234,  236,  192,
 /*   340 */    37,   36,   33,  192,  192,  192,  192,  192,  236,  236,
 /*   350 */   192,  192,  236,  236,   57,  192,  195,  199,  204,  209,
 /*   360 */   212,  192,  192,  192,  240,   51,  235,  195,  199,  204,
 /*   370 */   209,  212,   11,  192,  240,  240,  229,   90,  240,  240,
 /*   380 */   238,  192,  192,  192,  192,  192,  192,  192,   11,  192,
 /*   390 */   238,  238,   21,   91,  238,  238,   22,   82,  130,   42,
 /*   400 */   192,   21,  192,  192,   14,   22,   70,  130,   42,  192,
 /*   410 */    21,  192,  192,   13,   22,  100,  130,   42,  192,  192,
 /*   420 */   192,   47,   18,  195,  199,  204,  209,  212,  192,   50,
 /*   430 */   192,  195,  199,  204,  209,  212,   45,  192,  195,  199,
 /*   440 */   204,  209,  212,  192,   52,  192,  195,  199,  204,  209,
 /*   450 */   212,  192,   99,  192,  195,  199,  204,  209,  212,  192,
 /*   460 */    68,  192,  195,  199,  204,  209,  212,   46,  192,  195,
 /*   470 */   199,  204,  209,  212,  192,   72,  192,  195,  199,  204,
 /*   480 */   209,  212,  192,   93,  192,  195,  199,  204,  209,  212,
 /*   490 */   192,   94,  192,  195,  199,  204,  209,  212,   95,  192,
 /*   500 */   195,  199,  204,  209,  212,  192,   60,  192,  195,  199,
 /*   510 */   204,  209,  212,  192,   58,  192,  195,  199,  204,  209,
 /*   520 */   212,  192,   96,  192,  195,  199,  204,  209,  212,   73,
 /*   530 */   192,  195,  199,  204,  209,  212,  192,   61,  192,  195,
 /*   540 */   199,  204,  209,  212,  192,   62,  192,  195,  199,  204,
 /*   550 */   209,  212,  192,   63,  192,  195,  199,  204,  209,  212,
 /*   560 */    64,  192,  195,  199,  204,  209,  212,  192,   65,  192,
 /*   570 */   195,  199,  204,  209,  212,  192,   66,  192,  195,  199,
 /*   580 */   204,  209,  212,  192,   97,  192,  195,  199,  204,  209,
 /*   590 */   212,   67,  192,  195,  199,  204,  209,  212,  192,   59,
 /*   600 */   192,  195,  199,  204,  209,  212,  192,   98,  192,  195,
 /*   610 */   199,  204,  209,  212,  192,   74,  192,  195,  199,  204,
 /*   620 */   209,  212,   75,  192,  195,  199,  204,  209,  212,  192,
 /*   630 */    56,  192,  195,  199,  204,  209,  212,  192,   48,  192,
 /*   640 */   195,  199,  204,  209,  212,  192,   49,  192,  195,  199,
 /*   650 */   204,  209,  212,   55,  238,  195,  199,  204,  209,  212,
 /*   660 */   192,  192,   11,  192,  192,    8,   12,   91,   21,  238,
 /*   670 */   192,  192,   22,  100,  130,   42,  192,  192,   21,  192,
 /*   680 */   192,  192,   22,   69,  130,   42,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    10 */    12,   13,   14,   15,   13,   14,   15,   44,   45,   21,
 /*    20 */     1,   17,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    30 */    10,   11,   12,   13,   14,   15,   30,    1,   32,   33,
 /*    40 */    34,   35,   36,   17,    1,   25,   40,   41,    2,    3,
 /*    50 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*    60 */    14,   15,   30,   31,   32,   33,   34,   35,   36,   25,
 /*    70 */    19,   25,   21,    1,    2,    3,    4,    5,    6,    7,
 /*    80 */     8,    9,   10,   11,   12,   13,   14,   15,    2,    3,
 /*    90 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   100 */    14,   15,   20,    1,   22,    1,   25,   21,   15,   19,
 /*   110 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   120 */    12,   13,   14,   15,   22,   47,   22,   47,   20,   47,
 /*   130 */    47,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*   140 */    11,   12,   13,   14,   15,    2,    3,    4,    5,    6,
 /*   150 */     7,    8,    9,   10,   11,   12,   13,   14,   15,    2,
 /*   160 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   170 */    13,   14,   15,   29,   30,   11,   32,   33,   34,   35,
 /*   180 */    36,   37,   38,   39,   47,   47,   42,   43,   47,   25,
 /*   190 */    46,   11,   11,    2,    3,    4,    5,    6,    7,    8,
 /*   200 */     9,   10,   22,   12,   13,   14,   15,   26,    4,    5,
 /*   210 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   220 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   230 */    15,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   240 */    14,   15,    0,   30,   47,   32,   33,   34,   35,   36,
 /*   250 */    11,   11,   39,   47,   12,   42,   47,   11,   16,   17,
 /*   260 */    18,   19,   22,   47,   47,   26,   24,   21,   47,   27,
 /*   270 */    28,   30,   47,   32,   33,   34,   35,   36,   47,   47,
 /*   280 */    39,   47,   47,   42,   47,   30,   47,   32,   33,   34,
 /*   290 */    35,   36,   47,   47,   39,   47,   47,   42,   47,   47,
 /*   300 */    30,   47,   32,   33,   34,   35,   36,   47,   47,   47,
 /*   310 */    40,   41,   47,   47,   30,   47,   32,   33,   34,   35,
 /*   320 */    36,   47,   47,   39,   47,   30,   42,   32,   33,   34,
 /*   330 */    35,   36,   47,   47,   39,   47,   47,   42,   11,   47,
 /*   340 */    13,   14,   15,   47,   47,   47,   47,   47,   21,   22,
 /*   350 */    47,   47,   25,   26,   30,   47,   32,   33,   34,   35,
 /*   360 */    36,   47,   47,   47,   11,   30,   42,   32,   33,   34,
 /*   370 */    35,   36,   19,   47,   21,   22,   41,   24,   25,   26,
 /*   380 */    11,   47,   47,   47,   47,   47,   47,   47,   19,   47,
 /*   390 */    21,   22,   12,   24,   25,   26,   16,   17,   18,   19,
 /*   400 */    47,   12,   47,   47,   24,   16,   17,   18,   19,   47,
 /*   410 */    12,   47,   47,   24,   16,   17,   18,   19,   47,   47,
 /*   420 */    47,   30,   24,   32,   33,   34,   35,   36,   47,   30,
 /*   430 */    47,   32,   33,   34,   35,   36,   30,   47,   32,   33,
 /*   440 */    34,   35,   36,   47,   30,   47,   32,   33,   34,   35,
 /*   450 */    36,   47,   30,   47,   32,   33,   34,   35,   36,   47,
 /*   460 */    30,   47,   32,   33,   34,   35,   36,   30,   47,   32,
 /*   470 */    33,   34,   35,   36,   47,   30,   47,   32,   33,   34,
 /*   480 */    35,   36,   47,   30,   47,   32,   33,   34,   35,   36,
 /*   490 */    47,   30,   47,   32,   33,   34,   35,   36,   30,   47,
 /*   500 */    32,   33,   34,   35,   36,   47,   30,   47,   32,   33,
 /*   510 */    34,   35,   36,   47,   30,   47,   32,   33,   34,   35,
 /*   520 */    36,   47,   30,   47,   32,   33,   34,   35,   36,   30,
 /*   530 */    47,   32,   33,   34,   35,   36,   47,   30,   47,   32,
 /*   540 */    33,   34,   35,   36,   47,   30,   47,   32,   33,   34,
 /*   550 */    35,   36,   47,   30,   47,   32,   33,   34,   35,   36,
 /*   560 */    30,   47,   32,   33,   34,   35,   36,   47,   30,   47,
 /*   570 */    32,   33,   34,   35,   36,   47,   30,   47,   32,   33,
 /*   580 */    34,   35,   36,   47,   30,   47,   32,   33,   34,   35,
 /*   590 */    36,   30,   47,   32,   33,   34,   35,   36,   47,   30,
 /*   600 */    47,   32,   33,   34,   35,   36,   47,   30,   47,   32,
 /*   610 */    33,   34,   35,   36,   47,   30,   47,   32,   33,   34,
 /*   620 */    35,   36,   30,   47,   32,   33,   34,   35,   36,   47,
 /*   630 */    30,   47,   32,   33,   34,   35,   36,   47,   30,   47,
 /*   640 */    32,   33,   34,   35,   36,   47,   30,   47,   32,   33,
 /*   650 */    34,   35,   36,   30,   11,   32,   33,   34,   35,   36,
 /*   660 */    47,   47,   19,   47,   47,   22,   23,   24,   12,   26,
 /*   670 */    47,   47,   16,   17,   18,   19,   47,   47,   12,   47,
 /*   680 */    47,   47,   16,   17,   18,   19,
};
#define YY_SHIFT_USE_DFLT (686)
#define YY_SHIFT_COUNT    (100)
#define YY_SHIFT_MIN      (-2)
#define YY_SHIFT_MAX      (666)
static const short yy_shift_ofst[] = {
 /*     0 */   686,  242,  380,  389,  389,  389,  380,  389,  389,  389,
 /*    10 */   380,  656,  398,  656,  656,  656,  666,  656,  656,  656,
 /*    20 */   656,  656,  656,  656,  656,  656,  656,  656,  656,  656,
 /*    30 */   656,  656,  656,  656,  656,  656,  656,  656,  656,  656,
 /*    40 */   656,  656,  656,  656,   -2,   20,   46,   72,   86,  108,
 /*    50 */   129,  143,  143,  157,  143,  143,  143,  191,  204,  204,
 /*    60 */   215,  226,  226,  226,  226,  226,  226,  215,  327,  353,
 /*    70 */   369,  643,    1,    1,    1,    1,  102,  180,  181,  164,
 /*    80 */   246,  104,   51,  240,  239,   82,   19,    4,   36,   26,
 /*    90 */    44,   81,   43,   93,   93,   93,   93,   93,   93,   93,
 /*   100 */    90,
};
#define YY_REDUCE_USE_DFLT (-28)
#define YY_REDUCE_COUNT (43)
#define YY_REDUCE_MIN   (-27)
#define YY_REDUCE_MAX   (623)
static const short yy_reduce_ofst[] = {
 /*     0 */   -27,  144,    6,  213,  241,  255,  270,  284,  295,  324,
 /*    10 */   335,   32,  391,  399,  406,  414,  422,  430,  437,  445,
 /*    20 */   453,  461,  468,  476,  484,  492,  499,  507,  515,  523,
 /*    30 */   530,  538,  546,  554,  561,  569,  577,  422,  585,  592,
 /*    40 */   600,  608,  616,  623,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   247,  233,  252,  233,  233,  233,  252,  233,  233,  252,
 /*    10 */   252,  196,  252,  252,  252,  252,  252,  252,  252,  252,
 /*    20 */   252,  252,  252,  252,  252,  252,  252,  252,  252,  252,
 /*    30 */   252,  252,  252,  252,  252,  252,  252,  252,  252,  252,
 /*    40 */   252,  252,  252,  252,  252,  252,  252,  252,  252,  252,
 /*    50 */   252,  230,  231,  252,  197,  198,  194,  237,  211,  210,
 /*    60 */   214,  223,  222,  221,  220,  219,  218,  213,  200,  190,
 /*    70 */   190,  190,  203,  202,  201,  200,  252,  252,  252,  252,
 /*    80 */   252,  252,  190,  252,  252,  252,  252,  252,  252,  252,
 /*    90 */   252,  252,  252,  208,  216,  215,  207,  217,  206,  205,
 /*   100 */   190,
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
  cmdl_internal_ParseARG_SDECL                /* A place to hold %extra_argument */
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
void cmdl_internal_ParseTrace(FILE *TraceFILE, char *zTracePrompt){
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
  "LEFT_SQUARE",   "RIGHT_SQUARE",  "ARROW",         "MODEL_NAME",  
  "INCLUDE",       "error",         "expression",    "arguments",   
  "comparison",    "sum",           "product",       "conjunction", 
  "disjunction",   "assignment",    "reaction",      "reactionSide",
  "reactionSpecifiers",  "reactionSpecifier",  "reactionComponent",  "preprocessorDirective",
  "model",         "statements",    "statement",   
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
 /*  36 */ "reaction ::= reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON",
 /*  37 */ "reaction ::= IDENTIFIER COMMA reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON",
 /*  38 */ "reactionSpecifiers ::= reactionSpecifier",
 /*  39 */ "reactionSpecifiers ::= reactionSpecifiers COMMA reactionSpecifier",
 /*  40 */ "reactionSpecifier ::= expression",
 /*  41 */ "reactionSpecifier ::= IDENTIFIER COLON expression",
 /*  42 */ "reactionSpecifier ::= LEFT_SQUARE expression RIGHT_SQUARE",
 /*  43 */ "reactionSide ::=",
 /*  44 */ "reactionSide ::= reactionComponent",
 /*  45 */ "reactionSide ::= reactionSide PLUS reactionComponent",
 /*  46 */ "reactionSide ::= expression PLUS expression",
 /*  47 */ "reactionSide ::= reactionSide PLUS expression",
 /*  48 */ "reactionComponent ::= IDENTIFIER",
 /*  49 */ "reactionComponent ::= IDENTIFIER LEFT_SQUARE RIGHT_SQUARE",
 /*  50 */ "reactionComponent ::= expression MULTIPLY IDENTIFIER",
 /*  51 */ "reactionComponent ::= expression MULTIPLY IDENTIFIER LEFT_SQUARE RIGHT_SQUARE",
 /*  52 */ "reactionComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionSide COLON reactionSide RIGHT_SQUARE",
 /*  53 */ "preprocessorDirective ::= INCLUDE IDENTIFIER SEMICOLON",
 /*  54 */ "preprocessorDirective ::= MODEL_NAME IDENTIFIER SEMICOLON",
 /*  55 */ "model ::= statements",
 /*  56 */ "statements ::= statements statement",
 /*  57 */ "statements ::=",
 /*  58 */ "statement ::= assignment",
 /*  59 */ "statement ::= reaction",
 /*  60 */ "statement ::= preprocessorDirective",
 /*  61 */ "statement ::= error",
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
** second argument to cmdl_internal_ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
void cmdl_internal_ParseInit(void *yypParser){
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

#ifndef cmdl_internal_Parse_ENGINEALWAYSONSTACK
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
** to cmdl_internal_Parse and cmdl_internal_ParseFree.
*/
void *cmdl_internal_ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ) cmdl_internal_ParseInit(pParser);
  return pParser;
}
#endif /* cmdl_internal_Parse_ENGINEALWAYSONSTACK */


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
  cmdl_internal_ParseARG_FETCH;
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
    case 27: /* MODEL_NAME */
    case 28: /* INCLUDE */
{
#line 8 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"

	delete (yypminor->yy0);
	(yypminor->yy0) = nullptr;

#line 759 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 30: /* expression */
{
#line 83 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy40);
	(yypminor->yy40) = nullptr;

#line 769 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 31: /* arguments */
{
#line 133 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy61);
	(yypminor->yy61) = nullptr;

#line 779 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 32: /* comparison */
{
#line 117 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy49);
	(yypminor->yy49) = nullptr;

#line 789 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 33: /* sum */
{
#line 154 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy60);
	(yypminor->yy60) = nullptr;

#line 799 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 34: /* product */
{
#line 183 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy84);
	(yypminor->yy84) = nullptr;

#line 809 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 35: /* conjunction */
{
#line 213 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy29);
	(yypminor->yy29) = nullptr;

#line 819 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 36: /* disjunction */
{
#line 233 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy75);
	(yypminor->yy75) = nullptr;

#line 829 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 39: /* reactionSide */
{
#line 405 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy7);
	(yypminor->yy7) = nullptr;

#line 839 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 40: /* reactionSpecifiers */
{
#line 351 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy5);
	(yypminor->yy5) = nullptr;

#line 849 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 41: /* reactionSpecifier */
{
#line 374 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy44);
	(yypminor->yy44) = nullptr;

#line 859 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 42: /* reactionComponent */
{
#line 445 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy63);
	(yypminor->yy63) = nullptr;

#line 869 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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
void cmdl_internal_ParseFinalize(void *p){
  yyParser *pParser = (yyParser*)p;
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
}

#ifndef cmdl_internal_Parse_ENGINEALWAYSONSTACK
/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void cmdl_internal_ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  cmdl_internal_ParseFinalize(p);
  (*freeProc)(p);
}
#endif /* cmdl_internal_Parse_ENGINEALWAYSONSTACK */

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int cmdl_internal_ParseStackPeak(void *p){
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
   cmdl_internal_ParseARG_FETCH;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
#line 5 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
throw std::exception("Parser stack overflow while parsing cmdl file.");
#line 1050 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
/******** End %stack_overflow code ********************************************/
   cmdl_internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
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
  cmdl_internal_ParseTOKENTYPE yyMinor        /* The minor token to shift in */
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
  { 30, -1 },
  { 30, -4 },
  { 30, -1 },
  { 30, -3 },
  { 32, -5 },
  { 30, -1 },
  { 31, 0 },
  { 31, -1 },
  { 31, -3 },
  { 30, -1 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 33, -3 },
  { 30, -1 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 30, -1 },
  { 35, -3 },
  { 35, -3 },
  { 30, -1 },
  { 36, -3 },
  { 36, -3 },
  { 30, -2 },
  { 30, -2 },
  { 30, -3 },
  { 30, -3 },
  { 30, -3 },
  { 30, -3 },
  { 30, -3 },
  { 30, -3 },
  { 30, -3 },
  { 37, -4 },
  { 37, -6 },
  { 38, -6 },
  { 38, -8 },
  { 40, -1 },
  { 40, -3 },
  { 41, -1 },
  { 41, -3 },
  { 41, -3 },
  { 39, 0 },
  { 39, -1 },
  { 39, -3 },
  { 39, -3 },
  { 39, -3 },
  { 42, -1 },
  { 42, -3 },
  { 42, -3 },
  { 42, -5 },
  { 42, -7 },
  { 43, -3 },
  { 43, -3 },
  { 44, -1 },
  { 45, -2 },
  { 45, 0 },
  { 46, -1 },
  { 46, -1 },
  { 46, -1 },
  { 46, -1 },
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
  cmdl_internal_ParseARG_FETCH;
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
#line 87 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new VariableExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1256 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 1: /* expression ::= IDENTIFIER LEFT_ROUND arguments RIGHT_ROUND */
#line 92 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto func = new FunctionExpression(*yymsp[-3].minor.yy0);
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yylhsminor.yy40 = nullptr;
	for(auto& argument : *yymsp[-1].minor.yy61)
	{
		func->PushBack(std::move(argument));
	}
	delete yymsp[-1].minor.yy61;
	yymsp[-1].minor.yy61 = nullptr;
	yylhsminor.yy40 = func;
	func = nullptr;
}
#line 1275 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,20,&yymsp[0].minor);
  yymsp[-3].minor.yy40 = yylhsminor.yy40;
        break;
      case 2: /* expression ::= VALUE */
#line 107 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new NumberExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1287 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 3: /* expression ::= LEFT_ROUND expression RIGHT_ROUND */
{  yy_destructor(yypParser,19,&yymsp[-2].minor);
#line 112 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-2].minor.yy40 = yymsp[-1].minor.yy40;
}
#line 1296 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
        break;
      case 4: /* comparison ::= expression QUESTIONMARK expression COLON expression */
#line 121 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy49 = new ConditionalExpression(std::unique_ptr<IExpression>(yymsp[-4].minor.yy40), std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
	yymsp[-2].minor.yy40 = nullptr;
	yymsp[0].minor.yy40 = nullptr;
	yymsp[-4].minor.yy40 = nullptr;
}
#line 1308 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-4].minor.yy49 = yylhsminor.yy49;
        break;
      case 5: /* expression ::= comparison */
#line 127 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = yymsp[0].minor.yy49;
}
#line 1318 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 6: /* arguments ::= */
#line 137 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[1].minor.yy61 = new FunctionArguments();
}
#line 1326 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
        break;
      case 7: /* arguments ::= expression */
#line 140 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy61 = new FunctionArguments();
	yylhsminor.yy61->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy40));
	yymsp[0].minor.yy40 = nullptr;
}
#line 1335 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy61 = yylhsminor.yy61;
        break;
      case 8: /* arguments ::= arguments COMMA expression */
#line 145 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy61 = yymsp[-2].minor.yy61;
	yymsp[-2].minor.yy61 = nullptr;
	yylhsminor.yy61->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy40));
	yymsp[0].minor.yy40 = nullptr;
}
#line 1346 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy61 = yylhsminor.yy61;
        break;
      case 9: /* expression ::= sum */
#line 158 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = yymsp[0].minor.yy60;
}
#line 1355 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 10: /* sum ::= expression PLUS expression */
#line 161 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy60 = new SumExpression();
	yylhsminor.yy60->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy40));
	yylhsminor.yy60->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1365 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy60 = yylhsminor.yy60;
        break;
      case 11: /* sum ::= expression MINUS expression */
#line 166 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy60 = new SumExpression();
	yylhsminor.yy60->PushBack(false,  std::unique_ptr<IExpression>(yymsp[-2].minor.yy40));
	yylhsminor.yy60->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1376 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy60 = yylhsminor.yy60;
        break;
      case 12: /* sum ::= sum PLUS expression */
#line 171 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy60 = yymsp[-2].minor.yy60;
	yylhsminor.yy60->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1386 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy60 = yylhsminor.yy60;
        break;
      case 13: /* sum ::= sum MINUS expression */
#line 175 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy60 = yymsp[-2].minor.yy60;
	yylhsminor.yy60->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1396 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy60 = yylhsminor.yy60;
        break;
      case 14: /* expression ::= product */
#line 187 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = yymsp[0].minor.yy84;
}
#line 1405 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 15: /* product ::= expression MULTIPLY expression */
#line 190 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy84 = new ProductExpression();
	yylhsminor.yy84->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy40));
	yylhsminor.yy84->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));

}
#line 1416 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy84 = yylhsminor.yy84;
        break;
      case 16: /* product ::= expression DIVIDE expression */
#line 196 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy84 = new ProductExpression();
	yylhsminor.yy84->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy40));
	yylhsminor.yy84->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));

}
#line 1428 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy84 = yylhsminor.yy84;
        break;
      case 17: /* product ::= product MULTIPLY expression */
#line 202 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy84 = yymsp[-2].minor.yy84;
	yylhsminor.yy84->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1438 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy84 = yylhsminor.yy84;
        break;
      case 18: /* product ::= product DIVIDE expression */
#line 206 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy84 = yymsp[-2].minor.yy84;
	yylhsminor.yy84->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1448 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy84 = yylhsminor.yy84;
        break;
      case 19: /* expression ::= conjunction */
#line 217 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = yymsp[0].minor.yy29;
}
#line 1457 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 20: /* conjunction ::= expression AND expression */
#line 220 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy29 = new ConjunctionExpression();
	yylhsminor.yy29->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy40));
	yylhsminor.yy29->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));

}
#line 1468 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy29 = yylhsminor.yy29;
        break;
      case 21: /* conjunction ::= conjunction AND expression */
#line 226 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy29 = yymsp[-2].minor.yy29;
	yylhsminor.yy29->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1478 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy29 = yylhsminor.yy29;
        break;
      case 22: /* expression ::= disjunction */
#line 237 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = yymsp[0].minor.yy75;
}
#line 1487 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy40 = yylhsminor.yy40;
        break;
      case 23: /* disjunction ::= expression OR expression */
#line 240 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy75 = new DisjunctionExpression();
	yylhsminor.yy75->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy40));
	yylhsminor.yy75->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));

}
#line 1498 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy75 = yylhsminor.yy75;
        break;
      case 24: /* disjunction ::= disjunction OR expression */
#line 246 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy75 = yymsp[-2].minor.yy75;
	yylhsminor.yy75->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1508 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy75 = yylhsminor.yy75;
        break;
      case 25: /* expression ::= NOT expression */
{  yy_destructor(yypParser,16,&yymsp[-1].minor);
#line 252 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-1].minor.yy40 = new UnaryNotExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1518 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
        break;
      case 26: /* expression ::= MINUS expression */
{  yy_destructor(yypParser,12,&yymsp[-1].minor);
#line 257 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-1].minor.yy40 = new UnaryMinusExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1527 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
        break;
      case 27: /* expression ::= expression EXP expression */
#line 262 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new ExponentiationExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40));
}
#line 1535 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,15,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 28: /* expression ::= expression EQUAL expression */
#line 268 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40), ComparisonExpression::type_equal);
}
#line 1544 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 29: /* expression ::= expression NOT_EQUAL expression */
#line 271 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40), ComparisonExpression::type_not_equal);
}
#line 1553 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 30: /* expression ::= expression GREATER expression */
#line 274 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40), ComparisonExpression::type_greater);
}
#line 1562 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 31: /* expression ::= expression GREATER_EQUAL expression */
#line 277 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40), ComparisonExpression::type_greater_equal);
}
#line 1571 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 32: /* expression ::= expression LESS expression */
#line 280 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40), ComparisonExpression::type_less);
}
#line 1580 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 33: /* expression ::= expression LESS_EQUAL expression */
#line 283 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy40 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy40), std::unique_ptr<IExpression>(yymsp[0].minor.yy40), ComparisonExpression::type_less_equal);
}
#line 1589 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy40 = yylhsminor.yy40;
        break;
      case 34: /* assignment ::= IDENTIFIER ASSIGN expression SEMICOLON */
#line 295 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-3].minor.yy0 and yymsp[-1].minor.yy40 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy40);
	yymsp[-1].minor.yy40 = nullptr;

	parseTree->CreateVariable(std::move(name), parseTree->GetExpressionValue(e_temp.get()));
}
#line 1606 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,23,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 35: /* assignment ::= IDENTIFIER ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 307 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy0 and yymsp[-2].minor.yy40 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-5].minor.yy0;
	delete yymsp[-5].minor.yy0;
	yymsp[-5].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy40);
	yymsp[-2].minor.yy40 = nullptr;

	parseTree->CreateVariable(std::move(name), std::move(e_temp));
}
#line 1623 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,23,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-3].minor);
  yy_destructor(yypParser,25,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 36: /* reaction ::= reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON */
#line 321 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy7, yymsp[-3].minor.yy7 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionSide>(yymsp[-5].minor.yy7);
	auto products_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy7);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy5);
	yymsp[-1].minor.yy5 = nullptr;
	yymsp[-5].minor.yy7 = nullptr;
	yymsp[-3].minor.yy7 = nullptr;

	parseTree->CreateReaction(std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1643 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 37: /* reaction ::= IDENTIFIER COMMA reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON */
#line 334 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy7, yymsp[-3].minor.yy7 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionSide>(yymsp[-5].minor.yy7);
	auto products_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy7);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy5);
	identifier name = *yymsp[-7].minor.yy0; 
	yymsp[-1].minor.yy5 = nullptr;
	yymsp[-5].minor.yy7 = nullptr;
	yymsp[-3].minor.yy7 = nullptr;
	delete yymsp[-7].minor.yy0;
	yymsp[-7].minor.yy0 = nullptr;

	parseTree->CreateReaction(std::move(name), std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1665 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-6].minor);
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 38: /* reactionSpecifiers ::= reactionSpecifier */
#line 355 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rss_temp = std::make_unique<ReactionSpecifiers>();
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy44);
	yymsp[0].minor.yy44 = nullptr;
	yylhsminor.yy5 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy5 = rss_temp.release();
}
#line 1681 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy5 = yylhsminor.yy5;
        break;
      case 39: /* reactionSpecifiers ::= reactionSpecifiers COMMA reactionSpecifier */
#line 363 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-2].minor.yy5);
	yymsp[-2].minor.yy5 = nullptr;
	yylhsminor.yy5 = nullptr;
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy44);
	yymsp[0].minor.yy44 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy5 = rss_temp.release();
}
#line 1695 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy5 = yylhsminor.yy5;
        break;
      case 40: /* reactionSpecifier ::= expression */
#line 378 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy40);
	yymsp[0].minor.yy40 = nullptr;
	yylhsminor.yy44 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy44 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::make_unique<NumberExpression>(value));
}
#line 1708 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy44 = yylhsminor.yy44;
        break;
      case 41: /* reactionSpecifier ::= IDENTIFIER COLON expression */
#line 386 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy40);
	yymsp[0].minor.yy40 = nullptr;
	yylhsminor.yy44 = nullptr;
	identifier name = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy44 = new ReactionSpecifier(name, std::make_unique<NumberExpression>(value));
}
#line 1723 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-2].minor.yy44 = yylhsminor.yy44;
        break;
      case 42: /* reactionSpecifier ::= LEFT_SQUARE expression RIGHT_SQUARE */
{  yy_destructor(yypParser,24,&yymsp[-2].minor);
#line 397 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy40);
	yymsp[-1].minor.yy40 = nullptr;
	yymsp[-2].minor.yy44 = nullptr;
	yymsp[-2].minor.yy44 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::move(e_temp));
}
#line 1736 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      case 43: /* reactionSide ::= */
#line 409 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[1].minor.yy7 = new ReactionSide();
}
#line 1745 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
        break;
      case 44: /* reactionSide ::= reactionComponent */
#line 412 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionComponent>(yymsp[0].minor.yy63);
	yymsp[0].minor.yy63 = nullptr;

	yylhsminor.yy7 = new ReactionSide();
	yylhsminor.yy7->PushBack(std::move(rc_temp));
}
#line 1756 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy7 = yylhsminor.yy7;
        break;
      case 45: /* reactionSide ::= reactionSide PLUS reactionComponent */
#line 419 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy7 = yymsp[-2].minor.yy7;
	yymsp[-2].minor.yy7 = nullptr;
	auto rc_temp = std::unique_ptr<ReactionComponent>(yymsp[0].minor.yy63);
	yymsp[0].minor.yy63 = nullptr;

	yylhsminor.yy7->PushBack(std::move(rc_temp));
}
#line 1769 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy7 = yylhsminor.yy7;
        break;
      case 46: /* reactionSide ::= expression PLUS expression */
#line 428 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	delete(yymsp[-2].minor.yy40);
	yymsp[-2].minor.yy40=nullptr;
	delete(yymsp[0].minor.yy40);
	yymsp[0].minor.yy40=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1782 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 47: /* reactionSide ::= reactionSide PLUS expression */
#line 436 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	delete(yymsp[0].minor.yy40);
	yymsp[0].minor.yy40=nullptr;
	delete(yymsp[-2].minor.yy7);
	yymsp[-2].minor.yy7=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1794 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 48: /* reactionComponent ::= IDENTIFIER */
#line 449 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	yylhsminor.yy63 = nullptr;

	yylhsminor.yy63 = new ReactionComponent(state, 1, false);
}
#line 1807 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy63 = yylhsminor.yy63;
        break;
      case 49: /* reactionComponent ::= IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 458 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	yylhsminor.yy63 = nullptr;

	yylhsminor.yy63 = new ReactionComponent(state, 1, true);
}
#line 1820 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
  yymsp[-2].minor.yy63 = yylhsminor.yy63;
        break;
      case 50: /* reactionComponent ::= expression MULTIPLY IDENTIFIER */
#line 467 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy40);
	yymsp[-2].minor.yy40 = nullptr;
	yylhsminor.yy63 = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy63 = new ReactionComponent(state, stochiometry, false);
}
#line 1838 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy63 = yylhsminor.yy63;
        break;
      case 51: /* reactionComponent ::= expression MULTIPLY IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 479 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-4].minor.yy40);
	yymsp[-4].minor.yy40 = nullptr;
	yylhsminor.yy63 = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy63 = new ReactionComponent(state, stochiometry, true);
}
#line 1855 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-3].minor);
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
  yymsp[-4].minor.yy63 = yylhsminor.yy63;
        break;
      case 52: /* reactionComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionSide COLON reactionSide RIGHT_SQUARE */
{  yy_destructor(yypParser,24,&yymsp[-6].minor);
#line 491 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-5].minor.yy40);
	yymsp[-5].minor.yy40 = nullptr;
	auto s1_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy7);
	auto s2_temp = std::unique_ptr<ReactionSide>(yymsp[-1].minor.yy7);
	yymsp[-3].minor.yy7 = nullptr;
	yymsp[-1].minor.yy7 = nullptr;
	yymsp[-6].minor.yy63 = nullptr;

	identifier state = parseTree->CreateChoice(std::move(e_temp), std::move(s1_temp), std::move(s2_temp));
	yymsp[-6].minor.yy63 = new ReactionComponent(state, 1, false);
}
#line 1876 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-4].minor);
  yy_destructor(yypParser,21,&yymsp[-2].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      case 53: /* preprocessorDirective ::= INCLUDE IDENTIFIER SEMICOLON */
{  yy_destructor(yypParser,28,&yymsp[-2].minor);
#line 508 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier fileName = *yymsp[-1].minor.yy0;
	delete yymsp[-1].minor.yy0;
	yymsp[-1].minor.yy0 = nullptr;
	parseTree->IncludeFile(fileName);
}
#line 1891 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
        break;
      case 54: /* preprocessorDirective ::= MODEL_NAME IDENTIFIER SEMICOLON */
{  yy_destructor(yypParser,27,&yymsp[-2].minor);
#line 505 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
}
#line 1900 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,17,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
        break;
      default:
      /* (55) model ::= statements */ yytestcase(yyruleno==55);
      /* (56) statements ::= statements statement */ yytestcase(yyruleno==56);
      /* (57) statements ::= */ yytestcase(yyruleno==57);
      /* (58) statement ::= assignment (OPTIMIZED OUT) */ assert(yyruleno!=58);
      /* (59) statement ::= reaction (OPTIMIZED OUT) */ assert(yyruleno!=59);
      /* (60) statement ::= preprocessorDirective (OPTIMIZED OUT) */ assert(yyruleno!=60);
      /* (61) statement ::= error (OPTIMIZED OUT) */ assert(yyruleno!=61);
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
  cmdl_internal_ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
#line 4 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
throw std::exception("Syntax error.");
#line 1959 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
/************ End %parse_failure code *****************************************/
  cmdl_internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  cmdl_internal_ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  cmdl_internal_ParseARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
/************ End %syntax_error code ******************************************/
  cmdl_internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  cmdl_internal_ParseARG_FETCH;
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
  cmdl_internal_ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "cmdl_internal_ParseAlloc" which describes the current state of the parser.
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
void cmdl_internal_Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  cmdl_internal_ParseTOKENTYPE yyminor       /* The value for the token */
  cmdl_internal_ParseARG_PDECL               /* Optional %extra_argument parameter */
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
  cmdl_internal_ParseARG_STORE;

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