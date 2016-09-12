#include "RecoTracker/GeometryESProducer/plugins/TrackerRecoGeometryESProducer.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoTracker/TkDetLayers/interface/GeometricSearchTrackerBuilder.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"


#include <memory>
#include <string>

using namespace edm;

TrackerRecoGeometryESProducer::TrackerRecoGeometryESProducer(const edm::ParameterSet & p) 
{
    setWhatProduced(this);
    // 08-Oct-2007 - Patrick Janot
    // Allow several reco geometries to be created, corresponding to the labelled  
    // TrackerDigiGeometry's - that must created beforehand. Useful to handle an 
    // aligned and a misaligned geometry in the same job. 
    // The default parameter ("") makes this change transparent to the user
    // See FastSimulation/Configuration/data/ for examples of cfi's.
    geoLabel = p.getUntrackedParameter<std::string>("trackerGeometryLabel","");
//    _usePhase2Stacks = false;
//    if (p.existsAs<edm::InputTag>("usePhase2Stacks")) {
      _usePhase2Stacks = p.getParameter<bool>("usePhase2Stacks");
//      std::cout << "exists useStacks! " << _usePhase2Stacks << std::endl;
//    }
}

TrackerRecoGeometryESProducer::~TrackerRecoGeometryESProducer() {}

std::shared_ptr<GeometricSearchTracker> 
TrackerRecoGeometryESProducer::produce(const TrackerRecoGeometryRecord & iRecord){ 

  edm::ESHandle<TrackerGeometry> tG;
  iRecord.getRecord<TrackerDigiGeometryRecord>().get( geoLabel, tG );

  edm::ESHandle<TrackerTopology> tTopoHand;
  iRecord.getRecord<TrackerTopologyRcd>().get(tTopoHand);
  const TrackerTopology *tTopo=tTopoHand.product();

  GeometricSearchTrackerBuilder builder;
  _tracker  = std::shared_ptr<GeometricSearchTracker>(builder.build( tG->trackerDet(), &(*tG), tTopo, _usePhase2Stacks ));
  return _tracker;
}


DEFINE_FWK_EVENTSETUP_MODULE(TrackerRecoGeometryESProducer);
