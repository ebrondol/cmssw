#include "DataFormats/TrackCandidate/interface/TrackCandidate.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/TrackingRecHit/interface/InvalidTrackingRecHit.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TkTransientTrackingRecHitBuilder.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "RecoTracker/TrackProducer/interface/DAFTrackProducerAlgorithm.h"
#include "RecoTracker/SiTrackerMRHTools/interface/SiTrackerMultiRecHitUpdator.h"
#include "RecoTracker/SiTrackerMRHTools/interface/MultiRecHitCollector.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryFitter.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TkClonerImpl.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateWithArbitraryError.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiTrackerMultiRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/OmniClusterRef.h"
#include "DataFormats/TrackerRecHit2D/interface/TkCloner.h"
#include "TrackingTools/PatternTools/interface/TrajAnnealing.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateCombiner.h"
#include "TrackingTools/TrajectoryCleaning/interface/TrajectoryCleaner.h"

DAFTrackProducerAlgorithm::DAFTrackProducerAlgorithm(const edm::ParameterSet& conf):
  conf_(conf),
  minHits_(conf.getParameter<int>("MinHits")){}


void DAFTrackProducerAlgorithm::runWithCandidate(const TrackingGeometry * theG,
					         const MagneticField * theMF,
						 const std::vector<Trajectory>& theTrajectoryCollection,
						 const MeasurementTrackerEvent *measTk,
					         const TrajectoryFitter * theFitter,
					         const TransientTrackingRecHitBuilder* builder,
						 const MultiRecHitCollector* measurementCollector,
						 const SiTrackerMultiRecHitUpdator* updator,
						 const reco::BeamSpot& bs,
						 const TrajectoryCleaner* theTrajCleaner, 
					         AlgoProductCollection& algoResults,
						 TrajAnnealingCollection& trajann,
						 bool TrajAnnSaving_) const
{
  LogDebug("DAFTrackProducerAlgorithm") << "Number of Trajectories: " << theTrajectoryCollection.size() << "\n";
  int cont = 0;
  std::vector<Trajectory> theDAFTrajectories;

  //running on src trajectory collection
  for (std::vector<Trajectory>::const_iterator ivtraj = theTrajectoryCollection.begin(); 
       ivtraj != theTrajectoryCollection.end(); ivtraj++)
  {

    float ndof = 0;
    Trajectory currentTraj;

    //getting trajectory
    //no need to have std::vector<Trajectory> vtraj !
    if ( (*ivtraj).isValid() ){

      LogDebug("DAFTrackProducerAlgorithm") << "The trajectory #" << cont+1 << " is valid. \n";

      //getting the MultiRecHit collection and the trajectory with a first fit-smooth round
      std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface> hits = 
							collectHits(*ivtraj, measurementCollector, &*measTk);
      
      currentTraj = fit(hits, theFitter, *ivtraj);

      //starting the annealing program
      for (std::vector<double>::const_iterator ian = updator->getAnnealingProgram().begin(); 
           ian != updator->getAnnealingProgram().end(); ian++){

        if (currentTraj.isValid()){

	  LogDebug("DAFTrackProducerAlgorithm") << "Seed direction is " << currentTraj.seed().direction() 
	 	                                << ".Traj direction is " << currentTraj.direction();

          //updating MultiRecHits and fit-smooth again 
	  std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface> curiterationhits = 
									updateHits(currentTraj, updator, &*measTk, *ian);
          if( curiterationhits.first.size() < 3 ){
            LogDebug("DAFTrackProducerAlgorithm") << "Rejecting trajectory with "
                                                  << curiterationhits.first.size() <<" hits";
            currentTraj = Trajectory();
            break;
          }
	  currentTraj = fit(curiterationhits, theFitter, currentTraj);

 	  //saving trajectory for each annealing cycle ...
	  if(TrajAnnSaving_){
            TrajAnnealing temp(currentTraj, *ian);
	    trajann.push_back(temp);
          }

          LogDebug("DAFTrackProducerAlgorithm") << "done annealing value "  <<  (*ian) ;

	} 
        else break;


      } //end of annealing program

      LogDebug("DAFTrackProducerAlgorithm") << "Ended annealing program with " << (1.*checkHits(*ivtraj, currentTraj))/(1.*(*ivtraj).measurements().size())*100. << " unchanged." << std::endl;

      //computing the ndof keeping into account the weights
      ndof = calculateNdof(currentTraj);

      //checking if the trajectory has the minimum number of valid hits ( weight (>1e-6) )
      //in order to remove tracks with too many outliers.

      int goodHits = countingGoodHits(currentTraj);

      if( goodHits >= minHits_) {

        bool ok = buildTrack(currentTraj, algoResults, ndof, bs) ;
	// or filtered?
        if(ok){
	  cont++;
          theDAFTrajectories.push_back(currentTraj);
	}

      }
      else{
        LogDebug("DAFTrackProducerAlgorithm")  << "Rejecting trajectory with " 
 					       << currentTraj.foundHits()<<" hits"; 
      }
    }
    else 
      LogDebug("DAFTrackProducerAlgorithm") << "Rejecting empty trajectory" << std::endl;

  } //end run on track collection

  LogDebug("DAFTrackProducerAlgorithm") << "Number of Tracks found: " << cont;
  LogDebug("DAFTrackProducerAlgorithm") << "Number of Trajectories found: " << theDAFTrajectories.size() << "\n";

  theTrajCleaner->clean(theDAFTrajectories);
  LogDebug("DAFTrackProducerAlgorithm") << "Number of Tracks found after cleaning: " << theDAFTrajectories.size() << "\n";

}
/*------------------------------------------------------------------------------------------------------*/
std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface>
DAFTrackProducerAlgorithm::collectHits(const Trajectory vtraj,
                                       const MultiRecHitCollector* measurementCollector,
                                       const MeasurementTrackerEvent *measTk) const
{

  LogDebug("DAFTrackProducerAlgorithm") << "Calling DAFTrackProducerAlgorithm::collectHits";

  //getting the traj measurements from the MeasurementCollector 
  int nHits = 0;
  TransientTrackingRecHit::RecHitContainer hits;
  std::vector<TrajectoryMeasurement> collectedmeas = measurementCollector->recHits(vtraj, measTk);

  //if the MeasurementCollector is empty, make an "empty" pair 
  //else taking the collected measured hits and building the pair
  if( collectedmeas.empty() ) 
    return std::make_pair(TransientTrackingRecHit::RecHitContainer(), TrajectoryStateOnSurface());

  for(  std::vector<TrajectoryMeasurement>::const_iterator iter = collectedmeas.begin(); 
	iter!=collectedmeas.end(); iter++ ){

    nHits++;
    hits.push_back(iter->recHit());
  
  }
  

  //TrajectoryStateWithArbitraryError() == Creates a TrajectoryState with the same parameters 
  //     as the input one, but with "infinite" errors, i.e. errors so big that they don't
  //     bias a fit starting with this state. 
  //return std::make_pair(hits,TrajectoryStateWithArbitraryError()(collectedmeas.front().predictedState()));

  // I do not have to rescale the error because it is already rescaled in the fit code 
  TrajectoryStateOnSurface initialStateFromTrack = collectedmeas.front().predictedState();

  LogDebug("DAFTrackProducerAlgorithm") << "Pair (hits, TSOS)  with TSOS predicted(collectedmeas.front().predictedState())";
  return std::make_pair(hits, initialStateFromTrack);

}
/*------------------------------------------------------------------------------------------------------*/
std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface>
DAFTrackProducerAlgorithm::updateHits(const Trajectory vtraj,
				      const SiTrackerMultiRecHitUpdator* updator,
				      const MeasurementTrackerEvent* theMTE,
				      double annealing) const 
{
  LogDebug("DAFTrackProducerAlgorithm") << "Calling DAFTrackProducerAlgorithm::updateHits";
  TransientTrackingRecHit::RecHitContainer hits;
  std::vector<TrajectoryMeasurement> vmeas = vtraj.measurements();
  std::vector<TrajectoryMeasurement>::reverse_iterator imeas;
  unsigned int hitcounter = 1;

  //I run inversely on the trajectory obtained and update the state
  for (imeas = vmeas.rbegin(); imeas != vmeas.rend(); imeas++, hitcounter++){

    DetId id = imeas->recHit()->geographicalId();
    MeasurementDetWithData measDet = theMTE->idToDet(id);

    TrajectoryStateCombiner combiner;
    TrajectoryStateOnSurface combtsos;
    if (hitcounter == vmeas.size()) combtsos = imeas->predictedState();   //fwd
    else if (hitcounter == 1) combtsos = imeas->backwardPredictedState(); //bwd
    else combtsos = combiner(imeas->backwardPredictedState(), imeas->predictedState());

    PrintHit(&*imeas->recHit(), combtsos);
    if(imeas->recHit()->isValid()){
    TransientTrackingRecHit::RecHitPointer updated = updator->update(imeas->recHit(), 
						combtsos, measDet, annealing);
    hits.push_back(updated);
    } else {
      hits.push_back(imeas->recHit());
    }
  }

  TrajectoryStateOnSurface updatedStateFromTrack = vmeas.back().predictedState();

  //return std::make_pair(hits,TrajectoryStateWithArbitraryError()(vmeas.back().updatedState()));
  LogDebug("DAFTrackProducerAlgorithm") << "Pair (hits, TSOS)  with TSOS predicted (vmeas.back().predictedState())";

  return std::make_pair(hits,updatedStateFromTrack);
}
/*------------------------------------------------------------------------------------------------------*/
Trajectory DAFTrackProducerAlgorithm::fit(const std::pair<TransientTrackingRecHit::RecHitContainer,
                                    TrajectoryStateOnSurface>& hits,
                                    const TrajectoryFitter * theFitter,
                                    Trajectory vtraj) const {

  //creating a new trajectory starting from the direction of the seed of the input one and the hits
  Trajectory newVec = theFitter->fitOne(TrajectorySeed(PTrajectoryStateOnDet(),
                                                                 BasicTrajectorySeed::recHitContainer(),
                                                                 vtraj.seed().direction()),
                                                                 hits.first, hits.second);

  if( newVec.isValid() )  return newVec; 
  else{
    LogDebug("DAFTrackProducerAlgorithm") << "Fit no valid.";
    return Trajectory();
  }

}
/*------------------------------------------------------------------------------------------------------*/
bool DAFTrackProducerAlgorithm::buildTrack (const Trajectory vtraj,
					    AlgoProductCollection& algoResults,
					    float ndof,
					    const reco::BeamSpot& bs) const
{
  //variable declarations
  reco::Track * theTrack;
  Trajectory * theTraj; 
      
  LogDebug("DAFTrackProducerAlgorithm") <<" BUILDER " << std::endl;;
  TrajectoryStateOnSurface innertsos;
  
  if ( vtraj.isValid() ){

    theTraj = new Trajectory( vtraj );
    
    if (vtraj.direction() == alongMomentum) {
    //if (theTraj->direction() == oppositeToMomentum) {
      innertsos = vtraj.firstMeasurement().updatedState();
    } else { 
      innertsos = vtraj.lastMeasurement().updatedState();
    }
    
    TSCBLBuilderNoMaterial tscblBuilder;
    TrajectoryStateClosestToBeamLine tscbl = tscblBuilder(*(innertsos.freeState()),bs);

    if (tscbl.isValid()==false) return false;

    GlobalPoint v = tscbl.trackStateAtPCA().position();
    math::XYZPoint  pos( v.x(), v.y(), v.z() );
    GlobalVector p = tscbl.trackStateAtPCA().momentum();
    math::XYZVector mom( p.x(), p.y(), p.z() );

    //    LogDebug("TrackProducer") <<v<<p<<std::endl;

    theTrack = new reco::Track(vtraj.chiSquared(),
			       ndof, //in the DAF the ndof is not-integer
			       pos, mom, tscbl.trackStateAtPCA().charge(), 
			       tscbl.trackStateAtPCA().curvilinearError());

    AlgoProduct aProduct(theTraj,std::make_pair(theTrack,vtraj.direction()));
    algoResults.push_back(aProduct);

    return true;
  } 
  else {
    LogDebug("DAFTrackProducerAlgorithm") <<" BUILDER NOT POSSIBLE: traj is not valid" << std::endl;;
    return false;
  }
}
/*------------------------------------------------------------------------------------------------------*/
int DAFTrackProducerAlgorithm::countingGoodHits(const Trajectory traj) const{

  int ngoodhits = 0;
  Trajectory myTraj = traj;
  std::vector<TrajectoryMeasurement> vtm = traj.measurements();

  for (std::vector<TrajectoryMeasurement>::const_iterator tm = vtm.begin(); tm != vtm.end(); tm++){
    //if the rechit is valid
    if (tm->recHit()->isValid()) {
      SiTrackerMultiRecHit const & mHit = dynamic_cast<SiTrackerMultiRecHit const &>(*tm->recHit());
      std::vector<const TrackingRecHit*> components = mHit.recHits();

      int iComp = 0;

      for(std::vector<const TrackingRecHit*>::const_iterator iter = components.begin(); iter != components.end(); iter++, iComp++){ 
        //if there is at least one component with weight higher than 1e-6 then the hit is not an outlier
        if (mHit.weight(iComp)>1e-6) {
	  ngoodhits++; 
	  iComp++; 
	  break;
	}
      }

    }   
  }
  
  LogDebug("DAFTrackProducerAlgorithm") << "Original number of valid hits " << traj.foundHits() << " -> hit with good weight (>1e-6) are " << ngoodhits;
  return ngoodhits;

}
/*------------------------------------------------------------------------------------------------------*/

void  DAFTrackProducerAlgorithm::filter(const TrajectoryFitter* fitter, std::vector<Trajectory>& input, 
					int minhits, std::vector<Trajectory>& output,
					const TransientTrackingRecHitBuilder* builder) const 
{
  if (input.empty()) return;

  int ngoodhits = 0;
  std::vector<TrajectoryMeasurement> vtm = input[0].measurements();	
  TransientTrackingRecHit::RecHitContainer hits;

  //count the number of non-outlier and non-invalid hits	
  for (std::vector<TrajectoryMeasurement>::reverse_iterator tm=vtm.rbegin(); tm!=vtm.rend();tm++){
    //if the rechit is valid
    if (tm->recHit()->isValid()) {
      SiTrackerMultiRecHit const & mHit = dynamic_cast<SiTrackerMultiRecHit const &>(*tm->recHit());
      std::vector<const TrackingRecHit*> components = mHit.recHits();
      int iComp = 0;
      bool isGood = false;
      for(std::vector<const TrackingRecHit*>::const_iterator iter = components.begin(); iter != components.end(); iter++, iComp++){        
	//if there is at least one component with weight higher than 1e-6 then the hit is not an outlier
        if (mHit.weight(iComp)>1e-6) {ngoodhits++; iComp++; isGood = true; break;}
      }
      if (isGood) {
        TkClonerImpl hc = static_cast<TkTransientTrackingRecHitBuilder const *>(builder)->cloner();
        auto tempHit = hc.makeShared(tm->recHit(),tm->updatedState());
        hits.push_back(tempHit);
      }
      else hits.push_back(std::make_shared<InvalidTrackingRecHit>(*tm->recHit()->det(), TrackingRecHit::missing));
    } else {
      TkClonerImpl hc = static_cast<TkTransientTrackingRecHitBuilder const *>(builder)->cloner();
      auto tempHit = hc.makeShared(tm->recHit(),tm->updatedState());
      hits.push_back(tempHit);
    }
  }


	LogDebug("DAFTrackProducerAlgorithm") << "Original number of valid hits " << input[0].foundHits() << "; after filtering " << ngoodhits;
	//debug
	if (ngoodhits>input[0].foundHits()) edm::LogError("DAFTrackProducerAlgorithm") << "Something wrong: the number of good hits from DAFTrackProducerAlgorithm::filter " << ngoodhits << " is higher than the original one " << input[0].foundHits();
	
	if (ngoodhits < minhits) return;	

	TrajectoryStateOnSurface curstartingTSOS = input.front().lastMeasurement().updatedState();
	LogDebug("DAFTrackProducerAlgorithm") << "starting tsos for final refitting " << curstartingTSOS ;
        //curstartingTSOS.rescaleError(100);

	output = fitter->fit(TrajectorySeed(PTrajectoryStateOnDet(),
                                                BasicTrajectorySeed::recHitContainer(),
                                                input.front().seed().direction()),
                                hits,
                                TrajectoryStateWithArbitraryError()(curstartingTSOS));
	LogDebug("DAFTrackProducerAlgorithm") << "After filtering " << output.size() << " trajectories";

}
/*------------------------------------------------------------------------------------------------------*/
float DAFTrackProducerAlgorithm::calculateNdof(const Trajectory vtraj) const 
{

  if (!vtraj.isValid()) return 0;
  float ndof = 0;
  const std::vector<TrajectoryMeasurement>& meas = vtraj.measurements();
  for (std::vector<TrajectoryMeasurement>::const_iterator iter = meas.begin(); iter != meas.end(); iter++){
    if (iter->recHit()->isValid()){
      SiTrackerMultiRecHit const & mHit = dynamic_cast<SiTrackerMultiRecHit const &>(*iter->recHit());
      std::vector<const TrackingRecHit*> components = mHit.recHits();
      int iComp = 0;
      for(std::vector<const TrackingRecHit*>::const_iterator iter2 = components.begin(); iter2 != components.end(); iter2++, iComp++){
        if ((*iter2)->isValid())
	  ndof += ((*iter2)->dimension())*mHit.weight(iComp);
      }

    }
  }

  return ndof-5;

}
//------------------------------------------------------------------------------------------------
int DAFTrackProducerAlgorithm::checkHits( Trajectory iInitTraj, const Trajectory iFinalTraj) const {

  std::vector<TrajectoryMeasurement> initmeasurements = iInitTraj.measurements();
  std::vector<TrajectoryMeasurement> finalmeasurements = iFinalTraj.measurements();
  std::vector<TrajectoryMeasurement>::iterator imeas;
  std::vector<TrajectoryMeasurement>::iterator jmeas;
  int nSame = 0;
  int ihit = 0;

  if( initmeasurements.empty() || finalmeasurements.empty() ){
    LogDebug("DAFTrackProducerAlgorithm") << "Initial or Final Trajectory empty.";
    return 0;
  }

  if( initmeasurements.size() != finalmeasurements.size() ) {
    LogDebug("DAFTrackProducerAlgorithm") << "Initial and Final Trajectory have different size.";
    return 0;
  }
          
  for(jmeas = initmeasurements.begin(); jmeas != initmeasurements.end(); jmeas++){

    const TrackingRecHit* initHit = jmeas->recHit()->hit();

    if(!initHit->isValid() && ihit == 0 ) continue;

    if(initHit->isValid()){

      TrajectoryMeasurement imeas = finalmeasurements.at(ihit);
      const TrackingRecHit* finalHit = imeas.recHit()->hit();
      const TrackingRecHit* MaxWeightHit=0;
      float maxweight = 0;
  
      const SiTrackerMultiRecHit* mrh = dynamic_cast<const SiTrackerMultiRecHit*>(finalHit);
      if (mrh){
        std::vector<const TrackingRecHit*> components = mrh->recHits();
        std::vector<const TrackingRecHit*>::const_iterator icomp;
        int hitcounter=0;

        for (icomp = components.begin(); icomp != components.end(); icomp++) {
          if((*icomp)->isValid()) {
            double weight = mrh->weight(hitcounter);
            if(weight > maxweight) {
              MaxWeightHit = *icomp;
              maxweight = weight;
            }
          }
          hitcounter++;
        }

        auto myref1 = reinterpret_cast<const BaseTrackerRecHit *>(initHit)->firstClusterRef();
        auto myref2 = reinterpret_cast<const BaseTrackerRecHit *>(MaxWeightHit)->firstClusterRef();

        if( myref1 == myref2 ){
          nSame++; 	
        }
      }
    } else {

      TrajectoryMeasurement imeas = finalmeasurements.at(ihit);
      const TrackingRecHit* finalHit = imeas.recHit()->hit();
      if(!finalHit->isValid()){
        nSame++;
      }
    }
    
      ihit++;
  }

  return nSame;
}



void DAFTrackProducerAlgorithm::PrintHit(const TrackingRecHit* const& hit, TrajectoryStateOnSurface& tsos) const
{
    if (hit->isValid()){

      LogTrace("DAFTrackProducerAlgorithm") << "  Valid Hit with DetId " << hit->geographicalId().rawId() << " and dim:" << hit->dimension()
                      //<< " type " << typeid(hit).name()
                        << " local position " << hit->localPosition()
                        << " global position " << hit->globalPosition()
                        << " and r " << hit->globalPosition().perp() ;
      LogTrace("DAFTrackProducerAlgorithm") << "  TSOS combtsos " << tsos.localPosition() ;

    } else {
      LogTrace("DAFTrackProducerAlgorithm") << "  Invalid Hit with DetId " << hit->geographicalId().rawId();
    }

}

