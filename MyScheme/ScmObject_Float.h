#pragma once
#include "ScmObject.h"
#include <string>
#include <memory>

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

    std::shared_ptr<const ScmObject> copy() const;

    std::string const getOutputString() const;

    const std::string getDisplayString() const;
};

