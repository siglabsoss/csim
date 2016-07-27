#include <core/parameters.hpp>
#include <cassert>
#include <cstring>
#include <iostream>

param_value_t::param_value_t() :
        type(PARAM_TYPE_INT),
        val()
{}

param_value_t::param_value_t(type_t type, void *data)
{
    this->type = type;
    switch(type) {
        case PARAM_TYPE_INT:
            memcpy(&val.i, data, sizeof(val.i));
            break;
        case PARAM_TYPE_FLOATING_POINT:
            memcpy(&val.f, data, sizeof(val.f));
            break;
    }
}

param_value_t::param_value_t(const param_value_t &other)
{
    this->type = other.type;
    memcpy(&val, &(other.val), sizeof(this->val));
}

Parameters * Parameters::g_instance = nullptr;

Parameters * Parameters::get()
{
    if (g_instance == nullptr) {
        g_instance = new Parameters();
    }
    return g_instance;
}

Parameters::Parameters() :
        m_params()
{

}

void Parameters::get(const std::string &handle, param_value_t &param) const
{
    //TODO assert param exists
    param = m_params.at(handle);
}

void Parameters::get(const std::string &handle, double &value) const
{
    param_value_t param;
    get(handle, param);
    assert(param.type == param_value_t::PARAM_TYPE_FLOATING_POINT);
    value = param.val.f;
}

void Parameters::get(const std::string &handle, int &value) const
{
    param_value_t param;
    get(handle, param);
    assert(param.type == param_value_t::PARAM_TYPE_INT);
    value = param.val.i;
}

void Parameters::set(const std::string &handle, const param_value_t &param)
{
    std::string paramName = handle;
    m_params[paramName] = param;
}

void Parameters::set(const std::string &handle, int value)
{
    param_value_t param(param_value_t::PARAM_TYPE_INT, &value);
    set(handle, param);
}

void Parameters::set(const std::string &handle, double value)
{
    param_value_t param(param_value_t::PARAM_TYPE_FLOATING_POINT, &value);
    set(handle, param);
}

