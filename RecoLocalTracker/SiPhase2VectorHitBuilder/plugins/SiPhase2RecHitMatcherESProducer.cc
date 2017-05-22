#include "RecoLocalTracker/SiPhase2VectorHitBuilder/plugins/SiPhase2RecHitMatcherESProducer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "RecoLocalTracker/Records/interface/TkPhase2OTCPERecord.h"

SiPhase2RecHitMatcherESProducer::SiPhase2RecHitMatcherESProducer(const edm::ParameterSet & p)
{
  name = p.getParameter<std::string>("ComponentName");
//  cpeName_ = p.getParameter<std::string>("Phase2CPE_name");
  pset_ = p;
  setWhatProduced(this,name);
}

std::shared_ptr<VectorHitBuilderEDProducer> SiPhase2RecHitMatcherESProducer::
produce(const TkPhase2OTCPERecord & iRecord)
{ 
  if( name == "SiPhase2VectorHitMatcher" ){
    matcher_  = std::make_shared<VectorHitBuilderEDProducer>(pset_);

    edm::ESHandle<TrackerGeometry> tGeomHandle;
    edm::ESHandle<TrackerTopology> tTopoHandle;
//    edm::ESHandle<ClusterParameterEstimator<Phase2TrackerCluster1D> > cpeHandle;
  
    iRecord.getRecord<TrackerDigiGeometryRecord>().get(tGeomHandle);
    iRecord.getRecord<TrackerTopologyRcd>().get(tTopoHandle);
//    iRecord.getRecord<TkPhase2OTCPERecord>().get(cpeName_,cpeHandle);

    matcher_->algo()->initTkGeom(tGeomHandle);
    matcher_->algo()->initTkTopo(tTopoHandle);
//    matcher_->algo()->initCpe(cpeHandle);
  }
  return matcher_;
}


