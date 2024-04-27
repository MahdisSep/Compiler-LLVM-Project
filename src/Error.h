#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include "Lexer.h"
using namespace std;

class Error {

public:
	static void SemiColonNotFound();
	static void DefineInsideScope();
	static void AssignmentEqualNotFound();


};

#endif
