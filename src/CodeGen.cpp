#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"


using namespace llvm;

// Define a visitor class for generating LLVM IR from the AST.
namespace
{
    class ToIRVisitor : public ASTVisitor
    {
        Module* M;
        IRBuilder<> Builder;
        Type* VoidTy;
        Type* Int32Ty;
        Type* Int8PtrTy;
        Type* Int8PtrPtrTy;
        Constant* Int32Zero;
        

        Value* V;
        StringMap<AllocaInst*> nameMap;

        FunctionType* MainFty;
        Function* MainFn;
        FunctionType *CalcWriteFnTy;
        Function *CalcWriteFn;
    public:
        // Constructor for the visitor class.
        ToIRVisitor(Module* M) : M(M), Builder(M->getContext())
        {
            // Initialize LLVM types and constants.
            VoidTy = Type::getVoidTy(M->getContext());
            Int32Ty = Type::getInt32Ty(M->getContext());
            Int8PtrTy = Type::getInt8PtrTy(M->getContext());
            Int8PtrPtrTy = Int8PtrTy->getPointerTo();
            Int32Zero = ConstantInt::get(Int32Ty, 0, true);
            CalcWriteFn = Function::Create(CalcWriteFnTy, GlobalValue::ExternalLinkage, "print", M);
        }
        void run(AST* Tree)
        {
            // Create the main function with the appropriate function type.
            MainFty = FunctionType::get(Int32Ty, { Int32Ty, Int8PtrPtrTy }, false);
            MainFn = Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);

            // Create a basic block for the entry point of the main function.
            BasicBlock* BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
            Builder.SetInsertPoint(BB);

            // Visit the root node of the AST to generate IR.
            Tree->accept(*this);

            // Create a return instruction at the end of the main function.
            Builder.CreateRet(Int32Zero);
        }
        virtual void visit(Base& Node) override
        {
            for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
            {
                (*I)->accept(*this);
            }
        }
        virtual void visit(Statement& Node) override
        {
             if (Node.getKind() == Statement::StateMentType::DeclarationInt) 
            {
                DefInt* declaration = (DefInt*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::DeclarationBool)
            {
                DefBool* declaration = (DefBool*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::Assignment)
            {
                AssignStatement* declaration = (AssignStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::If)
            {
                IfStatement* declaration = (IfStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::ElseIf)
            {
                ElseIfStatement* declaration = (ElseIfStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::Else)
            {
                ElseStatement* declaration = (ElseStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::While)
            {
                WhileStatement* declaration = (WhileStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::For)
            {
                ForStatement* declaration = (ForStatement*)&Node;
                declaration->accept(*this);
            }
            
            
        }

        virtual void visit(Expression& Node) override
        {
            if (Node.getKind() == Expression::ExpressionType::Identifier)
            {
                V = Builder.CreateLoad(Int32Ty, nameMap[Node.getValue()]);
            }
            else if (Node.getKind() == Expression::ExpressionType::Number)
            {
               
                int intval = Node.getNumber();
                V = ConstantInt::get(Int32Ty, intval, true);
            }
            else if (Node.getKind() == Expression::ExpressionType::Boolean)
            {
               
                bool boolVal = Node.getBoolean();
                V = ConstantInt::get(Int32Ty, boolVal, true);                              //boolean
            }
            else if (Node.getKind() == Expression::ExpressionType::BooleanOpType) {
                BooleanOp* temp = Node.getBooleanOp();
                if (temp->getOperator() == BooleanOp::Operator::And)
                {
                    (temp->getLeft())->accept(*this);
                    Value* Left = V;

                    (temp->getRight())->accept(*this);
                    Value* Right = V;

                    V = Builder.CreateAnd(Left, Right);
                }
                else if (temp->getOperator() == BooleanOp::Operator::Or)
                {
                    (temp->getLeft())->accept(*this);
                    Value* Left = V;

                    (temp->getRight())->accept(*this);
                    Value* Right = V;

                    V = Builder.CreateOr(Left, Right);
                }
            }
        }

        virtual void visit(BooleanOp& Node) override
        {
            // Visit the left-hand side of the binary operation and get its value.
            Node.getLeft()->accept(*this);
            Value* Left = V;

            // Visit the right-hand side of the binary operation and get its value.
            Node.getRight()->accept(*this);
            Value* Right = V;

            // Perform the boolean operation based on the operator type and create the corresponding instruction.
            switch (Node.getOperator())
            {
            case BooleanOp::Equal:
                V = Builder.CreateICmpEQ(Left, Right);
                break;
            case BooleanOp::NotEqual:
                V = Builder.CreateICmpNE(Left, Right);
                break;
            case BooleanOp::Less:
                V = Builder.CreateICmpSLT(Left, Right);
                break;
            case BooleanOp::LessEqual:
                V = Builder.CreateICmpSLE(Left, Right);
                break;
            case BooleanOp::Greater:
                V = Builder.CreateICmpSGT(Left, Right);
                break;
            case BooleanOp::GreaterEqual:
                V = Builder.CreateICmpSGE(Left, Right);
                break;
            case BooleanOp::And:
                V = Builder.CreateAnd(Left, Right);
                break;
            case BooleanOp::Or:
                V = Builder.CreateOr(Left, Right);
                break;
            }
        }
        virtual void visit(BinaryOp& Node) override
        {
            // Visit the left-hand side of the binary operation and get its value.
            Node.getLeft()->accept(*this);
            Value* Left = V;

            // Visit the right-hand side of the binary operation and get its value.
            Node.getRight()->accept(*this);
            Value* Right = V;

            // Perform the binary operation based on the operator type and create the corresponding instruction.
            switch (Node.getOperator())
            {
            case BinaryOp::Plus:
                V = Builder.CreateNSWAdd(Left, Right);
                break;
            case BinaryOp::Minus:
                V = Builder.CreateNSWSub(Left, Right);
                break;
            case BinaryOp::Mul:
                V = Builder.CreateNSWMul(Left, Right);
                break;
            case BinaryOp::Div:
                V = Builder.CreateSDiv(Left, Right);
                break;
            case BinaryOp::Pow:
                if ((Node.getRight())->isNumber())
                {
                    int power = (Node.getRight())->getNumber();
                    Value* result = Left;
                    for (int i=1; i<power; i++)
                    {
                        result = Builder.CreateNSWMul(result, Left);
                    }
                    V = result;
                }
                break;
            case BinaryOp::Mod:
                Value* division = Builder.CreateSDiv(Left, Right);
                Value* multiplication = Builder.CreateNSWMul(division, Right);
                V = Builder.CreateNSWSub(Left, multiplication);
            }
        }
          virtual void visit(UneryOp& Node) override
        {
            // Visit the left-hand side of the unary operation and get its value.
            Node.getLeft()->accept(*this);
            Value* Left = V;


            // Perform the unary operation based on the operator type and create the corresponding instruction.
            switch (Node.getOperator())
            {
            case UneryOp::Plus:
                V = Builder.CreateNSWAdd(Left, 1);
                break;
            case UneryOp::Minus:
                V = Builder.CreateNSWSub(Left, 1);
                break;

            }
        }

    };
}; // namespace

