#pragma once

#include <random>
#include <complex>

#include <types/complexdouble.hpp>

class ComplexGaussianNoise
{
public:
    ComplexGaussianNoise();
    ComplexGaussianNoise(double stddev);
    ComplexDouble getNext();

private:
    std::default_random_engine m_realGenerator;
    std::default_random_engine m_imagGenerator;
    std::normal_distribution<double> m_realDistribution;
    std::normal_distribution<double> m_imagDistribution;
};
