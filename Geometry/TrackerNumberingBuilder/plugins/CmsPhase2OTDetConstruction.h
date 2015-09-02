#ifndef Geometry_TrackerNumberingBuilder_CmsPhase2OTDetConstruction_H
#define Geometry_TrackerNumberingBuilderCmsPhase2OTDetConstruction_H
#include<string>
#include<vector>
#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerLevelBuilder.h"
/**
 * Adds GeometricDets representing final modules to the previous level
 */
class CmsPhase2OTDetConstruction : public CmsTrackerLevelBuilder {
 public:
  void  buildComponent(DDFilteredView& , GeometricDet*, std::string);
 private:
  void buildDets(DDFilteredView& , GeometricDet* , std::string);
  void buildSmallDets(DDFilteredView& , GeometricDet* , std::string);
};



#endif
