#include "RecoLocalTracker/Phase2ITPixelRecHits/interface/Phase2PixelCPEGeneric.h"

#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/RectangularPixelTopology.h"

#include "DataFormats/DetId/interface/DetId.h"

// Services
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "boost/multi_array.hpp"

#include <iostream>

using namespace std;

namespace {
  constexpr float micronsToCm = 1.0e-4;
  const bool MYDEBUG = false;
}

//-----------------------------------------------------------------------------
//!  The constructor.
//-----------------------------------------------------------------------------
Phase2PixelCPEGeneric::Phase2PixelCPEGeneric(edm::ParameterSet const & conf, 
				 const MagneticField * mag,
                                 const TrackerGeometry& geom,
				 const TrackerTopology& ttopo,
				 const SiPixelLorentzAngle * lorentzAngle, 
				 const SiPixelLorentzAngle * lorentzAngleWidth=0) 
  : Phase2PixelCPEBase(conf, mag, geom, ttopo, lorentzAngle, lorentzAngleWidth,0) {

  if (theVerboseLevel > 0) 
    LogDebug("Phase2PixelCPEGeneric") 
      << " constructing a generic algorithm for ideal pixel detector.\n"
      << " CPEGeneric:: VerboseLevel = " << theVerboseLevel;

  // Externally settable cuts  
  the_eff_charge_cut_lowX = conf.getParameter<double>("eff_charge_cut_lowX");
  the_eff_charge_cut_lowY = conf.getParameter<double>("eff_charge_cut_lowY");
  the_eff_charge_cut_highX = conf.getParameter<double>("eff_charge_cut_highX");
  the_eff_charge_cut_highY = conf.getParameter<double>("eff_charge_cut_highY");
  the_size_cutX = conf.getParameter<double>("size_cutX");
  the_size_cutY = conf.getParameter<double>("size_cutY");

  EdgeClusterErrorX_ = conf.getParameter<double>("EdgeClusterErrorX");
  EdgeClusterErrorY_ = conf.getParameter<double>("EdgeClusterErrorY");

  // Externally settable flags to inflate errors
  inflate_errors = conf.getParameter<bool>("inflate_errors");
  inflate_all_errors_no_trk_angle = conf.getParameter<bool>("inflate_all_errors_no_trk_angle");

  DoCosmics_                 = conf.getParameter<bool>("DoCosmics");  
  
  if(MYDEBUG) {
    std::cout << "From Phase2PixelCPEGeneric::Phase2PixelCPEGeneric(...)" << std::endl;
  }


}

Phase2PixelCPEBase::ClusterParam* Phase2PixelCPEGeneric::createClusterParam(const Phase2ITPixelCluster & cl) const
{
   return new ClusterParamGeneric(cl);
}



//-----------------------------------------------------------------------------
//! Hit position in the local frame (in cm).  Unlike other CPE's, this
//! one converts everything from the measurement frame (in channel numbers) 
//! into the local frame (in centimeters).  
//-----------------------------------------------------------------------------
LocalPoint
Phase2PixelCPEGeneric::localPosition(DetParam const & theDetParam, ClusterParam & theClusterParamBase) const 
{

  ClusterParamGeneric & theClusterParam = static_cast<ClusterParamGeneric &>(theClusterParamBase);

  //  std::cout<<" in Phase2PixelCPEGeneric:localPosition - "<< theDetParam.lorentzShiftInCmY << " " << theDetParam.widthLAFractionY << std::endl;

  float chargeWidthX = (theDetParam.lorentzShiftInCmX * theDetParam.widthLAFractionX);
  float chargeWidthY = (theDetParam.lorentzShiftInCmY * theDetParam.widthLAFractionY);
  float shiftX = 0.5f*theDetParam.lorentzShiftInCmX;
  float shiftY = 0.5f*theDetParam.lorentzShiftInCmY;

  //cout<<" main la width "<<chargeWidthX<<" "<<chargeWidthY<<endl;

  float qclus = theClusterParam.theCluster->charge();     
  float locBz = theDetParam.bz;
  //cout << "Phase2PixelCPEGeneric::localPosition(...) : locBz = " << locBz << endl;
  
  theClusterParam.pixmx  = -999.9; // max pixel charge for truncation of 2-D cluster
  theClusterParam.sigmay = -999.9; // CPE Generic y-error for multi-pixel cluster
  theClusterParam.deltay = -999.9; // CPE Generic y-bias for multi-pixel cluster
  theClusterParam.sigmax = -999.9; // CPE Generic x-error for multi-pixel cluster
  theClusterParam.deltax = -999.9; // CPE Generic x-bias for multi-pixel cluster
  theClusterParam.sy1    = -999.9; // CPE Generic y-error for single single-pixel
  theClusterParam.dy1    = -999.9; // CPE Generic y-bias for single single-pixel cluster
  theClusterParam.sy2    = -999.9; // CPE Generic y-error for single double-pixel cluster
  theClusterParam.dy2    = -999.9; // CPE Generic y-bias for single double-pixel cluster
  theClusterParam.sx1    = -999.9; // CPE Generic x-error for single single-pixel cluster
  theClusterParam.dx1    = -999.9; // CPE Generic x-bias for single single-pixel cluster
  theClusterParam.sx2    = -999.9; // CPE Generic x-error for single double-pixel cluster
  theClusterParam.dx2    = -999.9; // CPE Generic x-bias for single double-pixel cluster
  
  // These numbers come in microns from the qbin(...) call. Transform them to cm.
  theClusterParam.deltax = theClusterParam.deltax * micronsToCm;
  theClusterParam.dx1 = theClusterParam.dx1 * micronsToCm;
  theClusterParam.dx2 = theClusterParam.dx2 * micronsToCm;
  
  theClusterParam.deltay = theClusterParam.deltay * micronsToCm;
  theClusterParam.dy1 = theClusterParam.dy1 * micronsToCm;
  theClusterParam.dy2 = theClusterParam.dy2 * micronsToCm;
  
  theClusterParam.sigmax = theClusterParam.sigmax * micronsToCm;
  theClusterParam.sx1 = theClusterParam.sx1 * micronsToCm;
  theClusterParam.sx2 = theClusterParam.sx2 * micronsToCm;
  
  theClusterParam.sigmay = theClusterParam.sigmay * micronsToCm;
  theClusterParam.sy1 = theClusterParam.sy1 * micronsToCm;
  theClusterParam.sy2 = theClusterParam.sy2 * micronsToCm;
  
  float Q_f_X = 0.0;        //!< Q of the first  pixel  in X 
  float Q_l_X = 0.0;        //!< Q of the last   pixel  in X
  float Q_f_Y = 0.0;        //!< Q of the first  pixel  in Y 
  float Q_l_Y = 0.0;        //!< Q of the last   pixel  in Y
  collect_edge_charges( theClusterParam, 
			Q_f_X, Q_l_X, 
			Q_f_Y, Q_l_Y );

  //--- Find the inner widths along X and Y in one shot.  We
  //--- compute the upper right corner of the inner pixels
  //--- (== lower left corner of upper right pixel) and
  //--- the lower left corner of the inner pixels
  //--- (== upper right corner of lower left pixel), and then
  //--- subtract these two points in the formula.

  //--- Upper Right corner of Lower Left pixel -- in measurement frame
  MeasurementPoint meas_URcorn_LLpix( theClusterParam.theCluster->minPixelRow()+1.0,
				      theClusterParam.theCluster->minPixelCol()+1.0 );

  //--- Lower Left corner of Upper Right pixel -- in measurement frame
  MeasurementPoint meas_LLcorn_URpix( theClusterParam.theCluster->maxPixelRow(),
				      theClusterParam.theCluster->maxPixelCol() );

  //--- These two now converted into the local  
  LocalPoint local_URcorn_LLpix;
  LocalPoint local_LLcorn_URpix;

  // Phase2PixelCPEGeneric can be used with or without track angles
  // If Phase2PixelCPEGeneric is called with track angles, use them to correct for bows/kinks:
  if ( theClusterParam.with_track_angle ) {
    local_URcorn_LLpix = theDetParam.theTopol->localPosition(meas_URcorn_LLpix, theClusterParam.loc_trk_pred);
    local_LLcorn_URpix = theDetParam.theTopol->localPosition(meas_LLcorn_URpix, theClusterParam.loc_trk_pred);
  } else {
    local_URcorn_LLpix = theDetParam.theTopol->localPosition(meas_URcorn_LLpix);
    local_LLcorn_URpix = theDetParam.theTopol->localPosition(meas_LLcorn_URpix);
  }

 #ifdef EDM_ML_DEBUG
  if (theVerboseLevel > 20) {
    cout  
      << "\n\t >>> theClusterParam.theCluster->x = " << theClusterParam.theCluster->x()
      << "\n\t >>> theClusterParam.theCluster->y = " << theClusterParam.theCluster->y()
      << "\n\t >>> cluster: minRow = " << theClusterParam.theCluster->minPixelRow()
      << "  minCol = " << theClusterParam.theCluster->minPixelCol()
      << "\n\t >>> cluster: maxRow = " << theClusterParam.theCluster->maxPixelRow()
      << "  maxCol = " << theClusterParam.theCluster->maxPixelCol()
      << "\n\t >>> meas: inner lower left  = " << meas_URcorn_LLpix.x() 
      << "," << meas_URcorn_LLpix.y()
      << "\n\t >>> meas: inner upper right = " << meas_LLcorn_URpix.x() 
      << "," << meas_LLcorn_URpix.y() 
      << endl;
  }
#endif

  //--- &&& Note that the cuts below should not be hardcoded (like in Orca and
  //--- &&& CPEFromDetPosition/PixelCPEInitial), but rather be
  //--- &&& externally settable (but tracked) parameters.  

  //--- Position, including the half lorentz shift

 #ifdef EDM_ML_DEBUG
  if (theVerboseLevel > 20) 
    cout << "\t >>> Generic:: processing X" << endl;
#endif

  //  std::cout<<" generic_position_formula_X LWid/LShift "<< chargeWidthX/micronsToCm << " " << shiftX/micronsToCm <<std::endl;
  float xPos = 
    generic_position_formula( theClusterParam.theCluster->sizeX(),
			      Q_f_X, Q_l_X, 
			      local_URcorn_LLpix.x(), local_LLcorn_URpix.x(),
			      chargeWidthX,   // lorentz shift in cm
			      theDetParam.theThickness,
			      theClusterParam.cotalpha,
			      theDetParam.thePitchX,
			      theDetParam.theRecTopol->isItBigPixelInX( theClusterParam.theCluster->minPixelRow() ),
			      theDetParam.theRecTopol->isItBigPixelInX( theClusterParam.theCluster->maxPixelRow() ),
			      the_eff_charge_cut_lowX,
                              the_eff_charge_cut_highX,
                              the_size_cutX);           // cut for eff charge width &&&


  // apply the lorentz offset correction 			     
  xPos = xPos + shiftX;

#ifdef EDM_ML_DEBUG
  if (theVerboseLevel > 20) 
    cout << "\t >>> Generic:: processing Y" << endl;
#endif

  //  std::cout<<" generic_position_formula_Y LWid/LShift "<< chargeWidthY/micronsToCm << " " << shiftY/micronsToCm <<std::endl;
  float yPos = 
    generic_position_formula( theClusterParam.theCluster->sizeY(),
			      Q_f_Y, Q_l_Y, 
			      local_URcorn_LLpix.y(), local_LLcorn_URpix.y(),
			      chargeWidthY,   // lorentz shift in cm
			      theDetParam.theThickness,
			      theClusterParam.cotbeta,
			      theDetParam.thePitchY,  
			      theDetParam.theRecTopol->isItBigPixelInY( theClusterParam.theCluster->minPixelCol() ),
			      theDetParam.theRecTopol->isItBigPixelInY( theClusterParam.theCluster->maxPixelCol() ),
			      the_eff_charge_cut_lowY,
                              the_eff_charge_cut_highY,
                              the_size_cutY);           // cut for eff charge width &&&

  // apply the lorentz offset correction 			     
  yPos = yPos + shiftY;
	
  //cout<<" in Phase2PixelCPEGeneric:localPosition - pos = "<<xPos<<" "<<yPos<<endl; //dk

  //--- Now put the two together
  LocalPoint pos_in_local( xPos, yPos );
  return pos_in_local;
}



//-----------------------------------------------------------------------------
//!  A generic version of the position formula.  Since it works for both
//!  X and Y, in the interest of the simplicity of the code, all parameters
//!  are passed by the caller.  The only class variable used by this method
//!  is the theThickness, since that's common for both X and Y.
//-----------------------------------------------------------------------------
float
Phase2PixelCPEGeneric::    
generic_position_formula( int size,                //!< Size of this projection.
			  float Q_f,              //!< Charge in the first pixel.
			  float Q_l,              //!< Charge in the last pixel.
			  float upper_edge_first_pix, //!< As the name says.
			  float lower_edge_last_pix,  //!< As the name says.
			  float lorentz_shift,   //!< L-shift at half thickness
                          float theThickness,   //detector thickness
			  float cot_angle,        //!< cot of alpha_ or beta_
			  float pitch,            //!< thePitchX or thePitchY
			  bool first_is_big,       //!< true if the first is big
			  bool last_is_big,        //!< true if the last is big
			  float eff_charge_cut_low, //!< Use edge if > W_eff  &&&
			  float eff_charge_cut_high,//!< Use edge if < W_eff  &&&
			  float size_cut         //!< Use edge when size == cuts
			 ) const
{

  //  std::cout<<" in Phase2PixelCPEGeneric:generic_position_formula - T/P/LS"<< theThickness << "/"<< pitch <<"/"<< lorentz_shift<<std::endl;

  float geom_center = 0.5f * ( upper_edge_first_pix + lower_edge_last_pix );

  //--- The case of only one pixel in this projection is separate.  Note that
  //--- here first_pix == last_pix, so the average of the two is still the
  //--- center of the pixel.
  if ( size == 1 ) {return geom_center;}

  //--- Width of the clusters minus the edge (first and last) pixels.
  //--- In the note, they are denoted x_F and x_L (and y_F and y_L)
  float W_inner      = lower_edge_last_pix - upper_edge_first_pix;  // in cm

  //--- Predicted charge width from geometry
  float W_pred = theThickness * cot_angle                     // geometric correction (in cm)
    - lorentz_shift;                    // (in cm) &&& check fpix!  

  //cout<<" in Phase2PixelCPEGeneric:generic_position_formula - "<<W_inner<<" "<<W_pred<<endl; //dk

  //--- Total length of the two edge pixels (first+last)
  float sum_of_edge = 2.0f;
  if (first_is_big) sum_of_edge += 1.0f;
  if (last_is_big)  sum_of_edge += 1.0f;
  

  //--- The `effective' charge width -- particle's path in first and last pixels only
  float W_eff = std::abs( W_pred ) - W_inner;


  //--- If the observed charge width is inconsistent with the expectations
  //--- based on the track, do *not* use W_pred-W_innner.  Instead, replace
  //--- it with an *average* effective charge width, which is the average
  //--- length of the edge pixels.
  //
  //  bool usedEdgeAlgo = false;
  if ( (size >= size_cut) || (
       ( W_eff/pitch < eff_charge_cut_low ) |
       ( W_eff/pitch > eff_charge_cut_high ) ) ) 
    {
      W_eff = pitch * 0.5f * sum_of_edge;  // ave. length of edge pixels (first+last) (cm)
      //  usedEdgeAlgo = true;
#ifdef EDM_ML_DEBUG
      nRecHitsUsedEdge_++;
#endif
    }

  
  //--- Finally, compute the position in this projection
  float Qdiff = Q_l - Q_f;
  float Qsum  = Q_l + Q_f;

  //--- Temporary fix for clusters with both first and last pixel with charge = 0
  if(Qsum==0) Qsum=1.0f;
  //float hit_pos = geom_center + 0.5f*(Qdiff/Qsum) * W_eff + half_lorentz_shift;
  float hit_pos = geom_center + 0.5f*(Qdiff/Qsum) * W_eff;

  //cout<<" in Phase2PixelCPEGeneric:generic_position_formula - "<<hit_pos<<" "<<lorentz_shift*0.5<<endl; //dk

 #ifdef EDM_ML_DEBUG
  //--- Debugging output
#warning "Debug printouts in Phase2PixelCPEGeneric.cc has been commented because they cannot be compiled"
  /*   This part is commented because some variables used here are not defined !!
  if (theVerboseLevel > 20) {
    if ( theDetParam.thePart == GeomDetEnumerators::PixelBarrel || theDetParam.thePart == GeomDetEnumerators::P1PXB ) {
      cout << "\t >>> We are in the Barrel." ;
    } else if ( theDetParam.thePart == GeomDetEnumerators::PixelEndcap || 
		theDetParam.thePart == GeomDetEnumerators::P1PXEC ||
		theDetParam.thePart == GeomDetEnumerators::P2PXEC ) {
      cout << "\t >>> We are in the Forward." ;
    } else {
      cout << "\t >>> We are in an unexpected subdet " << theDetParam.thePart;
    }
    cout 
      << "\n\t >>> cot(angle) = " << cot_angle << "  pitch = " << pitch << "  size = " << size
      << "\n\t >>> upper_edge_first_pix = " << upper_edge_first_pix
      << "\n\t >>> lower_edge_last_pix  = " << lower_edge_last_pix
      << "\n\t >>> geom_center          = " << geom_center
      << "\n\t >>> half_lorentz_shift   = " << half_lorentz_shift
      << "\n\t >>> W_inner              = " << W_inner
      << "\n\t >>> W_pred               = " << W_pred
      << "\n\t >>> W_eff(orig)          = " << fabs( W_pred ) - W_inner
      << "\n\t >>> W_eff(used)          = " << W_eff
      << "\n\t >>> sum_of_edge          = " << sum_of_edge
      << "\n\t >>> Qdiff = " << Qdiff << "  Qsum = " << Qsum 
      << "\n\t >>> hit_pos              = " << hit_pos 
      << "\n\t >>> RecHits: total = " << nRecHitsTotal_ 
      << "  used edge = " << nRecHitsUsedEdge_
      << endl;
    if (usedEdgeAlgo) 
      cout << "\n\t >>> Used Edge algorithm." ;
    else
      cout << "\n\t >>> Used angle information." ;
    cout << endl;
  }
  */
#endif

  return hit_pos;
}


//-----------------------------------------------------------------------------
//!  Collect the edge charges in x and y, in a single pass over the pixel vector.
//!  Calculate charge in the first and last pixel projected in x and y
//!  and the inner cluster charge, projected in x and y.
//-----------------------------------------------------------------------------
void
Phase2PixelCPEGeneric::
collect_edge_charges(ClusterParam & theClusterParamBase,  //!< input, the cluster
		     float & Q_f_X,              //!< output, Q first  in X 
		     float & Q_l_X,              //!< output, Q last   in X
		     float & Q_f_Y,              //!< output, Q first  in Y 
		     float & Q_l_Y               //!< output, Q last   in Y
		     ) const
{
  ClusterParamGeneric & theClusterParam = static_cast<ClusterParamGeneric &>(theClusterParamBase);

  // Initialize return variables.
  Q_f_X = Q_l_X = 0.0;
  Q_f_Y = Q_l_Y = 0.0;


  // Obtain boundaries in index units
  uint32_t xmin = theClusterParam.theCluster->minPixelRow();
  uint32_t xmax = theClusterParam.theCluster->maxPixelRow();
  uint32_t ymin = theClusterParam.theCluster->minPixelCol();
  uint32_t ymax = theClusterParam.theCluster->maxPixelCol();


  // Iterate over the pixels.
  int isize = theClusterParam.theCluster->size();
  for (int i = 0;  i != isize; ++i) 
    {
      auto const & pixel = theClusterParam.theCluster->pixel(i); 
      float pix_adc = pixel.adc;
      //
      // X projection
      if ( pixel.x == xmin ) Q_f_X += pix_adc;
      if ( pixel.x == xmax ) Q_l_X += pix_adc;
      //
      // Y projection
      if ( pixel.y == ymin ) Q_f_Y += pix_adc;
      if ( pixel.y == ymax ) Q_l_Y += pix_adc;
    }
  
  return;
} 


//-------------------------------------------------------------------------
//  Hit error in the local frame
//-------------------------------------------------------------------------
LocalError 
Phase2PixelCPEGeneric::localError(DetParam const & theDetParam,  ClusterParam & theClusterParamBase) const 
{

  ClusterParamGeneric & theClusterParam = static_cast<ClusterParamGeneric &>(theClusterParamBase);

  const bool localPrint = false;
  // Default errors are the maximum error used for edge clusters.
  // These are determined by looking at residuals for edge clusters
  float xerr = EdgeClusterErrorX_ * micronsToCm;
  float yerr = EdgeClusterErrorY_ * micronsToCm;
  

  // Find if cluster is at the module edge. 
  int maxPixelCol = theClusterParam.theCluster->maxPixelCol();
  int maxPixelRow = theClusterParam.theCluster->maxPixelRow();
  int minPixelCol = theClusterParam.theCluster->minPixelCol();
  int minPixelRow = theClusterParam.theCluster->minPixelRow();       

  bool edgex = ( theDetParam.theRecTopol->isItEdgePixelInX( minPixelRow ) ) || ( theDetParam.theRecTopol->isItEdgePixelInX( maxPixelRow ) );
  bool edgey = ( theDetParam.theRecTopol->isItEdgePixelInY( minPixelCol ) ) || ( theDetParam.theRecTopol->isItEdgePixelInY( maxPixelCol ) );

  unsigned int sizex = theClusterParam.theCluster->sizeX();
  unsigned int sizey = theClusterParam.theCluster->sizeY();
  if(MYDEBUG) {
    if( int(sizex) != (maxPixelRow - minPixelRow+1) ) cout<<" wrong x"<<endl;
    if( int(sizey) != (maxPixelCol - minPixelCol+1) ) cout<<" wrong y"<<endl;
  }

  // Find if cluster contains double (big) pixels. 
  bool bigInX = theDetParam.theRecTopol->containsBigPixelInX( minPixelRow, maxPixelRow ); 	 
  bool bigInY = theDetParam.theRecTopol->containsBigPixelInY( minPixelCol, maxPixelCol );

  if(localPrint) {
   cout<<" enge clus "<<xerr<<" "<<yerr<<endl;  //dk
   if(bigInX || bigInY) cout<<" big "<<bigInX<<" "<<bigInY<<endl;
   if(edgex || edgey) cout<<" edge "<<edgex<<" "<<edgey<<endl;
   if(theClusterParam.qBin_ == 0) 
     cout<<" qbin 0! "<<edgex<<" "<<edgey<<" "<<bigInX<<" "<<bigInY<<" "
	<<sizex<<" "<<sizey<<endl;
  }

    // This are the simple errors, hardcoded in the code 
    // cout << "Track angles are not known " << endl; 
    // cout << "Default angle estimation which assumes track from PV (0,0,0) does not work." << endl;

  if ( GeomDetEnumerators::isTrackerPixel(theDetParam.thePart) ) { // EM 2016.05 check if GeomDetEnumerator is ok for phase2
    
    xerr = theDetParam.thePitchX / std::sqrt( 12.0f );
    if ( theDetParam.theRecTopol->isItBigPixelInX( theClusterParam.theCluster->minPixelRow() ) )
	 xerr = theDetParam.thePitchX*2./std::sqrt(12.0f);

    yerr = theDetParam.thePitchY / std::sqrt( 12.0f );
    if ( theDetParam.theRecTopol->isItBigPixelInY( theClusterParam.theCluster->minPixelCol() ) )
	 yerr = theDetParam.thePitchY*2./std::sqrt(12.0f);


    if(inflate_errors) {
      int n_bigx = 0;
      int n_bigy = 0;
      
      for (int irow = 0; irow < 7; ++irow) {
	if ( theDetParam.theRecTopol->isItBigPixelInX( irow+minPixelRow ) ) ++n_bigx;
      }
      
      for (int icol = 0; icol < 21; ++icol) {
	  if ( theDetParam.theRecTopol->isItBigPixelInY( icol+minPixelCol ) ) ++n_bigy;
      }
      
      xerr = (float)(sizex + n_bigx) * theDetParam.thePitchX / std::sqrt( 12.0f );
      yerr = (float)(sizey + n_bigy) * theDetParam.thePitchY / std::sqrt( 12.0f );
      
    } // if(inflate_errors)

  } // end 

#ifdef EDM_ML_DEBUG
  if ( !(xerr > 0.0) )
    throw cms::Exception("Phase2PixelCPEGeneric::localError") 
      << "\nERROR: Negative pixel error xerr = " << xerr << "\n\n";
  
  if ( !(yerr > 0.0) )
    throw cms::Exception("Phase2PixelCPEGeneric::localError") 
      << "\nERROR: Negative pixel error yerr = " << yerr << "\n\n";
#endif
 
  //if(localPrint) {
  //cout<<" errors  "<<xerr<<" "<<yerr<<endl;  //dk
  //if(theClusterParam.qBin_ == 0) cout<<" qbin 0 "<<xerr<<" "<<yerr<<endl;
  //}

  auto xerr_sq = xerr*xerr; 
  auto yerr_sq = yerr*yerr;
  
  return LocalError( xerr_sq, 0, yerr_sq );

}






