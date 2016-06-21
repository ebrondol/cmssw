#include "RecoLocalTracker/SiPhase2VectorHitBuilder/plugins/SiPhase2RecHitMatcherESProducer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"

SiPhase2RecHitMatcherESProducer::SiPhase2RecHitMatcherESProducer(const edm::ParameterSet & p) 
{
  std::cout << "ci siamo! :SiPhase2RecHitMatcherESProduce" << std::endl;
  name = p.getParameter<std::string>("ComponentName");
  pset_ = p;
  setWhatProduced(this,name);
}

std::shared_ptr<VectorHitBuilderEDProducer> SiPhase2RecHitMatcherESProducer::
produce(const TkPhase2OTCPERecord & iRecord)
{ 
  if( name == "SiPhase2VectorHitMatcher" ){
    matcher_  = std::make_shared<VectorHitBuilderEDProducer>(pset_);
//    matcher_  = std::shared_ptr<VectorHitBuilderEDProducer>(new VectorHitBuilderEDProducer(pset_));

    edm::ESHandle<TrackerGeometry> tGeomHandle;
    edm::ESHandle<TrackerTopology> tTopoHandle;
  
    iRecord.getRecord<TrackerDigiGeometryRecord>().get(tGeomHandle);
    iRecord.getRecord<TrackerTopologyRcd>().get(tTopoHandle);

    matcher_->algo()->initTkGeom(tGeomHandle);
    matcher_->algo()->initTkTopo(tTopoHandle);
  }
  return matcher_;
}


