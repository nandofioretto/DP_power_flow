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
#include "PbalanceConstraint.h"
#include "QbalanceConstraint.h"
#include "LineFlowConstraint.h"
#include "Constant.hpp"

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
                                          vector<Variable::ptr> variables,
                                          vector<Constant::ptr> constants)
{
    string name = conXML->first_attribute("name")->value();
    string reference_name = conXML->first_attribute("reference")->value();

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
    if (reference_name == "global:weightedSum")
    {
        /// todo
        /// constraint = createWsumConstraint(conXML, variables);
    }
    else if (reference_name == "global:XmultC")
    {
        constraint = createXmultCConstraint(conXML, variables);
    }
    else if (reference_name == "global:active_power_balance")
    {
        constraint = createPbalanceConstraint(conXML, variables, constants);
    }
    else if (reference_name == "global:reactive_power_balance")
    {
        constraint = createQbalanceConstraint(conXML, variables, constants);
    }
    else if (reference_name == "global:line_flow")
    {
        constraint = createLineFlowConstraint(conXML, variables, constants);
    }

    else if (utils::find(reference_name, relationNames))
    {
        // Retrieve the relation associated to this constraint:
        int size = xmlutils::getIntAttribute(relsXML, "nbRelations");
        if (size <= 0)
            throw std::logic_error("No relations presents");

        xml_node<> *relXML = relsXML->first_node("relation");
        while (reference_name.compare(relXML->first_attribute("name")->value()) != 0)
        {
            relXML = relXML->next_sibling();
            if (!relXML)
                throw std::logic_error("No Relation associated to constraint " + name);
        }

        constraint = createTableConstraint(conXML, relXML, variables);
    }
        // Note: For the moment these are not handled
    else if (utils::find(reference_name, functionNames))
    {

        // Retrieve the relation associated to this constraint:
        int size = xmlutils::getIntAttribute(relsXML, "nbFunctions");
        if (size > 0)
            throw std::logic_error("No Function associated to constraint " + name);

        xml_node<> *relXML = relsXML->first_node("function");
        while (reference_name.compare(relXML->first_attribute("name")->value()) != 0)
        {
            relXML = relXML->next_sibling();
            if (!relXML)
                throw std::logic_error("No Function associated to constraint " + name);
        }
    }


    // Proces constraints according to their type.
    // string semantics = relXML->first_attribute("semantics")->value();

    #ifdef TEMP
    if (!constraint)
        throw std::logic_error("Error in parsing constraint " + name);

    setProperties(constraint, name, agents);
    #endif
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

std::vector<Constant::ptr> ConstraintFactory::getConstants(rapidxml::xml_node<> *conXML,
                                                           std::vector<Constant::ptr> constants)
{
    // Read constants
    string c_names = conXML->first_attribute("constants")->value();
    auto vec_names = strutils::split(c_names);
    std::vector<Constant::ptr> out;

    for (auto c_name : vec_names)
    {
        Constant::ptr c_target = nullptr;
        for (auto &c : constants)
        {
            if (c->getName().compare(c_name) == 0)
            {
                c_target = c;
                break;
            }
        }
        ASSERT(c_target, "Error in retrieving constants of constraint\n");
        out.push_back(c_target);
    }

    return out;
}


//<constraint name="power_1" arity="8" scope="Pg1 v1_m v2_m v3_m v4_m v1_a v2_a v3_a v4_a"
//constants="Pd Y Theta" sum_index="i" range="1 2 3 4" reference="global:active_power_balance">
// <parameters>
//  1 : Pg1,
// -1 : Pd[1],
// -1 : v1_m * SUM_i Y[1][{i}] * v{i}_m * cos(v1_a - v{i}_a - Theta[1][{i}])
//      <eq> 0 </eq>
//</parameters>
//</constraint>

// 1. Read old power constraint and create it (not as a table though)
// 2. Create the weighted constraint in the code for the rest of the terms
//    and populate table
TableConstraint::ptr ConstraintFactory::createPbalanceConstraint(rapidxml::xml_node<> *conXML,
                                                                 std::vector<Variable::ptr> variables,
                                                                 std::vector<Constant::ptr> constants)
{
    // Read Relation Properties
    string name = xmlutils::getStrAttribute(conXML, "name");
    int   arity = xmlutils::getIntAttribute(conXML, "arity");
    vector<string> scope_str = strutils::split(conXML->first_attribute("scope")->value());
    vector<Variable::ptr> scope     = getScope(conXML, variables);

    vector<string> const_str = strutils::split(conXML->first_attribute("constants")->value());
    vector<Constant::ptr> vec_const = getConstants(conXML, constants);

    vector<string> idx_range = strutils::split( conXML->first_attribute("range")->value() );
    string idx_name          = conXML->first_attribute("sum_index")->value();
    xml_node<>* params       = conXML->first_node("parameters");
    if (!params)
        throw std::invalid_argument("No parameters found while parsing constraint " + name);

    auto xml_rel = params->first_node();
    if (!xml_rel)
        throw std::invalid_argument("Invalid boolean expression in constraint " + name);
    BooleanExpression bexpr = to_booleanExpression(xml_rel->name());

    std::vector<std::vector<value_t>> scope_values;
    for (int i = 0; i < scope.size(); i++)
    {
        Variable::ptr var = scope[i];
        scope_values.push_back(var->getValues());
    }

    std::vector <Variable::ptr> scope_p_balance;
    std::vector <Variable::ptr> const_p_balance;

    std::string function = params->value();
    strutils::replace_all(function, "(", " ");
    std::vector<char> delim = { ')', '\t', '\r'};
    function = strutils::erase(function, delim);
    vector<string> split_sum = strutils::split(function, ',');

    // weights of the weighted sum
    vector<double> terms_weight;

    // for each term of the weighted sum, it stores a list of idx for the variables in "scope".
    vector<vector<int>> terms_scope_idx;
    vector<vector<double>> terms_const_values;
    vector<vector<bool>> terms_is_constant;
    vector<vector<string>> terms_str_elements;
    vector<std::string> terms_type;

    for (string& term : split_sum)
    {
        vector<string> tmp_expr = strutils::split(term, ':');
        terms_weight.push_back(std::stod(tmp_expr[0]));
        vector<string> split_function = strutils::split(tmp_expr[1]);


        if (term != split_sum.back())
        {
            ASSERT(split_function.size() == 1,
                   " Error processing Pbalance constraint. Expected 1 variable or constant.");

            int idx_v = utils::findIdx(scope_str, split_function[0]);
            if (idx_v != -1)
            {
                terms_type.push_back("var");
                vector<int> T = {idx_v};
                terms_scope_idx.push_back(T);
                vector<double> D = {(double)idx_v};   // empty
                terms_const_values.push_back(D);
                vector<string> S = {split_function[0]};
                terms_str_elements.push_back(S);
                vector<bool> B = {false};
                terms_is_constant.push_back(B);
                continue;
            }
            else
            {
                terms_type.push_back("const");
                vector<int> T = {-1};
                terms_scope_idx.push_back(T);
                vector<double> D = {getConstantValue(vec_const, split_function[0])};
                terms_const_values.push_back(D);
                vector<string> S = {split_function[0]};
                terms_str_elements.push_back(S);
                vector<bool> B = {true};
                terms_is_constant.push_back(B);
                continue;
            }
        }

        terms_type.push_back("expr");

        //   0  1   2       3     4    5   6  7   8   9  10    11     12
        // v1_m * SUM_i Y[1][{i}] * v{i}_m * cos v1_a - v{i}_a - Theta[1][{i}]
        std::string const_Y = split_function[3];
        std::string const_T = split_function[12];
        std::string var_k_m = split_function[0];
        std::string var_i_m = split_function[5];
        std::string var_k_a = split_function[8];
        std::string var_i_a = split_function[10];

        // 0. Create vector which maps names (or ptr) of variables seen in the equation
        //          (i+0         i+1        i+2     i+3          i+4)          last
        //  SUM_i Y[1][{i}] * v{i}_m * cos(v1_a - v{i}_a - Theta[1][{i}])   *  v1_m
        vector<string> unrolled_function;
        vector<int> unrolled_idx;
        vector<value_t> unrolled_values;
        vector<bool> is_constant;
        for (string &idx : idx_range)
        {
            // Y[k][{i}]
            string tmp = const_Y;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            unrolled_values.push_back(getConstantValue(vec_const, tmp));
            unrolled_idx.push_back(-1);
            is_constant.push_back(true);

            // v{i}_m
            tmp = var_i_m;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            int idx_vim = utils::findIdx(scope_str, tmp);
            unrolled_idx.push_back(idx_vim);

            unrolled_values.push_back(idx_vim);
            is_constant.push_back(false);

            // vk_a
            unrolled_function.push_back(var_k_a);
            int idx_vka = utils::findIdx(scope_str, var_k_a);
            unrolled_idx.push_back(idx_vka);

            unrolled_values.push_back(idx_vka);
            is_constant.push_back(false);

            // v{i}_a
            tmp = var_i_a;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            int idx_via = utils::findIdx(scope_str, tmp);
            unrolled_idx.push_back(idx_via);

            unrolled_values.push_back(idx_via);
            is_constant.push_back(false);

            // T[k][{i}]
            tmp = const_T;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            unrolled_values.push_back(getConstantValue(vec_const, tmp));
            unrolled_idx.push_back(-2);
            is_constant.push_back(true);
        }

        // vk_m
        unrolled_function.push_back(var_k_m);
        int idx_vkm = utils::findIdx(scope_str, var_k_m);
        unrolled_idx.push_back(idx_vkm);
        unrolled_values.push_back(idx_vkm);
        is_constant.push_back(false);

        /////////////
        terms_scope_idx.push_back(unrolled_idx);
        terms_const_values.push_back(unrolled_values);
        terms_str_elements.push_back(unrolled_function);
        terms_is_constant.push_back(is_constant);
        /////////////
    }

    double rhs_val = stoi(xml_rel->value());
    std::vector<Variable::ptr> TMP;
    auto wsumCon = std::make_shared<WSumConstraint<double>>(TMP, terms_weight, bexpr, rhs_val);
    auto pBalanCon = std::make_shared<PbalanceConstraint>();
    auto tabCon = std::make_shared<TableConstraint>(scope, wsumCon->getDefaultUtil());

    std::vector<value_t> tuple_values(terms_weight.size());
    combinatorics::Permutation<value_t> perms_big_expr(scope_values);

    for (auto& p : perms_big_expr.getPermutations())
    {
        for (int w = 0; w < terms_weight.size(); w++)
        {
            if (terms_type[w] == "var")
            {
                tuple_values[w] = p [ terms_scope_idx[w][0] ];
            }
            else if (terms_type[w] == "const")
            {
                tuple_values[w] = terms_const_values[w][0];
            }
            else if (terms_type[w] == "expr")
            {
                for (int i = 0; i < terms_str_elements[w].size(); i++)
                {
                    if (terms_is_constant[w][i])
                        continue;

                    terms_const_values[w][i] = p[ terms_scope_idx[w][i] ];
                }
                tuple_values[w] = pBalanCon->getUtil(terms_const_values[w]);
            }
        }
        tabCon->setUtil(p, wsumCon->getUtil(tuple_values));
//        if (wsumCon->getUtil(tuple_values) == 0)
//        {
//            std::cout << "Found tuple: " << strutils::to_string(tuple_values) << "\n";
//        }
    }

    return tabCon;
}


TableConstraint::ptr ConstraintFactory::createQbalanceConstraint(rapidxml::xml_node<> *conXML,
                                                                 std::vector<Variable::ptr> variables,
                                                                 std::vector<Constant::ptr> constants)
{
    // Read Relation Properties
    string name = xmlutils::getStrAttribute(conXML, "name");
    int   arity = xmlutils::getIntAttribute(conXML, "arity");
    vector<string> scope_str = strutils::split(conXML->first_attribute("scope")->value());
    vector<Variable::ptr> scope     = getScope(conXML, variables);

    vector<string> const_str = strutils::split(conXML->first_attribute("constants")->value());
    vector<Constant::ptr> vec_const = getConstants(conXML, constants);

    vector<string> idx_range = strutils::split( conXML->first_attribute("range")->value() );
    string idx_name          = conXML->first_attribute("sum_index")->value();

    xml_node<>* params       = conXML->first_node("parameters");
    if (!params)
        throw std::invalid_argument("No parameters found while parsing constraint " + name);

    auto xml_rel = params->first_node();
    if (!xml_rel)
        throw std::invalid_argument("Invalid boolean expression in constraint " + name);
    BooleanExpression bexpr = to_booleanExpression(xml_rel->name());

    std::vector<std::vector<value_t>> scope_values;
    for (int i = 0; i < scope.size(); i++)
    {
        Variable::ptr var = scope[i];
        scope_values.push_back(var->getValues());
    }

    std::vector <Variable::ptr> scope_q_balance;
    std::vector <Variable::ptr> const_q_balance;

    std::string function = params->value();
    strutils::replace_all(function, "(", " ");
    std::vector<char> delim = { ')', '\t', '\r'};
    function = strutils::erase(function, delim);
    vector<string> split_sum = strutils::split(function, ',');

    // weights of the weighted sum
    vector<double> terms_weight;

    // for each term of the weighted sum, it stores a list of idx for the variables in "scope".
    vector<vector<int>> terms_scope_idx;
    vector<vector<double>> terms_const_values;
    vector<vector<bool>> terms_is_constant;
    vector<vector<string>> terms_str_elements;
    vector<std::string> terms_type;

    for (string& term : split_sum)
    {
        vector<string> tmp_expr = strutils::split(term, ':');
        terms_weight.push_back(std::stod(tmp_expr[0]));
        vector<string> split_function = strutils::split(tmp_expr[1]);


        if (term != split_sum.back())
        {
            ASSERT(split_function.size() == 1,
                   " Error processing Pbalance constraint. Expected 1 variable or constant.");

            int idx_v = utils::findIdx(scope_str, split_function[0]);
            if (idx_v != -1)
            {
                terms_type.push_back("var");
                vector<int> T = {idx_v};
                terms_scope_idx.push_back(T);
                vector<double> D = {(double)idx_v};   // empty
                terms_const_values.push_back(D);
                vector<string> S = {split_function[0]};
                terms_str_elements.push_back(S);
                vector<bool> B = {false};
                terms_is_constant.push_back(B);
                continue;
            }
            else
            {
                terms_type.push_back("const");
                vector<int> T = {-1};
                terms_scope_idx.push_back(T);
                vector<double> D = {getConstantValue(vec_const, split_function[0])};
                terms_const_values.push_back(D);
                vector<string> S = {split_function[0]};
                terms_str_elements.push_back(S);
                vector<bool> B = {true};
                terms_is_constant.push_back(B);
                continue;
            }
        }

        terms_type.push_back("expr");

        //   0  1   2       3     4    5   6  7   8   9  10    11     12
        // v1_m * SUM_i Y[1][{i}] * v{i}_m * cos v1_a - v{i}_a - Theta[1][{i}]
        std::string const_Y = split_function[3];
        std::string const_T = split_function[12];
        std::string var_k_m = split_function[0];
        std::string var_i_m = split_function[5];
        std::string var_k_a = split_function[8];
        std::string var_i_a = split_function[10];

        // 0. Create vector which maps names (or ptr) of variables seen in the equation
        //          (i+0         i+1        i+2     i+3          i+4)          last
        //  SUM_i Y[1][{i}] * v{i}_m * cos(v1_a - v{i}_a - Theta[1][{i}])   *  v1_m
        vector<string> unrolled_function;
        vector<int> unrolled_idx;
        vector<value_t> unrolled_values;
        vector<bool> is_constant;
        for (string &idx : idx_range)
        {
            // Y[k][{i}]
            string tmp = const_Y;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            unrolled_values.push_back(getConstantValue(vec_const, tmp));
            unrolled_idx.push_back(-1);
            is_constant.push_back(true);

            // v{i}_m
            tmp = var_i_m;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            int idx_vim = utils::findIdx(scope_str, tmp);
            unrolled_idx.push_back(idx_vim);

            unrolled_values.push_back(idx_vim);
            is_constant.push_back(false);

            // vk_a
            unrolled_function.push_back(var_k_a);
            int idx_vka = utils::findIdx(scope_str, var_k_a);
            unrolled_idx.push_back(idx_vka);

            unrolled_values.push_back(idx_vka);
            is_constant.push_back(false);

            // v{i}_a
            tmp = var_i_a;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            int idx_via = utils::findIdx(scope_str, tmp);
            unrolled_idx.push_back(idx_via);

            unrolled_values.push_back(idx_via);
            is_constant.push_back(false);

            // T[k][{i}]
            tmp = const_T;
            strutils::replace_all(tmp, "{" + idx_name + "}", idx);
            unrolled_function.push_back(tmp);
            unrolled_values.push_back(getConstantValue(vec_const, tmp));
            unrolled_idx.push_back(-2);
            is_constant.push_back(true);
        }

        // vk_m
        unrolled_function.push_back(var_k_m);
        int idx_vkm = utils::findIdx(scope_str, var_k_m);
        unrolled_idx.push_back(idx_vkm);
        unrolled_values.push_back(idx_vkm);
        is_constant.push_back(false);

        /////////////
        terms_scope_idx.push_back(unrolled_idx);
        terms_const_values.push_back(unrolled_values);
        terms_str_elements.push_back(unrolled_function);
        terms_is_constant.push_back(is_constant);
        /////////////
    }

    double rhs_val = stoi(xml_rel->value());
    std::vector<Variable::ptr> TMP;
    auto wsumCon = std::make_shared<WSumConstraint<double>>(TMP, terms_weight, bexpr, rhs_val);
    auto qBalanCon = std::make_shared<QbalanceConstraint>();
    auto tabCon = std::make_shared<TableConstraint>(scope, wsumCon->getDefaultUtil());

    std::vector<value_t> tuple_values(terms_weight.size());
    combinatorics::Permutation<value_t> perms_big_expr(scope_values);

    for (auto& p : perms_big_expr.getPermutations())
    {
        for (int w = 0; w < terms_weight.size(); w++)
        {
            if (terms_type[w] == "var")
            {
                tuple_values[w] = p [ terms_scope_idx[w][0] ];
            }
            else if (terms_type[w] == "const")
            {
                tuple_values[w] = terms_const_values[w][0];
            }
            else if (terms_type[w] == "expr")
            {
                for (int i = 0; i < terms_str_elements[w].size(); i++)
                {
                    if (terms_is_constant[w][i])
                        continue;

                    terms_const_values[w][i] = p[ terms_scope_idx[w][i] ];
                }
                tuple_values[w] = qBalanCon->getUtil(terms_const_values[w]);
            }
        }
        tabCon->setUtil(p, wsumCon->getUtil(tuple_values));
//        if (wsumCon->getUtil(tuple_values) == 0)
//        {
//            std::cout << "Found tuple: " << strutils::to_string(tuple_values) << "\n";
//        }
    }

    return tabCon;
}


//<constraint name="Pflow_1" arity="3" scope="v1_m v1_a v2_a" constants="Pf x" reference="global:line_flow">
// <parameters>
//   -1 : Pf[1],
//   1  : abs( v1_m * v1_m * sin(v1_a - v2_a) * x[1] )
//<le>0</le>
//</parameters>

TableConstraint::ptr ConstraintFactory::createLineFlowConstraint(rapidxml::xml_node<> *conXML,
                                                                 std::vector<Variable::ptr> variables,
                                                                 std::vector<Constant::ptr> constants)
{
    // Read Relation Properties
    string name = xmlutils::getStrAttribute(conXML, "name");
    int   arity = xmlutils::getIntAttribute(conXML, "arity");
    vector<string> scope_str = strutils::split(conXML->first_attribute("scope")->value());
    vector<Variable::ptr> scope     = getScope(conXML, variables);

    vector<string> const_str = strutils::split(conXML->first_attribute("constants")->value());
    vector<Constant::ptr> vec_const = getConstants(conXML, constants);

    xml_node<>* params       = conXML->first_node("parameters");
    if (!params)
        throw std::invalid_argument("No parameters found while parsing constraint " + name);

    auto xml_rel = params->first_node();
    if (!xml_rel)
        throw std::invalid_argument("Invalid boolean expression in constraint " + name);
    BooleanExpression bexpr = to_booleanExpression(xml_rel->name());

    std::vector<std::vector<value_t>> scope_values;
    for (int i = 0; i < scope.size(); i++)
    {
        Variable::ptr var = scope[i];
        scope_values.push_back(var->getValues());
    }

    std::string function = params->value();
    strutils::replace_all(function, "(", " ");
    std::vector<char> delim = { ')', '\t', '\r'};
    function = strutils::erase(function, delim);
    vector<string> split_sum = strutils::split(function, ',');

    ASSERT(split_sum.size() == 2,
           " Error processing line flow constraint. Expected 2 parameters in the weighted sum.");

    // weights of the weighted sum
    double w1=1,
            w2=1;

    // for each term of the weighted sum, it stores a list of idx for the variables in "scope".

    vector<string> expr1 = strutils::split(split_sum[0], ':');
    vector<string> expr2 = strutils::split(split_sum[1], ':');
    w1 = std::stod(expr1[0]);
    w2 = std::stod(expr2[0]);
    double c1 = getConstantValue(vec_const, expr1[1]);

    //std::vector<char> delim2 = {'*', '-'};
    expr2[1] = strutils::erase(expr2[1], {'*', '-'});
    vector<string> split_function = strutils::split(expr2[1]);

    //  0    1     2       3   4      5       6
    // abs v1_m * v1_m * sin(v1_a - v4_a) * x[3]
    std::string var_i_m = split_function[1];
    std::string var_i_a = split_function[4];
    std::string var_j_a = split_function[5];
    std::string const_x = split_function[6];
    value_t     const_x_val = getConstantValue(vec_const, const_x);

    std::cout << const_x_val << "\n";

    auto line_flow = std::make_shared<LineFlowConstraint>(scope);
    auto tabCon    = std::make_shared<TableConstraint>(scope, line_flow->getDefaultUtil());
    combinatorics::Permutation<value_t> perms(scope_values);

    vector<value_t> unrolled_values(4, 0);
    unrolled_values[3] = const_x_val;

    for (auto& p : perms.getPermutations())
    {
        for (int i = 0; i < unrolled_values.size() - 1; i++)
        {
            unrolled_values[ i ] = p[ i ];
        }

        double val = (w1 * c1) + (w2 * abs(line_flow->getUtil(unrolled_values)));
        tabCon->setUtil(p, val);
    }

    return tabCon;
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


double ConstraintFactory::getConstantValue(std::vector<Constant::ptr> vec_const, std::string const_str)
{
    std::vector<char> delim = { ']', '}', '{', ' '};
    const_str = strutils::erase(const_str, delim);
    std::vector<std::string> const_str_vec = strutils::split(const_str, '[');

    // get constant
    Constant::ptr c_ptr = nullptr;
    for (auto& c : vec_const)
    {
        if (c->getName().compare(const_str_vec[0]) == 0)
        {
            c_ptr = c;
            break;
        }
    }
    ASSERT(c_ptr, "Error in retrieving Y constant\n");

    if (c_ptr->isConstant())
    {
        return c_ptr->getValue();
    }
    if (c_ptr->isVector())
    {
        int i = stoi(const_str_vec[1]) - 1; // convention: const indexes start from 1
        return c_ptr->getValue(i);
    }
    if (c_ptr->isMatrix())
    {
        int i = stoi(const_str_vec[1]) - 1; // convention: const indexes start from 1
        int j = stoi(const_str_vec[2]) - 1; // convention: const indexes start from 1
        return c_ptr->getValue(i, j);
    }
    else return Constants::NaN;
}
