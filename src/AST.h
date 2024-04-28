#ifndef AST_H
#define AST_H

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

class AST; // Abstract Syntax Tree
class Expression; // top level expression that is evaluated to boolean, int or variable name at last
class Base; // top level program
class Statement; // top level statement
class BinaryOp; // binary operation of numbers and identifiers
class AssignStatement; // assignment statement like a = 3;
class DefInt; // declaration statement like int a;
class DefBool;// declaration statement like bool a;
class BooleanOp; // boolean operation like 3 > 6*2;
class UneryOp;
class ForStatement;
class WhileStatement;
class IfStatement;
class ElseIfStatement;
class ElseStatement;
class Print;

class ASTVisitor
{
public:
	// Virtual visit functions for each AST node type
	virtual void visit(AST&) {}
	virtual void visit(Expression&) {}
	virtual void visit(Base&) = 0;
	virtual void visit(Statement&) = 0;
	virtual void visit(BinaryOp&) = 0;
	virtual void visit(UneryOp&) = 0;
	virtual void visit(DefInt&) = 0;
	virtual void visit(DefBool&) = 0;
	virtual void visit(AssignStatement&) = 0;
	virtual void visit(BooleanOp&) = 0;
	virtual void visit(IfStatement&) = 0;
	virtual void visit(ElseIfStatement&) = 0;
	virtual void visit(ElseStatement&) = 0;
	virtual void visit(ForStatement&) = 0;
	virtual void visit(WhileStatement&) = 0;
	virtual void visit(Print&) = 0;           // =0 ?
};


class AST {
public:
	virtual ~AST() {}
	virtual void accept(ASTVisitor& V) = 0;
};
// base Node that contains all the syntax nodes
class Base : public AST {
private:
	llvm::SmallVector<Statement*> statements;                          // Stores the list of expressions

public:
	Base(llvm::SmallVector<Statement*> Statements) : statements(Statements) {}
	llvm::SmallVector<Statement*> getStatements() { return statements; }

	llvm::SmallVector<Statement*>::const_iterator begin() { return statements.begin(); }

	llvm::SmallVector<Statement*>::const_iterator end() { return statements.end(); }
	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}

};
class TopLevelEntity : AST {
public:
	TopLevelEntity() {}
};


// Value Expression class that holds information about
// numbers or variable names. forexample a or 56 are 
// ValExpression and become part of the syntax tree
class Expression : public TopLevelEntity {
public:
	enum ExpressionType {
		Number,
		Identifier,
		Boolean,
		BinaryOpType,
		BooleanOpType
	};
private:
	ExpressionType Type; // can be number of variable name

	// if it holds a number NumberVal is used else Value is
	// used to store variable name
	llvm::StringRef Value;
	int NumberVal;
	bool BoolVal;
	bool Sign; // true -> +
	BooleanOp* BOVal;

public:
	Expression() {}
	Expression(llvm::StringRef value) : Type(ExpressionType::Identifier), Value(value) {} // store string
	Expression(int value) : Type(ExpressionType::Number), NumberVal(value) {} // store number
	Expression(bool value) : Type(ExpressionType::Boolean), BoolVal(value) {} // store boolean -->true,false
	Expression(BooleanOp* value) : Type(ExpressionType::BooleanOpType), BOVal(value) {} // store boolean
	Expression(ExpressionType type) : Type(type) {}
	Expression(bool sign, int value) : Sign(sign), NumberVal(value),Type(ExpressionType::Number) {}
	Expression(bool sign, ExpressionType type) : Sign(sign), Type(type) {} 

	bool isNumber() {
		if (Type == ExpressionType::Number)
			return true;
		return false;
	}

	bool isBoolean() {
		if (Type == ExpressionType::Boolean)
			return true;
		return false;
	}

	bool isVariable() {
		if (Type == ExpressionType::Identifier)
			return true;
		return false;
	}

	bool getSign(){
		return Sign;
	}

	llvm::StringRef getValue() {
		return Value;
	}

	int getNumber() {
		return NumberVal;
	}

	BooleanOp* getBooleanOp() {
		return BOVal;
	}

	bool getBoolean() {
		return BoolVal;
	}
	// returns the kind of expression. can be identifier,
	// number, or an operation
	ExpressionType getKind()
	{
		return Type;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
	class Statement : public TopLevelEntity {
public:
	enum StateMentType {
		DeclarationInt,
		DeclarationBool,
		Assignment,
		If,
		ElseIf,
		Else,
		While,
		For
	};

private:
	StateMentType Type;

public:

	StateMentType getKind()
	{
		return Type;
	}

	Statement(StateMentType type) : Type(type) {}
	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};


class WhileStatement : public Statement {

private:
	Expression* Condition;
	llvm::SmallVector<Statement*> Statements;

public:
	WhileStatement(Expression* condition, llvm::SmallVector<Statement*> statements, StateMentType type) : Condition(condition), Statements(statements), Statement(type) { }

	Expression* getCondition()
	{
		return Condition;
	}

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

class ForStatement : public Statement {

private:
	AssignStatement* Assign1;
	Expression* Condition;
    AssignStatement* Assign2;
	llvm::SmallVector<Statement*> Statements;

public:
	ForStatement(AssignStatement* a1,Expression* condition,AssignStatement* a2, llvm::SmallVector<Statement*> statements, StateMentType type) : Assign1(a1), Condition(condition), Assign1(a1), Statements(statements), Statement(type) { }

	AssignStatement* getFirstAssign()
	{
		return Assign1;
	}

	Expression* getCondition()
	{
		return Condition;
	}

	AssignStatement* getSecondAssign()
	{
		return Assign2;
	}

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};
class IfStatement : public Statement {

private:
	Expression* Condition;
	llvm::SmallVector<Statement*> Statements;
	llvm::SmallVector<ElseIfStatement*> ElseIfsStatements;
	ElseStatement* ElseStatements;
	bool HasElseIf;
	bool HasElse;

public:
	IfStatement(Expression* condition,
	 llvm::SmallVector<Statement*> statements,
	 llvm::SmallVector<ElseIfStatement*> elseIfsStatements,
	 ElseStatement* elseStatement,
	 bool hasElseIf, bool hasElse,
	 StateMentType type): Condition(condition),
	 Statements(statements),
	 ElseIfsStatements(elseIfsStatements),
	 ElseStatements(elseStatement),
	 HasElseIf(hasElseIf),
	 HasElse(hasElse),
	 Statement(type) { }

	Expression* getCondition()
	{
		return Condition;
	}

	bool hasElseIf(){
		return HasElseIf;
	}

	bool hasElse(){
		return HasElse;
	}

	llvm::SmallVector<ElseIfStatement*> getElseIfsStatements()
	{
		return elseIfsStatements;
	}

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	ElseStatement* getElseStatement()
	{
		return ElseStatements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

class ElseIfStatement : public Statement {

private:
	Expression* Condition;
	llvm::SmallVector<Statement*> Statements;

public:
	ElseIfStatement(Expression* condition, llvm::SmallVector<Statement*> statements, StateMentType type): 
	tion(condition), Statements(statements), Statement(type) { }

	Expression* getCondition()
	{
		return Condition;
	}

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};
class ElseStatement : public Statement {

private:
	llvm::SmallVector<Statement*> Statements;

public:
	ElseStatement(llvm::SmallVector<Statement*> statements, StateMentType type): Statements(statements), Statement(type) { }

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

/*
	Declaration statement like int x; or int a = 3;
*/
class DefInt : public Statement {
private:

	Expression* lvalue;
	Expression* rvalue;
	Statement::StateMentType type;

public:
	DefInt(Expression* lvalue, Expression* rvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::DeclarationInt), Statement(type) { }
	DefInt(Expression* lvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::DeclarationInt), Statement(type) { rvalue = new Expression(0); }

	Expression* getLValue() {
		return lvalue;
	}

	Expression* getRValue() {
		return rvalue;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};
class DefBool : public Statement {
private:

	Expression* lvalue;
	Expression* rvalue;
	Statement::StateMentType type;

public:
	DefBool(Expression* lvalue, Expression* rvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::DeclarationBool), Statement(type) { }
	DefBool(Expression* lvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::DeclarationBool), Statement(type) { rvalue = new Expression(0); }

	Expression* getLValue() {
		return lvalue;
	}

	Expression* getRValue() {
		return rvalue;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};



	
};







#endif