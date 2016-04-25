#include "Phase2OTEndcapRing.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TrackingTools/DetLayers/interface/DetLayerException.h"
#include "TrackingTools/DetLayers/interface/DetLayerException.h"
#include "TrackingTools/DetLayers/interface/MeasurementEstimator.h"
#include "TrackingTools/GeomPropagators/interface/HelixForwardPlaneCrossing.h"
#include "TrackingTools/DetLayers/interface/rangesIntersect.h"
#include "TrackingTools/DetLayers/interface/PhiLess.h"
#include "TrackingTools/DetLayers/interface/ForwardRingDiskBuilderFromDet.h"

#include "LayerCrossingSide.h"
#include "DetGroupMerger.h"
#include "CompatibleDetToGroupAdder.h"

#include "TkDetUtil.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include <boost/function.hpp>

using namespace std;

typedef GeometricSearchDet::DetWithState DetWithState;

class DetGroupElementZLess {
public:
  bool operator()(DetGroup a,DetGroup b)
  {
    return (fabs(a.front().det()->position().z()) < fabs(b.front().det()->position().z()));
  }
};

Phase2OTEndcapRing::Phase2OTEndcapRing(vector<const GeomDet*>& innerDets,
			       vector<const GeomDet*>& outerDets,
			       const vector<const GeomDet*>& innerDetBrothers,
			       const vector<const GeomDet*>& outerDetBrothers):
  theFrontDets(innerDets.begin(),innerDets.end()), 
  theBackDets(outerDets.begin(),outerDets.end()),
  theFrontDetBrothers(innerDetBrothers.begin(),innerDetBrothers.end()), 
  theBackDetBrothers(outerDetBrothers.begin(),outerDetBrothers.end())
{
  theDets.assign(theFrontDets.begin(),theFrontDets.end());
  theDets.insert(theDets.end(),theBackDets.begin(),theBackDets.end());
  theDets.insert(theDets.end(),theFrontDetBrothers.begin(),theFrontDetBrothers.end());
  theDets.insert(theDets.end(),theBackDetBrothers.begin(),theBackDetBrothers.end());

  theDisk = ForwardRingDiskBuilderFromDet()( theDets );

  theFrontDisk = ForwardRingDiskBuilderFromDet()( theFrontDets );
  theBackDisk  = ForwardRingDiskBuilderFromDet()( theBackDets );

  theFrontBinFinder = BinFinderType( theFrontDets.front()->surface().position().phi(),
				     theFrontDets.size());
  theBackBinFinder  = BinFinderType( theBackDets.front()->surface().position().phi(),
				     theBackDets.size());  


  LogDebug("TkDetLayers") << "==== DEBUG Phase2OTEndcapRing =====" ;
  if(theFrontDetBrothers.empty() && theBackDetBrothers.empty())   LogDebug("TkDetLayers") << "====       with stacks       =====" ;
  if(!theFrontDetBrothers.empty() && !theBackDetBrothers.empty()) LogDebug("TkDetLayers") << "====     without stacks      =====" ;
  for (vector<const GeomDet*>::const_iterator i=theFrontDets.begin();
       i != theFrontDets.end(); i++){
    LogDebug("TkDetLayers") << "inner Phase2OTEndcapRing's Det pos z,perp,eta,phi: "
                            << (**i).position().z() << " , "
                            << (**i).position().perp() << " , "
                            << (**i).position().eta() << " , "
                            << (**i).position().phi() ;
  }

  if(!theFrontDetBrothers.empty()){
    for (vector<const GeomDet*>::const_iterator i=theFrontDetBrothers.begin();
       i != theFrontDetBrothers.end(); i++){
      LogDebug("TkDetLayers") << "inner Phase2OTEndcapRing's Det Brother pos z,perp,eta,phi: "
                              << (**i).position().z() << " , "
                              << (**i).position().perp() << " , "
                              << (**i).position().eta() << " , "
                              << (**i).position().phi() ;
    }
  }

  for (vector<const GeomDet*>::const_iterator i=theBackDets.begin();
       i != theBackDets.end(); i++){
    LogDebug("TkDetLayers") << "outer Phase2OTEndcapRing's Det pos z,perp,eta,phi: "
                            << (**i).position().z() << " , "
                            << (**i).position().perp() << " , "
                            << (**i).position().eta() << " , "
                            << (**i).position().phi() ;
  }

  if(!theBackDetBrothers.empty()){
    for (vector<const GeomDet*>::const_iterator i=theBackDetBrothers.begin();
         i != theBackDetBrothers.end(); i++){
      LogDebug("TkDetLayers") << "outer Phase2OTEndcapRing's Det Brother pos z,perp,eta,phi: "
                            << (**i).position().z() << " , "
                            << (**i).position().perp() << " , "
                            << (**i).position().eta() << " , "
                            << (**i).position().phi() ;
    }
  }
  LogDebug("TkDetLayers") << "==== end DEBUG Phase2OTEndcapRing =====" ;

}

Phase2OTEndcapRing::~Phase2OTEndcapRing(){

} 

const vector<const GeometricSearchDet*>& 
Phase2OTEndcapRing::components() const 
{
  throw DetLayerException("Phase2OTEndcapRing doesn't have GeometricSearchDet components");
}

  
pair<bool, TrajectoryStateOnSurface>
Phase2OTEndcapRing::compatible( const TrajectoryStateOnSurface&, const Propagator&, 
		  const MeasurementEstimator&) const{
  edm::LogError("TkDetLayers") << "temporary dummy implementation of Phase2OTEndcapRing::compatible()!!" ;
  return pair<bool,TrajectoryStateOnSurface>();
}



void 
Phase2OTEndcapRing::groupedCompatibleDetsV( const TrajectoryStateOnSurface& tsos,
				 const Propagator& prop,
				 const MeasurementEstimator& est,
				 std::vector<DetGroup>& result) const
{
  SubLayerCrossings  crossings; 
  crossings = computeCrossings( tsos, prop.propagationDirection());
  if(! crossings.isValid()) return;


  std::vector<DetGroup> closestResult;
  std::vector<DetGroup> closestBrotherResult;
  addClosest( tsos, prop, est, crossings.closest(), closestResult,closestBrotherResult); 
  if (closestResult.empty())     return;
  
  DetGroupElement closestGel( closestResult.front().front());  
  int crossingSide = LayerCrossingSide().endcapSide( closestGel.trajectoryState(), prop);
  float phiWindow =  tkDetUtil::computeWindowSize( closestGel.det(), closestGel.trajectoryState(), est); 
  searchNeighbors( tsos, prop, est, crossings.closest(), phiWindow,
		   closestResult, closestBrotherResult, false); 

  vector<DetGroup> closestCompleteResult;
  DetGroupMerger::orderAndMergeTwoLevels(std::move(closestResult),std::move(closestBrotherResult),closestCompleteResult,
					 0, crossingSide);

  vector<DetGroup> nextResult;
  vector<DetGroup> nextBrotherResult;
  searchNeighbors( tsos, prop, est, crossings.other(), phiWindow,
		   nextResult, nextBrotherResult, true); 

  vector<DetGroup> nextCompleteResult;
  DetGroupMerger::orderAndMergeTwoLevels(std::move(nextResult),std::move(nextBrotherResult),nextCompleteResult,
					 0, crossingSide);

  DetGroupMerger::orderAndMergeTwoLevels( std::move(closestCompleteResult), std::move(nextCompleteResult), result,
					  crossings.closestIndex(), crossingSide);

  //due to propagator problems, when we add single pt sub modules, we should order them in z (endcap)
  sort(result.begin(),result.end(),DetGroupElementZLess());
  for (auto&  grp : result) {
    if ( grp.empty() )  continue;
    LogTrace("TkDetLayers") <<"New group in Phase2OTEndcapRing made by : ";
    for (auto const & det : grp) {
      LogTrace("TkDetLayers") <<" geom det at r: " << det.det()->position().perp() <<" id:" << det.det()->geographicalId().rawId()
                              <<" tsos at:" << det.trajectoryState().globalPosition();
    }
  }

}


SubLayerCrossings 
Phase2OTEndcapRing::computeCrossings(const TrajectoryStateOnSurface& startingState,
			  PropagationDirection propDir) const
{
  double rho( startingState.transverseCurvature());
  
  HelixPlaneCrossing::PositionType startPos( startingState.globalPosition() );
  HelixPlaneCrossing::DirectionType startDir( startingState.globalMomentum() );
  HelixForwardPlaneCrossing crossing(startPos,startDir,rho,propDir);

  pair<bool,double> frontPath = crossing.pathLength( *theFrontDisk);
  if (!frontPath.first) return SubLayerCrossings();

  GlobalPoint gFrontPoint(crossing.position(frontPath.second));

  int frontIndex = theFrontBinFinder.binIndex(gFrontPoint.phi()); 
  SubLayerCrossing frontSLC( 0, frontIndex, gFrontPoint);



  pair<bool,double> backPath = crossing.pathLength( *theBackDisk);
  if (!backPath.first) return SubLayerCrossings();

  GlobalPoint gBackPoint( crossing.position(backPath.second));
  int backIndex = theBackBinFinder.binIndex(gBackPoint.phi());
  SubLayerCrossing backSLC( 1, backIndex, gBackPoint);

  
  // 0ss: frontDisk has index=0, backDisk has index=1
  float frontDist = std::abs(Geom::deltaPhi( double(gFrontPoint.barePhi()), 
					     double(theFrontDets[frontIndex]->surface().phi())));
  float backDist = std::abs(Geom::deltaPhi( double(gBackPoint.barePhi()), 
					    double(theBackDets[backIndex]->surface().phi())));


  if (frontDist < backDist) {
    return SubLayerCrossings( frontSLC, backSLC, 0);
  }
  else {
    return SubLayerCrossings( backSLC, frontSLC, 1);
  } 
}

bool Phase2OTEndcapRing::addClosest( const TrajectoryStateOnSurface& tsos,
				 const Propagator& prop,
				 const MeasurementEstimator& est,
				 const SubLayerCrossing& crossing,
				 vector<DetGroup>& result,
				 vector<DetGroup>& brotherresult) const
{
  const vector<const GeomDet*>& sub( subLayer( crossing.subLayerIndex()));
  const GeomDet* det(sub[crossing.closestDetIndex()]);
  bool firstgroup = CompatibleDetToGroupAdder::add( *det, tsos, prop, est, result); 
  if(theFrontDetBrothers.empty() && theBackDetBrothers.empty())   return firstgroup;
  // it assumes that the closestDetIndex is ok also for the brother detectors: the crossing is NOT recomputed
  const vector<const GeomDet*>& subBrothers( subLayerBrothers( crossing.subLayerIndex()));
  const GeomDet* detBrother(subBrothers[crossing.closestDetIndex()]);
  bool brothergroup = CompatibleDetToGroupAdder::add( *detBrother, tsos, prop, est, brotherresult); 
  return firstgroup || brothergroup;
}



void Phase2OTEndcapRing::searchNeighbors( const TrajectoryStateOnSurface& tsos,
				     const Propagator& prop,
				     const MeasurementEstimator& est,
				     const SubLayerCrossing& crossing,
				     float window, 
				     vector<DetGroup>& result,
				     vector<DetGroup>& brotherresult,
				     bool checkClosest) const
{
  GlobalPoint gCrossingPos = crossing.position();

  const vector<const GeomDet*>& sLayer( subLayer( crossing.subLayerIndex()));
  // It assumes that what is ok for the front modules in the pt modules is ok also for the back module
  const vector<const GeomDet*>& sBrotherLayer( subLayerBrothers( crossing.subLayerIndex()));
 
  int closestIndex = crossing.closestDetIndex();
  int negStartIndex = closestIndex-1;
  int posStartIndex = closestIndex+1;

  if (checkClosest) { // must decide if the closest is on the neg or pos side
    if ( Geom::phiLess( gCrossingPos.barePhi(), sLayer[closestIndex]->surface().phi())) {
      posStartIndex = closestIndex;
    }
    else {
      negStartIndex = closestIndex;
    }
  }

  const BinFinderType& binFinder = (crossing.subLayerIndex()==0 ? theFrontBinFinder : theBackBinFinder);

  typedef CompatibleDetToGroupAdder Adder;
  int half = sLayer.size()/2;  // to check if dets are called twice....
  for (int idet=negStartIndex; idet >= negStartIndex - half; idet--) {
    const GeomDet & neighborDet = *sLayer[binFinder.binIndex(idet)];
    if (!tkDetUtil::overlapInPhi( gCrossingPos, neighborDet, window)) break;
    if (!Adder::add( neighborDet, tsos, prop, est, result)) break;
    if(theFrontDetBrothers.empty() && theBackDetBrothers.empty()) break;
    // If the two above checks are passed also the brother module will be added with no further checks
    const GeomDet & neighborBrotherDet = *sBrotherLayer[binFinder.binIndex(idet)];
    Adder::add( neighborBrotherDet, tsos, prop, est, brotherresult);
    // maybe also add shallow crossing angle test here???
  }
  for (int idet=posStartIndex; idet < posStartIndex + half; idet++) {
    const GeomDet & neighborDet = *sLayer[binFinder.binIndex(idet)];
    if (!tkDetUtil::overlapInPhi( gCrossingPos, neighborDet, window)) break;
    if (!Adder::add( neighborDet, tsos, prop, est, result)) break;
    if(theFrontDetBrothers.empty() && theBackDetBrothers.empty()) break;
    // If the two above checks are passed also the brother module will be added with no further checks
    const GeomDet & neighborBrotherDet = *sBrotherLayer[binFinder.binIndex(idet)];
    Adder::add( neighborBrotherDet, tsos, prop, est, brotherresult);
    // maybe also add shallow crossing angle test here???
  }
}
