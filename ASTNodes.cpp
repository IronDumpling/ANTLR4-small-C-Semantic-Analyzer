//
//  ASTNodes.cpp
//  ECE467 Lab 3
//
//  Created by Tarek Abdelrahman on 2023-09-13.
//  Based on code written by Ao (Dino) Li for ECE467.
//  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
//
//  Permission is hereby granted to use this code in ECE467 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.

#include "ASTNodes.h"

#include <iostream>
#include <cassert>

using namespace smallc;

/**********************************************************************************/
/* The ASTNode Class                                                              */
/**********************************************************************************/
ASTNode::ASTNode () {
    parent = nullptr;
    location.first = 0;
    location.second = 0;
    root = nullptr;
}

ASTNode::~ASTNode () {
    cout << "DESTRUCTOR INVOKED\n";
    for (size_t i = 0; i < children.size(); i++)
        delete children[i];
}

ASTNode*
ASTNode::getParent () { return parent;}

ASTNode*
ASTNode::getChild (unsigned int i) { return children[i]; }

unsigned int
ASTNode::getNumChildren () { return (unsigned int) children.size(); }

unsigned int
ASTNode::getLine () { return location.first; }

unsigned int
ASTNode::getCol () { return location.second;}

std::pair<unsigned int, unsigned int>
ASTNode::getLocation () { return location; }

ProgramNode*
ASTNode::getRoot () { return root; }

// Mutators
void
ASTNode::addChild (ASTNode* child) {
    children.push_back(child);
    if (child != nullptr) child->setParent(this);
}

void
ASTNode::setParent (ASTNode *p) { parent = p; }

void
ASTNode::setRoot (ProgramNode *r) { root = r; }

void
ASTNode::setLine (unsigned int line) { location.first = line; }

void
ASTNode::setColumn (unsigned int column) { location.second = column; }

void
ASTNode::setLocation (unsigned int line, unsigned int column) {
    location.first = line;
    location.second = column;
}

void
ASTNode::setLocation (std::pair<unsigned int, unsigned int> loc) { location = loc; }

bool
ASTNode::hasVarTable () {
    return false;
}

FunctionDeclNode*
ASTNode::getFunction () {
    ASTNode* now = this;
    while (now) {
        if(typeid(*now) == typeid(FunctionDeclNode)) {
            return (FunctionDeclNode*)now;
        }
        now = now->getParent();
    }
    return nullptr;
}

void
ASTNode::visit(ASTVisitorBase* visitor) { }

/**********************************************************************************/
/* The ProgramNode Class                                                          */
/**********************************************************************************/
ProgramNode::ProgramNode () : ASTNode(), iolib(false) {
    this->fenv = new SymTable<FunctionEntry>();
    this->venv = new SymTable<VariableEntry>();
}

void
ProgramNode::setIo (bool flag) {
    iolib = flag;
}

bool
ProgramNode::useIo () {
    return iolib;
}


SymTable<FunctionEntry>*
ProgramNode::getFuncTable () {
    return fenv;
}

SymTable<VariableEntry>*
ProgramNode::getVarTable () {
    return venv;
}

bool
ProgramNode::hasVarTable () {
    return this->venv != nullptr;
}

void
ProgramNode::visit (ASTVisitorBase* visitor) {
    visitor->visitProgramNode(this);
}

/**********************************************************************************/
/* The TypeNode Class                                                             */
/**********************************************************************************/
void
TypeNode::setType(TypeEnum type){

}

TypeNode::TypeEnum 
TypeNode::getTypeEnum() const{
    
}

bool 
TypeNode::isArray(){
    return false;
}

void 
TypeNode::visit(ASTVisitorBase* visitor){
    visitor->visitTypeNode(this);
}

/**********************************************************************************/
/* The PrimitiveTypeNode Class                                                    */
/**********************************************************************************/

PrimitiveTypeNode::PrimitiveTypeNode(){
    this->type = Void;
}

PrimitiveTypeNode::PrimitiveTypeNode(TypeEnum type_){
    this->type = type_;
}

void 
PrimitiveTypeNode::setType(TypeEnum type_){
    this->type = type_;
}

PrimitiveTypeNode::TypeEnum 
PrimitiveTypeNode::getTypeEnum() const{
    return this->type;
}

bool 
PrimitiveTypeNode::operator == (const PrimitiveTypeNode& t){
    return this->type == t.type;
}

bool 
PrimitiveTypeNode::operator != (const PrimitiveTypeNode& t){
    return this->type != t.type;
}

void 
PrimitiveTypeNode::visit(ASTVisitorBase* visitor){
    visitor->visitPrimitiveTypeNode(this);
}

/**********************************************************************************/
/* The ArrayTypeNode Class                                                        */
/**********************************************************************************/

ArrayTypeNode::ArrayTypeNode(){
    type = nullptr;
    size = 0;
}

ArrayTypeNode::ArrayTypeNode(PrimitiveTypeNode *type_){
    this->type = type_;
    if(type_ != nullptr) type_->setParent(this);
    this->size = 0;
}

ArrayTypeNode::ArrayTypeNode(PrimitiveTypeNode* type_, int size_){
    this->type = type_;
    if(type_ != nullptr) type_->setParent(this);
    this->size = size_;
}

void 
ArrayTypeNode::setType(ArrayTypeNode::TypeEnum type_){
    type->setType(type_);
}

ArrayTypeNode::TypeEnum 
ArrayTypeNode::getTypeEnum() const{
    return type->getTypeEnum();
}

void 
ArrayTypeNode::setSize(int size_){
    size = size_;
}

int 
ArrayTypeNode::getSize(){
    return size;
}

bool 
ArrayTypeNode::operator == (const ArrayTypeNode& t){
    return (this->type == t.type) && (size == t.size);
}

bool 
ArrayTypeNode::operator != (const ArrayTypeNode& t){
    return (this->type != t.type) || (size != t.size);
}

bool 
ArrayTypeNode::isArray() {
    return true;
}

void 
ArrayTypeNode::visit(ASTVisitorBase* visitor){
    visitor->visitArrayTypeNode(this);
}
    
/**********************************************************************************/
/* The IdentifierNode Class                                                       */
/**********************************************************************************/

IdentifierNode::IdentifierNode(){
    name = "";
}

IdentifierNode::IdentifierNode(const std::string &text){
    name = text;
}

const std::string& 
IdentifierNode::getName(){
    return name;
}

void 
IdentifierNode::visit(ASTVisitorBase* visitor){
    visitor->visitIdentifierNode(this);
}

/**********************************************************************************/
/* The ParameterNode Class                                                        */
/**********************************************************************************/

ParameterNode::ParameterNode(){
    type = nullptr;
    name = nullptr;
}

ParameterNode::ParameterNode(TypeNode *type_, IdentifierNode *name_){
    this->type = type_;
    if(type_ != nullptr) type_->setParent(this);
    this->name = name_;
    if(name_ != nullptr) name_->setParent(this);
}

void
ParameterNode::setType(TypeNode *type_){
    type = type_;
    if(type_ != nullptr) type_->setParent(this);
}

TypeNode*& 
ParameterNode::getType(){
    return type;
}

void 
ParameterNode::setIdent(IdentifierNode *&name_){
    name = name_;
    if(name_ != nullptr) name_->setParent(this);
}

IdentifierNode*& 
ParameterNode::getIdent(){
    return name;
}

void
ParameterNode::visit(ASTVisitorBase* visitor){
    visitor->visitParameterNode(this);
}

/**********************************************************************************/
/* The Expression Class                                                           */
/**********************************************************************************/

ExprNode::ExprNode(){
    type = nullptr;
    this->setTypeVoid();
}

void 
ExprNode::setType(PrimitiveTypeNode* type_){
    this->type = type_;
    if(type_ != nullptr) type_->setParent(this);
}

void 
ExprNode::setTypeInt(){
    PrimitiveTypeNode * t = new PrimitiveTypeNode(TypeNode::Int);
    t->setLocation(this->getLocation());
    this->setType(t);
}

void
ExprNode::setTypeBool(){
    PrimitiveTypeNode * t = new PrimitiveTypeNode(TypeNode::Bool);
    t->setLocation(this->getLocation());
    this->setType(t);
}

void
ExprNode::setTypeVoid(){
    PrimitiveTypeNode * t = new PrimitiveTypeNode(TypeNode::Void);
    t->setLocation(this->getLocation());
    this->setType(t);   
}

PrimitiveTypeNode* 
ExprNode::getType(){
    return this->type;
}

void 
ExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitExprNode(this);
}

/**********************************************************************************/
/* The Unary Expression Class                                                     */
/**********************************************************************************/

UnaryExprNode::UnaryExprNode(){
    operand = nullptr;
    opcode = Unset;
}

UnaryExprNode::UnaryExprNode(ExprNode *expr){
    this->operand = expr;
    if(expr != nullptr) expr->setParent(this);
}

UnaryExprNode::UnaryExprNode(ExprNode *expr, Opcode code){
    this->operand = expr;
    if(expr != nullptr) expr->setParent(this);
    opcode = code; 
}

ExprNode* 
UnaryExprNode::getOperand(){
    return operand;
}

void 
UnaryExprNode::setOperand(ExprNode *operand_){
    this->operand = operand_;
    if(operand_ != nullptr) operand_->setParent(this);
}

UnaryExprNode::Opcode 
UnaryExprNode::getOpcode(){
    return opcode;
}

void 
UnaryExprNode::setOpcode(UnaryExprNode::Opcode code){
    opcode = code;
}

ExprNode::Opcode
strToCode(std::string code){
    ExprNode::Opcode res = ExprNode::Unset;
    if(code == "+"){
        res = ExprNode::Addition;
    }else if(code == "-"){
        res = ExprNode::Subtraction;
    }else if(code == "*"){
        res = ExprNode::Multiplication;
    }else if(code == "/"){
        res = ExprNode::Division;
    }else if(code == "!"){
        res = ExprNode::Not;
    }else if(code == "-"){
        res = ExprNode::Minus;
    }else if(code == "&&"){
        res = ExprNode::And;
    }else if(code == "||"){
        res = ExprNode::Or;
    }else if(code == "=="){
        res = ExprNode::Equal;
    }else if(code == "!="){
        res = ExprNode::NotEqual;
    }else if(code == "<"){
        res = ExprNode::LessThan;
    }else if(code == "<="){
        res = ExprNode::LessorEqual;
    }else if(code == ">"){
        res = ExprNode::Greater;
    }else if(code == ">="){
        res = ExprNode::GreaterorEqual;
    }
    return res;
} 

void
UnaryExprNode::setOpcode(std::string code){
    opcode = strToCode(code);
}

void 
UnaryExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitUnaryExprNode(this);
}

/**********************************************************************************/
/* The Binary Expression Class                                                    */
/**********************************************************************************/

BinaryExprNode::BinaryExprNode(){
    left = nullptr;
    right = nullptr;
    opcode = Unset;
}

BinaryExprNode::BinaryExprNode(ExprNode *l, ExprNode *r){
    left = l;
    if(l != nullptr) l->setParent(this);
    right = r;
    if(r != nullptr) r->setParent(this);
    opcode = Unset;
}

BinaryExprNode::BinaryExprNode(ExprNode *l, ExprNode *r, Opcode code){
    left = l;
    if(l != nullptr) l->setParent(this);
    right = r;
    if(r != nullptr) r->setParent(this);
    opcode = code;
}

ExprNode*
BinaryExprNode::getLeft(){
    return left;
}

void
BinaryExprNode::setLeft(ExprNode *l){
    left = l;
    if(l != nullptr) l->setParent(this);
}

ExprNode*
BinaryExprNode::getRight(){
    return right;
}

void
BinaryExprNode::setRight(ExprNode* r){
    right = r;
    if(r != nullptr) r->setParent(this);
}

BinaryExprNode::Opcode 
BinaryExprNode::getOpcode(){
    return opcode;
}

void 
BinaryExprNode::setOpcode(BinaryExprNode::Opcode code){
    opcode = code;
}

void
BinaryExprNode::setOpcode(std::string code){
    opcode = strToCode(code);
}

void
BinaryExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitBinaryExprNode(this);
}

/**********************************************************************************/
/* The Boolean Expression Class                                                   */
/**********************************************************************************/

BoolExprNode::BoolExprNode(){
    this->value = nullptr;
    this->setTypeBool();
}

BoolExprNode::BoolExprNode(ExprNode *val){
    this->value = val;
    if(val != nullptr) val->setParent(this);
    this->setTypeBool();
}

ExprNode*
BoolExprNode::getValue(){
    return this->value;
}

void
BoolExprNode::setValue(ExprNode *val){
    this->value = val;
    if(val != nullptr) val->setParent(this);
}

BoolExprNode::Opcode 
BoolExprNode::getOpcode(){
    BinaryExprNode* cast_bi = dynamic_cast<BinaryExprNode*>(this->value);
    if(cast_bi != nullptr) return cast_bi->getOpcode();
    
    UnaryExprNode* cast_un = dynamic_cast<UnaryExprNode*>(this->value);
    if(cast_un != nullptr) return cast_un->getOpcode();
}

void
BoolExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitBoolExprNode(this);
}

/**********************************************************************************/
/* The Integer Expression Class                                                   */
/**********************************************************************************/

IntExprNode::IntExprNode(){
    this->value = nullptr;
}

IntExprNode::IntExprNode(ExprNode *val){
    this->value = val;
    if(val != nullptr) val->setParent(this);
    this->setType(val->getType());
}

ExprNode*
IntExprNode::getValue(){
    return this->value;
}

void
IntExprNode::setValue(ExprNode *val){
    this->value = val;
    if(val != nullptr) val->setParent(this);
}

IntExprNode::Opcode 
IntExprNode::getOpcode(){
    BinaryExprNode* cast_bi = dynamic_cast<BinaryExprNode*>(this->value);
    if(cast_bi != nullptr) return cast_bi->getOpcode();
    
    UnaryExprNode* cast_un = dynamic_cast<UnaryExprNode*>(this->value);
    if(cast_un != nullptr) return cast_un->getOpcode();
}

void
IntExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitIntExprNode(this);
}

/**********************************************************************************/
/* The Constant Class                                                             */
/**********************************************************************************/

ConstantExprNode::ConstantExprNode(){

}

ConstantExprNode::ConstantExprNode(const std::string &source_){
    this->source = source_;
}

void
ConstantExprNode::setSource(const std::string &source_){
    this->source = source_;
}

void
ConstantExprNode::setVal(int value){
    this->val = value;
}

int
ConstantExprNode::getVal(){
    return this->val;
}

void
ConstantExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitConstantExprNode(this);
}

/**********************************************************************************/
/* The Boolean Constant Class                                                     */
/**********************************************************************************/

BoolConstantNode::BoolConstantNode(const std::string &source){
    this->setSource(source);
    this->setTypeBool();
}

void
BoolConstantNode::visit(ASTVisitorBase* visitor){
    visitor->visitBoolConstantNode(this);
}

/**********************************************************************************/
/* The Integer Constant Class                                                     */
/**********************************************************************************/

IntConstantNode::IntConstantNode(const std::string &source){
    this->setSource(source);
    this->setTypeInt();
}

void 
IntConstantNode::visit(ASTVisitorBase* visitor){
    visitor->visitIntConstantNode(this);
}

/**********************************************************************************/
/* The Function Argument Class                                                    */
/**********************************************************************************/

ArgumentNode::ArgumentNode(){
    this->expr = nullptr;
}

ArgumentNode::ArgumentNode(ExprNode *expr){
    this->expr = expr;
    if(expr != nullptr) expr->setParent(this);
}

ExprNode*
ArgumentNode::getExpr(){
    return this->expr;
}

void 
ArgumentNode::setExpr(ExprNode *expr_){
    this->expr = expr_;
    if(expr_ != nullptr) expr_->setParent(this);
}

void 
ArgumentNode::visit(ASTVisitorBase* visitor){
    visitor->visitArgumentNode(this);
}

/**********************************************************************************/
/* The Call Expression Class                                                      */
/**********************************************************************************/

CallExprNode::CallExprNode(){
    this->name = nullptr;
}

CallExprNode::CallExprNode(IdentifierNode *callee){
    this->name = callee;
    if(callee != nullptr) callee->setParent(this);
}

CallExprNode::CallExprNode(IdentifierNode *callee, std::vector<ArgumentNode*> arglist){
    this->name = callee;
    if(callee != nullptr) callee->setParent(this);
    this->args = arglist;
    for(auto arg : arglist){
        arg->setParent(this);
    }
}

ArgumentNode * 
CallExprNode::getArgument(unsigned int i){
    return this->args[i];
}

std::vector<ArgumentNode *> 
CallExprNode::getArguments(){
    return this->args;
}

void 
CallExprNode::addArgument(ArgumentNode *arg){
    this->args.push_back(arg);
    if(arg != nullptr) arg->setParent(this);
}

void 
CallExprNode::setArguments(std::vector<ArgumentNode *> args_){
    this->args = args_;
    for(auto arg : args_){
        arg->setParent(this);
    }
}

void 
CallExprNode::setIdent(IdentifierNode *callee){
    this->name = callee;
    if(callee != nullptr) callee->setParent(this);
}

IdentifierNode * 
CallExprNode::getIdent(){
    return this->name;
}

void 
CallExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitCallExprNode(this);
}

/**********************************************************************************/
/* The Reference Expression Class                                                 */
/**********************************************************************************/

ReferenceExprNode::ReferenceExprNode(){
    this->name = nullptr;
    this->index = nullptr;
}

ReferenceExprNode::ReferenceExprNode(IdentifierNode *name_){
    this->name = name_;
    if(name_ != nullptr) name_->setParent(this);
    this->index = nullptr;
}

ReferenceExprNode::ReferenceExprNode(IdentifierNode *name_, IntExprNode *exp){
    this->name = name_;
    if(name_ != nullptr) name_->setParent(this);
    this->index = exp;
    if(exp != nullptr) exp->setParent(this);
}

IdentifierNode*
ReferenceExprNode::getIdent(){
    return this->name;
}

void 
ReferenceExprNode::setIdent(IdentifierNode *name_){
    this->name = name_;
    if(name_ != nullptr) name_->setParent(this);
}

void
ReferenceExprNode::setIndex(IntExprNode *index_){
    this->index = index_;
    if(index_ != nullptr) index_->setParent(this);
}

IntExprNode* 
ReferenceExprNode::getIndex(){
    return this->index;
}

void
ReferenceExprNode::visit(ASTVisitorBase* visitor){
    visitor->visitReferenceExprNode(this);
}

/**********************************************************************************/
/* The Declaration Class                                                          */
/**********************************************************************************/

DeclNode::DeclNode(){
    this->type = nullptr;
    this->name = nullptr;
}

DeclNode::DeclNode(TypeNode* type_, IdentifierNode* name_){
    this->type = type_;
    if(type_ != nullptr) type_->setParent(this);
    this->name = name_;
    if(name_ != nullptr) name_->setParent(this);
}

void
DeclNode::setName(IdentifierNode* name_){
    this->name = name_;
    if(name_ != nullptr) name_->setParent(this);
}

void 
DeclNode::setType(TypeNode* type_){
    this->type = type_;
    if(type_ != nullptr) type_->setParent(this);
}

IdentifierNode*
DeclNode::getIdent(){
    return this->name;
}

TypeNode* 
DeclNode::getType(){
    return this->type;
}

bool 
DeclNode::isGlobal(){
    ProgramNode * node = dynamic_cast<ProgramNode*>(this->getParent());
    if(node == nullptr) return false;
    else return true; 
}

void 
DeclNode::visit(ASTVisitorBase* visitor){
    visitor->visitDeclNode(this);
}

/**********************************************************************************/
/* The Scalar Declaration Class                                                   */
/**********************************************************************************/

ScalarDeclNode::ScalarDeclNode(){
    this->setType(nullptr);
    this->setName(nullptr);
}

ScalarDeclNode::ScalarDeclNode(PrimitiveTypeNode*& type_, IdentifierNode*& name_){
    this->setType(type_);
    this->setName(name_);
}

PrimitiveTypeNode*
ScalarDeclNode::getType(){
    PrimitiveTypeNode* node = dynamic_cast<PrimitiveTypeNode*>(this->DeclNode::getType());
    return node;
}

void
ScalarDeclNode::visit(ASTVisitorBase* visitor){
    visitor->visitScalarDeclNode(this);
}
    
/**********************************************************************************/
/* The Array Declaration Class                                                    */
/**********************************************************************************/

ArrayDeclNode::ArrayDeclNode(){
    this->setType(nullptr);
    this->setName(nullptr);
}

ArrayDeclNode::ArrayDeclNode(ArrayTypeNode* type_, IdentifierNode* name_){
    this->setType(type_);
    this->setName(name_);
}

ArrayTypeNode*
ArrayDeclNode::getType(){
    ArrayTypeNode* node = dynamic_cast<ArrayTypeNode*>(this->DeclNode::getType());
    return node;
}

void 
ArrayDeclNode::visit(ASTVisitorBase* visitor){
    visitor->visitArrayDeclNode(this);
}

/**********************************************************************************/
/* The Stmt Class                                                                 */
/**********************************************************************************/

StmtNode::StmtNode(){
    
}

void 
StmtNode::visit(ASTVisitorBase* visitor){
    visitor->visitStmtNode(this);
}

/**********************************************************************************/
/* The Scope Class                                                                */
/**********************************************************************************/

ScopeNode::ScopeNode(){
    this->env = new SymTable<VariableEntry>();
}

void
ScopeNode::addDeclaration(DeclNode *decl){
    decls.push_back(decl);
    if(decl != nullptr) decl->setParent(this);
}

std::vector<DeclNode*> 
ScopeNode::getDeclarations(){
    return this->decls;
}

SymTable<VariableEntry>* 
ScopeNode::getVarTable(){
    return this->env;
}

bool ScopeNode::hasVarTable(){
    return env != nullptr;
}

void 
ScopeNode::visit(ASTVisitorBase* visitor){
    visitor->visitScopeNode(this);
}

/**********************************************************************************/
/* The Function Declaration Class                                                 */
/**********************************************************************************/

FunctionDeclNode::FunctionDeclNode(){
    body = nullptr;
}

void 
FunctionDeclNode::setProto(bool val){
    this->isProto = val;
}

void
FunctionDeclNode::setBody(ScopeNode* val){
    this->body = val;
    if(val != nullptr) val->setParent(this);
}

void
FunctionDeclNode::setRetType(PrimitiveTypeNode* type){
    this->setType(type);
    if(type != nullptr) type->setParent(this);
}

void 
FunctionDeclNode::setParameter(std::vector<ParameterNode*> parameters){
    this->params = parameters;
    for(auto param : parameters){
        param->setParent(this);
    }
}

void
FunctionDeclNode::addParameter(ParameterNode* param){
    this->params.push_back(param);
    if(param != nullptr) param->setParent(this);
}

bool
FunctionDeclNode::getProto(){
    return isProto;
}

ScopeNode* 
FunctionDeclNode::getBody(){
    return this->body;
}

PrimitiveTypeNode*
FunctionDeclNode::getRetType(){
    PrimitiveTypeNode * node = dynamic_cast<PrimitiveTypeNode*>(this->DeclNode::getType()); 
    // TODO: Correct?
    return node;
}

std::vector<ParameterNode*> 
FunctionDeclNode::getParams(){
    return this->params;
}

unsigned int 
FunctionDeclNode::getNumParameters(){
    return this->params.size();
}

PrimitiveTypeNode *
FunctionDeclNode::getType(){
    PrimitiveTypeNode * node = dynamic_cast<PrimitiveTypeNode*>(this->DeclNode::getType());
    return node;
}

void 
FunctionDeclNode::visit(ASTVisitorBase* visitor){
    visitor->visitFunctionDeclNode(this);
}

/**********************************************************************************/
/* The Expression Statement Class                                                 */
/**********************************************************************************/

ExprStmtNode::ExprStmtNode(){
    this->expr = nullptr;
}

ExprStmtNode::ExprStmtNode(ExprNode* exp){
    this->expr = exp;
    if(exp != nullptr) exp->setParent(this);
}

void 
ExprStmtNode::setExpr(ExprNode* expr_){
    this->expr = expr_;
    if(expr_ != nullptr) expr_->setParent(this);
}

ExprNode*
ExprStmtNode::getExpr(){
    return this->expr;
}

void
ExprStmtNode::visit(ASTVisitorBase* visitor){
    visitor->visitExprStmtNode(this);
}

/**********************************************************************************/
/* The Assignment Statement Class                                                 */
/**********************************************************************************/

AssignStmtNode::AssignStmtNode(){
    target = nullptr;
    val = nullptr;
}

AssignStmtNode::AssignStmtNode(ReferenceExprNode* target_, ExprNode* val_){
    this->target = target_;
    if(target_ != nullptr) target_->setParent(this);
    this->val = val_;
    if(val_ != nullptr) val_->setParent(this);
}

ReferenceExprNode *
AssignStmtNode::getTarget(){
    return this->target;
}

void
AssignStmtNode::setTarget(ReferenceExprNode* name){
    this->target = name;
    if(name != nullptr) name->setParent(this);
}

ExprNode* 
AssignStmtNode::getValue(){
    return this->val;
}  

void 
AssignStmtNode::setValue(ExprNode* value){
    this->val = value;
    if(value != nullptr) value->setParent(this);
}

void 
AssignStmtNode::visit(ASTVisitorBase* visitor){
    visitor->visitAssignStmtNode(this);
}

/**********************************************************************************/
/* The If Statement Class                                                         */
/**********************************************************************************/

IfStmtNode::IfStmtNode(){
    this->condition = nullptr;
    this->Then = nullptr;
    this->Else = nullptr;
    this->setHasElse(false);
}

IfStmtNode::IfStmtNode(ExprNode* cond, StmtNode* then_){
    this->condition = cond;
    if(cond != nullptr) cond->setParent(this);
    this->Then = then_;
    if(then_ != nullptr) then_->setParent(this);
    this->Else = nullptr;
    this->setHasElse(false);
}

IfStmtNode::IfStmtNode(ExprNode* cond, StmtNode* then_, StmtNode* else_){
    this->condition = cond;
    if(cond != nullptr) cond->setParent(this);
    this->Then = then_;
    if(then_ != nullptr) then_->setParent(this);
    this->Else = else_;
    if(else_ != nullptr) else_->setParent(this);
    this->setHasElse(true);
}

ExprNode* 
IfStmtNode::getCondition(){
    return this->condition;
}

void 
IfStmtNode::setCondition(ExprNode* condition_){
    this->condition = condition_;
    if(condition_ != nullptr) condition_->setParent(this);
}

bool 
IfStmtNode::getHasElse(){
    return this->hasElse;
}

void 
IfStmtNode::setHasElse(bool hasElse_){
    this->hasElse = hasElse_;
}

StmtNode *
IfStmtNode::getThen(){
    return this->Then;
}

void 
IfStmtNode::setThen(StmtNode* then_){
    this->Then = then_;
    if(then_ != nullptr) then_->setParent(this);
}

StmtNode *
IfStmtNode::getElse(){
    return this->Else;
}

void 
IfStmtNode::setElse(StmtNode* else_){
    this->Else = else_;
    this->setHasElse(true);
    if(else_ != nullptr) else_->setParent(this);
}

void 
IfStmtNode::visit(ASTVisitorBase* visitor){
    visitor->visitIfStmtNode(this);
}

/**********************************************************************************/
/* The While Statement Class                                                      */
/**********************************************************************************/

WhileStmtNode::WhileStmtNode(){
    this->condition = nullptr;
    this->body = nullptr;
}

WhileStmtNode::WhileStmtNode(ExprNode* cond, StmtNode* body_){
    this->condition = cond;
    if(cond != nullptr) cond->setParent(this);
    this->body = body_;
    if(body_ != nullptr) body_->setParent(this);
}

ExprNode* 
WhileStmtNode::getCondition(){
    return this->condition;
}

void 
WhileStmtNode::setCondition(ExprNode* cond){
    this->condition = cond;
    if(cond != nullptr) cond->setParent(this);
}

StmtNode* 
WhileStmtNode::getBody(){
    return this->body;
}

void 
WhileStmtNode::setBody(StmtNode* body_){
    this->body = body_;
    if(body_ != nullptr) body_->setParent(this);
}

void 
WhileStmtNode::visit(ASTVisitorBase* visitor){
    visitor->visitWhileStmtNode(this);
}

/**********************************************************************************/
/* The Return Statement Class                                                     */
/**********************************************************************************/

ReturnStmtNode::ReturnStmtNode(){
    this->ret = nullptr;
}

ReturnStmtNode::ReturnStmtNode(ExprNode* exp){
    this->ret = exp;
    if(exp != nullptr) exp->setParent(this);
}

ExprNode* 
ReturnStmtNode::getReturn(){
    return this->ret;
}

void 
ReturnStmtNode::setReturn(ExprNode* value){
    this->ret = value;
    if(value != nullptr) value->setParent(this);
}

bool
ReturnStmtNode::returnVoid(){
    return this->ret == nullptr;
}

void 
ReturnStmtNode::visit(ASTVisitorBase* visitor){
    visitor->visitReturnStmtNode(this);
}

// this expects the decls production to return an attribute called "declarations", 
// which you have to define and return manually in the decls production.