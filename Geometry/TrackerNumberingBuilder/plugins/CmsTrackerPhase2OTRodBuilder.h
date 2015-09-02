#ifndef Geometry_TrackerNumberingBuilder_CmsTrackerPhase2OTRodBuilder_H
#define Geometry_TrackerNumberingBuilder_CmsTrackerPhase2OTRodBuilder_H

#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerLevelBuilder.h"
#include "FWCore/ParameterSet/interface/types.h"
#include <string>
/**
 * Class which constructs TOB rods
 */
class CmsTrackerPhase2OTRodBuilder : public CmsTrackerLevelBuilder {
  
 private:
  virtual void sortNS(DDFilteredView& , GeometricDet*);
  virtual void buildComponent(DDFilteredView& , GeometricDet*, std::string);

};

#endif
