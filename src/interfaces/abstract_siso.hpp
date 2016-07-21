/**
 * Definition of an abstract single-input single-output system block
 */
#pragma once

template <typename INPUT_T, typename OUTPUT_T>
class AbstractSISO
{
public:
    virtual ~AbstractSISO() {}
    /**
     * input - consume a new input sample and process it.
     * @return false if the SISO block couldn't handle the input
     */
    virtual bool input(const INPUT_T &data) = 0;
    /**
     * output - provide an output sample to the caller.
     * @return false if no output sample is available.
     */
    virtual bool output(OUTPUT_T &data) = 0;

    virtual void tick() = 0;
};
