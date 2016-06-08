#include "TkStackMeasurementDet.h"

#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"
#include "RecoLocalTracker/SiPhase2VectorHitBuilder/interface/VectorHitBuilderAlgorithmBase.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const StackGeomDet* gdet,
                                              const VectorHitBuilderEDProducer* matcher,
                                              const PixelClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  theMatcher(matcher),
  thePixelCPE(cpe),
  theLowerDet(nullptr), theUpperDet(nullptr)
{}

void TkStackMeasurementDet::init(const MeasurementDet* lowerDet,
                                 const MeasurementDet* upperDet) {
  theLowerDet = dynamic_cast<const TkPhase2OTMeasurementDet *>(lowerDet);
  theUpperDet = dynamic_cast<const TkPhase2OTMeasurementDet *>(upperDet);

  if ((theLowerDet == 0) || (theUpperDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkPhase2OTMeasurementDet");
  }
}

TkStackMeasurementDet::RecHitContainer
TkStackMeasurementDet::recHits( const TrajectoryStateOnSurface& ts, const MeasurementTrackerEvent & data) const
{
  LogDebug("MeasurementTracker")<<"TkStackMeasurementDet::recHits";
  RecHitContainer result;
  if ( isEmpty(data.phase2OTData())== true ) return result;
  LogTrace("MeasurementTracker")<<" is not empty";
  if (isActive(data) == false) return result;
  LogTrace("MeasurementTracker")<<" and is active";

  const detset & lowerDetSet = data.phase2OTData().detSet(lowerDet()->index());
  const detset & upperDetSet = data.phase2OTData().detSet(upperDet()->index());
  LogTrace("MeasurementTracker")<<" DetSets set with sizes:" << lowerDetSet.size() << " and " << upperDetSet.size() << "!";
  result.reserve(lowerDetSet.size()>upperDetSet.size() ? lowerDetSet.size() : upperDetSet.size());

  VectorHitBuilderAlgorithmBase * algo = theMatcher->algo() ;
  //VectorHitBuilderAlgorithm* vhalgo = dynamic_cast<VectorHitBuilderAlgorithm *>(algobase);
  LogTrace("MeasurementTracker") << "TkStackMeasurementDet::recHits algo has been set" << std::endl;
  std::vector<VectorHit> vhs;
  vhs = algo->buildVectorHits(&specificGeomDet(), data.phase2OTData().handle(), lowerDetSet, upperDetSet, specificGeomDet().lowerDet(), specificGeomDet().upperDet());

  for ( auto vh : vhs ){
    LogTrace("MeasurementTracker") << "TkStackMeasurementDet::rechits adding VectorHits!" << std::endl;
    result.push_back( std::make_shared<VectorHit>( vh ));
  }

  return result;
}

bool TkStackMeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
                                          const MeasurementEstimator& est, const MeasurementTrackerEvent & data,
                                          TempMeasurements & result) const {
  LogDebug("MeasurementTracker")<<"TkStackMeasurementDet::measurements";

  if (!isActive(data)) {
    result.add(theInactiveHit, 0.F);
    return true;
  }
  
  LogTrace("MeasurementTracker")<<" is active";

  auto oldSize = result.size();
  MeasurementDet::RecHitContainer && allHits = recHits(stateOnThisDet, data);

  for (auto && hit : allHits) {
    std::pair<bool,double> diffEst = est.estimate( stateOnThisDet, *hit);
    LogDebug("MeasurementTracker")<< "New vh added with chi2: " << diffEst.second ;
    if ( diffEst.first)
      result.add(std::move(hit), diffEst.second);
  }

  if (result.size()>oldSize) return true;

  // create a TrajectoryMeasurement with an invalid RecHit and zero estimate
  result.add(theMissingHit, 0.F);
  LogDebug("MeasurementTracker")<< "adding missing hit";
  return false;
}
