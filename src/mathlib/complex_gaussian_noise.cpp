#include <mathlib/complex_gaussian_noise.hpp>

ComplexGaussianNoise::ComplexGaussianNoise() :
    m_realGenerator(),
    m_imagGenerator(),
    m_realDistribution(0.0, 1.0),
    m_imagDistribution(0.0, 1.0)
{
}

ComplexGaussianNoise::ComplexGaussianNoise(double stddev) :
    m_realGenerator(),
    m_imagGenerator(),
    m_realDistribution(0.0, stddev),
    m_imagDistribution(0.0, stddev)
{
}

std::complex<double> ComplexGaussianNoise::getNext()
{
    return std::complex<double>(m_realDistribution(m_realGenerator), m_imagDistribution(m_imagGenerator));
}
