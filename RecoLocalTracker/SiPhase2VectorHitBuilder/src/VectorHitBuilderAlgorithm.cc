#include "RecoLocalTracker/SiPhase2VectorHitBuilder/interface/VectorHitBuilderAlgorithm.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "DataFormats/TrackerRecHit2D/interface/VectorHit2D.h"


bool VectorHitBuilderAlgorithm::LocalPositionSort::operator()(Phase2TrackerCluster1DRef clus1, Phase2TrackerCluster1DRef clus2) const
{
  const PixelGeomDetUnit* gdu1 = dynamic_cast< const PixelGeomDetUnit* >(geomDet_);
  auto && lparams1 = cpe_->localParameters( *clus1, *gdu1 );          // x, y, z, e2_xx, e2_xy, e2_yy
  auto && lparams2 = cpe_->localParameters( *clus2, *gdu1 );          // x, y, z, e2_xx, e2_xy, e2_yy
  return lparams1.first.x() < lparams2.first.x();
}


//FIXME::ERICA: not clear yet how to fullfill the acc/rej output
void VectorHitBuilderAlgorithm::run(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters, 
                                    VectorHitCollectionNew& vhAcc, 
                                    VectorHitCollectionNew& vhRej, 
                                    edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, 
                                    edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej)
{

  LogDebug("VectorHitBuilderAlgorithm") << "Run VectorHitBuilderAlgorithm ... \n" ;
  const  edmNew::DetSetVector<Phase2TrackerCluster1D>* ClustersPhase2Collection = clusters.product();


  std::map< DetId, std::vector<VectorHit> > tempVHAcc, tempVHRej;
  //std::map< DetId, std::vector<Phase2TrackerCluster1D> > tempCLacc, tempCLrej;
  std::map< DetId, std::vector<VectorHit> >::iterator it_temporary;

  //loop over the DetSetVector
  LogDebug("VectorHitBuilderAlgorithm") << "with #clusters : " << ClustersPhase2Collection->size() << std::endl ;
  for( auto DSViter : *ClustersPhase2Collection){

    unsigned int rawDetId1(DSViter.detId());
    DetId detId1(rawDetId1);
    DetId lowerDetId, upperDetId;
    if( theTkTopo->isLower(detId1) ){
      lowerDetId = detId1;
      upperDetId = theTkTopo->partnerDetId(detId1);
    } else if (theTkTopo->isUpper(detId1)) {
      upperDetId = detId1;
      lowerDetId = theTkTopo->partnerDetId(detId1);
    }
    DetId detIdStack = theTkTopo->stack(detId1);

    //debug
    LogDebug("VectorHitBuilderAlgorithm") << "  DetId stack : " << detIdStack.rawId() << std::endl;
    LogDebug("VectorHitBuilderAlgorithm") << "  DetId lower set of clusters  : " << lowerDetId.rawId();
    LogDebug("VectorHitBuilderAlgorithm") << "  DetId upper set of clusters  : " << upperDetId.rawId() << std::endl;

    it_temporary = tempVHAcc.find(detIdStack);
    if ( it_temporary != tempVHAcc.end() ) {
      LogTrace("VectorHitBuilderAlgorithm") << " this stack has already been analyzed -> skip it ";
      continue;
    }

    const GeomDet* gd;
    const StackGeomDet* stackDet;
    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator it_detLower = ClustersPhase2Collection->find( lowerDetId );
    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator it_detUpper = ClustersPhase2Collection->find( upperDetId );

    if ( it_detLower != ClustersPhase2Collection->end() && it_detUpper != ClustersPhase2Collection->end() ){

      gd = theTkGeom->idToDet(detIdStack);
      stackDet = dynamic_cast<const StackGeomDet*>(gd);
      std::vector<VectorHit> vhsInStack_Acc;
      std::vector<VectorHit> vhsInStack_Rej; 
      std::map<VectorHit,bool> vhsInStack_AccRej = buildVectorHits(stackDet, clusters, *it_detLower, *it_detUpper);

      //storing accepted and rejected VHs
      for(auto vh : vhsInStack_AccRej ) {
        if(vh.second == true)  vhsInStack_Acc.push_back(vh.first);
        if(vh.second == false) vhsInStack_Rej.push_back(vh.first);
      }
      tempVHAcc[detIdStack] = vhsInStack_Acc;
      tempVHRej[detIdStack] = vhsInStack_Rej;
    
    }

  }

  loadDetSetVector(tempVHAcc, vhAcc);
  loadDetSetVector(tempVHRej, vhRej);

  LogDebug("VectorHitBuilderAlgorithm") << "End run VectorHitBuilderAlgorithm ... \n" ;
  return;

}

bool VectorHitBuilderAlgorithm::checkClustersCompatibilityBeforeBuilding(edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters,
                                                                         const detset & theLowerDetSet,
                                                                         const detset & theUpperDetSet)
{
  if(theLowerDetSet.size()==1 && theUpperDetSet.size()==1) return true;

  //order lower clusters in u
  std::vector<Phase2TrackerCluster1D> lowerClusters;
  if(theLowerDetSet.size()>1) LogDebug("VectorHitBuilderAlgorithm") << " more than 1 lower cluster! " << std::endl;
  if(theUpperDetSet.size()>1) LogDebug("VectorHitBuilderAlgorithm") << " more than 1 upper cluster! " << std::endl;
  for ( const_iterator cil = theLowerDetSet.begin(); cil != theLowerDetSet.end(); ++ cil ) {
    Phase2TrackerCluster1DRef clusterLower = edmNew::makeRefTo( clusters, cil );
    lowerClusters.push_back(*clusterLower);
  }
  return true;
}

bool VectorHitBuilderAlgorithm::checkClustersCompatibility(Local3DPoint& poslower, 
                                                           Local3DPoint& posupper, 
                                                           LocalError& errlower, 
                                                           LocalError& errupper)
{

  return true;

}

//----------------------------------------------------------------------------
//ERICA::in the DT code the global position is used to compute the alpha angle and put a cut on that.
std::map<VectorHit,bool> VectorHitBuilderAlgorithm::buildVectorHits(const StackGeomDet * stack, 
                                                                  edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters, 
                                                                  const detset & theLowerDetSet, 
                                                                  const detset & theUpperDetSet,
                                                                  const std::vector<bool>& phase2OTClustersToSkip)
{

  std::map<VectorHit,bool> result;
  if(checkClustersCompatibilityBeforeBuilding(clusters, theLowerDetSet, theUpperDetSet)){
    LogDebug("VectorHitBuilderAlgorithm") << "  compatible -> continue ... " << std::endl;
  } else { LogTrace("VectorHitBuilderAlgorithm") << "  not compatible, going to the next cluster"; }

  LogDebug("VectorHitBuilderAlgorithm") << "---- lower clusters " << std::endl;
  std::vector<Phase2TrackerCluster1DRef> lowerClusters;
  for ( const_iterator cil = theLowerDetSet.begin(); cil != theLowerDetSet.end(); ++ cil ) {
    Phase2TrackerCluster1DRef clusterLower = edmNew::makeRefTo( clusters, cil );
    printCluster(stack->lowerDet(),&*clusterLower);
    lowerClusters.push_back(clusterLower);
  }
  LogDebug("VectorHitBuilderAlgorithm") << "---- upper clusters " << std::endl;
  std::vector<Phase2TrackerCluster1DRef> upperClusters;
  for ( const_iterator ciu = theUpperDetSet.begin(); ciu != theUpperDetSet.end(); ++ ciu ) {
    Phase2TrackerCluster1DRef clusterUpper = edmNew::makeRefTo( clusters, ciu );
    printCluster(stack->upperDet(),&*clusterUpper);
    upperClusters.push_back(clusterUpper);
  }

  std::sort(lowerClusters.begin(), lowerClusters.end(), LocalPositionSort(&*theTkGeom,&*cpe,&*stack->lowerDet()));
  std::sort(upperClusters.begin(), upperClusters.end(), LocalPositionSort(&*theTkGeom,&*cpe,&*stack->upperDet()));

  LogDebug("VectorHitBuilderAlgorithm") << "---- after ordering: lower clusters " << std::endl;
  for ( auto clu : lowerClusters)
    printCluster(stack->lowerDet(),&*clu);
  LogDebug("VectorHitBuilderAlgorithm") << "---- after ordering: upper clusters " << std::endl;
  for ( auto clu : upperClusters)
    printCluster(stack->upperDet(),&*clu);

  
  for ( auto cluL : lowerClusters){
    LogDebug("VectorHitBuilderAlgorithm") << " lower clusters " << std::endl;
    printCluster(stack->lowerDet(),&*cluL);
    const PixelGeomDetUnit* gduLow = dynamic_cast< const PixelGeomDetUnit* >(stack->lowerDet());
    auto && lparamsLow = cpe->localParameters( *cluL, *gduLow );
    for ( auto cluU : upperClusters){
      LogDebug("VectorHitBuilderAlgorithm") << "\t upper clusters " << std::endl;
      printCluster(stack->upperDet(),&*cluU);
      const PixelGeomDetUnit* gduUpp = dynamic_cast< const PixelGeomDetUnit* >(stack->upperDet());
      auto && lparamsUpp = cpe->localParameters( *cluU, *gduUpp );

      //building my tolerance : 10*sigma
      double delta = 10.0*sqrt(lparamsLow.second.xx()+lparamsUpp.second.xx()); 
      LogDebug("VectorHitBuilderAlgorithm") << " \t delta: " << delta << std::endl;

      if( (lparamsUpp.first.x() < lparamsLow.first.x() + delta) && 
          (lparamsUpp.first.x() > lparamsLow.first.x() - delta) ){

        LogDebug("VectorHitBuilderAlgorithm") << " building VH! " << std::endl;
        VectorHit vh = buildVectorHit( stack, cluL, cluU);
        LogTrace("VectorHitBuilderAlgorithm") << "-> Vectorhit " << vh ;
        LogTrace("VectorHitBuilderAlgorithm") << std::endl;
        //protection: the VH can also be empty!!
        if (vh.isValid()){
          result[vh] = true;
        }

      } else {
        //storing vh rejected for combinatiorial studies
        VectorHit vh = buildVectorHit( stack, cluL, cluU);
        result[vh] = false;
      }
      
    }
  }





/*
  for ( const_iterator cil = theLowerDetSet.begin(); cil != theLowerDetSet.end(); ++ cil ) {
    //possibility to introducing the skipping of the clusters
    //if(phase2OTClustersToSkip.empty() or (not phase2OTClustersToSkip[cil]) ) {

    Phase2TrackerCluster1DRef clusterLower = edmNew::makeRefTo( clusters, cil );

    for ( const_iterator ciu = theUpperDetSet.begin(); ciu != theUpperDetSet.end(); ++ ciu ) {

      LogTrace("VectorHitBuilderAlgorithm")<<" in the loop for upper clusters with index " << ciu << " on detId " << stack->geographicalId().rawId();

      Phase2TrackerCluster1DRef clusterUpper = edmNew::makeRefTo( clusters, ciu );
      VectorHit vh = buildVectorHit( stack, clusterLower, clusterUpper);
      LogTrace("VectorHitBuilderAlgorithm") << "-> Vectorhit " << vh ;
      LogTrace("VectorHitBuilderAlgorithm") << std::endl;
      //protection: the VH can also be empty!!

      if (vh.isValid()){
        result.push_back(vh);
      }

    }
  }
*/

  //ERICA:: to be checked with map!
  //sorting vhs for best chi2
  //std::sort(result.begin(), result.end());

  //if( result.size() > nMaxVHforeachStack ){
  //  result.erase(result.begin()+nMaxVHforeachStack, result.end());
  //}

  return result;

}

VectorHit VectorHitBuilderAlgorithm::buildVectorHit(const StackGeomDet * stack, 
                                                    Phase2TrackerCluster1DRef lower, 
                                                    Phase2TrackerCluster1DRef upper)
{

  LogTrace("VectorHitBuilderAlgorithm") << "Build VH with: ";
  //printCluster(stack->lowerDet(),&*lower);
  //printCluster(stack->upperDet(),&*upper);

  const PixelGeomDetUnit* geomDetLower = dynamic_cast< const PixelGeomDetUnit* >(stack->lowerDet());
  const PixelGeomDetUnit* geomDetUpper = dynamic_cast< const PixelGeomDetUnit* >(stack->upperDet());
  LogTrace("VectorHitBuilderAlgorithm") << "after geom det unit ";

  auto && lparamsLower = cpe->localParameters( *lower, *geomDetLower );          // x, y, z, e2_xx, e2_xy, e2_yy
  Global3DPoint gparamsLower = geomDetLower->surface().toGlobal(lparamsLower.first);
  LogTrace("VectorHitBuilderAlgorithm") << "lower done ";
  LogTrace("VectorHitBuilderAlgorithm") << "\t lower global pos: " << gparamsLower ;

  auto && lparamsUpper = cpe->localParameters( *upper, *geomDetUpper );
  Global3DPoint gparamsUpper = geomDetUpper->surface().toGlobal(lparamsUpper.first);
  LogTrace("VectorHitBuilderAlgorithm") << "upper done ";
  LogTrace("VectorHitBuilderAlgorithm") << "\t upper global pos: " << gparamsUpper ;

  //local parameters of upper cluster in lower system of reference
  Local3DPoint lparamsUpperInLower = geomDetLower->surface().toLocal(gparamsUpper);

  LogTrace("VectorHitBuilderAlgorithm") << "\t lower global pos: " << gparamsLower ;
  LogTrace("VectorHitBuilderAlgorithm") << "\t upper global pos: " << gparamsUpper ;

  LogTrace("VectorHitBuilderAlgorithm") << "A:\t lower local pos: " << lparamsLower.first << " with error: " << lparamsLower.second << std::endl;
  LogTrace("VectorHitBuilderAlgorithm") << "A:\t upper local pos in the lower sof " << lparamsUpperInLower << " with error: " << lparamsUpper.second << std::endl;

  bool ok = checkClustersCompatibility(lparamsLower.first, lparamsUpper.first, lparamsLower.second, lparamsUpper.second);

  if(ok){

    AlgebraicSymMatrix22 covMat2Dzx;
    double chi22Dzx = 0.0;
    Local3DPoint pos2Dzx;
    Local3DVector dir2Dzx;
    fit2Dzx(lparamsLower.first, lparamsUpperInLower, lparamsLower.second, lparamsUpper.second, pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);
    LogTrace("VectorHitBuilderAlgorithm") << "\t  pos2Dzx: " << pos2Dzx;
    LogTrace("VectorHitBuilderAlgorithm") << "\t  dir2Dzx: " << dir2Dzx;
    LogTrace("VectorHitBuilderAlgorithm") << "\t  cov2Dzx: " << covMat2Dzx;
    VectorHit2D vh2Dzx = VectorHit2D(pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);

    AlgebraicSymMatrix22 covMat2Dzy;
    double chi22Dzy = 0.0;
    Local3DPoint pos2Dzy;
    Local3DVector dir2Dzy;
    fit2Dzy(lparamsLower.first, lparamsUpperInLower, lparamsLower.second, lparamsUpper.second, pos2Dzy, dir2Dzy, covMat2Dzy, chi22Dzy);
    LogTrace("VectorHitBuilderAlgorithm") << "\t  pos2Dzy: " << pos2Dzy;
    LogTrace("VectorHitBuilderAlgorithm") << "\t  dir2Dzy: " << dir2Dzy;
    LogTrace("VectorHitBuilderAlgorithm") << "\t  cov2Dzy: " << covMat2Dzy;
    VectorHit2D vh2Dzy = VectorHit2D(pos2Dzy, dir2Dzy, covMat2Dzy, chi22Dzy);

    OmniClusterRef lowerOmni(lower); 
    OmniClusterRef upperOmni(upper); 
    VectorHit vh = VectorHit(*stack, vh2Dzx, vh2Dzy, lowerOmni, upperOmni);
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
    //difference in z is the difference of the lowermost and the uppermost cluster z pos
    float slopeZ = x.at(1) - x.at(0);
    dir = LocalVector(slope,0.,slopeZ);
  } else {
    dir = LocalVector(slope,0.,-1.);
  }

}
