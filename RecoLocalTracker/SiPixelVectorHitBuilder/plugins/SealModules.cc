
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Utilities/interface/typelookup.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "RecoLocalTracker/SiPixelVectorHitBuilder/interface/SiPixelVectorHitBuilder.h"
#include "RecoLocalTracker/SiPixelVectorHitBuilder/plugins/SiPhase2RecHitMatcherESProducer.h"

DEFINE_FWK_EVENTSETUP_MODULE(SiPhase2RecHitMatcherESProducer);
DEFINE_FWK_MODULE(SiPixelVectorHitBuilder);

