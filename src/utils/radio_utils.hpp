#pragma once

#include <filters/mapper.hpp>
#include <core/filter_chain.hpp>
#include <core/radio_set.hpp>


void construct_radio_set(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, MCS::modulation_t scheme = MCS::MOD_BPSK);
void construct_radio_set_ldpc_ebn0(RadioSet &rs, const std::vector <std::pair<double, double> > &coords, double ebn0);
