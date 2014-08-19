#include "TrackingTools/KalmanUpdators/interface/MRHKFUpdatorESProducer.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include <string>
#include <memory>

using namespace edm;

MRHKFUpdatorESProducer::MRHKFUpdatorESProducer(const edm::ParameterSet & p) 
{
  UpdatorName = p.getParameter<std::string>("ComponentName");
  pset_ = p;
  setWhatProduced(this,UpdatorName);
}

MRHKFUpdatorESProducer::~MRHKFUpdatorESProducer() {}

boost::shared_ptr<TrajectoryStateUpdator> 
MRHKFUpdatorESProducer::produce(const TrackingComponentsRecord & iRecord){ 

/*  std::string uname = pset_.getParameter<std::string>("simpleUpdator");
  edm::ESHandle<TrajectoryStateUpdator> upd;
  iRecord.getRecord<TrackingComponentsRecord>().get(uname, upd);

  _updator  = boost::shared_ptr<TrajectoryStateUpdator>(new MRHKFUpdator(upd.product()));
*/
  _updator  = boost::shared_ptr<TrajectoryStateUpdator>(new MRHKFUpdator());
  return _updator;

}


