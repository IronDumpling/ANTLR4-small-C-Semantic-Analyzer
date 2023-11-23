//
//  A3Sema.cpp
//  ECE467 Lab 3
//
//  Created by Tarek Abdelrahman on 2023-07-31.
//  Modified by Tarek Abdelrahman on 2023-09-27.
//  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
//
//  Permission is hereby granted to use this code in ECE467 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "antlr4-runtime.h"
#include "smallCLexer.h"
#include "smallCParser.h"
#include "ASTVisitorBase.h"
#include "ASTPrinter.h"
#include "SemanticAnalyzer.h"

using namespace antlrcpp;
using namespace antlr4;
using namespace std;
using namespace smallc;

int main(int argc, const char *argv[]) {
    // Check for the number of arguments
    if (argc != 2) {
	    cerr << "Usage: " << argv[0] << " filename" << std::endl;
	    return -1;
    }

    // Input stream handler
    ifstream inputStream;

    // Open the input file
    inputStream.open(argv[1]);
    if (!inputStream) {
    	cerr << "fatal: " << argv[1] << " not found or cannot be opened" << std::endl;
	    return -1;
    }

    // Create the input stream to the lexer
    ANTLRInputStream input(inputStream);
   
    // Create a lexer which scans the input stream
    // to create a token stream.
    smallCLexer* lexer = new smallCLexer(&input);
    CommonTokenStream tokens(lexer);

    // Get the tokens
    tokens.fill();

    // Create a parser 
    smallCParser* parser = new smallCParser(&tokens);

    // Invoke the parser and get the root of the AST, i.e., the ProgramNode
    ProgramNode* prg = parser->program()->prg;

    // Uncomment these lines to print the AST tree using the provided ASTPrinter class
    // ASTPrinter *printer = new smallc::ASTPrinter();
    // if (parser->getNumberOfSyntaxErrors() == 0) {
        // printer->visitProgramNode(prg);
    // }
    // else {
        // cout << "cannot print AST with parse errors\n";
    // }

    // ECE467 STUDENT: add code to create your semantic analyzer object
    if(parser->getNumberOfSyntaxErrors() != 0){
        cout << "cannot perform semantic checks with parse errors\n";
        return 0;
    }

    SemanticAnalyzer* sema = new smallc::SemanticAnalyzer();

    // invoke it by by visiting the program node
    sema->visitProgramNode(prg);

    // Once done, check if there are error messages and print them
    if(!sema->success()) sema->printErrorMsgs();
        
    return 0;
}

