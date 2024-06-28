#include "AST.h"
#include "Lexer.h"
#include <iostream>
#include <string>
#include <cstdio>
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"


class OptVisitor : public ASTVisitor {

    bool completeUnrolling;
    bool isFor,isWhile,assignInFirst,logicInSecond,assignInThird,checkForBody;
    bool checkWhileCond,checkWhileBody,findItrStep,notInc;
    int k,itrBegin,itrEnd,itrStep,nThItr;
    Logic* condition;
    llvm::SmallVector<AST *> primaryBody;

    public:
    OptVisitor(){
        completeUnrolling = true;
        isFor = false;
        isWhile = false;
        assignInFirst = false;
        assignInThird = false;
        logicInSecond = false;
        checkForBody = false;
        checkWhileBody = false;
        checkWhileCond = false;
        findItrStep = false;
        notInc = true; 
        k = 4;
        nThItr=0;
    }

               
    virtual void visit(Logic &Node) {}             // Visit the Logic node
    virtual void visit(SignedNumber &Node) {}
    virtual void visit(NegExpr &Node) {}
    virtual void visit(DeclarationInt &Node) {}     // Visit the variable declaration node
    virtual void visit(DeclarationBool &Node) {}   // Visit the variable declaration node
    virtual void visit(LogicalExpr &Node) {}       // Visit the LogicalExpr node
    virtual void visit(IfStmt &Node) {}            // Visit the IfStmt node
    virtual void visit(elifStmt &Node) {}          // Visit the elifStmt node
    virtual void visit(PrintStmt &Node) {}
    virtual void visit(Expr &Node) {} 
     

    int strToInt(llvm::StringRef strNum){
        std::string str = (std::string)strNum;
        int num = std::stoi(str);
        return num;
    }

    char* intToStr(int number){
        char* str = new char[20];
        int numDigits = 0;
        int tempNumber = number;
        if (number == 0){
            str[0] = '0';
            str[1] = '\0';
            return str;
        }
        while (tempNumber != 0){
            tempNumber /= 10;
            numDigits++;
        }
        for (int i = numDigits - 1; i >= 0; i--){
            int digit = number % 10;
            str[i] = '0' + digit;
            number /= 10;
        }

        str[numDigits] = '\0'; 
        return str;
    }

    int itrnumber(){ 
        if(isFor){
            return (itrEnd - itrBegin) / itrStep;
        }
        else{
            return itrEnd / itrStep;
        }
    }

    virtual void visit(Program &Node){
        auto v = Node.getdata();
        auto end = v.end();
        auto begin = v.begin();

        while(end != begin){
            isFor = false;
            isWhile = false;
            nThItr=0;
            
            (*begin)->accept(*this);

            if(isFor && !completeUnrolling){
                //llvm::errs() <<"while\n";
                Assignment::AssignKind AK = Assignment::Plus_assign;
                Final* F = new Final(Final::Ident, "i"); // name of itr
                Expr* E = new Final(Final::Number, intToStr(itrStep)); // step of itr
                Assignment* inc = new Assignment(F, E, AK, nullptr);
                primaryBody.push_back(inc);

                Comparison::Operator op = Comparison::Less;
                Final* lc = new Final(Final::Ident, "i");
                Expr* rc = new Final(Final::Number, intToStr(itrEnd));
                Logic* comp = new Comparison(lc,rc,op);

                WhileStmt* wh = new WhileStmt(comp, primaryBody);
                //v.push_back(wh); 
                v.insert(begin+1,wh);
                begin++;
            }
            if(isWhile && !completeUnrolling){
                Comparison::Operator op = Comparison::Less;
                Final* lc = new Final(Final::Ident, "i");
                Expr* rc = new Final(Final::Number, intToStr(itrEnd));
                Logic* comp = new Comparison(lc,rc,op);

                WhileStmt* wh = new WhileStmt(comp, primaryBody);
                //v.push_back(wh); 
                v.insert(begin+1,wh);
                begin++;
            }
            begin++;
        }
        Node.setdata(v);
    }

    virtual void visit(ForStmt &Node) { 
        isFor = true;
        condition = Node.getSecond();
        primaryBody = Node.getBody();
        
        assignInFirst = true;
        Node.getFirst()->accept(*this);
        assignInFirst = false;

        if (Node.getThirdAssign() == nullptr)
            itrStep = 1;
        else{
            assignInThird = true;
            Node.getThirdAssign()->accept(*this);
            assignInThird = false;
        }

        logicInSecond = true;
        Logic* second = Node.getSecond();
        Comparison* com = (Comparison*)second;
        com->accept(*this);
        logicInSecond = false;

        auto v = Node.getBody();
        auto end = v.end();
        auto begin = v.begin();
        auto begin2 = v.begin();

        llvm::SmallVector<AST *> v2;
        v2 = Node.getBody();
        
        checkForBody = true;
        if(completeUnrolling){
            for(int i=0; i<itrnumber()-1;i++){
                v2 = Node.getBody();
                begin = v2.begin();
                end = v2.end();
                // inc
                llvm::errs() << itrStep <<"\n" ;
                Assignment::AssignKind AK = Assignment::Plus_assign;
                Final* F = new Final(Final::Ident, "i"); // name of itr
                Expr* E = new Final(Final::Number, intToStr(itrStep)); // step of itr
                Assignment* inc = new Assignment(F, E, AK, nullptr);
                v.push_back(inc);

                while(end != begin){
                    (*begin)->accept(*this);
                    v.push_back(*begin);
                    begin++;
                }
                nThItr++;
            }
            Node.setBody(v);
        } 
        else{ // uncomplete
            llvm::errs() <<"uncomplete for\n";
            v = Node.getBody();
            end = v.end();
            begin = v.begin();
            auto begin2 = v.begin();

            for(int i=0; i<k-1;i++){  // -1 nemikhad -> ..=i+...
                begin = begin2;
                // inc
                llvm::errs() << itrStep <<"\n" ;
                Assignment::AssignKind AK = Assignment::Plus_assign;
                Final* F = new Final(Final::Ident, "i"); // name of itr
                Expr* E = new Final(Final::Number, intToStr(itrStep)); // step of itr
                Assignment* inc = new Assignment(F, E, AK, nullptr);
                v.push_back(inc);

                while(end != begin){
                    (*begin)->accept(*this);
                    v.push_back(*begin);
                    begin++;
                }
                
                nThItr++;
            }
            Node.setBody(v);

        }
        checkForBody = false;      
    }
    
    virtual void visit(Comparison &Node) {
        if(logicInSecond || checkWhileCond){
            Expr* right = Node.getRight();
            Final* r = (Final*)right;
            r->accept(*this);
        }
        if(!completeUnrolling && (logicInSecond || checkWhileCond)){
            BinaryOp::Operator Op = BinaryOp::Operator::Plus;
            Expr* leftBOp = Node.getLeft();
            Expr* rightBOp = new Final(Final::Number, intToStr(itrStep*k -1));
            Expr* newLeft = new BinaryOp(Op, leftBOp, rightBOp);
            Node.setLeft(newLeft);      
        }


    } 

    virtual void visit(Assignment &Node) {
        if(assignInFirst || assignInThird){
            Expr* right = Node.getRightExpr();
            Final* r = (Final*)right;
            r->accept(*this);
        }
        if(findItrStep){
            llvm::errs() << "find step\n" ;
            Final* left = Node.getLeft();
            Expr* right = Node.getRightExpr();
            if(left->getVal() == "i" && Node.getAssignKind() == Assignment::Plus_assign){
               Final* r = (Final*)right; 
               itrStep = strToInt(r->getVal());    
            }  
        }
        Final* left = Node.getLeft();
        if((left->getVal() == "i" && Node.getAssignKind() == Assignment::Plus_assign)){
            notInc = false;
        }  
    }

    virtual void visit(BinaryOp &Node) {
        if(completeUnrolling && isFor && checkForBody){
            //llvm::errs() << "im here: "<< nThItr <<"\n" ;
            Expr* left = Node.getLeft();
            Expr* right = Node.getRight();

            //left->accept(*this);
            //right->accept(*this);

            Final* l = (Final*)left;
            if (l->getKind() == Final::Ident && l->getVal() == "i"){
                llvm::errs() << "i update " << nThItr*itrStep << "\n";
                BinaryOp::Operator Op = BinaryOp::Operator::Plus;
                Expr* leftBOp = left;
                Expr* rightBOp = new Final(Final::Number, intToStr(nThItr*itrStep));
                Expr* newLeft = new BinaryOp(Op, leftBOp, rightBOp);
                //Node.setLeft(newLeft);
                // Final* F = new Final(Final::Number, intToStr(nThItr*itrStep));
                // Node.setLeft(F);
            }

            // Final* r = (Final*)right;
            // if (r->getKind() == Final::Ident && r->getVal() == "i"){
            //     llvm::errs() << "i update " << nThItr << " " << itrStep << "\n";
            //     BinaryOp::Operator Op = BinaryOp::Operator::Plus;
            //     Expr* leftBOp = right;
            //     Expr* rightBOp = new Final(Final::Number, intToStr(nThItr*itrStep));
            //     Expr* newRight = new BinaryOp(Op, leftBOp, rightBOp);
                //Node.setRight(newRight);
            }
        }
    }
    
    virtual void visit(Final &Node) {
        //llvm::errs() << "final"<<"\n";
        if(assignInFirst){
            llvm::StringRef strNum = Node.getVal();
            itrBegin = strToInt(strNum);
            //Node.setVal("5");
        }
        if(logicInSecond){
            llvm::StringRef strNum = Node.getVal();
            itrEnd = strToInt(strNum);
            if(completeUnrolling && isFor)
                Node.setVal("1");  
                   
        }
        if(assignInThird){
            llvm::StringRef strNum = Node.getVal();
            itrStep = strToInt(strNum);
            //if(!completeUnrolling)
                //Node.setVal(intToStr(k*itrStep));
        }

        if(checkWhileCond){
            llvm::StringRef strNum = Node.getVal();
            itrEnd = strToInt(strNum);
            if(completeUnrolling && isWhile)
                Node.setVal("1");

        }
    }

    virtual void visit(UnaryOp &Node) {
        if(findItrStep && Node.getIdent() == "i"){
            llvm::errs() << "unary\n" ;
            itrStep = 1;
        }
        if(checkWhileBody && Node.getIdent() == "i"){
            llvm::errs() << "unary2\n" ;
            notInc = false;
        }
    }
     
    virtual void visit(WhileStmt &Node) { 
        isWhile = true; 
        condition = Node.getCond();
        primaryBody = Node.getBody();
    
        

        auto v = Node.getBody();
        auto begin = v.begin();
        auto begin2 = v.begin();
        auto end = v.end();

        findItrStep = true;
        while(end != begin){
            (*begin)->accept(*this);
            v.push_back(*begin);
            begin++;
        }
        findItrStep = false;

        checkWhileCond = true;
        Logic* cond =  Node.getCond();
        Comparison* com = (Comparison*) cond;
        com->accept(*this);
        checkWhileCond = false;

        v = Node.getBody();
        begin = v.begin();
        begin2 = v.begin();
        end = v.end();

        checkWhileBody = true;
        if(completeUnrolling){
            for(int i=0; i<itrnumber()-1;i++){
                begin = begin2;

                while(end != begin){
                    (*begin)->accept(*this);
                    v.push_back(*begin);
                    begin++;
                }
                
                nThItr++;
            }
            Node.setBody(v);
        }
        else{ // uncomlete
            v = Node.getBody();
            end = v.end();
            begin = v.begin();
            auto begin2 = v.begin();

            for(int i=0; i<k-1;i++){  
                begin = begin2;

                while(end != begin){
                    (*begin)->accept(*this);
                    v.push_back(*begin);
                    begin++;
                }
                nThItr++;
            }
            Node.setBody(v);
        }
        checkWhileBody = false;
    }      
};

class Optimization{
    public:
    void Optimize(AST *Tree) {
        OptVisitor Op;
        Tree->accept(Op);
    }
};