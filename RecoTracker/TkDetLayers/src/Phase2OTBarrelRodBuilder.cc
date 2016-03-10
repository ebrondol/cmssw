#include "Phase2OTBarrelRodBuilder.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace edm;
using namespace std;

Phase2OTBarrelRod* Phase2OTBarrelRodBuilder::build(const GeometricDet* thePhase2OTBarrelRod,
						   const TrackerGeometry* theGeomDetGeometry)
{  
  vector<const GeometricDet*> allGeometricDets = thePhase2OTBarrelRod->components();
  vector<const GeometricDet*> compGeometricDets;
  LogDebug("TkDetLayers") << "Phase2OTBarrelRodBuilder with #Modules: " << allGeometricDets.size() << std::endl;

  vector<const GeomDet*> innerGeomDets;
  vector<const GeomDet*> outerGeomDets;

  double meanR = 0;
  for(vector<const GeometricDet*>::const_iterator compGeometricDets=allGeometricDets.begin(); compGeometricDets!=allGeometricDets.end();compGeometricDets++){
    LogTrace("TkDetLayers") << " compGeometricDets.positionBounds().perp() " << (*compGeometricDets)->positionBounds().perp() << std::endl;
    meanR = meanR + (*compGeometricDets)->positionBounds().perp();
  }
  meanR = meanR/allGeometricDets.size();
  LogDebug("TkDetLayers") << " meanR Lower " << meanR << std::endl;

  for(vector<const GeometricDet*>::const_iterator compGeometricDets=allGeometricDets.begin(); compGeometricDets!=allGeometricDets.end(); compGeometricDets++){
    const GeomDet* theGeomDet = theGeomDetGeometry->idToDet( (*compGeometricDets)->geographicalID() );
    LogTrace("TkDetLayers") << " inserisco " << (*compGeometricDets)->geographicalID().rawId() << std::endl;

    if( (*compGeometricDets)->positionBounds().perp() < meanR)
      innerGeomDets.push_back(theGeomDet);

    if( (*compGeometricDets)->positionBounds().perp() > meanR)
      outerGeomDets.push_back(theGeomDet);
  }

  LogDebug("TkDetLayers") << "innerGeomDets.size(): " << innerGeomDets.size() ;
  LogDebug("TkDetLayers") << "outerGeomDets.size(): " << outerGeomDets.size() ;

  return new Phase2OTBarrelRod(innerGeomDets,outerGeomDets);
 
}
