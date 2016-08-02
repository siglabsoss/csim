#include <string>
#include <map>
#include <vector>

#pragma once

struct param_value_t
{
    enum type_t : uint8_t {
        PARAM_TYPE_FLOATING_POINT,
        PARAM_TYPE_INT
    };

    param_value_t();
    param_value_t(type_t type, void *data);
    param_value_t(const param_value_t &other);

    type_t type;
    union {
        int i;
        double f;
    } val;
};

class Parameters
{
public:
    static Parameters *get();

    void get(const std::string &handle, int &value) const;
    void get(const std::string &handle, double &value) const;

    void set(const std::string &handle, int value);
    void set(const std::string &handle, double value);

private:
    Parameters();
    void set(const std::string &handle, const param_value_t &param);
    void get(const std::string &handle, param_value_t &param) const;
    std::map<std::string, param_value_t>    m_params;

    static Parameters * g_instance;
};

extern Parameters g_params;

/* Performing some trickery here to execute code before main() by way of global
 * initialization with dummy struct definitions
 */
#define PARAM_DEFINE(name, value) \
    struct param__##name { \
        param__##name() { \
            Parameters::get()->set(#name, value); \
        }\
    } g_param__##name


#define param_get(name, value) Parameters::get()->get(name, value)
