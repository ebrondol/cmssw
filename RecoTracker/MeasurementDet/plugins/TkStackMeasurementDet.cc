#include "TkStackMeasurementDet.h"

#include "TrackingTools/MeasurementDet/interface/MeasurementDetException.h"
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"
//#include "RecoTracker/TransientTrackingRecHit/interface/TVectorHit.h"

using namespace std;

TkStackMeasurementDet::TkStackMeasurementDet( const StackGeomDet* gdet,
                                              const PixelClusterParameterEstimator* cpe) :
  MeasurementDet(gdet),
  thePixelCPE(cpe),
  theLowerDet(nullptr), theUpperDet(nullptr),
  theFitter(new LinearFit())
{}

void TkStackMeasurementDet::init(const MeasurementDet* lowerDet,
                                 const MeasurementDet* upperDet) {
  theLowerDet = dynamic_cast<const TkPixelMeasurementDet *>(lowerDet);
  theUpperDet = dynamic_cast<const TkPixelMeasurementDet *>(upperDet);

  if ((theLowerDet == 0) || (theUpperDet == 0)) {
    throw MeasurementDetException("TkStackMeasurementDet ERROR: Trying to glue a det which is not a TkPixelMeasurementDet");
  }
}

TkStackMeasurementDet::RecHitContainer
TkStackMeasurementDet::recHits( const TrajectoryStateOnSurface& ts) const
{
  std::cout << "\tTkStackMeasurementDet::rechits ok" << std::endl;
  RecHitContainer result;
  if (Empty == true ) return result;
  if (isActive() == false) return result;
  const Phase2TrackerCluster1D* begin=0;
  if(0!=theHandle->data().size()) {
     begin = &(theHandle->data().front());
  }

  result.reserve(theLowerDetSet.size());

  for ( const_iterator cil = theLowerDetSet.begin(); cil != theLowerDetSet.end(); ++ cil ) {

    if (cil < begin){
      edm::LogError("IndexMisMatch")<<"TkStackMeasurementDet cannot create hit because of index mismatch.";
      return result;
    }

    Phase2TrackerCluster1DRef clusterLower = edmNew::makeRefTo( theHandle, cil );

    for ( const_iterator ciu = theUpperDetSet.begin(); ciu != theUpperDetSet.end(); ++ ciu ) {

      Phase2TrackerCluster1DRef clusterUpper = edmNew::makeRefTo( theHandle, ciu );
      std::cout << "\tTkStackMeasurementDet::rechits adding VectorHits!" << std::endl;
      result.push_back( buildVectorHit( clusterLower, clusterUpper, ts.localParameters() ) );

    }

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

    std::pair<bool,double> diffEst = est.estimate( stateOnThisDet, *hit);
    if ( diffEst.first)
      result.add(std::move(hit), diffEst.second);

  }
  if (result.size()>oldSize) return true;

  //here there is the possibility to add hasBadComponents
  return true;
}

TransientTrackingRecHit::RecHitPointer
TkStackMeasurementDet::buildVectorHit( const Phase2TrackerCluster1DRef & clusterLower,
                                       const Phase2TrackerCluster1DRef & clusterUpper,
                                       const LocalTrajectoryParameters & ltp) const
{
/*
  const GeomDetUnit& gDUnitLow( theLowerDet->specificGeomDet() );
  const GeomDetUnit& gDUnitUpp( theUpperDet->specificGeomDet() );

  //FIXME::you should put the correct error when the StripCPE is ready
  //FIXME StripClusterParameterEstimator::LocalValues parameters = parameterestimator->localParameters(*clusterLowerClus_iter,*gDUnitLow);
  MeasurementPoint mpCluLow(clusterLower->center(), clusterLower->column() + 0.5);
  Local3DPoint localPosCluLow = gDUnitLow.topology().localPosition(mpCluLow);
  MeasurementError meCluLow(1./12,0.0,1./12);
  LocalError localErrCluLow  = gDUnitLow.topology().localError(mpCluLow,meCluLow);

  //FIXME::you should put the correct error when the StripCPE is ready
  //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
  MeasurementPoint mpCluUpp(clusterUpper->center(), clusterUpper->column() + 0.5);
  Local3DPoint localPosCluUpp = gDUnitUpp.topology().localPosition(mpCluUpp);
  Global3DPoint globalPosCluUpp = gDUnitUpp.surface().toGlobal(localPosCluUpp);
  Local3DPoint localPosCluUppLOW = gDUnitLow.surface().toLocal(globalPosCluUpp);
  MeasurementError meCluUpp(1./12,0.0,1./12);
  LocalError localErrCluUppLOW = gDUnitLow.topology().localError(mpCluUpp,meCluUpp);

  //debug
  Global3DPoint globalPosCluLow = gDUnitLow.surface().toGlobal(localPosCluLow);
  std::cout << "\t clusterLower global pos " << globalPosCluLow;
  std::cout << "\t clusterUpper global pos " << globalPosCluUpp;
  std::cout << "\t clusterUpper local pos " << localPosCluUpp;

  std::cout << "\t clusterLower local pos " << localPosCluLow << " with error: " << localErrCluLow;
  std::cout << "\t clusterUpper local pos in the clusterLower sof " << localPosCluUppLOW << " with error: " << localErrCluUppLOW;

  bool ok = checkClustersCompatibility(localPosCluLow, localPosCluUppLOW, localErrCluLow, localErrCluUppLOW);
  if(ok){

    //in the clusterLower reference of frame
    Local3DVector localVecLOW = localPosCluUppLOW - localPosCluLow;
    LogTrace("TkStackMeasurementDet") << "\t local vec in the clusterLower sof " << localVecLOW;

    AlgebraicSymMatrix22 covMat2Dzx;
    double chi22Dzx = 0.0;
    Local3DPoint pos2Dzx;
    Local3DVector dir2Dzx;
    fit2Dzx(localPosCluLow, localPosCluUppLOW, localErrCluLow,localErrCluUppLOW, pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);
    VectorHit2D vh2Dzx = VectorHit2D(pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);

    AlgebraicSymMatrix22 covMat2Dzy;
    double chi22Dzy = 0.0;
    Local3DPoint pos2Dzy;
    Local3DVector dir2Dzy;
    fit2Dzy(localPosCluLow, localPosCluUppLOW, localErrCluLow,localErrCluUppLOW, pos2Dzy, dir2Dzy, covMat2Dzy, chi22Dzy);
    VectorHit2D vh2Dzy = VectorHit2D(pos2Dzy, dir2Dzy, covMat2Dzy, chi22Dzy);

//    const VectorHit* vh = new VectorHit(fastGeomDet().geographicalId(), vh2Dzx, vh2Dzy, clusterLower, clusterUpper);
    return TVectorHit::build( clusterLower, clusterUpper, vh2Dzx, vh2Dzy, &fastGeomDet(), thePixelCPE);
  }
*/
  return InvalidTransientRecHit::build(&geomDet(), TrackingRecHit::inactive);

}

bool TkStackMeasurementDet::checkClustersCompatibility (Local3DPoint& posLower, Local3DPoint& posUpper, LocalError& errLower, LocalError& errUpper) const{

  return true;

}
/*
void TkStackMeasurementDet::fit2Dzx(const Local3DPoint lpCI, const Local3DPoint lpCO,
                           const LocalError leCI, const LocalError leCO,
                           Local3DPoint& pos, Local3DVector& dir,
                           AlgebraicSymMatrix22& covMatrix,
                           double& chi2) const {
  std::vector<float> x = {lpCI.z(), lpCO.z()};
  std::vector<float> y = {lpCI.x(), lpCO.x()};
  float sqCI = sqrt(leCI.xx());
  float sqCO = sqrt(leCO.xx());
  std::vector<float> sigy = {sqCI, sqCO};

  fit(x,y,sigy,pos,dir,covMatrix,chi2);

  return;

}

void TkStackMeasurementDet::fit2Dzy(const Local3DPoint lpCI, const Local3DPoint lpCO,
                           const LocalError leCI, const LocalError leCO,
                           Local3DPoint& pos, Local3DVector& dir,
                           AlgebraicSymMatrix22& covMatrix,
                           double& chi2) const {
  std::vector<float> x = {lpCI.z(), lpCO.z()};
  std::vector<float> y = {lpCI.y(), lpCO.y()};
  float sqCI = sqrt(leCI.yy());
  float sqCO = sqrt(leCO.yy());
  std::vector<float> sigy = {sqCI, sqCO};

  fit(x,y,sigy,pos,dir,covMatrix,chi2);

  return;

}

void TkStackMeasurementDet::fit(const std::vector<float>& x,
                          const std::vector<float>& y,
                          const std::vector<float>& sigy,
                          Local3DPoint& pos, Local3DVector& dir,
                          AlgebraicSymMatrix22& covMatrix,
                          double& chi2) const {

  if(x.size() != y.size() || x.size() != sigy.size()){
    edm::LogError("TkStackMeasurementDet") << "Different size for x,z !! No fit possible.";
    return;
  }

  float slope     = 0.;
  float intercept = 0.;
  float covss     = 0.;
  float covii     = 0.;
  float covsi     = 0.;

  theFitter->fit(x,y,x.size(),sigy,slope,intercept,covss,covii,covsi);

  covMatrix[0][0] = covss; // this is var(dy/dz)
  covMatrix[1][1] = covii; // this is var(y)
  covMatrix[1][0] = covsi; // this is cov(dy/dz,y)

  for (unsigned int j=0; j < x.size(); j++){
    const double ypred = intercept + slope*x[j];
    const double dy = (y[j] - ypred)/sigy[j];
    chi2 += dy*dy;
 }

  pos = Local3DPoint(intercept,0.,0.);
  if(x.size()==2){
    //difference in z is the difference of the innermost and the outermost cluster z pos
    float slopeZ = x.at(1) - x.at(0);
    dir = LocalVector(slope,0.,slopeZ);
  } else {
    dir = LocalVector(slope,0.,-1.);
  }

}
*/
