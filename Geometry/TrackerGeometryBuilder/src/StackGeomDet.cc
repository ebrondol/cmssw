#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"
#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"

StackGeomDet::StackGeomDet( Plane* sp, const GeomDetUnit* lowerDet, const GeomDetUnit* upperDet) : 
  GeomDet(sp),theLowerDet(lowerDet),theUpperDet(upperDet){
  child.push_back(theLowerDet);
  child.push_back(theUpperDet);
  SiStripDetId lowerId(theLowerDet->geographicalId().rawId());
  setDetId(lowerId.stack());	//in the Glued is used the info in the subdet
}

StackGeomDet::~StackGeomDet()
{}

std::vector<const GeomDet*> StackGeomDet::components() const {
  return child;
}

