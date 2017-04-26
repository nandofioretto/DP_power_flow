//
// Created by Ferdinando Fioretto on 10/31/15.
//

#ifndef CUDA_DBE_VARIABLE_H
#define CUDA_DBE_VARIABLE_H


#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include "Assert.hpp"

#include "Types.hpp"

class Agent;

class Variable
{

public:
    typedef std::shared_ptr<Variable> ptr;

    static bool orderLt(const ptr &lhs, const ptr &rhs); //  {
    //   return Agent::orderLt(lhs->getAgt(), rhs->getAgt());
    // }

    static bool orderGt(const ptr &lhs, const ptr &rhs); // {
    //   return Agent::orderGt(lhs->getAgt(), rhs->getAgt());
    // }

    Variable()
    {
    }

    Variable(std::vector<value_t> values, std::shared_ptr<Agent> &agt,
             value_t prior = Constants::NaN);

    Variable(value_t min, value_t max, value_t discr, std::shared_ptr<Agent> &agt,
             value_t prior = Constants::NaN);

    value_t &operator[](std::size_t idx)
    {
        return values[idx];
    }

    value_t at(std::size_t idx)
    {
        return values[idx];
    }

    const value_t &operator[](std::size_t idx) const
    {
        return values[idx];
    }

    bool operator==(const Variable &rhs) const
    {
        // std::cout << "operator == Variable (classic)\n";
        return agtID == rhs.getAgtID();
    }

    bool operator==(const Variable::ptr &rhs) const
    {
        //std::cout << "operator == Variable (std::ptr)\n";
        return agtID == rhs->getAgtID();
    }

    // bool operator < (const Variable& other) const {
    //   if (agt->getPriority() < other.getAgt()->getPriority()) return true;
    //   return agt->getPriority() == other.getAgt()->getPriority() &&
    //     agtID < other.getAgtID();
    // }

    // bool operator<(const Variable::ptr& other) {
    //   if (agt->getPriority() < other->getAgt()->getPriority()) return true;
    //   return agt->getPriority() == other->getAgt()->getPriority() &&
    //     agtID < other->getAgtID();
    // }

    void setName(std::string _name)
    {
        Variable::name = _name;
    }

    std::string getName() const
    {
        return name;
    }

    int getAgtID() const
    {
        return agtID;
    }

    std::shared_ptr<Agent> &getAgt()
    {
        return agt;
    }

    value_t getMin() const
    {
        return min;
    }

    value_t getMax() const
    {
        return max;
    }

    size_t getDomSize() const
    {
        return values.size();
    }

    value_t getPrior() const
    {
        return prior;
    }

    const std::vector<value_t> &getValues() const
    {
        return values;
    }

    void setPrior(value_t _pr)
    {
        Variable::prior = _pr;
    }

    value_t getValue() const
    {
        return value;
    }

    int getValueIdx()
    {
        ASSERT(value_idx != -1, "Error assigning value to variable " << name);
        return value_idx;
    }

    void setValue(value_t _val)
    {
        value_idx = -1;
        for (int i = 0; i < values.size(); i++)
            if (std::abs(values[i] - _val) < 1E-10)
            {
                value_idx = i;
                break;
            }
        ASSERT(value_idx != -1, "Error assigning value to variable " << name);

        Variable::value = values[value_idx];
    }

    std::string to_string() const
    {
        std::string ret = name + " range: [" + std::to_string(getMin()) + " "
                          + std::to_string(getMax()) + "] ";
        if (prior != Constants::NaN)
            ret += "Prior: " + std::to_string(prior);

        for (value_t p : values)
            ret += std::to_string(p) + ",";
        return ret;
    }

private:
    // Domain Info
    value_t min, max;
    value_t prior;
    std::vector<value_t> values;
    value_t value; // selected value.
    int value_idx;

    std::unordered_map<value_t, int> value_idx_map;

    std::shared_ptr<Agent> agt;
    int agtID;
    std::string name;
};


#endif // CUDA_DBE_VARIABLE_H
