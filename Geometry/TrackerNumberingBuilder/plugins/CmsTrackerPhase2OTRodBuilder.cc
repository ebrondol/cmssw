#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerPhase2OTRodBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsPhase2OTDetConstruction.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>


void CmsTrackerPhase2OTRodBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){
  CmsPhase2OTDetConstruction theCmsPhase2OTDetConstruction;
  theCmsPhase2OTDetConstruction.buildComponent(fv,g,s);  
}

void CmsTrackerPhase2OTRodBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
  GeometricDet::GeometricDetContainer & comp = det->components();

  std::stable_sort(comp.begin(),comp.end(),LessModZ()); 	

  for(uint32_t i=0; i<comp.size();i++){
     comp[i]->setGeographicalID(i+1);
  }
    
  if (comp.empty() ){
   edm::LogError("CmsTrackerPhase2OTRodBuilder")<<"Where are the Phase2OTRod's modules?";
  }
}
