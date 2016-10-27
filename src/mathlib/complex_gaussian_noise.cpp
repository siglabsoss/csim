#include <mathlib/complex_gaussian_noise.hpp>

ComplexGaussianNoise::ComplexGaussianNoise() :
    m_sampleGenerator(),
    m_realDistribution(0.0, 1.0),
    m_imagDistribution(0.0, 1.0)
{
}

ComplexGaussianNoise::ComplexGaussianNoise(double variance) :
    m_sampleGenerator(),
    m_realDistribution(0.0, sqrt(variance/2)), //set the variance of the individual components such that the user provided variance is the variances of the magnitude
    m_imagDistribution(0.0, sqrt(variance/2))
{
}

ComplexDouble ComplexGaussianNoise::getNext()
{
    return ComplexDouble(m_realDistribution(m_sampleGenerator), m_imagDistribution(m_sampleGenerator));
}
