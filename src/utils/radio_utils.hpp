#pragma once

#include <filters/modulator.hpp>
#include <core/filter_chain.hpp>
#include <core/radio_set.hpp>


void construct_radio_set(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, Modulator::mod_scheme_t scheme = Modulator::MOD_SCHEME_BPSK);
void construct_radio_set_ebn0(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, double ebn0, Modulator::mod_scheme_t scheme = Modulator::MOD_SCHEME_BPSK);
