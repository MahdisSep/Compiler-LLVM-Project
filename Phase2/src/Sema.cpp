#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"


namespace nms{
class InputCheck : public ASTVisitor {
  llvm::StringSet<> IntScope; // StringSet to store declared int variables
  llvm::StringSet<> BoolScope;
  bool HasError; // Flag to indicate if an error occurred

  enum ErrorType { Twice, Not }; // Enum to represent error types: Twice - variable declared twice, Not - variable not declared

  void error(ErrorType ET, llvm::StringRef V) {
    // Function to report errors
    llvm::errs() << "Variable " << V << " is "
                 << (ET == Twice ? "already" : "not")
                 << " declared\n";
    HasError = true; // Set error flag to true
  }

public:
  InputCheck() : HasError(false) {} // Constructor

  bool hasError() { return HasError; } // Function to check if an error occurred

  // Visit function for Program nodes
  virtual void visit(Program &Node) override { 

    for (llvm::SmallVector<AST *>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I)
    {
      (*I)->accept(*this); // Visit each child node
    }
  };

  virtual void visit(AST &Node) override {
    Node.accept(*this);
  }

  // Visit function for Final nodes
  virtual void visit(Final &Node) override {
    if (Node.getKind() == Final::Ident) {
      // Check if identifier is in the scope
      if (IntScope.find(Node.getVal()) == IntScope.end() && BoolScope.find(Node.getVal()) == BoolScope.end())
        error(Not, Node.getVal());
    }
  };

  // Visit function for BinaryOp nodes
  virtual void visit(BinaryOp &Node) override {
    Expr* right = Node.getRight();
    Expr* left = Node.getLeft();
    if (left)
      left->accept(*this);
    else
      HasError = true;

    if (right)
      right->accept(*this);
    else
      HasError = true;

    Final* l = (Final*)left;
    if (l->getKind() == Final::Ident){
      if (BoolScope.find(l->getVal()) != BoolScope.end()) {
        llvm::errs() << "Cannot use binary operation on a boolean variable: " << l->getVal() << "\n";
        HasError = true;
      }
    }

    Final* r = (Final*)right;
    if (r->getKind() == Final::Ident){
      if (BoolScope.find(r->getVal()) != BoolScope.end()) {
        llvm::errs() << "Cannot use binary operation on a boolean variable: " << r->getVal() << "\n";
        HasError = true;
      }
    }
    

    if (Node.getOperator() == BinaryOp::Operator::Div || Node.getOperator() == BinaryOp::Operator::Mod ) {
      Final* f = (Final*)right;

      if (f->getKind() == Final::ValueKind::Number) {
        llvm::StringRef intval = f->getVal();

        if (intval == "0") {
          llvm::errs() << "Division by zero is not allowed." << "\n";
          HasError = true;
        }
      }
    }
    
  };

  // Visit function for Assignment nodes
  virtual void visit(Assignment &Node) override {
    Final *dest = Node.getLeft();
    Expr *RightExpr;
    Logic *RightLogic;

    dest->accept(*this);

    if (dest->getKind() == Final::Number) {
        llvm::errs() << "Assignment destination must be an identifier, not a number.";
        HasError = true;
    }

    if (BoolScope.find(dest->getVal()) != BoolScope.end()) {
      RightLogic = Node.getRightLogic();
      if (RightLogic){
        RightLogic->accept(*this);
        if(Node.getAssignKind() != Assignment::AssignKind::Assign){
          llvm::errs() << "Cannot use mathematical operation on boolean variable: " << dest->getVal() << "\n";
          HasError = true;
        }
      }
      else{
        llvm::errs() << "you should assign a boolean value to boolean variable: " << dest->getVal() << "\n";
        HasError = true;
      }
    }
      
    else if (IntScope.find(dest->getVal()) != IntScope.end()){
      RightExpr = Node.getRightExpr();
      RightLogic = Node.getRightLogic();
      if (RightExpr){
        RightExpr->accept(*this);
      }
      else if(RightLogic){
        RightLogic->accept(*this);
        Comparison* RL = (Comparison*) RightLogic;
        if (RL){
          if (RL->getOperator() == Comparison::Ident){
            Final* F = (Final*)(RL->getLeft());
            if (IntScope.find(F->getVal()) == IntScope.end()) {
              llvm::errs() << "you should assign an integer value to an integer variable: " << dest->getVal() << "\n";
              HasError = true;
            } 
          }
          else{
            llvm::errs() << "you should assign an integer value to an integer variable: " << dest->getVal() << "\n";
            HasError = true;
          }
        }
        
      }
      else{
        llvm::errs() << "you should assign an integer value to an integer variable: " << dest->getVal() << "\n";
        HasError = true;
      }
        
    }
    
    
    if (Node.getAssignKind() == Assignment::AssignKind::Slash_assign) {

      Final* f = (Final*)(RightExpr);
      if (f)
      {
        if (f->getKind() == Final::ValueKind::Number) {
        llvm::StringRef intval = f->getVal();

        if (intval == "0") {
          llvm::errs() << "Division by zero is not allowed." << "\n";
          HasError = true;
        }
        }
      }
    }
  };

  virtual void visit(DeclarationInt &Node) override {
    for (llvm::SmallVector<Expr *>::const_iterator I = Node.valBegin(), E = Node.valEnd(); I != E; ++I){
      (*I)->accept(*this); // If the Declaration node has an expression, recursively visit the expression node
    }
    for (llvm::SmallVector<llvm::StringRef>::const_iterator I = Node.varBegin(), E = Node.varEnd(); I != E;
         ++I) {
      if(BoolScope.find(*I) != BoolScope.end()){
        llvm::errs() << "Variable " << *I << " is already declared as an boolean" << "\n";
        HasError = true; 
      }
      else{
        if (!IntScope.insert(*I).second)
          error(Twice, *I); // If the insertion fails (element already exists in Scope), report a "Twice" error
      }
    }
  };

  virtual void visit(DeclarationBool &Node) override {
    for (llvm::SmallVector<Logic *>::const_iterator I = Node.valBegin(), E = Node.valEnd(); I != E; ++I){
      (*I)->accept(*this); // If the Declaration node has an expression, recursively visit the expression node
    }
    for (llvm::SmallVector<llvm::StringRef>::const_iterator I = Node.varBegin(), E = Node.varEnd(); I != E;
         ++I) {
      if(IntScope.find(*I) != IntScope.end()){
        llvm::errs() << "Variable " << *I << " is already declared as an integer" << "\n";
        HasError = true; 
      }
      else{
        if (!BoolScope.insert(*I).second)
          error(Twice, *I); // If the insertion fails (element already exists in Scope), report a "Twice" error
      }
    }
    
  };

  virtual void visit(Comparison &Node) override {
    if(Node.getLeft()){
      Node.getLeft()->accept(*this);
    }
    if(Node.getRight()){
      Node.getRight()->accept(*this);
    }
    // else{
    //   if (Node.getOperator() == Comparison::Ident){
    //     Final* F = (Final*)(Node.getLeft());
    //     if (BoolScope.find(F->getVal()) == BoolScope.end()) {
    //       llvm::errs() << "you need a boolean varaible to compare or assign: "<< F->getVal() << "\n";
    //       HasError = true;
    //     } 
    //   }
    // }

    if (Node.getOperator() != Comparison::True && Node.getOperator() != Comparison::False && Node.getOperator() != Comparison::Ident){
      Final* L = (Final*)(Node.getLeft());
      if(L){
        if (L->getKind() == Final::ValueKind::Ident && IntScope.find(L->getVal()) == IntScope.end()) {
          llvm::errs() << "you can only compare a defined integer variable: "<< L->getVal() << "\n";
          HasError = true;
        } 
      }
      
      Final* R = (Final*)(Node.getRight());
      if(R){
        if (R->getKind() == Final::ValueKind::Ident && IntScope.find(R->getVal()) == IntScope.end()) {
          llvm::errs() << "you can only compare a defined integer variable: "<< R->getVal() << "\n";
          HasError = true;
        } 
      }
    }
  };

  virtual void visit(LogicalExpr &Node) override {
    if(Node.getLeft()){
      Node.getLeft()->accept(*this);
    }
    if(Node.getRight()){
      Node.getRight()->accept(*this);
    }
  };

  virtual void visit(UnaryOp &Node) override {
    if (IntScope.find(Node.getIdent()) == IntScope.end()){
      llvm::errs() << "Variable "<<Node.getIdent() << " is not a defined integer variable." << "\n";
      HasError = true;
    }
  };

  virtual void visit(NegExpr &Node) override {
    Expr *expr = Node.getExpr();
    (*expr).accept(*this);
  };

  virtual void visit(PrintStmt &Node) override {
    // Check if identifier is in the scope
    if (IntScope.find(Node.getVar()) == IntScope.end() && BoolScope.find(Node.getVar()) == BoolScope.end())
      error(Not, Node.getVar());
    
  };

  virtual void visit(IfStmt &Node) override {
    Logic *l = Node.getCond();
    (*l).accept(*this);

    for (llvm::SmallVector<AST *>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
    for (llvm::SmallVector<AST *>::const_iterator I = Node.beginElse(), E = Node.endElse(); I != E; ++I){
      (*I)->accept(*this);
    }
    for (llvm::SmallVector<elifStmt *>::const_iterator I = Node.beginElif(), E = Node.endElif(); I != E; ++I){
      (*I)->accept(*this);
    }
  };

  virtual void visit(elifStmt &Node) override {
    Logic* l = Node.getCond();
    (*l).accept(*this);

    for (llvm::SmallVector<AST *>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(WhileStmt &Node) override {
    Logic* l = Node.getCond();
    (*l).accept(*this);

    for (llvm::SmallVector<AST *>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(ForStmt &Node) override {
    Assignment *first = Node.getFirst();
    (*first).accept(*this);

    Logic *second = Node.getSecond();
    (*second).accept(*this);

    Assignment *assign = Node.getThirdAssign();
    if(assign)
      (*assign).accept(*this);
    else{
      UnaryOp *unary = Node.getThirdUnary();
      (*unary).accept(*this);
    }
      

    for (llvm::SmallVector<AST *>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(SignedNumber &Node) override {
  };

};
}

bool Sema::semantic(Program *Tree) {
  if (!Tree)
    return false; // If the input AST is not valid, return false indicating no errors
  nms::InputCheck *Check = new nms::InputCheck();;// Create an instance of the InputCheck class for semantic analysis
  Tree->accept(*Check); // Initiate the semantic analysis by traversing the AST using the accept function

  return Check->hasError(); // Return the result of Check.hasError() indicating if any errors were detected during the analysis
}
