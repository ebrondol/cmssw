#include "RecoLocalTracker/SiPixelVectorHitBuilder/plugins/SiPhase2RecHitMatcherESProducer.h"

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

boost::shared_ptr<SiPixelVectorHitBuilder> SiPhase2RecHitMatcherESProducer::
produce(const TkPixelCPERecord & iRecord)
{ 
  if( name == "SiPixelVectorHitMatcher" )
    matcher_  = boost::shared_ptr<SiPixelVectorHitBuilder>(new SiPixelVectorHitBuilder(pset_));
  return matcher_;
}


