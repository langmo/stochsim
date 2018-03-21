// Configuration of output
%token_prefix TOKEN_
%start_symbol model
%parse_failure {throw std::exception("Syntax error.");}
%stack_overflow {throw std::exception("Parser stack overflow while parsing cmdl file.");}
%name cmdl_internal_Parse
%token_type {TerminalSymbol*}
%token_destructor {
	delete $$;
	$$ = nullptr;
}
%extra_argument {CmdlParseTree* parseTree}

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

%include {#include "cmdl_symbols.h"}
%include {#include "CmdlParseTree.h"}
%include {#include "CmdlParser.h"}

%include {#include  <assert.h>}
%include {using namespace expression;}
%include {using namespace cmdlparser;}


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


////////////////////////////
// Variables
////////////////////////////
%type variable {identifier*}
%destructor variable { 
	delete $$;
	$$ = nullptr;
}
variable(v) ::= IDENTIFIER(I) LEFT_SQUARE expression(e) RIGHT_SQUARE. {
	identifier name = *I;
	delete I;
	I = nullptr;
	v = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;

	auto value = static_cast<size_t>(parseTree->GetExpressionValue(e_temp.get())+0.5);
	v = new identifier(name+"["+std::to_string(value)+"]");
}
variable(v) ::= IDENTIFIER(I). {
	v = new identifier(*I);
	delete I;
	I = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// Rules to parse mathematical expressions
/////////////////////////////////////////////////////////////////////////////

// Basic mathematical expressions
%type expression {IExpression*}
%destructor expression { 
	delete $$;
	$$ = nullptr;
}
expression(e) ::= variable(I). [SEMICOLON] {
	e = new VariableExpression(*I);
	delete I;
	I = nullptr;
}
expression(e) ::= variable(I) LEFT_ROUND arguments(as) RIGHT_ROUND . [SEMICOLON] {
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
// Rules specific for CMDL files
/////////////////////////////////////////////////////////////////////////////
// assignments
assignment ::= variable(I) ASSIGN expression(e) SEMICOLON. {
	// create_variable might throw an exception, which results in automatic destruction of I and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *I;
	delete I;
	I = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;

	parseTree->CreateVariable(std::move(name), parseTree->GetExpressionValue(e_temp.get()));
}

assignment ::= variable(I) ASSIGN LEFT_SQUARE expression(e) RIGHT_SQUARE SEMICOLON. {
	// create_variable might throw an exception, which results in automatic destruction of I and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	identifier name = *I;
	delete I;
	I = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;

	parseTree->CreateVariable(std::move(name), std::move(e_temp));
}


// reaction
reaction ::= reactionLeftSide(reactants) ARROW reactionRightSide(products) COMMA reactionSpecifiers(rss) SEMICOLON. {
	// create_reaction might throw an exception, which results in automatic destruction of reactants, products and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionLeftSide>(reactants);
	auto products_temp = std::unique_ptr<ReactionRightSide>(products);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(rss);
	rss = nullptr;
	reactants = nullptr;
	products = nullptr;

	parseTree->CreateReaction(std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}

reaction ::= variable(I) COMMA reactionLeftSide(reactants) ARROW reactionRightSide(products) COMMA reactionSpecifiers(rss) SEMICOLON. {
	// create_reaction might throw an exception, which results in automatic destruction of reactants, products and e by the parser. We thus have to make sure that
	// they point to null to avoid double deletion.
	auto reactants_temp = std::unique_ptr<ReactionLeftSide>(reactants);
	auto products_temp = std::unique_ptr<ReactionRightSide>(products);
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(rss);
	identifier name = *I; 
	rss = nullptr;
	reactants = nullptr;
	products = nullptr;
	delete I;
	I = nullptr;

	parseTree->CreateReaction(std::move(name), std::move(reactants_temp), std::move(products_temp), std::move(rss_temp));
}

%type reactionSpecifiers {ReactionSpecifiers*}
%destructor reactionSpecifiers { 
	delete $$;
	$$ = nullptr;
}
reactionSpecifiers(rss) ::= reactionSpecifier(rs) . {
	auto rss_temp = std::make_unique<ReactionSpecifiers>();
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(rs);
	rs = nullptr;
	rss = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	rss = rss_temp.release();
}
reactionSpecifiers(rss_new) ::= reactionSpecifiers(rss_old) COMMA reactionSpecifier(rs) . {
	auto rss_temp = std::unique_ptr<ReactionSpecifiers>(rss_old);
	rss_old = nullptr;
	rss_new = nullptr;
	auto rs_temp = std::unique_ptr<ReactionSpecifier>(rs);
	rs = nullptr;
	rss_temp->PushBack(std::move(rs_temp));
	rss_new = rss_temp.release();
}

%type reactionSpecifier {ReactionSpecifier*}
%destructor reactionSpecifier { 
	delete $$;
	$$ = nullptr;
}
reactionSpecifier(rs) ::= expression(e). {
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;
	rs = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	rs = new ReactionSpecifier(ReactionSpecifier::rate_type, std::make_unique<NumberExpression>(value));
}

reactionSpecifier(rs) ::= variable(I) COLON expression(e). {
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;
	rs = nullptr;
	identifier name = *I;
	delete I;
	I = nullptr;
	auto value = parseTree->GetExpressionValue(e_temp.get());
	rs = new ReactionSpecifier(name, std::make_unique<NumberExpression>(value));
}

reactionSpecifier(rs) ::= LEFT_SQUARE expression(e) RIGHT_SQUARE. {
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;
	rs = nullptr;
	rs = new ReactionSpecifier(ReactionSpecifier::rate_type, std::move(e_temp));
}

///////////////////////////
// Left side of reaction
///////////////////////////

%type reactionLeftSide {ReactionLeftSide*}
%destructor reactionLeftSide { 
	delete $$;
	$$ = nullptr;
}
reactionLeftSide(rs) ::= . [SEMICOLON] {
	rs = new ReactionLeftSide();
}
reactionLeftSide(rs) ::= reactionLeftComponent(rc). [MULTIPLY]{
	auto rc_temp = std::unique_ptr<ReactionLeftComponent>(rc);
	rc = nullptr;
	rs = nullptr;

	auto rs_temp = std::make_unique<ReactionLeftSide>();
	rs_temp->PushBack(std::move(rc_temp));
	rs = rs_temp.release();
}
reactionLeftSide(rs_new) ::= reactionLeftSide(rs_old) PLUS reactionLeftComponent(rc). [MULTIPLY]{
	rs_new = rs_old;
	rs_old = nullptr;
	auto rc_temp = std::unique_ptr<ReactionLeftComponent>(rc);
	rc = nullptr;

	rs_new->PushBack(std::move(rc_temp));
}

reactionLeftSide ::= expression(e1) PLUS expression(e2). [MULTIPLY] {
	delete(e1);
	e1=nullptr;
	delete(e2);
	e2=nullptr;
	throw std::exception("Reactants or modifiers of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}

reactionLeftSide ::= reactionLeftSide(rs_old) PLUS expression(e). [PLUS] {
	delete(e);
	e=nullptr;
	delete(rs_old);
	rs_old=nullptr;
	throw std::exception("Reactants or modifiers of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}

%type moleculePropertyNames {MoleculePropertyNames*}
%destructor moleculePropertyNames { 
	delete $$;
	$$ = nullptr;
}
moleculePropertyNames(as) ::= . [SEMICOLON] {
	as = new MoleculePropertyNames();
	as->push_back("");
}
moleculePropertyNames(as) ::= variable(I). [COMMA]{
	identifier name = *I;
	delete I;
	I = nullptr;

	as = new MoleculePropertyNames();
	as->push_back(name);
}
moleculePropertyNames(as_new) ::= moleculePropertyNames(as_old) COMMA variable(I). [COMMA]{
	as_new = as_old;
	as_old = nullptr;
	identifier name = *I;
	delete I;
	I = nullptr;

	as_new->push_back(name);
}
moleculePropertyNames(as_new) ::= moleculePropertyNames(as_old) COMMA . [COMMA]{
	as_new = as_old;
	as_old = nullptr;
	as_new->push_back("");
}

%type reactionLeftComponent {ReactionLeftComponent*}
%destructor reactionLeftComponent { 
	delete $$;
	$$ = nullptr;
}
reactionLeftComponent(rc) ::= variable(I). [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;

	rc = new ReactionLeftComponent(state, 1, false);
}
reactionLeftComponent(rc) ::= variable(I) LEFT_CURLY moleculePropertyNames(as) RIGHT_CURLY. [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyNames>(as);
	as = nullptr;

	rc = new ReactionLeftComponent(state, 1, false, std::move(as_temp));
}

reactionLeftComponent(rc) ::= DOLLAR variable(I). [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;

	rc = new ReactionLeftComponent(state, 1, true);
}

reactionLeftComponent(rc) ::= DOLLAR variable(I) LEFT_CURLY moleculePropertyNames(as) RIGHT_CURLY. [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyNames>(as);
	as = nullptr;

	rc = new ReactionLeftComponent(state, 1, true, std::move(as_temp));
}

reactionLeftComponent(rc_new) ::= expression(e) MULTIPLY reactionLeftComponent(rc_old). [EXP]{
	auto rc_temp = std::unique_ptr<ReactionLeftComponent>(rc_old);
	rc_old = nullptr;
	rc_new = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	if(stochiometry<=0)
		throw std::exception("Stochiometry must be positive.");
	rc_temp->SetStochiometry(static_cast<stochsim::Stochiometry>(rc_temp->GetStochiometry()*stochiometry));
	rc_new = rc_temp.release();
}

///////////////////////////
// Right side of reaction
///////////////////////////

%type reactionRightSide {ReactionRightSide*}
%destructor reactionRightSide { 
	delete $$;
	$$ = nullptr;
}
reactionRightSide(rs) ::= . [SEMICOLON] {
	rs = new ReactionRightSide();
}
reactionRightSide(rs) ::= reactionRightComponent(rc). [MULTIPLY]{
	auto rc_temp = std::unique_ptr<ReactionRightComponent>(rc);
	rc = nullptr;
	rs = nullptr;

	auto rs_temp = std::make_unique<ReactionRightSide>();
	rs_temp->PushBack(std::move(rc_temp));
	rs = rs_temp.release();
}
reactionRightSide(rs_new) ::= reactionRightSide(rs_old) PLUS reactionRightComponent(rc). [MULTIPLY]{
	rs_new = rs_old;
	rs_old = nullptr;
	auto rc_temp = std::unique_ptr<ReactionRightComponent>(rc);
	rc = nullptr;

	rs_new->PushBack(std::move(rc_temp));
}

reactionRightSide ::= expression(e1) PLUS expression(e2). [MULTIPLY] {
	delete(e1);
	e1=nullptr;
	delete(e2);
	e2=nullptr;
	throw std::exception("Products or transformees of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}

reactionRightSide ::= reactionRightSide(rs_old) PLUS expression(e). [PLUS] {
	delete(e);
	e=nullptr;
	delete(rs_old);
	rs_old=nullptr;
	throw std::exception("Products or transformees of a reaction must either be state names, or an expression (representing the stochiometry of the state) times the state name, in this order.");
}

%type moleculePropertyExpressions {MoleculePropertyExpressions*}
%destructor moleculePropertyExpressions { 
	delete $$;
	$$ = nullptr;
}
moleculePropertyExpressions(as) ::= . [SEMICOLON] {
	as = new MoleculePropertyExpressions();
	as->push_back(std::unique_ptr<IExpression>(nullptr));
}
moleculePropertyExpressions(as) ::= expression(e). [COMMA]{
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;

	as = new MoleculePropertyExpressions();
	as->push_back(std::move(e_temp));
}
moleculePropertyExpressions(as_new) ::= moleculePropertyExpressions(as_old) COMMA expression(e). [COMMA]{
	as_new = as_old;
	as_old = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;

	as_new->push_back(std::move(e_temp));
}
moleculePropertyExpressions(as_new) ::= moleculePropertyExpressions(as_old) COMMA . [COMMA]{
	as_new = as_old;
	as_old = nullptr;

	as_new->push_back(std::unique_ptr<IExpression>(nullptr));
}

%type reactionRightComponent {ReactionRightComponent*}
%destructor reactionRightComponent { 
	delete $$;
	$$ = nullptr;
}
reactionRightComponent(rc) ::= variable(I). [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;

	rc = new ReactionRightComponent(state, 1, false);
}
reactionRightComponent(rc) ::= variable(I) LEFT_CURLY moleculePropertyExpressions(as) RIGHT_CURLY. [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyExpressions>(as);
	as = nullptr;

	rc = new ReactionRightComponent(state, 1, false, std::move(as_temp));
}

reactionRightComponent(rc) ::= DOLLAR variable(I). [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;

	rc = new ReactionRightComponent(state, 1, true);
}

reactionRightComponent(rc) ::= DOLLAR variable(I) LEFT_CURLY moleculePropertyExpressions(as) RIGHT_CURLY. [EXP]{
	identifier state = *I;
	delete I;
	I = nullptr;
	rc = nullptr;
	auto as_temp = std::unique_ptr<MoleculePropertyExpressions>(as);
	as = nullptr;

	rc = new ReactionRightComponent(state, 1, true, std::move(as_temp));
}

reactionRightComponent(rc_new) ::= expression(e) MULTIPLY reactionRightComponent(rc_old). [EXP]{
	auto rc_temp = std::unique_ptr<ReactionRightComponent>(rc_old);
	rc_old = nullptr;
	rc_new = nullptr;
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;

	auto stochiometry = parseTree->GetExpressionValue(e_temp.get());
	if(stochiometry<=0)
		throw std::exception("Stochiometry must be positive.");
	rc_temp->SetStochiometry(static_cast<stochsim::Stochiometry>(rc_temp->GetStochiometry()*stochiometry));
	rc_new = rc_temp.release();
}

reactionRightComponent(rc) ::= LEFT_SQUARE expression(e) QUESTIONMARK reactionRightSide(s1) COLON reactionRightSide(s2) RIGHT_SQUARE . [EXP]{
	auto e_temp = std::unique_ptr<IExpression>(e);
	e = nullptr;
	auto s1_temp = std::unique_ptr<ReactionRightSide>(s1);
	auto s2_temp = std::unique_ptr<ReactionRightSide>(s2);
	s1 = nullptr;
	s2 = nullptr;
	rc = nullptr;

	identifier state = parseTree->CreateChoice(std::move(e_temp), std::move(s1_temp), std::move(s2_temp));
	rc = new ReactionRightComponent(state, 1, false);
}




// We just ignore that a model has a name. Included only for compatibility with Dizzy files
preprocessorDirective ::= MODEL_NAME IDENTIFIER SEMICOLON.

// include a file
preprocessorDirective ::= INCLUDE variable(I) SEMICOLON. {
	identifier fileName = *I;
	delete I;
	I = nullptr;
	parseTree->IncludeFile(fileName);
}

// A model consists of a set of statements.
model ::= statements.
statements ::= statements statement.
statements ::= .

// A statement can either be a variable assignment a reaction, or a preprocessor directive
statement ::= assignment.
statement ::= reaction.
statement ::= preprocessorDirective.
statement ::= error. // we have to define a symbol of type error somewhere to trigger the error handling routines