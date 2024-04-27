#ifndef PARSER_H
#define PARSER_H
#include "Parser.h"
#include "Error.h"
#endif

/*
	parsing many of statements. it is called two times.
	one at the start of program to get all the statements, 
	the other inside control and loop statetments because they 
	have a number of statements inside the bodies. it returns
	the base class that consists of all the statements inside the
	scope body.
*/
Base* Parser::parseS()   
{
	llvm::SmallVector<Statement*> statements;
	while (!Tok.is(Token::eof))
	{
		switch (Tok.getKind())
		{
		case Token::KW_int:
		{
			llvm::SmallVector<DecStatement*> states = parseDefineInt();
			if (states.size() == 0)
			{
				return nullptr;
			}

			while (states.size() > 0)
			{
				statements.push_back(states.back());
				states.pop_back();
			}
			break;
		}
		case Token::KW_bool:
		{
			llvm::SmallVector<DecStatement*> states = parseDefineBool();
			if (states.size() == 0)
			{
				return nullptr;
			}

			while (states.size() > 0)
			{
				statements.push_back(states.back());
				states.pop_back();	
			}
			break;
			
		}
		case Token::ident:
		{
			Token temp = Tok;
			const char* ptr = Lexer::getBufferPtr();
			AssignStatement* statement = parseAssignBool();
			if (!statement){
				Tok = temp;
				Tok.setBufferPtr(ptr);
				AssignStatement* statement = parseAssignInt();
			}
			statements.push_back(statement);
			break;
		}
		case Token::KW_if:
		{
			IfStatement* statement = parseIf();
			statements.push_back(statement);
			break;
		}
		case Token::KW_for:
		{
			ForStatement* statement = parseFor();
			statements.push_back(statement);
			break;
		}
		case Token::KW_while:
		{
			WhileStatement* statement = parseWhile();
			statements.push_back(statement);
			break;
		}
		case Token::start_comment:
		{
			parseComment();
			break;
		}
		case Token::KW_print:
		{
			PrintStatement* statement = parsePrint();
			statements.push_back(statement);           
			break;
		}

		default:
		{
			return new Base(statements);
		}

		}
	}
	return new Base(statements);
}


/*
	parses declaration statements like int a, b, c;
*/
llvm::SmallVector<DecStatement*> Parser::parseDefineInt()  
{
	llvm::SmallVector<DecStatement*> assignments;
	llvm::SmallVector<Expression*> variables;
	llvm::SmallVector<Expression*> values;

	advance();  // pass "int"

	if (!Tok.is(Token::ident))
	{
		Error::VariableNameNotFound();
	}
	Expression* lhand = new Expression(Tok.getText());
	variables.push_back(lhand);
	advance(); // pass id

	if(Tok.is(Token::equal)){
		advance(); // pass "="
		Expression* rhand = parseExpr();
		values.push_back(rhand);
	}
	else{
		Expression* rhand = new Expression(0);  // default init
		values.push_back(rhand);
	}

	bool SeenTokenVariable = true;
	while (SeenTokenVariable){

		if (Tok.is(Token::comma)){
			
			advance(); pass ","
			if (!Tok.is(Token::ident)){
				Error::VariableNameNotFound();
			}
			Expression* lhand = new Expression(Tok.getText());
			variables.push_back(lhand);
			advance(); // pass id

			if (Tok.is(Token::equal)){
				advance(); // pass "="
				Expression* rhand = parseExpr();
				values.push_back(rhand);
			}
			else {
				Expression* rhand = new Expression(0);  // default init
				values.push_back(rhand);
			}

		}
		else if (Tok.is(Token::semi_colon)){
			SeenTokenVariable = false;
		}
		else {
			Error::SemiColonNotFound();
		}
	}

	while (variables.size() != 0)
	{
		assignments.push_back(new DecStatement(variables.front(), values.front()));
		variables.erase(variables.begin());
		values.erase(values.begin());
	}

	advance(); // pass ";"
	return assignments;
}


llvm::SmallVector<DecStatement*> Parser::parseDefineBool()  
{
	llvm::SmallVector<DecStatement*> assignments;
	llvm::SmallVector<Expression*> variables;
	llvm::SmallVector<Expression*> values;

	advance();  // pass "Bool"

	if (!Tok.is(Token::ident))
	{
		Error::VariableNameNotFound();
	}
	Expression* lhand = new Expression(Tok.getText());
	variables.push_back(lhand);
	advance(); // pass id

	if(Tok.is(Token::equal)){
		advance(); // pass "="
		Expression* rhand = parseCondition();
		values.push_back(rhand);
	}
	else{
		Expression* rhand = new Expression(false);  // default init
		values.push_back(rhand);
	}

	bool SeenTokenVariable = true;
	while (SeenTokenVariable){

		if (Tok.is(Token::comma)){
			
			advance(); pass ","
			if (!Tok.is(Token::ident)){
				Error::VariableNameNotFound();
			}
			Expression* lhand = new Expression(Tok.getText());
			variables.push_back(lhand);
			advance(); // pass id

			if (Tok.is(Token::equal)){
				advance(); // pass "="
				Expression* rhand = parseCondition();
				values.push_back(rhand);
			}
			else {
				Expression* rhand = new Expression(false);  // default init
				values.push_back(rhand);
			}

		}
		else if (Tok.is(Token::semi_colon)){
			SeenTokenVariable = false;
		}
		else {
			Error::SemiColonNotFound();
		}
	}

	while (variables.size() != 0)
	{
		assignments.push_back(new DecStatement(variables.front(), values.front()));
		variables.erase(variables.begin());
		values.erase(values.begin());
	}

	advance(); // pass ";"
	return assignments;
}


/*
	parses the expression
*/
Expression* Parser::parseExpr()
{
	Expression* Left = parseTerm();
	while (Tok.isOneOf(Token::plus, Token::minus))
	{
		BinaryOp::Operator Op = Tok.is(Token::plus) ? BinaryOp::Plus : BinaryOp::Minus;
		advance();
		Expression* Right = parseTerm();
		Left = new BinaryOp(Op, Left, Right);
	}
	return Left;
}

/*
	parses the term
*/
Expression* Parser::parseTerm()
{
	Expression* Left = parsePower();
	while (Tok.isOneOf(Token::star, Token::slash, Token::mod))
	{
		BinaryOp::Operator Op = Tok.is(Token::star) ? BinaryOp::Mul : Tok.is(Token::slash) ? BinaryOp::Div : BinaryOp::Mod;
		advance();
		Expression* Right = parsePower();
		Left = new BinaryOp(Op, Left, Right);
	}
	return Left;
}

Expression* Parser::parsePower()
{
	Expression* Left = parseFactor();
	while (Tok.is(Token::power))
	{
		BinaryOp::Operator Op = BinaryOp::Pow;
		advance();
		Expression* Right = parseFactor();
		Left = new BinaryOp(Op, Left, Right);
	}
	return Left;
}

Expression* Parser::parseFactor()  
{
	Expression* Res = nullptr;
	switch (Tok.getKind())
	{
	case Token::number:
	{
		int number;
		Tok.getText().getAsInteger(10, number);
		Res = new Expression(number);
		advance();
		break;
	}
	case Token::ident:            
	{
		Res = new Expression(Tok.getText());
		advance();
		break;
	}
	case Token::l_paren:
	{
		advance();
		Res = parseExpr();
		if (!consume(Token::r_paren))
			break;
	}
	case Token::minus:
	{
		advance(); // pass minus
		if(!consume(Token::l_paren))
		{
			Res = parseExpr();
			if (!consume(Token::r_paren))
			{
				return new Expression(false,Res);
			}
			else
			{
				Error::RightParenthesisExpected();
			}

		}
		else if (!consume(Token::number))
		{
			int number;
			Tok.getText().getAsInteger(10, number);
			Res = new Expression(number);
			return new Expression(false,Res); 
		}

		else
		{
			Error::UnexpectedTokenAfterMinusOrPlus();
		}

		break;
	}
	case Token::plus:
	{
		advance(); // pass plus
		if(!consume(Token::l_paren))
		{
			Res = parseExpr();
			if (!consume(Token::r_paren))
			{
				return new Expression(true,Res);
			}
			else
			{
				Error::RightParenthesisExpected();
			}

		}
		else if (!consume(Token::number))
		{
			int number;
			Tok.getText().getAsInteger(10, number);
			Res = new Expression(number);
			return new Expression(true,Res);
		}

		else
		{
			Error::UnexpectedKOOFTafterMinusOrPlus();
		}

		break;
	}
	default: // error handling
	{
		Error::NumberVariableExpected();
	}

	}
	return Res;
}



/*
	parse assignment like a = 3;
*/
AssignStatement* Parser::parseAssignInt() 
{
	Expression* value;
	Expression* Res;
	
	Res = new Expression(Tok.getText()); 
	Res2 = Tok.getText();
	advance();  // pass Id

	if (Tok.is(Token::unery_minus))
	{
		advance();
		value = new UneryOp(UneryOp::Minus, Res2);
	}
	else if (Tok.is(Token::unery_plus))
	{
		advance();
		value = new UneryOp(UneryOp::Plus, Res2);
	}
	else if (Tok.is(Token::minus_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Minus, Res, value);
	}
	else if (Tok.is(Token::plus_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Plus, Res, value);
	}
	else if (Tok.is(Token::star_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Mul, Res, value);
	}
	else if (Tok.is(Token::slash_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Div, Res, value);
	}
	else if (Tok.is(Token::mod_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Mod, Res, value);
	}
	else if (Tok.is(Token::equal))
	{
		advance(); // pass equal
		value = parseExpr();
	}
	else
	{
		Error::AssignmentEqualNotFound();
	}

	if (!Tok.is(Token::semi_colon))
	{
		Error::SemiColonNotFound();
	}

	advance(); // pass semicolon
	return new AssignStatement(Res, value);  
}


AssignStatement* Parser::parseAssignBool()  
{
	Expression* Res = new Expression(Tok.getText());
	advance();  // pass Id

	if (!Tok.is(Token::equal))
	{
		cout<< "Assignment Equal Not Found";
		return nullptr;
	}
	advance();  // pass "="

	Expression* value = parseCondition();

	if (!Tok.is(Token::semi_colon))
	{
		cout<< "Semi colon Not Found";
		return nullptr;
	}

	advance(); // pass semicolon
	return new AssignStatement(Res, value);  
}


WhileStatement* Parser::parseWhile()  
{
	advance(); // pass while

	if (!Tok.is(Token::l_paren))         
	{
		Error::LeftParenthesisExpected();
	}
	advance();
	
	Expression* condition = parseCondition();

	if (!Tok.is(Token::r_paren))
	{
		Error::RightParenthesisExpected();
	}
	advance();

	if (Tok.is(Token::KW_begin))
	{
		advance();

		Base* AllStates = parseStatement();

		if (!consume(Token::KW_end))
		{

			return new WhileStatement(condition, AllStates->getStatements(), Statement::StateMentType::While);  
		}
		else
		{
			Error::EndNotSeenForIf();
		}
	}
	else
	{
		Error::BeginExpectedAfterColon();  
	}
}

// for parser

ForStatement* Parser::parseFor()  
{
	advance();	// pass for

	if (!Tok.is(Token::l_paren))        
	{
		Error::LeftParenthesisExpected();
	}
	advance();

	AssignStatement* assign1 = parseAssignInt();  // check class type

	if (!Tok.is(Token::semi_colon))         
	{
		Error::SemiColonNotFound();  
	}
	advance();

	Expression* condition = parseCondition();

	if (!Tok.is(Token::semi_colon))         
	{
		Error::SemiColonNotFound();  
	}
	advance();

	AssignStatement* assign2 = parseAssignInt();  // check class type

	if (!Tok.is(Token::r_paren))
	{
		Error::RightParenthesisExpected();
	}
	advance();


	if (Tok.is(Token::KW_begin))
	{
		advance();

		Base* AllStates = parseStatement();

		if (!consume(Token::KW_end))
		{

			return new ForStatement(a1, condition, a2, AllStates->getStatements(), Statement::StateMentType::For); 
		}
		else
		{
			Error::EndNotSeenForIf();
		}
	}
	else
	{
		Error::BeginExpectedAfterColon();  
	}
}


/*
	parses condition like 3 > 5+1 and true
*/
Expression* Parser::parseCondition()  
{
	Expression* lcondition;

	lcondition = parseSubCondition();
	if (Tok.is(Token::KW_and))
	{
		advance();
		Expression* rcondition = parseCondition();
		return new Expression(new BooleanOp(BooleanOp::And, lcondition, rcondition));
	}
	else if (Tok.is(Token::KW_or))
	{
		advance();
		Expression* rcondition = parseCondition();
		return new Expression(new BooleanOp(BooleanOp::Or, lcondition, rcondition));
	}

}


/*
	parses a single subcondition like 3 > 5+1
	it does not deal with and, or
*/
Expression* Parser::parseSubCondition()        
{
	if (Tok.is(Token::l_paren))
	{
		advance();
		Expression* expressionInside = parseCondition();
		if (!consume(Token::r_paren))
		{
			return expressionInside;
		}
		else
		{
			cout<< "Right Parenthesis Expected";
			return nullptr;
		}
	}
	else if (Tok.is(Token::KW_true))
	{
		advance();
		return new Expression(true);
	}
	else if (Tok.is(Token::KW_false))
	{
		advance();
		return new Expression(false);
	}
	else if (Tok.is(Token::ident))
	{
		Expression* Res = new Expression(Tok.getText());
		advance();
		return Res;
	}
	else
	{
		Expression* lhand = parseExpr();
		BooleanOp::Operator Op;

		if (Tok.is(Token::less))
		{
			Op = BooleanOp::Less;
		}
		else if (Tok.is(Token::less_equal))
		{
			Op = BooleanOp::LessEqual;
		}
		else if (Tok.is(Token::greater))
		{
			Op = BooleanOp::Greater;
		}
		else if (Tok.is(Token::greater_equal))
		{
			Op = BooleanOp::GreaterEqual;
		}
		else if (Tok.is(Token::equal_equal))
		{
			Op = BooleanOp::Equal;
		}
		else if (Tok.is(Token::not_equal))
		{
			Op = BooleanOp::NotEqual;
		}
		else
		{
			cout<< "Boolean Value Expected";
			return nullptr;
		}

		advance();
		Expression* rhand = parseExpr();
		return new BooleanOp(Op, lhand, rhand);

	}
}


Base* Parser::parse()
{
	Base* Res = parseS();
	return Res;
}