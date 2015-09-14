#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerOTRingBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsPhase2OTDetConstruction.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/DetId/interface/DetId.h"
#include <vector>

#include "Geometry/TrackerNumberingBuilder/plugins/TrackerStablePhiSort.h"

void CmsTrackerOTRingBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){
  CmsPhase2OTDetConstruction theCmsPhase2OTDetConstruction;
  theCmsPhase2OTDetConstruction.buildComponent(fv,g,s);
/*
  CmsPhase2OTDetConstruction theCmsPhase2OTDetConstruction;
  switch (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv))){
  case GeometricDet::DetUnit:
           theCmsPhase2OTDetConstruction.buildComponent(fv,g,s);
    break;
  default:
    edm::LogError("CmsTrackerOTRingBuilder")<<" ERROR - I was expecting a Plaq, I got a "<<ExtractStringFromDDD::getString(s,&fv);
    ;
  }
*/
}

void CmsTrackerOTRingBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
 GeometricDet::GeometricDetContainer & comp = det->components();

 TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhi());
 //in Glued:   TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhiGluedModuleMirror());
 stable_sort(comp.begin(), comp.end() ,PhiSortNP());
   
  for(uint32_t i=0; i<comp.size();i++){
    comp[i]->setGeographicalID(i+1);
  } 

  if (comp.empty() ){
   edm::LogError("CmsTrackerOTRingBuilder")<<"Where are the Phase2 OT Ring's modules?";
  }
 
}
