#pragma once
#include "algorithms/moead/moead.h"
#include "algorithms/moead_de/moead_de.h"
#include "algorithms/nsga2/nsga2.h"
#include "algorithms/ibea/ibea.h"
#include "algorithms/spea2/spea2.h"
#include "algorithms/spea2_sde/spea2_sde.h"
#include "algorithms/smsemoa/smsemoa.h"
#include "algorithms/moead_dra/moead_dra.h"
#include "algorithms/moead_frrmab/moead_frrmab.h"
#include "algorithms/hype/hype.h"
#include "algorithms/ens_moead/ens_moead.h"
#include "algorithms/moead_gra/moead_gra.h"
#include "algorithms/moead_ira/moead_ira.h"
#include "algorithms/moead_dyts/moead_dyts.h"
#include "algorithms/moead_swts/moead_swts.h"
#include "algorithms/moead_cdts/moead_cdts.h"
#include "algorithms/moead_dts/moead_dts.h"
#include "algorithms/moead_cde/moead_cde.h"
#include "algorithms/moead_stm/moead_stm.h"
#include "algorithms/rvea/rvea.h"
#include "algorithms/moead_pas/moead_pas.h"
#include "algorithms/moead_m2m/moead_m2m.h"

namespace emoc {

	void EMOCAlgorithmRegister();

}