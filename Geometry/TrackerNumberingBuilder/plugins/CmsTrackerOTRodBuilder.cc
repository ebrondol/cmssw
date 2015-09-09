#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerOTRodBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsPhase2OTDetConstruction.h"
#include <vector>

void CmsTrackerOTRodBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){

  CmsPhase2OTDetConstruction theCmsPhase2OTDetConstruction;
  theCmsPhase2OTDetConstruction.buildComponent(fv,g,s);  
}

void CmsTrackerOTRodBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
  GeometricDet::GeometricDetContainer & comp = det->components();

  if (comp.front()->type()==GeometricDet::DetUnit) 
    std::sort(comp.begin(),comp.end(),LessZ());
  else
   edm::LogError("CmsTrackerOTRodBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type(); 
  
 
  for(uint32_t i=0; i<comp.size();i++){
    comp[i]->setGeographicalID(i+1);
  } 
 

}




