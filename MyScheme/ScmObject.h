#pragma once
#include <string>

enum class ScmObjectType
{
	NIL,
	FUNCTION_CALL,
	FUNCTION_DEFINITION,
	FUNCTION_EXECUTION,
	SYMBOL,
	INTERNAL_ERROR,
	CONS,
	STRING,
	INT,
	FLOAT,
	BOOL
};

class ScmObject
{
private:
	ScmObjectType type;

protected:
	ScmObject(ScmObjectType);

public:
	const ScmObjectType getType() const;

	virtual bool equals(const ScmObject* _other) const = 0;

	virtual ScmObject* copy() const = 0;

	virtual std::string const getOutputString() const = 0;
};

