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

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"

/* Collaborating Class Declarations */
//#include "DataFormats/DTRecHit/interface/DTSLRecSegment2D.h"
//#include "DataFormats/DTRecHit/interface/DTChamberRecSegment2D.h"

/* C++ Headers */
//#include <iosfwd>

class VectorHit : public RecSegment {
 public:

  VectorHit() : thePosition(), theDirection(), theCovMatrix(), theDimension(0) {}
  VectorHit(const LocalPoint& posInner, const LocalVector& dir) ;

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
  /// Covariance matrix fo parameters()
  AlgebraicSymMatrix parametersError() const ;

  /// The projection matrix relates the trajectory state parameters to the segment parameters().
  virtual AlgebraicMatrix projectionMatrix() const;

  /// Local position in Chamber frame
  virtual LocalPoint localPosition() const { return thePosition;}

  /// Local position error in Chamber frame
  virtual LocalError localPositionError() const ;

  /// Local direction in Chamber frame
  virtual LocalVector localDirection() const { return theDirection; }

  /// Local direction error in the Chamber frame
  virtual LocalError localDirectionError() const ;

  // Chi2 of the segment fit
  virtual double chi2() const ;
  
  // Degrees of freedom of the segment fit
  virtual int degreesOfFreedom() const ;

  // Dimension (in parameter space)
  virtual int dimension() const { return theDimension; }

  // Access to component RecHits (if any)
  virtual std::vector<const TrackingRecHit*> recHits() const ;

  // Non-const access to component RecHits (if any)
  virtual std::vector<TrackingRecHit*> recHits() ;

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
    
  /// Set position
  void setPosition(LocalPoint pos) { thePosition = pos; }

  /// Set direction
  void setDirection(LocalVector dir) { theDirection = dir; }

  /// Set covariance matrix
  void setCovMatrix(AlgebraicSymMatrix mat) { theCovMatrix = mat; }

  /// The (specific) DetId of the chamber on which the segment resides 
  virtual DTChamberId chamberId() const;
*/    
 private:
/*
  /// Which projections are actually there
  enum Projection {full, phi, Z, none};
  Projection theProjection;

  /// the superPhi segment 
  DTChamberRecSegment2D *phiSegment() {return &thePhiSeg;}
    
  /// the Z segment
  DTSLRecSegment2D *zSegment() {return &theZedSeg;}
*/
  LocalPoint thePosition;   // in chamber frame
  LocalVector theDirection; // in chamber frame

//  void setCovMatrixForZed(const LocalPoint& posZInCh);
    
  // the covariance matrix, has the following meaning
  // mat[0][0]=sigma (dx/dz)
  // mat[1][1]=sigma (dy/dz)
  // mat[2][2]=sigma (x)
  // mat[3][3]=sigma (y)
  // mat[0][2]=cov(dx/dz,x)
  // mat[1][3]=cov(dy/dz,y)
  AlgebraicSymMatrix theCovMatrix; 
/*
  DTChamberRecSegment2D thePhiSeg;
  DTSLRecSegment2D theZedSeg;
*/
  int theDimension; // the dimension of this rechit

};

///FIXME IMPLEMENT COMPARISON OPERATORS PROPERLY
inline bool operator<( const VectorHit& one, const VectorHit& other) {

  // right now the comparison is done with the chi2
  if ( one.chi2() < other.chi2() ) {
    return true;
  }

  return false;
}

std::ostream& operator<<(std::ostream& os, const VectorHit& vh);

typedef edm::DetSetVector<VectorHit> VectorHitCollection;
typedef edmNew::DetSetVector<VectorHit> VectorHitCollectionNew;

#endif // TrackingRecHit_VectorHit_h

