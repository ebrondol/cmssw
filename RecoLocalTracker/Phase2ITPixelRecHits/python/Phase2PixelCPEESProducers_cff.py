import FWCore.ParameterSet.Config as cms

#
# Load all Pixel Cluster Position Estimator ESProducers
#
#
# 1. RecHits using angles from module position (EM 2016.05.17: Phase2PixelCPEInitial_cfi is not loading anything)
#
# from RecoLocalTracker.Phase2ITPixelRecHits.Phase2PixelCPEInitial_cfi import *
#
# 2. TrackingRechits using angles from tracks(EM 2016.05.17: Phase2PixelCPEParamError_cfi is not loading anything)
#
# from RecoLocalTracker.Phase2ITPixelRecHits.Phase2PixelCPEParmError_cfi import *
#
# 3. Template algorithm (EM 2016.05.17: skipped for the moment) 
#
# 4. Pixel Generic CPE
#
from RecoLocalTracker.Phase2ITPixelRecHits.Phase2PixelCPEGeneric_cfi import *
#
# 5. The new ESProducer for the Magnetic-field dependent template record (EM 2016.05.17: skipped for the moment) 
#

