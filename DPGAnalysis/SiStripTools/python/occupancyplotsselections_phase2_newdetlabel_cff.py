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
    # Endcap : 15 side -
    cms.PSet(detSelection=cms.uint32(201),detLabel=cms.string("PXF_Side1-_Disk01"),selection=cms.untracked.vstring("0x1fbc0000-0x14840000")),
    cms.PSet(detSelection=cms.uint32(202),detLabel=cms.string("PXF_Side1-_Disk02"),selection=cms.untracked.vstring("0x1fbc0000-0x14880000")),
    cms.PSet(detSelection=cms.uint32(203),detLabel=cms.string("PXF_Side1-_Disk03"),selection=cms.untracked.vstring("0x1fbc0000-0x148c0000")),
    cms.PSet(detSelection=cms.uint32(204),detLabel=cms.string("PXF_Side1-_Disk04"),selection=cms.untracked.vstring("0x1fbc0000-0x14900000")),
    cms.PSet(detSelection=cms.uint32(205),detLabel=cms.string("PXF_Side1-_Disk05"),selection=cms.untracked.vstring("0x1fbc0000-0x14940000")),
    cms.PSet(detSelection=cms.uint32(206),detLabel=cms.string("PXF_Side1-_Disk06"),selection=cms.untracked.vstring("0x1fbc0000-0x14980000")),
    cms.PSet(detSelection=cms.uint32(207),detLabel=cms.string("PXF_Side1-_Disk07"),selection=cms.untracked.vstring("0x1fbc0000-0x149c0000")),
    cms.PSet(detSelection=cms.uint32(208),detLabel=cms.string("PXF_Side1-_Disk08"),selection=cms.untracked.vstring("0x1fbc0000-0x14a00000")),
    cms.PSet(detSelection=cms.uint32(209),detLabel=cms.string("PXF_Side1-_Disk09"),selection=cms.untracked.vstring("0x1fbc0000-0x14a40000")),
    cms.PSet(detSelection=cms.uint32(210),detLabel=cms.string("PXF_Side1-_Disk10"),selection=cms.untracked.vstring("0x1fbc0000-0x14a80000")),
    cms.PSet(detSelection=cms.uint32(211),detLabel=cms.string("TID_Side1-_Wheel01"),selection=cms.untracked.vstring("0x1fbc0000-0x18840000")),
    cms.PSet(detSelection=cms.uint32(212),detLabel=cms.string("TID_Side1-_Wheel02"),selection=cms.untracked.vstring("0x1fbc0000-0x18880000")),
    cms.PSet(detSelection=cms.uint32(213),detLabel=cms.string("TID_Side1-_Wheel03"),selection=cms.untracked.vstring("0x1fbc0000-0x188c0000")),
    cms.PSet(detSelection=cms.uint32(214),detLabel=cms.string("TID_Side1-_Wheel04"),selection=cms.untracked.vstring("0x1fbc0000-0x18900000")),
    cms.PSet(detSelection=cms.uint32(215),detLabel=cms.string("TID_Side1-_Wheel05"),selection=cms.untracked.vstring("0x1fbc0000-0x18940000")),
    ## Endcap : 15 side +   
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
    cms.PSet(detSelection=cms.uint32(261),detLabel=cms.string("TID_Side2+_Wheel01"),selection=cms.untracked.vstring("0x1fbc0000-0x19040000")),
    cms.PSet(detSelection=cms.uint32(262),detLabel=cms.string("TID_Side2+_Wheel02"),selection=cms.untracked.vstring("0x1fbc0000-0x19080000")),
    cms.PSet(detSelection=cms.uint32(263),detLabel=cms.string("TID_Side2+_Wheel03"),selection=cms.untracked.vstring("0x1fbc0000-0x190c0000")),
    cms.PSet(detSelection=cms.uint32(264),detLabel=cms.string("TID_Side2+_Wheel04"),selection=cms.untracked.vstring("0x1fbc0000-0x19100000")),
    cms.PSet(detSelection=cms.uint32(265),detLabel=cms.string("TID_Side2+_Wheel05"),selection=cms.untracked.vstring("0x1fbc0000-0x19140000"))

)
OccupancyPlotsPXBWantedSubDets = cms.VPSet (
    cms.PSet(detSelection=cms.uint32(1001),detLabel=cms.string("PXB_Layer1_Module1"),selection=cms.untracked.vstring("0x1ef00ffc-0x12100004")),
    cms.PSet(detSelection=cms.uint32(1002),detLabel=cms.string("PXB_Layer1_Module2"),selection=cms.untracked.vstring("0x1ef00ffc-0x12100008")),
    cms.PSet(detSelection=cms.uint32(1003),detLabel=cms.string("PXB_Layer1_Module3"),selection=cms.untracked.vstring("0x1ef00ffc-0x1210000c")),
    cms.PSet(detSelection=cms.uint32(1004),detLabel=cms.string("PXB_Layer1_Module4"),selection=cms.untracked.vstring("0x1ef00ffc-0x12100010")),
    cms.PSet(detSelection=cms.uint32(1005),detLabel=cms.string("PXB_Layer1_Module5"),selection=cms.untracked.vstring("0x1ef00ffc-0x12100014")),
    cms.PSet(detSelection=cms.uint32(1006),detLabel=cms.string("PXB_Layer1_Module6"),selection=cms.untracked.vstring("0x1ef00ffc-0x12100018")),
    cms.PSet(detSelection=cms.uint32(1007),detLabel=cms.string("PXB_Layer1_Module7"),selection=cms.untracked.vstring("0x1ef00ffc-0x1210001c")),
    cms.PSet(detSelection=cms.uint32(1008),detLabel=cms.string("PXB_Layer1_Module8"),selection=cms.untracked.vstring("0x1ef00ffc-0x12100020")),
    cms.PSet(detSelection=cms.uint32(1011),detLabel=cms.string("PXB_Layer2_Module1"),selection=cms.untracked.vstring("0x1ef00ffc-0x12200004")),
    cms.PSet(detSelection=cms.uint32(1012),detLabel=cms.string("PXB_Layer2_Module2"),selection=cms.untracked.vstring("0x1ef00ffc-0x12200008")),
    cms.PSet(detSelection=cms.uint32(1013),detLabel=cms.string("PXB_Layer2_Module3"),selection=cms.untracked.vstring("0x1ef00ffc-0x1220000c")),
    cms.PSet(detSelection=cms.uint32(1014),detLabel=cms.string("PXB_Layer2_Module4"),selection=cms.untracked.vstring("0x1ef00ffc-0x12200010")),
    cms.PSet(detSelection=cms.uint32(1015),detLabel=cms.string("PXB_Layer2_Module5"),selection=cms.untracked.vstring("0x1ef00ffc-0x12200014")),
    cms.PSet(detSelection=cms.uint32(1016),detLabel=cms.string("PXB_Layer2_Module6"),selection=cms.untracked.vstring("0x1ef00ffc-0x12200018")),
    cms.PSet(detSelection=cms.uint32(1017),detLabel=cms.string("PXB_Layer2_Module7"),selection=cms.untracked.vstring("0x1ef00ffc-0x1220001c")),
    cms.PSet(detSelection=cms.uint32(1018),detLabel=cms.string("PXB_Layer2_Module8"),selection=cms.untracked.vstring("0x1ef00ffc-0x12200020")),
    cms.PSet(detSelection=cms.uint32(1021),detLabel=cms.string("PXB_Layer3_Module1"),selection=cms.untracked.vstring("0x1ef00ffc-0x12300004")),
    cms.PSet(detSelection=cms.uint32(1022),detLabel=cms.string("PXB_Layer3_Module2"),selection=cms.untracked.vstring("0x1ef00ffc-0x12300008")),
    cms.PSet(detSelection=cms.uint32(1023),detLabel=cms.string("PXB_Layer3_Module3"),selection=cms.untracked.vstring("0x1ef00ffc-0x1230000c")),
    cms.PSet(detSelection=cms.uint32(1024),detLabel=cms.string("PXB_Layer3_Module4"),selection=cms.untracked.vstring("0x1ef00ffc-0x12300010")),
    cms.PSet(detSelection=cms.uint32(1025),detLabel=cms.string("PXB_Layer3_Module5"),selection=cms.untracked.vstring("0x1ef00ffc-0x12300014")),
    cms.PSet(detSelection=cms.uint32(1026),detLabel=cms.string("PXB_Layer3_Module6"),selection=cms.untracked.vstring("0x1ef00ffc-0x12300018")),
    cms.PSet(detSelection=cms.uint32(1027),detLabel=cms.string("PXB_Layer3_Module7"),selection=cms.untracked.vstring("0x1ef00ffc-0x1230001c")),
    cms.PSet(detSelection=cms.uint32(1028),detLabel=cms.string("PXB_Layer3_Module8"),selection=cms.untracked.vstring("0x1ef00ffc-0x12300020")),
    cms.PSet(detSelection=cms.uint32(1031),detLabel=cms.string("PXB_Layer4_Module1"),selection=cms.untracked.vstring("0x1ef00ffc-0x12400004")),
    cms.PSet(detSelection=cms.uint32(1032),detLabel=cms.string("PXB_Layer4_Module2"),selection=cms.untracked.vstring("0x1ef00ffc-0x12400008")),
    cms.PSet(detSelection=cms.uint32(1033),detLabel=cms.string("PXB_Layer4_Module3"),selection=cms.untracked.vstring("0x1ef00ffc-0x1240000c")),
    cms.PSet(detSelection=cms.uint32(1034),detLabel=cms.string("PXB_Layer4_Module4"),selection=cms.untracked.vstring("0x1ef00ffc-0x12400010")),
    cms.PSet(detSelection=cms.uint32(1035),detLabel=cms.string("PXB_Layer4_Module5"),selection=cms.untracked.vstring("0x1ef00ffc-0x12400014")),
    cms.PSet(detSelection=cms.uint32(1036),detLabel=cms.string("PXB_Layer4_Module6"),selection=cms.untracked.vstring("0x1ef00ffc-0x12400018")),
    cms.PSet(detSelection=cms.uint32(1037),detLabel=cms.string("PXB_Layer4_Module7"),selection=cms.untracked.vstring("0x1ef00ffc-0x1240001c")),
    cms.PSet(detSelection=cms.uint32(1038),detLabel=cms.string("PXB_Layer4_Module8"),selection=cms.untracked.vstring("0x1ef00ffc-0x12400020"))
)
OccupancyPlotsPXFmWantedSubDets = cms.VPSet (
    cms.PSet(detSelection = cms.uint32(1041),detLabel = cms.string("PXF_Side1-_Disk01_Blade01"),selection=cms.untracked.vstring("0x1fbff000-0x14841000")),
    cms.PSet(detSelection = cms.uint32(1042),detLabel = cms.string("PXF_Side1-_Disk01_Blade02"),selection=cms.untracked.vstring("0x1fbff000-0x14842000")),
#                                                                                                               "0x1fbff000-0x14843000",
#                                                                                                               "0x1fbff000-0x14844000",
#                                                                                                               "0x1fbff000-0x14845000",
#                                                                                                               "0x1fbff000-0x14846000",
#                                                                                                               "0x1fbff000-0x14847000",
#                                                                                                               "0x1fbff000-0x14848000",
#                                                                                                               "0x1fbff000-0x14849000",
#                                                                                                               "0x1fbff000-0x1484a000",
#                                                                                                               "0x1fbff000-0x1484b000",
#                                                                                                               "0x1fbff000-0x1484c000",
#                                                                                                               "0x1fbff000-0x1484d000",
#                                                                                                               "0x1fbff000-0x1484e000",
#                                                                                                               "0x1fbff000-0x1484f000",
#                                                                                                               "0x1fbff000-0x14850000",
#                                                                                                               "0x1fbff000-0x14851000",
#                                                                                                               "0x1fbff000-0x14852000",
#                                                                                                               "0x1fbff000-0x14853000",
#                                                                                                               "0x1fbff000-0x14854000",
#                                                                                                               "0x1fbff000-0x14855000",
#                                                                                                               "0x1fbff000-0x14856000")),
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
