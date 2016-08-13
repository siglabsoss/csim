#include <mathlib/complex_gaussian_noise.hpp>

ComplexGaussianNoise::ComplexGaussianNoise() :
    m_sampleGenerator(),
    m_realDistribution(0.0, 1.0),
    m_imagDistribution(0.0, 1.0)
{
}

ComplexGaussianNoise::ComplexGaussianNoise(double stddev) :
    m_sampleGenerator(),
    m_realDistribution(0.0, stddev),
    m_imagDistribution(0.0, stddev)
{
}

ComplexDouble ComplexGaussianNoise::getNext()
{
    return ComplexDouble(m_realDistribution(m_sampleGenerator), m_imagDistribution(m_sampleGenerator));
}
