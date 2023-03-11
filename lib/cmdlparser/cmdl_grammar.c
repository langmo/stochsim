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
#line 14 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "expression_common.h"
#line 15 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "ComparisonExpression.h"
#line 16 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "ConditionalExpression.h"
#line 17 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "ExponentiationExpression.h"
#line 18 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "ConjunctionExpression.h"
#line 19 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "DisjunctionExpression.h"
#line 20 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "UnaryNotExpression.h"
#line 21 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "NumberExpression.h"
#line 22 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "ProductExpression.h"
#line 23 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "SumExpression.h"
#line 24 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "UnaryMinusExpression.h"
#line 25 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "VariableExpression.h"
#line 26 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "FunctionExpression.h"
#line 28 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "cmdl_symbols.h"
#line 29 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "CmdlParseTree.h"
#line 30 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include "CmdlParser.h"
#line 32 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
#include  <assert.h>
#line 33 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
using namespace expression;
#line 34 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
using namespace cmdlparser;
#line 66 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
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
#define YYNOCODE 56
#define YYACTIONTYPE unsigned short int
#define cmdl_internal_ParseTOKENTYPE TerminalSymbol*
typedef union {
  int yyinit;
  cmdl_internal_ParseTOKENTYPE yy0;
  ReactionRightSide* yy5;
  ReactionLeftComponent* yy14;
  ReactionSpecifiers* yy29;
  MoleculePropertyExpressions* yy30;
  ConjunctionExpression* yy45;
  ProductExpression* yy50;
  IExpression* yy64;
  ConditionalExpression* yy73;
  ReactionRightComponent* yy77;
  SumExpression* yy78;
  DisjunctionExpression* yy87;
  ReactionLeftSide* yy96;
  ReactionSpecifier* yy97;
  identifier* yy100;
  FunctionArguments* yy103;
  MoleculePropertyNames* yy110;
  int yy111;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define cmdl_internal_ParseARG_SDECL CmdlParseTree* parseTree;
#define cmdl_internal_ParseARG_PDECL ,CmdlParseTree* parseTree
#define cmdl_internal_ParseARG_FETCH CmdlParseTree* parseTree = yypParser->parseTree
#define cmdl_internal_ParseARG_STORE yypParser->parseTree = parseTree
#define YYERRORSYMBOL 32
#define YYERRSYMDT yy111
#define YYNSTATE             126
#define YYNRULE              83
#define YY_MAX_SHIFT         125
#define YY_MIN_SHIFTREDUCE   162
#define YY_MAX_SHIFTREDUCE   244
#define YY_MIN_REDUCE        245
#define YY_MAX_REDUCE        327
#define YY_ERROR_ACTION      328
#define YY_ACCEPT_ACTION     329
#define YY_NO_ACTION         330
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
#define YY_ACTTAB_COUNT (845)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    49,   42,   41,   39,   38,   37,   36,   35,   34,   46,
 /*    10 */    45,   44,   43,   40,   44,   43,   40,  125,  206,  329,
 /*    20 */     1,   49,   42,   41,   39,   38,   37,   36,   35,   34,
 /*    30 */    21,   45,   10,   43,   40,   28,  200,  201,  109,   29,
 /*    40 */   125,  166,   25,  112,   47,   49,   42,   41,   39,   38,
 /*    50 */    37,   36,   35,   34,   46,   45,   44,   43,   40,   12,
 /*    60 */    12,  114,   16,  116,   19,  198,   49,   42,   41,   39,
 /*    70 */    38,   37,   36,   35,   34,   46,   45,   44,   43,   40,
 /*    80 */    49,   42,   41,   39,   38,   37,   36,   35,   34,   46,
 /*    90 */    45,   44,   43,   40,  297,   22,   22,  236,   56,  233,
 /*   100 */   231,   25,  219,  296,   49,   42,   41,   39,   38,   37,
 /*   110 */    36,   35,   34,   46,   45,   44,   43,   40,  106,  237,
 /*   120 */   110,   56,   14,   40,  167,  217,  296,   49,   42,   41,
 /*   130 */    39,   38,   37,   36,   35,   34,   46,   45,   44,   43,
 /*   140 */    40,  107,  322,   88,   70,  162,  252,  256,  261,  266,
 /*   150 */   269,  322,  322,   97,    9,   51,  165,  291,   48,  199,
 /*   160 */    16,  322,  247,  235,  322,   50,  247,   49,   42,   41,
 /*   170 */    39,   38,   37,   36,   35,   34,   46,   45,   44,   43,
 /*   180 */    40,    4,   42,   41,   39,   38,   37,   36,   35,   34,
 /*   190 */    46,   45,   44,   43,   40,   49,   42,   41,   39,   38,
 /*   200 */    37,   36,   35,   34,   21,   45,   10,   43,   40,   49,
 /*   210 */    42,   41,   39,   38,   37,   36,   35,   34,   23,   45,
 /*   220 */    13,   43,   40,   49,   42,   41,   39,   38,   37,   36,
 /*   230 */    35,   34,  247,   45,   13,   43,   40,   49,   42,   41,
 /*   240 */    39,   38,   37,   36,   35,   34,  247,   45,   10,   43,
 /*   250 */    40,  247,   41,   39,   38,   37,   36,   35,   34,   46,
 /*   260 */    45,   44,   43,   40,   39,   38,   37,   36,   35,   34,
 /*   270 */    46,   45,   44,   43,   40,  328,  328,  328,  328,  328,
 /*   280 */   328,   46,   45,   44,   43,   40,  321,    9,    9,   87,
 /*   290 */    67,   11,  252,  256,  261,  266,  269,  247,   28,    3,
 /*   300 */    96,    6,   29,  125,  166,  305,    2,   47,  247,  247,
 /*   310 */   247,  247,   11,    9,  247,   55,  111,   53,   87,   67,
 /*   320 */   247,  252,  256,  261,  266,  269,    7,    5,  247,  101,
 /*   330 */   247,  299,   87,   58,  305,  252,  256,  261,  266,  269,
 /*   340 */   247,   16,  247,  102,    8,   17,  299,   52,  305,   87,
 /*   350 */    67,  247,  252,  256,  261,  266,  269,  247,  247,  247,
 /*   360 */   100,  247,  247,   99,   64,  305,  252,  256,  261,  266,
 /*   370 */   269,  247,  247,  247,  247,   95,  285,   99,   64,  247,
 /*   380 */   252,  256,  261,  266,  269,  247,  247,  247,  247,   98,
 /*   390 */   285,  247,  247,  247,   90,   70,  247,  252,  256,  261,
 /*   400 */   266,  269,  247,  247,  105,  247,  247,  247,  291,   87,
 /*   410 */    75,  247,  252,  256,  261,  266,  269,   87,  113,  247,
 /*   420 */   252,  256,  261,  266,  269,  306,  293,  247,   44,   43,
 /*   430 */    40,   90,   74,  317,  252,  256,  261,  266,  269,  247,
 /*   440 */   247,  293,  247,   99,   64,  292,  252,  256,  261,  266,
 /*   450 */   269,  247,  247,  247,   90,  115,  286,  252,  256,  261,
 /*   460 */   266,  269,  247,  247,  247,  247,  124,   69,  303,  252,
 /*   470 */   256,  261,  266,  269,  247,  247,  247,  247,  247,  247,
 /*   480 */   247,  124,   69,  103,  252,  256,  261,  266,  269,  124,
 /*   490 */    72,  108,  252,  256,  261,  266,  269,   28,  104,  247,
 /*   500 */   247,   29,  125,  166,   20,  247,   47,  247,  247,  247,
 /*   510 */   247,  247,  247,  247,   54,  124,   60,  247,  252,  256,
 /*   520 */   261,  266,  269,  124,   57,  247,  252,  256,  261,  266,
 /*   530 */   269,  124,   65,  247,  252,  256,  261,  266,  269,  247,
 /*   540 */   124,   66,  247,  252,  256,  261,  266,  269,  124,   86,
 /*   550 */   247,  252,  256,  261,  266,  269,  124,   68,  247,  252,
 /*   560 */   256,  261,  266,  269,  124,   89,  247,  252,  256,  261,
 /*   570 */   266,  269,  124,   59,  247,  252,  256,  261,  266,  269,
 /*   580 */   124,   71,  247,  252,  256,  261,  266,  269,  124,   91,
 /*   590 */   247,  252,  256,  261,  266,  269,  124,  117,  247,  252,
 /*   600 */   256,  261,  266,  269,  124,  118,  247,  252,  256,  261,
 /*   610 */   266,  269,  124,  119,  247,  252,  256,  261,  266,  269,
 /*   620 */   124,   78,  247,  252,  256,  261,  266,  269,  124,   76,
 /*   630 */   247,  252,  256,  261,  266,  269,  124,  120,  247,  252,
 /*   640 */   256,  261,  266,  269,  124,   92,  247,  252,  256,  261,
 /*   650 */   266,  269,  124,   79,  247,  252,  256,  261,  266,  269,
 /*   660 */   124,   80,  247,  252,  256,  261,  266,  269,  124,   81,
 /*   670 */   247,  252,  256,  261,  266,  269,  124,   82,  247,  252,
 /*   680 */   256,  261,  266,  269,  124,   83,  247,  252,  256,  261,
 /*   690 */   266,  269,  124,   84,  247,  252,  256,  261,  266,  269,
 /*   700 */   124,  121,  247,  252,  256,  261,  266,  269,  124,   85,
 /*   710 */   247,  252,  256,  261,  266,  269,  124,   77,  247,  252,
 /*   720 */   256,  261,  266,  269,  124,  122,  247,  252,  256,  261,
 /*   730 */   266,  269,  124,  123,  247,  252,  256,  261,  266,  269,
 /*   740 */   124,   93,  247,  252,  256,  261,  266,  269,  124,   94,
 /*   750 */   247,  252,  256,  261,  266,  269,  124,   62,  247,  252,
 /*   760 */   256,  261,  266,  269,  124,   73,  247,  252,  256,  261,
 /*   770 */   266,  269,  124,   61,  247,  252,  256,  261,  266,  269,
 /*   780 */   124,   63,  247,  252,  256,  261,  266,  269,  307,  247,
 /*   790 */    44,   43,   40,  247,  247,  247,  299,  307,   28,  247,
 /*   800 */   307,  307,   29,  125,  166,   18,   16,   47,  247,  247,
 /*   810 */    28,  299,   52,  247,   29,  125,  166,   28,  247,   47,
 /*   820 */   247,   29,  125,  166,   24,  247,   47,   55,  313,  247,
 /*   830 */   247,  247,  247,  247,  247,  247,  247,  313,   16,  247,
 /*   840 */   313,  313,  247,  247,   15,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    10 */    12,   13,   14,   15,   13,   14,   15,   17,   20,   52,
 /*    20 */    53,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    30 */    11,   12,   13,   14,   15,   12,    1,    1,   33,   16,
 /*    40 */    17,   18,   23,   33,   21,    2,    3,    4,    5,    6,
 /*    50 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   24,
 /*    60 */    24,   33,   21,   20,   23,    1,    2,    3,    4,    5,
 /*    70 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*    80 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    90 */    12,   13,   14,   15,   33,   24,   24,    1,   24,   28,
 /*   100 */    28,   23,   28,   33,    2,    3,    4,    5,    6,    7,
 /*   110 */     8,    9,   10,   11,   12,   13,   14,   15,   48,    1,
 /*   120 */    17,   24,   27,   15,   22,   28,   33,    2,    3,    4,
 /*   130 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   140 */    15,   48,   32,   33,   34,   20,   36,   37,   38,   39,
 /*   150 */    40,   41,   42,   43,   11,   27,   22,   47,   24,    1,
 /*   160 */    21,   51,   55,   20,   54,   19,   55,    2,    3,    4,
 /*   170 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   180 */    15,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*   190 */    11,   12,   13,   14,   15,    2,    3,    4,    5,    6,
 /*   200 */     7,    8,    9,   10,   11,   12,   13,   14,   15,    2,
 /*   210 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   220 */    13,   14,   15,    2,    3,    4,    5,    6,    7,    8,
 /*   230 */     9,   10,   55,   12,   13,   14,   15,    2,    3,    4,
 /*   240 */     5,    6,    7,    8,    9,   10,   55,   12,   13,   14,
 /*   250 */    15,   55,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   260 */    12,   13,   14,   15,    5,    6,    7,    8,    9,   10,
 /*   270 */    11,   12,   13,   14,   15,    5,    6,    7,    8,    9,
 /*   280 */    10,   11,   12,   13,   14,   15,    0,   11,   11,   33,
 /*   290 */    34,   11,   36,   37,   38,   39,   40,   55,   12,   23,
 /*   300 */    44,   24,   16,   17,   18,   49,   26,   21,   55,   55,
 /*   310 */    55,   55,   11,   11,   55,   29,   30,   31,   33,   34,
 /*   320 */    55,   36,   37,   38,   39,   40,   24,   26,   55,   44,
 /*   330 */    55,   11,   33,   34,   49,   36,   37,   38,   39,   40,
 /*   340 */    55,   21,   55,   44,   24,   25,   26,   27,   49,   33,
 /*   350 */    34,   55,   36,   37,   38,   39,   40,   55,   55,   55,
 /*   360 */    44,   55,   55,   33,   34,   49,   36,   37,   38,   39,
 /*   370 */    40,   55,   55,   55,   55,   45,   46,   33,   34,   55,
 /*   380 */    36,   37,   38,   39,   40,   55,   55,   55,   55,   45,
 /*   390 */    46,   55,   55,   55,   33,   34,   55,   36,   37,   38,
 /*   400 */    39,   40,   55,   55,   43,   55,   55,   55,   47,   33,
 /*   410 */    34,   55,   36,   37,   38,   39,   40,   33,   34,   55,
 /*   420 */    36,   37,   38,   39,   40,   49,   11,   55,   13,   14,
 /*   430 */    15,   33,   34,   49,   36,   37,   38,   39,   40,   55,
 /*   440 */    55,   26,   55,   33,   34,   47,   36,   37,   38,   39,
 /*   450 */    40,   55,   55,   55,   33,   34,   46,   36,   37,   38,
 /*   460 */    39,   40,   55,   55,   55,   55,   33,   34,   47,   36,
 /*   470 */    37,   38,   39,   40,   55,   55,   55,   55,   55,   55,
 /*   480 */    55,   33,   34,   50,   36,   37,   38,   39,   40,   33,
 /*   490 */    34,   35,   36,   37,   38,   39,   40,   12,   50,   55,
 /*   500 */    55,   16,   17,   18,   19,   55,   21,   55,   55,   55,
 /*   510 */    55,   55,   55,   55,   29,   33,   34,   55,   36,   37,
 /*   520 */    38,   39,   40,   33,   34,   55,   36,   37,   38,   39,
 /*   530 */    40,   33,   34,   55,   36,   37,   38,   39,   40,   55,
 /*   540 */    33,   34,   55,   36,   37,   38,   39,   40,   33,   34,
 /*   550 */    55,   36,   37,   38,   39,   40,   33,   34,   55,   36,
 /*   560 */    37,   38,   39,   40,   33,   34,   55,   36,   37,   38,
 /*   570 */    39,   40,   33,   34,   55,   36,   37,   38,   39,   40,
 /*   580 */    33,   34,   55,   36,   37,   38,   39,   40,   33,   34,
 /*   590 */    55,   36,   37,   38,   39,   40,   33,   34,   55,   36,
 /*   600 */    37,   38,   39,   40,   33,   34,   55,   36,   37,   38,
 /*   610 */    39,   40,   33,   34,   55,   36,   37,   38,   39,   40,
 /*   620 */    33,   34,   55,   36,   37,   38,   39,   40,   33,   34,
 /*   630 */    55,   36,   37,   38,   39,   40,   33,   34,   55,   36,
 /*   640 */    37,   38,   39,   40,   33,   34,   55,   36,   37,   38,
 /*   650 */    39,   40,   33,   34,   55,   36,   37,   38,   39,   40,
 /*   660 */    33,   34,   55,   36,   37,   38,   39,   40,   33,   34,
 /*   670 */    55,   36,   37,   38,   39,   40,   33,   34,   55,   36,
 /*   680 */    37,   38,   39,   40,   33,   34,   55,   36,   37,   38,
 /*   690 */    39,   40,   33,   34,   55,   36,   37,   38,   39,   40,
 /*   700 */    33,   34,   55,   36,   37,   38,   39,   40,   33,   34,
 /*   710 */    55,   36,   37,   38,   39,   40,   33,   34,   55,   36,
 /*   720 */    37,   38,   39,   40,   33,   34,   55,   36,   37,   38,
 /*   730 */    39,   40,   33,   34,   55,   36,   37,   38,   39,   40,
 /*   740 */    33,   34,   55,   36,   37,   38,   39,   40,   33,   34,
 /*   750 */    55,   36,   37,   38,   39,   40,   33,   34,   55,   36,
 /*   760 */    37,   38,   39,   40,   33,   34,   55,   36,   37,   38,
 /*   770 */    39,   40,   33,   34,   55,   36,   37,   38,   39,   40,
 /*   780 */    33,   34,   55,   36,   37,   38,   39,   40,   11,   55,
 /*   790 */    13,   14,   15,   55,   55,   55,   11,   20,   12,   55,
 /*   800 */    23,   24,   16,   17,   18,   19,   21,   21,   55,   55,
 /*   810 */    12,   26,   27,   55,   16,   17,   18,   12,   55,   21,
 /*   820 */    55,   16,   17,   18,   19,   55,   21,   29,   11,   55,
 /*   830 */    55,   55,   55,   55,   55,   55,   55,   20,   21,   55,
 /*   840 */    23,   24,   55,   55,   27,
};
#define YY_SHIFT_USE_DFLT (845)
#define YY_SHIFT_COUNT    (125)
#define YY_SHIFT_MIN      (-2)
#define YY_SHIFT_MAX      (817)
static const short yy_shift_ofst[] = {
 /*     0 */   845,  286,  485,  485,  485,  485,  786,  786,  798,  485,
 /*    10 */   485,  798,  786,  798,   23,   23,   23,  805,   23,   23,
 /*    20 */    23,   23,   23,   23,   23,   23,   23,   23,   23,   23,
 /*    30 */    23,   23,   23,   23,   23,   23,   23,   23,   23,   23,
 /*    40 */    23,   23,   23,   23,   23,   23,   23,   23,   23,   23,
 /*    50 */    23,    0,    0,    0,    0,    0,    0,   -2,   19,   43,
 /*    60 */    64,   78,  102,  125,  165,  165,  179,  193,  165,  165,
 /*    70 */   207,  165,  165,  165,  221,  235,  248,  248,  259,  270,
 /*    80 */   270,  270,  270,  270,  270,  259,  777,  817,  320,  415,
 /*    90 */   785,    1,    1,    1,    1,   35,  277,  280,   36,   41,
 /*   100 */   302,  143,  276,   71,   72,  301,   74,   97,  134,   96,
 /*   110 */   118,  103,   95,  108,  128,  108,  158,  108,  108,  108,
 /*   120 */   108,  108,  108,  108,  139,  146,
};
#define YY_REDUCE_USE_DFLT (-34)
#define YY_REDUCE_COUNT (56)
#define YY_REDUCE_MIN   (-33)
#define YY_REDUCE_MAX   (747)
static const short yy_reduce_ofst[] = {
 /*     0 */   -33,  110,  256,  285,  299,  316,  330,  344,  361,  376,
 /*    10 */   384,  398,  410,  421,  433,  448,  456,  482,  490,  498,
 /*    20 */   507,  515,  523,  531,  539,  547,  555,  563,  571,  579,
 /*    30 */   587,  595,  603,  611,  619,  627,  635,  643,  651,  659,
 /*    40 */   667,  675,  683,  691,  699,  707,  715,  723,  731,  739,
 /*    50 */   747,   70,   93,    5,   10,   28,   61,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   323,  290,  304,  304,  304,  304,  328,  328,  290,  328,
 /*    10 */   328,  328,  328,  328,  309,  309,  253,  328,  328,  328,
 /*    20 */   328,  328,  312,  328,  328,  328,  328,  328,  328,  328,
 /*    30 */   328,  328,  328,  328,  328,  328,  328,  328,  328,  328,
 /*    40 */   328,  328,  328,  328,  328,  328,  328,  328,  328,  328,
 /*    50 */   328,  295,  295,  328,  328,  328,  298,  328,  328,  328,
 /*    60 */   328,  328,  328,  328,  287,  288,  328,  328,  311,  310,
 /*    70 */   328,  251,  254,  255,  294,  308,  268,  267,  271,  280,
 /*    80 */   279,  278,  277,  276,  275,  270,  257,  247,  247,  257,
 /*    90 */   247,  260,  259,  258,  257,  328,  328,  328,  328,  247,
 /*   100 */   328,  328,  328,  328,  328,  328,  328,  328,  328,  328,
 /*   110 */   328,  328,  315,  262,  301,  262,  328,  265,  273,  272,
 /*   120 */   264,  274,  263,  262,  247,  246,
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
  "NOT",           "IDENTIFIER",    "VALUE",         "LEFT_SQUARE", 
  "RIGHT_SQUARE",  "LEFT_ROUND",    "RIGHT_ROUND",   "COLON",       
  "COMMA",         "ASSIGN",        "ARROW",         "LEFT_CURLY",  
  "RIGHT_CURLY",   "DOLLAR",        "MODEL_NAME",    "INCLUDE",     
  "error",         "variable",      "expression",    "arguments",   
  "comparison",    "sum",           "product",       "conjunction", 
  "disjunction",   "assignment",    "reaction",      "reactionLeftSide",
  "reactionRightSide",  "reactionSpecifiers",  "reactionSpecifier",  "reactionLeftComponent",
  "moleculePropertyNames",  "reactionRightComponent",  "moleculePropertyExpressions",  "preprocessorDirective",
  "model",         "statements",    "statement",   
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "variable ::= IDENTIFIER LEFT_SQUARE expression RIGHT_SQUARE",
 /*   1 */ "variable ::= IDENTIFIER",
 /*   2 */ "expression ::= variable",
 /*   3 */ "expression ::= variable LEFT_ROUND arguments RIGHT_ROUND",
 /*   4 */ "expression ::= VALUE",
 /*   5 */ "expression ::= LEFT_ROUND expression RIGHT_ROUND",
 /*   6 */ "comparison ::= expression QUESTIONMARK expression COLON expression",
 /*   7 */ "expression ::= comparison",
 /*   8 */ "arguments ::=",
 /*   9 */ "arguments ::= expression",
 /*  10 */ "arguments ::= arguments COMMA expression",
 /*  11 */ "expression ::= sum",
 /*  12 */ "sum ::= expression PLUS expression",
 /*  13 */ "sum ::= expression MINUS expression",
 /*  14 */ "sum ::= sum PLUS expression",
 /*  15 */ "sum ::= sum MINUS expression",
 /*  16 */ "expression ::= product",
 /*  17 */ "product ::= expression MULTIPLY expression",
 /*  18 */ "product ::= expression DIVIDE expression",
 /*  19 */ "product ::= product MULTIPLY expression",
 /*  20 */ "product ::= product DIVIDE expression",
 /*  21 */ "expression ::= conjunction",
 /*  22 */ "conjunction ::= expression AND expression",
 /*  23 */ "conjunction ::= conjunction AND expression",
 /*  24 */ "expression ::= disjunction",
 /*  25 */ "disjunction ::= expression OR expression",
 /*  26 */ "disjunction ::= disjunction OR expression",
 /*  27 */ "expression ::= NOT expression",
 /*  28 */ "expression ::= MINUS expression",
 /*  29 */ "expression ::= expression EXP expression",
 /*  30 */ "expression ::= expression EQUAL expression",
 /*  31 */ "expression ::= expression NOT_EQUAL expression",
 /*  32 */ "expression ::= expression GREATER expression",
 /*  33 */ "expression ::= expression GREATER_EQUAL expression",
 /*  34 */ "expression ::= expression LESS expression",
 /*  35 */ "expression ::= expression LESS_EQUAL expression",
 /*  36 */ "assignment ::= variable ASSIGN expression SEMICOLON",
 /*  37 */ "assignment ::= variable ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON",
 /*  38 */ "reaction ::= reactionLeftSide ARROW reactionRightSide COMMA reactionSpecifiers SEMICOLON",
 /*  39 */ "reaction ::= variable COMMA reactionLeftSide ARROW reactionRightSide COMMA reactionSpecifiers SEMICOLON",
 /*  40 */ "reactionSpecifiers ::= reactionSpecifier",
 /*  41 */ "reactionSpecifiers ::= reactionSpecifiers COMMA reactionSpecifier",
 /*  42 */ "reactionSpecifier ::= expression",
 /*  43 */ "reactionSpecifier ::= variable COLON expression",
 /*  44 */ "reactionSpecifier ::= LEFT_SQUARE expression RIGHT_SQUARE",
 /*  45 */ "reactionLeftSide ::=",
 /*  46 */ "reactionLeftSide ::= reactionLeftComponent",
 /*  47 */ "reactionLeftSide ::= reactionLeftSide PLUS reactionLeftComponent",
 /*  48 */ "reactionLeftSide ::= expression PLUS expression",
 /*  49 */ "reactionLeftSide ::= reactionLeftSide PLUS expression",
 /*  50 */ "moleculePropertyNames ::=",
 /*  51 */ "moleculePropertyNames ::= variable",
 /*  52 */ "moleculePropertyNames ::= moleculePropertyNames COMMA variable",
 /*  53 */ "moleculePropertyNames ::= moleculePropertyNames COMMA",
 /*  54 */ "reactionLeftComponent ::= variable",
 /*  55 */ "reactionLeftComponent ::= variable LEFT_CURLY moleculePropertyNames RIGHT_CURLY",
 /*  56 */ "reactionLeftComponent ::= DOLLAR variable",
 /*  57 */ "reactionLeftComponent ::= DOLLAR variable LEFT_CURLY moleculePropertyNames RIGHT_CURLY",
 /*  58 */ "reactionLeftComponent ::= expression MULTIPLY reactionLeftComponent",
 /*  59 */ "reactionRightSide ::=",
 /*  60 */ "reactionRightSide ::= reactionRightComponent",
 /*  61 */ "reactionRightSide ::= reactionRightSide PLUS reactionRightComponent",
 /*  62 */ "reactionRightSide ::= expression PLUS expression",
 /*  63 */ "reactionRightSide ::= reactionRightSide PLUS expression",
 /*  64 */ "moleculePropertyExpressions ::=",
 /*  65 */ "moleculePropertyExpressions ::= expression",
 /*  66 */ "moleculePropertyExpressions ::= moleculePropertyExpressions COMMA expression",
 /*  67 */ "moleculePropertyExpressions ::= moleculePropertyExpressions COMMA",
 /*  68 */ "reactionRightComponent ::= variable",
 /*  69 */ "reactionRightComponent ::= variable LEFT_CURLY moleculePropertyExpressions RIGHT_CURLY",
 /*  70 */ "reactionRightComponent ::= DOLLAR variable",
 /*  71 */ "reactionRightComponent ::= DOLLAR variable LEFT_CURLY moleculePropertyExpressions RIGHT_CURLY",
 /*  72 */ "reactionRightComponent ::= expression MULTIPLY reactionRightComponent",
 /*  73 */ "reactionRightComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionRightSide COLON reactionRightSide RIGHT_SQUARE",
 /*  74 */ "preprocessorDirective ::= INCLUDE variable SEMICOLON",
 /*  75 */ "preprocessorDirective ::= MODEL_NAME IDENTIFIER SEMICOLON",
 /*  76 */ "model ::= statements",
 /*  77 */ "statements ::= statements statement",
 /*  78 */ "statements ::=",
 /*  79 */ "statement ::= assignment",
 /*  80 */ "statement ::= reaction",
 /*  81 */ "statement ::= preprocessorDirective",
 /*  82 */ "statement ::= error",
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
    case 19: /* LEFT_SQUARE */
    case 20: /* RIGHT_SQUARE */
    case 21: /* LEFT_ROUND */
    case 22: /* RIGHT_ROUND */
    case 23: /* COLON */
    case 24: /* COMMA */
    case 25: /* ASSIGN */
    case 26: /* ARROW */
    case 27: /* LEFT_CURLY */
    case 28: /* RIGHT_CURLY */
    case 29: /* DOLLAR */
    case 30: /* MODEL_NAME */
    case 31: /* INCLUDE */
{
#line 8 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"

	delete (yypminor->yy0);
	(yypminor->yy0) = nullptr;

#line 827 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 33: /* variable */
{
#line 81 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy100);
	(yypminor->yy100) = nullptr;

#line 837 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 34: /* expression */
{
#line 108 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy64);
	(yypminor->yy64) = nullptr;

#line 847 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 35: /* arguments */
{
#line 158 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy103);
	(yypminor->yy103) = nullptr;

#line 857 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 36: /* comparison */
{
#line 142 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy73);
	(yypminor->yy73) = nullptr;

#line 867 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 37: /* sum */
{
#line 179 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy78);
	(yypminor->yy78) = nullptr;

#line 877 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 38: /* product */
{
#line 208 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy50);
	(yypminor->yy50) = nullptr;

#line 887 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 39: /* conjunction */
{
#line 238 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy45);
	(yypminor->yy45) = nullptr;

#line 897 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 40: /* disjunction */
{
#line 258 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy87);
	(yypminor->yy87) = nullptr;

#line 907 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 43: /* reactionLeftSide */
{
#line 434 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy96);
	(yypminor->yy96) = nullptr;

#line 917 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 44: /* reactionRightSide */
{
#line 570 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy5);
	(yypminor->yy5) = nullptr;

#line 927 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 45: /* reactionSpecifiers */
{
#line 376 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy29);
	(yypminor->yy29) = nullptr;

#line 937 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 46: /* reactionSpecifier */
{
#line 399 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy97);
	(yypminor->yy97) = nullptr;

#line 947 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 47: /* reactionLeftComponent */
{
#line 508 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy14);
	(yypminor->yy14) = nullptr;

#line 957 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 48: /* moleculePropertyNames */
{
#line 476 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy110);
	(yypminor->yy110) = nullptr;

#line 967 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 49: /* reactionRightComponent */
{
#line 643 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy77);
	(yypminor->yy77) = nullptr;

#line 977 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
      break;
    case 50: /* moleculePropertyExpressions */
{
#line 612 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
 
	delete (yypminor->yy30);
	(yypminor->yy30) = nullptr;

#line 987 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
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
#line 5 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
throw std::runtime_error("Parser stack overflow while parsing cmdl file.");
#line 1168 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
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
  { 33, -4 },
  { 33, -1 },
  { 34, -1 },
  { 34, -4 },
  { 34, -1 },
  { 34, -3 },
  { 36, -5 },
  { 34, -1 },
  { 35, 0 },
  { 35, -1 },
  { 35, -3 },
  { 34, -1 },
  { 37, -3 },
  { 37, -3 },
  { 37, -3 },
  { 37, -3 },
  { 34, -1 },
  { 38, -3 },
  { 38, -3 },
  { 38, -3 },
  { 38, -3 },
  { 34, -1 },
  { 39, -3 },
  { 39, -3 },
  { 34, -1 },
  { 40, -3 },
  { 40, -3 },
  { 34, -2 },
  { 34, -2 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 34, -3 },
  { 41, -4 },
  { 41, -6 },
  { 42, -6 },
  { 42, -8 },
  { 45, -1 },
  { 45, -3 },
  { 46, -1 },
  { 46, -3 },
  { 46, -3 },
  { 43, 0 },
  { 43, -1 },
  { 43, -3 },
  { 43, -3 },
  { 43, -3 },
  { 48, 0 },
  { 48, -1 },
  { 48, -3 },
  { 48, -2 },
  { 47, -1 },
  { 47, -4 },
  { 47, -2 },
  { 47, -5 },
  { 47, -3 },
  { 44, 0 },
  { 44, -1 },
  { 44, -3 },
  { 44, -3 },
  { 44, -3 },
  { 50, 0 },
  { 50, -1 },
  { 50, -3 },
  { 50, -2 },
  { 49, -1 },
  { 49, -4 },
  { 49, -2 },
  { 49, -5 },
  { 49, -3 },
  { 49, -7 },
  { 51, -3 },
  { 51, -3 },
  { 52, -1 },
  { 53, -2 },
  { 53, 0 },
  { 54, -1 },
  { 54, -1 },
  { 54, -1 },
  { 54, -1 },
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
      case 0: /* variable ::= IDENTIFIER LEFT_SQUARE expression RIGHT_SQUARE */
#line 85 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier name = *yymsp[-3].minor.yy0;
	delete yymsp[-3].minor.yy0;
	yymsp[-3].minor.yy0 = nullptr;
	yylhsminor.yy100 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy64);
	yymsp[-1].minor.yy64 = nullptr;

	auto value = static_cast<size_t>(parseTree->GetExpressionValue(e_temp.get())+0.5);
	yylhsminor.yy100 = new identifier(name+"["+std::to_string(value)+"]");
}
#line 1401 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,20,&yymsp[0].minor);
  yymsp[-3].minor.yy100 = yylhsminor.yy100;
        break;
      case 1: /* variable ::= IDENTIFIER */
#line 96 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy100 = new identifier(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1413 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy100 = yylhsminor.yy100;
        break;
      case 2: /* expression ::= variable */
#line 112 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new VariableExpression(*yymsp[0].minor.yy100);
	delete yymsp[0].minor.yy100;
	yymsp[0].minor.yy100 = nullptr;
}
#line 1423 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 3: /* expression ::= variable LEFT_ROUND arguments RIGHT_ROUND */
#line 117 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto func = new FunctionExpression(*yymsp[-3].minor.yy100);
	delete yymsp[-3].minor.yy100;
	yymsp[-3].minor.yy100 = nullptr;
	yylhsminor.yy64 = nullptr;
	for(auto& argument : *yymsp[-1].minor.yy103)
	{
		func->PushBack(std::move(argument));
	}
	delete yymsp[-1].minor.yy103;
	yymsp[-1].minor.yy103 = nullptr;
	yylhsminor.yy64 = func;
	func = nullptr;
}
#line 1442 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,21,&yymsp[-2].minor);
  yy_destructor(yypParser,22,&yymsp[0].minor);
  yymsp[-3].minor.yy64 = yylhsminor.yy64;
        break;
      case 4: /* expression ::= VALUE */
#line 132 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new NumberExpression(*yymsp[0].minor.yy0);
	delete yymsp[0].minor.yy0;
	yymsp[0].minor.yy0 = nullptr;
}
#line 1454 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 5: /* expression ::= LEFT_ROUND expression RIGHT_ROUND */
{  yy_destructor(yypParser,21,&yymsp[-2].minor);
#line 137 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[-2].minor.yy64 = yymsp[-1].minor.yy64;
}
#line 1463 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,22,&yymsp[0].minor);
}
        break;
      case 6: /* comparison ::= expression QUESTIONMARK expression COLON expression */
#line 146 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy73 = new ConditionalExpression(std::unique_ptr<IExpression>(yymsp[-4].minor.yy64), std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
	yymsp[-2].minor.yy64 = nullptr;
	yymsp[0].minor.yy64 = nullptr;
	yymsp[-4].minor.yy64 = nullptr;
}
#line 1475 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,23,&yymsp[-1].minor);
  yymsp[-4].minor.yy73 = yylhsminor.yy73;
        break;
      case 7: /* expression ::= comparison */
#line 152 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy73;
}
#line 1485 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 8: /* arguments ::= */
#line 162 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[1].minor.yy103 = new FunctionArguments();
}
#line 1493 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
        break;
      case 9: /* arguments ::= expression */
#line 165 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy103 = new FunctionArguments();
	yylhsminor.yy103->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy64));
	yymsp[0].minor.yy64 = nullptr;
}
#line 1502 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy103 = yylhsminor.yy103;
        break;
      case 10: /* arguments ::= arguments COMMA expression */
#line 170 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy103 = yymsp[-2].minor.yy103;
	yymsp[-2].minor.yy103 = nullptr;
	yylhsminor.yy103->push_back(typename FunctionArguments::value_type(yymsp[0].minor.yy64));
	yymsp[0].minor.yy64 = nullptr;
}
#line 1513 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yymsp[-2].minor.yy103 = yylhsminor.yy103;
        break;
      case 11: /* expression ::= sum */
#line 183 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy78;
}
#line 1522 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 12: /* sum ::= expression PLUS expression */
#line 186 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy78 = new SumExpression();
	yylhsminor.yy78->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy78->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1532 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy78 = yylhsminor.yy78;
        break;
      case 13: /* sum ::= expression MINUS expression */
#line 191 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy78 = new SumExpression();
	yylhsminor.yy78->PushBack(false,  std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy78->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1543 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy78 = yylhsminor.yy78;
        break;
      case 14: /* sum ::= sum PLUS expression */
#line 196 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy78 = yymsp[-2].minor.yy78;
	yylhsminor.yy78->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1553 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy78 = yylhsminor.yy78;
        break;
      case 15: /* sum ::= sum MINUS expression */
#line 200 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy78 = yymsp[-2].minor.yy78;
	yylhsminor.yy78->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1563 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy78 = yylhsminor.yy78;
        break;
      case 16: /* expression ::= product */
#line 212 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy50;
}
#line 1572 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 17: /* product ::= expression MULTIPLY expression */
#line 215 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy50 = new ProductExpression();
	yylhsminor.yy50->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy50->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1583 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy50 = yylhsminor.yy50;
        break;
      case 18: /* product ::= expression DIVIDE expression */
#line 221 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy50 = new ProductExpression();
	yylhsminor.yy50->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy50->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1595 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy50 = yylhsminor.yy50;
        break;
      case 19: /* product ::= product MULTIPLY expression */
#line 227 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy50 = yymsp[-2].minor.yy50;
	yylhsminor.yy50->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1605 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy50 = yylhsminor.yy50;
        break;
      case 20: /* product ::= product DIVIDE expression */
#line 231 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy50 = yymsp[-2].minor.yy50;
	yylhsminor.yy50->PushBack(true, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1615 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy50 = yylhsminor.yy50;
        break;
      case 21: /* expression ::= conjunction */
#line 242 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy45;
}
#line 1624 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 22: /* conjunction ::= expression AND expression */
#line 245 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy45 = new ConjunctionExpression();
	yylhsminor.yy45->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy45->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1635 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy45 = yylhsminor.yy45;
        break;
      case 23: /* conjunction ::= conjunction AND expression */
#line 251 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy45 = yymsp[-2].minor.yy45;
	yylhsminor.yy45->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1645 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy45 = yylhsminor.yy45;
        break;
      case 24: /* expression ::= disjunction */
#line 262 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = yymsp[0].minor.yy87;
}
#line 1654 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy64 = yylhsminor.yy64;
        break;
      case 25: /* disjunction ::= expression OR expression */
#line 265 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy87 = new DisjunctionExpression();
	yylhsminor.yy87->PushBack(false, std::unique_ptr<IExpression>(yymsp[-2].minor.yy64));
	yylhsminor.yy87->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));

}
#line 1665 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy87 = yylhsminor.yy87;
        break;
      case 26: /* disjunction ::= disjunction OR expression */
#line 271 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy87 = yymsp[-2].minor.yy87;
	yylhsminor.yy87->PushBack(false, std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1675 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy87 = yylhsminor.yy87;
        break;
      case 27: /* expression ::= NOT expression */
{  yy_destructor(yypParser,16,&yymsp[-1].minor);
#line 277 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[-1].minor.yy64 = new UnaryNotExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1685 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
        break;
      case 28: /* expression ::= MINUS expression */
{  yy_destructor(yypParser,12,&yymsp[-1].minor);
#line 282 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[-1].minor.yy64 = new UnaryMinusExpression(std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1694 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
        break;
      case 29: /* expression ::= expression EXP expression */
#line 287 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new ExponentiationExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64));
}
#line 1702 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,15,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 30: /* expression ::= expression EQUAL expression */
#line 293 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_equal);
}
#line 1711 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 31: /* expression ::= expression NOT_EQUAL expression */
#line 296 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_not_equal);
}
#line 1720 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 32: /* expression ::= expression GREATER expression */
#line 299 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_greater);
}
#line 1729 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 33: /* expression ::= expression GREATER_EQUAL expression */
#line 302 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_greater_equal);
}
#line 1738 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 34: /* expression ::= expression LESS expression */
#line 305 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_less);
}
#line 1747 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 35: /* expression ::= expression LESS_EQUAL expression */
#line 308 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy64 = new ComparisonExpression(std::unique_ptr<IExpression>(yymsp[-2].minor.yy64), std::unique_ptr<IExpression>(yymsp[0].minor.yy64), ComparisonExpression::type_less_equal);
}
#line 1756 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy64 = yylhsminor.yy64;
        break;
      case 36: /* assignment ::= variable ASSIGN expression SEMICOLON */
#line 320 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-3].minor.yy100 and yymsp[-1].minor.yy64 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-3].minor.yy100;
	delete yymsp[-3].minor.yy100;
	yymsp[-3].minor.yy100 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy64);
	yymsp[-1].minor.yy64 = nullptr;

	parseTree->CreateVariable(std::move(name), parseTree->GetExpressionValue(e_temp.get()));
}
#line 1773 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,25,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 37: /* assignment ::= variable ASSIGN LEFT_SQUARE expression RIGHT_SQUARE SEMICOLON */
#line 332 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	// create_variable might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy100 and yymsp[-2].minor.yy64 by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *yymsp[-5].minor.yy100;
	delete yymsp[-5].minor.yy100;
	yymsp[-5].minor.yy100 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64 = nullptr;

	parseTree->CreateVariable(std::move(name), std::move(e_temp));
}
#line 1790 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,25,&yymsp[-4].minor);
  yy_destructor(yypParser,19,&yymsp[-3].minor);
  yy_destructor(yypParser,20,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 38: /* reaction ::= reactionLeftSide ARROW reactionRightSide COMMA reactionSpecifiers SEMICOLON */
#line 346 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy96, yymsp[-3].minor.yy5 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionLeftSide>(yymsp[-5].minor.yy96);
	auto products_temp = std::unique_ptr<ReactionRightSide>(yymsp[-3].minor.yy5);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy29);
	yymsp[-1].minor.yy29 = nullptr;
	yymsp[-5].minor.yy96 = nullptr;
	yymsp[-3].minor.yy5 = nullptr;

	parseTree->CreateReaction(std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1810 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 39: /* reaction ::= variable COMMA reactionLeftSide ARROW reactionRightSide COMMA reactionSpecifiers SEMICOLON */
#line 359 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	// create_reaction might throw an exception, which results in automatic destruction of yymsp[-5].minor.yy96, yymsp[-3].minor.yy5 and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionLeftSide>(yymsp[-5].minor.yy96);
	auto products_temp = std::unique_ptr<ReactionRightSide>(yymsp[-3].minor.yy5);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-1].minor.yy29);
	identifier name = *yymsp[-7].minor.yy100; 
	yymsp[-1].minor.yy29 = nullptr;
	yymsp[-5].minor.yy96 = nullptr;
	yymsp[-3].minor.yy5 = nullptr;
	delete yymsp[-7].minor.yy100;
	yymsp[-7].minor.yy100 = nullptr;

	parseTree->CreateReaction(std::move(name), std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}
#line 1832 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-6].minor);
  yy_destructor(yypParser,26,&yymsp[-4].minor);
  yy_destructor(yypParser,24,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
        break;
      case 40: /* reactionSpecifiers ::= reactionSpecifier */
#line 380 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto rss_temp = std::make_unique<ReactionSpecifiers>();
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy97);
	yymsp[0].minor.yy97 = nullptr;
	yylhsminor.yy29 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy29 = rss_temp.release();
}
#line 1848 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy29 = yylhsminor.yy29;
        break;
      case 41: /* reactionSpecifiers ::= reactionSpecifiers COMMA reactionSpecifier */
#line 388 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(yymsp[-2].minor.yy29);
	yymsp[-2].minor.yy29 = nullptr;
	yylhsminor.yy29 = nullptr;
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(yymsp[0].minor.yy97);
	yymsp[0].minor.yy97 = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	yylhsminor.yy29 = rss_temp.release();
}
#line 1862 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yymsp[-2].minor.yy29 = yylhsminor.yy29;
        break;
      case 42: /* reactionSpecifier ::= expression */
#line 403 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64 = nullptr;
	yylhsminor.yy97 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy97 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::make_unique<NumberExpression>(value));
}
#line 1875 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy97 = yylhsminor.yy97;
        break;
      case 43: /* reactionSpecifier ::= variable COLON expression */
#line 411 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64 = nullptr;
	yylhsminor.yy97 = nullptr;
	identifier name = *yymsp[-2].minor.yy100;
	delete yymsp[-2].minor.yy100;
	yymsp[-2].minor.yy100 = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	yylhsminor.yy97 = new ReactionSpecifier(name, std::make_unique<NumberExpression>(value));
}
#line 1890 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,23,&yymsp[-1].minor);
  yymsp[-2].minor.yy97 = yylhsminor.yy97;
        break;
      case 44: /* reactionSpecifier ::= LEFT_SQUARE expression RIGHT_SQUARE */
{  yy_destructor(yypParser,19,&yymsp[-2].minor);
#line 422 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-1].minor.yy64);
	yymsp[-1].minor.yy64 = nullptr;
	yymsp[-2].minor.yy97 = nullptr;
	yymsp[-2].minor.yy97 = new ReactionSpecifier(ReactionSpecifier::rate_type, std::move(e_temp));
}
#line 1903 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
        break;
      case 45: /* reactionLeftSide ::= */
#line 438 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[1].minor.yy96 = new ReactionLeftSide();
}
#line 1912 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
        break;
      case 46: /* reactionLeftSide ::= reactionLeftComponent */
#line 441 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionLeftComponent>(yymsp[0].minor.yy14);
	yymsp[0].minor.yy14 = nullptr;
	yylhsminor.yy96 = nullptr;

	auto rs_temp = std::make_unique<ReactionLeftSide>();
	rs_temp->PushBack(std::move(rc_temp));
	yylhsminor.yy96 = rs_temp.release();
}
#line 1925 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy96 = yylhsminor.yy96;
        break;
      case 47: /* reactionLeftSide ::= reactionLeftSide PLUS reactionLeftComponent */
#line 450 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy96 = yymsp[-2].minor.yy96;
	yymsp[-2].minor.yy96 = nullptr;
	auto rc_temp = std::unique_ptr<ReactionLeftComponent>(yymsp[0].minor.yy14);
	yymsp[0].minor.yy14 = nullptr;

	yylhsminor.yy96->PushBack(std::move(rc_temp));
}
#line 1938 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy96 = yylhsminor.yy96;
        break;
      case 48: /* reactionLeftSide ::= expression PLUS expression */
#line 459 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	delete(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64=nullptr;
	delete(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64=nullptr;
	throw std::runtime_error("Reactants or modifiers of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1951 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 49: /* reactionLeftSide ::= reactionLeftSide PLUS expression */
#line 467 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	delete(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64=nullptr;
	delete(yymsp[-2].minor.yy96);
	yymsp[-2].minor.yy96=nullptr;
	throw std::runtime_error("Reactants or modifiers of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 1963 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 50: /* moleculePropertyNames ::= */
#line 480 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[1].minor.yy110 = new MoleculePropertyNames();
	yymsp[1].minor.yy110->push_back("");
}
#line 1972 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
        break;
      case 51: /* moleculePropertyNames ::= variable */
#line 484 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier name = *yymsp[0].minor.yy100;
	delete yymsp[0].minor.yy100;
	yymsp[0].minor.yy100 = nullptr;

	yylhsminor.yy110 = new MoleculePropertyNames();
	yylhsminor.yy110->push_back(name);
}
#line 1984 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy110 = yylhsminor.yy110;
        break;
      case 52: /* moleculePropertyNames ::= moleculePropertyNames COMMA variable */
#line 492 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy110 = yymsp[-2].minor.yy110;
	yymsp[-2].minor.yy110 = nullptr;
	identifier name = *yymsp[0].minor.yy100;
	delete yymsp[0].minor.yy100;
	yymsp[0].minor.yy100 = nullptr;

	yylhsminor.yy110->push_back(name);
}
#line 1998 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yymsp[-2].minor.yy110 = yylhsminor.yy110;
        break;
      case 53: /* moleculePropertyNames ::= moleculePropertyNames COMMA */
#line 501 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy110 = yymsp[-1].minor.yy110;
	yymsp[-1].minor.yy110 = nullptr;
	yylhsminor.yy110->push_back("");
}
#line 2009 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[0].minor);
  yymsp[-1].minor.yy110 = yylhsminor.yy110;
        break;
      case 54: /* reactionLeftComponent ::= variable */
#line 512 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy100;
	delete yymsp[0].minor.yy100;
	yymsp[0].minor.yy100 = nullptr;
	yylhsminor.yy14 = nullptr;

	yylhsminor.yy14 = new ReactionLeftComponent(state, 1, false);
}
#line 2023 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy14 = yylhsminor.yy14;
        break;
      case 55: /* reactionLeftComponent ::= variable LEFT_CURLY moleculePropertyNames RIGHT_CURLY */
#line 520 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[-3].minor.yy100;
	delete yymsp[-3].minor.yy100;
	yymsp[-3].minor.yy100 = nullptr;
	yylhsminor.yy14 = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyNames>(yymsp[-1].minor.yy110);
	yymsp[-1].minor.yy110 = nullptr;

	yylhsminor.yy14 = new ReactionLeftComponent(state, 1, false, std::move(as_temp));
}
#line 2038 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,27,&yymsp[-2].minor);
  yy_destructor(yypParser,28,&yymsp[0].minor);
  yymsp[-3].minor.yy14 = yylhsminor.yy14;
        break;
      case 56: /* reactionLeftComponent ::= DOLLAR variable */
{  yy_destructor(yypParser,29,&yymsp[-1].minor);
#line 531 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy100;
	delete yymsp[0].minor.yy100;
	yymsp[0].minor.yy100 = nullptr;
	yymsp[-1].minor.yy14 = nullptr;

	yymsp[-1].minor.yy14 = new ReactionLeftComponent(state, 1, true);
}
#line 2054 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
        break;
      case 57: /* reactionLeftComponent ::= DOLLAR variable LEFT_CURLY moleculePropertyNames RIGHT_CURLY */
{  yy_destructor(yypParser,29,&yymsp[-4].minor);
#line 540 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[-3].minor.yy100;
	delete yymsp[-3].minor.yy100;
	yymsp[-3].minor.yy100 = nullptr;
	yymsp[-4].minor.yy14 = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyNames>(yymsp[-1].minor.yy110);
	yymsp[-1].minor.yy110 = nullptr;

	yymsp[-4].minor.yy14 = new ReactionLeftComponent(state, 1, true, std::move(as_temp));
}
#line 2070 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,27,&yymsp[-2].minor);
  yy_destructor(yypParser,28,&yymsp[0].minor);
}
        break;
      case 58: /* reactionLeftComponent ::= expression MULTIPLY reactionLeftComponent */
#line 551 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionLeftComponent>(yymsp[0].minor.yy14);
	yymsp[0].minor.yy14 = nullptr;
	yylhsminor.yy14 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64 = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	if(stochiometry<=0)
		throw std::runtime_error("Stochiometry must be positive.");
	rc_temp->SetStochiometry(static_cast<stochsim::Stochiometry>(rc_temp->GetStochiometry()*stochiometry));
	yylhsminor.yy14 = rc_temp.release();
}
#line 2090 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy14 = yylhsminor.yy14;
        break;
      case 59: /* reactionRightSide ::= */
#line 574 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[1].minor.yy5 = new ReactionRightSide();
}
#line 2099 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
        break;
      case 60: /* reactionRightSide ::= reactionRightComponent */
#line 577 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionRightComponent>(yymsp[0].minor.yy77);
	yymsp[0].minor.yy77 = nullptr;
	yylhsminor.yy5 = nullptr;

	auto rs_temp = std::make_unique<ReactionRightSide>();
	rs_temp->PushBack(std::move(rc_temp));
	yylhsminor.yy5 = rs_temp.release();
}
#line 2112 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy5 = yylhsminor.yy5;
        break;
      case 61: /* reactionRightSide ::= reactionRightSide PLUS reactionRightComponent */
#line 586 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy5 = yymsp[-2].minor.yy5;
	yymsp[-2].minor.yy5 = nullptr;
	auto rc_temp = std::unique_ptr<ReactionRightComponent>(yymsp[0].minor.yy77);
	yymsp[0].minor.yy77 = nullptr;

	yylhsminor.yy5->PushBack(std::move(rc_temp));
}
#line 2125 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy5 = yylhsminor.yy5;
        break;
      case 62: /* reactionRightSide ::= expression PLUS expression */
#line 595 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	delete(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64=nullptr;
	delete(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64=nullptr;
	throw std::runtime_error("Products or transformees of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 2138 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 63: /* reactionRightSide ::= reactionRightSide PLUS expression */
#line 603 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	delete(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64=nullptr;
	delete(yymsp[-2].minor.yy5);
	yymsp[-2].minor.yy5=nullptr;
	throw std::runtime_error("Products or transformees of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
#line 2150 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,11,&yymsp[-1].minor);
        break;
      case 64: /* moleculePropertyExpressions ::= */
#line 616 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yymsp[1].minor.yy30 = new MoleculePropertyExpressions();
	yymsp[1].minor.yy30->push_back(std::unique_ptr<IExpression>(nullptr));
}
#line 2159 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
        break;
      case 65: /* moleculePropertyExpressions ::= expression */
#line 620 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64 = nullptr;

	yylhsminor.yy30 = new MoleculePropertyExpressions();
	yylhsminor.yy30->push_back(std::move(e_temp));
}
#line 2170 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy30 = yylhsminor.yy30;
        break;
      case 66: /* moleculePropertyExpressions ::= moleculePropertyExpressions COMMA expression */
#line 627 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy30 = yymsp[-2].minor.yy30;
	yymsp[-2].minor.yy30 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[0].minor.yy64);
	yymsp[0].minor.yy64 = nullptr;

	yylhsminor.yy30->push_back(std::move(e_temp));
}
#line 2183 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yymsp[-2].minor.yy30 = yylhsminor.yy30;
        break;
      case 67: /* moleculePropertyExpressions ::= moleculePropertyExpressions COMMA */
#line 635 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	yylhsminor.yy30 = yymsp[-1].minor.yy30;
	yymsp[-1].minor.yy30 = nullptr;

	yylhsminor.yy30->push_back(std::unique_ptr<IExpression>(nullptr));
}
#line 2195 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,24,&yymsp[0].minor);
  yymsp[-1].minor.yy30 = yylhsminor.yy30;
        break;
      case 68: /* reactionRightComponent ::= variable */
#line 647 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy100;
	delete yymsp[0].minor.yy100;
	yymsp[0].minor.yy100 = nullptr;
	yylhsminor.yy77 = nullptr;

	yylhsminor.yy77 = new ReactionRightComponent(state, 1, false);
}
#line 2209 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yymsp[0].minor.yy77 = yylhsminor.yy77;
        break;
      case 69: /* reactionRightComponent ::= variable LEFT_CURLY moleculePropertyExpressions RIGHT_CURLY */
#line 655 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[-3].minor.yy100;
	delete yymsp[-3].minor.yy100;
	yymsp[-3].minor.yy100 = nullptr;
	yylhsminor.yy77 = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyExpressions>(yymsp[-1].minor.yy30);
	yymsp[-1].minor.yy30 = nullptr;

	yylhsminor.yy77 = new ReactionRightComponent(state, 1, false, std::move(as_temp));
}
#line 2224 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,27,&yymsp[-2].minor);
  yy_destructor(yypParser,28,&yymsp[0].minor);
  yymsp[-3].minor.yy77 = yylhsminor.yy77;
        break;
      case 70: /* reactionRightComponent ::= DOLLAR variable */
{  yy_destructor(yypParser,29,&yymsp[-1].minor);
#line 666 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[0].minor.yy100;
	delete yymsp[0].minor.yy100;
	yymsp[0].minor.yy100 = nullptr;
	yymsp[-1].minor.yy77 = nullptr;

	yymsp[-1].minor.yy77 = new ReactionRightComponent(state, 1, true);
}
#line 2240 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
}
        break;
      case 71: /* reactionRightComponent ::= DOLLAR variable LEFT_CURLY moleculePropertyExpressions RIGHT_CURLY */
{  yy_destructor(yypParser,29,&yymsp[-4].minor);
#line 675 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier state = *yymsp[-3].minor.yy100;
	delete yymsp[-3].minor.yy100;
	yymsp[-3].minor.yy100 = nullptr;
	yymsp[-4].minor.yy77 = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyExpressions>(yymsp[-1].minor.yy30);
	yymsp[-1].minor.yy30 = nullptr;

	yymsp[-4].minor.yy77 = new ReactionRightComponent(state, 1, true, std::move(as_temp));
}
#line 2256 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,27,&yymsp[-2].minor);
  yy_destructor(yypParser,28,&yymsp[0].minor);
}
        break;
      case 72: /* reactionRightComponent ::= expression MULTIPLY reactionRightComponent */
#line 686 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto rc_temp = std::unique_ptr<ReactionRightComponent>(yymsp[0].minor.yy77);
	yymsp[0].minor.yy77 = nullptr;
	yylhsminor.yy77 = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-2].minor.yy64);
	yymsp[-2].minor.yy64 = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	if(stochiometry<=0)
		throw std::runtime_error("Stochiometry must be positive.");
	rc_temp->SetStochiometry(static_cast<stochsim::Stochiometry>(rc_temp->GetStochiometry()*stochiometry));
	yylhsminor.yy77 = rc_temp.release();
}
#line 2276 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy77 = yylhsminor.yy77;
        break;
      case 73: /* reactionRightComponent ::= LEFT_SQUARE expression QUESTIONMARK reactionRightSide COLON reactionRightSide RIGHT_SQUARE */
{  yy_destructor(yypParser,19,&yymsp[-6].minor);
#line 700 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	auto e_temp = std::unique_ptr<IExpression>(yymsp[-5].minor.yy64);
	yymsp[-5].minor.yy64 = nullptr;
	auto s1_temp = std::unique_ptr<ReactionRightSide>(yymsp[-3].minor.yy5);
	auto s2_temp = std::unique_ptr<ReactionRightSide>(yymsp[-1].minor.yy5);
	yymsp[-3].minor.yy5 = nullptr;
	yymsp[-1].minor.yy5 = nullptr;
	yymsp[-6].minor.yy77 = nullptr;

	identifier state = parseTree->CreateChoice(std::move(e_temp), std::move(s1_temp), std::move(s2_temp));
	yymsp[-6].minor.yy77 = new ReactionRightComponent(state, 1, false);
}
#line 2295 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,2,&yymsp[-4].minor);
  yy_destructor(yypParser,23,&yymsp[-2].minor);
  yy_destructor(yypParser,20,&yymsp[0].minor);
}
        break;
      case 74: /* preprocessorDirective ::= INCLUDE variable SEMICOLON */
{  yy_destructor(yypParser,31,&yymsp[-2].minor);
#line 720 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
	identifier fileName = *yymsp[-1].minor.yy100;
	delete yymsp[-1].minor.yy100;
	yymsp[-1].minor.yy100 = nullptr;
	parseTree->IncludeFile(fileName);
}
#line 2310 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
        break;
      case 75: /* preprocessorDirective ::= MODEL_NAME IDENTIFIER SEMICOLON */
{  yy_destructor(yypParser,30,&yymsp[-2].minor);
#line 717 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
{
}
#line 2319 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
  yy_destructor(yypParser,17,&yymsp[-1].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
        break;
      default:
      /* (76) model ::= statements */ yytestcase(yyruleno==76);
      /* (77) statements ::= statements statement */ yytestcase(yyruleno==77);
      /* (78) statements ::= */ yytestcase(yyruleno==78);
      /* (79) statement ::= assignment (OPTIMIZED OUT) */ assert(yyruleno!=79);
      /* (80) statement ::= reaction (OPTIMIZED OUT) */ assert(yyruleno!=80);
      /* (81) statement ::= preprocessorDirective (OPTIMIZED OUT) */ assert(yyruleno!=81);
      /* (82) statement ::= error (OPTIMIZED OUT) */ assert(yyruleno!=82);
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
#line 4 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.y"
throw std::runtime_error("Syntax error.");
#line 2378 "/home/langmo/stochsim/lib/cmdlparser/cmdl_grammar.c"
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