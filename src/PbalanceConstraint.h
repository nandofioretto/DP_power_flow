//
// Created by Ferdinando Fioretto on 3/10/17.
//

#ifndef CPUBE_PBALANCECONSTRAINT_H
#define CPUBE_PBALANCECONSTRAINT_H


#include <memory>
#include <cmath>
#include "Variable.hpp"
#include "Assert.hpp"
#include "Constraint.hpp"
#include "Types.hpp"

class PbalanceConstraint : public Constraint
{
public:
    typedef std::shared_ptr<PbalanceConstraint> ptr;

    PbalanceConstraint()
            : defaultUtil(Constants::unsat), LB(Constants::inf), UB(Constants::inf)
    { }

    PbalanceConstraint(std::vector<Variable::ptr>& scope,
                       std::vector<Constant::ptr>& constants)
    : defaultUtil(Constants::unsat), LB(Constants::inf), UB(Constants::inf)
    {
        ASSERT(scope.size() == constants.size() * 4, "Error in parsing Pbalance constraint");

        Constraint::scope = scope;
        for(auto& v : Constraint::scope) {
            scopeAgtID.push_back(v->getAgtID());
        }
    }

    virtual ~PbalanceConstraint()
    { }

    //          (i+0         i+1        i+2     i+3          i+4)          last
    //  SUM_i Y[1][{i}] * v{i}_m * cos(v1_a - v{i}_a - Theta[1][{i}])   *  v1_m
    util_t getUtil(std::vector<value_t> &values) override
    {
        util_t util = 0;
        int n_iter = (values.size() - 1) / 5;
        for (int i = 0; i < n_iter; i++)
        {
            util += values[i+0] * values[i+1]  * cos(values[i+2] - values[i+3] - values[i+4]);
        }
        util *= values.back();
        return util;
    }

    void setUtil(std::vector<value_t> &values, util_t util) override
    {

    }

    std::string to_string() override
    {
        std::string ret = "constraint P-balance: ";
        return ret;
    }

    size_t getSizeBytes() override
    {
        return Constraint::getSizeBytes();
    }

    util_t getDefaultUtil() const
    {
        return defaultUtil;
    }

private:

    // Default util, that is the util associated to any value combination that is
    // not explicitally listed in the relation.
    util_t defaultUtil;

    // The best and worst finite utils of this constraint which is used as bounds
    // in some searches.
    util_t LB;
    util_t UB;

};
#endif //CPUBE_PBALANCECONSTRAINT_H
