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




#endif