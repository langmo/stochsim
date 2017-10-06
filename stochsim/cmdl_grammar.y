// Configuration of output
%token_prefix TOKEN_
%start_symbol model
%parse_failure {throw std::exception("Syntax error.");}
%stack_overflow {throw std::exception("Parser stack overflow while parsing cmdl file.");}
%name Parse
%token_type {terminal_symbol*}
%token_destructor {
	delete $$;
	$$ = nullptr;
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
			fopen_s(&traceFile_, errorFileName.c_str(), "w");
			if(traceFile_)
				ParseTrace(traceFile_, "cmdl_");
			else
				ParseTrace(0, "cmdl_");
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
		ParseTrace(0, "cmdl_");
		if(handle_)
			ParseFree(handle_, free); 
		handle_ = nullptr;
		if (traceFile_)
			fclose(traceFile_);
		traceFile_ = nullptr;
	}

	void stochsim::cmdl::parser::operator()(int tokenID, cmdl::terminal_symbol* token, cmdl::parse_tree& parseTree)
	{
		if (!handle_)
		{
			throw std::exception("Parser handle invalid.");
		}
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
%left AND.
%left OR.
%nonassoc EQUAL NOT_EQUAL GREATER GREATER_EQUAL LESS LESS_EQUAL.
%left PLUS MINUS.
%left MULTIPLY DIVIDE.
%right EXP NOT.
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
statement ::= error. // we have to define a symbol of type error somewhere to trigger the error handling routines

// Basic mathematical expressions
%type expression {expression_base*}
%destructor expression { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= IDENTIFIER(I). [SEMICOLON] {
	e = new variable_expression(*I);
	delete I;
	I = nullptr;
}
expression(e) ::= VALUE(V). [SEMICOLON]{
	e = new number_expression(*V);
	delete V;
	V = nullptr;
}
expression(e_new) ::= LEFT_ROUND expression(e_old) RIGHT_ROUND. {
	e_new = e_old;
} // forces sums, ..., to be converted to an expression.


// Sum
%type sum {sum_expression*}
%destructor sum { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= sum(s). [SEMICOLON] {
	e = s;
}
sum(s) ::= expression(e1) PLUS expression(e2). {
	s = new sum_expression();
	s->push_back(false, std::unique_ptr<expression_base>(e1));
	s->push_back(false, std::unique_ptr<expression_base>(e2));
}
sum(s) ::= expression(e1) MINUS expression(e2). {
	s = new sum_expression();
	s->push_back(false,  std::unique_ptr<expression_base>(e1));
	s->push_back(true, std::unique_ptr<expression_base>(e2));
}
sum(s_new) ::= sum(s_old) PLUS expression(e). {
	s_new = s_old;
	s_new->push_back(false, std::unique_ptr<expression_base>(e));
}
sum(s_new) ::= sum(s_old) MINUS expression(e). {
	s_new = s_old;
	s_new->push_back(true, std::unique_ptr<expression_base>(e));
}


// Product
%type product {product_expression*}
%destructor product { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= product(p). [SEMICOLON] {
	e = p;
}
product(p) ::= expression(e1) MULTIPLY expression(e2). {
	p = new product_expression();
	p->push_back(false, std::unique_ptr<expression_base>(e1));
	p->push_back(false, std::unique_ptr<expression_base>(e2));

}
product(p) ::= expression(e1) DIVIDE expression(e2). {
	p = new product_expression();
	p->push_back(false, std::unique_ptr<expression_base>(e1));
	p->push_back(true, std::unique_ptr<expression_base>(e2));

}
product(p_new) ::= product(p_old) MULTIPLY expression(e). {
	p_new = p_old;
	p_new->push_back(false, std::unique_ptr<expression_base>(e));
}
product(p_new) ::= product(p_old) DIVIDE expression(e). {
	p_new = p_old;
	p_new->push_back(true, std::unique_ptr<expression_base>(e));
}

// conjunction
%type conjunction {conjunction_expression*}
%destructor conjunction { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= conjunction(c). [SEMICOLON] {
	e = c;
}
conjunction(c) ::= expression(e1) AND expression(e2). {
	c = new conjunction_expression();
	c->push_back(false, std::unique_ptr<expression_base>(e1));
	c->push_back(false, std::unique_ptr<expression_base>(e2));

}
conjunction(c_new) ::= conjunction(c_old) AND expression(e). {
	c_new = c_old;
	c_new->push_back(false, std::unique_ptr<expression_base>(e));
}

// disjunction
%type disjunction {disjunction_expression*}
%destructor disjunction { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= disjunction(c). [SEMICOLON] {
	e = c;
}
disjunction(c) ::= expression(e1) OR expression(e2). {
	c = new disjunction_expression();
	c->push_back(false, std::unique_ptr<expression_base>(e1));
	c->push_back(false, std::unique_ptr<expression_base>(e2));

}
disjunction(c_new) ::= disjunction(c_old) OR expression(e). {
	c_new = c_old;
	c_new->push_back(false, std::unique_ptr<expression_base>(e));
}

// not
expression(e_new) ::= NOT expression(e_old). {
	e_new = new not_expression(std::unique_ptr<expression_base>(e_old));
}

// unary minus
expression(e_new) ::= MINUS expression(e_old). [NOT] {
	e_new = new minus_expression(std::unique_ptr<expression_base>(e_old));
}

// Exponentiation
expression(e_new) ::= expression(e1) EXP expression(e2). {
	e_new = new exponentiation_expression(std::unique_ptr<expression_base>(e1), std::unique_ptr<expression_base>(e2));
}


// Comparison
expression(e_new) ::= expression(e1) EQUAL expression(e2). {
	e_new = new comparison_expression(std::unique_ptr<expression_base>(e1), std::unique_ptr<expression_base>(e2), comparison_expression::type_equal);
}
expression(e_new) ::= expression(e1) NOT_EQUAL expression(e2). {
	e_new = new comparison_expression(std::unique_ptr<expression_base>(e1), std::unique_ptr<expression_base>(e2), comparison_expression::type_not_equal);
}
expression(e_new) ::= expression(e1) GREATER expression(e2). {
	e_new = new comparison_expression(std::unique_ptr<expression_base>(e1), std::unique_ptr<expression_base>(e2), comparison_expression::type_greater);
}
expression(e_new) ::= expression(e1) GREATER_EQUAL expression(e2). {
	e_new = new comparison_expression(std::unique_ptr<expression_base>(e1), std::unique_ptr<expression_base>(e2), comparison_expression::type_greater_equal);
}
expression(e_new) ::= expression(e1) LESS expression(e2). {
	e_new = new comparison_expression(std::unique_ptr<expression_base>(e1), std::unique_ptr<expression_base>(e2), comparison_expression::type_less);
}
expression(e_new) ::= expression(e1) LESS_EQUAL expression(e2). {
	e_new = new comparison_expression(std::unique_ptr<expression_base>(e1), std::unique_ptr<expression_base>(e2), comparison_expression::type_less_equal);
}

// assignments
assignment ::= IDENTIFIER(I) ASSIGN expression(e) SEMICOLON. {
	// create_variable might throw an exception, which results in automatic destruction of I and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto I_temp = I;
	auto e_temp = e;
	I = nullptr;
	e = nullptr;

	parseTree->create_variable(std::unique_ptr<terminal_symbol>(I_temp), parseTree->get_expression_value(e_temp));
	delete e_temp;
	e_temp = nullptr;
}
assignment ::= IDENTIFIER(I) ASSIGN LEFT_SQUARE expression(e) RIGHT_SQUARE SEMICOLON. {
	// create_variable might throw an exception, which results in automatic destruction of I and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto I_temp = I;
	auto e_temp = e;
	I = nullptr;
	e = nullptr;

	parseTree->create_variable(std::unique_ptr<terminal_symbol>(I_temp), std::unique_ptr<expression_base>(e_temp));
}


// reaction
reaction ::= reactionSide(reactants) ARROW reactionSide(products) COMMA expression(e) SEMICOLON. {
	// create_reaction might throw an exception, which results in automatic destruction of reactants, products and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = reactants;
	auto products_temp = products;
	auto e_temp = e;
	reactants = nullptr;
	products = nullptr;
	e = nullptr;

	parseTree->create_reaction(std::unique_ptr<sum_expression>(reactants_temp), std::unique_ptr<sum_expression>(products_temp), parseTree->get_expression_value(e_temp));
	delete e_temp;
	e_temp = nullptr;
}
reaction ::= reactionSide(reactants) ARROW reactionSide(products) COMMA LEFT_SQUARE expression(e) RIGHT_SQUARE SEMICOLON. {
	// create_reaction might throw an exception, which results in automatic destruction of reactants, products and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = reactants;
	auto products_temp = products;
	auto e_temp = e;
	reactants = nullptr;
	products = nullptr;
	e = nullptr;

	parseTree->create_reaction(std::unique_ptr<sum_expression>(reactants_temp), std::unique_ptr<sum_expression>(products_temp), std::unique_ptr<expression_base>(e_temp));
}

%type reactionSide {sum_expression*}
%destructor reactionSide { 
	delete $$;
	$$ = nullptr;
}
reactionSide(rs) ::= . [SEMICOLON] {
	rs = new sum_expression();
}
reactionSide(rs) ::= reactionComponent(rc). [MULTIPLY]{
	rs = new sum_expression();
	rs->push_back(false, std::unique_ptr<product_expression>(rc));
}
reactionSide(rs_new) ::= reactionSide(rs_old) PLUS reactionComponent(rc). [MULTIPLY]{
	rs_new = rs_old;
	rs_new->push_back(false, std::unique_ptr<product_expression>(rc));
}

reactionSide ::= expression(e). [PLUS] {
	delete(e);
	e=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}
reactionSide ::= reactionSide(rs_old) PLUS expression(e). [PLUS] {
	delete(e);
	e=nullptr;
	delete(rs_old);
	rs_old=nullptr;
	throw std::exception("Reactants or products of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}

%type reactionComponent {product_expression*}
%destructor reactionComponent { 
	delete $$;
	$$ = nullptr;
}
reactionComponent(rc) ::= IDENTIFIER(I). [EXP]{
	rc = new product_expression();
	rc->push_back(false, std::make_unique<number_expression>(1));
	rc->push_back(false, std::make_unique<variable_expression>(*I));
	delete I;
	I = nullptr;
}

reactionComponent(rc) ::= expression(e) MULTIPLY IDENTIFIER(I). [EXP]{
	rc = new product_expression();
	rc->push_back(false, std::unique_ptr<expression_base>(e));
	rc->push_back(false, std::make_unique<variable_expression>(*I));
	delete I;
	I = nullptr;
}



