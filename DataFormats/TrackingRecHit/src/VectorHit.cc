#include "DataFormats/TrackingRecHit/interface/VectorHit.h"
//#include "FWCore/Utilities/interface/Exception.h"

VectorHit::VectorHit(const VectorHit& vh):
  RecSegment(vh.geographicalId()),
  thePosition(vh.localPosition()),
  theDirection(vh.localDirection()),
  theCovMatrix(vh.parametersError()),
  theChi2(vh.chi2()),
  theDimension(vh.dimension()),
  theInnerCluster(vh.innerCluster()),
  theOuterCluster(vh.outerCluster())
{}

VectorHit::VectorHit(DetId id,
                     const LocalPoint& posInner,
                     const LocalVector& dir,
                     const AlgebraicSymMatrix& covMatrix,
		     const double& Chi2,
                     const Phase2TrackerCluster1DRef inner, 
                     const Phase2TrackerCluster1DRef outer):
  RecSegment(id),
  thePosition(posInner),
  theDirection(dir),
  theCovMatrix(covMatrix),
  theChi2(Chi2),
  theDimension(4),
  theInnerCluster(inner),
  theOuterCluster(outer)
{}

VectorHit::VectorHit(DetId id,const VectorHit2D& vh2Dzx, const VectorHit2D& vh2Dzy,
                     const Phase2TrackerCluster1DRef inner, const Phase2TrackerCluster1DRef outer) :
  RecSegment(id),
  theDimension(4),
  theInnerCluster(inner),
  theOuterCluster(outer)
{
  thePosition = LocalPoint(vh2Dzx.localPosition().x(), vh2Dzy.localPosition().x(), 0.);

  theDirection = LocalVector(vh2Dzx.localDirection().x(), vh2Dzy.localDirection().x(), vh2Dzy.localDirection().z());
  //theDirection = theDirection.unit();

  //building the cov matrix 4x4 starting from the 2x2
  AlgebraicSymMatrix22 covMatZX = vh2Dzx.covMatrix();
  AlgebraicSymMatrix22 covMatZY = vh2Dzy.covMatrix();

  theCovMatrix=AlgebraicSymMatrix(4);
  theCovMatrix[0][0] = covMatZX[0][0];   // sigma (dx/dz)
  theCovMatrix[1][1] = covMatZY[0][0];   // sigma (dy/dz)
  theCovMatrix[2][2] = covMatZX[1][1];   // sigma (x)
  theCovMatrix[3][3] = covMatZY[1][1];   // sigma (y)
  theCovMatrix[0][2] = covMatZX[0][1];   // cov(dx/dz,x)
  theCovMatrix[1][3] = covMatZY[0][1];   // cov(dy/dz,y)

  theChi2 = vh2Dzx.chi2() + vh2Dzy.chi2();
}

/*
VectorHit::VectorHit(const DTChamberRecSegment2D& phiSeg,
			       const DTSLRecSegment2D& zedSeg,
			       const LocalPoint& posZInCh,
			       const LocalVector& dirZInCh):
  RecSegment(phiSeg.chamberId()),
  theProjection(full),
  thePhiSeg(phiSeg),
  theZedSeg(zedSeg),
  theDimension(4)
{

  // Check consistency of 2 sub-segments
  if(DTChamberId(phiSeg.geographicalId().rawId()) != DTChamberId(zedSeg.geographicalId().rawId()))
    throw cms::Exception("VectorHit")
      <<"the z Segment and the phi segment have different chamber id"<<std::endl;

  // The position of 2D segments are defined in the SL frame: I must first
  // extrapolate that position at the Chamber reference plane
  LocalPoint posZAt0 = posZInCh +
    dirZInCh * (-posZInCh.z())/cos(dirZInCh.theta());


  thePosition=LocalPoint(phiSeg.localPosition().x(),posZAt0.y(),0.);
  LocalVector dirPhiInCh=phiSeg.localDirection();

  // given the actual definition of chamber refFrame, (with z poiniting to IP),
  // the zed component of direction is negative.
  theDirection=LocalVector(dirPhiInCh.x()/fabs(dirPhiInCh.z()),
                           dirZInCh.y()/fabs(dirZInCh.z()),
                           -1.);
  theDirection=theDirection.unit();

  // set cov matrix
  theCovMatrix=AlgebraicSymMatrix(4);
  theCovMatrix[0][0]=phiSeg.covMatrix()[0][0]; //sigma (dx/dz)
  theCovMatrix[0][2]=phiSeg.covMatrix()[0][1]; //cov(dx/dz,x)
  theCovMatrix[2][2]=phiSeg.covMatrix()[1][1]; //sigma (x)
//  setCovMatrixForZed(posZInCh);

}


VectorHit::VectorHit(const DTSLRecSegment2D& zedSeg,
			       const LocalPoint& posZInCh,
			       const LocalVector& dirZInCh) :
  RecSegment(zedSeg.superLayerId().chamberId()),
  theProjection(Z),
  thePhiSeg(DTChamberRecSegment2D()),
  theZedSeg(zedSeg),
  theDimension(2)
{

  LocalPoint posZAt0=posZInCh+
    dirZInCh*(-posZInCh.z()/cos(dirZInCh.theta()));

  thePosition=posZAt0;
  theDirection = dirZInCh;

  // set cov matrix
  theCovMatrix=AlgebraicSymMatrix(4);
  setCovMatrixForZed(posZInCh);
}
*/

VectorHit::~VectorHit() {}


AlgebraicVector VectorHit::parameters() const {

    // (dx/dz,dy/dz,x,y)
    AlgebraicVector result(4);

    result[0] = theDirection.x();
    result[1] = theDirection.y();
    result[2] = thePosition.x();
    result[3] = thePosition.y();
    return result;

}

Global3DVector VectorHit::globalDirection( const Surface& surf ) {
  Local3DVector theLocalDelta = LocalVector(theDirection.x()*theDirection.z(), theDirection.y()*theDirection.z(), theDirection.z());
  Global3DVector g = surf.toGlobal(theLocalDelta);
  return g;
}

AlgebraicMatrix VectorHit::projectionMatrix() const {

//  static bool isInitialized=false;
  static AlgebraicMatrix the4DProjectionMatrix(4, 5, 0);
/*
  static AlgebraicMatrix the2DPhiProjMatrix(2, 5, 0);
  static AlgebraicMatrix the2DZProjMatrix(2, 5, 0);

  if (!isInitialized) {
    the4DProjectionMatrix[0][1] = 1;
    the4DProjectionMatrix[1][2] = 1;
    the4DProjectionMatrix[2][3] = 1;
    the4DProjectionMatrix[3][4] = 1;

    the2DPhiProjMatrix[0][1] = 1;
    the2DPhiProjMatrix[1][3] = 1;

    the2DZProjMatrix[0][2] = 1;
    the2DZProjMatrix[1][4] = 1;

    isInitialized= true;
  }

  if (dimension()==4) {
    return the4DProjectionMatrix;
  } else if (theProjection==phi) {
    return the2DPhiProjMatrix;
  } else if (theProjection==Z) {
    return the2DZProjMatrix;
  } else {
    return AlgebraicMatrix();
  }
*/ return the4DProjectionMatrix; //ERICA:QUESTO NON ESISTEVA!!
}


LocalError VectorHit::localPositionError() const {
  return LocalError(theCovMatrix[2][2],theCovMatrix[2][3],theCovMatrix[3][3]);
}


LocalError VectorHit::localDirectionError() const {
  return LocalError(theCovMatrix[0][0],theCovMatrix[0][1],theCovMatrix[1][1]);
}



AlgebraicSymMatrix VectorHit::parametersError() const {

  //think about a more efficient method
  AlgebraicSymMatrix result(4);
  for(int i = 0; i < theDimension; i++){
    for(int j = 0; j < theDimension; j++){
      result[i][j] = theCovMatrix[i][j];
    }
  }
  return result;
}

/*
void VectorHit::setCovMatrixForZed(const LocalPoint& posZInCh){
  // Warning!!! the covariance matrix for Theta SL segment is defined in the SL
  // reference frame, here that in the Chamber ref frame must be used.
  // For direction, no problem, but the position is extrapolated, so we must
  // propagate the error properly.

  // many thanks to Paolo Ronchese for the help in deriving the formulas!

  // y=m*z+q in SL frame
  // y=m'*z+q' in CH frame

  // var(m') = var(m)
  theCovMatrix[1][1] = theZedSeg.parametersError()[0][0]; //sigma (dy/dz)

  // cov(m',q') = DeltaZ*Var(m) + Cov(m,q)
  theCovMatrix[1][3] =
    posZInCh.z()*theZedSeg.parametersError()[0][0]+
    theZedSeg.parametersError()[0][1]; //cov(dy/dz,y)

  // Var(q') = DeltaZ^2*Var(m) + Var(q) + 2*DeltaZ*Cov(m,q)
  // cout << "Var(q') = DeltaZ^2*Var(m) + Var(q) + 2*DeltaZ*Cov(m,q)" << endl;
  // cout << "Var(q')= " << posZInCh.z()*posZInCh.z() << "*" <<
  //   theZedSeg.parametersError()[0][0] << " + " <<
  //   theZedSeg.parametersError()[1][1] << " + " <<
  //   2*posZInCh.z() << "*" << theZedSeg.parametersError()[0][1] ;
  theCovMatrix[3][3] =
    2.*(posZInCh.z()*posZInCh.z())*theZedSeg.parametersError()[0][0] +
    theZedSeg.parametersError()[1][1] +
    2.*posZInCh.z()*theZedSeg.parametersError()[0][1];
  // cout << " = " << theCovMatrix[3][3] << endl;
}
*/
std::ostream& operator<<(std::ostream& os, const VectorHit& vh) {

  os << "Pos " << vh.localPosition() <<
    " Dir: " << vh.localDirection() << "\n" <<
    " cov: " << vh.parametersError() <<
    " dim: " << vh.dimension() <<
    " chi2/ndof: " << vh.chi2() << "/" << vh.degreesOfFreedom() << " :";

  return os;
}


/// Access to component RecHits (if any)
std::vector<const TrackingRecHit*> VectorHit::recHits() const{
  std::vector<const TrackingRecHit*> pointersOfRecHits;
/*
  if (hasPhi()) pointersOfRecHits.push_back(phiSegment());
  if (hasZed()) pointersOfRecHits.push_back(zSegment());
*/
  return pointersOfRecHits;
}


/// Non-const access to component RecHits (if any)
std::vector<TrackingRecHit*> VectorHit::recHits(){

  std::vector<TrackingRecHit*> pointersOfRecHits;
/*
  if (hasPhi()) pointersOfRecHits.push_back(phiSegment());
  if (hasZed()) pointersOfRecHits.push_back(zSegment());
*/
  return pointersOfRecHits;
}

/*
DTChamberId VectorHit::chamberId() const {
  return DTChamberId(geographicalId());
}

*/
