import FWCore.ParameterSet.Config as cms

OccupancyPlotsPixelWantedSubDets = cms.VPSet ()
OccupancyPlotsPixelWantedSubDetsGeneral = cms.VPSet (
    # Barrel : 10 layers
    cms.PSet(detSelection=cms.uint32(101),detLabel=cms.string("PXB_Layer01"),selection=cms.untracked.vstring("0x1ef00000-0x12100000")),
    cms.PSet(detSelection=cms.uint32(102),detLabel=cms.string("PXB_Layer02"),selection=cms.untracked.vstring("0x1ef00000-0x12200000")),
    cms.PSet(detSelection=cms.uint32(103),detLabel=cms.string("PXB_Layer03"),selection=cms.untracked.vstring("0x1ef00000-0x12300000")),
    cms.PSet(detSelection=cms.uint32(104),detLabel=cms.string("PXB_Layer04"),selection=cms.untracked.vstring("0x1ef00000-0x12400000")),
    cms.PSet(detSelection=cms.uint32(105),detLabel=cms.string("TOB_Layer01"),selection=cms.untracked.vstring("0x1ef00000-0x1a100000")),
    cms.PSet(detSelection=cms.uint32(106),detLabel=cms.string("TOB_Layer02"),selection=cms.untracked.vstring("0x1ef00000-0x1a200000")),
    cms.PSet(detSelection=cms.uint32(107),detLabel=cms.string("TOB_Layer03"),selection=cms.untracked.vstring("0x1ef00000-0x1a300000")),
    cms.PSet(detSelection=cms.uint32(108),detLabel=cms.string("TOB_Layer04"),selection=cms.untracked.vstring("0x1ef00000-0x1a400000")),
    cms.PSet(detSelection=cms.uint32(109),detLabel=cms.string("TOB_Layer05"),selection=cms.untracked.vstring("0x1ef00000-0x1a500000")),
    cms.PSet(detSelection=cms.uint32(110),detLabel=cms.string("TOB_Layer06"),selection=cms.untracked.vstring("0x1ef00000-0x1a600000")),
    # Endcap
    cms.PSet(detSelection=cms.uint32(211),detLabel=cms.string("PXF_Side1-_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x14840000")),
    cms.PSet(detSelection=cms.uint32(212),detLabel=cms.string("PXF_Side1-_Disk02"),selection=cms.untracked.vstring("0x1fbc0000-0x14880000")),
    cms.PSet(detSelection=cms.uint32(213),detLabel=cms.string("PXF_Side1-_Disk03"),selection=cms.untracked.vstring("0x1fbc0000-0x148c0000")),
    cms.PSet(detSelection=cms.uint32(214),detLabel=cms.string("PXF_Side1-_Disk04"),selection=cms.untracked.vstring("0x1fbc0000-0x14900000")),
    cms.PSet(detSelection=cms.uint32(215),detLabel=cms.string("PXF_Side1-_Disk05"),selection=cms.untracked.vstring("0x1fbc0000-0x14940000")),
    cms.PSet(detSelection=cms.uint32(216),detLabel=cms.string("PXF_Side1-_Disk06"),selection=cms.untracked.vstring("0x1fbc0000-0x14980000")),
    cms.PSet(detSelection=cms.uint32(217),detLabel=cms.string("PXF_Side1-_Disk07"),selection=cms.untracked.vstring("0x1fbc0000-0x149c0000")),
    cms.PSet(detSelection=cms.uint32(218),detLabel=cms.string("PXF_Side1-_Disk08"),selection=cms.untracked.vstring("0x1fbc0000-0x14a00000")),
    cms.PSet(detSelection=cms.uint32(219),detLabel=cms.string("PXF_Side1-_Disk09"),selection=cms.untracked.vstring("0x1fbc0000-0x14a40000")),
    cms.PSet(detSelection=cms.uint32(210),detLabel=cms.string("PXF_Side1-_Disk10"),selection=cms.untracked.vstring("0x1fbc0000-0x14a80000")),
    #cms.PSet(detSelection=cms.uint32(211),detLabel=cms.string("TID_Side2-_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x18840000")),
    #cms.PSet(detSelection=cms.uint32(212),detLabel=cms.string("TID_Side2-_Disk02"),selection=cms.untracked.vstring("0x1fbc0000-0x18880000")),
    #cms.PSet(detSelection=cms.uint32(213),detLabel=cms.string("TID_Side2-_Disk03"),selection=cms.untracked.vstring("0x1fbc0000-0x188c0000")),
    #cms.PSet(detSelection=cms.uint32(214),detLabel=cms.string("TID_Side2-_Disk04"),selection=cms.untracked.vstring("0x1fbc0000-0x18900000")),
    #cms.PSet(detSelection=cms.uint32(215),detLabel=cms.string("TID_Side2-_Disk05"),selection=cms.untracked.vstring("0x1fbc0000-0x18940000")),
    ## Endcap   
    cms.PSet(detSelection=cms.uint32(251),detLabel=cms.string("PXF_Side2+_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x15040000")),
    cms.PSet(detSelection=cms.uint32(252),detLabel=cms.string("PXF_Side2+_Disk02"),selection=cms.untracked.vstring("0x1fbc0000-0x15080000")),
    cms.PSet(detSelection=cms.uint32(253),detLabel=cms.string("PXF_Side2+_Disk03"),selection=cms.untracked.vstring("0x1fbc0000-0x150c0000")),
    cms.PSet(detSelection=cms.uint32(254),detLabel=cms.string("PXF_Side2+_Disk04"),selection=cms.untracked.vstring("0x1fbc0000-0x15100000")),
    cms.PSet(detSelection=cms.uint32(255),detLabel=cms.string("PXF_Side2+_Disk05"),selection=cms.untracked.vstring("0x1fbc0000-0x15140000")),
    cms.PSet(detSelection=cms.uint32(256),detLabel=cms.string("PXF_Side2+_Disk06"),selection=cms.untracked.vstring("0x1fbc0000-0x15180000")),
    cms.PSet(detSelection=cms.uint32(257),detLabel=cms.string("PXF_Side2+_Disk07"),selection=cms.untracked.vstring("0x1fbc0000-0x151c0000")),
    cms.PSet(detSelection=cms.uint32(258),detLabel=cms.string("PXF_Side2+_Disk08"),selection=cms.untracked.vstring("0x1fbc0000-0x15200000")),
    cms.PSet(detSelection=cms.uint32(259),detLabel=cms.string("PXF_Side2+_Disk09"),selection=cms.untracked.vstring("0x1fbc0000-0x15240000")),
    cms.PSet(detSelection=cms.uint32(260),detLabel=cms.string("PXF_Side2+_Disk10"),selection=cms.untracked.vstring("0x1fbc0000-0x15280000")),
    #cms.PSet(detSelection=cms.uint32(261),detLabel=cms.string("TID_Side2+_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x19040000")),
    #cms.PSet(detSelection=cms.uint32(262),detLabel=cms.string("TID_Side2+_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x19080000")),
    #cms.PSet(detSelection=cms.uint32(263),detLabel=cms.string("TID_Side2+_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x190c0000")),
    #cms.PSet(detSelection=cms.uint32(264),detLabel=cms.string("TID_Side2+_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x19100000")),
    #cms.PSet(detSelection=cms.uint32(265),detLabel=cms.string("TID_Side2+_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x19140000"))

)
OccupancyPlotsPXBWantedSubDets = cms.VPSet (
)
OccupancyPlotsPXFmWantedSubDets = cms.VPSet (
)
OccupancyPlotsPXFpWantedSubDets = cms.VPSet (
)

OccupancyPlotsPXFWantedSubDets = OccupancyPlotsPXFmWantedSubDets
OccupancyPlotsPXFWantedSubDets.extend(OccupancyPlotsPXFpWantedSubDets)

OccupancyPlotsPixelWantedSubDets.extend(OccupancyPlotsPixelWantedSubDetsGeneral)
OccupancyPlotsPixelWantedSubDets.extend(OccupancyPlotsPXBWantedSubDets)
OccupancyPlotsPixelWantedSubDets.extend(OccupancyPlotsPXFWantedSubDets)

OccupancyPlotsTOBWantedSubDets = cms.VPSet (
)
OccupancyPlotsPixelWantedSubDets.extend(OccupancyPlotsTOBWantedSubDets)


OccupancyPlotsFPIXmD1DetailedWantedSubDets = cms.VPSet(
)
OccupancyPlotsFPIXmDetailedWantedSubDets = OccupancyPlotsFPIXmD1DetailedWantedSubDets


OccupancyPlotsFPIXpD1DetailedWantedSubDets = cms.VPSet(
)
OccupancyPlotsFPIXpDetailedWantedSubDets = OccupancyPlotsFPIXpD1DetailedWantedSubDets
