#include "boost_pywrap.hpp"


PyWrap::PyWrap(int a)
{
	this->m_val = a;
}

void PyWrap::set(int a)
{
	this->m_val = a;
}
int PyWrap::get(void)
{
	return this->m_val;
}
