#pragma once
#include <string>
#include <memory>

enum class ScmObjectType
{
	NIL,
	FUNCTION_CALL,
	FUNCTION_DEFINITION,
	FUNCTION_EXECUTION,
	SYMBOL,
	QUOTE_SYMBOL,
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

	virtual std::shared_ptr<const ScmObject> copy() const = 0;

	virtual std::string const getOutputString() const = 0;

	virtual std::string const getDisplayString() const = 0;
};

