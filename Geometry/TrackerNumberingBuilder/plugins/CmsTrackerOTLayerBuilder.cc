#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerOTLayerBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerOTRodBuilder.h"
#include "Geometry/TrackerNumberingBuilder/plugins/TrackerStablePhiSort.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>

#include <bitset>

void CmsTrackerOTLayerBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){

  CmsTrackerOTRodBuilder theCmsTrackerPhase2OTRodBuilder;

  GeometricDet * subdet = new GeometricDet(&fv,theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv)));
  switch (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv))){
  case GeometricDet::OTPhase2Rod:
    theCmsTrackerPhase2OTRodBuilder.build(fv,subdet,s);      
    break;
  default:
    edm::LogError("CmsTrackerOTLayerBuilder")<<" ERROR - I was expecting a OTPhase2Rod, I got a "<<ExtractStringFromDDD::getString(s,&fv);

  }  
  g->addComponent(subdet);

}

void CmsTrackerOTLayerBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){

  GeometricDet::GeometricDetContainer comp = det->components();

  if(det->components().front()->type()== GeometricDet::OTPhase2Rod){

    TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhi());

    for(uint32_t i=0; i<comp.size();i++){
      comp[i]->setGeographicalID(DetId(i+1));
    }

    det->clearComponents();
    det->addComponents(comp);
  }else{
    edm::LogError("CmsTrackerLayerBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type();
  }

}

