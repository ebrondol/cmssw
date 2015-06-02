#include "RecoLocalTracker/SiPixelStubBuilder/interface/VectorHitBuilderAlgorithm.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit2D.h"

//FIXME::ERICA: not clear yet how to fullfill the acc/rej output
void VectorHitBuilderAlgorithm::run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters, VectorHitCollectionNew& vhAcc, VectorHitCollectionNew& vhRej,
  edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej){

  LogDebug("VectorHitBuilderAlgorithm") << "Run VectorHitBuilderAlgorithm ... " ;

  std::map< DetId, std::vector<VectorHit> > temporary;
  //std::map< DetId, std::vector<VectorHit> > tempVHacc, tempVHrej;
  //std::map< DetId, std::vector<Phase2TrackerCluster1D> > tempCLacc, tempCLrej;

  //loop over the DetSetVector
  edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator DSViter;
  for( DSViter = clusters.begin() ; DSViter != clusters.end(); DSViter++){

    unsigned int rawDetId1(DSViter->detId());
    DetId detId1(rawDetId1);

    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator DSViter2;
    bool sameStack = false;
    for( DSViter2 = DSViter+1 ; DSViter2 != clusters.end(); DSViter2++){

      unsigned int rawDetId2(DSViter2->detId());
      DetId detId2(rawDetId2);
      sameStack = checkModuleCompatibility(detId1, detId2);

      if(sameStack) {

        StackGeomDet stack = createNewStack(detId1, detId2);
        LogDebug("VectorHitBuilderAlgorithm") << "  Stack created with DetIds: " << rawDetId1 << "," << rawDetId2;
        std::vector<Phase2TrackerCluster1D> innerClustersInStack;
        std::vector<Phase2TrackerCluster1D> outerClustersInStack;

        //run on both DSV to find all the clusters associated to the stack
        edmNew::DetSet< Phase2TrackerCluster1D >::const_iterator clustIt;
        for ( clustIt = DSViter->begin(); clustIt != DSViter->end(); ++clustIt) {
          innerClustersInStack.push_back(*clustIt);
        }
        for ( clustIt = DSViter2->begin(); clustIt != DSViter2->end(); ++clustIt) {
          outerClustersInStack.push_back(*clustIt);
        }

        LogTrace("VectorHitBuilderAlgorithm") << "\t with " << int(innerClustersInStack.size() + outerClustersInStack.size()) << " clusters associated.";

        std::vector<VectorHit> vhsInStack = buildVectorHits(stack, innerClustersInStack, outerClustersInStack);
        temporary[rawDetId1] = vhsInStack;

        innerClustersInStack.clear();
        outerClustersInStack.clear();
        vhsInStack.clear();

      }
    }

  }

  loadDetSetVector(temporary, vhAcc);

  return;

}

bool VectorHitBuilderAlgorithm::checkModuleCompatibility(DetId detId1, DetId detId2){

  //exclude if are the same
  if(detId1 == detId2) return false;

  //exclude if they have different layer number
  if(getLayerNumber(detId1) != getLayerNumber(detId2)) return false;

  // if the module is odd (inner), I will search for the +1 (outer)
  unsigned int module1 = getModuleNumber(detId1);
  if(module1%2 != 0){
    unsigned int module2 = getModuleNumber(detId2);
    if(module2 == module1+1) {
      LogDebug("VectorHitBuilderAlgorithm") << "Modules ("<< module1 << "," << module2 <<") are compatible!";
      return true;
    }

  }
  return false;
}

bool VectorHitBuilderAlgorithm::checkClustersCompatibility(Local3DPoint& posinner, Local3DPoint& posouter, LocalError& errinner, LocalError& errouter){

  return true;

}

StackGeomDet VectorHitBuilderAlgorithm::createNewStack(DetId detId1, DetId detId2){

  //creation of the stack: the plane is the inner one
  const GeomDetUnit* geomDetUnit1(theTkGeom->idToDetUnit(detId1));
  const GeomDetUnit* geomDetUnit2(theTkGeom->idToDetUnit(detId2));
  const PixelGeomDetUnit* theGeomDet = dynamic_cast< const PixelGeomDetUnit* >(geomDetUnit1);
  Plane* sf = new Plane(theGeomDet->surface());

  if (!geomDetUnit1 || !geomDetUnit2) return StackGeomDet(sf);
  return StackGeomDet(sf,geomDetUnit1,geomDetUnit2);
}

//----------------------------------------------------------------------------
//ERICA::in the DT code the global position is used to compute the alpha angle and put a cut on that.
std::vector<VectorHit> VectorHitBuilderAlgorithm::buildVectorHits(StackGeomDet stack, std::vector<Phase2TrackerCluster1D> innerClus, std::vector<Phase2TrackerCluster1D> outerClus){

  std::vector<VectorHit> result;

  std::vector<Phase2TrackerCluster1D>::const_iterator innerClus_iter;
  for( innerClus_iter = innerClus.begin(); innerClus_iter != innerClus.end(); innerClus_iter++ ){

    std::vector<Phase2TrackerCluster1D>::const_iterator outerClus_iter;
    for( outerClus_iter = outerClus.begin(); outerClus_iter != outerClus.end(); outerClus_iter++ ){

      VectorHit vh = buildVectorHit( stack, *innerClus_iter, *outerClus_iter);
      LogTrace("VectorHitBuilderAlgorithm") << "\t vectorhit " << vh;

      //put a protection: the VH can also be empty!!
      //vh.isValid()?

      result.push_back(vh);

    }

  }
  //sorting vhs for best chi2
  std::sort(result.begin(), result.end());

  if( result.size() > nMaxVHforeachStack ){
    result.erase(result.begin()+nMaxVHforeachStack, result.end());
  }

  return result;

}

VectorHit VectorHitBuilderAlgorithm::buildVectorHit(StackGeomDet stack, const Phase2TrackerCluster1D& inner, const Phase2TrackerCluster1D& outer){

  const GeomDetUnit* gDUnitInn = stack.innerDet();
  const GeomDetUnit* gDUnitOut = stack.outerDet();

  //FIXME::you should put the correct error when the StripCPE is ready
  //FIXME StripClusterParameterEstimator::LocalValues parameters = parameterestimator->localParameters(*innerClus_iter,*gDUnitInn);
  MeasurementPoint mpCluInn(inner.center(), inner.column() + 0.5);
  Local3DPoint localPosCluInn = gDUnitInn->topology().localPosition(mpCluInn);
  MeasurementError meCluInn(1./12,0.0,1./12);
  LocalError localErrCluInn  = gDUnitInn->topology().localError(mpCluInn,meCluInn);

  //FIXME::you should put the correct error when the StripCPE is ready
  //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
  MeasurementPoint mpCluOut(outer.center(), outer.column() + 0.5);
  Local3DPoint localPosCluOut = gDUnitOut->topology().localPosition(mpCluOut);
  Global3DPoint globalPosCluOut = gDUnitOut->surface().toGlobal(localPosCluOut);
  Local3DPoint localPosCluOutINN = gDUnitInn->surface().toLocal(globalPosCluOut);
  MeasurementError meCluOut(1./12,0.0,1./12);
  LocalError localErrCluOutINN = gDUnitInn->topology().localError(mpCluOut,meCluOut);

  //debug
  //Global3DPoint globalPosCluInn = gDUnitInn->surface().toGlobal(localPosCluInn);
  //LogDebug("VectorHitBuilderAlgorithm") << "\t inner global pos " << globalPosCluInn;
  //LogDebug("VectorHitBuilderAlgorithm") << "\t outer global pos " << globalPosCluOut;
  //LogDebug("VectorHitBuilderAlgorithm") << "\t outer local pos " << localPosCluOut;

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

    VectorHit vh = VectorHit(vh2Dzx, vh2Dzy);
    return vh;

  }

  return VectorHit();

}



void VectorHitBuilderAlgorithm::fit2Dzx(const Local3DPoint lpCI, const Local3DPoint lpCO,
                           const LocalError leCI, const LocalError leCO,
                           Local3DPoint& pos, Local3DVector& dir,
                           AlgebraicSymMatrix22& covMatrix,
                           double& chi2){
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
                           double& chi2){
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
                          double& chi2){

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
  dir = LocalVector(-slope,0.,-1.);

}
