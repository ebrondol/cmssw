#include "DataFormats/TrackCandidate/interface/TrackCandidate.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
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
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateWithArbitraryError.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TSiTrackerMultiRecHit.h"

#include "TrackingTools/PatternTools/interface/TrajAnnealing.h"

void DAFTrackProducerAlgorithm::runWithCandidate(const TrackingGeometry * theG,
					         const MagneticField * theMF,
					         //const TrackCandidateCollection& theTrajectoryCollection,
						 const std::vector<Trajectory>& theTrajectoryCollection,
						 const MeasurementTrackerEvent *measTk,
					         const TrajectoryFitter * theFitter,
					         const TransientTrackingRecHitBuilder* builder,
						 const MultiRecHitCollector* measurementCollector,
						 const SiTrackerMultiRecHitUpdator* updator,
						 const reco::BeamSpot& bs,
					         AlgoProductCollection& algoResults,
						 TrajAnnealingCollection& trajann) const
{
  std::cout << "//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
  std::cout << "DAFTrackProducerAlgorithm::runWithCandidate: Number of Trajectories: " << theTrajectoryCollection.size() << std::endl;
  edm::LogInfo("TrackProducer") << "Number of Trajectories: " << theTrajectoryCollection.size() << "\n";
  int cont = 0;

  //running on src trajectory collection
  for (std::vector<Trajectory>::const_iterator ivtraj = theTrajectoryCollection.begin(); 
       ivtraj != theTrajectoryCollection.end(); ivtraj++)
  {

    float ndof = 0;
    Trajectory currentTraj;
    std::vector< std::pair<float, std::vector<float> >> annweight;

    //getting trajectory
    //no need to have std::vector<Trajectory> vtraj !
    if ( (*ivtraj).isValid() ){

      std::cout << " The trajectory is valid. " << std::endl;
      edm::LogInfo("TrackProducer") << "The trajectory is valid. \n";

      //getting the MultiRecHit collection and the trajectory with a first fit-smooth round
      std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface> hits = 
							collectHits(*ivtraj, measurementCollector, &*measTk);

      currentTraj = fit(hits, theFitter, *ivtraj);

      std::cout << " Starting the annealing program." << std::endl;
      //starting the annealing program
      for (std::vector<double>::const_iterator ian = updator->getAnnealingProgram().begin(); 
           ian != updator->getAnnealingProgram().end(); ian++){

        if (currentTraj.isValid()){

	  LogDebug("DAFTrackProducerAlgorithm") << "Seed direction is " << currentTraj.seed().direction() 
	 	                                << ".Traj direction is " << currentTraj.direction();

          //updating MultiRecHits and fit-smooth again 
	  std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface> curiterationhits = 
									updateHits(currentTraj,updator,*ian);
	  currentTraj = fit(curiterationhits, theFitter, currentTraj);

 	  //saving ...
  	  //Trajectory * theCurrTraj = new Trajectory( currentTraj );	
	  //TrajAnnealing temp(theCurrTraj, *ian);
	  TrajAnnealing temp(currentTraj, *ian);
	  trajann.push_back(temp);

	  for(TransientTrackingRecHit::RecHitContainer::const_iterator ihit = curiterationhits.first.begin();
              ihit != curiterationhits.first.end(); ++ihit) {
	    std::pair<float, std::vector<float> > temp = getAnnealingWeight(**ihit);
	    annweight.push_back(temp);
 	  }

          std::cout << " Ending the annealing program with value "<<(*ian)<<std::endl;
          LogDebug("DAFTrackProducerAlgorithm") << "done annealing value "  <<  (*ian) ;

	} 
        else break;

      } //end of annealing program

      LogDebug("DAFTrackProducerAlgorithm") << "Ended annealing program " << std::endl;

      //computing the ndof keeping into account the weights
      ndof = calculateNdof(currentTraj);

      //checking if the trajectory has the minimum number of valid hits ( weight (>1e-6) )
      //in order to remove tracks with too many outliers.

      //std::vector<Trajectory> filtered;
      //filter(theFitter, vtraj, conf_.getParameter<int>("MinHits"), filtered);				

      if(currentTraj.foundHits() >= conf_.getParameter<int>("MinHits")) {
      
        bool ok = buildTrack(currentTraj, algoResults, ndof, bs) ;
        if(ok) cont++;

      }
      else{
        LogDebug("DAFTrackProducerAlgorithm")  << "Rejecting trajectory with " 
 					       << currentTraj.foundHits()<<" hits"; 
      }
    }
    else 
      LogDebug("DAFTrackProducerAlgorithm") << "Rejecting empty trajectory" << std::endl;

  } //end run on track collection

  std::cout << "DAFTrackProducerAlgorithm::runWithCandidate: End. \n";
  std::cout << "//////////////////////////////////////////////////////////////////////////////////"<<std::endl;
  LogDebug("TrackProducer") << "Number of Tracks found: " << cont << "\n";

}
/*------------------------------------------------------------------------------------------------------*/
std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface>
DAFTrackProducerAlgorithm::collectHits(const Trajectory vtraj,
                                       const MultiRecHitCollector* measurementCollector,
                                       const MeasurementTrackerEvent *measTk) const
{

  std::cout << " Collecting hits ..." << std::endl; 
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
  
  std::cout << " Collected " << nHits << " MRHits. " << std::endl;

  //TrajectoryStateWithArbitraryError() == Creates a TrajectoryState with the same parameters 
  //     as the input one, but with "infinite" errors, i.e. errors so big that they don't
  //     bias a fit starting with this state. 
  
  return std::make_pair(hits,TrajectoryStateWithArbitraryError()(collectedmeas.front().predictedState()));

}
/*------------------------------------------------------------------------------------------------------*/
std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface>
DAFTrackProducerAlgorithm::updateHits(const Trajectory vtraj,
				      const SiTrackerMultiRecHitUpdator* updator,
				      double annealing) const 
{
	TransientTrackingRecHit::RecHitContainer hits;
	std::vector<TrajectoryMeasurement> vmeas = vtraj.measurements();
        std::vector<TrajectoryMeasurement>::reverse_iterator imeas;

	//I run inversely on the trajectory obtained and update the state
        for (imeas = vmeas.rbegin(); imeas != vmeas.rend(); imeas++){
              TransientTrackingRecHit::RecHitPointer updated = updator->update(imeas->recHit(), 
							imeas->updatedState(), annealing);
             hits.push_back(updated);
        }
        return std::make_pair(hits,TrajectoryStateWithArbitraryError()(vmeas.back().updatedState()));
}
/*------------------------------------------------------------------------------------------------------*/
Trajectory DAFTrackProducerAlgorithm::fit(const std::pair<TransientTrackingRecHit::RecHitContainer,
                                    TrajectoryStateOnSurface>& hits,
                                    const TrajectoryFitter * theFitter,
                                    Trajectory vtraj) const {

//  std::cout << "Calling DAFTrackProducerAlgorithm::fit" << std::endl;

  //creating a new trajectory starting from the direction of the seed of the input one and the hits
  Trajectory newVec = theFitter->fitOne(TrajectorySeed(PTrajectoryStateOnDet(),
                                                                 BasicTrajectorySeed::recHitContainer(),
                                                                 vtraj.seed().direction()),
                                                                 hits.first, hits.second);

  if( newVec.isValid() )  return newVec; 
  else{
    std::cout << "Fit no valid." << std::endl;
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
      
//  std::cout <<"Calling DAFTrackProducerAlgorithm::buildTrack" << std::endl;
  LogDebug("DAFTrackProducerAlgorithm") <<" BUILDER " << std::endl;;
  TrajectoryStateOnSurface innertsos;
  
  if ( vtraj.isValid() ){

    theTraj = new Trajectory( vtraj );
    
    if (vtraj.direction() == alongMomentum) {
    //if (theTraj->direction() == oppositeToMomentum) {
      innertsos = vtraj.firstMeasurement().updatedState();
      //std::cout << "Inner momentum " << innertsos.globalParameters().momentum().mag() << std::endl;	
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
  else  
    return false;
}
/*------------------------------------------------------------------------------------------------------*/
void  DAFTrackProducerAlgorithm::filter(const TrajectoryFitter* fitter, std::vector<Trajectory>& input, 
					int minhits, std::vector<Trajectory>& output) const 
{
	if (input.empty()) return;

	int ngoodhits = 0;

	std::vector<TrajectoryMeasurement> vtm = input[0].measurements();	

	TransientTrackingRecHit::RecHitContainer hits;

	//count the number of non-outlier and non-invalid hits	
	for (std::vector<TrajectoryMeasurement>::reverse_iterator tm=vtm.rbegin(); tm!=vtm.rend();tm++){
		//if the rechit is valid
		if (tm->recHit()->isValid()) {
			TransientTrackingRecHit::ConstRecHitContainer components = tm->recHit()->transientHits();
			bool isGood = false;
			for (TransientTrackingRecHit::ConstRecHitContainer::iterator rechit = components.begin(); rechit != components.end(); rechit++){
				//if there is at least one component with weight higher than 1e-6 then the hit is not an outlier
				if ((*rechit)->weight()>1e-6) {ngoodhits++; isGood = true; break;}
			}
			if (isGood) hits.push_back(tm->recHit()->clone(tm->updatedState()));
                        else hits.push_back(InvalidTransientRecHit::build(tm->recHit()->det(), TrackingRecHit::missing));
		} else {
			hits.push_back(tm->recHit()->clone(tm->updatedState()));
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
      TransientTrackingRecHit::ConstRecHitContainer components = iter->recHit()->transientHits();
      TransientTrackingRecHit::ConstRecHitContainer::const_iterator iter2;

      for (iter2 = components.begin(); iter2 != components.end(); iter2++){
        if ((*iter2)->isValid())
	  ndof += ((*iter2)->dimension())*(*iter2)->weight();
      }

    }
  }

  return ndof-5;

}
//------------------------------------------------------------------------------------------------
std::pair<float, std::vector<float> > DAFTrackProducerAlgorithm::getAnnealingWeight( const TrackingRecHit& aRecHit ) const {

  if (!aRecHit.isValid()) {
    std::cout << "Invalid RecHit passed " << std::endl;
  }

  TSiTrackerMultiRecHit const & mHit = dynamic_cast<TSiTrackerMultiRecHit const &>(aRecHit);
  return make_pair(mHit.getAnnealingFactor(), mHit.weights());

}

