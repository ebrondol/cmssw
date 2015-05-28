#include "RecoLocalTracker/SiPixelStubBuilder/interface/VectorHitBuilder.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"

VectorHitCollectionNew VectorHitBuilder::run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters){

  VectorHitCollectionNew result;
  //std::vector< std::pair< StackGeomDet, std::vector<Phase2TrackerCluster1D> > > groupClusterBySM;
  //groupClusterBySM = groupinginStackModules(clusters);

  //loop over the DetSetVector
  edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator DSViter;
  for( DSViter = clusters.begin() ; DSViter != clusters.end(); DSViter++){

    unsigned int rawDetId1(DSViter->detId());
    DetId detId1(rawDetId1);

    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator DSViter2;
    for( DSViter2 = DSViter+1 ; DSViter2 != clusters.end(); DSViter2++){

      unsigned int rawDetId2(DSViter2->detId());
      DetId detId2(rawDetId2);
      bool sameStack = checkModuleCompatibility(detId1, detId2);

      if(sameStack) {

        StackGeomDet stack = createNewStack(detId1, detId2);
        std::cout << "\t Stack created with DetIds: " << rawDetId1 << "," << rawDetId2;
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

        std::cout << "\t with " << int(innerClustersInStack.size() + outerClustersInStack.size()) << " clusters associated." << std::endl;

        //ERICA::check if creating the VH here or somewhere else
        std::vector<VectorHit> vhInStack = buildVectorHits(stack, innerClustersInStack, outerClustersInStack);

        innerClustersInStack.clear();
        outerClustersInStack.clear();
      }
    }
  }

  return result;

}

bool VectorHitBuilder::checkModuleCompatibility(DetId detId1, DetId detId2){

  //exclude if are the same
  if(detId1 == detId2) return false;

  //exclude if they have different layer number
  if(getLayerNumber(detId1) != getLayerNumber(detId2)) return false;

  // if the module is odd (inner), I will search for the +1 (outer)
  unsigned int module1 = getModuleNumber(detId1);
  if(module1%2 != 0){
    unsigned int module2 = getModuleNumber(detId2);
    if(module2 == module1+1) {
      std::cout << "\t Modules ("<< module1 << "," << module2 <<") are compatible!" << std::endl;
      return true;
    }

  }
  return false;
}

StackGeomDet VectorHitBuilder::createNewStack(DetId detId1, DetId detId2){

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
std::vector<VectorHit> VectorHitBuilder::buildVectorHits(StackGeomDet stack, std::vector<Phase2TrackerCluster1D> innerClus, std::vector<Phase2TrackerCluster1D> outerClus){

  std::vector<VectorHit> result;
  std::vector<Phase2TrackerCluster1D>::const_iterator innerClus_iter;
  const GeomDetUnit* gDUnitInn = stack.innerDet();
  const GeomDetUnit* gDUnitOut = stack.outerDet();
  for( innerClus_iter = innerClus.begin(); innerClus_iter != innerClus.end(); innerClus_iter++ ){

    MeasurementPoint mpCluInn(innerClus_iter->center(), innerClus_iter->column() + 0.5);
    Local3DPoint localPosCluInn   = gDUnitInn->topology().localPosition(mpCluInn);

    //FIXME::you should put the correct error when the StripCPE is ready
    //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
    MeasurementError meCluInn(1./12,0.0,1./12);
    LocalError localErrCluInn  = gDUnitInn->topology().localError(mpCluInn,meCluInn);

    std::vector<Phase2TrackerCluster1D>::const_iterator outerClus_iter;
    for( outerClus_iter = outerClus.begin(); outerClus_iter != outerClus.end(); outerClus_iter++ ){

      MeasurementPoint mpCluOut(outerClus_iter->center(), outerClus_iter->column() + 0.5);
      Local3DPoint localPosCluOut = gDUnitOut->topology().localPosition(mpCluOut);
      Global3DPoint globalPosCluOut = gDUnitOut->surface().toGlobal(localPosCluOut);
      Local3DPoint localPosCluOutINN = gDUnitInn->surface().toLocal(globalPosCluOut);

      //FIXME::you should put the correct error when the StripCPE is ready
      //FIXME StripClusterParameterEstimator::LocalValues parameters =  parameterestimator->localParameters(*clustIt,geomDetUnit);
      MeasurementError meCluOut(1./12,0.0,1./12);
      LocalError localErrCluOutINN = gDUnitInn->topology().localError(mpCluOut,meCluOut);

      //in the inner reference of frame
      Local3DVector localVecINN = localPosCluOutINN - localPosCluInn;

      //debug
      //Global3DPoint globalPosCluInn = gDUnitInn->surface().toGlobal(localPosCluInn);
      //std::cout << "\t inner global pos " << globalPosCluInn << std::endl;
      //std::cout << "\t outer global pos " << globalPosCluOut << std::endl;
      //std::cout << "\t outer local pos " << localPosCluOut << std::endl;

      std::cout << "\t inner local pos " << localPosCluInn << " with error: " << localErrCluInn << std::endl;
      std::cout << "\t outer local pos in the inner sof " << localPosCluOutINN << " with error: " << localErrCluOutINN << std::endl;
      std::cout << "\t local vec in the inner sof " << localVecINN << std::endl;

      VectorHit vh = VectorHit(localPosCluInn, localVecINN);

      std::cout << "\t vectorhit " << vh << std::endl;
    }

  }

  return result;

}
//----------------------------------------------------------------------------
void VectorHitBuilder::fit(const std::vector<float>& x,
                          const std::vector<float>& y,
                          const std::vector<float>& sigy,
                          LocalPoint& pos,
                          LocalVector& dir,
                          AlgebraicSymMatrix& covMatrix,
                          double& chi2){

  std::cout << "VectorHitBuilder::fit" << std::endl;

}
//----------------------------------------------------------------------------
void VectorHitBuilder::buildDetUnit( const edm::DetSetVector<Phase2TrackerCluster1D> & input,
                                     output_t& output)  {

  buildDetUnit_(input, output);


}
