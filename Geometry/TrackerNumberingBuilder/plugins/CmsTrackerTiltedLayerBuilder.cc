#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerTiltedLayerBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerOTRingBuilder.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerLadderBuilder.h"
#include "Geometry/TrackerNumberingBuilder/plugins/TrackerStablePhiSort.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>

#include <bitset>

void CmsTrackerTiltedLayerBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){

  CmsTrackerLadderBuilder theCmsTrackerLadderBuilder;
  CmsTrackerOTRingBuilder theCmsTrackerOTRingBuilder;

  GeometricDet * subdet = new GeometricDet(&fv,theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv)));
  switch (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv))){
  case GeometricDet::ladder:
    theCmsTrackerLadderBuilder.build(fv,subdet,s);      
    break;
  case GeometricDet::panel:
    theCmsTrackerOTRingBuilder.build( fv, subdet, s );
    break;
  default:
    edm::LogError("CmsTrackerTiltedLayerBuilder")<<" ERROR - I was expecting a ladder or a panel, I got a "<<ExtractStringFromDDD::getString(s,&fv);
  }  
  g->addComponent(subdet);

}

void CmsTrackerTiltedLayerBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){

  GeometricDet::GeometricDetContainer comp = det->components();

  //order ladder and rings together
  GeometricDet::GeometricDetContainer rods;
  GeometricDet::GeometricDetContainer ringsNeg;
  GeometricDet::GeometricDetContainer ringsPos;

  for(uint32_t i=0; i<comp.size();i++){
    if(comp[i]->type()== GeometricDet::ladder){
      rods.push_back(comp[i]);
    } else if(comp[i]->type()== GeometricDet::panel){
      if(comp[i]->translation().z() < 0.){
        ringsNeg.push_back(comp[i]);
      } else if (comp[i]->translation().z() > 0.) {
        ringsPos.push_back(comp[i]);
      }
    } else {
      edm::LogError("CmsTrackerTiltedLayerBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type();
    }
  }
      
  // negative rings before
  std::sort(ringsNeg.begin(),ringsNeg.end(),LessZ());  
  uint32_t  totalringsNeg = ringsNeg.size();

  for ( uint32_t ring = 0; ring < totalringsNeg; ring++) {
    uint32_t rn = ring+1;
    uint32_t part = 1;
    uint32_t temp = (part<<12) | rn;
    ringsNeg[ring]->setGeographicalID(temp);
  }

  // rods after
  TrackerStablePhiSort(rods.begin(), rods.end(), ExtractPhi());
  uint32_t  totalrods = rods.size();

  for ( uint32_t rod = 0; rod < totalrods; rod++) {
    uint32_t rd = rod+1;
    uint32_t part = 2;
    uint32_t temp = (part<<12) | rd;
    rods[rod]->setGeographicalID(DetId(temp));
  }

  // positive rings last
  std::sort(ringsPos.begin(),ringsPos.end(),LessZ());  
  uint32_t  totalringsPos = ringsPos.size();

  for ( uint32_t ring = 0; ring < totalringsPos; ring++) {
    uint32_t rn = ring+1;
    uint32_t part = 3;
    uint32_t temp = (part<<12) | rn;
    ringsPos[ring]->setGeographicalID(temp);
  }

  det->clearComponents();
  det->addComponents(ringsNeg);
  det->addComponents(rods);
  det->addComponents(ringsPos);

}

