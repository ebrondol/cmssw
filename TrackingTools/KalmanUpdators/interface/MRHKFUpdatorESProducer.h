#ifndef TrackingTools_ESProducers_MRHKFUpdatorESProducer_h
#define TrackingTools_ESProducers_MRHKFUpdatorESProducer_h

/** MRHKFUpdatorESProducer
 *  ESProducer for MRHKFUpdator class.
 *
 *  \author cerati
 */

#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/KalmanUpdators/interface/MRHKFUpdator.h"
#include <boost/shared_ptr.hpp>

class  MRHKFUpdatorESProducer: public edm::ESProducer{
 public:
  MRHKFUpdatorESProducer(const edm::ParameterSet & p);
  virtual ~MRHKFUpdatorESProducer(); 
  boost::shared_ptr<TrajectoryStateUpdator> produce(const TrackingComponentsRecord &);
 private:
  boost::shared_ptr<TrajectoryStateUpdator> _updator;
  edm::ParameterSet pset_;
  std::string UpdatorName;
};


#endif




