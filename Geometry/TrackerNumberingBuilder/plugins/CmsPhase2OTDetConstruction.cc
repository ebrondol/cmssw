#include "Geometry/TrackerNumberingBuilder/plugins/CmsPhase2OTDetConstruction.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"

void CmsPhase2OTDetConstruction::buildComponent(
					DDFilteredView& fv, 
					GeometricDet *mother, 
					std::string attribute){
  
  //
  // at this level I check whether it is a merged detector or not
  //

  LogTrace("DetConstruction") << " using CmsPhase2OTDetConstruction::buildComponent for " << ExtractStringFromDDD::getString(attribute,&fv);
  GeometricDet * det  = new GeometricDet(&fv,theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)));
  //FIXME::everytime is a GeometricDet::mergedDet !!
  if (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)) ==  GeometricDet::OTPhase2Stack){

  //if (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)) ==  GeometricDet::mergedDet){
    //
    // I have to go one step lower ...
    //
    bool dodets = fv.firstChild(); // descend to the first Layer
    while (dodets) {
      LogTrace("DetConstruction") << " stack det with new child! ";
      buildSmallDets(fv,det,attribute);
      dodets = fv.nextSibling(); // go to next layer
	/*
	Add algo to sort the merged DET
	*/
    }
    fv.parent();
  }
  
  mother->addComponent(det);
}

void CmsPhase2OTDetConstruction::buildSmallDets( 
					DDFilteredView& fv, 
					GeometricDet *mother, 
					std::string attribute){

  LogTrace("DetConstruction") << " using CmsPhase2OTDetConstruction::buildSmallDets for " << ExtractStringFromDDD::getString(attribute,&fv);
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

