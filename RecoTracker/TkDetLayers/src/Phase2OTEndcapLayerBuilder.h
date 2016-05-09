#ifndef TkDetLayers_Phase2OTEndcapLayerBuilder_h
#define TkDetLayers_Phase2OTEndcapLayerBuilder_h


#include "Phase2OTEndcapLayer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"

/** A concrete builder for Phase2OTEndcapLayer 
 */

#pragma GCC visibility push(hidden)
class Phase2OTEndcapLayerBuilder {  
 public:
  Phase2OTEndcapLayerBuilder(){};
  Phase2OTEndcapLayer* build(const GeometricDet* aPhase2OTEndcapLayer,
		             const TrackerGeometry* theGeomDetGeometry,
                             const bool usePhase2Stacks);

   
};


#pragma GCC visibility pop
#endif 
