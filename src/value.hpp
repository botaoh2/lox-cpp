#pragma once

#include "stmt.hpp"

#include <unordered_map>
#include <variant>

enum class ValueType
{
    Nil = 0,
    Boolean,
    Number,
    String,
    Callable,
};

class Interpreter;
class Value;
class Environment;
class LoxInstance;

class ICallable
{
public:
    virtual ~ICallable() = default;
    virtual Value call(Interpreter& interpreter, const std::vector<Value>& arguments) = 0;
    virtual std::string toString() const = 0;
    virtual int arity() const = 0;
};

class LoxFunction : public ICallable
{
public:
    LoxFunction(const Stmt::Fun& declaration, std::shared_ptr<Environment> closure);

    std::string toString() const override { return fmt::format("<fun {}>", declaration.name.lexeme); }
    int arity() const override { return declaration.params.size(); }
    Value call(Interpreter& interpreter, const std::vector<Value>& arguments) override;
    LoxFunction bind(const Value& instance) const;

    const Stmt::Fun& declaration;
    std::shared_ptr<Environment> closure;
};

class LoxClass : public ICallable
{
public:
    LoxClass(const std::string& name, std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods)
        : name(name), methods(std::move(methods))
    {
    }

    std::string toString() const override { return fmt::format("<class {}>", name); }
    int arity() const override;
    Value call(Interpreter& interpreter, const std::vector<Value>& arguments) override;

public:
    std::string name;
    const std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
};

class LoxInstance
{
public:
    LoxInstance(LoxClass& clazz) : clazz(clazz) {}

    std::string toString() const;
    Value get(const Token& name) const;
    void set(const Token& name, const Value& value);

    LoxClass& clazz;

    std::unordered_map<std::string, Value> fields;
};

class Value
{
public:
    using Nil = std::monostate;
    using Boolean = bool;
    using Number = double;
    using String = std::string;
    using Callable = std::shared_ptr<ICallable>;
    using Instance = std::shared_ptr<LoxInstance>;

    explicit Value() : m_variant(Nil{}) {}
    explicit Value(bool value) : m_variant(Boolean{value}) {}
    explicit Value(double value) : m_variant(Number{value}) {}
    explicit Value(const std::string& value) : m_variant(String{value}) {}
    explicit Value(const Callable& value) : m_variant(Callable{value}) {}
    explicit Value(const Instance& value) : m_variant(Instance{value}) {}

    ValueType getType() const { return static_cast<ValueType>(m_variant.index()); }

    bool isNil() const { return std::holds_alternative<Nil>(m_variant); }
    bool isBoolean() const { return std::holds_alternative<Boolean>(m_variant); }
    bool isNumber() const { return std::holds_alternative<Number>(m_variant); }
    bool isString() const { return std::holds_alternative<String>(m_variant); }
    bool isCallable() const { return std::holds_alternative<Callable>(m_variant); }
    bool isInstance() const { return std::holds_alternative<Instance>(m_variant); }

    void setNil() { m_variant = Nil{}; }
    void setBoolean(const Boolean& boolean) { m_variant = boolean; }
    void setNumber(const Number& number) { m_variant = number; }
    void setString(const String& string) { m_variant = string; }
    void setCallable(const Callable& callable) { m_variant = callable; }
    void setInstance(const Instance& instance) { m_variant = instance; }

    Boolean getBoolean() const { return std::get<Boolean>(m_variant); }
    Number getNumber() const { return std::get<Number>(m_variant); }
    String getString() const { return std::get<String>(m_variant); }
    Callable getCallable() const { return std::get<Callable>(m_variant); }
    Instance getInstance() const { return std::get<Instance>(m_variant); }

private:
    std::variant<Nil, Boolean, Number, String, Callable, Instance> m_variant = Nil{};
};

std::string format_as(const Value& value);
