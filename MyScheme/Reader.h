#pragma once
#include <string>
#include "ScmObject.h"
#include <memory>

using namespace std;

class Reader
{
private:
	static shared_ptr<ScmObject> readLambda(string& _remainingInput);

	static shared_ptr<ScmObject> readFunctionKeyword(string& _remainingInput);

	Reader() {}

public:
	static shared_ptr<ScmObject> ReadNextSymbol(string& _remainingInput);
};

