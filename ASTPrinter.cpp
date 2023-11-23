//
//  ASTPrinter.cpp
//  ECE467 Lab 3
//
//  Created by Tarek Abdelrahman on 2023-07-31.
//  Based on code written by Ao (Dino) Li for ECE467.
//  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
//
//  Permission is hereby granted to use this code in ECE467 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.


#include "ASTPrinter.h"

namespace smallc {

ASTPrinter::ASTPrinter():indent(0), root(nullptr) { }

ASTPrinter::ASTPrinter(ProgramNode* prg):indent(0), root(prg) { }

void
ASTPrinter::incrIndent() { indent++; }

void
ASTPrinter::decrIndent() { indent--; }

std::string
ASTPrinter::genPrefix() { return std::string(indent, '\t'); }

std::string
ASTPrinter::genLocation(ASTNode *node) {
    return "(" + std::to_string(node->getLine()) + "," +
    std::to_string(node->getCol()) + ")\n";
}

void 
ASTPrinter::visitASTNode(ASTNode *node) {
    incrIndent();
    ASTVisitorBase::visitASTNode(node);
    decrIndent();
}

void ASTPrinter::visitProgramNode(ProgramNode *prg) {
    std::string res = genPrefix();
    res += "Program [useIO=" + std::to_string(prg->useIo()) + "]";
    res += genLocation(prg);
    std::cout << res;
    ASTVisitorBase::visitProgramNode(prg);
}

void ASTPrinter::visitScalarDeclNode(ScalarDeclNode *scalar) {
    std::string res = genPrefix();
    res += "Scalar Declaration";
    std::cout << res;
    scalar->getType()->visit(this);
    incrIndent();
    scalar->getIdent()->visit(this);
    decrIndent();
    ASTVisitorBase::visitScalarDeclNode(scalar);
}

void ASTPrinter::visitPrimitiveTypeNode(PrimitiveTypeNode *type) {
    std::string res = genPrefix();
    if (type->getTypeEnum() == smallc::TypeNode::Int)
        res += "Int";
    else if (type->getTypeEnum() == smallc::TypeNode::Bool)
        res += "Bool";
    else
        res += "Void";
    res += genLocation(type);
    std::cout << res;
    ASTVisitorBase::visitPrimitiveTypeNode(type);
}

void ASTPrinter::visitFunctionDeclNode(FunctionDeclNode *func) {
    std::string res = genPrefix();
    res += "Function[isProto=" + std::to_string(func->getProto()) + "]";
    std::cout << res;
    func->getRetType()->visit(this);
    incrIndent();
    func->getIdent()->visit(this);
    for (auto i: func->getParams())
        i->visit(this);
    if (func->getBody())
        func->getBody()->visit(this);
    decrIndent();
    ASTVisitorBase::visitFunctionDeclNode(func);
}

void ASTPrinter::visitIdentifierNode(IdentifierNode *id) {
    std::string res = genPrefix();
    res += "Identifier[name:" + id->getName() + "]";
    res += genLocation(id);
    std::cout << res;
    ASTVisitorBase::visitIdentifierNode(id);
}

void ASTPrinter::visitParameterNode(ParameterNode *param) {
    std::string res = genPrefix();
    res += "Parameter";
    std::cout << res;
    param->getType()->visit(this);
    incrIndent();
    param->getIdent()->visit(this);
    decrIndent();
    ASTVisitorBase::visitParameterNode(param);
}

void
ASTPrinter::visitArrayTypeNode(ArrayTypeNode *type) {
    std::string res = genPrefix();
    if (type->getTypeEnum() == smallc::TypeNode::Int)
        res += "Int";
    else if (type->getTypeEnum() == smallc::TypeNode::Bool)
        res += "Bool";
    else
        res += "Void";
    if (type->getSize() == 0)
        res += "*";
    else
        res += "[" + std::to_string(type->getSize()) + "]";
    res += genLocation(type);
    std::cout << res;
    ASTVisitorBase::visitArrayTypeNode(type);
}

void
ASTPrinter::visitScopeNode(ScopeNode *scope) {
    std::string res = genPrefix();
    res += "Scope";
    res += genLocation(scope);
    std::cout << res;
    incrIndent();
    for (auto i: scope->getDeclarations())
        i->visit(this);
    decrIndent();
    ASTVisitorBase::visitScopeNode(scope);
}

void 
ASTPrinter::visitArrayDeclNode(ArrayDeclNode *array) {
    std::string res = genPrefix();
    res += "Array Declaration";
    std::cout << res;
    array->getType()->visit(this);
    incrIndent();
    array->getIdent()->visit(this);
    decrIndent();
    ASTVisitorBase::visitArrayDeclNode(array);
}

void 
ASTPrinter::visitIfStmtNode(IfStmtNode *ifStmt) {
    std::string res = genPrefix();
    res += "IfStmt[hasElse=" + std::to_string(ifStmt->getHasElse()) + "]";
    res += genLocation(ifStmt);
    std::cout << res;
    incrIndent();
    ifStmt->getCondition()->visit(this);
    ifStmt->getThen()->visit(this);
    if (ifStmt->getHasElse())
        ifStmt->getElse()->visit(this);
    decrIndent();
    ASTVisitorBase::visitIfStmtNode(ifStmt);
}

void ASTPrinter::visitBoolExprNode(BoolExprNode *boolExpr) {
    std::string res = genPrefix();
    res += "Bool Expression";
    res += genLocation(boolExpr);
    std::cout << res;
    incrIndent();
    boolExpr->getValue()->visit(this);
    decrIndent();
    ASTVisitorBase::visitBoolExprNode(boolExpr);
}

void ASTPrinter::visitBinaryExprNode(BinaryExprNode *bin) {
    std::string res = genPrefix();
    res += "Binary Expression[opcode:" + ExprNode::codeToStr(bin->getOpcode()) + "]";
    res += genLocation(bin);
    std::cout << res;
    incrIndent();
    bin->getLeft()->visit(this);
    bin->getRight()->visit(this);
    decrIndent();
    ASTVisitorBase::visitBinaryExprNode(bin);
}

void ASTPrinter::visitIntExprNode(IntExprNode *intExpr) {
    std::string res = genPrefix();
    res += "Int Expression";
    res += genLocation(intExpr);
    std::cout << res;
    incrIndent();
    intExpr->getValue()->visit(this);
    decrIndent();
    ASTVisitorBase::visitIntExprNode(intExpr);
}

void ASTPrinter::visitReferenceExprNode(ReferenceExprNode *ref) {
    std::string res = genPrefix();
    res += "Reference";
    res += genLocation(ref);
    std::cout << res;
    incrIndent();
    ref->getIdent()->visit(this);
    if (ref->getIndex())
        ref->getIndex()->visit(this);
    decrIndent();
    ASTVisitorBase::visitReferenceExprNode(ref);
}

void ASTPrinter::visitAssignStmtNode(AssignStmtNode *assign) {
    std::string res = genPrefix();
    res += "Assignment";
    res += genLocation(assign);
    std::cout << res;
    incrIndent();
    assign->getTarget()->visit(this);
    assign->getValue()->visit(this);
    decrIndent();
    ASTVisitorBase::visitAssignStmtNode(assign);
}

void ASTPrinter::visitExprStmtNode(ExprStmtNode *expr) {
    std::string res = genPrefix();
    res += "Expression Statement";
    res += genLocation(expr);
    std::cout << res;
    incrIndent();
    expr->getExpr()->visit(this);
    decrIndent();
    ASTVisitorBase::visitExprStmtNode(expr);
}

void ASTPrinter::visitWhileStmtNode(WhileStmtNode *whileStmt) {
    std::string res = genPrefix();
    res += "WhileStmt";
    res += genLocation(whileStmt);
    std::cout << res;
    incrIndent();
    whileStmt->getCondition()->visit(this);
    whileStmt->getBody()->visit(this);
    decrIndent();
    ASTVisitorBase::visitWhileStmtNode(whileStmt);
}

void ASTPrinter::visitIntConstantNode(IntConstantNode *intConst) {
    std::string res = genPrefix();
    res += "IntConstant[val=" + std::to_string(intConst->getVal()) + "]";
    res += genLocation(intConst);
    std::cout << res;
    ASTVisitorBase::visitIntConstantNode(intConst);
}

void ASTPrinter::visitBoolConstantNode(BoolConstantNode *boolConst) {
    std::string res = genPrefix();
    res += "BoolConstant[val=" + std::to_string(boolConst->getVal()) + "]";
    res += genLocation(boolConst);
    std::cout << res;
    ASTVisitorBase::visitBoolConstantNode(boolConst);
}

void ASTPrinter::visitArgumentNode(ArgumentNode *arg) {
    std::string res = genPrefix();
    res += "Argument";
    res += genLocation(arg);
    std::cout << res;
    incrIndent();
    arg->getExpr()->visit(this);
    decrIndent();
    ASTVisitorBase::visitArgumentNode(arg);
}

void ASTPrinter::visitCallExprNode(CallExprNode *call) {
    std::string res = genPrefix();
    res += "CallExpr";
    res += genLocation(call);
    std::cout << res;
    incrIndent();
    call->getIdent()->visit(this);
    for (auto i: call->getArguments())
        i->visit(this);
    decrIndent();
    ASTVisitorBase::visitCallExprNode(call);
}

void ASTPrinter::visitUnaryExprNode(UnaryExprNode *unary) {
    std::string res = genPrefix();
    res += "UnaryExpr[opcode:" + ExprNode::codeToStr(unary->getOpcode()) + "]";
    res += genLocation(unary);
    std::cout << res;
    incrIndent();
    unary->getOperand()->visit(this);
    decrIndent();
    ASTVisitorBase::visitUnaryExprNode(unary);
}

void ASTPrinter::visitReturnStmtNode(ReturnStmtNode *ret) {
    std::string res = genPrefix();
    res += "ReturnStmt";
    res += genLocation(ret);
    std::cout << res;
    incrIndent();
    if (!ret->returnVoid())
        ret->getReturn()->visit(this);
    decrIndent();
    ASTVisitorBase::visitReturnStmtNode(ret);
}

} // namespace smallc
