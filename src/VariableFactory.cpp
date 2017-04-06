#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <rapidxml.hpp>

#include "Assert.hpp"
#include "VariableFactory.hpp"
#include "Variable.hpp"
#include "Agent.hpp"

using namespace rapidxml;

// Initializes static members
int VariableFactory::variablesCt = 0;


Variable::ptr VariableFactory::create(xml_node<>* varXML, 
                                     xml_node<>* domsXML, 
                                     std::vector<Agent::ptr> agents)
{
  // after you create the variable you can use the var id to identify it
  std::string name = varXML->first_attribute("name")->value();
  std::string ownerName = varXML->first_attribute("agent")->value();
  std::string domain = varXML->first_attribute("domain")->value();
  
  // Retrieve domain xml_node:
  xml_node<>* domXML = domsXML->first_node("domain");
  while (domain.compare(domXML->first_attribute("name")->value()) != 0)
  {
    domXML = domXML->next_sibling();
    ASSERT(domXML, 
      "No domain associated to variable " << name << " could be found.");
  }

  // look for owner in agents vector:
  Agent::ptr owner = nullptr;
  for (auto a : agents) {
    if( a->getName().compare(ownerName) == 0 )
      owner = a;
  }
  ASSERT(owner, 
    "No agent associated to variable " << name << " could be found.");
  
  std::string content = domXML->value();
  size_t ival = content.find("..");
  // ASSERT (ival != std::string::npos, "Cannot handle not contiguous domains");
  size_t idis = content.find("[");

  value_t min = 0, max = 0;
  if (ival == std::string::npos && idis == std::string::npos)
  {
    min = max = std::stod(content);
  }
  else
  {
    value_t min = std::stod(content.substr(0, ival));
    value_t max = std::stod(content.substr(ival + 2, idis - ival + 2));
  }
  value_t discr = (idis != std::string::npos)
                  ? std::stod(content.substr(idis+1))
                  : 1;

  return create(name, owner, min, max, discr);
}


Variable::ptr VariableFactory::create(std::string name, Agent::ptr owner, 
				     std::vector<value_t> dom)
{

  ASSERT(owner, "No agent associated to variable " << name << " given.");
    
  Variable::ptr var = std::make_shared<Variable>(dom, owner);
  var->setName( name );
  // Register variable in the agent owning it
  owner->addVariable( var );
  
  ++variablesCt;
  return var;
}


Variable::ptr VariableFactory::create(std::string name, Agent::ptr owner, 
				      value_t min, value_t max, value_t discr)
{
  ASSERT(owner, "No agent associated to variable " << name << " given.");
    
  Variable::ptr var = std::make_shared<Variable>(min, max, discr, owner);
  var->setName( name );
  // Register variable in the agent owning it
  owner->addVariable( var );
  ++variablesCt;

  return var;
}
