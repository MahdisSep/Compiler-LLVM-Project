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




Base* Parser::parse()
{
	Base* Res = parseS();
	return Res;
}