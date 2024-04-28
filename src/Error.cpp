#include "Error.h"

void Error::SemiColonNotFound()
{
	cout << "Semicolon not found...\n";
	exit(3);
}
void Error::DefineInsideScope()
{
	cout << "Can't define variable inside scope...\n";
	exit(3);
}

void Error::AssignmentEqualNotFound()
{
	cout << "Assignment does not have a '=' character...\n";
	exit(3);
}

