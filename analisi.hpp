#pragma once

#include <iostream>
#include <fstream>

#include "types.hpp"
#include "wrapper.hpp"
#include "my_histo.hpp"

#include <rapidjson/document.h>



using silicio_t=silicio<float>;
using detector_t=std::vector<silicio_t>;


void analisi_dati(detector_t&, const rapidjson::Document& p);
