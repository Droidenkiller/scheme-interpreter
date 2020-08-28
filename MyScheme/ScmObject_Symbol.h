#pragma once
#include "ScmObject.h"
#include <string>

class ScmObject_Symbol :
    public ScmObject
{
private:
    std::string m_name;

public:
    ScmObject_Symbol(std::string _name);

    const std::string* getName() const;

    bool equals(const ScmObject* _other) const;

    ScmObject* copy() const;

    std::string const getOutputString() const;
};

