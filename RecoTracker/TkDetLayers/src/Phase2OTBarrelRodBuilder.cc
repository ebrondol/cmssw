#include "Phase2OTBarrelRodBuilder.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace edm;
using namespace std;

Phase2OTBarrelRod* Phase2OTBarrelRodBuilder::build(const GeometricDet* thePhase2OTBarrelRod,
						   const TrackerGeometry* theGeomDetGeometry,
                                                   const bool usePhase2Stacks)
{  
  //std::cout << "Phase2OTBarrelRodBuilder::build usePhase2Stacks: " << usePhase2Stacks << std::endl;
  vector<const GeometricDet*> allGeometricDets = thePhase2OTBarrelRod->components();
  LogDebug("TkDetLayers") << "Phase2OTBarrelRodBuilder with #Modules: " << allGeometricDets.size() << std::endl;
  
  vector<const GeomDet*> innerGeomDets;
  vector<const GeomDet*> outerGeomDets;
  double meanR = 0;

  if(usePhase2Stacks){  
    vector<const GeometricDet*> compGeometricDets;
  
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

  } else {

    vector<const GeomDet*> innerGeomDetBrothers;
    vector<const GeomDet*> outerGeomDetBrothers;
  
    // compute meanR using the first and the third module because of the pt module pairs
    LogDebug("TkDetLayers") << "mean computed with " 
  				     << allGeometricDets[0]->positionBounds().perp() 
  				     << " and " << allGeometricDets[2]->positionBounds().perp() 
  				     << " and " << allGeometricDets[1]->positionBounds().perp() 
  				     << " and " << allGeometricDets[3]->positionBounds().perp() ;
    meanR = (allGeometricDets[0]->positionBounds().perp()+allGeometricDets[2]->positionBounds().perp())/2;
    double meanRBrothers = (allGeometricDets[1]->positionBounds().perp()+allGeometricDets[3]->positionBounds().perp())/2;
  
    unsigned int counter=0;
    for(vector<const GeometricDet*>::iterator it=allGeometricDets.begin(); it!=allGeometricDets.end(); it++,counter++){
      const GeomDet* theGeomDet = theGeomDetGeometry->idToDet( (*it)->geographicalID() );
  
      if(counter%2==0) {
        if( (*it)->positionBounds().perp() < meanR) 
  	innerGeomDets.push_back(theGeomDet);
      
        if( (*it)->positionBounds().perp() > meanR) 
  	outerGeomDets.push_back(theGeomDet);
      } else {
        if( (*it)->positionBounds().perp() < meanRBrothers) 
  	innerGeomDetBrothers.push_back(theGeomDet);
      
        if( (*it)->positionBounds().perp() > meanRBrothers) 
  	outerGeomDetBrothers.push_back(theGeomDet);
      }
    }
    
    LogDebug("TkDetLayers") << "innerGeomDets.size(): " << innerGeomDets.size() ;
    LogDebug("TkDetLayers") << "outerGeomDets.size(): " << outerGeomDets.size() ;
    LogDebug("TkDetLayers") << "innerGeomDetsBrothers.size(): " << innerGeomDetBrothers.size() ;
    LogDebug("TkDetLayers") << "outerGeomDetsBrothers.size(): " << outerGeomDetBrothers.size() ;
    return new Phase2OTBarrelRod(innerGeomDets,outerGeomDets,innerGeomDetBrothers,outerGeomDetBrothers);
  }

  return new Phase2OTBarrelRod(innerGeomDets,outerGeomDets);

}
