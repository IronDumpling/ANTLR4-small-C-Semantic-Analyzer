# Makefile 
# ECE467 Lab 3
#
#  Created by Tarek Abdelrahman on 2023-09-13.
#  Copyright © 2023 Tarek Abdelrahman. All rights reserved.
#
#  Permission is hereby granted to use this code in ECE467 at
#  the University of Toronto. It is prohibited to distribute
#  this code, either publicly or to third parties.

SHELL := /bin/bash

ANTLR_VER     = 4.7.2
ECE467_ROOT   = /cad2/ece467f
ANTLR_INC_DIR = $(ECE467_ROOT)/ANTLR-$(ANTLR_VER)/include/antlr4-runtime/
ANTLR_LIB_DIR = $(ECE467_ROOT)/ANTLR-$(ANTLR_VER)/lib

CC            = g++ 
CC_OPT        = -std=c++17 -w

ANTLR         = java -jar $(ECE467_ROOT)/ANTLR-$(ANTLR_VER)/antlr-$(ANTLR_VER)-complete.jar
ANTLR_OPTS    = -no-listener -visitor -Dlanguage=Cpp

TARGET        = smallC
EXE           = A3Sema

GEN_SRCS      = $(TARGET)Lexer.cpp  $(TARGET)Parser.cpp $(TARGET)BaseVisitor.cpp \
                $(TARGET)Visitor.cpp 

GEN_OBJS      = $(patsubst %.cpp,%.o,$(GEN_SRCS))

GEN_INCS      = $(patsubst %.cpp,%.h,$(GEN_SRCS))

GEN_OTHR      = $(TARGET).interp $(TARGET).tokens $(TARGET)Lexer.interp $(TARGET)Lexer.tokens

SRCS          = $(EXE).cpp ASTNodes.cpp ASTVisitorBase.cpp ASTPrinter.cpp \
                SymTable.cpp SemanticAnalyzer.cpp
OBJS          = $(patsubst %.cpp,%.o,$(SRCS))
INCS          = $(patsubst %.cpp,%.h,$(SRCS))

$(EXE):	$(OBJS) $(GEN_OBJS)
	$(CC) $(CC_OPT) -I$(ANTLR_INC_DIR) -L$(ANTLR_LIB_DIR) $(OBJS) \
	          $(GEN_OBJS) -o $(EXE) -lantlr4-runtime

$(OBJS):	%.o:	%.cpp $(GEN_INCS)
	$(CC) $(CC_OPT) -c -I$(ANTLR_INC_DIR) -o $@ $<
	
$(GEN_OBJS):	%.o:	%.cpp %.h
	$(CC) $(CC_OPT) -c -I$(ANTLR_INC_DIR) -o $@ $<

$(GEN_INCS):	$(TARGET).g4
	$(ANTLR) $(ANTLR_OPTS)  $(TARGET).g4

$(GEN_SRCS):	$(TARGET).g4
	$(ANTLR) $(ANTLR_OPTS)  $(TARGET).g4

depend:
	@makedepend -- $(CC_OPT) -I$(ANTLR_INC_DIR) -L$(ANTLR_LIB_DIR) -- \
		                               $(SRCS) $(GEN_SRCS) >& /dev/null

.PHONY: clean
clean:
	@rm -f $(GEN_SRCS) $(GEN_INCS) $(GEN_OBJS) $(GEN_OTHR) $(OBJS) $(EXE) Makefile.bak

