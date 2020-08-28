#pragma once
#include "ScmObject.h"
#include <string>

class ScmObject_Integer :
    public ScmObject
{
private:
    long long m_value;

public:
    ScmObject_Integer(std::string _intLiteral);

    ScmObject_Integer(long long _value);

    long long const getValue() const;

    bool equals(const ScmObject* _other) const;

    ScmObject* copy() const;

    std::string const getOutputString() const;
};

