//
// Created by Ferdinando Fioretto on 12/20/16.
//

#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <BE/CpuBEAgent.hpp>
#include <BE/CpuAggregator.hpp>
#include <BE/CpuProjector.hpp>

#include "BE/BESolver.hpp"
#include "Problem.hpp"

using namespace std;

class CpuAggregatorTest : public ::testing::Test
{
public:
    void SetUp(std::string file)
    {
        Problem::parse(file, InputParams::xml, InputParams::cpuBE);
        Problem::makePseudoTreeOrder(0);
        Cpu::Info::initialize();
        BESolver::ptr solver = std::make_shared<BESolver>();
    }

//    void SetUp(Constraint::ptr con1, Constraint::ptr con2)
//    {
//        table1 = std::make_shared<CpuTable>(con1);
//        table2 = std::make_shared<CpuTable>(con2);
//    }

protected:
    BESolver::ptr solver;

//    CpuTable::ptr table1;
//    CpuTable::ptr table2;
};



TEST_F(CpuAggregatorTest, aggregationTest_0)
{
    const std::string filename = "/Users/nandofioretto/Repositories/NMSU/CUDA/cpuBE/tests/data/test_aggr_1.xml";

    SetUp(filename);

    auto agents = Problem::getAgents();
    std::cout << "Agents:\n";
    for (auto agt : Problem::getAgents())
    {
        std::cout << agt->to_string() << "\n";
    }
    for (auto agt : agents)
    {
        auto BEagt = std::dynamic_pointer_cast<CpuBEAgent>(agt);
        auto agtTable = BEagt->getTable();
        std::cout << agtTable->to_string() << "\n";
    }


    auto agt = std::dynamic_pointer_cast<CpuBEAgent>(agents[1]);
    auto chAgt = std::dynamic_pointer_cast<CpuBEAgent>(agents[2]);
    auto chTable = chAgt->getTable();
    CpuAggregator::join(agt->getTable(), chTable);
    std::cout << agt->getTable()->to_string() << "\n";

    agt->getTable() = CpuProjector::project(agt->getTable(), agt->getVariable());

    std::cout << agt->getTable() << "\n";
}

#ifdef FALSE
TEST_F(CpuAggregatorTest, aggregationTest_1)
{
    const std::string filename = "Users/nandofioretto/Repositories/NMSU/CUDA/cpuBE/tests/data/test_aggr_1.xml";

    int min = 1, max = 2;
    int min2 = 1, max2 = 3;
    std::vector<Agent::ptr> agts;

    CpuBEAgent::ptr a1 = make_shared<CpuBEAgent>(0, "a1");
    CpuBEAgent::ptr a2 = make_shared<CpuBEAgent>(1, "a2");
    CpuBEAgent::ptr a3 = make_shared<CpuBEAgent>(2, "a3");
    Variable::ptr v1 = VariableFactory::create("v1", a1, min, max);
    Variable::ptr v2 = VariableFactory::create("v2", a2, min2, max2);
    Variable::ptr v3 = VariableFactory::create("v3", a3, min, max);

    std::vector<Variable::ptr> scope;
    TableConstraint::ptr con1 = make_shared<TableConstraint>(scope={v1, v2}, 0);
    TableConstraint::ptr con2 = make_shared<TableConstraint>(scope={v2, v3}, 0);

    vector<value_t> values(2);
    for (int i=min; i<=max; i++)
        for (int j=min2; j<=max2; j++)
            con1->setUtil(values={i, j}, i*j);

    for (int i=min2; i<=max2; i++)
        for (int j=min; j<=max; j++)
            con2->setUtil(values={i, j}, i*j);

    /////////////////////////
    Problem::addAgent(a1);
    Problem::addAgent(a2);
    Problem::addAgent(a3);
    Problem::makeMaps();
    Problem::makePseudoTreeOrder(0);


    std::cout << "Agents:\n";
    for (auto agt : Problem::getAgents())
    {
        std::cout << agt->to_string() << "\n";
    }

    ///////////////////////

    SetUp(con1, con2);

    std::cout << table1->to_string() << "\n";
    std::cout << table2->to_string() << "\n";

    CpuTable::ptr joinedTable = std::make_shared<CpuTable>(agts={a1,a2,a3});

    CpuAggregator::join(joinedTable, table1);

    std::cout << joinedTable->to_string() << "\n";

    CpuAggregator::join(joinedTable, table2);

    std::cout << joinedTable->to_string() << "\n";

//    values = {0,0};
//    EXPECT_EQ( table->getUtil(0), con->getUtil(values) );
//    values = {0,1};
//    EXPECT_EQ( table->getUtil(1), con->getUtil(values) );
//    values = {1,0};
//    EXPECT_EQ( table->getUtil(2), con->getUtil(values) );
//    values = {1,1};
//    EXPECT_EQ( table->getUtil(3), con->getUtil(values) );
};
#endif