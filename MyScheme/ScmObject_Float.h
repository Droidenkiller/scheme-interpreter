#pragma once
#include "ScmObject.h"
#include <string>

class ScmObject_Float :
    public ScmObject
{
private:
    double m_value;

public:
    ScmObject_Float(std::string _floatLiteral);

    ScmObject_Float(double _value);

    double const getValue() const;

    bool equals(const ScmObject* _other) const;

    ScmObject* copy() const;

    std::string const getOutputString() const;
};

