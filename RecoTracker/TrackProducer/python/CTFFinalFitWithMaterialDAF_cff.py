import FWCore.ParameterSet.Config as cms


# KFUpdatorESProducer used by the fitting-smoother for MRH
from TrackingTools.KalmanUpdators.MRHKFUpdatorESProducer_cfi import *

# Chi2MeasurementEstimatorESProducer used by the fitting-smootheri for MRH
from TrackingTools.KalmanUpdators.MRHChi2MeasurementEstimatorESProducer_cfi import *

import copy
from TrackingTools.MaterialEffects.OppositeMaterialPropagator_cfi import *
# PropagatorWithMaterialESProducer
OppositeRungeKuttaTrackerPropagator = copy.deepcopy(OppositeMaterialPropagator)
OppositeRungeKuttaTrackerPropagator.ComponentName = 'OppositeRungeKuttaTrackerPropagator'
OppositeRungeKuttaTrackerPropagator.useRungeKutta = True

# KFTrajectoryFitterESProducer
from TrackingTools.TrackFitters.TrackFitters_cff import *

#MultiMeasurementTracker
from RecoTracker.SiTrackerMRHTools.GroupedMultiRecHitCollector_cff import *
from RecoTracker.SiTrackerMRHTools.SimpleMultiRecHitCollector_cff import *

#multiRecHitUpdator
from RecoTracker.SiTrackerMRHTools.SiTrackerMultiRecHitUpdator_cff import *


# stripCPE
from RecoLocalTracker.SiStripRecHitConverter.StripCPEfromTrackAngle_cfi import *
from RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitMatcher_cfi import *

# pixelCPE
from RecoLocalTracker.SiPixelRecHits.PixelCPEParmError_cfi import *

#TransientTrackingBuilder
from RecoTracker.TransientTrackingRecHit.TransientTrackingRecHitBuilder_cfi import *

# TrackProducer
from RecoTracker.TrackProducer.CTFFinalFitWithMaterialDAF_cfi import *


