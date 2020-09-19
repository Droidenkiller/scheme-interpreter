#include "ScmObject_Cons.h"

ScmObject_Cons::ScmObject_Cons(std::shared_ptr<const ScmObject> _car, std::shared_ptr<const ScmObject> _cdr) : ScmObject(ScmObjectType::CONS)
{
	m_car = _car;
	m_cdr = _cdr;
}

std::shared_ptr<const ScmObject> ScmObject_Cons::car() const
{
	return m_car;
}

std::shared_ptr<const ScmObject> ScmObject_Cons::cdr() const
{
	return m_cdr;
}

bool ScmObject_Cons::equals(const ScmObject* _other) const
{
	return this == &*_other;
}

std::shared_ptr<const ScmObject> ScmObject_Cons::copy() const
{
	// Deep copy not required, as the returned instance is const anyways and therefore cannot be changed.
	return std::make_shared<const ScmObject_Cons>(*this);
}

const std::string ScmObject_Cons::getOutputString() const
{
	if (m_car == nullptr)
	{
		return "()";
	}
	else
	{
		if (m_cdr != nullptr)
		{
			if (m_cdr->getType() == ScmObjectType::CONS)
			{
				return "(" + m_car->getOutputString() + " " + std::static_pointer_cast<const ScmObject_Cons>(m_cdr)->getOutputStringInternal() + ")";
			}
			else
			{
				return "(" + m_car->getOutputString() + " . " + m_cdr->getOutputString() + ")";
			}
		}
		else
		{
			return "(" + m_car->getOutputString() + ")";
		}
	}
}

const std::string ScmObject_Cons::getOutputStringInternal() const
{
	if (m_car == nullptr)
	{
		return "()";
	}
	else
	{
		if (m_cdr != nullptr)
		{
			if (m_cdr->getType() == ScmObjectType::CONS)
			{
				return m_car->getOutputString() + " " + std::static_pointer_cast<const ScmObject_Cons>(m_cdr)->getOutputStringInternal();
			}
			else
			{
				return m_car->getOutputString() + " . " + m_cdr->getOutputString();
			}
		}
		else
		{
			return m_car->getOutputString();
		}
	}
}

const std::string ScmObject_Cons::getDisplayString() const
{
	return getOutputString();
}
