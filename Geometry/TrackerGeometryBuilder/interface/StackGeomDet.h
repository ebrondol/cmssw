#ifndef Geometry_TrackerGeometryBuilder_StackGeomDet_H
#define Geometry_TrackerGeometryBuilder_StackGeomDet_H

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "DataFormats/DetId/interface/DetId.h"

// FIXME::now the Stack object is not created by the geometry.
class StackGeomDet : public GeomDet{
public:

  StackGeomDet(Plane* sp) : GeomDet(sp) {};
  StackGeomDet( Plane* sp, const GeomDetUnit* innerDet,  const GeomDetUnit* outerDet);
  
  virtual ~StackGeomDet();

  virtual SubDetector subDetector() const { return theInnerDet->subDetector(); };
  virtual std::vector<const GeomDet*> components() const;

  const GeomDetUnit* innerDet() const { return theInnerDet; };
  const GeomDetUnit* outerDet() const { return theOuterDet; };

private:
  const GeomDetUnit* theInnerDet;
  const GeomDetUnit* theOuterDet;  
  std::vector<const GeomDet*> child;
};

#endif
