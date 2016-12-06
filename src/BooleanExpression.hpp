//
// Created by Ferdinando Fioretto on 11/25/16.
//

#ifndef CPUBE_BOOLEANEXPRESSION_HPP
#define CPUBE_BOOLEANEXPRESSION_HPP

enum class BooleanExpression { EQ, NEQ, LEQ, GEQ, GT, LT, OR, AND, XOR, IFF, NOT};

inline BooleanExpression to_booleanExpression(const std::string& str) {

    std::string _str = str;
    std::transform(_str.begin(), _str.end(), _str.begin(), ::toupper);

    if (_str == "EQ") {
        return BooleanExpression::EQ;
    }
    if (_str == "LT") {
        return BooleanExpression::LT;
    }
    if (_str == "LEQ") {
        return BooleanExpression::LEQ;
    }
    if (_str == "GT") {
        return BooleanExpression::GT;
    }
    if (_str == "GEQ") {
        return BooleanExpression::GEQ;
    }
    if (_str == "OR") {
        return BooleanExpression::OR;
    }
    if (_str == "AND") {
        return BooleanExpression::AND;
    }
    if (_str == "XOR") {
        return BooleanExpression::OR;
    }
    if (_str == "IFF") {
        return BooleanExpression::IFF;
    }

    return BooleanExpression::NOT;
}
#endif //CPUBE_BOOLEANEXPRESSION_HPP
