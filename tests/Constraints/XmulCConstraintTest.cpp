//
// Created by Ferdinando Fioretto on 12/3/16.
//

#include <iostream>
#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include "VariableFactory.hpp"
#include "BE/CpuBEAgent.hpp"
#include "XmulCConstraint.hpp"

using namespace std;

class XmultCConstraintTest : public ::testing::Test
{
public:
    void SetUp(
            vector<Variable::ptr>& scope, int mult)
    {
        xMultC_i = make_shared<XmultCConstraint<int>>(scope, mult);
    }

    void SetUp(
            vector<Variable::ptr>& scope, float mult)
    {
        xMultC_f = make_shared<XmultCConstraint<float>>(scope, mult);
    }

protected:
    XmultCConstraint<int>::ptr xMultC_i;
    XmultCConstraint<float>::ptr xMultC_f;
};


TEST_F(XmultCConstraintTest, testConstraint_1)
{
    int min = 0, max = 4;
    int c = 3;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    vector<Variable::ptr> scope = {v1};
    // v1 * 1 + v2 * 2 = 0
    SetUp(scope, c);
    vector<value_t> values(1);

    for (int x = min; x <= max; x++)
    {
        EXPECT_EQ(xMultC_i->getUtil(values = {x}), c * x);
    }
}

TEST_F(XmultCConstraintTest, testConstraint_2)
{
    int min = -4, max = 4;
    int c = 3;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    vector<Variable::ptr> scope = {v1};
    // v1 * 1 + v2 * 2 = 0
    SetUp(scope, c);
    vector<value_t> values(1);

    for (int x = min; x <= max; x++)
    {
        EXPECT_EQ(xMultC_i->getUtil(values = {x}), c * x);
    }
}
