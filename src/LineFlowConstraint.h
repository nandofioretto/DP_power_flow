//
// Created by Ferdinando Fioretto on 3/10/17.
//

#ifndef CPUBE_LINEFLOWCONSTRAINT_H
#define CPUBE_LINEFLOWCONSTRAINT_H


#include <memory>
#include <cmath>
#include "Variable.hpp"
#include "Assert.hpp"
#include "Constraint.hpp"
#include "Types.hpp"

class LineFlowConstraint : public Constraint
{
public:
    typedef std::shared_ptr<LineFlowConstraint> ptr;

    LineFlowConstraint(std::vector<Variable::ptr>& scope)
        : defaultUtil(Constants::unsat), LB(Constants::inf), UB(Constants::inf)
    {
        Constraint::scope = scope;
        for(auto& v : Constraint::scope) {
            scopeAgtID.push_back(v->getAgtID());
        }
    }

    virtual ~LineFlowConstraint()
    { }

    //      0                1       2       3
    //abs (v1_m * v1_m * sin(v1_a - v3_a) * x[2] )
//    util_t getUtil(std::vector<value_t> &values) override
//    {
//        ASSERT( values.size() == 4, "Error in processing Line flow constraint. Expected 4 arguments");
//        util_t util = values[0] * values[0] * sin(values[1] - values[2]) * values[3];
//        LOG(INFO) << values[0] << " * " << values[0]  << " * sin("
//                  << values[1] << " - " << values[2] << ") * "
//                  << values[3] << " = " << util;
//        return fabs(util);
//    }

    // v1_m  = 0
    // v2_m  = 1
    // v1_a  = 2
    // v2_a  = 3
    // Y     = 4
    // T     = 5
    // v1_m * v1_m * Y[1][2] * cos(Theta[1][2]) - v1_m * v2_m * Y[1][2] * cos(v1_a - v2_a - Theta[1][2])
    util_t getUtil(std::vector<value_t> &values) override
    {
        ASSERT( values.size() == 6, "Error in processing Line flow constraint. Expected 6 arguments");
        util_t util = values[0] * values[0] * values[4] * cos(values[5]) -
                values[0] * values[1] * values[4] * cos(values[2] - values[3] -  values[5]);
        LOG(INFO) << values[0] << " * " << values[0]  << " * " << values[4] << " * cos("
                  << values[5] << " ) - " << values[0] << " * "
                  << values[1] << " * " << values[4] << " * cos("
                  << values[2] << " - " << values[3] << " - " << values[5] << " ) = " << util;
//        return fabs(util);
        return util;

    }


    void setUtil(std::vector<value_t> &values, util_t util) override
    {

    }

    std::string to_string() override
    {
        std::string ret = "constraint LineFlow: ";
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
#endif //CPUBE_LINEFLOWCONSTRAINT_H
