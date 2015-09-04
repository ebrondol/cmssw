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
  if (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)) ==  GeometricDet::mergedDet){
    //
    // I have to go one step lower ...
    //
    bool dodets = fv.firstChild(); // descend to the first Layer
    while (dodets) {
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
  //static const std::string stereo = "TrackerStereoDetectors";
  //if (ExtractStringFromDDD::getString(stereo,&fv) == "true"){
    uint32_t temp = 1;
    det->setGeographicalID(DetId(temp));
  //}else{
  //  uint32_t temp = 2;
  //  det->setGeographicalID(DetId(temp));
  //}
  
  mother->addComponent(det); 
}

