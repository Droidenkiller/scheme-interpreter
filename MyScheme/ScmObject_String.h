#pragma once
#include "ScmObject.h"
#include <string>
#include <memory>

class ScmObject_String :
    public ScmObject
{
private:
    std::string m_value;

public:
    ScmObject_String(std::string _value);

    const std::string* getValue() const;

    void setValue(std::string _value);

    bool equals(const ScmObject* _other) const;

    std::shared_ptr<const ScmObject> copy() const;

    std::string const getOutputString() const;

    const std::string getDisplayString() const;
};

