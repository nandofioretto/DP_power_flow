project(CpuBE)

#include_directories(BE)
#include_directories(MiniBE)
#include_directories(Utils)

set(SOURCE_FILES
        BE/BESolver.cpp
    BE/BESolver.hpp
    BE/CpuAggregator.cpp
    BE/CpuAggregator.hpp
    BE/CpuBEAgent.cpp
    BE/CpuBEAgent.hpp
    BE/CpuProjector.cpp
    BE/CpuProjector.hpp
    BE/CpuTable.cpp
    BE/CpuTable.hpp
    MiniBE/CpuMiniBEAgent.cpp
    MiniBE/CpuMiniBEAgent.hpp
    Utils/CpuAllocator.cpp
    Utils/CpuAllocator.hpp
    Utils/CpuInfo.cpp
    Utils/CpuInfo.hpp
    Utils/Permutation.hpp
    Utils/Timer.hpp
    Utils/Transform.hpp
    Agent.hpp
    AgentFactory.cpp
    AgentFactory.hpp
    Assert.hpp
    Constraint.hpp
    ConstraintFactory.cpp
    ConstraintFactory.hpp
    InputParams.cpp
    InputParams.hpp
    Preferences.cpp
    Preferences.hpp
    Problem.cpp
    Problem.hpp
    Solver.hpp
    TableConstraint.hpp
    WSumConstraint.hpp
    Types.cpp
    Types.hpp
    Variable.cpp
    Variable.hpp
    VariableFactory.cpp
    VariableFactory.hpp
        BooleanExpression.hpp
        MulConstraint.hpp
        Utils/xmlutils.h
        XmulCConstraint.hpp
        Constant.hpp ConstantFactory.hpp
        PbalanceConstraint.h
        QbalanceConstraint.h
        LineFlowConstraint.h)

add_library(cpuBE_core STATIC ${SOURCE_FILES})
