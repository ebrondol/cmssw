#ifndef RecoLocaltracker_SiPixelVectorHitBuilder_SiPhase2RecHitMatcherESProducer_h
#define RecoLocaltracker_SiPixelVectorHitBuilder_SiPhase2RecHitMatcherESProducer_h

#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoLocalTracker/Records/interface/TkPixelCPERecord.h"
#include "RecoLocalTracker/SiPixelVectorHitBuilder/plugins/SiPixelVectorHitBuilder.h"
#include <boost/shared_ptr.hpp>

class SiPhase2RecHitMatcherESProducer: public edm::ESProducer {
 public:
  SiPhase2RecHitMatcherESProducer(const edm::ParameterSet&);
  boost::shared_ptr<SiPixelVectorHitBuilder> produce(const TkPixelCPERecord&);
 private:
  std::string name;
  boost::shared_ptr<SiPixelVectorHitBuilder> matcher_;
  edm::ParameterSet pset_;
};
#endif




