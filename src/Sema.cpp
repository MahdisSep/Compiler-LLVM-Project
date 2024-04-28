#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
    class DeclCheck : public ASTVisitor {
        llvm::StringSet<> Scope;
        llvm::StringSet<> Scope2; //boolean

        bool HasError;

        enum ErrorType { Twice, Not, DivByZero ,MathOp,DiffType };

        
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
