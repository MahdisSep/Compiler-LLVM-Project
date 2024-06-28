#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

// Forward declarations of classes used in the AST
class AST;
class Expr;
class Program;
class DeclarationInt;
class DeclarationBool;
class Final;
class BinaryOp;
class UnaryOp;
class SignedNumber;
class NegExpr;
class Assignment;
class Logic;
class Comparison;
class LogicalExpr;
class IfStmt;
class WhileStmt;
class elifStmt;
class ForStmt;
class PrintStmt;

// ASTVisitor class defines a visitor pattern to traverse the AST
class ASTVisitor
{
public:
  // Virtual visit functions for each AST node type
  virtual void visit(AST &) {}               // Visit the base AST node
  virtual void visit(Expr &) {}              // Visit the expression node
  virtual void visit(Logic &) {}             // Visit the Logic node
  virtual void visit(Program &) {};          // Visit the group of expressions node
  virtual void visit(Final &) = 0;           // Visit the Final node
  virtual void visit(BinaryOp &) = 0;        // Visit the binary operation node
  virtual void visit(UnaryOp &) = 0;
  virtual void visit(SignedNumber &) = 0;
  virtual void visit(NegExpr &) = 0;
  virtual void visit(Assignment &) = 0;      // Visit the assignment expression node
  virtual void visit(DeclarationInt &) = 0;     // Visit the variable declaration node
  virtual void visit(DeclarationBool &) = 0;     // Visit the variable declaration node
  virtual void visit(Comparison &) = 0;      // Visit the Comparison node
  virtual void visit(LogicalExpr &) = 0;     // Visit the LogicalExpr node
  virtual void visit(IfStmt &) = 0;          // Visit the IfStmt node
  virtual void visit(WhileStmt &) = 0;        // Visit the IterStmt node
  virtual void visit(elifStmt &) = 0;        // Visit the elifStmt node
  virtual void visit(ForStmt &) = 0;
  virtual void visit(PrintStmt &) = 0;
};

// AST class serves as the base class for all AST nodes
class AST
{
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;    // Accept a visitor for traversal
};

// Expr class represents an expression in the AST
class Expr : public AST
{
public:
  Expr() {}
};

class Logic : public AST
{
public:
  Logic() {}
};

// Program class represents a group of expressions in the AST
class Program : public AST
{
  using dataVector = llvm::SmallVector<AST *>;

private:
  dataVector data;                          // Stores the list of expressions

public:
  Program(llvm::SmallVector<AST *> data) : data(data) {}
  Program() = default;

  llvm::SmallVector<AST *> getdata() { return data; }

  void setdata(llvm::SmallVector<AST *> Indata) { data = Indata; }

  dataVector::const_iterator begin() { return data.begin(); }

  dataVector::const_iterator end() { return data.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Declaration class represents a variable declaration with an initializer in the AST
class DeclarationInt : public Program
{
  using VarVector = llvm::SmallVector<llvm::StringRef>;
  using ValueVector = llvm::SmallVector<Expr *>;
  VarVector Vars;                           // Stores the list of variables
  ValueVector Values;                       // Stores the list of initializers

public:
  // Declaration(llvm::SmallVector<llvm::StringRef> Vars, Expr *E) : Vars(Vars), E(E) {}
  DeclarationInt(llvm::SmallVector<llvm::StringRef> Vars, llvm::SmallVector<Expr *> Values) : Vars(Vars), Values(Values) {}

  VarVector::const_iterator varBegin() { return Vars.begin(); }

  VarVector::const_iterator varEnd() { return Vars.end(); }

  ValueVector::const_iterator valBegin() { return Values.begin(); }

  ValueVector::const_iterator valEnd() { return Values.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Declaration class represents a variable declaration with an initializer in the AST
class DeclarationBool : public Program
{
  using VarVector = llvm::SmallVector<llvm::StringRef>;
  using ValueVector = llvm::SmallVector<Logic *>;
  VarVector Vars;                           // Stores the list of variables
  ValueVector Values;                       // Stores the list of initializers

public:
  // Declaration(llvm::SmallVector<llvm::StringRef> Vars, Expr *E) : Vars(Vars), E(E) {}
  DeclarationBool(llvm::SmallVector<llvm::StringRef> Vars, llvm::SmallVector<Logic *> Values) : Vars(Vars), Values(Values) {}

  VarVector::const_iterator varBegin() { return Vars.begin(); }

  VarVector::const_iterator varEnd() { return Vars.end(); }

  ValueVector::const_iterator valBegin() { return Values.begin(); }

  ValueVector::const_iterator valEnd() { return Values.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};


// Final class represents a Final in the AST (either an identifier or a number or true or false)
class Final : public Expr
{
public:
  enum ValueKind
  {
    Ident,
    Number
  };

private:
  ValueKind Kind;                            // Stores the kind of Final (identifier or number or true or false)
  llvm::StringRef Val;                       // Stores the value of the Final

public:
  Final(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val) {}

  ValueKind getKind() { return Kind; }

  llvm::StringRef getVal() { return Val; }
  void setVal(llvm::StringRef v){Val=v;}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// BinaryOp class represents a binary operation in the AST (plus, minus, multiplication, division)
class BinaryOp : public Expr
{
public:
  enum Operator
  {
    Plus,
    Minus,
    Mul,
    Div,
    Mod,
    Exp
  };

private:
  Expr *Left;                               // Left-hand side expression
  Expr *Right;                              // Right-hand side expression
  Operator Op;                              // Operator of the binary operation

public:
  BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R) {}

  Expr *getLeft() { return Left; }
  void setLeft(Expr * l) {Left = l;}

  Expr *getRight() { return Right; }
  void setRight(Expr * r) {Right = r;}

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// naryOp class represents a unary operation in the AST (plus plus, minus minus)
class UnaryOp : public Expr
{
public:
  enum Operator
  {
    Plus_plus,
    Minus_minus
  };

private:
  llvm::StringRef Ident;                      
  Operator Op;                              // Operator of the unary operation

public:
  UnaryOp(Operator Op, llvm::StringRef I) : Op(Op), Ident(I) {}

  llvm::StringRef getIdent() { return Ident; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class SignedNumber : public Expr
{
public:
  enum Sign
  {
    Plus,
    Minus
  };

private:
  llvm::StringRef Value;                              
  Sign s;                              

public:
  SignedNumber(Sign S, llvm::StringRef V) : s(S), Value(V) {}

  llvm::StringRef getValue() { return Value; }

  Sign getSign() { return s; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class NegExpr : public Expr
{

private:
  Expr *expr;                              

public:
  NegExpr(Expr *E) : expr(E) {}

  Expr *getExpr() { return expr; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Assignment class represents an assignment expression in the AST
class Assignment : public Program
{
  public:
  enum AssignKind
  {
    Assign,         // =
    Minus_assign,   // -=
    Plus_assign,    // +=
    Star_assign,    // *=
    Slash_assign,   // /=
};
private:
  Final *Left;                             // Left-hand side Final (identifier)
  Expr *RightExpr;                         // Right-hand side expression
  Logic *RightLogicExpr;                   // Right-hand side logical expression
  AssignKind AK;                           // Kind of assignment

public:
  Assignment(Final *L, Expr *RE, AssignKind AK, Logic *RL) : Left(L), RightExpr(RE), AK(AK), RightLogicExpr(RL) {}

  Final *getLeft() { return Left; }

  Expr *getRightExpr() { return RightExpr; }
  void setRightExpr(Expr * r) {RightExpr = r;}

  Logic *getRightLogic() { return RightLogicExpr; }

  AssignKind getAssignKind() { return AK; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Comparison class represents a comparison expression in the AST
class Comparison : public Logic
{
  public:
  enum Operator
  {
    Equal,          // ==
    Not_equal,      // !=
    Greater,        // >
    Less,           // <
    Greater_equal,  // >=
    Less_equal,     // <=
    True,           //CHECK???
    False,
    Ident           // only one boolean ident
  };
    
private:
  Expr *Left;                                // Left-hand side expression
  Expr *Right;                               // Right-hand side expression
  Operator Op;                               // Kind of assignment

public:
  Comparison(Expr *L, Expr *R, Operator Op) : Left(L), Right(R), Op(Op) {}

  Expr *getLeft() { return Left; }
  void setLeft(Expr* l){Left = l;}

  Expr *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// LogicalExpr class represents a logical expression in the AST
class LogicalExpr : public Logic
{
  public:
  enum Operator
  {
    And,          // &&
    Or,           // ||
  };

private:
  Logic *Left;                                // Left-hand side expression
  Logic *Right;                               // Right-hand side expression
  Operator Op;                                // Kind of assignment

public:
  LogicalExpr(Logic *L, Logic *R, Operator Op) : Left(L), Right(R), Op(Op) {}

  Logic *getLeft() { return Left; }

  Logic *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class elifStmt : public Program
{
  using Stmts = llvm::SmallVector<AST *>;

private:
  Stmts S;
  Logic *Cond;

public:
  elifStmt(Logic *Cond, llvm::SmallVector<AST *> S) : Cond(Cond), S(S) {}

  Logic *getCond() { return Cond; }

  Stmts::const_iterator begin() { return S.begin(); }

  Stmts::const_iterator end() { return S.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

};

class IfStmt : public Program
{
using BodyVector = llvm::SmallVector<AST *>;
using elifVector = llvm::SmallVector<elifStmt *>;

private:
  BodyVector ifStmts;
  elifVector elifStmts;
  BodyVector elseStmts;
  Logic *Cond;

public:
  IfStmt(Logic *Cond, llvm::SmallVector<AST *> ifStmts, llvm::SmallVector<AST *> elseStmts, llvm::SmallVector<elifStmt *> elifStmts) : Cond(Cond), ifStmts(ifStmts), elseStmts(elseStmts), elifStmts(elifStmts) {}

  Logic *getCond() { return Cond; }

  BodyVector::const_iterator begin() { return ifStmts.begin(); }

  BodyVector::const_iterator end() { return ifStmts.end(); }

  BodyVector::const_iterator beginElse() { return elseStmts.begin(); }

  BodyVector::const_iterator endElse() { return elseStmts.end(); }

  elifVector::const_iterator beginElif() { return elifStmts.begin(); }

  elifVector::const_iterator endElif() { return elifStmts.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class WhileStmt : public Program
{
using BodyVector = llvm::SmallVector<AST *>;
BodyVector Body;

private:
  Logic *Cond;

public:
  WhileStmt(Logic *Cond, llvm::SmallVector<AST *> Body) : Cond(Cond), Body(Body) {}

  Logic *getCond() { return Cond; }
  void setCond(Logic * c){Cond=c;}

  llvm::SmallVector<AST *> getBody(){return Body;}
  void setBody(llvm::SmallVector<AST *> b){Body=b;}

  BodyVector::const_iterator begin() { return Body.begin(); }

  BodyVector::const_iterator end() { return Body.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};


class ForStmt : public Program
{
using BodyVector = llvm::SmallVector<AST *>;
BodyVector Body;

private:
  Assignment *First;
  Logic *Second;
  Assignment *ThirdAssign;
  UnaryOp *ThirdUnary;


public:
  ForStmt(Assignment *First, Logic *Second, Assignment *ThirdAssign, UnaryOp* ThirdUnary, llvm::SmallVector<AST *> Body) : First(First), Second(Second), ThirdAssign(ThirdAssign), ThirdUnary(ThirdUnary), Body(Body) {}

  Assignment *getFirst() { return First; }
  void setFirst(Assignment *f){First = f;}

  Logic *getSecond() { return Second; }
  void setSecond(Logic *s){Second = s;}

  Assignment *getThirdAssign() { return ThirdAssign; }
  void setThirdAssign(Assignment *t){ThirdAssign = t;}

  UnaryOp *getThirdUnary() { return ThirdUnary; }
  void setThirdUnary(UnaryOp *u){ThirdUnary = u;}

  llvm::SmallVector<AST *> getBody(){return Body;}
  void setBody(llvm::SmallVector<AST *> b){Body = b;}

  BodyVector::const_iterator begin() { return Body.begin(); }

  BodyVector::const_iterator end() { return Body.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class PrintStmt : public Program
{
private:
  llvm::StringRef Var;
  
public:
  PrintStmt(llvm::StringRef Var) : Var(Var) {}

  llvm::StringRef getVar() { return Var; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

#endif
