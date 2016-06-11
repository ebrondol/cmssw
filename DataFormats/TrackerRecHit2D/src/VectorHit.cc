#include "DataFormats/TrackerRecHit2D/interface/VectorHit.h"
#include "Geometry/TrackerGeometryBuilder/interface/StackGeomDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"

VectorHit::VectorHit(const VectorHit& vh):
  BaseTrackerRecHit(*vh.det(), trackerHitRTTI::vector),
  thePosition(vh.localPosition()),
  theDirection(vh.localDirection()),
  theCovMatrix(vh.parametersError()),
  theChi2(vh.chi2()),
  theDimension(vh.dimension()),
  theLowerCluster(vh.lowerClusterRef()),
  theUpperCluster(vh.upperClusterRef())
{}

VectorHit::VectorHit(const GeomDet& idet,
                     const LocalPoint& posLower,
                     const LocalVector& dir,
                     const AlgebraicSymMatrix& covMatrix,
		     const double& Chi2,
                     OmniClusterRef const& lower, OmniClusterRef const& upper) :
  BaseTrackerRecHit(idet, trackerHitRTTI::vector),
  thePosition(posLower),
  theDirection(dir),
  theCovMatrix(covMatrix),
  theChi2(Chi2),
  theDimension(4),
  theLowerCluster(lower),
  theUpperCluster(upper)
{}

VectorHit::VectorHit(const GeomDet& idet, const VectorHit2D& vh2Dzx, const VectorHit2D& vh2Dzy,
                     OmniClusterRef const& lower, OmniClusterRef const& upper) :
  BaseTrackerRecHit(idet, trackerHitRTTI::vector),
  theDimension(4),
  theLowerCluster(lower),
  theUpperCluster(upper)
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

bool VectorHit::sharesInput( const TrackingRecHit* other, SharedInputType what) const 
{
  if (what==all && (geographicalId() != other->geographicalId())) return false;
 
  if (!sameDetModule(*other)) return false;

  if (trackerHitRTTI::isVector(*other) ) {
    const VectorHit* otherVh = static_cast<const VectorHit*>(other);
    return sharesClusters(*this, *otherVh, what);
  }
   
  if (what==all)  return false;

  // what about multi???
  auto const & otherClus = reinterpret_cast<const BaseTrackerRecHit *>(other)->firstClusterRef();
  return (otherClus==lowerClusterRef())  ||  (otherClus==upperClusterRef());
  
}

bool VectorHit::sharesClusters(VectorHit const & h1, VectorHit const & h2,
            SharedInputType what) const {
  bool lower =  h1.lowerClusterRef()== h2.lowerClusterRef();
  bool upper =  h1.upperClusterRef()== h2.upperClusterRef();
  
  return (what==TrackingRecHit::all) ? (lower && upper) : (upper||lower);
  
} 

void VectorHit::getKfComponents4D( KfComponentsHolder & holder ) const {
  //if (!hasPositionAndError()) throwExceptionUninitialized("getKfComponents");
  AlgebraicVector4 & pars = holder.params<4>();
  pars[0] = theDirection.x();
  pars[1] = theDirection.y();
  pars[2] = thePosition.x();
  pars[3] = thePosition.y();

  AlgebraicSymMatrix44 & errs = holder.errors<4>();
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      errs(i,j) = theCovMatrix[i][j];
    }
  }

  ProjectMatrix<double,5,4>  & pf = holder.projFunc<4>();
  pf.index[0] = 1;
  pf.index[1] = 2;
  pf.index[2] = 3;
  pf.index[3] = 4;

  holder.measuredParams<4>() = AlgebraicVector4( & holder.tsosLocalParameters().At(1), 4 );
  holder.measuredErrors<4>() = holder.tsosLocalErrors().Sub<AlgebraicSymMatrix44>( 1, 1 );

}

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

double VectorHit::curvatureORphi(std::string curvORphi) const {

std::cout << "VectorHit::curvature" << std::endl;
  double curvature = 0.0;
  double phi = 0.0;

  const StackGeomDet* stackDet = dynamic_cast< const StackGeomDet* >(det());
  const PixelGeomDetUnit* lowerDet = dynamic_cast< const PixelGeomDetUnit* >(stackDet->lowerDet());
  const PixelGeomDetUnit* upperDet = dynamic_cast< const PixelGeomDetUnit* >(stackDet->upperDet());

  MeasurementPoint mpLower(lowerCluster()->center(), lowerCluster()->column() + 0.5);
  Local3DPoint lPositionLower  = lowerDet->topology().localPosition(mpLower);
  Global3DPoint gPositionLower = lowerDet->surface().toGlobal(lPositionLower);
std::cout << "gPositionLower: " << gPositionLower << std::endl;

  MeasurementPoint mpUpper(upperCluster()->center(), upperCluster()->column() + 0.5);
  Local3DPoint lPositionUpper  = upperDet->topology().localPosition(mpUpper);
  Global3DPoint gPositionUpper = upperDet->surface().toGlobal(lPositionUpper);
std::cout << "gPositionUpper: " << gPositionUpper << std::endl;

  double h1 = gPositionLower.x()*gPositionUpper.y() - gPositionUpper.x()*gPositionLower.y();
  if(h1!=0) {
    double h2 = 2*h1;
    double r12 = pow(gPositionLower.x(),2) + pow(gPositionLower.y(),2);
    double r22 = pow(gPositionUpper.x(),2) + pow(gPositionUpper.y(),2);
    double h3 = r12 + r22 - 2*gPositionLower.x()*gPositionUpper.x() - 2*gPositionLower.y()*gPositionUpper.y();
    double h4 = - pow(gPositionLower.x(),2)*gPositionUpper.x() + gPositionLower.x()*pow(gPositionUpper.x(),2) 
                + gPositionLower.x()*pow(gPositionUpper.y(),2) - gPositionUpper.x()*pow(gPositionLower.y(),2);
    double h5 = pow(gPositionLower.x(),2)*gPositionUpper.y() - pow(gPositionUpper.x(),2)*gPositionLower.y()
              + pow(gPositionLower.y(),2)*gPositionUpper.y() - gPositionLower.y()*pow(gPositionUpper.y(),2);

    //radius of circle
    double rho = sqrt(r12*r22*h3)/(2*h1);
    curvature = 1./rho;
std::cout << "curvature:" << curvature << std::endl;

    //center of circle
    double xcentre = h5/h2;
    double ycentre = h4/h2;
    double dx1 = gPositionLower.x() - xcentre;
    double dy1 = gPositionLower.y() - ycentre;

    //tangent vector at (x1/y1)
    double xtg = dy1;
    double ytg = -(dx1);
    phi = atan2(ytg,xtg);
std::cout << "phi:" << phi << std::endl;

    AlgebraicROOTObject<4,4>::Matrix jacobian;
    for(int i = 0; i < 4; i++){
      for(int j = 0; j < 4; j++){
        jacobian[i][j] = 0.0;
      }
    }

    jacobian[0][0] = 1.0;    // dx1/dx1 dx1/dy1 dx2/dx1 dy2/dx1
    jacobian[1][1] = 1.0;    //dy1/dx1 dy1/dy1 dy2/dx1 dy2/dx1
    jacobian[2][0] = (h1*(2*gPositionLower.x()*r22*h3 + (2*gPositionLower.x() - 2*gPositionUpper.x())*r12*r22))/(pow(r12*r22*h3,3/2)) 
                  - (2*gPositionUpper.y())/sqrt(r12*r22*h3); // dkappa/dx1
    jacobian[2][1] = (2*gPositionUpper.x())/sqrt(r12*r22*h3) + (h1*(2*gPositionLower.y()*r22*h3 + r12*r22*(2*gPositionLower.y() 
                  - 2*gPositionUpper.y())))/pow(r12*r22*h3,3/2); // dkappa/dy1
    jacobian[2][2] = (2*gPositionLower.y())/sqrt(r12*r22*h3) + (h1*(2*gPositionUpper.x()*r12*h3 
                  - 2*(gPositionLower.x() - gPositionUpper.x())*r12*r22))/pow(r12*r22*h3,3/2); // dkappa/dx2
    jacobian[2][3] = (h1*(2*gPositionUpper.y()*r12*h3 - r12*r22*2*(gPositionLower.y() - gPositionUpper.y())))/pow(r12*r22*h3,3/2)
                  - (2*gPositionLower.x())/sqrt(r12*r22*h3); // dkappa/dy2

    AlgebraicVector2 M;
    M[0] = (gPositionLower.y() - ycentre)/(pow(gPositionLower.x() - xcentre,2) + pow(gPositionLower.y() - ycentre,2)); // dphi/dxcentre
    M[1] =-(gPositionLower.x() - xcentre)/(pow(gPositionLower.x() - xcentre,2) + pow(gPositionLower.y() - ycentre,2)); // dphi/dycentre
std::cout << "M:" << M << std::endl;

    AlgebraicROOTObject<2,4>::Matrix K;
    K[0][0]=(2*gPositionLower.x()*gPositionUpper.y())/h2 - (2*gPositionUpper.y()*h5)/pow(h2,2); // dxm/dx1
    K[0][1]=(2*gPositionUpper.x()*h5)/pow(h2,2) - (pow(gPositionUpper.x(),2) + pow(gPositionUpper.y(),2) - 2*gPositionLower.y()*gPositionUpper.y())/h2; // dxm/dy1
    K[0][2]=(2*gPositionLower.y()*h5)/pow(h2,2) - (2*gPositionUpper.x()*gPositionLower.y())/h2; // dxm/dx2
    K[0][3]=(pow(gPositionLower.x(),2) + pow(gPositionLower.y(),2) - 2*gPositionUpper.y()*gPositionLower.y())/h2 - (2*gPositionLower.x()*h5)/pow(h2,2); // dxm/dy2
    K[1][0]=(pow(gPositionUpper.x(),2) - 2*gPositionLower.x()*gPositionUpper.x() + pow(gPositionUpper.y(),2))/h2 - (2*gPositionUpper.y()*h4)/pow(h2,2); // dym/dx1
    K[1][1]=(2*gPositionUpper.x()*h4)/pow(h2,2) - (2*gPositionUpper.x()*gPositionLower.y())/h2; // dym/dy1
    K[1][2]=(2*gPositionLower.y()*h4)/pow(h2,2) - (pow(gPositionLower.x(),2) - 2*gPositionUpper.x()*gPositionLower.x() + pow(gPositionLower.y(),2))/h2; // dym/dx2
    K[1][3]=(2*gPositionLower.x()*gPositionUpper.y())/h2 - (2*gPositionLower.x()*h4)/pow(h2,2); // dym/dy2
std::cout << "K:" << K << std::endl;

    AlgebraicVector4 N = M*K;
    jacobian[3][0] = N[0]; // dphi/(dx1,dy1,dx2,dy2)
    jacobian[3][1] = N[1]; // dphi/(dx1,dy1,dx2,dy2)
    jacobian[3][2] = N[2]; // dphi/(dx1,dy1,dx2,dy2)
    jacobian[3][3] = N[3]; // dphi/(dx1,dy1,dx2,dy2)
std::cout << "jacobian:" << jacobian << std::endl;

  } else {
std::cout << " straight line!" << std::endl;
    return 0;
  }
  
  if( curvORphi == "curvature" ) return curvature;
  else if( curvORphi == "phi"  ) return phi;
  else return 0.0;

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

std::ostream& operator<<(std::ostream& os, const VectorHit& vh) {

  os << " DetId: " << vh.geographicalId() << "\n" <<
        " Pos: " << vh.localPosition() << "\n" <<
        " Dir: " << vh.localDirection() << "\n" <<
        " Cov: " << vh.parametersError() << "\n" <<
        " Dim: " << vh.dimension() << "\n" <<
        " chi2/ndof: " << vh.chi2() << "/" << vh.degreesOfFreedom();

  return os;
}


/// Access to component RecHits (if any)
std::vector<const TrackingRecHit*> VectorHit::recHits() const{
  std::vector<const TrackingRecHit*> pointersOfRecHits;
  return pointersOfRecHits;
}


/// Non-const access to component RecHits (if any)
std::vector<TrackingRecHit*> VectorHit::recHits(){

  std::vector<TrackingRecHit*> pointersOfRecHits;
  return pointersOfRecHits;
}

