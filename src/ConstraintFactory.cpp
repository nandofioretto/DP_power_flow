#include <string>
#include <sstream>
#include <rapidxml.hpp>
#include <iterator>
#include <vector>
#include <memory>
#include <Utils/xmlutils.h>
#include <Utils/Permutation.hpp>
#include "string_utils.hpp"

#include "ConstraintFactory.hpp"
#include "Assert.hpp"
#include "Types.hpp"
#include "Agent.hpp"
#include "Variable.hpp"
#include "TableConstraint.hpp"
#include "BooleanExpression.hpp"
#include "WSumConstraint.hpp"
#include "XmulCConstraint.hpp"

using namespace rapidxml;
using namespace misc_utils;
using namespace std;

// Initializes static members
int ConstraintFactory::constraintsCt = 0;

// It constructs and returns a new constraint.
Constraint::ptr ConstraintFactory::create(xml_node<>* conXML,
                                          xml_node<>* relsXML,
                                          xml_node<>* funXML,
                                          vector<Agent::ptr> agents,
                                          vector<Variable::ptr> variables)
{
    string name = conXML->first_attribute("name")->value();
    string rel = conXML->first_attribute("reference")->value();

    vector<string> relationNames;
    xml_node<> *node = relsXML ? relsXML->first_node("relation") : nullptr;
    while (node)
    {
        relationNames.push_back(xmlutils::getStrAttribute(node, "name"));
        node = node->next_sibling();
    }

    vector<string> functionNames;
    node = funXML ? funXML->first_node("function") : nullptr;
    while (node)
    {
        functionNames.push_back(xmlutils::getStrAttribute(node, "name"));
        node = node->next_sibling();
    }


    Constraint::ptr constraint = nullptr;
    // Differentiate between types of references:
    if (rel == "global:weightedSum")
    {
        constraint = createWsumConstraint(conXML, variables);
    }
    else if (rel == "global:XmultC")
    {
        constraint = createXmultCConstraint(conXML, variables);
    }
    else if (utils::find(rel, relationNames))
    {
        // Retrieve the relation associated to this constraint:
        int size = xmlutils::getIntAttribute(relsXML, "nbRelations");
        if (size <= 0)
            throw std::logic_error("No relations presents");

        xml_node<> *relXML = relsXML->first_node("relation");
        while (rel.compare(relXML->first_attribute("name")->value()) != 0)
        {
            relXML = relXML->next_sibling();
            if (!relXML)
                throw std::logic_error("No Relation associated to constraint " + name);
        }

        constraint = createTableConstraint(conXML, relXML, variables);
    }
        // Note: For the moment these are not handled
    else if (utils::find(rel, functionNames))
    {

        // Retrieve the relation associated to this constraint:
        int size = xmlutils::getIntAttribute(relsXML, "nbFunctions");
        if (size > 0)
            throw std::logic_error("No Function associated to constraint " + name);

        xml_node<> *relXML = relsXML->first_node("function");
        while (rel.compare(relXML->first_attribute("name")->value()) != 0)
        {
            relXML = relXML->next_sibling();
            if (!relXML)
                throw std::logic_error("No Function associated to constraint " + name);
        }
    }


    // Proces constraints according to their type.
    // string semantics = relXML->first_attribute("semantics")->value();

    if (!constraint)
        throw std::logic_error("Error in parsing constraint " + name);

    setProperties(constraint, name, agents);
    ++constraintsCt;
    return constraint;
}

// Jul 5, ok
void ConstraintFactory::setProperties
(Constraint::ptr c, string name, vector<Agent::ptr> agents)
{
    c->setID(constraintsCt);
    c->setName(name);

    // Registers this constraint in the agents owning the variables of the
    // constraint scope.
    for (auto v : c->getScope())
    {
        Agent::ptr v_owner = nullptr;
        for (auto a : agents) if (a->getID() == v->getAgtID()) { v_owner = a; }
        ASSERT(v_owner, "Error in finding variable owner\n");
        v_owner->addConstraint(c);
    }
}


// Jul 5, ok
std::vector<Variable::ptr> ConstraintFactory::getScope
(xml_node<>* conXML, std::vector<Variable::ptr> variables)
{
    int arity = atoi(conXML->first_attribute("arity")->value());

    // Read constraint scope
    string p_scope = conXML->first_attribute("scope")->value();
    std::vector<Variable::ptr> scope(arity, nullptr);
    stringstream ss(p_scope);
    int c = 0;
    string var;
    while (c < arity)
    {
        ss >> var;
        Variable::ptr v_target = nullptr;
        for (auto &v : variables)
            if (v->getName().compare(var) == 0)
                v_target = v;
        ASSERT(v_target, "Error in retrieving scope of constraint\n");

        scope[c++] = v_target;
    }

    return scope;
}

TableConstraint::ptr ConstraintFactory::createXmultCConstraint
        (xml_node<>* conXML, std::vector<Variable::ptr> variables)
{
    // Read Relation Properties
    string name = xmlutils::getStrAttribute(conXML, "name");
    int arity = xmlutils::getIntAttribute(conXML, "arity");

    if (arity != 1)
        throw logic_error("Error: XMultC constraint " + name + " has arity != 1");

    std::vector<Variable::ptr> scope = getScope(conXML, variables);

    xml_node<>* params = conXML->first_node("parameters");
    if (!params)
        throw std::invalid_argument("No parameters found while parsing constraint " + name);

    // Read constant
    std::vector<std::string> constantsStr = strutils::split(params->value());

    if (constantsStr.size() != 1)
        throw logic_error("Error: multiple constants found in constraint " + name + " of type cMultX");

    auto &cStr = constantsStr[0];

    // Temp Store for variable's values
    std::vector<value_t> values(1,0);

    if (utils::isInteger(cStr)) {
        auto con = std::make_shared<XmultCConstraint<int>>(scope, stoi(cStr));
        auto tabCon = std::make_shared<TableConstraint>(scope, con->getDefaultUtil());

        for (value_t v : scope[0]->getValues()) {
            values[0] = v;
            tabCon->setUtil(values, con->getUtil(values));
        }
        return tabCon;
    }
    else {
        auto con = std::make_shared<XmultCConstraint<double>>(scope, stoi(cStr));
        auto tabCon = std::make_shared<TableConstraint>(scope, con->getDefaultUtil());

        for (value_t v : scope[0]->getValues()) {
            values[0] = v;
            tabCon->setUtil(values, con->getUtil(values));
        }
        return tabCon;
    }
    return nullptr;
}

TableConstraint::ptr ConstraintFactory::createWsumConstraint
        (xml_node<>* conXML, std::vector<Variable::ptr> variables)
{
    // Read Relation Properties
    string name = xmlutils::getStrAttribute(conXML, "name");
    int arity = xmlutils::getIntAttribute(conXML, "arity");
    std::vector<Variable::ptr> scope = getScope(conXML, variables);

    std::vector<std::vector<value_t>> scope_values;
    for (int i = 0; i < scope.size(); i++)
    {
        Variable::ptr var = scope[i];
        scope_values.push_back(var->getValues());
    }

    xml_node<>* params = conXML->first_node("parameters");
    if (!params)
        throw std::invalid_argument("No parameters found while parsing constraint " + name);

    std::string values = params->value();
    std::vector<char> delim = {'[', ']', '}', '{'};
    values = strutils::erase(values, delim);
    std::vector<std::string> pairs = strutils::split(values);
    // check if weights are integers or doule


    auto xml_rel = params->first_node();
    if (!xml_rel)
        throw std::invalid_argument("Invalid boolean expression in constraint " + name);

    BooleanExpression bexpr = to_booleanExpression(xml_rel->name());
    combinatorics::Permutation<value_t> perms(scope_values);

    if (utils::isInteger(pairs[0])) {

        std::vector<int> weights;
        for (int i=0; i<pairs.size(); i += 2)
            weights.push_back( stoi(pairs[i]) );

        int rhs_val = stoi(xml_rel->value());
        auto wsumCon = std::make_shared<WSumConstraint<int>>(scope, weights, bexpr, rhs_val);
        auto tabCon = std::make_shared<TableConstraint>(scope, wsumCon->getDefaultUtil());

        for (auto& p : perms.getPermutations()) {
            tabCon->setUtil(p, wsumCon->getUtil(p));
        }
        return tabCon;
    }
    else {
        std::vector<double> weights;
        for (int i=0; i<pairs.size(); i += 2)
            weights.push_back( stod(pairs[i]) );

        double rhs_val = stod(xml_rel->value());
        auto wsumCon = std::make_shared<WSumConstraint<double>>(scope, weights, bexpr, rhs_val);
        auto tabCon = std::make_shared<TableConstraint>(scope, wsumCon->getDefaultUtil());

        for (auto& p : perms.getPermutations()) {
            tabCon->setUtil(p, wsumCon->getUtil(p));
        }
        return tabCon;
    }
    return nullptr;
}

// Jul 5, ok
TableConstraint::ptr ConstraintFactory::createTableConstraint
(xml_node<>* conXML, xml_node<>* relXML, 
 std::vector<Variable::ptr> variables)
{
  // Read Relation Properties
  string name = relXML->first_attribute("name")->value();
  int arity = atoi(relXML->first_attribute("arity")->value());
  size_t nb_tuples = atoi(relXML->first_attribute("nbTuples")->value());
  ASSERT(nb_tuples > 0, "Extensional Soft Constraint " << name << " is empty");

  // Okey works
  std::vector<Variable::ptr> scope = getScope(conXML, variables);

  // Get the default cost
  util_t def_cost = Constants::worstvalue;

  if (relXML->first_attribute("defaultCost"))
  {
    string cost = relXML->first_attribute("defaultCost")->value();
    if (cost.compare("infinity") == 0)
      def_cost = Constants::inf;
    else if (cost.compare("-infinity") == 0)
      def_cost = -Constants::inf;
    else
      def_cost = std::stod(cost);
  }

  auto con = std::make_shared<TableConstraint>(scope, def_cost);

  string content = relXML->value();
  size_t lhs = 0, rhs = 0;

  // replace all the occurrences of 'infinity' with a 'INFTY'
  while (true)
  {
    rhs = content.find("infinity", lhs);
    if (rhs != string::npos)
      content.replace(rhs, 8, to_string(Constants::inf));
    else break;
  };

  // replace all the occurrences of ':' with a '\b'
  // cost_t best_bound = Constants::worstvalue;
  // cost_t worst_bound = Constants::bestvalue;

  util_t m_cost;
  bool multiple_cost;
  int *tuple = new int[arity];
  int trim_s, trim_e;
  size_t count = 0;
  string str_tuples;
  lhs = 0;
  while (count < nb_tuples)
  {
    //multiple_cost = true;
    rhs = content.find(":", lhs);
    if (rhs < content.find("|", lhs))
    {
      if (rhs != string::npos)
      {
        m_cost = atoi(content.substr(lhs, rhs).c_str());

        // Keep track of the best/worst bounds
        // best_bound = Utils::getBest(m_cost, best_bound);
        // worst_bound = Utils::getWorst(m_cost, worst_bound);

        lhs = rhs + 1;
      }
    }

    rhs = content.find("|", lhs);
    trim_s = lhs, trim_e = rhs;
    lhs = trim_e + 1;

    if (trim_e == string::npos) trim_e = content.size();
    else while (content[trim_e - 1] == ' ') trim_e--;

    str_tuples = content.substr(trim_s, trim_e - trim_s);
    str_tuples = strutils::rtrim(str_tuples);
    stringstream ss(str_tuples);

    //int tmp;
    while (ss.good())
    {
      for (int i = 0; i < arity; ++i)
      {
        // ss >> tmp;
        // tuple[ i ] = scope[ i ]->getDomain().get_pos( tmp );
        ss >> tuple[i];
      }
      std::vector<value_t> v(tuple, tuple + arity);
      con->setUtil(v, m_cost);
      count++;
    }
  }

  // con->setBestCost(best_bound);
  // con->setWorstCost(worst_bound);

  delete[] tuple;

  return con;
}



// For WCSPs
Constraint::ptr 
ConstraintFactory::create(size_t nTuples, util_t defCost, util_t ub,
			  string content,
			  vector<int> scopeIDs,
			  vector<Agent::ptr> agents,
			  vector<Variable::ptr> variables)
{
  vector<Variable::ptr> scope = getScope(scopeIDs, variables);
  auto con = make_shared<TableConstraint>(scope, defCost);
  
  int arity = scopeIDs.size();
  util_t util;
  std::stringstream data(content);

  // Read tuples
  for (int l=0; l<nTuples; l++) { // lines
    std::vector<value_t> tuple(arity);
    
    for (int i=0; i<arity; i++) 
      data >> tuple[i];
    data >> util;
    // if (util >= ub) util = Constants::unsat;
    con->setUtil(tuple, util);
  }
  
  string name = "c_";
  for (auto id : scopeIDs) name += to_string(id);
  setProperties(con, name, agents);
  ++constraintsCt;
  return con;

}


std::vector<Variable::ptr> 
ConstraintFactory::getScope(vector<int> scopeIDs, 
			    vector<Variable::ptr> variables)
{
  vector<Variable::ptr> ret;
  for (auto id : scopeIDs)
  {
    for (auto vptr : variables)
    {
      if (vptr->getAgtID() == id)
      {
        ret.push_back(vptr);
        break;
      }
    }
  }
  return ret;
}
