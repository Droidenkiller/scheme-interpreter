#include "Selftest.h"
#include "ScmObjectIncludes.h"
#include <string>
#include <iostream>

using namespace std;

bool checkFloat(string& _input, double _expectedValue)
{
	bool result = false;
	shared_ptr<ScmObject> object = Reader::ReadNextSymbol(_input);

	if (object != nullptr)
	{
		switch (object->getType())
		{
		case ScmObjectType::FLOAT:
		{
			double value = static_pointer_cast<ScmObject_Float>(object)->getValue();
			if (abs(value - _expectedValue) < numeric_limits<float>::epsilon())
			{
				cout << "Float with expected value of " << _expectedValue << " passed parsing test.\n";
				result = true;
			}
			else
			{
				cerr << "Parsing of float failed. Value did not match expected value."
					" Expected: " << _expectedValue << ", got: " << value << endl;
			}
		}
		break;
		case ScmObjectType::INTERNAL_ERROR:
			cerr << "Internal error on float with value: " << _expectedValue << ". Error: " <<
				static_pointer_cast<ScmObject_InternalError>(object)->getMessage() << endl;
			break;
		default:
			cerr << "Float with expected value of " << _expectedValue << " was parsed to wrong type.\n";
			break;
		}
	}
	else
	{
		cerr << "Got nullptr from parsing. Expected value was: " << _expectedValue << endl;
	}

	return result;
}

bool testFloats()
{
	bool result = true;
	string correctFormats = "-2.33 234.164,.45  , ,,  -32453.2351";
	double numbers[] = { -2.33, 234.164, 0.45, -32453.2351 };

	for (int i = 0; i < 4; ++i)
	{
		if (!checkFloat(correctFormats, numbers[i]))
		{
			result = false;
		}
	}

	return result;
}

bool checkInteger(string& _input, long long _expectedValue)
{
	bool result = false;
	shared_ptr<ScmObject> object = Reader::ReadNextSymbol(_input);

	if (object != nullptr)
	{
		switch (object->getType())
		{
		case ScmObjectType::INT:
		{
			long long value = static_pointer_cast<ScmObject_Integer>(object)->getValue();
			if (value == _expectedValue)
			{
				cout << "Integer with expected value of " << _expectedValue << " passed parsing test.\n";
				result = true;
			}
			else
			{
				cerr << "Parsing of integer failed. Value did not match expected value."
					" Expected: " << _expectedValue << ", got: " << value << endl;
			}
		}
		break;
		case ScmObjectType::INTERNAL_ERROR:
			cerr << "Internal error on integer with value: " << _expectedValue << ". Error: " <<
				static_pointer_cast<ScmObject_InternalError>(object)->getMessage() << endl;
			break;
		default:
			cerr << "Integer with expected value of " << _expectedValue << " was parsed to wrong type.\n";
			break;
		}
	}
	else
	{
		cerr << "Got nullptr from parsing. Expected value was: " << _expectedValue << endl;
	}

	return result;
}

bool testIntegers()
{
	bool result = true;
	string correctFormats = "-233 234164,045  , ,,  -324532351, 0";
	double numbers[] = { -233, 234164, 45, -324532351, 0 };

	for (int i = 0; i < 5; ++i)
	{
		if (!checkInteger(correctFormats, numbers[i]))
		{
			result = false;
		}
	}

	return result;
}

bool testScheme()
{
	bool result = true;

	if (!testFloats())
	{
		result = false;
	}
	if (!testIntegers())
	{
		result = false;
	}

	return result;
}