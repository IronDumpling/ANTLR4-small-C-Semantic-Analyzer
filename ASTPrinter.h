//
//  ASTPrinter.h
//  ECE467 Lab 3
//
//  Created by Tarek Abdelrahman on 2023-09-29.
//  Based on code written by Ao (Dino) Li for ECE467.
//  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
//
//  Permission is hereby granted to use this code in ECE467 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.


#ifndef ASTPrinter_h
#define ASTPrinter_h

#include <iostream>
using namespace std;
#include "ASTNodes.h"
#include "ASTVisitorBase.h"

namespace smallc{
class ASTPrinter: public ASTVisitorBase{
private:
    unsigned int indent;    // indentation of printing
    ProgramNode* root;      // Pointer to ProgramNode
    
public:
    ASTPrinter();         // Constructor
    explicit ASTPrinter(ProgramNode* prg); // Constructor with ProgramNode

    void incrIndent();     // Increase indent
    void decrIndent();     // Decrease indent
    std::string genPrefix(); // Get the prefix
    std::string genLocation(ASTNode* node); // Generate string with location

    // Visitors
    void visitASTNode(ASTNode* node) override;
    void visitProgramNode(ProgramNode *prg) override;
    void visitScalarDeclNode(ScalarDeclNode *scalar) override;
    void visitPrimitiveTypeNode(PrimitiveTypeNode *type) override;
    void visitFunctionDeclNode(FunctionDeclNode *func) override;
    void visitIdentifierNode(IdentifierNode *id) override;
    void visitParameterNode(ParameterNode *param) override;
    void visitArrayTypeNode(ArrayTypeNode *type) override;
    void visitScopeNode(ScopeNode *scope) override;
    void visitArrayDeclNode(ArrayDeclNode *array) override;
    void visitIfStmtNode(IfStmtNode *ifStmt) override;
    void visitBoolExprNode(BoolExprNode *boolExpr) override;
    void visitBinaryExprNode(BinaryExprNode *bin) override;
    void visitIntExprNode(IntExprNode *intExpr) override;
    void visitReferenceExprNode(ReferenceExprNode *ref) override;
    void visitAssignStmtNode(AssignStmtNode *assign) override;
    void visitExprStmtNode(ExprStmtNode *expr) override;
    void visitWhileStmtNode(WhileStmtNode *whileStmt) override;
    void visitIntConstantNode(IntConstantNode *intConst) override;
    void visitBoolConstantNode(BoolConstantNode *boolConst) override;
    void visitArgumentNode(ArgumentNode *arg) override;
    void visitCallExprNode(CallExprNode *call) override;
    void visitUnaryExprNode(UnaryExprNode *unary) override;
    void visitReturnStmtNode(ReturnStmtNode *ret) override;
};
}
#endif /* ASTPrinter_h */
