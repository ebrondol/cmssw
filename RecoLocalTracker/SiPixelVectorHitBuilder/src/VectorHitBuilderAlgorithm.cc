#include "RecoLocalTracker/SiPixelVectorHitBuilder/interface/VectorHitBuilderAlgorithm.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit2D.h"

//FIXME::ERICA: not clear yet how to fullfill the acc/rej output
void VectorHitBuilderAlgorithm::run(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters, 
                                    VectorHitCollectionNew& vhAcc, 
                                    VectorHitCollectionNew& vhRej, 
                                    edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, 
                                    edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej)
{

  LogDebug("VectorHitBuilderAlgorithm") << "Run VectorHitBuilderAlgorithm ... \n" ;
  const  edmNew::DetSetVector<Phase2TrackerCluster1D>* ClustersPhase2Collection = clusters.product();


  std::map< DetId, std::vector<VectorHit> > temporary;
  //std::map< DetId, std::vector<VectorHit> > tempVHacc, tempVHrej;
  //std::map< DetId, std::vector<Phase2TrackerCluster1D> > tempCLacc, tempCLrej;
  std::map< DetId, std::vector<VectorHit> >::iterator it_temporary;

  //loop over the DetSetVector
  LogDebug("VectorHitBuilderAlgorithm") << "with #clusters : " << ClustersPhase2Collection->size() << std::endl ;
  for( auto DSViter : *ClustersPhase2Collection){

    unsigned int rawDetId1(DSViter.detId());
    DetId detId1(rawDetId1);
    DetId detId2 = theTkTopo->PartnerDetId(detId1);
    DetId detIdStack = theTkTopo->Stack(detId1);
    LogTrace("VectorHitBuilderAlgorithm") << "  DetId stack : " << detIdStack.rawId();
    LogTrace("VectorHitBuilderAlgorithm") << "  DetId lower cluster : " << rawDetId1;
    LogTrace("VectorHitBuilderAlgorithm") << "  DetId upper cluster : " << detId2.rawId();

    it_temporary = temporary.find(detIdStack);
    if ( it_temporary != temporary.end() ) {
      LogTrace("VectorHitBuilderAlgorithm") << " this stack has already been analyzed -> skip it ";
      //continue;
    }

    if ( theTkTopo->isLower(detId1) )
      LogTrace("VectorHitBuilderAlgorithm") << " is lower";
    else
      LogTrace("VectorHitBuilderAlgorithm") << " is not lower ";
    if ( theTkTopo->isUpper(detId2) )
      LogTrace("VectorHitBuilderAlgorithm") << " is upper ";
    else
      LogTrace("VectorHitBuilderAlgorithm") << " is not upper ";

    const GeomDet* gd;
    const StackGeomDet* stackDet;
    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator it_lower;
    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator it_upper;
/*
    if( theTkTopo->isLower(detId1) && theTkTopo->isUpper(detId2) ) {
      it_lower = ClustersPhase2Collection->find( detId1 );
      it_upper = ClustersPhase2Collection->find( detId2 );
    }
    if( theTkTopo->isLower(detId2) && theTkTopo->isUpper(detId1) ) {
      it_lower = ClustersPhase2Collection->find( detId2 );
      it_upper = ClustersPhase2Collection->find( detId1 );
    }
*/    
    if ( it_lower != ClustersPhase2Collection->end() && it_upper != ClustersPhase2Collection->end() ){
      gd = theTkGeom->idToDet(detIdStack);
      stackDet = dynamic_cast<const StackGeomDet*>(gd);
      std::vector<VectorHit> vhsInStack = buildVectorHits(*stackDet, clusters, *it_lower, *it_upper);
      temporary[detIdStack] = vhsInStack;
    }

  }

  loadDetSetVector(temporary, vhAcc);

  LogDebug("VectorHitBuilderAlgorithm") << "End run VectorHitBuilderAlgorithm ... \n" ;
  return;

}

bool VectorHitBuilderAlgorithm::checkClustersCompatibility(Local3DPoint& posinner, 
                                                           Local3DPoint& posouter, 
                                                           LocalError& errinner, 
                                                           LocalError& errouter)
{

  return true;

}

//----------------------------------------------------------------------------
//ERICA::in the DT code the global position is used to compute the alpha angle and put a cut on that.
std::vector<VectorHit> VectorHitBuilderAlgorithm::buildVectorHits(const StackGeomDet& stack, 
                                                                  edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters, 
                                                                  const detset & theLowerDetSet, 
                                                                  const detset & theUpperDetSet)
{

  LogTrace("VectorHitBuilderAlgorithm") << "  theLowerDetSet : " << theLowerDetSet.size();
  LogTrace("VectorHitBuilderAlgorithm") << "  theUpperDetSet : " << theUpperDetSet.size();
  std::vector<VectorHit> result;

  for ( const_iterator cil = theLowerDetSet.begin(); cil != theLowerDetSet.end(); ++ cil ) {

    Phase2TrackerCluster1DRef clusterLower = edmNew::makeRefTo( clusters, cil );

    for ( const_iterator ciu = theUpperDetSet.begin(); ciu != theUpperDetSet.end(); ++ ciu ) {

      Phase2TrackerCluster1DRef clusterUpper = edmNew::makeRefTo( clusters, ciu );
      VectorHit vh = buildVectorHit( stack, clusterLower, clusterUpper);
      LogTrace("VectorHitBuilderAlgorithm") << "  Vectorhit " << vh ;
      LogTrace("VectorHitBuilderAlgorithm") << std::endl;
      //protection: the VH can also be empty!!

      if (vh.isValid()){
        result.push_back(vh);
      }

    }
  }

  //sorting vhs for best chi2
  std::sort(result.begin(), result.end());

  if( result.size() > nMaxVHforeachStack ){
    result.erase(result.begin()+nMaxVHforeachStack, result.end());
  }

  return result;

}

VectorHit VectorHitBuilderAlgorithm::buildVectorHit(const StackGeomDet& stack, 
                                                    Phase2TrackerCluster1DRef inner, 
                                                    Phase2TrackerCluster1DRef outer)
{

  const GeomDetUnit* gDUnitInn = stack.lowerDet();
  const GeomDetUnit* gDUnitOut = stack.upperDet();

  //FIXME::you should put the correct error when the StripCPE is ready
  //FIXME StripClusterParameterEstimator::LocalValues parameters = parameterestimator->localParameters(*innerClus_iter,*gDUnitInn);
  MeasurementPoint mpCluInn(inner->center(), inner->column() + 0.5);
  Local3DPoint localPosCluInn = gDUnitInn->topology().localPosition(mpCluInn);
  MeasurementError meCluInn(1./12,0.0,1./12);
  LocalError localErrCluInn  = gDUnitInn->topology().localError(mpCluInn,meCluInn);

  //FIXME::you should put the correct error when the StripCPE is ready
  //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
  MeasurementPoint mpCluOut(outer->center(), outer->column() + 0.5);
  Local3DPoint localPosCluOut = gDUnitOut->topology().localPosition(mpCluOut);
  Global3DPoint globalPosCluOut = gDUnitOut->surface().toGlobal(localPosCluOut);
  Local3DPoint localPosCluOutINN = gDUnitInn->surface().toLocal(globalPosCluOut);
  MeasurementError meCluOut(1./12,0.0,1./12);
  LocalError localErrCluOutINN = gDUnitInn->topology().localError(mpCluOut,meCluOut);

  //debug
  Global3DPoint globalPosCluInn = gDUnitInn->surface().toGlobal(localPosCluInn);
  LogTrace("VectorHitBuilderAlgorithm") << "\t inner global pos " << globalPosCluInn;
  LogTrace("VectorHitBuilderAlgorithm") << "\t outer global pos " << globalPosCluOut;
  //LogTrace("VectorHitBuilderAlgorithm") << "\t outer local pos " << localPosCluOut;

  LogTrace("VectorHitBuilderAlgorithm") << "\t inner local pos " << localPosCluInn << " with error: " << localErrCluInn;
  LogTrace("VectorHitBuilderAlgorithm") << "\t outer local pos in the inner sof " << localPosCluOutINN << " with error: " << localErrCluOutINN;

  bool ok = checkClustersCompatibility(localPosCluInn, localPosCluOutINN, localErrCluInn, localErrCluOutINN);

  if(ok){

    //in the inner reference of frame
    Local3DVector localVecINN = localPosCluOutINN - localPosCluInn;
    LogTrace("VectorHitBuilderAlgorithm") << "\t local vec in the inner sof " << localVecINN;

    AlgebraicSymMatrix22 covMat2Dzx;
    double chi22Dzx = 0.0;
    Local3DPoint pos2Dzx;
    Local3DVector dir2Dzx;
    fit2Dzx(localPosCluInn, localPosCluOutINN, localErrCluInn,localErrCluOutINN, pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);
    VectorHit2D vh2Dzx = VectorHit2D(pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);

    AlgebraicSymMatrix22 covMat2Dzy;
    double chi22Dzy = 0.0;
    Local3DPoint pos2Dzy;
    Local3DVector dir2Dzy;
    fit2Dzy(localPosCluInn, localPosCluOutINN, localErrCluInn,localErrCluOutINN, pos2Dzy, dir2Dzy, covMat2Dzy, chi22Dzy);
    VectorHit2D vh2Dzy = VectorHit2D(pos2Dzy, dir2Dzy, covMat2Dzy, chi22Dzy);

    VectorHit vh = VectorHit(stack.lowerDet()->geographicalId(), vh2Dzx, vh2Dzy, inner, outer);
    return vh;

  }

  return VectorHit();

}



void VectorHitBuilderAlgorithm::fit2Dzx(const Local3DPoint lpCI, const Local3DPoint lpCO,
                                        const LocalError leCI, const LocalError leCO,
                                        Local3DPoint& pos, Local3DVector& dir,
                                        AlgebraicSymMatrix22& covMatrix,
                                        double& chi2)
{
  std::vector<float> x = {lpCI.z(), lpCO.z()};
  std::vector<float> y = {lpCI.x(), lpCO.x()};
  float sqCI = sqrt(leCI.xx());
  float sqCO = sqrt(leCO.xx());
  std::vector<float> sigy = {sqCI, sqCO};

  fit(x,y,sigy,pos,dir,covMatrix,chi2);

  return;

}

void VectorHitBuilderAlgorithm::fit2Dzy(const Local3DPoint lpCI, const Local3DPoint lpCO,
                                        const LocalError leCI, const LocalError leCO,
                                        Local3DPoint& pos, Local3DVector& dir,
                                        AlgebraicSymMatrix22& covMatrix,
                                        double& chi2)
{
  std::vector<float> x = {lpCI.z(), lpCO.z()};
  std::vector<float> y = {lpCI.y(), lpCO.y()};
  float sqCI = sqrt(leCI.yy());
  float sqCO = sqrt(leCO.yy());
  std::vector<float> sigy = {sqCI, sqCO};

  fit(x,y,sigy,pos,dir,covMatrix,chi2);

  return;

}

void VectorHitBuilderAlgorithm::fit(const std::vector<float>& x,
                                    const std::vector<float>& y,
                                    const std::vector<float>& sigy,
                                    Local3DPoint& pos, Local3DVector& dir,
                                    AlgebraicSymMatrix22& covMatrix,
                                    double& chi2)
{

  if(x.size() != y.size() || x.size() != sigy.size()){
    edm::LogError("VectorHitBuilderAlgorithm") << "Different size for x,z !! No fit possible.";
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
