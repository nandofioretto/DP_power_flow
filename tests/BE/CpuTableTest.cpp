//
// Created by Ferdinando Fioretto on 12/16/16.
//
#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include "BE/CpuBEAgent.hpp"
#include "BE/CpuTable.hpp"
#include "TableConstraint.hpp"
#include "VariableFactory.hpp"
#include "Utils/CpuInfo.hpp"
#include "XmulCConstraint.hpp"
#include "WSumConstraint.hpp"

using namespace std;

class CpuTableTest : public ::testing::Test
{
public:
    void SetUp()
    {
        Cpu::Info::initialize();
    }
    void SetUp(Constraint::ptr con)
    {
        table = std::make_shared<CpuTable>(con);
    }

protected:
    CpuTable::ptr table;
};

TEST_F(CpuTableTest, testConstructorConstraintTable_1)
{
    int domSize = 2;
    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    CpuBEAgent::ptr a2 = make_shared<CpuBEAgent>(1, "a2");
    Variable::ptr v1 = VariableFactory::create("v1", a1, 0, domSize-1);
    Variable::ptr v2 = VariableFactory::create("v2", a2, 0, domSize-1);

    std::vector<Variable::ptr> scope = {v1, v2};
    TableConstraint::ptr con = make_shared<TableConstraint>(scope, 0);

    vector<value_t> values(2);
    for (int i=0; i<domSize; i++)
    {
        values[0] = i;
        for (int j=0; j<domSize; j++)
        {
            values[1] = j;
            con->setUtil(values, i*j);
        }
    }

    SetUp(con);
    values = {0,0};
    EXPECT_EQ( table->getUtil(0), con->getUtil(values) );
    values = {0,1};
    EXPECT_EQ( table->getUtil(1), con->getUtil(values) );
    values = {1,0};
    EXPECT_EQ( table->getUtil(2), con->getUtil(values) );
    values = {1,1};
    EXPECT_EQ( table->getUtil(3), con->getUtil(values) );
};

TEST_F(CpuTableTest, testConstructorConstraintTable_2)
{
    int domSize = 3;
    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    CpuBEAgent::ptr a2 = make_shared<CpuBEAgent>(1, "a2");
    Variable::ptr v1 = VariableFactory::create("v1", a1, -1, domSize-1);
    Variable::ptr v2 = VariableFactory::create("v2", a2, -1, domSize-1);

    std::vector<Variable::ptr> scope = {v1, v2};
    TableConstraint::ptr con = make_shared<TableConstraint>(scope, 0);

    vector<value_t> values(9);
    for (int i=-1; i<=1; i++)
    {
        values[0] = i;
        for (int j=-1; j<=1; j++)
        {
            values[1] = j;
            con->setUtil(values, i*j);
        }
    }


    SetUp(con);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <=1; j++) {
            values = {i,j};
            EXPECT_EQ( table->getUtil(0), con->getUtil(values) );
        }
    }
};

TEST_F(CpuTableTest, testConstructorConstraintXMulC)
{
    XmultCConstraint<int>::ptr xMultC_i;
    int min = -4, max = 4;
    int c = 3;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    vector<Variable::ptr> scope = {v1};
    xMultC_i = make_shared<XmultCConstraint<int>>(scope, c);

    SetUp(xMultC_i);
    vector<int> values(1);

    for (int i = min; i <= max; i++)
    {
        int k = i + (-min);
        EXPECT_EQ( table->getUtil(k), xMultC_i->getUtil(values={i}) );
    }
};


TEST_F(CpuTableTest, testConstructorConstraintWSum)
{
    WSumConstraint<int>::ptr wsum_i;

    int min = -5, max = 5;
    vector<int> weights = {1, 2};
    int rhs = 2;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    CpuBEAgent::ptr a2 = make_shared<CpuBEAgent>(1, "a2");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    Variable::ptr v2 = VariableFactory::create("v2", a2, min, max);
    vector<Variable::ptr> scope = {v1, v2};
    wsum_i = make_shared<WSumConstraint<int>>(scope, weights, BooleanExpression::EQ, rhs);

    SetUp(wsum_i);

    vector<value_t> values(2);

    for (int x = min; x <= max; x++)
    {
        for (int y = min; y <= max; y++)
        {
            int k = ((x + (-min)) * (max - min + 1)) + y + (-min);
            EXPECT_EQ( table->getUtil(k), wsum_i->getUtil(values={x,y}) );
        }
    }

};
