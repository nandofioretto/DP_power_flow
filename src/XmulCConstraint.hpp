//
// Created by Ferdinando Fioretto on 12/6/16.
//

#ifndef CPUBE_CMULXCONSTRAINT_HPP
#define CPUBE_CMULXCONSTRAINT_HPP

#include <vector>
#include <string>

#include "string_utils.hpp"
#include "Types.hpp"
#include "Variable.hpp"
#include "Constraint.hpp"

using namespace misc_utils;

template<class c_t>
class XmultCConstraint : public Constraint
{
public:
    typedef std::shared_ptr<XmultCConstraint<c_t>> ptr;

    XmultCConstraint
            (std::vector<Variable::ptr>& _scope, c_t _const)
            : constant(_const),
              defaultUtil(Constants::unsat), LB(Constants::inf), UB(Constants::inf)
    {
        if ( !Constants::isSat(_const) ) {
            throw std::logic_error("XMultCConstraint: constant (" + std::to_string(_const) + ") must be a finite number");
        }
        if (sizeof(_const) != sizeof(util_t))
            throw std::logic_error("XMultCConstraint: constant (" + std::to_string(_const)
                                   + ") weight type must equal constraint utility/cost type");

        Constraint::scope = _scope;
        for(auto& v : Constraint::scope) {
            scopeAgtID.push_back(v->getAgtID());
        }
    }

    ~XmultCConstraint() { };

    virtual util_t getUtil(std::vector<value_t> &values) override
    {
        return (values[0] * constant);
    }

    virtual void setUtil(std::vector<value_t> &values, util_t util) override
    {

    }

    virtual std::string to_string() override
    {
        std::string ret = "constr XmultC: c=" + std::to_string(constant);
        return ret;
    }

    virtual size_t getSizeBytes() override
    {
        return Constraint::getSizeBytes();
    }

    util_t getDefaultUtil() const
    {
        return defaultUtil;
    }

protected:
    c_t constant;

    // Default util, that is the util associated to any value combination that is
    // not explicitally listed in the relation.
    util_t defaultUtil;

    // The best and worst finite utils of this constraint which is used as bounds
    // in some searches.
    util_t LB;
    util_t UB;
};

#endif //CPUBE_CMULXCONSTRAINT_HPP
