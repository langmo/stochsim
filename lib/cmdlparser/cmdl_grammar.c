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
#define YYNOCODE 45
#define YYACTIONTYPE unsigned char
#define cmdl_internal_ParseTOKENTYPE TerminalSymbol*
typedef union {
  int yyinit;
  cmdl_internal_ParseTOKENTYPE yy0;
  DisjunctionExpression* yy21;
  ProductExpression* yy28;
  ReactionSpecifiers* yy29;
  ConjunctionExpression* yy45;
  ConditionalExpression* yy51;
  SumExpression* yy56;
  FunctionArguments* yy59;
  IExpression* yy64;
  ReactionSpecifier* yy74;
  ReactionComponent* yy75;
  ReactionSide* yy81;
  int yy89;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define cmdl_internal_ParseARG_SDECL CmdlParseTree* parseTree;
#define cmdl_internal_ParseARG_PDECL ,CmdlParseTree* parseTree
#define cmdl_internal_ParseARG_FETCH CmdlParseTree* parseTree = yypParser->parseTree
#define cmdl_internal_ParseARG_STORE yypParser->parseTree = parseTree
#define YYERRORSYMBOL 27
#define YYERRSYMDT yy89
#define YYNSTATE             97
#define YYNRULE              59
#define YY_MAX_SHIFT         96
#define YY_MIN_SHIFTREDUCE   121
#define YY_MAX_SHIFTREDUCE   179
#define YY_MIN_REDUCE        180
#define YY_MAX_REDUCE        238
#define YY_ERROR_ACTION      239
#define YY_ACCEPT_ACTION     240
#define YY_NO_ACTION         241
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
#define YY_ACTTAB_COUNT (654)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    41,   35,   34,   32,   31,   30,   29,   28,   27,   17,
 /*    10 */    38,   16,   36,   33,   37,   36,   33,  240,    1,   40,
 /*    20 */   172,  170,   41,   35,   34,   32,   31,   30,   29,   28,
 /*    30 */    27,   39,   38,   37,   36,   33,   51,  156,  185,  189,
 /*    40 */   194,  199,  202,   33,   11,  163,   76,  218,   41,   35,
 /*    50 */    34,   32,   31,   30,   29,   28,   27,   39,   38,   37,
 /*    60 */    36,   33,   54,   85,  185,  189,  194,  199,  202,  182,
 /*    70 */    11,   88,   15,  155,   41,   35,   34,   32,   31,   30,
 /*    80 */    29,   28,   27,   39,   38,   37,   36,   33,   41,   35,
 /*    90 */    34,   32,   31,   30,   29,   28,   27,   39,   38,   37,
 /*   100 */    36,   33,  122,  157,   43,  158,  182,   40,  182,  182,
 /*   110 */    41,   35,   34,   32,   31,   30,   29,   28,   27,   39,
 /*   120 */    38,   37,   36,   33,   10,  182,   10,  182,  124,  182,
 /*   130 */   182,    5,   35,   34,   32,   31,   30,   29,   28,   27,
 /*   140 */    39,   38,   37,   36,   33,   41,   35,   34,   32,   31,
 /*   150 */    30,   29,   28,   27,   39,   38,   37,   36,   33,   41,
 /*   160 */    35,   34,   32,   31,   30,   29,   28,   27,   17,   38,
 /*   170 */    16,   36,   33,   41,   35,   34,   32,   31,   30,   29,
 /*   180 */    28,   27,  182,   38,   16,   36,   33,  234,   53,    9,
 /*   190 */   185,  189,  194,  199,  202,  234,  234,   78,  182,  182,
 /*   200 */   224,  182,  182,  234,    3,   34,   32,   31,   30,   29,
 /*   210 */    28,   27,   39,   38,   37,   36,   33,   32,   31,   30,
 /*   220 */    29,   28,   27,   39,   38,   37,   36,   33,  239,  239,
 /*   230 */   239,  239,  239,  239,   39,   38,   37,   36,   33,   53,
 /*   240 */   182,  185,  189,  194,  199,  202,    9,  182,   77,    9,
 /*   250 */    53,  224,  185,  189,  194,  199,  202,    2,    9,   79,
 /*   260 */     6,   44,  224,  185,  189,  194,  199,  202,    4,  182,
 /*   270 */    80,    9,   51,  224,  185,  189,  194,  199,  202,    9,
 /*   280 */   182,  182,   81,  218,   53,  173,  185,  189,  194,  199,
 /*   290 */   202,  182,  182,   83,    7,   53,  224,  185,  189,  194,
 /*   300 */   199,  202,  182,  182,   84,  182,  182,  224,  226,  233,
 /*   310 */    37,   36,   33,  182,  182,  182,  182,  182,  226,  226,
 /*   320 */   182,   21,  226,  226,  182,   22,   71,  123,   42,  182,
 /*   330 */   182,  182,  182,   13,  182,  182,   57,  182,  185,  189,
 /*   340 */   194,  199,  202,  182,  182,  182,  182,   51,  225,  185,
 /*   350 */   189,  194,  199,  202,  230,  182,  182,  182,  219,  182,
 /*   360 */   182,  182,   11,  228,  230,  230,  182,   86,  230,  230,
 /*   370 */   182,   11,  182,  228,  228,   21,   87,  228,  228,   22,
 /*   380 */    82,  123,   42,  182,   21,  182,  182,   14,   22,   70,
 /*   390 */   123,   42,  182,   21,  182,  182,   13,   22,   96,  123,
 /*   400 */    42,  182,  182,  182,   47,   18,  185,  189,  194,  199,
 /*   410 */   202,   50,  182,  185,  189,  194,  199,  202,   45,  182,
 /*   420 */   185,  189,  194,  199,  202,   52,  182,  185,  189,  194,
 /*   430 */   199,  202,  182,   95,  182,  185,  189,  194,  199,  202,
 /*   440 */    68,  182,  185,  189,  194,  199,  202,   46,  182,  185,
 /*   450 */   189,  194,  199,  202,   72,  182,  185,  189,  194,  199,
 /*   460 */   202,  182,   89,  182,  185,  189,  194,  199,  202,   90,
 /*   470 */   182,  185,  189,  194,  199,  202,   91,  182,  185,  189,
 /*   480 */   194,  199,  202,   60,  182,  185,  189,  194,  199,  202,
 /*   490 */   182,   58,  182,  185,  189,  194,  199,  202,   92,  182,
 /*   500 */   185,  189,  194,  199,  202,   73,  182,  185,  189,  194,
 /*   510 */   199,  202,   61,  182,  185,  189,  194,  199,  202,  182,
 /*   520 */    62,  182,  185,  189,  194,  199,  202,   63,  182,  185,
 /*   530 */   189,  194,  199,  202,   64,  182,  185,  189,  194,  199,
 /*   540 */   202,   65,  182,  185,  189,  194,  199,  202,  182,   66,
 /*   550 */   182,  185,  189,  194,  199,  202,   93,  182,  185,  189,
 /*   560 */   194,  199,  202,   67,  182,  185,  189,  194,  199,  202,
 /*   570 */    59,  182,  185,  189,  194,  199,  202,  182,   94,  182,
 /*   580 */   185,  189,  194,  199,  202,   74,  182,  185,  189,  194,
 /*   590 */   199,  202,   75,  182,  185,  189,  194,  199,  202,   56,
 /*   600 */   182,  185,  189,  194,  199,  202,  182,   48,  182,  185,
 /*   610 */   189,  194,  199,  202,   49,  182,  185,  189,  194,  199,
 /*   620 */   202,   55,  228,  185,  189,  194,  199,  202,  182,  182,
 /*   630 */    11,  182,  182,    8,   12,   87,   21,  228,  182,  182,
 /*   640 */    22,   96,  123,   42,  182,  182,   21,  182,  182,  182,
 /*   650 */    22,   69,  123,   42,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    10 */    12,   13,   14,   15,   13,   14,   15,   41,   42,   21,
 /*    20 */    25,   25,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    30 */    10,   11,   12,   13,   14,   15,   28,    1,   30,   31,
 /*    40 */    32,   33,   34,   15,   19,   25,   38,   39,    2,    3,
 /*    50 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*    60 */    14,   15,   28,   29,   30,   31,   32,   33,   34,   44,
 /*    70 */    19,   25,   21,    1,    2,    3,    4,    5,    6,    7,
 /*    80 */     8,    9,   10,   11,   12,   13,   14,   15,    2,    3,
 /*    90 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   100 */    14,   15,   20,    1,   22,    1,   44,   21,   44,   44,
 /*   110 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   120 */    12,   13,   14,   15,   22,   44,   22,   44,   20,   44,
 /*   130 */    44,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*   140 */    11,   12,   13,   14,   15,    2,    3,    4,    5,    6,
 /*   150 */     7,    8,    9,   10,   11,   12,   13,   14,   15,    2,
 /*   160 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   170 */    13,   14,   15,    2,    3,    4,    5,    6,    7,    8,
 /*   180 */     9,   10,   44,   12,   13,   14,   15,   27,   28,   11,
 /*   190 */    30,   31,   32,   33,   34,   35,   36,   37,   44,   44,
 /*   200 */    40,   44,   44,   43,   26,    4,    5,    6,    7,    8,
 /*   210 */     9,   10,   11,   12,   13,   14,   15,    5,    6,    7,
 /*   220 */     8,    9,   10,   11,   12,   13,   14,   15,    5,    6,
 /*   230 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   28,
 /*   240 */    44,   30,   31,   32,   33,   34,   11,   44,   37,   11,
 /*   250 */    28,   40,   30,   31,   32,   33,   34,   22,   11,   37,
 /*   260 */    22,   28,   40,   30,   31,   32,   33,   34,   21,   44,
 /*   270 */    37,   11,   28,   40,   30,   31,   32,   33,   34,   11,
 /*   280 */    44,   44,   38,   39,   28,   25,   30,   31,   32,   33,
 /*   290 */    34,   44,   44,   37,   26,   28,   40,   30,   31,   32,
 /*   300 */    33,   34,   44,   44,   37,   44,   44,   40,   11,    0,
 /*   310 */    13,   14,   15,   44,   44,   44,   44,   44,   21,   22,
 /*   320 */    44,   12,   25,   26,   44,   16,   17,   18,   19,   44,
 /*   330 */    44,   44,   44,   24,   44,   44,   28,   44,   30,   31,
 /*   340 */    32,   33,   34,   44,   44,   44,   44,   28,   40,   30,
 /*   350 */    31,   32,   33,   34,   11,   44,   44,   44,   39,   44,
 /*   360 */    44,   44,   19,   11,   21,   22,   44,   24,   25,   26,
 /*   370 */    44,   19,   44,   21,   22,   12,   24,   25,   26,   16,
 /*   380 */    17,   18,   19,   44,   12,   44,   44,   24,   16,   17,
 /*   390 */    18,   19,   44,   12,   44,   44,   24,   16,   17,   18,
 /*   400 */    19,   44,   44,   44,   28,   24,   30,   31,   32,   33,
 /*   410 */    34,   28,   44,   30,   31,   32,   33,   34,   28,   44,
 /*   420 */    30,   31,   32,   33,   34,   28,   44,   30,   31,   32,
 /*   430 */    33,   34,   44,   28,   44,   30,   31,   32,   33,   34,
 /*   440 */    28,   44,   30,   31,   32,   33,   34,   28,   44,   30,
 /*   450 */    31,   32,   33,   34,   28,   44,   30,   31,   32,   33,
 /*   460 */    34,   44,   28,   44,   30,   31,   32,   33,   34,   28,
 /*   470 */    44,   30,   31,   32,   33,   34,   28,   44,   30,   31,
 /*   480 */    32,   33,   34,   28,   44,   30,   31,   32,   33,   34,
 /*   490 */    44,   28,   44,   30,   31,   32,   33,   34,   28,   44,
 /*   500 */    30,   31,   32,   33,   34,   28,   44,   30,   31,   32,
 /*   510 */    33,   34,   28,   44,   30,   31,   32,   33,   34,   44,
 /*   520 */    28,   44,   30,   31,   32,   33,   34,   28,   44,   30,
 /*   530 */    31,   32,   33,   34,   28,   44,   30,   31,   32,   33,
 /*   540 */    34,   28,   44,   30,   31,   32,   33,   34,   44,   28,
 /*   550 */    44,   30,   31,   32,   33,   34,   28,   44,   30,   31,
 /*   560 */    32,   33,   34,   28,   44,   30,   31,   32,   33,   34,
 /*   570 */    28,   44,   30,   31,   32,   33,   34,   44,   28,   44,
 /*   580 */    30,   31,   32,   33,   34,   28,   44,   30,   31,   32,
 /*   590 */    33,   34,   28,   44,   30,   31,   32,   33,   34,   28,
 /*   600 */    44,   30,   31,   32,   33,   34,   44,   28,   44,   30,
 /*   610 */    31,   32,   33,   34,   28,   44,   30,   31,   32,   33,
 /*   620 */    34,   28,   11,   30,   31,   32,   33,   34,   44,   44,
 /*   630 */    19,   44,   44,   22,   23,   24,   12,   26,   44,   44,
 /*   640 */    16,   17,   18,   19,   44,   44,   12,   44,   44,   44,
 /*   650 */    16,   17,   18,   19,
};
#define YY_SHIFT_USE_DFLT (654)
#define YY_SHIFT_COUNT    (96)
#define YY_SHIFT_MIN      (-5)
#define YY_SHIFT_MAX      (634)
static const short yy_shift_ofst[] = {
 /*     0 */   654,  309,  363,  372,  372,  372,  363,  372,  372,  372,
 /*    10 */   363,  624,  381,  624,  624,  624,  634,  624,  624,  624,
 /*    20 */   624,  624,  624,  624,  624,  624,  624,  624,  624,  624,
 /*    30 */   624,  624,  624,  624,  624,  624,  624,  624,  624,  624,
 /*    40 */   624,  624,  624,  624,   -2,   20,   46,   72,   86,  108,
 /*    50 */   129,  143,  143,  157,  143,  143,  143,  171,  201,  201,
 /*    60 */   212,  223,  223,  223,  223,  223,  223,  212,  297,  343,
 /*    70 */   352,  611,    1,    1,    1,    1,  102,  235,  178,  260,
 /*    80 */   247,  104,   51,  238,  268,   82,   -5,   -4,   36,   28,
 /*    90 */    28,   28,   28,   28,   28,   28,   25,
};
#define YY_REDUCE_USE_DFLT (-25)
#define YY_REDUCE_COUNT (43)
#define YY_REDUCE_MIN   (-24)
#define YY_REDUCE_MAX   (593)
static const short yy_reduce_ofst[] = {
 /*     0 */   -24,  160,    8,  211,  222,  233,  244,  256,  267,  308,
 /*    10 */   319,   34,  376,  383,  390,  397,  405,  412,  419,  426,
 /*    20 */   434,  441,  448,  455,  463,  470,  477,  484,  492,  499,
 /*    30 */   506,  513,  521,  528,  535,  542,  550,  405,  557,  564,
 /*    40 */   571,  579,  586,  593,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   235,  223,  239,  223,  223,  223,  239,  223,  223,  239,
 /*    10 */   239,  186,  239,  239,  239,  239,  239,  239,  239,  239,
 /*    20 */   239,  239,  239,  239,  239,  239,  239,  239,  239,  239,
 /*    30 */   239,  239,  239,  239,  239,  239,  239,  239,  239,  239,
 /*    40 */   239,  239,  239,  239,  239,  239,  239,  239,  239,  239,
 /*    50 */   239,  220,  221,  239,  187,  188,  184,  227,  201,  200,
 /*    60 */   204,  213,  212,  211,  210,  209,  208,  203,  190,  180,
 /*    70 */   180,  180,  193,  192,  191,  190,  239,  239,  239,  239,
 /*    80 */   239,  239,  180,  239,  239,  239,  239,  239,  239,  198,
 /*    90 */   206,  205,  197,  207,  196,  195,  180,
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
  "LEFT_SQUARE",   "RIGHT_SQUARE",  "ARROW",         "error",       
  "expression",    "arguments",     "comparison",    "sum",         
  "product",       "conjunction",   "disjunction",   "assignment",  
  "reaction",      "reactionSide",  "reactionSpecifiers",  "reactionSpecifier",
  "reactionComponent",  "model",         "statements",    "statement",   
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
 /*  53 */ "model ::= statements",
 /*  54 */ "statements ::= statements statement",
 /*  55 */ "statements ::=",
 /*  56 */ "statement ::= assignment",
 /*  57 */ "statement ::= reaction",
 /*  58 */ "statement ::= error",
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
{
#line 8 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"

	delete (yypminor->yy0);
	(yypminor->yy0) = nullptr;

#line 745 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 28: /* expression */
{
#line 83 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy64);
	(yypminor->yy64) = nullptr;

#line 755 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 29: /* arguments */
{
#line 133 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy59);
	(yypminor->yy59) = nullptr;

#line 765 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 30: /* comparison */
{
#line 117 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy51);
	(yypminor->yy51) = nullptr;

#line 775 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 31: /* sum */
{
#line 154 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy56);
	(yypminor->yy56) = nullptr;

#line 785 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 32: /* product */
{
#line 183 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy28);
	(yypminor->yy28) = nullptr;

#line 795 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 33: /* conjunction */
{
#line 213 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy45);
	(yypminor->yy45) = nullptr;

#line 805 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 34: /* disjunction */
{
#line 233 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy21);
	(yypminor->yy21) = nullptr;

#line 815 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 37: /* reactionSide */
{
#line 405 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy81);
	(yypminor->yy81) = nullptr;

#line 825 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 38: /* reactionSpecifiers */
{
#line 351 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy29);
	(yypminor->yy29) = nullptr;

#line 835 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 39: /* reactionSpecifier */
{
#line 374 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy74);
	(yypminor->yy74) = nullptr;

#line 845 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 40: /* reactionComponent */
{
#line 445 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy75);
	(yypminor->yy75) = nullptr;

#line 855 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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
#line 1036 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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
  { 28, -1 },
  { 28, -4 },
  { 28, -1 },
  { 28, -3 },
  { 30, -5 },
  { 28, -1 },
  { 29, 0 },
  { 29, -1 },
  { 29, -3 },
  { 28, -1 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 28, -1 },
  { 32, -3 },
  { 32, -3 },
  { 32, -3 },
  { 32, -3 },
  { 28, -1 },
  { 33, -3 },
  { 33, -3 },
  { 28, -1 },
  { 34, -3 },
  { 34, -3 },
  { 28, -2 },
  { 28, -2 },
  { 28, -3 },
  { 28, -3 },
  { 28, -3 },
  { 28, -3 },
  { 28, -3 },
  { 28, -3 },
  { 28, -3 },
  { 35, -4 },
  { 35, -6 },
  { 36, -6 },
  { 36, -8 },
  { 38, -1 },
  { 38, -3 },
  { 39, -1 },
  { 39, -3 },
  { 39, -3 },
  { 37, 0 },
  { 37, -1 },
  { 37, -3 },
  { 37, -3 },
  { 37, -3 },
  { 40, -1 },
  { 40, -3 },
  { 40, -3 },
  { 40, -5 },
  { 40, -7 },
  { 41, -1 },
  { 42, -2 },
  { 42, 0 },
  { 43, -1 },
  { 43, -1 },
  { 43, -1 },
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
	yylhsminor.yy64 = new VariableExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1239 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 1: /* expression ::= IDENTIFIER LEFT_ROUND arguments RIGHT_ROUND */
#line 92 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto func = new FunctionExpression(*yymsp[-3].minor.yy0);
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yylhsminor.yy64 = nullptr;
	for(auto& argument : *yymsp[-1].minor.yy59)
	{
		func->PushBack(std::move(argument));
	}
	delete yymsp[-1].minor.yy59;
	yymsp[-1].minor.yy59 = nullptr;
	yylhsminor.yy64 = func;
	func = nullptr;
}
#line 1258 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,20,&yymsp[0].minor);
  yymsp[-3].minor.yy64 = yylhsminor.yy64;
        break;
      case 2: /* expression ::= VALUE */
#line 107 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new NumberExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1270 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 3: /* expression ::= LEFT_ROUND expression RIGHT_ROUND */
{  yy_destructor(yypParser,19,&yymsp[-2].minor);
#line 112 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-2].minor.yy64 = yymsp[-1].minor.yy64;
}
#line 1279 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
        break;
      case 4: /* comparison ::= expression QUESTIONMARK expression COLON expression */
#line 121 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy51 = new ConditionalExpression(std::unique_ptr<IExpression>(yymsp[-4].minor.yy64), std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
	yymsp[-2].minor.yy64 = nullptr;
	yymsp[0].minor.yy64 = nullptr;
	yymsp[-4].minor.yy64 = nullptr;
}
#line 1291 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-4].minor.yy51 = yylhsminor.yy51;
        break;
      case 5: /* expression ::= comparison */
#line 127 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy51;
}
#line 1301 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 6: /* arguments ::= */
#line 137 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[1].minor.yy59 = new FunctionArguments();
}
#line 1309 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
        break;
      case 7: /* arguments ::= expression */
#line 140 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy59 = new FunctionArguments();
	yylhsminor.yy59->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy64));
	yymsp[0].minor.yy64 = nullptr;
}
#line 1318 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy59 = yylhsminor.yy59;
        break;
      case 8: /* arguments ::= arguments COMMA expression */
#line 145 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy59 = yymsp[-2].minor.yy59;
	yymsp[-2].minor.yy59 = nullptr;
	yylhsminor.yy59->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy64));
	yymsp[0].minor.yy64 = nullptr;
}
#line 1329 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy59 = yylhsminor.yy59;
        break;
      case 9: /* expression ::= sum */
#line 158 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy56;
}
#line 1338 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 10: /* sum ::= expression PLUS expression */
#line 161 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy56 = new SumExpression();
	yylhsminor.yy56->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy56->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1348 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy56 = yylhsminor.yy56;
        break;
      case 11: /* sum ::= expression MINUS expression */
#line 166 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy56 = new SumExpression();
	yylhsminor.yy56->PushBack(false,  std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy56->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1359 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy56 = yylhsminor.yy56;
        break;
      case 12: /* sum ::= sum PLUS expression */
#line 171 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy56 = yymsp[-2].minor.yy56;
	yylhsminor.yy56->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1369 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy56 = yylhsminor.yy56;
        break;
      case 13: /* sum ::= sum MINUS expression */
#line 175 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy56 = yymsp[-2].minor.yy56;
	yylhsminor.yy56->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1379 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy56 = yylhsminor.yy56;
        break;
      case 14: /* expression ::= product */
#line 187 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy28;
}
#line 1388 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 15: /* product ::= expression MULTIPLY expression */
#line 190 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy28 = new ProductExpression();
	yylhsminor.yy28->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy28->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1399 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy28 = yylhsminor.yy28;
        break;
      case 16: /* product ::= expression DIVIDE expression */
#line 196 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy28 = new ProductExpression();
	yylhsminor.yy28->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy28->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1411 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy28 = yylhsminor.yy28;
        break;
      case 17: /* product ::= product MULTIPLY expression */
#line 202 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy28 = yymsp[-2].minor.yy28;
	yylhsminor.yy28->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1421 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy28 = yylhsminor.yy28;
        break;
      case 18: /* product ::= product DIVIDE expression */
#line 206 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy28 = yymsp[-2].minor.yy28;
	yylhsminor.yy28->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1431 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy28 = yylhsminor.yy28;
        break;
      case 19: /* expression ::= conjunction */
#line 217 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy45;
}
#line 1440 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 20: /* conjunction ::= expression AND expression */
#line 220 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy45 = new ConjunctionExpression();
	yylhsminor.yy45->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy45->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1451 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy45 = yylhsminor.yy45;
        break;
      case 21: /* conjunction ::= conjunction AND expression */
#line 226 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy45 = yymsp[-2].minor.yy45;
	yylhsminor.yy45->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1461 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy45 = yylhsminor.yy45;
        break;
      case 22: /* expression ::= disjunction */
#line 237 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy21;
}
#line 1470 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 23: /* disjunction ::= expression OR expression */
#line 240 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy21 = new DisjunctionExpression();
	yylhsminor.yy21->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy21->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1481 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy21 = yylhsminor.yy21;
        break;
      case 24: /* disjunction ::= disjunction OR expression */
#line 246 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy21 = yymsp[-2].minor.yy21;
	yylhsminor.yy21->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1491 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy21 = yylhsminor.yy21;
        break;
      case 25: /* expression ::= NOT expression */
{  yy_destructor(yypParser,16,&yymsp[-1].minor);
#line 252 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-1].minor.yy64 = new UnaryNotExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1501 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
        break;
      case 26: /* expression ::= MINUS expression */
{  yy_destructor(yypParser,12,&yymsp[-1].minor);
#line 257 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-1].minor.yy64 = new UnaryMinusExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1510 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
        break;
      case 27: /* expression ::= expression EXP expression */
#line 262 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new ExponentiationExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1518 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,15,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 28: /* expression ::= expression EQUAL expression */
#line 268 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_equal);
}
#line 1527 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 29: /* expression ::= expression NOT_EQUAL expression */
#line 271 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_not_equal);
}
#line 1536 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 30: /* expression ::= expression GREATER expression */
#line 274 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_greater);
}
#line 1545 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 31: /* expression ::= expression GREATER_EQUAL expression */
#line 277 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_greater_equal);
}
#line 1554 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 32: /* expression ::= expression LESS expression */
#line 280 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_less);
}
#line 1563 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 33: /* expression ::= expression LESS_EQUAL expression */
#line 283 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_less_equal);
}
#line 1572 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 34: /* assignment ::= IDENTIFIER ASSIGN expression SEMICOLON */
#line 295 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-3].minor.yy0 and yymsp[-1].minor.yy64 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy64);
	yymsp[-1].minor.yy64 = nullptr;

	parseTree->CreateVariable(std::move(name), parseTree->GetExpressionValue(e_temp.get()));
}
#line 1589 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,23,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 35: /* assignment ::= IDENTIFIER ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 307 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy0 and yymsp[-2].minor.yy64 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-5].minor.yy0;
	delete yymsp[-5].minor.yy0;
	yymsp[-5].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64 = nullptr;

	parseTree->CreateVariable(std::move(name), std::move(e_temp));
}
#line 1606 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,23,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-3].minor);
  yy_destructor(yypParser,25,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 36: /* reaction ::= reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON */
#line 321 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy81, yymsp[-3].minor.yy81 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionSide>(yymsp[-5].minor.yy81);
	auto products_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy81);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy29);
	yymsp[-1].minor.yy29 = nullptr;
	yymsp[-5].minor.yy81 = nullptr;
	yymsp[-3].minor.yy81 = nullptr;

	parseTree->CreateReaction(std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1626 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 37: /* reaction ::= IDENTIFIER COMMA reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON */
#line 334 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy81, yymsp[-3].minor.yy81 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionSide>(yymsp[-5].minor.yy81);
	auto products_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy81);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy29);
	identifier name = *yymsp[-7].minor.yy0; 
	yymsp[-1].minor.yy29 = nullptr;
	yymsp[-5].minor.yy81 = nullptr;
	yymsp[-3].minor.yy81 = nullptr;
	delete yymsp[-7].minor.yy0;
	yymsp[-7].minor.yy0 = nullptr;

	parseTree->CreateReaction(std::move(name), std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1648 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-6].minor);
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 38: /* reactionSpecifiers ::= reactionSpecifier */
#line 355 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rss_temp = std::make_unique<ReactionSpecifiers>();
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy74);
	yymsp[0].minor.yy74 = nullptr;
	yylhsminor.yy29 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy29 = rss_temp.release();
}
#line 1664 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy29 = yylhsminor.yy29;
        break;
      case 39: /* reactionSpecifiers ::= reactionSpecifiers COMMA reactionSpecifier */
#line 363 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-2].minor.yy29);
	yymsp[-2].minor.yy29 = nullptr;
	yylhsminor.yy29 = nullptr;
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy74);
	yymsp[0].minor.yy74 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy29 = rss_temp.release();
}
#line 1678 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy29 = yylhsminor.yy29;
        break;
      case 40: /* reactionSpecifier ::= expression */
#line 378 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64 = nullptr;
	yylhsminor.yy74 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy74 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::make_unique<NumberExpression>(value));
}
#line 1691 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy74 = yylhsminor.yy74;
        break;
      case 41: /* reactionSpecifier ::= IDENTIFIER COLON expression */
#line 386 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64 = nullptr;
	yylhsminor.yy74 = nullptr;
	identifier name = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy74 = new ReactionSpecifier(name, std::make_unique<NumberExpression>(value));
}
#line 1706 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-2].minor.yy74 = yylhsminor.yy74;
        break;
      case 42: /* reactionSpecifier ::= LEFT_SQUARE expression RIGHT_SQUARE */
{  yy_destructor(yypParser,24,&yymsp[-2].minor);
#line 397 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy64);
	yymsp[-1].minor.yy64 = nullptr;
	yymsp[-2].minor.yy74 = nullptr;
	yymsp[-2].minor.yy74 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::move(e_temp));
}
#line 1719 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      case 43: /* reactionSide ::= */
#line 409 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[1].minor.yy81 = new ReactionSide();
}
#line 1728 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
        break;
      case 44: /* reactionSide ::= reactionComponent */
#line 412 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionComponent>(yymsp[0].minor.yy75);
	yymsp[0].minor.yy75 = nullptr;

	yylhsminor.yy81 = new ReactionSide();
	yylhsminor.yy81->PushBack(std::move(rc_temp));
}
#line 1739 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy81 = yylhsminor.yy81;
        break;
      case 45: /* reactionSide ::= reactionSide PLUS reactionComponent */
#line 419 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy81 = yymsp[-2].minor.yy81;
	yymsp[-2].minor.yy81 = nullptr;
	auto rc_temp = std::unique_ptr<ReactionComponent>(yymsp[0].minor.yy75);
	yymsp[0].minor.yy75 = nullptr;

	yylhsminor.yy81->PushBack(std::move(rc_temp));
}
#line 1752 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy81 = yylhsminor.yy81;
        break;
      case 46: /* reactionSide ::= expression PLUS expression */
#line 428 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	delete(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64=nullptr;
	delete(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1765 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 47: /* reactionSide ::= reactionSide PLUS expression */
#line 436 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	delete(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64=nullptr;
	delete(yymsp[-2].minor.yy81);
	yymsp[-2].minor.yy81=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1777 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 48: /* reactionComponent ::= IDENTIFIER */
#line 449 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	yylhsminor.yy75 = nullptr;

	yylhsminor.yy75 = new ReactionComponent(state, 1, false);
}
#line 1790 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy75 = yylhsminor.yy75;
        break;
      case 49: /* reactionComponent ::= IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 458 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	yylhsminor.yy75 = nullptr;

	yylhsminor.yy75 = new ReactionComponent(state, 1, true);
}
#line 1803 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
  yymsp[-2].minor.yy75 = yylhsminor.yy75;
        break;
      case 50: /* reactionComponent ::= expression MULTIPLY IDENTIFIER */
#line 467 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64 = nullptr;
	yylhsminor.yy75 = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy75 = new ReactionComponent(state, stochiometry, false);
}
#line 1821 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy75 = yylhsminor.yy75;
        break;
      case 51: /* reactionComponent ::= expression MULTIPLY IDENTIFIER LEFT_SQUARE RIGHT_SQUARE */
#line 479 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-4].minor.yy64);
	yymsp[-4].minor.yy64 = nullptr;
	yylhsminor.yy75 = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy75 = new ReactionComponent(state, stochiometry, true);
}
#line 1838 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-3].minor);
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
  yymsp[-4].minor.yy75 = yylhsminor.yy75;
        break;
      case 52: /* reactionComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionSide COLON reactionSide RIGHT_SQUARE */
{  yy_destructor(yypParser,24,&yymsp[-6].minor);
#line 491 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-5].minor.yy64);
	yymsp[-5].minor.yy64 = nullptr;
	auto s1_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy81);
	auto s2_temp = std::unique_ptr<ReactionSide>(yymsp[-1].minor.yy81);
	yymsp[-3].minor.yy81 = nullptr;
	yymsp[-1].minor.yy81 = nullptr;
	yymsp[-6].minor.yy75 = nullptr;

	identifier state = parseTree->CreateChoice(std::move(e_temp), std::move(s1_temp), std::move(s2_temp));
	yymsp[-6].minor.yy75 = new ReactionComponent(state, 1, false);
}
#line 1859 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-4].minor);
  yy_destructor(yypParser,21,&yymsp[-2].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      default:
      /* (53) model ::= statements */ yytestcase(yyruleno==53);
      /* (54) statements ::= statements statement */ yytestcase(yyruleno==54);
      /* (55) statements ::= */ yytestcase(yyruleno==55);
      /* (56) statement ::= assignment (OPTIMIZED OUT) */ assert(yyruleno!=56);
      /* (57) statement ::= reaction (OPTIMIZED OUT) */ assert(yyruleno!=57);
      /* (58) statement ::= error (OPTIMIZED OUT) */ assert(yyruleno!=58);
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
#line 1918 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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