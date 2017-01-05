//
// Created by Ferdinando Fioretto on 12/3/16.
//

#include <iostream>
#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include "VariableFactory.hpp"
#include "BE/CpuBEAgent.hpp"
#include "WSumConstraint.hpp"

using namespace std;

class WSumConstraintTest : public ::testing::Test
{
public:
    void SetUp(
            vector<Variable::ptr>& scope,
            vector<int>& weights,
            BooleanExpression expr,
            int rhs_val)
    {
        wsum_i = make_shared<WSumConstraint<int>>(scope, weights, expr, rhs_val);
    }

    void SetUp(
            vector<Variable::ptr>& scope,
            vector<float>& weights,
            BooleanExpression expr,
            float rhs_val)
    {
        wsum_f = make_shared<WSumConstraint<float>>(scope, weights, expr, rhs_val);
    }

protected:
    WSumConstraint<int>::ptr wsum_i;

    WSumConstraint<float>::ptr wsum_f;

};


TEST_F(WSumConstraintTest, testConstraint_1)
{
    int min = 0, max = 4;
    vector<int> weights = {1, 2};
    int rhs = 3;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    CpuBEAgent::ptr a2 = make_shared<CpuBEAgent>(1, "a2");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    Variable::ptr v2 = VariableFactory::create("v2", a2, min, max);
    vector<Variable::ptr> scope = {v1, v2};
    // v1 * 1 + v2 * 2 = 0
    SetUp(scope, weights, BooleanExpression::EQ, rhs);
    vector<value_t> values(2);

    for (int x = min; x <= max; x++)
    {
        for (int y = min; y <= max; y++)
        {
            if (weights[0] * x + weights[1] * y == rhs)
                EXPECT_EQ(wsum_i->getUtil(values = {x,y}), 0);
            else
                EXPECT_EQ(wsum_i->getUtil(values = {x,y}), Constants::unsat);
        }
    }
}

TEST_F(WSumConstraintTest, testConstraint_2)
{
    int min = -5, max = 5;
    vector<int> weights = {1, 2};
    int rhs = 2;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    CpuBEAgent::ptr a2 = make_shared<CpuBEAgent>(1, "a2");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    Variable::ptr v2 = VariableFactory::create("v2", a2, min, max);
    vector<Variable::ptr> scope = {v1, v2};
    // v1 * 1 + v2 * 2 = 0
    SetUp(scope, weights, BooleanExpression::EQ, rhs);
    vector<value_t> values(2);

    for (int x = min; x <= max; x++)
    {
        for (int y = min; y <= max; y++)
        {
            if (weights[0] * x + weights[1] * y == rhs)
                EXPECT_EQ(wsum_i->getUtil(values = {x,y}), 0);
            else
                EXPECT_EQ(wsum_i->getUtil(values = {x,y}), Constants::unsat);
        }
    }
}


TEST_F(WSumConstraintTest, testConstraint_3)
{
    int min = -5, max = 5;
    vector<float> weights = {1.5, 2.5};
    util_t rhs = 2;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    CpuBEAgent::ptr a2 = make_shared<CpuBEAgent>(1, "a2");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    Variable::ptr v2 = VariableFactory::create("v2", a2, min, max);
    vector<Variable::ptr> scope = {v1, v2};
    // v1 * 1 + v2 * 2 = 0
    SetUp(scope, weights, BooleanExpression::EQ, rhs);
    vector<value_t> values(2);

    for (int x = min; x <= max; x++)
    {
        for (int y = min; y <= max; y++)
        {
            if (weights[0] * x + weights[1] * y == rhs)
                EXPECT_EQ(wsum_f->getUtil(values = {x,y}), 0);
            else
                EXPECT_EQ(wsum_f->getUtil(values = {x,y}), Constants::unsat);
        }
    }
}
