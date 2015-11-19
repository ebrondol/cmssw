#include "TkStackMeasurementDet.h"

#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const StackGeomDet* gdet,
                                              const PixelClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  thePixelCPE(cpe),
  theInnerDet(nullptr), theOuterDet(nullptr)
{}

void TkStackMeasurementDet::init(const MeasurementDet* lowerDet,
                                 const MeasurementDet* upperDet) {
  theInnerDet = dynamic_cast<const TkPixelMeasurementDet *>(lowerDet);
  theOuterDet = dynamic_cast<const TkPixelMeasurementDet *>(upperDet);

  if ((theInnerDet == 0) || (theOuterDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkPixelMeasurementDet");
  }
}

TkStackMeasurementDet::RecHitContainer
TkStackMeasurementDet::recHits( const TrajectoryStateOnSurface& ts) const
{
  std::cout << "\tTkStackMeasurementDet::rechits ok" << std::endl;
  RecHitContainer result;
  if (Empty == true ) return result;
  std::cout << "\tTkStackMeasurementDet::rechits no empty" << std::endl;
  if (isActive() == false) return result;
  const Phase2TrackerCluster1D* begin=0;
  std::cout << "\tTkStackMeasurementDet::rechits set first cluster" << std::endl;
  if(0!=theHandle->data().size()) {
     begin = &(theHandle->data().front());
  }

  std::cout << "\tTkStackMeasurementDet::rechits cluster size" << theDetSet.size() << std::endl;
  result.reserve(theDetSet.size());

  for ( const_iterator ci = theDetSet.begin(); ci != theDetSet.end(); ++ ci ) {

    if (ci < begin){
      edm::LogError("IndexMisMatch")<<"TkPixelMeasurementDet cannot create hit because of index mismatch.";
      return result;
    }
    Phase2TrackerCluster1DRef cluster = edmNew::makeRefTo( theHandle, ci );
    //unsigned int rawid(cluster->detId());
    std::cout << "\tTkStackMeasurementDet::rechits adding clusters?" << std::endl;
//    result.push_back( buildRecHit( cluster, ts.localParameters() ) );

  }

  return result;
}

bool TkStackMeasurementDet::measurements( const TrajectoryStateOnSurface& stateOnThisDet,
                                          const MeasurementEstimator& est,
                                          TempMeasurements & result) const {
  std::cout << "\tTkStackMeasurementDet::measurements" << std::endl;

  //here there is the possibility to add if(isActive)

  auto oldSize = result.size();
  MeasurementDet::RecHitContainer && allHits = recHits(stateOnThisDet);

  for (auto && hit : allHits) {
    std::cout << "\tTkStackMeasurementDet::newhit" << hit->globalPosition() << std::endl;
/*
    std::pair<bool,double> diffEst = est.estimate( stateOnThisDet, *hit);
    if ( diffEst.first)
      result.add(std::move(hit), diffEst.second);
*/
  }
  if (result.size()>oldSize) return true;

  //here there is the possibility to add hasBadComponents
  return true;
}
/*
RecHitPointer
TkStackMeasurementDet::buildRecHit( const Phase2TrackerCluster1DRef & cluster,
                                    const LocalTrajectoryParameters & ltp) const
{
  const GeomDetUnit& gdu( specificGeomDet());
  LocalValues lv = theCPE->localParameters( * cluster, gdu, ltp );
  return TSiPixelRecHit::build( lv.first, lv.second, &fastGeomDet(), cluster, theCPE);
}
*/
