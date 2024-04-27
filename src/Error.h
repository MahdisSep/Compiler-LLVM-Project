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
	static void AssignmentSidesNotEqual();
	static void VariableNameNotFound();
	static void BooleanValueExpected();
	static void NumberVariableExpected();


};

#endif
