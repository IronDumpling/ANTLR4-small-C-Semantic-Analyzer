//
//  SymTable.cpp
//  ECE467 Lab 3
//
//  Created by Tarek Abdelrahman on 2023-09-13.
//  Based on code written by Ao (Dino) Li for ECE467.
//  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
//
//  Permission is hereby granted to use this code in ECE467 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.

#include <map>
#include <string>

#include "ASTNodes.h"
#include "SymTable.h"

using namespace smallc;

namespace smallc {

/**********************************************************************************/
/* The VariableEntry Class                                                        */
/**********************************************************************************/

VariableEntry::VariableEntry(){
    this->type = nullptr;
    isArray = false;
}

VariableEntry::VariableEntry(PrimitiveTypeNode * p){
    this->type = p;
    isArray = false;
}

VariableEntry::VariableEntry(ArrayTypeNode * arr){
    this->type = arr;
    isArray = true;
}

TypeNode*
VariableEntry::getType(){
    return this->type;
}

bool 
VariableEntry::getIsArray(){
    return isArray;
}

/**********************************************************************************/
/* The FunctionEntry Class                                                        */
/**********************************************************************************/

FunctionEntry::FunctionEntry(){
    returnType = nullptr;
}

FunctionEntry::FunctionEntry(PrimitiveTypeNode* retType, 
                            std::vector<TypeNode*> paraTypes, 
                            bool isProto){
    this->returnType = retType;
    this->parameterTypes = paraTypes;
    this->proto = isProto;
}

PrimitiveTypeNode* 
FunctionEntry::getReturnType(){
    return this->returnType;
}

std::vector<TypeNode*> 
FunctionEntry::getParameterTypes(){
    return this->parameterTypes;
}

bool
FunctionEntry::getProto(){
    return this->proto;
}

/**********************************************************************************/
/* The SymTable Class                                                             */
/**********************************************************************************/

template<class T> bool 
SymTable<T>::contains(const std::string &name){
    return table.find(name) != table.end();
}
 
template<class T> T 
SymTable<T>::get(const std::string &name){
    auto iter = table.find(name);
    return iter->second;
}
 
template<class T> void 
SymTable<T>::insert(const std::string& name, T ent){
    table[name] = ent;
}

// Explicit template class instantiation
template class SymTable<FunctionEntry>;
template class SymTable<VariableEntry>;

} // namespace smallc

