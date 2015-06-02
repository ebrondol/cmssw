#ifndef TrackingRecHit_VectorHit_h
#define TrackingRecHit_VectorHit_h

/** \class VectorHit
 *
 * 4-parameter RecHits for Phase2 Tracker (x,y, dx/dz, dy/dz)
 *
 * $Date: 2015/03/30 $
 * \author Erica Brondolin
 *
 */

#include "DataFormats/TrackingRecHit/interface/RecSegment.h"
#include "DataFormats/TrackingRecHit/interface/VectorHit2D.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"

class VectorHit : public RecSegment {
 public:

  VectorHit() : thePosition(), theDirection(), theCovMatrix(), theDimension(0) {}
  VectorHit(const LocalPoint& posInner, const LocalVector& dir, 
            const AlgebraicSymMatrix44& covMatrix,
            const double& Chi2) ;
  VectorHit(const VectorHit2D& vh2Dzx, const VectorHit2D& vh2Dzy) ;

  ~VectorHit() ;
  virtual VectorHit* clone() const { return new VectorHit(*this);}

  // Parameters of the segment, for the track fit 
  // For a 4D segment: (dx/dy,dy/dz,x,y)
  // FIXME::getKFcomponents ??
  AlgebraicVector parameters() const ;
/*
  friend class DTSegmentUpdator;
  VectorHit() : theProjection(none), theDimension(0) {}
  
  /// Construct from phi and Z projections
  VectorHit(const DTChamberRecSegment2D& phiSeg, const DTSLRecSegment2D& zedSeg, const LocalPoint& posZInCh, const LocalVector& dirZInCh);

  /// Construct from phi projection
  VectorHit(const DTChamberRecSegment2D& phiSeg);

  /// Construct from Z projection
  VectorHit(const DTSLRecSegment2D& zedSeg, const LocalPoint& posZInCh, const LocalVector& dirZInCh);


*/
  // returning methods
  virtual LocalPoint localPosition() const { return thePosition; }
  virtual LocalVector localDirection() const { return theDirection; }
  AlgebraicSymMatrix parametersError() const ;
  virtual double chi2() const { return theChi2; }
  virtual int dimension() const { return theDimension; }

  /// The projection matrix relates the trajectory state parameters to the segment parameters().
  virtual AlgebraicMatrix projectionMatrix() const;

  /// Local position error in Chamber frame
  virtual LocalError localPositionError() const ;

  /// Local direction error in the Chamber frame
  virtual LocalError localDirectionError() const ;
  
  // Degrees of freedom of the segment fit
  virtual int degreesOfFreedom() const { return 0; } //number of hits (2+2) - dimension

  // Access to component RecHits (if any)
  virtual std::vector<const TrackingRecHit*> recHits() const ;

  // Non-const access to component RecHits (if any)
  virtual std::vector<TrackingRecHit*> recHits() ;

  // setting methods
  void setPosition(LocalPoint pos) { thePosition = pos; }
  void setDirection(LocalVector dir) { theDirection = dir; }
  void setCovMatrix(AlgebraicSymMatrix44 mat) { theCovMatrix = mat; }

/*

  //--- Extension of the interface

  
  /// Does it have the Phi projection?
  bool hasPhi() const {return (theProjection==full || theProjection==phi);}
  
  /// Does it have the Z projection?
  bool hasZed() const {return (theProjection==full || theProjection==Z);}
  
  /// The superPhi segment: 0 if no phi projection available
  const DTChamberRecSegment2D *phiSegment() const {
    return hasPhi()? &thePhiSeg: 0;
  }
    
  /// The Z segment: 0 if not zed projection available
  const DTSLRecSegment2D *zSegment() const {
    return hasZed()? &theZedSeg : 0;
  }
    
  /// The (specific) DetId of the chamber on which the segment resides 
  virtual DTChamberId chamberId() const;
*/    
 private:
/*
  /// Which projections are actually there
  enum Projection {full, phi, Z, none};
  Projection theProjection;
*/

  //VectorHit2D *ZXSegment() {return &theVh2Dzx;}
  //VectorHit2D *ZYSegment() {return &theVh2Dzy;}
  LocalPoint thePosition;
  LocalVector theDirection; 

//  void setCovMatrixForZed(const LocalPoint& posZInCh);
    
  // the covariance matrix, has the following meaning
  // mat[0][0]=sigma (dx/dz)
  // mat[1][1]=sigma (dy/dz)
  // mat[2][2]=sigma (x)
  // mat[3][3]=sigma (y)
  // mat[0][2]=cov(dx/dz,x)
  // mat[1][3]=cov(dy/dz,y)
  AlgebraicSymMatrix44 theCovMatrix; 
  double theChi2;
  int theDimension; 
  //VectorHit2D theVh2Dzx;
  //VectorHit2D theVh2Dzy;

};

inline bool operator<( const VectorHit& one, const VectorHit& other) {

  if ( one.chi2() > other.chi2() ) {
    return true;
  }

  return false;
}

std::ostream& operator<<(std::ostream& os, const VectorHit& vh);

typedef edm::DetSetVector<VectorHit> VectorHitCollection;
typedef edmNew::DetSetVector<VectorHit> VectorHitCollectionNew;

#endif // TrackingRecHit_VectorHit_h

