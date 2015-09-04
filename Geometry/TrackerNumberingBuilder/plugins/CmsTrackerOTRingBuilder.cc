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
}

void CmsTrackerOTRingBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
 GeometricDet::GeometricDetContainer & comp = det->components();

 if (comp.front()->type()==GeometricDet::DetUnit){ 

   TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhi());
   stable_sort(comp.begin(), comp.end() ,PhiSortNP());
   
 }
 else
   edm::LogError("CmsTrackerOTRingBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type(); 

  for(uint32_t i=0; i<comp.size();i++){
    comp[i]->setGeographicalID(i+1);
  } 
 
}
