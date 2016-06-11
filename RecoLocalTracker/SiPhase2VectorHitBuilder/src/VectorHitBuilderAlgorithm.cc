#include "RecoLocalTracker/SiPhase2VectorHitBuilder/interface/VectorHitBuilderAlgorithm.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "DataFormats/TrackerRecHit2D/interface/VectorHit2D.h"



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
    DetId detId2 = theTkTopo->partnerDetId(detId1);
    DetId detIdStack = theTkTopo->stack(detId1);

    //debug
    LogTrace("VectorHitBuilderAlgorithm") << "  DetId stack : " << detIdStack.rawId();
    LogTrace("VectorHitBuilderAlgorithm") << "  DetId first set of clusters  : " << rawDetId1;
    LogTrace("VectorHitBuilderAlgorithm") << "  DetId second set of clusters : " << detId2.rawId();

    it_temporary = temporary.find(detIdStack);
    if ( it_temporary != temporary.end() ) {
      LogTrace("VectorHitBuilderAlgorithm") << " this stack has already been analyzed -> skip it ";
      continue;
    }

    const GeomDet* gd;
    const StackGeomDet* stackDet;
    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator it_det1 = ClustersPhase2Collection->find( detId1 );
    edmNew::DetSetVector<Phase2TrackerCluster1D>::const_iterator it_det2 = ClustersPhase2Collection->find( detId2 );

    // it can also be moved in buildVectorHit directly
    const GeomDetUnit* geomDetUnit1(theTkGeom->idToDetUnit(detId1));
    const GeomDetUnit* geomDetUnit2(theTkGeom->idToDetUnit(detId2));

    if ( it_det1 != ClustersPhase2Collection->end() && it_det2 != ClustersPhase2Collection->end() ){
      gd = theTkGeom->idToDet(detIdStack);
      stackDet = dynamic_cast<const StackGeomDet*>(gd);
      std::vector<VectorHit> vhsInStack = buildVectorHits(stackDet, clusters, *it_det1, *it_det2, geomDetUnit1, geomDetUnit2);
      temporary[detIdStack] = vhsInStack;
    }

  }

  loadDetSetVector(temporary, vhAcc);

  LogDebug("VectorHitBuilderAlgorithm") << "End run VectorHitBuilderAlgorithm ... \n" ;
  return;

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
std::vector<VectorHit> VectorHitBuilderAlgorithm::buildVectorHits(const StackGeomDet * stack, 
                                                                  edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> > clusters, 
                                                                  const detset & theLowerDetSet, 
                                                                  const detset & theUpperDetSet, 
                                                                  const GeomDetUnit * theLowerGeomDetUnit, 
                                                                  const GeomDetUnit * theUpperGeomDetUnit)
{

  LogTrace("VectorHitBuilderAlgorithm") << "theLowerDetSet : " << theLowerDetSet.size();
  LogTrace("VectorHitBuilderAlgorithm") << "theUpperDetSet : " << theUpperDetSet.size();
  std::vector<VectorHit> result;


  for ( const_iterator cil = theLowerDetSet.begin(); cil != theLowerDetSet.end(); ++ cil ) {

    Phase2TrackerCluster1DRef clusterLower = edmNew::makeRefTo( clusters, cil );

    for ( const_iterator ciu = theUpperDetSet.begin(); ciu != theUpperDetSet.end(); ++ ciu ) {

      Phase2TrackerCluster1DRef clusterUpper = edmNew::makeRefTo( clusters, ciu );
      VectorHit vh = buildVectorHit( stack, clusterLower, clusterUpper, theLowerGeomDetUnit, theUpperGeomDetUnit);
      LogTrace("VectorHitBuilderAlgorithm") << "-> Vectorhit " << vh ;
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

VectorHit VectorHitBuilderAlgorithm::buildVectorHit(const StackGeomDet * stack, 
                                                    Phase2TrackerCluster1DRef lower, 
                                                    Phase2TrackerCluster1DRef upper, 
                                                    const GeomDetUnit* theLowerGeomDetUnit, 
                                                    const GeomDetUnit* theUpperGeomDetUnit)
{

  LogTrace("VectorHitBuilderAlgorithm") << "Build VH with: ";
  printCluster(theLowerGeomDetUnit,&*lower);
  printCluster(theUpperGeomDetUnit,&*upper);

  //FIXME::you should put the correct error when the StripCPE is ready
  //FIXME StripClusterParameterEstimator::LocalValues parameters = parameterestimator->localParameters(*lowerClus_iter,*theLowerGeomDetUnit);
  const PixelGeomDetUnit* geomDetLower = dynamic_cast< const PixelGeomDetUnit* >(theLowerGeomDetUnit);
  const PixelGeomDetUnit* geomDetUpper = dynamic_cast< const PixelGeomDetUnit* >(theUpperGeomDetUnit);

  const PixelTopology * topoLower = &geomDetLower->specificTopology();
  const PixelTopology * topoUpper = &geomDetUpper->specificTopology();

  float pitchXlower = topoLower->pitch().first;
  float pitchYlower = topoLower->pitch().second;
  float ixLower = lower->center();
  float iyLower = lower->column()+0.5; // halfway the column

  LocalPoint lpLower( topoLower->localX(ixLower), topoLower->localY(iyLower), 0 );          // x, y, z
  LocalError leLower( pow(pitchXlower, 2) / 12, 0, pow(pitchYlower, 2) / 12);               // e2_xx, e2_xy, e2_yy

  float pitchXupper = topoUpper->pitch().first;
  float pitchYupper = topoUpper->pitch().second;
  float ixUpper = upper->center();
  float iyUpper = upper->column()+0.5; // halfway the column

  LocalPoint lpUpper( topoUpper->localX(ixUpper), topoUpper->localY(iyUpper), 0 );          // x, y, z
  LocalError leUpper( pow(pitchXupper, 2) / 12, 0, pow(pitchYupper, 2) / 12);               // e2_xx, e2_xy, e2_yy

  //moving the upper into the lower s.o.r.
  Global3DPoint gpUpper = geomDetUpper->surface().toGlobal(lpUpper);
  Local3DPoint lpUpperInLower = geomDetLower->surface().toLocal(gpUpper);

  Global3DPoint gPositionLower = geomDetLower->surface().toGlobal(lpLower);
  Global3DPoint gPositionUpper = geomDetUpper->surface().toGlobal(lpUpper);

  LogTrace("VectorHitBuilderAlgorithm") << "A:\t lower local pos " << lpLower << " with error: " << leLower << std::endl;
  std::cout << "\t lower global pos " << gPositionLower << std::endl;
  std::cout << "\t upper global pos " << gPositionUpper << std::endl;
  LogTrace("VectorHitBuilderAlgorithm") << "A:\t upper local pos in the lower sof " << lpUpperInLower << " with error: " << leUpper << std::endl;


  bool ok = checkClustersCompatibility(lpLower, lpUpper, leLower, leUpper);

  if(ok){

    //in the lower reference of frame
    //Local3DVector localVecINN = localPosCluOutINN - localPosCluInn;
    //LogTrace("VectorHitBuilderAlgorithm") << "\t local vec in the lower sof " << localVecINN;

    AlgebraicSymMatrix22 covMat2Dzx;
    double chi22Dzx = 0.0;
    Local3DPoint pos2Dzx;
    Local3DVector dir2Dzx;
    fit2Dzx(lpLower, lpUpperInLower, leLower,leUpper, pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);
    LogTrace("VectorHitBuilderAlgorithm") << "\t  pos2Dzx: " << pos2Dzx;
    LogTrace("VectorHitBuilderAlgorithm") << "\t  dir2Dzx: " << dir2Dzx;
    LogTrace("VectorHitBuilderAlgorithm") << "\t  cov2Dzx: " << covMat2Dzx;
    VectorHit2D vh2Dzx = VectorHit2D(pos2Dzx, dir2Dzx, covMat2Dzx, chi22Dzx);

    AlgebraicSymMatrix22 covMat2Dzy;
    double chi22Dzy = 0.0;
    Local3DPoint pos2Dzy;
    Local3DVector dir2Dzy;
    fit2Dzy(lpLower, lpUpperInLower, leLower,leUpper, pos2Dzy, dir2Dzy, covMat2Dzy, chi22Dzy);
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
