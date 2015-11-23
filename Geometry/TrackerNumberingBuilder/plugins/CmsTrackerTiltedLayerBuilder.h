#ifndef Geometry_TrackerNumberingBuilder_CmsTrackerTiltedLayerBuilder_H
#define Geometry_TrackerNumberingBuilder_CmsTrackerTiltedLayerBuilder_H

#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerLevelBuilder.h"
#include "FWCore/ParameterSet/interface/types.h"
#include <string>
/**
 * Class which contructs TIB/TOB layers
 */
class CmsTrackerTiltedLayerBuilder : public CmsTrackerLevelBuilder {
  
 private:
  virtual void sortNS(DDFilteredView& , GeometricDet*);
  virtual void buildComponent(DDFilteredView& , GeometricDet*, std::string);

};

#endif
