//
//  SemanticAnalyzer.cpp
//  ECE467 Lab 3
//
//  Created by Tarek Abdelrahman on 2023-09-13.
//  Based on code written by Ao (Dino) Li for ECE467.
//  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
//
//  Permission is hereby granted to use this code in ECE467 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.

#include <iostream>
#include <cassert>
using namespace std;

#include "SemanticAnalyzer.h"
#include "SymTable.h"
using namespace smallc;

namespace smallc {

SemaError::SemaError (ErrorEnum code_, std::pair<unsigned int, unsigned int> location_) : code(code_), location(location_), msg() { }

SemaError::SemaError(ErrorEnum code_, std::pair<unsigned int, unsigned int> location_, std::string msg_) : code(code_), location(location_), msg(msg_) { }

// Constructor
SemanticAnalyzer::SemanticAnalyzer (): ASTVisitorBase(), prog(nullptr), errors() { }

// Print all the error messages at once
void
SemanticAnalyzer::printErrorMsgs () {
    for (const auto& e : errors) {
        std::cout << "sema: " << e.location.first
                  << ":" << e.location.second << " : ";
        switch (e.code) {
            case SemaError::IdentReDefined:
                std::cout << "redefinition of";
                break;
            case SemaError::IdentUnDefined:
                std::cout << "use of undefined identifier";
                break;
            case SemaError::NoMatchingDef:
                std::cout << "no matching definition for";
                break;
            case SemaError::MisMatchedReturn:
                std::cout << "mismatched return statement";
                break;
            case SemaError::InconsistentDef:
                std::cout << "definition inconsistent with earlier definition of";
                break;
            case SemaError::InvalidCond:
                std::cout << "invalid condition in";
                break;
            case SemaError::TypeMisMatch:
                std::cout << "type mismatch";
                break;
            case SemaError::InvalidArraySize:
                std::cout << "size cannot be negative for array";
                break;
            case SemaError::InvalidAccess:
                std::cout << "invalid use of identifier";
                break;
            default:
                std::cout << "unknown error number";
                break;
        }
        std::cout << " " << e.msg << std::endl;
    }
}

// Add an error to the list of errors
void
SemanticAnalyzer::addError (const SemaError& err) {
    errors.push_back(err);
}

// ECE467 STUDENT: implement the visitor classes, perfoming
// Error 0 (IdentReDefined): 
    // 1. No two variables have the same name in the same scope. (ident: variable name)
    // 2. No two functions have the same name, return value, parameters and types. (ident: function name)

// Error 1 (IdentUnDefined): 
    // 1. A variable (ident: variable name)
    // 2. A function is used before it is first declared in a visible scope. (ident: function name)

// Error 2 (NoMatchingDef): A function call must have the same number/types of arguments as its definition. (ident: function name)

// Error 3 (MisMatchedReturn): 
    // 1. A function's return statement expression must match the return type of the function. 
    // 2. A function with return type "void" must not have an expression in its return statement. It's acceptable to have no return statement as well. 
    // 3. A function with return type "non-void" must have an expression in its return statement.

// Error 4 (InconsistentDef): (ident: function name)
    // 1. A function definition must match one of its prototypes if the prototype is procided. Match means return types and the number/types of arguments must match. 
    // 2. Multiple identical prototype declarations of a function are allowed. 

// Error 5 (InvalidCond): 
    // 1. The condition of "if" statement must be of the boolean type. 
    // 2. The condition of "while" statement must be of the boolean type.

// Error 6 (TypeMisMatch): 
    // 1. Operands of arithmetic operators must of the integer type.
    // 2. Operands of logical operators must of the same type, either integer or boolen. 
    // 3. The type of the left hand side variable of an assignment statement must be of the same as the type of the expression of the right hand side. 

// Error 7 (InvalidArraySize): Array sizes in array declarations must be positive intergers. 

// Error 8 (InvalidAccess): 
    // 1. Scalar variables must not be used as arrays 
    // 2. Array variables must not be used as scalars

void 
SemanticAnalyzer::visitASTNode(ASTNode *node){
    ASTVisitorBase::visitASTNode(node);
}

void
SemanticAnalyzer::visitProgramNode(ProgramNode *prg){
    this->prog = prg;
    if(this->prog->useIo()){
        SymTable<FunctionEntry> * funcTable = this->prog->getFuncTable();
        // bool readBool();
        std::vector<TypeNode*> paraTypes;
        PrimitiveTypeNode * retType = new PrimitiveTypeNode(TypeNode::Bool);
        smallc::FunctionEntry * fun = new smallc::FunctionEntry(retType, paraTypes, false);
        funcTable->insert("readBool", *fun);

        // int readInt();
        retType = new PrimitiveTypeNode(TypeNode::Int);
        fun = new smallc::FunctionEntry(retType, paraTypes, false);
        funcTable->insert("readInt", *fun);
        
        // void writeBool(bool b);
        paraTypes.push_back(new PrimitiveTypeNode(TypeNode::Bool));
        retType = new PrimitiveTypeNode(TypeNode::Void);
        fun = new smallc::FunctionEntry(retType, paraTypes, false);
        funcTable->insert("writeBool", *fun);

        // void writeInt(int i);
        paraTypes.clear();
        paraTypes.push_back(new PrimitiveTypeNode(TypeNode::Int));
        retType = new PrimitiveTypeNode(TypeNode::Void);
        fun = new smallc::FunctionEntry(retType, paraTypes, false);
        funcTable->insert("writeInt", *fun);

        // void newLine();
        paraTypes.clear();
        retType = new PrimitiveTypeNode(TypeNode::Void);
        fun = new smallc::FunctionEntry(retType, paraTypes, false);
        funcTable->insert("newLine", *fun);
    }
    ASTVisitorBase::visitProgramNode(prg);
}

void 
SemanticAnalyzer::visitArgumentNode(ArgumentNode *arg){
    arg->getExpr()->visit(this);
    ASTVisitorBase::visitArgumentNode(arg);
}

void 
SemanticAnalyzer::visitDeclNode(DeclNode *decl){
    ASTVisitorBase::visitDeclNode(decl);
}

void
SemanticAnalyzer::visitFunctionDeclNode(FunctionDeclNode *func){
    func->getRetType()->visit(this);
    for (auto i : func->getParams())
        i->visit(this);
    if (func->getBody()){
        // insert variable table
        for(auto param : func->getParams()){
            std::string name = param->getIdent()->getName();
            PrimitiveTypeNode * primitive = dynamic_cast<PrimitiveTypeNode*>(param->getType());
            ArrayTypeNode * array = dynamic_cast<ArrayTypeNode*>(param->getType());
            VariableEntry * var;
            if(primitive && !array) var = new VariableEntry(primitive);
            else var = new VariableEntry(array);
            func->getBody()->getVarTable()->insert(name, *var);
        }
        func->getBody()->visit(this);
    }

    SymTable<FunctionEntry> * funcTable = this->prog->getFuncTable();
    bool noError = true;
    std::string name = func->getIdent()->getName();

    // 1. Function Prototype
    if(func->getProto()){
        // Error 4.1 InconsistentDef
        if(funcTable->contains(name) && funcTable->get(name).getProto()){
            FunctionEntry ent = funcTable->get(name);
            if(ent.getParameterTypes().size() != func->getNumParameters() ||
               ent.getReturnType() != func->getRetType()){
                noError = false;
            }else{
                for(int i = 0; i < func->getNumParameters(); i++){
                    TypeNode::TypeEnum type = func->getParams()[i]->getType()->getTypeEnum();
                    if(ent.getParameterTypes()[i]->getTypeEnum() != type){
                        noError = false;
                        break;
                    }
                }
            }
            if(!noError){
                SemaError * err = new smallc::SemaError(SemaError::InconsistentDef, 
                                                        func->getLocation(), name);
                this->addError(*err);
            }
            noError = false;
        }
    }
    // 2. Function Declaration
    else{
        // Error 0.3 IdentReDefined
        if(funcTable->contains(name) && !funcTable->get(name).getProto()){
            SemaError * err = new smallc::SemaError(SemaError::IdentReDefined, 
                                                    func->getLocation(), name);
            this->addError(*err);
            noError = false;
        }
        // TODO: Error 4.2 InconsistentDef or Error 2.1 NoMatchingDef ???
    }

    if(noError){
        // insert function table
        std::vector<TypeNode*> paraTypes;
        for(auto param : func->getParams()){
            PrimitiveTypeNode * primitive = dynamic_cast<PrimitiveTypeNode*>(param->getType());
            ArrayTypeNode * array = dynamic_cast<ArrayTypeNode*>(param->getType());
            if(primitive && !array) paraTypes.push_back(primitive);
            else paraTypes.push_back(array);
        }
        smallc::FunctionEntry * fun;
        if(func->getProto()) fun = new smallc::FunctionEntry(func->getRetType(), paraTypes, true);
        else fun = new smallc::FunctionEntry(func->getRetType(), paraTypes, false);
        funcTable->insert(func->getIdent()->getName(), *fun);
    }

    ASTVisitorBase::visitFunctionDeclNode(func);
}

SymTable<VariableEntry> *
findScopeVarTable(ASTNode * node, ScopeNode *& scope){
    ASTNode * curr = node;
    scope = nullptr;
    while(!scope && curr){
        scope = dynamic_cast<ScopeNode*>(curr);
        curr = curr->getParent();
    }
    
    SymTable<VariableEntry> * varTable;
    if(scope) varTable = scope->getVarTable();
    else varTable = nullptr;
    return varTable;
}

void 
SemanticAnalyzer::visitArrayDeclNode(ArrayDeclNode *array){
    array->getType()->visit(this);
    array->getIdent()->visit(this);

    ScopeNode * scope = nullptr;
    SymTable<VariableEntry> * varTable = findScopeVarTable(array, scope);
    if(!varTable) varTable = this->prog->getVarTable();

    bool noError = true;

    // Error 0.1 IdentReDefined
    if(varTable && varTable->contains(array->getIdent()->getName())){
        SemaError * err = new smallc::SemaError(SemaError::IdentReDefined, 
                                                array->getLocation(), 
                                                array->getIdent()->getName());
        this->addError(*err);
        noError = false;
    }

    // Error 7 InvalidArraySize
    if(array->getType()->getSize() < 0){
        SemaError * err = new smallc::SemaError(SemaError::InvalidArraySize, 
                                                array->getLocation(), 
                                                array->getIdent()->getName());
        this->addError(*err);
        noError = false;
    }

    if(noError && varTable){
        smallc::VariableEntry * var = new smallc::VariableEntry(array->getType());
        varTable->insert(array->getIdent()->getName(), *var);
    }

    ASTVisitorBase::visitArrayDeclNode(array);
}

void
SemanticAnalyzer::visitScalarDeclNode(ScalarDeclNode *scalar){
    scalar->getType()->visit(this);
    scalar->getIdent()->visit(this);

    ScopeNode * scope = nullptr;
    SymTable<VariableEntry> * varTable = findScopeVarTable(scalar, scope);
    if(!varTable) varTable = this->prog->getVarTable();
    bool noError = true;

    // Error 0.2 IdentReDefined
    if(varTable && varTable->contains(scalar->getIdent()->getName())){
        SemaError * err = new smallc::SemaError(SemaError::IdentReDefined, 
                                                scalar->getLocation(), 
                                                scalar->getIdent()->getName());
        this->addError(*err);
        noError = false;
    }

    if(noError && varTable){
        smallc::VariableEntry * var = new smallc::VariableEntry(scalar->getType());
        varTable->insert(scalar->getIdent()->getName(), *var);
    }

    ASTVisitorBase::visitScalarDeclNode(scalar);
}

void
SemanticAnalyzer::visitExprNode(ExprNode *exp){
    ASTVisitorBase::visitExprNode(exp);
}

TypeNode::TypeEnum
findVarType(std::string name, ScopeNode * scope, bool & isArray){
    TypeNode::TypeEnum type = TypeNode::TypeEnum::Void;
    if(!scope) return type;
    
    ASTNode * curr = scope;
    SymTable<VariableEntry> * varTable = scope->getVarTable();
    scope = nullptr;
    while(curr){
        scope = dynamic_cast<ScopeNode*>(curr);
        if(scope != nullptr){
            varTable = scope->getVarTable();
            if(varTable->contains(name)){
                type = varTable->get(name).getType()->getTypeEnum();
                isArray = varTable->get(name).getIsArray();
                break;
            }
        }
        curr = curr->getParent();
    }
    return type;
}

TypeNode::TypeEnum
findVarType(std::string name, ScopeNode * scope){
    TypeNode::TypeEnum type = TypeNode::TypeEnum::Void;
    if(!scope) return type;
    
    ASTNode * curr = scope;
    SymTable<VariableEntry> * varTable = scope->getVarTable();
    scope = nullptr;
    while(curr){
        scope = dynamic_cast<ScopeNode*>(curr);
        if(scope != nullptr){
            varTable = scope->getVarTable();
            if(varTable->contains(name)){
                type = varTable->get(name).getType()->getTypeEnum();
                break;
            }
        }
        curr = curr->getParent();
    }
    return type;
}

bool
findVariable(std::string name, ScopeNode * scope){
    bool isContain = false;
    if(!scope) return isContain;
    
    ASTNode * curr = scope;
    SymTable<VariableEntry> * varTable = scope->getVarTable();
    scope = nullptr;
    while(curr){
        scope = dynamic_cast<ScopeNode*>(curr);
        if(scope != nullptr){
            varTable = scope->getVarTable();
            if(varTable->contains(name)){
                isContain = true;
                break;
            }
        }
        curr = curr->getParent();
    }
    return isContain;
}

void 
SemanticAnalyzer::visitCallExprNode(CallExprNode *call){
    call->getIdent()->visit(this);
    for (auto i: call->getArguments())
        i->visit(this);

    SymTable<FunctionEntry> * funcTable = this->prog->getFuncTable();
    bool noError = true;

    // Error 1.2 IdentUnDefined
    if(!funcTable->contains(call->getIdent()->getName())){
        SemaError * err = new smallc::SemaError(SemaError::IdentUnDefined, 
                                                call->getLocation(), 
                                                call->getIdent()->getName());
        this->addError(*err);
        call->setType(new PrimitiveTypeNode(TypeNode::Error));
        noError = false;
    }
    // Error 2.2 NoMatchingDef
    else{
        FunctionEntry ent = funcTable->get(call->getIdent()->getName());
        if(ent.getParameterTypes().size() != call->getArguments().size()){
            noError = false;
        }else{
            for(int i = 0; i < call->getArguments().size(); i++){
                ExprNode * argu = call->getArgument(i)->getExpr();
                TypeNode::TypeEnum type = argu->getType()->getTypeEnum();
                IntExprNode * intExpr = dynamic_cast<IntExprNode*>(argu);
                bool isArray = false;
                if(intExpr){
                    ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
                    if(ref){
                        ScopeNode * scope = nullptr;
                        findScopeVarTable(ref, scope);
                        std::string name = ref->getIdent()->getName();
                        if(findVarType(name, scope, isArray) == TypeNode::Void){
                            SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                            if(varTable->contains(name)) type = varTable->get(name).getType()->getTypeEnum();
                        }else{
                            type = findVarType(name, scope, isArray);
                        }
                        if(ref->getType()->getTypeEnum() == TypeNode::Error) type = TypeNode::Error;
                    }
                }
                ArrayTypeNode * array = dynamic_cast<ArrayTypeNode*>(ent.getParameterTypes()[i]);
                bool paramIsArray = false;
                if(array) paramIsArray = true;
                
                if(ent.getParameterTypes()[i]->getTypeEnum() != type || (paramIsArray != isArray)){
                    noError = false;
                    break;
                }
            }
        }
        if(!noError){
            SemaError * err = new smallc::SemaError(SemaError::NoMatchingDef, 
                                                    call->getLocation(), 
                                                    call->getIdent()->getName());
            this->addError(*err);
            call->setType(new PrimitiveTypeNode(TypeNode::Error));
        }
    }
    ASTVisitorBase::visitCallExprNode(call);
}

void
SemanticAnalyzer::visitConstantExprNode(ConstantExprNode *constant){
    ASTVisitorBase::visitConstantExprNode(constant);
}

void
SemanticAnalyzer::visitBoolConstantNode(BoolConstantNode *boolConst){
    ASTVisitorBase::visitBoolConstantNode(boolConst);
}

void
SemanticAnalyzer::visitIntConstantNode(IntConstantNode *intConst){
    ASTVisitorBase::visitIntConstantNode(intConst);
}

void 
SemanticAnalyzer::visitUnaryExprNode(UnaryExprNode *unary){
    unary->getOperand()->visit(this);

    // Error 6.5 TypeMisMatch
    bool noError = true;
    ExprNode * expr = unary->getOperand();
    TypeNode::TypeEnum type = expr->getType()->getTypeEnum();
    IntExprNode * intExpr = dynamic_cast<IntExprNode*>(expr);
    if(intExpr){
        ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
        if(ref){
            ScopeNode * scope = nullptr;
            findScopeVarTable(ref, scope);
            std::string name = ref->getIdent()->getName();
            if(findVarType(name, scope) == TypeNode::Void){
                SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                if(varTable->contains(name)) type = varTable->get(name).getType()->getTypeEnum();
            }else{
                type = findVarType(name, scope);
            }
            if(ref->getType()->getTypeEnum() == TypeNode::Error) type = TypeNode::Error;
        }
    }

    std::string op = unary->codeToStr(unary->getOpcode());

    if(type == TypeNode::Error || type == TypeNode::Void || 
    (op == "!" && type != TypeNode::Bool) || 
    (op == "-" && type != TypeNode::Int)){
        noError = false;
    }

    if(!noError){
        SemaError * err = new smallc::SemaError(SemaError::TypeMisMatch, 
                                                unary->getLocation(), "");
        this->addError(*err);
        unary->setType(new PrimitiveTypeNode(TypeNode::Error));
    }

    ASTVisitorBase::visitUnaryExprNode(unary);
}

void 
SemanticAnalyzer::visitBinaryExprNode(BinaryExprNode *bin){
    bin->getLeft()->visit(this);
    bin->getRight()->visit(this);

    // Error 6.4 TypeMisMatch
    bool noError = true;
    ExprNode * expr = bin->getLeft();
    TypeNode::TypeEnum lType = expr->getType()->getTypeEnum();
    IntExprNode * intExpr = dynamic_cast<IntExprNode*>(expr);
    if(intExpr){
        ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
        if(ref){
            ScopeNode * scope = nullptr;
            findScopeVarTable(ref, scope);
            std::string name = ref->getIdent()->getName();
            if(findVarType(name, scope) == TypeNode::Void){
                SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                if(varTable->contains(name)) lType = varTable->get(name).getType()->getTypeEnum();
            }else{
                lType = findVarType(name, scope);
            }
            if(ref->getType()->getTypeEnum() == TypeNode::Error) lType = TypeNode::Error;
        }
    }

    expr = bin->getRight();
    TypeNode::TypeEnum rType = expr->getType()->getTypeEnum();
    intExpr = dynamic_cast<IntExprNode*>(expr);
    if(intExpr){
        ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
        if(ref){
            ScopeNode * scope = nullptr;
            findScopeVarTable(ref, scope);
            std::string name = ref->getIdent()->getName();
            if(findVarType(name, scope) == TypeNode::Void){
                SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                if(varTable->contains(name)) rType = varTable->get(name).getType()->getTypeEnum();
            }else{
                rType = findVarType(name, scope);
            }
            if(ref->getType()->getTypeEnum() == TypeNode::Error) rType = TypeNode::Error;
        }
    }
    
    std::string op = bin->codeToStr(bin->getOpcode());
        
    if(lType != rType) 
        noError = false;
    else if(lType == TypeNode::Error || lType == TypeNode::Void ||
            ((op == "&&" || op == "||") && lType != TypeNode::Bool) ||
            ((op == "<" || op == "<=" || op == ">" || op == ">=") && lType != TypeNode::Int) || 
            ((op == "+" || op == "-" || op == "*" || op == "/") && lType != TypeNode::Int)){
            noError = false;
    }

    if(!noError){
        SemaError * err = new smallc::SemaError(SemaError::TypeMisMatch, 
                                                bin->getLocation(), "");
        this->addError(*err);
        bin->setType(new PrimitiveTypeNode(TypeNode::Error));
    }

    ASTVisitorBase::visitBinaryExprNode(bin);
}

void
SemanticAnalyzer::visitBoolExprNode(BoolExprNode *boolExpr){
    boolExpr->getValue()->visit(this);
    ASTVisitorBase::visitBoolExprNode(boolExpr);
}

void 
SemanticAnalyzer::visitIntExprNode(IntExprNode *intExpr){
    intExpr->getValue()->visit(this);
    ASTVisitorBase::visitIntExprNode(intExpr);
}

void 
SemanticAnalyzer::visitReferenceExprNode(ReferenceExprNode *ref){
    ref->getIdent()->visit(this);
    if (ref->getIndex())
        ref->getIndex()->visit(this);

    ScopeNode * scope = nullptr;
    SymTable<VariableEntry> * varTable = findScopeVarTable(ref, scope);
    std::string name = ref->getIdent()->getName();

    // Error 1.1 IdentUnDefined
    if(!findVariable(name, scope) && 
        !this->prog->getVarTable()->contains(name)){
        SemaError * err = new smallc::SemaError(SemaError::IdentUnDefined, 
                                                ref->getLocation(), name);
        this->addError(*err);
        ref->setType(new PrimitiveTypeNode(TypeNode::Error));
    }

    // Error 8 InvalidAccess
    bool isArray = false;
    TypeNode::TypeEnum type = findVarType(name, scope, isArray);
    if((isArray && ref->getIndex() == nullptr) || (!isArray && ref->getIndex() != nullptr)){
        SemaError * err = new smallc::SemaError(SemaError::InvalidAccess, 
                                                ref->getLocation(), name);
        this->addError(*err);
        ref->setType(new PrimitiveTypeNode(TypeNode::Error));
    }

    ASTVisitorBase::visitReferenceExprNode(ref);
}

void 
SemanticAnalyzer::visitIdentifierNode(IdentifierNode *id){
    ASTVisitorBase::visitIdentifierNode(id);
}

void 
SemanticAnalyzer::visitParameterNode(ParameterNode *param){
    param->getType()->visit(this);
    param->getIdent()->visit(this);
    ASTVisitorBase::visitParameterNode(param);
}

void 
SemanticAnalyzer::visitStmtNode(StmtNode *stmt){
    ASTVisitorBase::visitStmtNode(stmt);
}

void 
SemanticAnalyzer::visitAssignStmtNode(AssignStmtNode *assign){
    assign->getTarget()->visit(this);
    assign->getValue()->visit(this);

    // Error 6.3 TypeMisMatch
    ExprNode * expr = assign->getValue();
    TypeNode::TypeEnum rType = expr->getType()->getTypeEnum();
    
    IntExprNode * intExpr = dynamic_cast<IntExprNode*>(expr);
    if(intExpr){
        ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
        if(ref){
            ScopeNode * scope = nullptr;
            findScopeVarTable(ref, scope);
            std::string name = ref->getIdent()->getName();
            if(findVarType(name, scope) == TypeNode::Void){
                SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                if(varTable->contains(name)) rType = varTable->get(name).getType()->getTypeEnum();
            }else{
                rType = findVarType(name, scope);
            }
            if(ref->getType()->getTypeEnum() == TypeNode::Error) rType = TypeNode::Error;
        }
    }

    CallExprNode * callExpr = dynamic_cast<CallExprNode*>(expr);
    if(callExpr){
        std::string name = callExpr->getIdent()->getName();
        SymTable<FunctionEntry> * funcTable = this->prog->getFuncTable();
        if(funcTable->contains(name)) rType = funcTable->get(name).getReturnType()->getTypeEnum();
        if(callExpr->getType()->getTypeEnum() == TypeNode::Error) rType = TypeNode::Error;
    }

    TypeNode::TypeEnum lType = expr->getType()->getTypeEnum();
    ReferenceExprNode * ref = assign->getTarget();
    ScopeNode * scope = nullptr;
    findScopeVarTable(ref, scope);
    std::string name = ref->getIdent()->getName();
    if(findVarType(name, scope) == TypeNode::Void){
        SymTable<VariableEntry> * varTable = this->prog->getVarTable();
        if(varTable->contains(name)) lType = varTable->get(name).getType()->getTypeEnum();
    }else{
        lType = findVarType(name, scope);
    }
    if(ref->getType()->getTypeEnum() == TypeNode::Error) lType = TypeNode::Error;
    
    if(rType == TypeNode::Void || rType == TypeNode::Error || 
        lType == TypeNode::Void || lType == TypeNode::Error || rType != lType){
        SemaError * err = new smallc::SemaError(SemaError::TypeMisMatch, 
                                                assign->getLocation(), "");
        this->addError(*err);
    }

    ASTVisitorBase::visitAssignStmtNode(assign);
}

void 
SemanticAnalyzer::visitExprStmtNode(ExprStmtNode *expr){
    expr->getExpr()->visit(this);
    ASTVisitorBase::visitExprStmtNode(expr);
}

void 
SemanticAnalyzer::visitIfStmtNode(IfStmtNode *ifStmt){
    ifStmt->getThen()->visit(this);
    if(ifStmt->getHasElse()) 
        ifStmt->getElse()->visit(this);
    ifStmt->getCondition()->visit(this);
    
    // Error 5.1 InvalidCond
    TypeNode::TypeEnum type;
    ExprNode * cond = ifStmt->getCondition();
    type = cond->getType()->getTypeEnum();
    IntExprNode * intExpr = dynamic_cast<IntExprNode*>(cond);
    if(intExpr){
        ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
        if(ref){
            ScopeNode * scope = nullptr;
            findScopeVarTable(ref, scope);
            std::string name = ref->getIdent()->getName();
            if(findVarType(name, scope) == TypeNode::Void){
                SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                if(varTable->contains(name)) type = varTable->get(name).getType()->getTypeEnum();
            }else{
                type = findVarType(name, scope);
            }
            if(ref->getType()->getTypeEnum() == TypeNode::Error) type = TypeNode::Error;
        }
    }

    if(cond == nullptr || type != TypeNode::Bool){
        SemaError * err = new smallc::SemaError(SemaError::InvalidCond, 
                                                ifStmt->getLocation(), 
                                                "if statement");
        this->addError(*err);
    }

    ASTVisitorBase::visitIfStmtNode(ifStmt);
}

void 
SemanticAnalyzer::visitReturnStmtNode(ReturnStmtNode *ret){
    if (!ret->returnVoid())
        ret->getReturn()->visit(this);
    
    // Error 3 MisMatchedReturn
    ASTNode * curr = ret;
    FunctionDeclNode * func = nullptr;
    while(!func && curr){
        func = dynamic_cast<FunctionDeclNode*>(curr);
        curr = curr->getParent();
    }
    TypeNode::TypeEnum retType = func->getRetType()->getTypeEnum();
    bool noError = true;

    // 3.1 void: accept return no expression
    if(retType == TypeNode::Void){
        if(!ret->returnVoid()) noError = false;
    }

    // 3.2 non-void: accept return with correct type
    else{
        if(ret->returnVoid()){
            noError = false;
        }else{
            TypeNode::TypeEnum type;
            type = ret->getReturn()->getType()->getTypeEnum();
            IntExprNode * intExpr = dynamic_cast<IntExprNode*>(ret->getReturn());
            if(intExpr){
                ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
                if(ref){
                    ScopeNode * scope = nullptr;
                    findScopeVarTable(ref, scope);
                    std::string name = ref->getIdent()->getName();
                    if(findVarType(name, scope) == TypeNode::Void){
                        SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                        if(varTable->contains(name)) type = varTable->get(name).getType()->getTypeEnum();
                    }else{
                        type = findVarType(name, scope);
                    }
                    if(ref->getType()->getTypeEnum() == TypeNode::Error) type = TypeNode::Error;
                }
            }
            if(retType != type) noError = false;
        }
    }

    if(!noError){
        SemaError * err = new smallc::SemaError(SemaError::MisMatchedReturn, 
                                                ret->getLocation(), "");
        this->addError(*err);
    }

    ASTVisitorBase::visitReturnStmtNode(ret);
}

void
SemanticAnalyzer::visitScopeNode(ScopeNode *scope){
    for (auto i: scope->getDeclarations())
        i->visit(this);
    ASTVisitorBase::visitScopeNode(scope);
}

void 
SemanticAnalyzer::visitWhileStmtNode(WhileStmtNode *whileStmt){
    whileStmt->getCondition()->visit(this);
    whileStmt->getBody()->visit(this);
    
    // Error 5.2 InvalidCond
    TypeNode::TypeEnum type;
    
    ExprNode * cond = whileStmt->getCondition();
    type = cond->getType()->getTypeEnum();

    IntExprNode * intExpr = dynamic_cast<IntExprNode*>(cond);
    if(intExpr){
        ReferenceExprNode * ref = dynamic_cast<ReferenceExprNode*>(intExpr->getValue());
        if(ref){
            ScopeNode * scope = nullptr;
            findScopeVarTable(ref, scope);
            std::string name = ref->getIdent()->getName();
            if(findVarType(name, scope) == TypeNode::Void){
                SymTable<VariableEntry> * varTable = this->prog->getVarTable();
                if(varTable->contains(name)) type = varTable->get(name).getType()->getTypeEnum();
            }else{
                type = findVarType(name, scope);
            }
            if(ref->getType()->getTypeEnum() == TypeNode::Error) type = TypeNode::Error;
        }
    }

    if(cond == nullptr || type != TypeNode::Bool){
        SemaError * err = new smallc::SemaError(SemaError::InvalidCond, 
                                                whileStmt->getLocation(), 
                                                "while statement");
        this->addError(*err);
    }

    ASTVisitorBase::visitWhileStmtNode(whileStmt);
}

void 
SemanticAnalyzer::visitTypeNode(TypeNode *type){
    ASTVisitorBase::visitTypeNode(type);
}

void 
SemanticAnalyzer::visitPrimitiveTypeNode(PrimitiveTypeNode *type){
    ASTVisitorBase::visitPrimitiveTypeNode(type);
}

void 
SemanticAnalyzer::visitArrayTypeNode(ArrayTypeNode *type){
    ASTVisitorBase::visitArrayTypeNode(type);
}
    
bool 
SemanticAnalyzer::success(){
    return this->errors.size() == 0;
}
}