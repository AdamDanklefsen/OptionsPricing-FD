#pragma once
#include <string>

struct OptionData {
    enum class Type { European, American, Call, Put };

    Type type = Type::European;
    double strike = 0.0;      // K
    double maturity = 0.0;    // T (years)
    double r = 0.0;           // risk-free rate
    double q = 0.0;           // dividend yield
    double sigma = 0.0;       // volatility
    std::string id;           // optional identifier

    bool is_call() const { return type == Type::Call; }
    bool is_american() const { return type == Type::American; }
};