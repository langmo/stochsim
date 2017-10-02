// Configuration of output
%token_prefix TOKEN_
%start_symbol model
%parse_failure {throw std::exception("Failed parsing cmdl file.");}
%stack_overflow {throw std::exception("Parser stack overflow while parsing cmdl file.");}
%name Parse
%token_type {terminal_symbol*}
%token_destructor {
	delete $$;
}
%extra_argument {parse_tree* parseTree}
%include {#include "cmdl_grammar_definitions.h"}
%include {#include  <assert.h>}
%include {using namespace stochsim::expression;}
%include {using namespace stochsim::cmdl;}
%include {#undef NDEBUG} // necessary for ParseTrace
// Convert c-style parsing functions to implementation of clean c++ class handling everything.
%include {
	// Forward declaration parser functions.
	void Parse(
		void *yyp,                   /* The parser */
		int yymajor,                 /* The major token code number */
		terminal_symbol* yyminor,       /* The value for the token */
		parse_tree* parse_tree               /* Optional %extra_argument parameter */
	);

	void *ParseAlloc(void* (*mallocProc)(size_t));

	void ParseFree(
		void *p,                    /* The parser to be deleted */
		void(*freeProc)(void*)     /* Function used to reclaim memory */
	);
	void ParseTrace(FILE *TraceFILE, char *zTracePrompt);
	stochsim::cmdl::parser::parser(std::string errorFileName)
	{
		if (errorFileName.empty())
		{
			traceFile_ = nullptr;
		}
		else
		{
			traceFile_ = fopen(errorFileName.c_str(), "w");
			if(traceFile_)
				ParseTrace(traceFile_, "test_");
		}
		try
		{
			handle_ = ParseAlloc(malloc);
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
	stochsim::cmdl::parser::~parser()
	{
		if(handle_)
			ParseFree(handle_, free); 
		handle_ = nullptr;
		if (traceFile_)
			fclose(traceFile_);
		traceFile_ = nullptr;
	}

	void stochsim::cmdl::parser::operator()(int tokenID, cmdl::terminal_symbol* token, cmdl::parse_tree& parseTree)
	{
		try
		{
			Parse(handle_, tokenID, token, &parseTree);
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
%left PLUS MINUS.
%left MULTIPLY DIVIDE.
%nonassoc IDENTIFIER VALUE. // always prefer longer rules



/////////////////////////////////////////////////////////////////////////////
// Rules
/////////////////////////////////////////////////////////////////////////////
// A model consists of a set of statements.
model ::= statements.
statements ::= statements statement.
statements ::= .

// A statement can either be a variable assignment or a reaction
statement ::= assignment.
statement ::= reaction.

// Basic mathematical expressions
%type expression {basic_expression*}
%destructor expression { 
	delete $$;
}
expression(e) ::= IDENTIFIER(I). {
	e = new variable_expression(*I);
	delete I;
}
expression(e) ::= VALUE(V). {
	e = new number_expression(*V);
	delete V;
}
expression(e_new) ::= LEFT_ROUND expression(e_old) RIGHT_ROUND. {
	e_new = e_old;
} // forces sums, ..., to be converted to an expression.


// Sum
%type sum {sum_expression*}
%destructor sum { 
	delete $$;
}
expression(e) ::= sum(s). [SEMICOLON] {
	e = s;
}
sum(s) ::= expression(e1) PLUS expression(e2). {
	s = new sum_expression();
	s->push_back(std::unique_ptr<basic_expression>(e1), true);
	s->push_back(std::unique_ptr<basic_expression>(e2), true);
}
sum(s) ::= expression(e1) MINUS expression(e2). {
	s = new sum_expression();
	s->push_back(std::unique_ptr<basic_expression>(e1), true);
	s->push_back(std::unique_ptr<basic_expression>(e2), false);
}
sum(s_new) ::= sum(s_old) PLUS expression(e). {
	s_new = s_old;
	s_new->push_back(std::unique_ptr<basic_expression>(e), true);
}
sum(s_new) ::= sum(s_old) MINUS expression(e). {
	s_new = s_old;
	s_new->push_back(std::unique_ptr<basic_expression>(e), false);
}


// Product
%type product {product_expression*}
%destructor product { 
	delete $$;
}
expression(e) ::= product(p). [SEMICOLON] {
	e = p;
}
product(p) ::= expression(e1) MULTIPLY expression(e2). {
	p = new product_expression();
	p->push_back(std::unique_ptr<basic_expression>(e1), true);
	p->push_back(std::unique_ptr<basic_expression>(e2), true);

}
product(p) ::= expression(e1) DIVIDE expression(e2). {
	p = new product_expression();
	p->push_back(std::unique_ptr<basic_expression>(e1), true);
	p->push_back(std::unique_ptr<basic_expression>(e2), false);

}
product(p_new) ::= product(p_old) MULTIPLY expression(e). {
	p_new = p_old;
	p_new->push_back(std::unique_ptr<basic_expression>(e), true);
}
product(p_new) ::= product(p_old) DIVIDE expression(e). {
	p_new = p_old;
	p_new->push_back(std::unique_ptr<basic_expression>(e), false);
}


// An assignment associates a variable with an expression.
assignment ::= IDENTIFIER(I) EQUAL expression(e) SEMICOLON. {
	parseTree->create_variable(std::unique_ptr<terminal_symbol>(I), e->eval(parseTree->get_lookup()));
	delete e;
}
assignment ::= IDENTIFIER(I) EQUAL LEFT_SQUARE expression(e) RIGHT_SQUARE SEMICOLON. {
	parseTree->create_variable(std::unique_ptr<terminal_symbol>(I), std::unique_ptr<basic_expression>(e));
}


// reaction
reaction ::= reactionSide(reactants) ARROW reactionSide(products) COMMA expression(e) SEMICOLON. {
	parseTree->create_reaction(std::unique_ptr<sum_expression>(reactants), std::unique_ptr<sum_expression>(products), e->eval(parseTree->get_lookup()));
	delete e;
}
reaction ::= reactionSide(reactants) ARROW reactionSide(products) COMMA LEFT_SQUARE expression(e) RIGHT_SQUARE SEMICOLON. {
	parseTree->create_reaction(std::unique_ptr<sum_expression>(reactants), std::unique_ptr<sum_expression>(products), std::unique_ptr<basic_expression>(e));
}

%type reactionSide {sum_expression*}
%destructor reactionSide { 
	delete $$;
}
reactionSide(rs) ::= sum(s). [PLUS] {
	rs = s;
}
reactionSide(rs) ::= expression(e). [SEMICOLON] {
	rs = new sum_expression();
	rs->push_back(std::unique_ptr<basic_expression>(e), true);
}
reactionSide(rs) ::= . [SEMICOLON] {
	rs = new sum_expression();
}


