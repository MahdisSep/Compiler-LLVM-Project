#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
    class DeclCheck : public ASTVisitor {
        llvm::StringSet<> Scope;
        llvm::StringSet<> Scope2; //boolean

        bool HasError;

        enum ErrorType { Twice, Not, DivByZero ,MathOp,DiffType };

          void error(ErrorType ET, llvm::StringRef V) {
            if (ET == ErrorType::DivByZero) {
                llvm::errs() << "Division by zero is not allowed." << "\n";
            }
            else if (ET == ErrorType::MathOp){
                llvm::errs() << "Boolean is not allowed in math operations." << "\n";
            }
            else if (ET == ErrorType::DiffType){
                llvm::errs() << "Assign different types of variables is not allowed." << "\n";
            }
            else
            {
                llvm::errs() << "Variable " << V << " is " << (ET == Twice ? "already" : "not") << " declared!\n";
            }
            HasError = true;
            exit(3);
        }
        
        //not --> expression , assignment , unery
        //twice --> define
        //divbyzero --> binary op , assignment
        //MathOp --> binaryOp , assignment
        //DiffType --> assignment

         public:
        DeclCheck() : HasError(false) {}

        bool hasError() { return HasError; }

        virtual void visit(Expression& Node) override {
            if (Node.getKind() == Expression::ExpressionType::Identifier) {
                if (Scope.find(Node.getValue()) == Scope.end() && Scope2.find(Node.getValue()) == Scope2.end())  
                    error(Not, Node.getValue());
            }
            else if (Node.getKind() == Expression::ExpressionType::BinaryOpType)
            {
                BinaryOp* declaration = (BinaryOp*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Expression::ExpressionType::BooleanOpType)
            {
                BooleanOp* temp = Node.getBooleanOp();

                (temp->getLeft())->accept(*this);

                (temp->getRight())->accept(*this);
            }
        };

        virtual void visit(BinaryOp& Node) override {
            if (Node.getLeft())
                Node.getLeft()->accept(*this);
            else
                HasError = true;
            if (Node.getRight())
                Node.getRight()->accept(*this);
            else
                HasError = true;
            if (!(Scope.count(Node.getLeft()->getValue()) && Scope.count(Node.getRight()->getValue()))) 
                error(MathOp, ((Expression*)Node.getLeft())->getValue());
                
            else if (Node.getOperator() == BinaryOp::Operator::Div || Node.getOperator() == BinaryOp::Operator::Mod)
            {
                Expression* right = (Expression*)Node.getRight();
                
            }
        };

    }
}

bool Sema::semantic(AST* Tree) {
    if (!Tree)
        return false;
    DeclCheck Check;
    Tree->accept(Check);
    return Check.hasError();
}
