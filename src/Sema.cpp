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

    }
}

bool Sema::semantic(AST* Tree) {
    if (!Tree)
        return false;
    DeclCheck Check;
    Tree->accept(Check);
    return Check.hasError();
}
