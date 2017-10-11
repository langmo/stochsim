// Configuration of output
%token_prefix TOKEN_
%start_symbol result
%parse_failure {throw std::exception("Syntax error while parsing expression.");}
%stack_overflow {throw std::exception("Parser stack overflow while parsing expression.");}
%name expression_Parse
%token_type {TerminalSymbol*}
%token_destructor {
	delete $$;
	$$ = nullptr;
}
%extra_argument {ExpressionParseTree* parseTree}

%include {#include "expression_common.h"}
%include {#include "ComparisonExpression.h"}
%include {#include "ConditionalExpression.h"}
%include {#include "ExponentiationExpression.h"}
%include {#include "ConjunctionExpression.h"}
%include {#include "DisjunctionExpression.h"}
%include {#include "UnaryNotExpression.h"}
%include {#include "NumberExpression.h"}
%include {#include "ProductExpression.h"}
%include {#include "SumExpression.h"}
%include {#include "UnaryMinusExpression.h"}
%include {#include "VariableExpression.h"}
%include {#include "FunctionExpression.h"}

%include {#include "expression_symbols.h"}
%include {#include "ExpressionParseTree.h"}
%include {#include "ExpressionParser.h"}

%include {#include  <assert.h>}
%include {using namespace expression;}

// Convert c-style parsing functions to implementation of clean c++ class handling everything.
%include {
	// Forward declaration parser functions.
	void expression_Parse(
		void *yyp,                   /* The parser */
		int yymajor,                 /* The major token code number */
		TerminalSymbol* yyminor,       /* The value for the token */
		ExpressionParseTree* parse_tree               /* Optional %extra_argument parameter */
	);

	void *expression_ParseAlloc(void* (*mallocProc)(size_t));

	void expression_ParseFree(
		void *p,                    /* The parser to be deleted */
		void(*freeProc)(void*)     /* Function used to reclaim memory */
	);
	#ifndef NDEBUG
		void expression_ParseTrace(FILE *TraceFILE, char *zTracePrompt);
	#endif
	void expression::ExpressionParser::InitializeInternal()
	{
		UninitializeInternal();
	#ifndef NDEBUG
		if (logFilePath_.empty())
		{
			logFile_ = nullptr;
		}
		else
		{
			fopen_s(&logFile_, logFilePath_.c_str(), "w");
			if(logFile_)
				expression_ParseTrace(logFile_, "cmdl_");
			else
				expression_ParseTrace(0, "cmdl_");
		}
	#endif
		try
		{
			handle_ = expression_ParseAlloc(malloc);
		}
		catch (...)
		{
			throw std::exception("Could not allocate space for expression parser.");
		}
		if (!handle_)
		{
			throw std::exception("Could not create expression parser.");
		}
	}
	void expression::ExpressionParser::UninitializeInternal()
	{
		if(!handle_)
			return;
		expression_ParseFree(handle_, free); 
		handle_ = nullptr;
	#ifndef NDEBUG
		expression_ParseTrace(0, "cmdl_");
		if (logFile_)
			fclose(logFile_);
		logFile_ = nullptr;
	#endif
	}

	void expression::ExpressionParser::ParseToken(int tokenID, TerminalSymbol* token, ExpressionParseTree& parseTree)
	{
		if (!handle_)
		{
			throw std::exception("Parser handle invalid.");
		}
		try
		{
			expression_Parse(handle_, tokenID, token, &parseTree);
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
}


/////////////////////////////////////////////////////////////////////////////
// Define precedence
/////////////////////////////////////////////////////////////////////////////
/* Copied from the lemon documentation:
* The precedence of non-terminals is transferred to rules as follows: The precedence of a grammar rule is equal to the precedence of the left-most terminal symbol in the rule for which a precedence is defined. This is normally what you want, but in those cases where you want to precedence of a grammar rule to be something different, you can specify an alternative precedence symbol by putting the symbol in square braces after the period at the end of the rule and before any C-code. For example:
* 
*    expr = MINUS expr.  [NOT]
* 
* This rule has a precedence equal to that of the NOT symbol, not the MINUS symbol as would have been the case by default.
* 
* With the knowledge of how precedence is assigned to terminal symbols and individual grammar rules, we can now explain precisely how parsing conflicts are resolved in Lemon. 
* Shift-reduce conflicts are resolved as follows:
* 
*     If either the token to be shifted or the rule to be reduced lacks precedence information, then resolve in favor of the shift, but report a parsing conflict.
*     If the precedence of the token to be shifted is greater than the precedence of the rule to reduce, then resolve in favor of the shift. No parsing conflict is reported.
*     If the precedence of the token it be shifted is less than the precedence of the rule to reduce, then resolve in favor of the reduce action. No parsing conflict is reported.
*     If the precedences are the same and the shift token is right-associative, then resolve in favor of the shift. No parsing conflict is reported.
*     If the precedences are the same the the shift token is left-associative, then resolve in favor of the reduce. No parsing conflict is reported.
*     Otherwise, resolve the conflict by doing the shift and report the parsing conflict. 
* 
* Reduce-reduce conflicts are resolved this way:
* 
*     If either reduce rule lacks precedence information, then resolve in favor of the rule that appears first in the grammar and report a parsing conflict.
*     If both rules have precedence and the precedence is different then resolve the dispute in favor of the rule with the highest precedence and do not report a conflict.
*     Otherwise, resolve the conflict by reducing by the rule that appears first in the grammar and report a parsing conflict. 
*/

// A statement is finished with a semicolon only after all rules having a precedence higher than a semicolon are applied. Rules having the precedence of a semicolon are only
// applied if they have to, which is convenient for conversion rules which should only be applied if nothing else works.
%right SEMICOLON. 
%right QUESTIONMARK.
%left AND.
%left OR.
%nonassoc EQUAL NOT_EQUAL GREATER GREATER_EQUAL LESS LESS_EQUAL.
%left PLUS MINUS.
%left MULTIPLY DIVIDE.
%right EXP NOT.
%nonassoc IDENTIFIER VALUE. // always prefer longer rules


/////////////////////////////////////////////////////////////////////////////
// Rules to parse mathematical expressions
/////////////////////////////////////////////////////////////////////////////

// Basic mathematical expressions
%type expression {IExpression*}
%destructor expression { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= IDENTIFIER(I). [SEMICOLON] {
	e = new VariableExpression(*I);
	delete I;
	I = nullptr;
}
expression(e) ::= IDENTIFIER(I) LEFT_ROUND arguments(as) RIGHT_ROUND . [SEMICOLON] {
	auto func = new FunctionExpression(*I);
	delete I;
	I = nullptr;
	e = nullptr;
	for(auto& argument : *as)
	{
		func->PushBack(std::move(argument));
	}
	delete as;
	as = nullptr;
	e = func;
	func = nullptr;
}

expression(e) ::= VALUE(V). [SEMICOLON]{
	e = new NumberExpression(*V);
	delete V;
	V = nullptr;
}
expression(e_new) ::= LEFT_ROUND expression(e_old) RIGHT_ROUND. {
	e_new = e_old;
} // forces sums, ..., to be converted to an expression.

%type comparison {ConditionalExpression*}
%destructor comparison { 
	delete $$;
	$$ = nullptr;
}
comparison(e) ::= expression(c) QUESTIONMARK expression(e1) COLON expression(e2). [QUESTIONMARK]{
	e = new ConditionalExpression(std::unique_ptr<IExpression>(c), std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2));
	e1 = nullptr;
	e2 = nullptr;
	c = nullptr;
}
expression(e) ::= comparison(s). [EXP] {
	e = s;
}

// Arguments
%type arguments {FunctionArguments*}
%destructor arguments { 
	delete $$;
	$$ = nullptr;
}
arguments(as) ::= . [SEMICOLON] {
	as = new FunctionArguments();
}
arguments(as) ::= expression(e). [COMMA]{
	as = new FunctionArguments();
	as->push_back(typename FunctionArguments::value_type(e));
	e = nullptr;
}
arguments(as_new) ::= arguments(as_old) COMMA expression(e). [COMMA]{
	as_new = as_old;
	as_old = nullptr;
	as_new->push_back(typename FunctionArguments::value_type(e));
	e = nullptr;
}

// Sum
%type sum {SumExpression*}
%destructor sum { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= sum(s). [PLUS] {
	e = s;
}
sum(s) ::= expression(e1) PLUS expression(e2). {
	s = new SumExpression();
	s->PushBack(false, std::unique_ptr<IExpression>(e1));
	s->PushBack(false, std::unique_ptr<IExpression>(e2));
}
sum(s) ::= expression(e1) MINUS expression(e2). {
	s = new SumExpression();
	s->PushBack(false,  std::unique_ptr<IExpression>(e1));
	s->PushBack(true, std::unique_ptr<IExpression>(e2));
}
sum(s_new) ::= sum(s_old) PLUS expression(e). {
	s_new = s_old;
	s_new->PushBack(false, std::unique_ptr<IExpression>(e));
}
sum(s_new) ::= sum(s_old) MINUS expression(e). {
	s_new = s_old;
	s_new->PushBack(true, std::unique_ptr<IExpression>(e));
}


// Product
%type product {ProductExpression*}
%destructor product { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= product(p). [MULTIPLY] {
	e = p;
}
product(p) ::= expression(e1) MULTIPLY expression(e2). {
	p = new ProductExpression();
	p->PushBack(false, std::unique_ptr<IExpression>(e1));
	p->PushBack(false, std::unique_ptr<IExpression>(e2));

}
product(p) ::= expression(e1) DIVIDE expression(e2). {
	p = new ProductExpression();
	p->PushBack(false, std::unique_ptr<IExpression>(e1));
	p->PushBack(true, std::unique_ptr<IExpression>(e2));

}
product(p_new) ::= product(p_old) MULTIPLY expression(e). {
	p_new = p_old;
	p_new->PushBack(false, std::unique_ptr<IExpression>(e));
}
product(p_new) ::= product(p_old) DIVIDE expression(e). {
	p_new = p_old;
	p_new->PushBack(true, std::unique_ptr<IExpression>(e));
}

// conjunction
%type conjunction {ConjunctionExpression*}
%destructor conjunction { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= conjunction(c). [AND] {
	e = c;
}
conjunction(c) ::= expression(e1) AND expression(e2). {
	c = new ConjunctionExpression();
	c->PushBack(false, std::unique_ptr<IExpression>(e1));
	c->PushBack(false, std::unique_ptr<IExpression>(e2));

}
conjunction(c_new) ::= conjunction(c_old) AND expression(e). {
	c_new = c_old;
	c_new->PushBack(false, std::unique_ptr<IExpression>(e));
}

// disjunction
%type disjunction {DisjunctionExpression*}
%destructor disjunction { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= disjunction(c). [OR] {
	e = c;
}
disjunction(c) ::= expression(e1) OR expression(e2). {
	c = new DisjunctionExpression();
	c->PushBack(false, std::unique_ptr<IExpression>(e1));
	c->PushBack(false, std::unique_ptr<IExpression>(e2));

}
disjunction(c_new) ::= disjunction(c_old) OR expression(e). {
	c_new = c_old;
	c_new->PushBack(false, std::unique_ptr<IExpression>(e));
}

// not
expression(e_new) ::= NOT expression(e_old). {
	e_new = new UnaryNotExpression(std::unique_ptr<IExpression>(e_old));
}

// unary minus
expression(e_new) ::= MINUS expression(e_old). [NOT] {
	e_new = new UnaryMinusExpression(std::unique_ptr<IExpression>(e_old));
}

// Exponentiation
expression(e_new) ::= expression(e1) EXP expression(e2). {
	e_new = new ExponentiationExpression(std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2));
}


// Comparison
expression(e_new) ::= expression(e1) EQUAL expression(e2). {
	e_new = new ComparisonExpression(std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2), ComparisonExpression::type_equal);
}
expression(e_new) ::= expression(e1) NOT_EQUAL expression(e2). {
	e_new = new ComparisonExpression(std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2), ComparisonExpression::type_not_equal);
}
expression(e_new) ::= expression(e1) GREATER expression(e2). {
	e_new = new ComparisonExpression(std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2), ComparisonExpression::type_greater);
}
expression(e_new) ::= expression(e1) GREATER_EQUAL expression(e2). {
	e_new = new ComparisonExpression(std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2), ComparisonExpression::type_greater_equal);
}
expression(e_new) ::= expression(e1) LESS expression(e2). {
	e_new = new ComparisonExpression(std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2), ComparisonExpression::type_less);
}
expression(e_new) ::= expression(e1) LESS_EQUAL expression(e2). {
	e_new = new ComparisonExpression(std::unique_ptr<IExpression>(e1), std::unique_ptr<IExpression>(e2), ComparisonExpression::type_less_equal);
}





/////////////////////////////////////////////////////////////////////////////
// Rules specific for Expressions
/////////////////////////////////////////////////////////////////////////////
result ::= expression(e). {
	parseTree->SetResult(std::unique_ptr<IExpression>(e));
	e = nullptr;
}
result ::= error. // we have to define a symbol of type error somewhere to trigger the error handling routines