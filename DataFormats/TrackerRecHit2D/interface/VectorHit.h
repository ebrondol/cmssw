#ifndef TrackerRecHit2D_VectorHit_h
#define TrackerRecHit2D_VectorHit_h

/** \class VectorHit
 *
 * 4-parameter RecHits for Phase2 Tracker (x,y, dx/dz, dy/dz)
 *
 * $Date: 2015/03/30 $
 * \author Erica Brondolin
 *
 */

#include "DataFormats/TrackerRecHit2D/interface/BaseTrackerRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/VectorHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/OmniClusterRef.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"

#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"

#include "DataFormats/GeometrySurface/interface/Surface.h"
#include "DataFormats/TrackingRecHit/interface/KfComponentsHolder.h"

class VectorHit GCC11_FINAL : public BaseTrackerRecHit {

  public:

  typedef OmniClusterRef::Phase2Cluster1DRef ClusterRef;

  VectorHit() : thePosition(), theDirection(), theCovMatrix(), theDimension(0) { setType(bad); }

  VectorHit(const VectorHit& vh) ;

  VectorHit(DetId id, const LocalPoint& posInner, const LocalVector& dir,
            const AlgebraicSymMatrix& covMatrix, const double& Chi2,
            OmniClusterRef const& lower, OmniClusterRef const& upper) ;

  VectorHit(DetId id, const VectorHit2D& vh2Dzx, const VectorHit2D& vh2Dzy,
            OmniClusterRef const& lower, OmniClusterRef const& upper) ;

  ~VectorHit() ;

  virtual VectorHit* clone() const override { return new VectorHit(*this);}

  virtual bool sharesInput( const TrackingRecHit* other, SharedInputType what) const override;
  bool sharesClusters(VectorHit const & h1, VectorHit const & h2,
                      SharedInputType what) const ;

  // Parameters of the segment, for the track fit
  // For a 4D segment: (dx/dz,dy/dz,x,y)
  bool hasPositionAndError() const  GCC11_FINAL{
  //bool hasPositionAndError() const {
    return true;
//      return (err_.xx() != 0) || (err_.yy() != 0) || (err_.xy() != 0) ||
//             (pos_.x()  != 0) || (pos_.y()  != 0) || (pos_.z()  != 0);
  };

  virtual AlgebraicVector parameters() const override;
  virtual void getKfComponents( KfComponentsHolder & holder ) const override { getKfComponents4D(holder); }
  void getKfComponents4D( KfComponentsHolder & holder ) const ;
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
  LocalPoint localPosition() const GCC11_FINAL { return thePosition; }
  virtual LocalVector localDirection() const { return theDirection; }
  AlgebraicSymMatrix parametersError() const override ;
  virtual double chi2() const { return theChi2; }
  virtual int dimension() const override { return theDimension; }
  ClusterRef lowerCluster() const { return theLowerCluster.cluster_phase2OT(); }
  ClusterRef upperCluster() const { return theUpperCluster.cluster_phase2OT(); }
  OmniClusterRef const lowerClusterRef() const { return theLowerCluster; }
  OmniClusterRef const upperClusterRef() const { return theUpperCluster; }

  virtual OmniClusterRef const & firstClusterRef() const GCC11_FINAL { return theLowerCluster;}

  //ERICA:change name! This method returns the delta (not the direction) in global coordinates
  Global3DVector globalDirection( const Surface& surf );

  /// The projection matrix relates the trajectory state parameters to the segment parameters().
  virtual AlgebraicMatrix projectionMatrix() const override;

  /// Local position error in Chamber frame
  LocalError localPositionError() const GCC11_FINAL ;

  /// Local direction error in the Chamber frame
  virtual LocalError localDirectionError() const ;

  // Degrees of freedom of the segment fit
  virtual int degreesOfFreedom() const { return 0; } //number of hits (2+2) - dimension

  // Access to component RecHits (if any)
  virtual std::vector<const TrackingRecHit*> recHits() const override;

  // Non-const access to component RecHits (if any)
  virtual std::vector<TrackingRecHit*> recHits() override ;

  // setting methods
  void setPosition(LocalPoint pos) { thePosition = pos; }
  void setDirection(LocalVector dir) { theDirection = dir; }
  void setCovMatrix(AlgebraicSymMatrix mat) { theCovMatrix = mat; }

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
  AlgebraicSymMatrix theCovMatrix;
  double theChi2;
  int theDimension;
  OmniClusterRef theLowerCluster;
  OmniClusterRef theUpperCluster;

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
