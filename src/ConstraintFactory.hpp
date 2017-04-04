#ifndef CUDA_DBE_CONSTRAINT_FACTORY_HPP 
#define CUDA_DBE_CONSTRAINT_FACTORY_HPP

#include <rapidxml.hpp>
#include <string>
#include <vector>
#include <memory>

#include "Types.hpp"
#include "Agent.hpp"
#include "Variable.hpp"
#include "Constraint.hpp"
#include "TableConstraint.hpp"
#include "Constant.hpp"

class ConstraintFactory
{
public:

    // It constructs and returns a new constraint.
    // XML
    static Constraint::ptr create(rapidxml::xml_node<> *conXML,
                                  rapidxml::xml_node<> *relsXML,
                                  rapidxml::xml_node<>* funXML,
                                  std::vector<Agent::ptr> agents,
                                  std::vector<Variable::ptr> variables,
                                  std::vector<Constant::ptr> constants);

    // XCSP
    // At the moment all constraints are treated as table constraints
    static Constraint::ptr create(size_t nTuples, util_t defCost, util_t ub,
                                  std::string content,
                                  std::vector<int> scopeIds,
                                  std::vector<Agent::ptr> agents,
                                  std::vector<Variable::ptr> variables);

    // It resets the constraints counter.
    static void resetCt()
    {
        constraintsCt = 0;
    }

private:

    // The Constraints counter. It holds the ID of the next constraint to be
    // created.
    static int constraintsCt;

    // It returns the scope of the constraint
    // XML
    static std::vector<Variable::ptr> getScope
            (rapidxml::xml_node<> *conXML, std::vector<Variable::ptr> variables);

    static std::vector<Constant::ptr> getConstants
            (rapidxml::xml_node<> *conXML, std::vector<Constant::ptr> constants);

    // WCSP
    static std::vector<Variable::ptr> getScope(std::vector<int> scopeIDs,
                                               std::vector<Variable::ptr> variables);

    // Sets common constraint properties and initializes mappings.
    static void setProperties(Constraint::ptr c, std::string name,
                              std::vector<Agent::ptr> agents);

    // Get the value of a constant from a string like C[i][k]
    static double getConstantValue(std::vector<Constant::ptr> vec_const, std::string const_str);

    //////// XML only /////////
    // It constructs an extensional hard constraint from the xml bit
    static TableConstraint::ptr createTableConstraint
            (rapidxml::xml_node<> *conXML, rapidxml::xml_node<> *relXML,
             std::vector<Variable::ptr> variables);

    // Constructs a Table constraint from a line_flow constraint
    static TableConstraint::ptr createLineFlowConstraint
    (rapidxml::xml_node<>* conXML, std::vector<Variable::ptr> variables, std::vector<Constant::ptr> constants);

    // Constructs a Table constraint from a active_power_balance constraint
    static TableConstraint::ptr createPbalanceConstraint
            (rapidxml::xml_node<>* conXML, std::vector<Variable::ptr> variables, std::vector<Constant::ptr> constants);

    // Constructs a Table constraint from a reactive_power_balance constraint
    static TableConstraint::ptr createQbalanceConstraint
            (rapidxml::xml_node<>* conXML, std::vector<Variable::ptr> variables, std::vector<Constant::ptr> constants);

    // Constructs a Table constraint from a weighted sum constraint
    static TableConstraint::ptr createWsumConstraint
            (rapidxml::xml_node<>* conXML, std::vector<Variable::ptr> variables);

    // Constructs a Table constraint from a XmultC constraint
    static TableConstraint::ptr createXmultCConstraint
            (rapidxml::xml_node<>* conXML, std::vector<Variable::ptr> variables);

};


#endif // ULYSSES_KERNEL__CONSTRAINTS__CONSTRAINT_FACTORY_H_
