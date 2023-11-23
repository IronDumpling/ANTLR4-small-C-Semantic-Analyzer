//
//  smallC.g4
//  ECE467 Lab 3
//
//  Created by Tarek Abdelrahman on 2023-09-13.
//  Based on code written by Ao (Dino) Li for ECE467.
//  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
//
//  Permission is hereby granted to use this code in ECE467 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.

grammar smallC;

@header {
#include "ASTNodes.h"
#include <iostream>
#include <string>
}


program
returns [smallc::ProgramNode *prg]
@init {
    $prg = new smallc::ProgramNode();
    $prg->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    $prg->setRoot($prg);
}
: (preamble {$prg->setIo(true);} | ) (decls{
    for(unsigned int i = 0; i < $decls.declarations->size();i++){
        $prg->addChild($decls.declarations->at(i));
    }
})* EOF
;


preamble: 
    '#include' '"scio.h"'
;


decls 
returns [std::vector<smallc::DeclNode *>* declarations] 
@init{
    $declarations = new std::vector<smallc::DeclNode *>();
}
:
    scalarDeclList {
        for(unsigned int i = 0; i < $scalarDeclList.scalars->size(); i++){
            $declarations->push_back($scalarDeclList.scalars->at(i));
        }
    }
    | arrDeclList {
        for(unsigned int i = 0; i < $arrDeclList.declList->size(); i++){
            $declarations->push_back($arrDeclList.declList->at(i));
        }
    }
    | fcnProto {
        $declarations->push_back($fcnProto.decl);
    }
    | fcnDecl {
        $declarations->push_back($fcnDecl.decl);
    }
;


scalarDeclList
returns [std::vector<smallc::ScalarDeclNode *>* scalars]
@init{
    $scalars = new std::vector<smallc::ScalarDeclNode *>();
}   
:
    scalarDecl {
        $scalars->push_back($scalarDecl.decl);
    }
    | scalarDecl {
        $scalars->push_back($scalarDecl.decl);
    } scalarDeclList
;


scalarDecl 
returns [smallc::ScalarDeclNode * decl]
@init{
    $decl = new smallc::ScalarDeclNode();
    $decl->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    varType varName ';' {
        $decl->setType($varType.variableType);
        $decl->setName($varName.variableName);
    }
;


arrDeclList 
returns [std::vector<smallc::ArrayDeclNode *>* declList]
@init{
    $declList = new std::vector<smallc::ArrayDeclNode *>();
}
:
    arrDecl {
        $declList->push_back($arrDecl.decl);
    }
    | arrDecl arrDeclList {
        $declList->push_back($arrDecl.decl);
        for(unsigned int i = 0; i < $arrDeclList.declList->size(); i++){
            $declList->push_back($arrDeclList.declList->at(i));
        }
    }
;


arrDecl 
returns [smallc::ArrayDeclNode * decl]
:
    varType arrName '[' intConst ']' ';' {
        smallc::ArrayTypeNode * type = new smallc::ArrayTypeNode($varType.variableType, $intConst.intConstant->getVal());
        type->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
        $decl = new smallc::ArrayDeclNode(type, $arrName.arrayName);
        $decl->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


fcnProto 
returns [smallc::FunctionDeclNode * decl]
@init{
    $decl = new smallc::FunctionDeclNode();
    $decl->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    retType fcnName '(' params ')' ';' {
        $decl->setProto(true);
        $decl->setBody(nullptr);
        $decl->setRetType($retType.returnType);
        $decl->setName($fcnName.functionName);
        if($params.parameters != nullptr) $decl->setParameter(*($params.parameters));
    }
;


fcnDecl 
returns [smallc::FunctionDeclNode * decl]
@init{
    $decl = new smallc::FunctionDeclNode();
    $decl->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    retType fcnName '(' params ')' scope {
        $decl->setProto(false);
        $decl->setBody($scope.scope_);
        $decl->setRetType($retType.returnType);
        $decl->setName($fcnName.functionName);
        if($params.parameters != nullptr) $decl->setParameter(*($params.parameters));
    }
;


varType 
returns [smallc::PrimitiveTypeNode * variableType]
@init{
    $variableType = new smallc::PrimitiveTypeNode();
    $variableType->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    'bool' {
        $variableType->setType(smallc::TypeNode::Bool);
    }
    | 'int' {
        $variableType->setType(smallc::TypeNode::Int);
    }
;


retType 
returns [smallc::PrimitiveTypeNode * returnType]
@init{
    $returnType = new smallc::PrimitiveTypeNode();
    $returnType->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    'void' {
        $returnType->setType(smallc::TypeNode::Void);
    }
    | varType {
        $returnType = $varType.variableType;
    }
;

scope
returns[smallc::ScopeNode* scope_]
@init{
    $scope_ = new smallc::ScopeNode();
    $scope_->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    '{' (scalarDecl {
        $scope_->addDeclaration($scalarDecl.decl);
    } | arrDecl {
        $scope_->addDeclaration($arrDecl.decl);
    })* (stmt{
        $scope_->addChild($stmt.statement);
    })* '}'
;


stmt 
returns [smallc::StmtNode * statement]
:
    expr ';' {
        $statement = new smallc::ExprStmtNode($expr.expression);
        $statement->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | assignStmt {
        $statement = $assignStmt.assStatement;
    }
    | ifStmt {
        $statement = $ifStmt.ifStatement;
    }
    | whileStmt {
        $statement = $whileStmt.whileStatement;
    }
    | retStmt {
        $statement = $retStmt.retStatement;
    }
    | scope {
        $statement = $scope.scope_;
    }
;


assignStmt 
returns [smallc::AssignStmtNode * assStatement]
:
    var '=' expr ';' {
        $assStatement = new smallc::AssignStmtNode($var.variable, $expr.expression);
        $assStatement->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


ifStmt 
returns [smallc::IfStmtNode * ifStatement]
@init{
    $ifStatement = new smallc::IfStmtNode();
    $ifStatement->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    'if' '(' expr ')' stmt {
        $ifStatement->setCondition($expr.expression);
        $ifStatement->setThen($stmt.statement);
    }
    | 'if' '(' expr ')' then=stmt 'else' e=stmt {
        $ifStatement->setCondition($expr.expression);
        $ifStatement->setThen($then.statement);
        $ifStatement->setElse($e.statement);
    }
;


whileStmt 
returns [smallc::WhileStmtNode * whileStatement]
:
    'while' '(' expr ')' stmt {
        $whileStatement = new smallc::WhileStmtNode($expr.expression, $stmt.statement);
        $whileStatement->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


retStmt 
returns [smallc::ReturnStmtNode * retStatement]
@init{
    $retStatement = new smallc::ReturnStmtNode();
    $retStatement->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    'return' expr ';' {
        $retStatement->setReturn($expr.expression);
    }
    |'return' ';'
;


expr 
returns [smallc::ExprNode * expression]
:
    intExpr {
        $expression = $intExpr.intExpression;
    }
    | '(' expr ')' {
        $expression = $expr.expression;
        $expression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | fcnName '(' args ')' {
        if($args.arguments != nullptr) 
            $expression = new smallc::CallExprNode($fcnName.functionName, *($args.arguments));
        else
            $expression = new smallc::CallExprNode($fcnName.functionName);
        $expression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | '!' expr {
        smallc::UnaryExprNode * un = new smallc::UnaryExprNode($expr.expression);
        un->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
        un->setOpcode("!");
        $expression = new smallc::BoolExprNode(un);
        $expression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | '-' expr {
        smallc::UnaryExprNode * un = new smallc::UnaryExprNode($expr.expression);
        un->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
        un->setOpcode("-");
        $expression = new smallc::IntExprNode(un);
        $expression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | l=expr op=('==' | '!=' | '<' | '<=' | '>' | '>=' | '||' | '&&') r=expr {
        smallc::BinaryExprNode * bin = new smallc::BinaryExprNode($l.expression, $r.expression);
        bin->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
        bin->setOpcode($op.text);
        $expression = new smallc::BoolExprNode(bin);
        $expression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


intExpr 
returns [smallc::IntExprNode * intExpression]
:
    var {
        $intExpression = new smallc::IntExprNode($var.variable);
        $intExpression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | constant {
        $intExpression = new smallc::IntExprNode($constant.const_);
        $intExpression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | l=intExpr op=('*' | '/' | '+' | '-') r=intExpr {
        smallc::BinaryExprNode * bin = new smallc::BinaryExprNode($l.intExpression, $r.intExpression);
        bin->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
        bin->setOpcode($op.text);
        $intExpression = new smallc::IntExprNode(bin);
        $intExpression->setTypeInt();
        $intExpression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | '(' intExpr ')' {
        $intExpression = $intExpr.intExpression;
        $intExpression->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


constant 
returns [smallc::ConstantExprNode * const_]
:
    boolConst {
        $const_ = $boolConst.boolConstant;
    }
    | intConst {
        $const_ = $intConst.intConstant;
    }
;


boolConst 
returns [smallc::BoolConstantNode * boolConstant]
:
    BOOL {
        $boolConstant = new smallc::BoolConstantNode($BOOL.text);
        if($BOOL.text == "true") 
            $boolConstant->setVal(1); 
        else 
            $boolConstant->setVal(0);
        $boolConstant->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


intConst 
returns [smallc::IntConstantNode * intConstant]
:
    INT {
        $intConstant = new smallc::IntConstantNode($INT.text);
        $intConstant->setVal(stoi($INT.text));
        $intConstant->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
    | '-' INT {
        $intConstant = new smallc::IntConstantNode("-" + $INT.text);
        $intConstant->setVal(0-stoi($INT.text));
        $intConstant->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


params 
returns [std::vector<smallc::ParameterNode *>* parameters]
:
    paramList {
        $parameters = $paramList.parameterList;
    }
    | {
        $parameters = nullptr;
    }
;


paramEntry 
returns [smallc::ParameterNode * parameter]
@init{
    $parameter = new smallc::ParameterNode();
    $parameter->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    varType varName {
        $parameter->setType($varType.variableType);
        $parameter->setIdent($varName.variableName);
    }
    | varType arrName '[]' {
        smallc::ArrayTypeNode * array = new smallc::ArrayTypeNode($varType.variableType);
        $parameter->setType(array);
        $parameter->setIdent($arrName.arrayName);
    }
;


paramList 
returns [std::vector<smallc::ParameterNode *>* parameterList]
@init{
    $parameterList = new std::vector<smallc::ParameterNode *>();
}
:
    paramEntry {
        $parameterList->push_back($paramEntry.parameter);
    }
    | paramEntry ',' paramList {
        $parameterList->push_back($paramEntry.parameter);
        for(unsigned int i = 0; i < $paramList.parameterList->size(); i++){
            $parameterList->push_back($paramList.parameterList->at(i));
        }
    }
;


args 
returns [std::vector<smallc::ArgumentNode *>* arguments]
:
    argList {
        $arguments = $argList.argumentList;
    }
    | {
        $arguments = nullptr;
    }
;


argEntry 
returns [smallc::ArgumentNode * argument]
:
    expr {
        $argument = new smallc::ArgumentNode($expr.expression);
        $argument->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
    }
;


argList 
returns [std::vector<smallc::ArgumentNode *>* argumentList]
@init{
    $argumentList = new std::vector<smallc::ArgumentNode *>();
}
:
    argEntry {
        $argumentList->push_back($argEntry.argument);
    }
    | argEntry ',' argList {
        $argumentList->push_back($argEntry.argument);
        for(unsigned int i = 0; i < $argList.argumentList->size(); i++){
            $argumentList->push_back($argList.argumentList->at(i));
        }
    }
;


var
returns [smallc::ReferenceExprNode * variable]
@init{
    $variable = new smallc::ReferenceExprNode();
    $variable->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
:
    varName {
        $variable->setIdent($varName.variableName);
    }
    | arrName '[' intExpr ']'{
        $variable->setIdent($arrName.arrayName);
        $variable->setIndex($intExpr.intExpression);
    }
;


varName 
returns [smallc::IdentifierNode * variableName]
: ID {
    $variableName = new smallc::IdentifierNode($ID.text);
    $variableName->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
;


arrName 
returns [smallc::IdentifierNode * arrayName]
: ID {
    $arrayName = new smallc::IdentifierNode($ID.text);
    $arrayName->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
;


fcnName 
returns [smallc::IdentifierNode * functionName]
: ID {
    $functionName = new smallc::IdentifierNode($ID.text);
    $functionName->setLocation($ctx->start->getLine(), $ctx->start->getCharPositionInLine());
}
;


BOOL: 'true' | 'false';
ID: [a-zA-Z][a-zA-Z0-9_]* ;
INT:    [0] | ([1-9][0-9]*);
WS:     [ \t\r\n]+ -> skip;
COMMENT: '//' (~[\r\n])* -> skip;
