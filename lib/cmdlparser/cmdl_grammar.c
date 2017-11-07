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
#define YYNOCODE 50
#define YYACTIONTYPE unsigned short int
#define cmdl_internal_ParseTOKENTYPE TerminalSymbol*
typedef union {
  int yyinit;
  cmdl_internal_ParseTOKENTYPE yy0;
  ReactionSide* yy1;
  FunctionArguments* yy9;
  ReactionComponent* yy15;
  ReactionSpecifier* yy18;
  IExpression* yy24;
  ProductExpression* yy25;
  DisjunctionExpression* yy39;
  ReactionSpecifiers* yy43;
  ConditionalExpression* yy49;
  ConjunctionExpression* yy75;
  std::vector<number>* yy76;
  SumExpression* yy98;
  int yy99;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define cmdl_internal_ParseARG_SDECL CmdlParseTree* parseTree;
#define cmdl_internal_ParseARG_PDECL ,CmdlParseTree* parseTree
#define cmdl_internal_ParseARG_FETCH CmdlParseTree* parseTree = yypParser->parseTree
#define cmdl_internal_ParseARG_STORE yypParser->parseTree = parseTree
#define YYERRORSYMBOL 30
#define YYERRSYMDT yy99
#define YYNSTATE             106
#define YYNRULE              66
#define YY_MAX_SHIFT         105
#define YY_MIN_SHIFTREDUCE   136
#define YY_MAX_SHIFTREDUCE   201
#define YY_MIN_REDUCE        202
#define YY_MAX_REDUCE        267
#define YY_ERROR_ACTION      268
#define YY_ACCEPT_ACTION     269
#define YY_NO_ACTION         270
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
#define YY_ACTTAB_COUNT (703)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    41,   35,   34,   32,   31,   30,   29,   28,   27,   39,
 /*    10 */    38,   37,   36,   33,   52,  185,  207,  211,  216,  221,
 /*    20 */   224,  269,    1,  178,   77,  240,   41,   35,   34,   32,
 /*    30 */    31,   30,   29,   28,   27,   17,   38,   10,   36,   33,
 /*    40 */    37,   36,   33,   86,   12,   40,   15,   87,   41,   35,
 /*    50 */    34,   32,   31,   30,   29,   28,   27,   39,   38,   37,
 /*    60 */    36,   33,   56,   88,  207,  211,  216,  221,  224,   96,
 /*    70 */   193,   97,  190,  170,   41,   35,   34,   32,   31,   30,
 /*    80 */    29,   28,   27,   39,   38,   37,   36,   33,   41,   35,
 /*    90 */    34,   32,   31,   30,   29,   28,   27,   39,   38,   37,
 /*   100 */    36,   33,  137,  172,   43,  173,   96,   40,   89,  188,
 /*   110 */    41,   35,   34,   32,   31,   30,   29,   28,   27,   39,
 /*   120 */    38,   37,   36,   33,   11,  194,   11,   91,  139,   44,
 /*   130 */    93,   41,   35,   34,   32,   31,   30,   29,   28,   27,
 /*   140 */    39,   38,   37,   36,   33,    5,   35,   34,   32,   31,
 /*   150 */    30,   29,   28,   27,   39,   38,   37,   36,   33,   41,
 /*   160 */    35,   34,   32,   31,   30,   29,   28,   27,   17,   38,
 /*   170 */    10,   36,   33,    9,  262,   55,    9,  207,  211,  216,
 /*   180 */   221,  224,  262,  262,   79,   33,  186,  246,    2,  262,
 /*   190 */   171,    3,  262,    9,   12,   41,   35,   34,   32,   31,
 /*   200 */    30,   29,   28,   27,    7,   38,   10,   36,   33,  204,
 /*   210 */    34,   32,   31,   30,   29,   28,   27,   39,   38,   37,
 /*   220 */    36,   33,   32,   31,   30,   29,   28,   27,   39,   38,
 /*   230 */    37,   36,   33,  268,  268,  268,  268,  268,  268,   39,
 /*   240 */    38,   37,   36,   33,  261,   55,  204,  207,  211,  216,
 /*   250 */   221,  224,    9,    9,   78,  204,   21,  246,  204,    9,
 /*   260 */    22,   72,  138,   42,    8,  204,  192,  204,   16,    4,
 /*   270 */   204,   94,   92,   90,   55,  204,  207,  211,  216,  221,
 /*   280 */   224,  204,  204,   82,  204,  204,  246,  204,   55,  204,
 /*   290 */   207,  211,  216,  221,  224,  204,  204,   84,  204,  204,
 /*   300 */   246,  204,  204,   47,  204,  207,  211,  216,  221,  224,
 /*   310 */   204,  204,   85,  204,  204,  246,  204,  204,   55,  204,
 /*   320 */   207,  211,  216,  221,  224,  204,  204,   83,  204,   52,
 /*   330 */   246,  207,  211,  216,  221,  224,  204,  204,  204,   80,
 /*   340 */   240,  248,  204,   37,   36,   33,  204,  204,  204,  204,
 /*   350 */   204,  248,  248,   21,  204,  248,  248,   22,   71,  138,
 /*   360 */    42,  204,  204,  204,  204,   16,  204,   59,   94,  207,
 /*   370 */   211,  216,  221,  224,  204,  204,  204,  204,  204,  247,
 /*   380 */   204,  204,  204,   95,  204,  207,  211,  216,  221,  224,
 /*   390 */   204,  204,  204,  204,  204,  257,  204,   52,  253,  207,
 /*   400 */   211,  216,  221,  224,  204,  204,   12,  253,  241,    6,
 /*   410 */    13,   45,  204,  253,  204,   12,  204,  253,  253,   21,
 /*   420 */    45,  253,  253,   22,  105,  138,   42,  204,   21,  204,
 /*   430 */   204,  204,   22,   81,  138,   42,  204,  204,  204,  204,
 /*   440 */    14,   21,  204,  204,  204,   22,  105,  138,   42,  204,
 /*   450 */   204,  204,   49,   18,  207,  211,  216,  221,  224,  204,
 /*   460 */   204,  204,  204,  204,   46,  204,  207,  211,  216,  221,
 /*   470 */   224,  204,   53,  204,  207,  211,  216,  221,  224,   54,
 /*   480 */   204,  207,  211,  216,  221,  224,   70,  204,  207,  211,
 /*   490 */   216,  221,  224,  204,  204,  204,   48,  204,  207,  211,
 /*   500 */   216,  221,  224,  204,   73,  204,  207,  211,  216,  221,
 /*   510 */   224,   98,  204,  207,  211,  216,  221,  224,   99,  204,
 /*   520 */   207,  211,  216,  221,  224,  204,  204,  204,  100,  204,
 /*   530 */   207,  211,  216,  221,  224,  204,   62,  204,  207,  211,
 /*   540 */   216,  221,  224,   60,  204,  207,  211,  216,  221,  224,
 /*   550 */   101,  204,  207,  211,  216,  221,  224,  204,  204,  204,
 /*   560 */    74,  204,  207,  211,  216,  221,  224,  204,   63,  204,
 /*   570 */   207,  211,  216,  221,  224,   64,  204,  207,  211,  216,
 /*   580 */   221,  224,   65,  204,  207,  211,  216,  221,  224,  204,
 /*   590 */   204,  204,   66,  204,  207,  211,  216,  221,  224,  204,
 /*   600 */    67,  204,  207,  211,  216,  221,  224,   68,  204,  207,
 /*   610 */   211,  216,  221,  224,  102,  204,  207,  211,  216,  221,
 /*   620 */   224,  204,  204,  204,   69,  204,  207,  211,  216,  221,
 /*   630 */   224,  204,   61,  204,  207,  211,  216,  221,  224,  103,
 /*   640 */   204,  207,  211,  216,  221,  224,  104,  204,  207,  211,
 /*   650 */   216,  221,  224,  204,  204,  204,   75,  204,  207,  211,
 /*   660 */   216,  221,  224,  204,   76,  204,  207,  211,  216,  221,
 /*   670 */   224,   58,  204,  207,  211,  216,  221,  224,   50,  204,
 /*   680 */   207,  211,  216,  221,  224,  204,  204,  204,   51,  204,
 /*   690 */   207,  211,  216,  221,  224,  204,   57,  204,  207,  211,
 /*   700 */   216,  221,  224,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    10 */    12,   13,   14,   15,   31,   18,   33,   34,   35,   36,
 /*    20 */    37,   46,   47,   25,   41,   42,    2,    3,    4,    5,
 /*    30 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*    40 */    13,   14,   15,   44,   19,   21,   21,   44,    2,    3,
 /*    50 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*    60 */    14,   15,   31,   32,   33,   34,   35,   36,   37,   22,
 /*    70 */     1,   25,   25,    1,    2,    3,    4,    5,    6,    7,
 /*    80 */     8,    9,   10,   11,   12,   13,   14,   15,    2,    3,
 /*    90 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   100 */    14,   15,   20,    1,   22,    1,   22,   21,   17,   25,
 /*   110 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   120 */    12,   13,   14,   15,   22,    1,   22,   17,   20,   24,
 /*   130 */    17,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*   140 */    11,   12,   13,   14,   15,    2,    3,    4,    5,    6,
 /*   150 */     7,    8,    9,   10,   11,   12,   13,   14,   15,    2,
 /*   160 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   170 */    13,   14,   15,   11,   30,   31,   11,   33,   34,   35,
 /*   180 */    36,   37,   38,   39,   40,   15,   18,   43,   26,   45,
 /*   190 */     1,   26,   48,   11,   19,    2,    3,    4,    5,    6,
 /*   200 */     7,    8,    9,   10,   22,   12,   13,   14,   15,   49,
 /*   210 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   220 */    14,   15,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   230 */    13,   14,   15,    5,    6,    7,    8,    9,   10,   11,
 /*   240 */    12,   13,   14,   15,    0,   31,   49,   33,   34,   35,
 /*   250 */    36,   37,   11,   11,   40,   49,   12,   43,   49,   11,
 /*   260 */    16,   17,   18,   19,   22,   49,   25,   49,   24,   21,
 /*   270 */    49,   27,   28,   29,   31,   49,   33,   34,   35,   36,
 /*   280 */    37,   49,   49,   40,   49,   49,   43,   49,   31,   49,
 /*   290 */    33,   34,   35,   36,   37,   49,   49,   40,   49,   49,
 /*   300 */    43,   49,   49,   31,   49,   33,   34,   35,   36,   37,
 /*   310 */    49,   49,   40,   49,   49,   43,   49,   49,   31,   49,
 /*   320 */    33,   34,   35,   36,   37,   49,   49,   40,   49,   31,
 /*   330 */    43,   33,   34,   35,   36,   37,   49,   49,   49,   41,
 /*   340 */    42,   11,   49,   13,   14,   15,   49,   49,   49,   49,
 /*   350 */    49,   21,   22,   12,   49,   25,   26,   16,   17,   18,
 /*   360 */    19,   49,   49,   49,   49,   24,   49,   31,   27,   33,
 /*   370 */    34,   35,   36,   37,   49,   49,   49,   49,   49,   43,
 /*   380 */    49,   49,   49,   31,   49,   33,   34,   35,   36,   37,
 /*   390 */    49,   49,   49,   49,   49,   43,   49,   31,   11,   33,
 /*   400 */    34,   35,   36,   37,   49,   49,   19,   11,   42,   22,
 /*   410 */    23,   24,   49,   26,   49,   19,   49,   21,   22,   12,
 /*   420 */    24,   25,   26,   16,   17,   18,   19,   49,   12,   49,
 /*   430 */    49,   49,   16,   17,   18,   19,   49,   49,   49,   49,
 /*   440 */    24,   12,   49,   49,   49,   16,   17,   18,   19,   49,
 /*   450 */    49,   49,   31,   24,   33,   34,   35,   36,   37,   49,
 /*   460 */    49,   49,   49,   49,   31,   49,   33,   34,   35,   36,
 /*   470 */    37,   49,   31,   49,   33,   34,   35,   36,   37,   31,
 /*   480 */    49,   33,   34,   35,   36,   37,   31,   49,   33,   34,
 /*   490 */    35,   36,   37,   49,   49,   49,   31,   49,   33,   34,
 /*   500 */    35,   36,   37,   49,   31,   49,   33,   34,   35,   36,
 /*   510 */    37,   31,   49,   33,   34,   35,   36,   37,   31,   49,
 /*   520 */    33,   34,   35,   36,   37,   49,   49,   49,   31,   49,
 /*   530 */    33,   34,   35,   36,   37,   49,   31,   49,   33,   34,
 /*   540 */    35,   36,   37,   31,   49,   33,   34,   35,   36,   37,
 /*   550 */    31,   49,   33,   34,   35,   36,   37,   49,   49,   49,
 /*   560 */    31,   49,   33,   34,   35,   36,   37,   49,   31,   49,
 /*   570 */    33,   34,   35,   36,   37,   31,   49,   33,   34,   35,
 /*   580 */    36,   37,   31,   49,   33,   34,   35,   36,   37,   49,
 /*   590 */    49,   49,   31,   49,   33,   34,   35,   36,   37,   49,
 /*   600 */    31,   49,   33,   34,   35,   36,   37,   31,   49,   33,
 /*   610 */    34,   35,   36,   37,   31,   49,   33,   34,   35,   36,
 /*   620 */    37,   49,   49,   49,   31,   49,   33,   34,   35,   36,
 /*   630 */    37,   49,   31,   49,   33,   34,   35,   36,   37,   31,
 /*   640 */    49,   33,   34,   35,   36,   37,   31,   49,   33,   34,
 /*   650 */    35,   36,   37,   49,   49,   49,   31,   49,   33,   34,
 /*   660 */    35,   36,   37,   49,   31,   49,   33,   34,   35,   36,
 /*   670 */    37,   31,   49,   33,   34,   35,   36,   37,   31,   49,
 /*   680 */    33,   34,   35,   36,   37,   49,   49,   49,   31,   49,
 /*   690 */    33,   34,   35,   36,   37,   49,   31,   49,   33,   34,
 /*   700 */    35,   36,   37,
};
#define YY_SHIFT_USE_DFLT (703)
#define YY_SHIFT_COUNT    (105)
#define YY_SHIFT_MIN      (-3)
#define YY_SHIFT_MAX      (429)
static const short yy_shift_ofst[] = {
 /*     0 */   703,  244,  341,  341,  341,  341,  341,  416,  416,  341,
 /*    10 */   341,  416,  407,  429,  407,  407,  407,  407,  407,  407,
 /*    20 */   407,  407,  407,  407,  407,  407,  407,  407,  407,  407,
 /*    30 */   407,  407,  407,  407,  407,  407,  407,  407,  407,  407,
 /*    40 */   407,  407,  407,  407,   -3,   -3,   -2,   24,   46,   72,
 /*    50 */    86,  108,  129,  129,  143,  157,  129,  129,  129,  193,
 /*    60 */   206,  206,  217,  228,  228,  228,  228,  228,  228,  217,
 /*    70 */   330,  396,  387,   27,   27,   27,   27,  102,  182,  162,
 /*    80 */   104,   25,  242,  165,  241,  248,   47,   84,   82,   69,
 /*    90 */    91,  124,  110,  105,  113,  170,  168,  189,  170,  170,
 /*   100 */   170,  170,  170,  170,  170,  175,
};
#define YY_REDUCE_USE_DFLT (-26)
#define YY_REDUCE_COUNT (45)
#define YY_REDUCE_MIN   (-25)
#define YY_REDUCE_MAX   (665)
static const short yy_reduce_ofst[] = {
 /*     0 */   -25,  144,  214,  243,  257,  272,  287,  -17,  298,  336,
 /*    10 */   352,  366,   31,  421,  433,  441,  448,  455,  465,  473,
 /*    20 */   480,  487,  497,  505,  512,  519,  529,  537,  544,  551,
 /*    30 */   561,  569,  576,  583,  593,  601,  608,  615,  625,  633,
 /*    40 */   640,  647,  657,  665,   -1,    3,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   263,  245,  245,  245,  245,  245,  245,  268,  268,  268,
 /*    10 */   268,  268,  208,  268,  268,  268,  268,  268,  268,  268,
 /*    20 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    30 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    40 */   268,  268,  268,  268,  250,  250,  268,  268,  268,  268,
 /*    50 */   268,  268,  242,  243,  268,  268,  209,  210,  206,  249,
 /*    60 */   223,  222,  226,  235,  234,  233,  232,  231,  230,  225,
 /*    70 */   212,  202,  202,  215,  214,  213,  212,  268,  268,  268,
 /*    80 */   268,  202,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    90 */   268,  268,  268,  255,  268,  217,  268,  268,  220,  228,
 /*   100 */   227,  219,  229,  218,  217,  202,
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
  "LEFT_SQUARE",   "RIGHT_SQUARE",  "ARROW",         "DOLLAR",      
  "MODEL_NAME",    "INCLUDE",       "error",         "expression",  
  "arguments",     "comparison",    "sum",           "product",     
  "conjunction",   "disjunction",   "assignment",    "reaction",    
  "reactionSide",  "reactionSpecifiers",  "reactionSpecifier",  "reactionComponent",
  "moleculeProperties",  "preprocessorDirective",  "model",         "statements",  
  "statement",   
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
 /*  48 */ "moleculeProperties ::=",
 /*  49 */ "moleculeProperties ::= VALUE",
 /*  50 */ "moleculeProperties ::= moleculeProperties COMMA VALUE",
 /*  51 */ "reactionComponent ::= IDENTIFIER",
 /*  52 */ "reactionComponent ::= IDENTIFIER LEFT_SQUARE moleculeProperties RIGHT_SQUARE",
 /*  53 */ "reactionComponent ::= DOLLAR IDENTIFIER",
 /*  54 */ "reactionComponent ::= DOLLAR IDENTIFIER LEFT_SQUARE moleculeProperties RIGHT_SQUARE",
 /*  55 */ "reactionComponent ::= expression MULTIPLY reactionComponent",
 /*  56 */ "reactionComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionSide COLON reactionSide RIGHT_SQUARE",
 /*  57 */ "preprocessorDirective ::= INCLUDE IDENTIFIER SEMICOLON",
 /*  58 */ "preprocessorDirective ::= MODEL_NAME IDENTIFIER SEMICOLON",
 /*  59 */ "model ::= statements",
 /*  60 */ "statements ::= statements statement",
 /*  61 */ "statements ::=",
 /*  62 */ "statement ::= assignment",
 /*  63 */ "statement ::= reaction",
 /*  64 */ "statement ::= preprocessorDirective",
 /*  65 */ "statement ::= error",
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
    case 27: /* DOLLAR */
    case 28: /* MODEL_NAME */
    case 29: /* INCLUDE */
{
#line 8 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"

	delete (yypminor->yy0);
	(yypminor->yy0) = nullptr;

#line 770 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 31: /* expression */
{
#line 83 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy24);
	(yypminor->yy24) = nullptr;

#line 780 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 32: /* arguments */
{
#line 133 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy9);
	(yypminor->yy9) = nullptr;

#line 790 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 33: /* comparison */
{
#line 117 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy49);
	(yypminor->yy49) = nullptr;

#line 800 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 34: /* sum */
{
#line 154 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy98);
	(yypminor->yy98) = nullptr;

#line 810 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 35: /* product */
{
#line 183 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy25);
	(yypminor->yy25) = nullptr;

#line 820 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 36: /* conjunction */
{
#line 213 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy75);
	(yypminor->yy75) = nullptr;

#line 830 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 37: /* disjunction */
{
#line 233 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy39);
	(yypminor->yy39) = nullptr;

#line 840 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 40: /* reactionSide */
{
#line 405 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy1);
	(yypminor->yy1) = nullptr;

#line 850 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 41: /* reactionSpecifiers */
{
#line 351 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy43);
	(yypminor->yy43) = nullptr;

#line 860 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 42: /* reactionSpecifier */
{
#line 374 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy18);
	(yypminor->yy18) = nullptr;

#line 870 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 43: /* reactionComponent */
{
#line 469 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy15);
	(yypminor->yy15) = nullptr;

#line 880 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
      break;
    case 44: /* moleculeProperties */
{
#line 445 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
 
	delete (yypminor->yy76);
	(yypminor->yy76) = nullptr;

#line 890 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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
#line 1071 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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
  { 31, -1 },
  { 31, -4 },
  { 31, -1 },
  { 31, -3 },
  { 33, -5 },
  { 31, -1 },
  { 32, 0 },
  { 32, -1 },
  { 32, -3 },
  { 31, -1 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 31, -1 },
  { 35, -3 },
  { 35, -3 },
  { 35, -3 },
  { 35, -3 },
  { 31, -1 },
  { 36, -3 },
  { 36, -3 },
  { 31, -1 },
  { 37, -3 },
  { 37, -3 },
  { 31, -2 },
  { 31, -2 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 31, -3 },
  { 38, -4 },
  { 38, -6 },
  { 39, -6 },
  { 39, -8 },
  { 41, -1 },
  { 41, -3 },
  { 42, -1 },
  { 42, -3 },
  { 42, -3 },
  { 40, 0 },
  { 40, -1 },
  { 40, -3 },
  { 40, -3 },
  { 40, -3 },
  { 44, 0 },
  { 44, -1 },
  { 44, -3 },
  { 43, -1 },
  { 43, -4 },
  { 43, -2 },
  { 43, -5 },
  { 43, -3 },
  { 43, -7 },
  { 45, -3 },
  { 45, -3 },
  { 46, -1 },
  { 47, -2 },
  { 47, 0 },
  { 48, -1 },
  { 48, -1 },
  { 48, -1 },
  { 48, -1 },
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
	yylhsminor.yy24 = new VariableExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1281 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy24 = yylhsminor.yy24;
        break;
      case 1: /* expression ::= IDENTIFIER LEFT_ROUND arguments RIGHT_ROUND */
#line 92 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto func = new FunctionExpression(*yymsp[-3].minor.yy0);
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yylhsminor.yy24 = nullptr;
	for(auto& argument : *yymsp[-1].minor.yy9)
	{
		func->PushBack(std::move(argument));
	}
	delete yymsp[-1].minor.yy9;
	yymsp[-1].minor.yy9 = nullptr;
	yylhsminor.yy24 = func;
	func = nullptr;
}
#line 1300 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,20,&yymsp[0].minor);
  yymsp[-3].minor.yy24 = yylhsminor.yy24;
        break;
      case 2: /* expression ::= VALUE */
#line 107 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new NumberExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1312 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy24 = yylhsminor.yy24;
        break;
      case 3: /* expression ::= LEFT_ROUND expression RIGHT_ROUND */
{  yy_destructor(yypParser,19,&yymsp[-2].minor);
#line 112 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-2].minor.yy24 = yymsp[-1].minor.yy24;
}
#line 1321 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
        break;
      case 4: /* comparison ::= expression QUESTIONMARK expression COLON expression */
#line 121 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy49 = new ConditionalExpression(std::unique_ptr<IExpression>(yymsp[-4].minor.yy24), std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
	yymsp[-2].minor.yy24 = nullptr;
	yymsp[0].minor.yy24 = nullptr;
	yymsp[-4].minor.yy24 = nullptr;
}
#line 1333 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-4].minor.yy49 = yylhsminor.yy49;
        break;
      case 5: /* expression ::= comparison */
#line 127 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = yymsp[0].minor.yy49;
}
#line 1343 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy24 = yylhsminor.yy24;
        break;
      case 6: /* arguments ::= */
#line 137 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[1].minor.yy9 = new FunctionArguments();
}
#line 1351 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
        break;
      case 7: /* arguments ::= expression */
#line 140 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy9 = new FunctionArguments();
	yylhsminor.yy9->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy24));
	yymsp[0].minor.yy24 = nullptr;
}
#line 1360 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy9 = yylhsminor.yy9;
        break;
      case 8: /* arguments ::= arguments COMMA expression */
#line 145 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy9 = yymsp[-2].minor.yy9;
	yymsp[-2].minor.yy9 = nullptr;
	yylhsminor.yy9->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy24));
	yymsp[0].minor.yy24 = nullptr;
}
#line 1371 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy9 = yylhsminor.yy9;
        break;
      case 9: /* expression ::= sum */
#line 158 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = yymsp[0].minor.yy98;
}
#line 1380 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy24 = yylhsminor.yy24;
        break;
      case 10: /* sum ::= expression PLUS expression */
#line 161 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy98 = new SumExpression();
	yylhsminor.yy98->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy24));
	yylhsminor.yy98->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1390 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy98 = yylhsminor.yy98;
        break;
      case 11: /* sum ::= expression MINUS expression */
#line 166 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy98 = new SumExpression();
	yylhsminor.yy98->PushBack(false,  std::unique_ptr<IExpression>(yymsp[-2].minor.yy24));
	yylhsminor.yy98->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1401 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy98 = yylhsminor.yy98;
        break;
      case 12: /* sum ::= sum PLUS expression */
#line 171 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy98 = yymsp[-2].minor.yy98;
	yylhsminor.yy98->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1411 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy98 = yylhsminor.yy98;
        break;
      case 13: /* sum ::= sum MINUS expression */
#line 175 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy98 = yymsp[-2].minor.yy98;
	yylhsminor.yy98->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1421 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy98 = yylhsminor.yy98;
        break;
      case 14: /* expression ::= product */
#line 187 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = yymsp[0].minor.yy25;
}
#line 1430 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy24 = yylhsminor.yy24;
        break;
      case 15: /* product ::= expression MULTIPLY expression */
#line 190 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy25 = new ProductExpression();
	yylhsminor.yy25->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy24));
	yylhsminor.yy25->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));

}
#line 1441 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy25 = yylhsminor.yy25;
        break;
      case 16: /* product ::= expression DIVIDE expression */
#line 196 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy25 = new ProductExpression();
	yylhsminor.yy25->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy24));
	yylhsminor.yy25->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));

}
#line 1453 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy25 = yylhsminor.yy25;
        break;
      case 17: /* product ::= product MULTIPLY expression */
#line 202 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy25 = yymsp[-2].minor.yy25;
	yylhsminor.yy25->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1463 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy25 = yylhsminor.yy25;
        break;
      case 18: /* product ::= product DIVIDE expression */
#line 206 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy25 = yymsp[-2].minor.yy25;
	yylhsminor.yy25->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1473 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy25 = yylhsminor.yy25;
        break;
      case 19: /* expression ::= conjunction */
#line 217 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = yymsp[0].minor.yy75;
}
#line 1482 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy24 = yylhsminor.yy24;
        break;
      case 20: /* conjunction ::= expression AND expression */
#line 220 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy75 = new ConjunctionExpression();
	yylhsminor.yy75->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy24));
	yylhsminor.yy75->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));

}
#line 1493 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy75 = yylhsminor.yy75;
        break;
      case 21: /* conjunction ::= conjunction AND expression */
#line 226 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy75 = yymsp[-2].minor.yy75;
	yylhsminor.yy75->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1503 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy75 = yylhsminor.yy75;
        break;
      case 22: /* expression ::= disjunction */
#line 237 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = yymsp[0].minor.yy39;
}
#line 1512 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy24 = yylhsminor.yy24;
        break;
      case 23: /* disjunction ::= expression OR expression */
#line 240 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy39 = new DisjunctionExpression();
	yylhsminor.yy39->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy24));
	yylhsminor.yy39->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));

}
#line 1523 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy39 = yylhsminor.yy39;
        break;
      case 24: /* disjunction ::= disjunction OR expression */
#line 246 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy39 = yymsp[-2].minor.yy39;
	yylhsminor.yy39->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1533 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy39 = yylhsminor.yy39;
        break;
      case 25: /* expression ::= NOT expression */
{  yy_destructor(yypParser,16,&yymsp[-1].minor);
#line 252 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-1].minor.yy24 = new UnaryNotExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1543 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
        break;
      case 26: /* expression ::= MINUS expression */
{  yy_destructor(yypParser,12,&yymsp[-1].minor);
#line 257 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[-1].minor.yy24 = new UnaryMinusExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1552 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
        break;
      case 27: /* expression ::= expression EXP expression */
#line 262 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new ExponentiationExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24));
}
#line 1560 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,15,&yymsp[-1].minor);
  yymsp[-2].minor.yy24 = yylhsminor.yy24;
        break;
      case 28: /* expression ::= expression EQUAL expression */
#line 268 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24), ComparisonExpression::type_equal);
}
#line 1569 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy24 = yylhsminor.yy24;
        break;
      case 29: /* expression ::= expression NOT_EQUAL expression */
#line 271 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24), ComparisonExpression::type_not_equal);
}
#line 1578 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy24 = yylhsminor.yy24;
        break;
      case 30: /* expression ::= expression GREATER expression */
#line 274 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24), ComparisonExpression::type_greater);
}
#line 1587 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy24 = yylhsminor.yy24;
        break;
      case 31: /* expression ::= expression GREATER_EQUAL expression */
#line 277 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24), ComparisonExpression::type_greater_equal);
}
#line 1596 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy24 = yylhsminor.yy24;
        break;
      case 32: /* expression ::= expression LESS expression */
#line 280 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24), ComparisonExpression::type_less);
}
#line 1605 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy24 = yylhsminor.yy24;
        break;
      case 33: /* expression ::= expression LESS_EQUAL expression */
#line 283 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy24 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy24), std::unique_ptr<IExpression>(yymsp[0].minor.yy24), ComparisonExpression::type_less_equal);
}
#line 1614 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy24 = yylhsminor.yy24;
        break;
      case 34: /* assignment ::= IDENTIFIER ASSIGN expression SEMICOLON */
#line 295 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-3].minor.yy0 and yymsp[-1].minor.yy24 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy24);
	yymsp[-1].minor.yy24 = nullptr;

	parseTree->CreateVariable(std::move(name), parseTree->GetExpressionValue(e_temp.get()));
}
#line 1631 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,23,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 35: /* assignment ::= IDENTIFIER ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 307 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy0 and yymsp[-2].minor.yy24 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-5].minor.yy0;
	delete yymsp[-5].minor.yy0;
	yymsp[-5].minor.yy0 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy24);
	yymsp[-2].minor.yy24 = nullptr;

	parseTree->CreateVariable(std::move(name), std::move(e_temp));
}
#line 1648 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,23,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-3].minor);
  yy_destructor(yypParser,25,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 36: /* reaction ::= reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON */
#line 321 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy1, yymsp[-3].minor.yy1 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionSide>(yymsp[-5].minor.yy1);
	auto products_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy1);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy43);
	yymsp[-1].minor.yy43 = nullptr;
	yymsp[-5].minor.yy1 = nullptr;
	yymsp[-3].minor.yy1 = nullptr;

	parseTree->CreateReaction(std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1668 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 37: /* reaction ::= IDENTIFIER COMMA reactionSide ARROW reactionSide COMMA reactionSpecifiers SEMICOLON */
#line 334 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy1, yymsp[-3].minor.yy1 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionSide>(yymsp[-5].minor.yy1);
	auto products_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy1);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy43);
	identifier name = *yymsp[-7].minor.yy0; 
	yymsp[-1].minor.yy43 = nullptr;
	yymsp[-5].minor.yy1 = nullptr;
	yymsp[-3].minor.yy1 = nullptr;
	delete yymsp[-7].minor.yy0;
	yymsp[-7].minor.yy0 = nullptr;

	parseTree->CreateReaction(std::move(name), std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1690 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-6].minor);
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 38: /* reactionSpecifiers ::= reactionSpecifier */
#line 355 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rss_temp = std::make_unique<ReactionSpecifiers>();
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy18);
	yymsp[0].minor.yy18 = nullptr;
	yylhsminor.yy43 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy43 = rss_temp.release();
}
#line 1706 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy43 = yylhsminor.yy43;
        break;
      case 39: /* reactionSpecifiers ::= reactionSpecifiers COMMA reactionSpecifier */
#line 363 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-2].minor.yy43);
	yymsp[-2].minor.yy43 = nullptr;
	yylhsminor.yy43 = nullptr;
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy18);
	yymsp[0].minor.yy18 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy43 = rss_temp.release();
}
#line 1720 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy43 = yylhsminor.yy43;
        break;
      case 40: /* reactionSpecifier ::= expression */
#line 378 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy24);
	yymsp[0].minor.yy24 = nullptr;
	yylhsminor.yy18 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy18 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::make_unique<NumberExpression>(value));
}
#line 1733 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy18 = yylhsminor.yy18;
        break;
      case 41: /* reactionSpecifier ::= IDENTIFIER COLON expression */
#line 386 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy24);
	yymsp[0].minor.yy24 = nullptr;
	yylhsminor.yy18 = nullptr;
	identifier name = *yymsp[-2].minor.yy0;
	delete yymsp[-2].minor.yy0;
	yymsp[-2].minor.yy0 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy18 = new ReactionSpecifier(name, std::make_unique<NumberExpression>(value));
}
#line 1748 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-2].minor.yy18 = yylhsminor.yy18;
        break;
      case 42: /* reactionSpecifier ::= LEFT_SQUARE expression RIGHT_SQUARE */
{  yy_destructor(yypParser,24,&yymsp[-2].minor);
#line 397 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy24);
	yymsp[-1].minor.yy24 = nullptr;
	yymsp[-2].minor.yy18 = nullptr;
	yymsp[-2].minor.yy18 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::move(e_temp));
}
#line 1761 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      case 43: /* reactionSide ::= */
#line 409 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[1].minor.yy1 = new ReactionSide();
}
#line 1770 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
        break;
      case 44: /* reactionSide ::= reactionComponent */
#line 412 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionComponent>(yymsp[0].minor.yy15);
	yymsp[0].minor.yy15 = nullptr;

	yylhsminor.yy1 = new ReactionSide();
	yylhsminor.yy1->PushBack(std::move(rc_temp));
}
#line 1781 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy1 = yylhsminor.yy1;
        break;
      case 45: /* reactionSide ::= reactionSide PLUS reactionComponent */
#line 419 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy1 = yymsp[-2].minor.yy1;
	yymsp[-2].minor.yy1 = nullptr;
	auto rc_temp = std::unique_ptr<ReactionComponent>(yymsp[0].minor.yy15);
	yymsp[0].minor.yy15 = nullptr;

	yylhsminor.yy1->PushBack(std::move(rc_temp));
}
#line 1794 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy1 = yylhsminor.yy1;
        break;
      case 46: /* reactionSide ::= expression PLUS expression */
#line 428 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	delete(yymsp[-2].minor.yy24);
	yymsp[-2].minor.yy24=nullptr;
	delete(yymsp[0].minor.yy24);
	yymsp[0].minor.yy24=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1807 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 47: /* reactionSide ::= reactionSide PLUS expression */
#line 436 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	delete(yymsp[0].minor.yy24);
	yymsp[0].minor.yy24=nullptr;
	delete(yymsp[-2].minor.yy1);
	yymsp[-2].minor.yy1=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1819 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 48: /* moleculeProperties ::= */
#line 449 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yymsp[1].minor.yy76 = new std::vector<number>();
}
#line 1827 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
        break;
      case 49: /* moleculeProperties ::= VALUE */
#line 452 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy76 = new std::vector<number>();
	yylhsminor.yy76->push_back(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1837 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy76 = yylhsminor.yy76;
        break;
      case 50: /* moleculeProperties ::= moleculeProperties COMMA VALUE */
#line 458 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	yylhsminor.yy76 = yymsp[-2].minor.yy76;
	yymsp[-2].minor.yy76 = nullptr;
	yylhsminor.yy76->push_back(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1849 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy76 = yylhsminor.yy76;
        break;
      case 51: /* reactionComponent ::= IDENTIFIER */
#line 473 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	yylhsminor.yy15 = nullptr;

	yylhsminor.yy15 = new ReactionComponent(state, 1, false);
}
#line 1863 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yymsp[0].minor.yy15 = yylhsminor.yy15;
        break;
      case 52: /* reactionComponent ::= IDENTIFIER LEFT_SQUARE moleculeProperties RIGHT_SQUARE */
#line 481 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yylhsminor.yy15 = nullptr;
	auto as_temp = std::unique_ptr<std::vector<number> >(yymsp[-1].minor.yy76);
	yymsp[-1].minor.yy76 = nullptr;

	yylhsminor.yy15 = new ReactionComponent(state, 1, false, std::move(as_temp));
}
#line 1878 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-2].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
  yymsp[-3].minor.yy15 = yylhsminor.yy15;
        break;
      case 53: /* reactionComponent ::= DOLLAR IDENTIFIER */
{  yy_destructor(yypParser,27,&yymsp[-1].minor);
#line 492 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy0;
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
	yymsp[-1].minor.yy15 = nullptr;

	yymsp[-1].minor.yy15 = new ReactionComponent(state, 1, true);
}
#line 1894 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
}
        break;
      case 54: /* reactionComponent ::= DOLLAR IDENTIFIER LEFT_SQUARE moleculeProperties RIGHT_SQUARE */
{  yy_destructor(yypParser,27,&yymsp[-4].minor);
#line 501 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier state = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yymsp[-4].minor.yy15 = nullptr;
	auto as_temp = std::unique_ptr<std::vector<number> >(yymsp[-1].minor.yy76);
	yymsp[-1].minor.yy76 = nullptr;

	yymsp[-4].minor.yy15 = new ReactionComponent(state, 1, true, std::move(as_temp));
}
#line 1910 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-2].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      case 55: /* reactionComponent ::= expression MULTIPLY reactionComponent */
#line 512 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionComponent>(yymsp[0].minor.yy15);
	yymsp[0].minor.yy15 = nullptr;
	yylhsminor.yy15 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy24);
	yymsp[-2].minor.yy24 = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	if(stochiometry<=0)
		throw std::exception("Stochiometry must be positive.");
	rc_temp->SetStochiometry(static_cast<stochsim::Stochiometry>(rc_temp->GetStochiometry()*stochiometry));
	yylhsminor.yy15 = rc_temp.release();
}
#line 1930 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy15 = yylhsminor.yy15;
        break;
      case 56: /* reactionComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionSide COLON reactionSide RIGHT_SQUARE */
{  yy_destructor(yypParser,24,&yymsp[-6].minor);
#line 526 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-5].minor.yy24);
	yymsp[-5].minor.yy24 = nullptr;
	auto s1_temp = std::unique_ptr<ReactionSide>(yymsp[-3].minor.yy1);
	auto s2_temp = std::unique_ptr<ReactionSide>(yymsp[-1].minor.yy1);
	yymsp[-3].minor.yy1 = nullptr;
	yymsp[-1].minor.yy1 = nullptr;
	yymsp[-6].minor.yy15 = nullptr;

	identifier state = parseTree->CreateChoice(std::move(e_temp), std::move(s1_temp), std::move(s2_temp));
	yymsp[-6].minor.yy15 = new ReactionComponent(state, 1, false);
}
#line 1949 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-4].minor);
  yy_destructor(yypParser,21,&yymsp[-2].minor);
  yy_destructor(yypParser,25,&yymsp[0].minor);
}
        break;
      case 57: /* preprocessorDirective ::= INCLUDE IDENTIFIER SEMICOLON */
{  yy_destructor(yypParser,29,&yymsp[-2].minor);
#line 543 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
	identifier fileName = *yymsp[-1].minor.yy0;
	delete yymsp[-1].minor.yy0;
	yymsp[-1].minor.yy0 = nullptr;
	parseTree->IncludeFile(fileName);
}
#line 1964 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
        break;
      case 58: /* preprocessorDirective ::= MODEL_NAME IDENTIFIER SEMICOLON */
{  yy_destructor(yypParser,28,&yymsp[-2].minor);
#line 540 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.y"
{
}
#line 1973 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
  yy_destructor(yypParser,17,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
        break;
      default:
      /* (59) model ::= statements */ yytestcase(yyruleno==59);
      /* (60) statements ::= statements statement */ yytestcase(yyruleno==60);
      /* (61) statements ::= */ yytestcase(yyruleno==61);
      /* (62) statement ::= assignment (OPTIMIZED OUT) */ assert(yyruleno!=62);
      /* (63) statement ::= reaction (OPTIMIZED OUT) */ assert(yyruleno!=63);
      /* (64) statement ::= preprocessorDirective (OPTIMIZED OUT) */ assert(yyruleno!=64);
      /* (65) statement ::= error (OPTIMIZED OUT) */ assert(yyruleno!=65);
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
#line 2032 "C:\\stochsim\\lib\\cmdlparser\\cmdl_grammar.c"
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