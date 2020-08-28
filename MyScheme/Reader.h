#pragma once
#include <string>
#include "ScmObject.h"

using namespace std;

class Reader
{
private:
	ScmObject* readLambda(string& _remainingInput);

	ScmObject* readFunctionKeyword(string& _remainingInput);

public:
	ScmObject* ReadNextSymbol(string& _remainingInput);
};

