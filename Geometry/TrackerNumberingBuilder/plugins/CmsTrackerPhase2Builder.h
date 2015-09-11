#ifndef Geometry_TrackerNumberingBuilder_CmsTrackerPhase2Builder_H
# define Geometry_TrackerNumberingBuilder_CmsTrackerPhase2Builder_H

# include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerLevelBuilder.h"
# include "FWCore/ParameterSet/interface/types.h"
# include <string>

/**
 * Class which builds the pixel phase 1 endcap
 */
class CmsTrackerPhase2Builder : public CmsTrackerLevelBuilder
{
public:
  CmsTrackerPhase2Builder();
  
private:
  virtual void sortNS( DDFilteredView& , GeometricDet* );
  virtual void buildComponent( DDFilteredView& , GeometricDet*, std::string );
};

#endif
