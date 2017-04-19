//
// Created by Ferdinando Fioretto on 11/25/16.
//

#ifndef CPUBE_WSUMCONSTRAINT_HPP
#define CPUBE_WSUMCONSTRAINT_HPP

#include <vector>
#include <string>

#include "string_utils.hpp"
#include "Types.hpp"
#include "Variable.hpp"
#include "Constraint.hpp"
#include "Preferences.hpp"
#include "BooleanExpression.hpp"

using namespace misc_utils;

template<class weight_t>
class WSumConstraint : public Constraint
{
public:
    typedef std::shared_ptr<WSumConstraint<weight_t>> ptr;

    WSumConstraint
    (std::vector<Variable::ptr>& _scope, const std::vector<weight_t>& _weights,
     BooleanExpression _expr, weight_t _rhs_val)
    : weights(_weights), expr(_expr), rhs_val(_rhs_val),
      defaultUtil(Constants::unsat), LB(Constants::inf), UB(Constants::inf)
    {
        Constraint::scope = _scope;
        for(auto& v : Constraint::scope) {
            scopeAgtID.push_back(v->getAgtID());
        }
    }

    ~WSumConstraint() { };

    virtual util_t getUtil(std::vector<value_t> &values) override
    {
        switch(expr)
        {
            case BooleanExpression::EQ:
                return std::abs(wsum(values) - rhs_val) < Preferences::precision ? 0 : Constants::unsat;
            case BooleanExpression::NEQ:
                return std::abs(wsum(values) - rhs_val) >= Preferences::precision ? 0 : Constants::unsat;
            case BooleanExpression::LT:
                return wsum(values) < rhs_val ? 0 : Constants::unsat;
            case BooleanExpression::LEQ:
                return wsum(values) <= rhs_val ? 0 : Constants::unsat;
            case BooleanExpression::GT:
                return wsum(values) > rhs_val ? 0 : Constants::unsat;
            case BooleanExpression::GEQ:
                return wsum(values) >= rhs_val ? 0 : Constants::unsat;
            default:
                return defaultUtil;
        }
        return defaultUtil;
    }

    virtual void setUtil(std::vector<value_t> &values, util_t util) override
    {

    }

    virtual std::string to_string() override
    {
        std::string ret = "constraint weights : ";
        for (auto w : weights) ret += std::to_string(w) + " ";
        // ret += " expr: " + (char)(expr);
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
    inline weight_t wsum(const std::vector<value_t> &values) const {
        if (values.size() != weights.size() )
            throw std::logic_error("values and weights vector sizes differ");
        weight_t wsum = 0;
        for (int i=0; i<weights.size(); i++) {
            wsum += values[i] * weights[i];
        }
        return wsum;
    }

protected:
    //std::map<std::vector<value_t>, util_t> values;
    std::vector<weight_t> weights;
    BooleanExpression expr;
    util_t rhs_val;    // RHS of the weighted sum

    // Default util, that is the util associated to any value combination that is
    // not explicitally listed in the relation.
    util_t defaultUtil;

    // The best and worst finite utils of this constraint which is used as bounds
    // in some searches.
    util_t LB;
    util_t UB;
};

#endif //CPUBE_WSUMCONSTRAINT_HPP
