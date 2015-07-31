#include "RecoLocalTracker/SiPixelStubBuilder/plugins/SiPixelStubBuilder.h"
#include "RecoLocalTracker/SiPixelStubBuilder/interface/VectorHitBuilderAlgorithm.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

SiPixelStubBuilder::SiPixelStubBuilder(edm::ParameterSet const& conf)
  : offlinestubsTag( conf.getParameter<std::string>( "offlinestubs" ) ),
    maxOfflinestubs(conf.getParameter<int>( "maxStubs" )),
    algoTag(conf.getParameter<std::string>( "Algorithm" )),
    clusterProducer(conf.getParameter<edm::InputTag>("Clusters")),
    readytobuild(false)
{
  produces< edmNew::DetSetVector< Phase2TrackerCluster1D > >( "ClustersAccepted" );
  produces< edmNew::DetSetVector< Phase2TrackerCluster1D > >( "ClustersRejected" );
  produces< VectorHitCollectionNew >( offlinestubsTag + "Accepted" );
  produces< VectorHitCollectionNew >( offlinestubsTag + "Rejected" );
  setupAlgorithm(conf);
}

SiPixelStubBuilder::~SiPixelStubBuilder() {
  delete stubsBuilder;
}

void SiPixelStubBuilder::produce(edm::Event& event, const edm::EventSetup& es)
{
  LogDebug("SiPixelStubBuilder") << "SiPixelStubBuilder::produce() begin";

  // get input clusters data
  edm::Handle< edmNew::DetSetVector<Phase2TrackerCluster1D> >  clustersHandle;
  event.getByLabel( clusterProducer, clustersHandle);

  // create the final output collection
  std::auto_ptr< edmNew::DetSetVector< Phase2TrackerCluster1D > > outputClustersAccepted( new edmNew::DetSetVector< Phase2TrackerCluster1D > );
  std::auto_ptr< edmNew::DetSetVector< Phase2TrackerCluster1D > > outputClustersRejected( new edmNew::DetSetVector< Phase2TrackerCluster1D > );
  std::auto_ptr< VectorHitCollectionNew > outputVHAccepted( new VectorHitCollectionNew() );
  std::auto_ptr< VectorHitCollectionNew > outputVHRejected( new VectorHitCollectionNew() );

  if(readytobuild)  stubsBuilder->initialize(es);
  else edm::LogError("SiPixelStubBuilder") << "Impossible initialization of builder!!";

  // check on the input clusters
  stubsBuilder->printClusters(*clustersHandle);

  // running the stub building algorithm
  //ERICA::output should be moved in the different algo classes?
  run( *clustersHandle, *outputClustersAccepted, *outputClustersRejected, *outputVHAccepted, *outputVHRejected);

  unsigned int numberOfStubs = 0;
  edmNew::DetSetVector<VectorHit>::const_iterator DSViter;
  for( DSViter = (*outputVHAccepted).begin() ; DSViter != (*outputVHAccepted).end(); DSViter++){

    edmNew::DetSet< VectorHit >::const_iterator vh;
    for ( vh = DSViter->begin(); vh != DSViter->end(); ++vh) {
      numberOfStubs++;
      LogDebug("SiPixelStubBuilder") << "\t vectorhit in output " << *vh << std::endl;
    }

  }
/*
  if(numberOfStubs > maxOfflinestubs) {
    edm::LogError("SiPixelStubBuilder") <<  "Limit on the number of stubs exceeded. An empty output collection will be produced instead.\n";
    VectorHitCollectionNew empty;
    empty.swap(outputAcc);
  }
*/
  // write output to file
  event.put( outputClustersAccepted, "ClustersAccepted" );
  event.put( outputClustersRejected, "ClustersRejected" );
  event.put( outputVHAccepted, offlinestubsTag + "Accepted" );
  event.put( outputVHRejected, offlinestubsTag + "Rejected" );

//  LogDebug("SiPixelStubBuilder") << " Executing " << algoTag << " resulted in " << numberOfStubs << ".";
  LogDebug("SiPixelStubBuilder") << "found\n" << numberOfStubs << " .\n" ;

}

void SiPixelStubBuilder::setupAlgorithm(edm::ParameterSet const& conf) {

  if ( algoTag == "VectorHitBuilderAlgorithm" ) {
    stubsBuilder = new VectorHitBuilderAlgorithm(conf);
    readytobuild = true;
  } else {
    edm::LogError("SiPixelStubBuilder") << " Choice " << algoTag << " is invalid.\n" ;
    readytobuild = false;
  }

}


void SiPixelStubBuilder::run(const edmNew::DetSetVector<Phase2TrackerCluster1D>& clusters,
   edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersAcc, edmNew::DetSetVector<Phase2TrackerCluster1D>& clustersRej,
   VectorHitCollectionNew& outputAcc, VectorHitCollectionNew& outputRej ){

  if ( !readytobuild ) {
    edm::LogError("SiPixelStubBuilder") << " No stub builder algorithm was found - cannot run!" ;
    return;
  }

  stubsBuilder->run(clusters, outputAcc, outputRej, clustersAcc, clustersRej);

}
