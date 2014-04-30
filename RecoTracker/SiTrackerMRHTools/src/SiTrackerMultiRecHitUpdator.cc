#include "RecoTracker/SiTrackerMRHTools/interface/SiTrackerMultiRecHitUpdator.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TSiTrackerMultiRecHit.h"
//#include "RecoTracker/SiTrackerMRHTools/interface/GenericProjectedRecHit2D.h"
//#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "DataFormats/TrackerRecHit2D/interface/SiTrackerMultiRecHit.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TrackingRecHitProjector.h"
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"
#include "DataFormats/TrackingRecHit/interface/KfComponentsHolder.h"
#include "DataFormats/Math/interface/invertPosDefMatrix.h"
#include "DataFormats/Math/interface/ProjectMatrix.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//ERICA: All the input parameter are in RecoTracker/SiTrackerMRHTools/python/SiTrackerMultiRecHitUpdator_cfi.py
SiTrackerMultiRecHitUpdator::SiTrackerMultiRecHitUpdator(const TransientTrackingRecHitBuilder* builder,
							 const TrackingRecHitPropagator* hitpropagator,
							 const float Chi2Cut,
						         const std::vector<double>& anAnnealingProgram):
  theBuilder(builder),
  theHitPropagator(hitpropagator),
  theChi2Cut(Chi2Cut),
  theAnnealingProgram(anAnnealingProgram){}
//theAnnealingStep(0),
//theIsUpdating(true){}


TransientTrackingRecHit::RecHitPointer  SiTrackerMultiRecHitUpdator::buildMultiRecHit(const std::vector<const TrackingRecHit*>& rhv,
                                                                          	      TrajectoryStateOnSurface tsos,
 										      float annealing) const{
  std::cout << "Calling SiTrackerMultiRecHitUpdator::buildMultiRecHit with AnnealingFactor: "  << annealing << std::endl; 
  TransientTrackingRecHit::ConstRecHitContainer tcomponents;	
  for (std::vector<const TrackingRecHit*>::const_iterator iter = rhv.begin(); iter != rhv.end(); iter++){
    TransientTrackingRecHit::RecHitPointer transient = theBuilder->build(*iter);
    if (transient->isValid()) tcomponents.push_back(transient);
  }
  
  return update(tcomponents, tsos, annealing); 
  
}

TransientTrackingRecHit::RecHitPointer SiTrackerMultiRecHitUpdator::update( TransientTrackingRecHit::ConstRecHitPointer original,
                                                                	    TrajectoryStateOnSurface tsos,
									    double annealing) const{
  std::cout << "Calling SiTrackerMultiRecHitUpdator::update with AnnealingFactor: "  << annealing << std::endl; 
  LogTrace("SiTrackerMultiRecHitUpdator") << "Calling SiTrackerMultiRecHitUpdator::update with AnnealingFactor: "  << annealing;
  if (original->isValid())
    LogTrace("SiTrackerMultiRecHitUpdator") << "Original Hit position " << original->localPosition() << " original error " 
					    << original->parametersError();
  else LogTrace("SiTrackerMultiRecHitUpdator") << "Invalid hit";	
  
  if(!tsos.isValid()) {
    //return original->clone();
    throw cms::Exception("SiTrackerMultiRecHitUpdator") << "!!! MultiRecHitUpdator::update(..): tsos NOT valid!!! ";
  }	
  
  //check if to clone is the right thing
  if (original->transientHits().empty()) return original->clone(tsos);
  
  TransientTrackingRecHit::ConstRecHitContainer tcomponents = original->transientHits();	
  return update(tcomponents, tsos, annealing);
}

/*------------------------------------------------------------------------------------------------------------------------*/
TransientTrackingRecHit::RecHitPointer SiTrackerMultiRecHitUpdator::update( TransientTrackingRecHit::ConstRecHitContainer& tcomponents,
                                                                	    TrajectoryStateOnSurface tsos,
									    double annealing) const{

  std::cout << "Calling SiTrackerMultiRecHitUpdator::update++ with AnnealingFactor: "  << annealing << std::endl;  
  if (tcomponents.empty()){
    std::cout << "Empty components vector passed to SiTrackerMultiRecHitUpdator::update, returning an InvalidTransientRecHit ";
    LogTrace("SiTrackerMultiRecHitUpdator") << "Empty components vector passed to SiTrackerMultiRecHitUpdator::update, returning an InvalidTransientRecHit ";
    return InvalidTransientRecHit::build(0); 
  }		
  
  if(!tsos.isValid()) {
    std::cout << "SiTrackerMultiRecHitUpdator::update: tsos NOT valid!!!, returning an InvalidTransientRecHit";
    LogTrace("SiTrackerMultiRecHitUpdator")<<"SiTrackerMultiRecHitUpdator::update: tsos NOT valid!!!, returning an InvalidTransientRecHit";
    return InvalidTransientRecHit::build(0);
  }
  
  std::vector<TransientTrackingRecHit::RecHitPointer> updatedcomponents;
  const GeomDet* geomdet = 0;

  //ERICA: running on all over the measurements for each vtraj 
  for (TransientTrackingRecHit::ConstRecHitContainer::const_iterator iter = tcomponents.begin(); iter != tcomponents.end(); iter++){

    //ERICA: the first rechit must belong to the same surface of TSOS + setting geomdet variable
    if (iter == tcomponents.begin()) {

      if (&((*iter)->det()->surface())!=&(tsos.surface())){
	throw cms::Exception("SiTrackerMultiRecHitUpdator") << "the Trajectory state and the first rechit passed to the SiTrackerMultiRecHitUpdator lay on different surfaces!: state lays on surface " << tsos.surface().position() << " hit with detid " << (*iter)->det()->geographicalId().rawId() << " lays on surface " << (*iter)->det()->surface().position();
      }

      geomdet = (*iter)->det();
      std::cout << "Current reference surface located at " << geomdet->surface().position();
      std::cout <<  "TSOS position " << tsos.localPosition(); 
      LogTrace("SiTrackerMultiRecHitUpdator") << "Current reference surface located at " << geomdet->surface().position();
      LogTrace("SiTrackerMultiRecHitUpdator")<<  "TSOS position " << tsos.localPosition(); 
    }

    //ERICA::WARNING you have to fix GenericProjectedRecHit2D.h
    if (&((*iter)->det()->surface())!=&(tsos.surface())){

    std::cout <<  "hit not propagated because GenericProjectedRecHit2D is still missing." << tsos.localPosition(); 
    return InvalidTransientRecHit::build(0);
      
//      TransientTrackingRecHit::RecHitPointer cloned = theHitPropagator->project<GenericProjectedRecHit2D>(*iter, *geomdet, tsos);
//      LogTrace("SiTrackerMultiRecHitUpdator") << "hit propagated";

//      if (cloned->isValid()) updatedcomponents.push_back(cloned);
    } else {
      TransientTrackingRecHit::RecHitPointer cloned = (*iter)->clone(tsos);
      if (cloned->isValid()){
        updatedcomponents.push_back(cloned);
        LogTrace("SiTrackerMultiRecHitUpdator") << "hit cloned";
        std::cout << "hit cloned" << std::endl;
      }
    }
  }	

  std::vector<std::pair<const TrackingRecHit*, float> > mymap;
  std::vector<std::pair<const TrackingRecHit*, float> > normmap;
  
  double a_sum=0, c_sum=0;


  for(std::vector<TransientTrackingRecHit::RecHitPointer>::iterator ihit = updatedcomponents.begin(); 
	ihit != updatedcomponents.end(); ihit++) {

    //ERICA: older version of the code
    //AlgebraicVector2 r(asSVector<2>((*ihit)->parameters()) - tsospos);
    //AlgebraicSymMatrix22 V(asSMatrix<2>((*ihit)->parametersError()))

    double a_i = ComputeWeight(tsos, *(*ihit), false, annealing); //exp(-0.5*Chi2)/(2.*M_PI*sqrt(det));
    double c_i = ComputeWeight(tsos, *(*ihit), true, annealing);  //exp(-0.5*theChi2Cut/annealing)/(2.*M_PI*sqrt(det));
    mymap.push_back(std::pair<const TrackingRecHit*, float>((*ihit)->hit(), a_i));

    a_sum += a_i;
    c_sum += c_i;   
  }
  double total_sum = a_sum + c_sum;    
  
  unsigned int counter = 0;
  TransientTrackingRecHit::ConstRecHitContainer finalcomponents;
  std::cout << "Filling finalComponents ..." << std::endl; 
  for(std::vector<TransientTrackingRecHit::RecHitPointer>::iterator ihit = updatedcomponents.begin(); 
	ihit != updatedcomponents.end(); ihit++) {

    //uncomment lines below to have like ORCA
    double p = ((mymap[counter].second)/total_sum > 1.e-6 ? (mymap[counter].second)/total_sum : 1.e-6);
    //float p = ((mymap[counter].second)/total_sum > 0.01 ? (mymap[counter].second)/total_sum : 1.e-6);
    normmap.push_back(std::pair<const TrackingRecHit*, float>(mymap[counter].first, p));

    //let's store the weight in the component TransientTrackingRecHit too
    (*ihit)->setWeight(p);
    (*ihit)->setAnnealingFactor(annealing);

    finalcomponents.push_back(*ihit);

    std::cout << "\t Component hit type " << typeid(*mymap[counter].first).name()
                                           << " position " << mymap[counter].first->localPosition()
                                           << " error " << mymap[counter].first->localPositionError()
                                           << " with weight " << p << std::endl;
	
    LogTrace("SiTrackerMultiRecHitUpdator")<< "Component hit type " << typeid(*mymap[counter].first).name() 
					   << " position " << mymap[counter].first->localPosition() 
					   << " error " << mymap[counter].first->localPositionError()
					   << " with weight " << p;
    counter++;
  }
 
  //ERICA: I do not understand what is the utility to do that. 
  mymap = normmap;
  SiTrackerMultiRecHitUpdator::LocalParameters param = calcParameters(tsos, finalcomponents);

//ERICA: older constructor
//SiTrackerMultiRecHit updated(param.first, param.second, normmap.front().first->>geographicalId(), normmap);
  SiTrackerMultiRecHit updated(param.first, param.second, *normmap.front().first->det(), normmap);

  LogTrace("SiTrackerMultiRecHitUpdator") << "Updated Hit position " << updated.localPosition() 
   					  << " updated error " << updated.localPositionError() << std::endl;
  std::cout << "Updated Hit position " << updated.localPosition() 
	    << " updated error " << updated.localPositionError() << std::endl;

  return TSiTrackerMultiRecHit::build(geomdet, &updated, finalcomponents, annealing);

}


//---------------------------------------------------------------------------------------------------------------
double SiTrackerMultiRecHitUpdator::ComputeWeight(const TrajectoryStateOnSurface& tsos, 
							const TransientTrackingRecHit& aRecHit, bool CutWeight, double annealing) const{

     if(CutWeight)  std::cout << "Calling SiTrackerMultiRecHitUpdator::ComputeWeight for CutWeight" << std::endl;
     else 	 std::cout << "Calling SiTrackerMultiRecHitUpdator::ComputeWeight for SingleWeight" << std::endl;
     switch (aRecHit.dimension()) {
         case 1: return ComputeWeight<1>(tsos,aRecHit,CutWeight,annealing);
         case 2: return ComputeWeight<2>(tsos,aRecHit,CutWeight,annealing);
         case 3: return ComputeWeight<3>(tsos,aRecHit,CutWeight,annealing);
         case 4: return ComputeWeight<4>(tsos,aRecHit,CutWeight,annealing);
         case 5: return ComputeWeight<5>(tsos,aRecHit,CutWeight,annealing);
     }
     throw cms::Exception("Rec hit of invalid dimension (not 1,2,3,4,5)") <<
          "The value was " << aRecHit.dimension() << 
         ", type is " << typeid(aRecHit).name() << "\n";
}
 
//---------------------------------------------------------------------------------------------------------------
template <unsigned int N>
double SiTrackerMultiRecHitUpdator::ComputeWeight(const TrajectoryStateOnSurface& tsos,
                                                const TransientTrackingRecHit& aRecHit, bool CutWeight, double annealing) const {

  //define tsos position depending on the dimension of the hit
  typename AlgebraicROOTObject<N>::Vector tsospos;
  if( N==1 ){		
    tsospos[0]=tsos.localPosition().x();
    std::cout <<  "TSOS position " << tsos.localPosition() << std::endl;
    LogTrace("SiTrackerMultiRecHitUpdator")<<  "TSOS position " << tsos.localPosition();
  }
  else if(N==2){	
    tsospos[0]=tsos.localPosition().x();
    tsospos[1]=tsos.localPosition().y();
    std::cout <<  "TSOS position " << tsos.localPosition() << std::endl;
    LogTrace("SiTrackerMultiRecHitUpdator")<<  "TSOS position " << tsos.localPosition();
  }
  else{
    std::cout << "TSOS position not correct. Rec hit of invalid dimension (not 1, 2) but " 
              << aRecHit.dimension() << std::endl;
    LogTrace("SiTrackerMultiRecHitUpdator")<<  "TSOS position not correct. Rec hit of invalid dimension (not 1, 2) but "     
              << aRecHit.dimension();
  }

  // define variables that will be used to setup the KfComponentsHolder
  ProjectMatrix<double,5,N>  pf;
  typename AlgebraicROOTObject<N,5>::Matrix H;
  typename AlgebraicROOTObject<N>::Vector r, rMeas;
  typename AlgebraicROOTObject<N,N>::SymMatrix V, VMeas, W;
  AlgebraicVector5 x = tsos.localParameters().vector();
  const AlgebraicSymMatrix55 &C = (tsos.localError().matrix());

  // setup the holder with the correct dimensions and get the values
  KfComponentsHolder holder;
  holder.template setup<N>(&r, &V, &H, &pf, &rMeas, &VMeas, x, C);
  aRecHit.getKfComponents(holder);
  std::cout << "Get KF components: RecHitPosition " << r << std::endl;

  typename AlgebraicROOTObject<N>::Vector diff;
  diff = r - tsospos;
  std::cout <<  "Created r AlgebraicVector2: " << diff << std::endl;

  //assume that TSOS is smoothed one
  V *= annealing;
  W = V;
  //V += me.measuredError(*ihit);// result = b*V + H*C*H.T()

  //ERICA: ierr will be set to true when inversion is successfull
  bool ierr = invertPosDefMatrix(W);

  //ERICA: the Det2 method will preserve the content of the Matrix 
  //and return true when the calculation is successfull
  double det;
  bool ierr2 = V.Det2(det);

  if( !ierr || !ierr2) {
    std::cout <<"SiTrackerMultiRecHitUpdator::ComputeWeight: W not valid!"<<std::endl;
    std::cout <<"V: "<<V<<" AnnealingFactor: "<<annealing<<std::endl;
    LogTrace("SiTrackerMultiRecHitUpdator")<<"SiTrackerMultiRecHitUpdator::ComputeWeight: W not valid!"<<std::endl;
    LogTrace("SiTrackerMultiRecHitUpdator")<<"V: "<<V<<" AnnealingFactor: "<<annealing<<std::endl;
  }

  double Chi2 =  ROOT::Math::Similarity(diff,W);// Chi2 = diff.T()*W*diff

  double temp_weight;
  if( CutWeight ) 	temp_weight = exp(-0.5*theChi2Cut/annealing)/(2.*M_PI*sqrt(det));
  else 			temp_weight = exp(-0.5*Chi2)/(2.*M_PI*sqrt(det)); 

  return temp_weight;

}

//-----------------------------------------------------------------------------------------------------------
SiTrackerMultiRecHitUpdator::LocalParameters SiTrackerMultiRecHitUpdator::calcParameters(const TrajectoryStateOnSurface& tsos, TransientTrackingRecHit::ConstRecHitContainer& map)const{

  AlgebraicSymMatrix22 W_sum;
  AlgebraicVector2 m_sum;

  for(TransientTrackingRecHit::ConstRecHitContainer::const_iterator ihit = map.begin(); ihit != map.end(); ihit ++) {
    //ERICA: old version of the code
    //AlgebraicVector2 m(asSVector<2>((*ihit)->parameters()));
    //AlgebraicSymMatrix22 V(asSMatrix<2>((*ihit)->parametersError()));
    //AlgebraicSymMatrix22 W(V.Inverse(ierr));

    std::pair<AlgebraicVector2,AlgebraicSymMatrix22> PositionAndError22;
    PositionAndError22 = ComputeParameters2dim(tsos, *(*ihit));

    W_sum += ((*ihit)->weight()*PositionAndError22.second);
    m_sum += ((*ihit)->weight()*(PositionAndError22.second*PositionAndError22.first));
    
  }

std::cout << "W_sum matrix" << W_sum <<std::endl;

  AlgebraicSymMatrix22 V_sum = W_sum; 
  bool ierr = invertPosDefMatrix(V_sum);
  if( !ierr ) {
    std::cout <<"SiTrackerMultiRecHitUpdator::calcParameters: V_sum not valid!"<<std::endl;
    edm::LogError("SiTrackerMultiRecHitUpdator")<<"SiTrackerMultiRecHitUpdator::calcParameters: V_sum not valid!"<<std::endl;
  }
  
  AlgebraicVector2 parameters = V_sum*m_sum;
  LocalError error = LocalError(V_sum(0,0), V_sum(0,1), V_sum(1,1));
  LocalPoint position = LocalPoint(parameters(0), parameters(1));
std::cout << "here" << std::endl;
  return std::make_pair(position,error);
}

//-----------------------------------------------------------------------------------------------------------
std::pair<AlgebraicVector2,AlgebraicSymMatrix22> SiTrackerMultiRecHitUpdator::ComputeParameters2dim(const TrajectoryStateOnSurface& tsos,
						                                                    const TransientTrackingRecHit& aRecHit) const{

     std::cout << "Calling SiTrackerMultiRecHitUpdator::ComputeParameters2dim" << std::endl;
     switch (aRecHit.dimension()) {
         case 2:       return ComputeParameters2dim<2>(tsos,aRecHit);
         //ERICA: avoid the not-2D  hit due to the final sum  
         case ( 1 || 3 || 4 || 5 ):{
	   AlgebraicVector2 dummyVector2D (0.0,0.0);
	   AlgebraicSymMatrix22 dummyMatrix2D;
	   dummyMatrix2D(0,0) = dummyMatrix2D(1,0) = dummyMatrix2D(1,1) = 0.0;
           std::cout << "WARNING:The hit is not 2D: does not count in the parameters calculation." <<  std::endl;
           return std::make_pair(dummyVector2D,dummyMatrix2D);
         } 
     }
     throw cms::Exception("Rec hit of invalid dimension (not 1,2,3,4,5)") <<
          "The value was " << aRecHit.dimension() <<
         ", type is " << typeid(aRecHit).name() << "\n";
}

//---------------------------------------------------------------------------------------------------------------
template <unsigned int N>
std::pair<AlgebraicVector2,AlgebraicSymMatrix22> SiTrackerMultiRecHitUpdator::ComputeParameters2dim(const TrajectoryStateOnSurface& tsos,
                                                const TransientTrackingRecHit& aRecHit) const {

  // define variables that will be used to setup the KfComponentsHolder
  ProjectMatrix<double,5,N>  pf;
  typename AlgebraicROOTObject<N,5>::Matrix H;
  typename AlgebraicROOTObject<N>::Vector r, rMeas;
  typename AlgebraicROOTObject<N,N>::SymMatrix V, VMeas, Wtemp;
  AlgebraicVector5 x = tsos.localParameters().vector();
  const AlgebraicSymMatrix55 &C = (tsos.localError().matrix());

  // setup the holder with the correct dimensions and get the values
  KfComponentsHolder holder;
  holder.template setup<N>(&r, &V, &H, &pf, &rMeas, &VMeas, x, C);
  aRecHit.getKfComponents(holder);

  Wtemp = V;
  bool ierr = invertPosDefMatrix(Wtemp);
  if( !ierr ) {
    std::cout <<"SiTrackerMultiRecHitUpdator::ComputeParameters2dim: W not valid!"<<std::endl;
    edm::LogError("SiTrackerMultiRecHitUpdator")<<"SiTrackerMultiRecHitUpdator::ComputeParameters2dim: W not valid!"<<std::endl;
  }

  // moving to N dimensions to 2 dimensions
  AlgebraicVector2 m2dim;
  AlgebraicSymMatrix22 W2dim;

  if( N == 1 ){
    std::cout <<"SiTrackerMultiRecHitUpdator::ComputeParameters2dim: Expanding RecHit dimensions"<<std::endl;
    edm::LogError("SiTrackerMultiRecHitUpdator")<<"SiTrackerMultiRecHitUpdator::ComputeParameters2dim: Expanding RecHit dimensions"<<std::endl;

    m2dim(0) = r(0);
    m2dim(1) = r(1);


    W2dim(0,0) = Wtemp(0,0);
    W2dim(1,0) = 0.0;
    //ERICA :: WARNING : is it correct?
    W2dim(1,1) = aRecHit.localPositionError().yy();

  }
  if( N >= 2 ){
    if( N > 2 ){
      std::cout <<"SiTrackerMultiRecHitUpdator::ComputeParameters2dim: Reducing RecHit dimensions"<<std::endl;
      edm::LogError("SiTrackerMultiRecHitUpdator")<<"SiTrackerMultiRecHitUpdator::ComputeParameters2dim: Reducing RecHit dimensions"<<std::endl;
      }
    m2dim(0) = r(0);
    m2dim(1) = r(1);

    W2dim(0,0) = Wtemp(0,0); 
    W2dim(1,0) = Wtemp(1,0); 
    W2dim(1,1) = Wtemp(1,1); 

  }

  return std::make_pair(m2dim,W2dim);

}

//---------------------------------------------------------------------------------------------------------------
LocalError SiTrackerMultiRecHitUpdator::calcParametersError(TransientTrackingRecHit::ConstRecHitContainer& map) const {
  AlgebraicSymMatrix22 W_sum;
  int ierr;
  for(TransientTrackingRecHit::ConstRecHitContainer::const_iterator ihit = map.begin(); ihit != map.end(); ihit ++) {
    AlgebraicSymMatrix22 V(asSMatrix<2>((*ihit)->parametersError()));
    AlgebraicSymMatrix22 W(V.Inverse(ierr));
    
    if(ierr != 0) {
      edm::LogError("SiTrackerMultiRecHitUpdator")<<"MultiRecHit::checkParametersError: W not valid!"<<std::endl;
    }
    
    else W_sum += ((*ihit)->weight()*W);
  }
  AlgebraicSymMatrix22 parametersError = W_sum.Inverse(ierr);
  return LocalError(parametersError(0,0), parametersError(0,1), parametersError(1,1));
} 

LocalPoint SiTrackerMultiRecHitUpdator::calcParameters(TransientTrackingRecHit::ConstRecHitContainer& map, const LocalError& er) const {
  AlgebraicVector2 m_sum;
  int ierr;
  for( TransientTrackingRecHit::ConstRecHitContainer::const_iterator ihit = map.begin(); ihit != map.end(); ihit ++) {
    AlgebraicVector2 m(asSVector<2>((*ihit)->parameters()));
    AlgebraicSymMatrix22 V(asSMatrix<2>((*ihit)->parametersError()));
    AlgebraicSymMatrix22 W(V.Inverse(ierr));
    
    if(ierr != 0) {
      edm::LogError("SiTrackerMultiRecHitUpdator")<<"MultiRecHit::checkParameters: W not valid!"<<std::endl;
    }
    
    //m_sum += ihit->weight()*(W*m);      
    else m_sum += ((*ihit)->weight()*(W*m));
  }
  AlgebraicSymMatrix22 V_sum;
	
  V_sum(0,0) = er.xx();
  V_sum(0,1) = er.xy();
  V_sum(1,1) = er.yy();
  //AlgebraicSymMatrix V_sum(parametersError());
  AlgebraicVector2 parameters = V_sum*m_sum;
  return LocalPoint(parameters(0), parameters(1));
}
                           
