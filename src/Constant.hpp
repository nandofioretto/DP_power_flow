//
// Created by Ferdinando Fioretto on 2/27/17.
//

#ifndef CPUBE_CONSTANT_HPP
#define CPUBE_CONSTANT_HPP

#include <memory>
#include <vector>
#include <string>

class Constant
{
public:
    typedef std::shared_ptr<Constant> ptr;

    Constant(const std::string &name) : _name(name), _nRows(1), _nCols(1)
    { }

    Constant(const std::string &name, double val)
            : _name(name), _nRows(1), _nCols(1)
    {
        _values.push_back(val);
    }

    Constant(const std::string &name, std::vector<double> vals)
            : _name(name), _nCols(vals.size()), _nRows(1)
    {
        _values = vals;
    }

    Constant(const std::string &name, std::vector<double> vals, int n_rows, int n_cols)
            : _name(name), _nRows(n_rows), _nCols(n_cols)
    {
        _values = vals;
    }

    double getValue() const
    {
        return _values[0];
    }

    double getValue(int i) const
    {
        return _values[i];
    }

    double getValue(int i, int j) const
    {
        return _values[i * _nRows + j];
    }

    bool isConstant() const
    {
        return (_nRows == 1 && _nCols == 1);
    }

    bool isVector() const
    {
        return (_nRows == 1 && _nCols > 1);
    }

    bool isMatrix() const
    {
        return (_nRows > 1 && _nCols > 1);
    }

    size_t size() const
    {
        return _values.size();
    }

    std::string to_string() const
    {
        std::string out;
        for (int r = 0; r < _nRows; r++)
        {
            for (int c = 0; c < _nCols; c++)
            {
                out += std::to_string(_values[r * _nRows + c]) + " ";
            }
            out += "\n";
        }
        return out;
    }

private:
    std::vector<double> _values;
    int _nRows;
    int _nCols;

    std::string _name;
};


#endif //CPUBE_CONSTANT_HPP
