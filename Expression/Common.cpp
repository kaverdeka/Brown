//
// Created by ka on 02.12.2021.
//

#include "Common.h"

class Value2 : public Expression {
public:
    Value2(int value) : _value(value) {}

    int Evaluate() const override {
        return _value;
    }

    std::string ToString() const override {
        return std::to_string(_value);
    }
private:
    int _value;
};

enum class OpType : uint8_t {
    PLUS,
    MULT
};

class Op : public Expression {
public:
    Op (ExpressionPtr&& ex1, ExpressionPtr&& ex2, OpType type) :
        _ex1(move(ex1)), _ex2(move(ex2)), _type(type) { }

    int Evaluate() const override {
       if (_type == OpType::MULT)
           return (_ex1->Evaluate()) * (_ex2->Evaluate());
       else if(_type == OpType::PLUS)
           return (_ex1->Evaluate()) + (_ex2->Evaluate());
       return  0;
    }

    std::string ToString() const override {
        if (_type == OpType::MULT)
            return "(" + (_ex1->ToString()) + ")*(" +  (_ex2->ToString()) + ")";
        else if(_type == OpType::PLUS)
            return "(" + (_ex1->ToString()) + ")+(" + (_ex2->ToString()) + ")";
        return std::string();
    }

private:
    ExpressionPtr _ex1;
    ExpressionPtr _ex2;
    OpType _type;
};

ExpressionPtr Value(int value) {
    return ExpressionPtr(new Value2(value));
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
    return ExpressionPtr(new Op(move(left), move(right), OpType::PLUS));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right){
    return ExpressionPtr(new Op(move(left), move(right), OpType::MULT));
}