//
// Created by Ferdinando Fioretto on 12/3/16.
//

#ifndef CPUBE_MULCONSTRAINT_H
#define CPUBE_MULCONSTRAINT_H

template<class T1, class T2>
class MulConstraint<T1, T2> : public Constraint
{
public:
    typedef std::shared_ptr <MulConstraint> ptr;


    // I think they want a special mult constraint cTimesX()

//    MulConstraint
//    (std::vector<Variable::ptr>& _scope, T1 ,
//            BooleanExpression _expr, util_t _defUtil)
//    : weights(_weights), expr(_expr), defaultUtil(_defUtil), LB(Constants::inf), UB(Constants::inf)
//    {
//        Constraint::scope = _scope;
//        for(auto& v : Constraint::scope) {
//            scopeAgtID.push_back(v->getAgtID());
//        }
//    }

};

#endif //CPUBE_MULCONSTRAINT_H
