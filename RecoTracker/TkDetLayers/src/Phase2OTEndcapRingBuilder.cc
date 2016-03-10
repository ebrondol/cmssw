#include "Phase2OTEndcapRingBuilder.h"

using namespace edm;
using namespace std;

Phase2OTEndcapRing* Phase2OTEndcapRingBuilder::build(const GeometricDet* aPhase2OTEndcapRing,
			 const TrackerGeometry* theGeomDetGeometry)
{
  vector<const GeometricDet*>  allGeometricDets = aPhase2OTEndcapRing->components();
  vector<const GeometricDet*>  compGeometricDets;

  LogDebug("TkDetLayers") << "Phase2OTEndcapRingBuilder with #Modules: " << allGeometricDets.size() << std::endl;

  vector<const GeomDet*> frontGeomDets;
  vector<const GeomDet*> backGeomDets;

  //---- to evaluate meanZ
  double meanZ = 0;
  for(vector<const GeometricDet*>::const_iterator compGeometricDets=allGeometricDets.begin(); compGeometricDets!=allGeometricDets.end();compGeometricDets++){
      LogTrace("TkDetLayers") << " compGeometricDets->positionBounds().perp() " << (*compGeometricDets)->positionBounds().z() << std::endl;
      meanZ = meanZ + (*compGeometricDets)->positionBounds().z();
  }
  meanZ = meanZ/allGeometricDets.size();
  LogDebug("TkDetLayers") << " meanZ Lower " << meanZ << std::endl;

  for(vector<const GeometricDet*>::const_iterator compGeometricDets=allGeometricDets.begin();
      compGeometricDets!=allGeometricDets.end();compGeometricDets++){
    const GeomDet* theGeomDet = theGeomDetGeometry->idToDet( (*compGeometricDets)->geographicalID() );
    LogTrace("TkDetLayers") << " inserisco " << (*compGeometricDets)->geographicalID().rawId() << std::endl;

    if( fabs( (*compGeometricDets)->positionBounds().z() ) < fabs(meanZ))
      frontGeomDets.push_back(theGeomDet);

    if( fabs( (*compGeometricDets)->positionBounds().z() ) > fabs(meanZ))
      backGeomDets.push_back(theGeomDet);      

  }

  LogDebug("TkDetLayers") << "frontGeomDets.size(): " << frontGeomDets.size() ;
  LogDebug("TkDetLayers") << "backGeomDets.size(): " << backGeomDets.size() ;

  return new Phase2OTEndcapRing(frontGeomDets,backGeomDets);

}
