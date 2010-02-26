#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DQM/RPCMonitorDigi/interface/RPCMonitorDigi.h"
DEFINE_FWK_MODULE(RPCMonitorDigi);
#include "DQM/RPCMonitorDigi/interface/RPCTTUMonitor.h"
DEFINE_FWK_MODULE(RPCTTUMonitor);
#include "DQM/RPCMonitorDigi/interface/RPCEfficiency.h"
DEFINE_FWK_MODULE(RPCEfficiency);

