//
// Created by Ferdinando Fioretto on 2/27/17.
//

#ifndef CPUBE_CONSTANTFACTORY_H
#define CPUBE_CONSTANTFACTORY_H

using namespace rapidxml;
using namespace misc_utils;

class ConstantFactory
{
public:
    static Constant::ptr create(rapidxml::xml_node<> *conXML)
    {
        std::string name = conXML->first_attribute("name")->value();
        std::string type = conXML->first_attribute("type")->value();
        int n_rows = 1;
        int n_cols = 1;

        if (type == "vector" || type == "matrix")
            n_rows = std::stod(conXML->first_attribute("nbRows")->value());
        if (type == "matrix")
            n_cols = std::stod(conXML->first_attribute("nbColumns")->value());

        size_t lhs = 0, rhs = 0;
        std::string content = conXML->value();

        // replace all the occurrences of 'infinity' with a 'INFTY'
        while (true)
        {
            rhs = content.find("infinity", lhs);
            if (rhs != std::string::npos)
                content.replace(rhs, 8, std::to_string(Constants::inf));
            else break;
        };

        bool multiple_cost;
        std::vector<double> values;
        double val;
        std::stringstream ss(content);

        while (ss.good())
        {
            ss >> val;
            values.push_back(val);
        }

        if (type == "constant")
            return std::make_shared<Constant>(name, values[0]);
        if (type == "vector")
            return std::make_shared<Constant>(name, values);
        if (type == "matrix")
            return std::make_shared<Constant>(name, values, n_rows, n_cols);

        return nullptr;
    }

};


#endif //CPUBE_CONSTANTFACTORY_H
