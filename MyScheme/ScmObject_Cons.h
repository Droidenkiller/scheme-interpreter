#pragma once
#include "ScmObject.h"
#include <string>
#include <memory>

class ScmObject_Cons :
    public ScmObject
{
private:
    std::shared_ptr<const ScmObject> m_car;
    std::shared_ptr<const ScmObject> m_cdr;

    std::string const getOutputStringInternal() const;

public:
    ScmObject_Cons(std::shared_ptr<const ScmObject> _car, std::shared_ptr<const ScmObject> _cdr);

    std::shared_ptr<const ScmObject> car() const;
    std::shared_ptr<const ScmObject> cdr() const;

    bool equals(const ScmObject* _other) const;

    std::shared_ptr<const ScmObject> copy() const;

    std::string const getOutputString() const;

    const std::string getDisplayString() const;
};

