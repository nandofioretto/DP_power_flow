#ifndef CUDA_DBE_CPUTABLE_HPP
#define CUDA_DBE_CPUTABLE_HPP

#include <memory>
#include <iostream>
#include <vector>
#include <cmath>

#include "Assert.hpp"
#include <string_utils.hpp>
#include "Agent.hpp"
#include "Constraint.hpp"
#include "Variable.hpp"
#include "Types.hpp"

using namespace misc_utils;


class CpuTable
{
public:
    typedef std::shared_ptr<CpuTable> ptr;

    CpuTable(std::vector<Variable::ptr> _vars);

    // Create the CPU Table Associated to the Constraint con
    CpuTable(Constraint::ptr con);

    // Creates an Empty CPU Table Associaed to the set of variables given
    CpuTable(std::vector<Agent::ptr> agts);

    ~CpuTable();


    void pushRow(std::vector<value_t> v, util_t u)
    {
        values.push_back(v);
        utils.push_back(u);
    }

    size_t getSize() const
    {
        return values.size();
    }

    const std::vector<Variable::ptr>& getScope() const
    {
        return scope;
    }

    std::vector<std::vector<value_t>> &getValues()
    {
        return values;
    }

    std::vector<value_t> &getValue(size_t idx)
    {
        return values[idx];
    }

    std::vector<util_t> &getUtils()
    {
        return utils;
    }

    util_t getUtil(size_t idx)
    {
        ASSERT(idx < utils.size(), "Tring to access to table index: " << idx << " >  table size: " << utils.size() );
        return utils[idx];
        //return idx < utils.size() ? utils[idx] : 0 ;
    }

    void incrUtil(size_t idx, util_t u)
    {
        ASSERT(idx < utils.size(), "Tring to access to table index: " << idx << " >  table size: " << utils.size() );
        utils[idx] = Constants::aggregate(utils[idx], u);
    }

    size_t getSizeBytes() const
    {
        return values.size() * scope.size() * sizeof(value_t);
    }

    util_t getUtil(std::vector<value_t>& scope_values)
    {
        size_t idx = 0;
        size_t mult = 1;
        value_t min = 0;
        for (int i = scope.size() - 1; i >= 0; i--)
        {
            min = scope[i]->getMin();
            idx += (scope_values[i] - min) * mult;
            mult *= scope[i]->getDomSize();
        }
        return getUtil(idx);
    }

    std::string to_string() const
    {
        std::string res;
        res += " scope = [";
        for (auto v : scope) res += v->getName() + ",";
        res += "] size :" + std::to_string(getSize()) + "\n";
        //for (int i=0; i<std::min((size_t)10, getSize()); i++) {
        for (int i = 0; i < getSize(); i++)
        {
            if (Constants::isFinite(utils[i]))
                res += std::to_string(i) + ": " +
                   strutils::to_string(values[i]) + " = " +
                   std::to_string(utils[i]) + "\n";
        }
        return res;
    }


private:
    std::vector<Variable::ptr> scope;
    std::vector<std::vector<value_t>> values;
    std::vector<util_t> utils;
};

#endif // CUDA_DBE_CPUTABLE_HPP
