#include "Geometry/TrackerNumberingBuilder/plugins/CmsPhase2OTDetConstruction.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
/**
 *  * CmsDetConstruction class developed for Phase2
 *   */
void CmsPhase2OTDetConstruction::buildComponent(
					DDFilteredView& fv, 
					GeometricDet *mother, 
					std::string attribute){
  
  LogTrace("DetConstruction") << " CmsPhase2OTDetConstruction::buildComponent ";
  GeometricDet * det  = new GeometricDet(&fv,theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)));
  if (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)) ==  GeometricDet::OTPhase2Stack){

    bool dodets = fv.firstChild(); // descend to the first Layer
    while (dodets) {
      LogTrace("DetConstruction") << " new child! ";
      buildSmallDets(fv,det,attribute);
      dodets = fv.nextSibling(); // go to next layer
    }
    fv.parent();
  }
  mother->addComponent(det);
}

void CmsPhase2OTDetConstruction::buildSmallDets( 
					DDFilteredView& fv, 
					GeometricDet *mother, 
					std::string attribute){

  GeometricDet * det  = new GeometricDet(&fv, theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)));
  static const std::string isInner = "TrackerInnerDetectors";
  static const std::string isOuter = "TrackerOuterDetectors";
  if (ExtractStringFromDDD::getString(isInner,&fv) == "true"){
    LogTrace("DetConstruction") << " inner ";
    uint32_t temp = 1;
    det->setGeographicalID(DetId(temp));
  } else if (ExtractStringFromDDD::getString(isOuter,&fv) == "true"){
    LogTrace("DetConstruction") << " outer ";
    uint32_t temp = 2;
    det->setGeographicalID(DetId(temp));
  } else {
    edm::LogError("DetConstruction") << " module defined as a Stack but not inner either outer!? ";
  }
  mother->addComponent(det); 
}

