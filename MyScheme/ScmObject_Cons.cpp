#include "ScmObject_Cons.h"

ScmObject_Cons::ScmObject_Cons() : ScmObject(ScmObjectType::CONS)
{
	setCar(nullptr);
	setCdr(nullptr);
}

const ScmObject* ScmObject_Cons::getCar() const
{
	return car;
}

const ScmObject* ScmObject_Cons::getCdr() const
{
	return cdr;
}

void ScmObject_Cons::setCar(ScmObject* _car)
{
	car = _car;
}

void ScmObject_Cons::setCdr(ScmObject* _cdr)
{
	cdr = _cdr;
}

bool ScmObject_Cons::equals(const ScmObject* _other) const
{
	// TODO: Implement this.
	throw new std::exception("Not yet implemented.");
}

ScmObject* ScmObject_Cons::copy() const
{
	return new ScmObject_Cons(*this);
}

const std::string ScmObject_Cons::getOutputString() const
{
	if (cdr != nullptr)
	{
		return "(" + car->getOutputString() + ", " + cdr->getOutputString() + ")";
	}
	else
	{
		return "(" + car->getOutputString() + ")";
	}
}
