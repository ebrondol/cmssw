#ifndef TrackingRecHit_VectorHit2D_h
#define TrackingRecHit_VectorHit2D_h

class VectorHit2D {

 public:
  VectorHit2D() :theDirection(), theCovMatrix(), theChi2(), theDimension(2) {}
  VectorHit2D(const LocalVector& dir, const AlgebraicSymMatrix22& covMatrix, const double& Chi2) : 
              theDirection(dir), theCovMatrix(covMatrix), theChi2(Chi2), theDimension(2){};
  virtual ~VectorHit2D() {};

  LocalVector localDirection() const { return theDirection; }
  LocalError localDirectionError() const { return LocalError(theCovMatrix[0][0],theCovMatrix[0][1],theCovMatrix[1][1]); }
  AlgebraicSymMatrix22 covMatrix() const { return theCovMatrix; }
  double chi2() const  { return theChi2; }
  int degreesOfFreedom() const { return 0.0; }
  int dimension() const { return 2; }

 private:
  LocalVector theDirection;
  AlgebraicSymMatrix22 theCovMatrix;
  double theChi2;
  int theDimension;

};
#endif 

