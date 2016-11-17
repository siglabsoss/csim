#pragma once

#include <string>
#include <vector>
#include <types/complexdouble.hpp>

namespace DDCUtils
{
    void runLoopbackTest(const std::string &down2CoeffFile, const std::string &down5CoeffFile,
            const std::string &up2CoeffFile, const std::string &up5CoeffFile,
            const std::vector<ComplexDouble> &inputs, std::vector<ComplexDouble> &outputs);
}
