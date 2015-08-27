#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"

StackGeomDet::StackGeomDet( Plane* sp, const GeomDetUnit* innerDet, const GeomDetUnit* outerDet) : 
  GeomDet(sp),theInnerDet(innerDet),theOuterDet(outerDet){
  child.push_back(theInnerDet);
  child.push_back(theOuterDet);
  //Subdetector subdet(theInnerDet->geographicalId().rawId());
  setDetId(theInnerDet->geographicalId());
}

StackGeomDet::~StackGeomDet()
{}

std::vector<const GeomDet*> StackGeomDet::components() const {
  return child;
}

