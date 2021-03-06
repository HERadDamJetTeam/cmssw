import FWCore.ParameterSet.Config as cms

#
# This cfi is should be included to build the GlobalTrackingGeometry with all 
# concrete TrackingGeometries for muon and tracker.
#
from Geometry.CSCGeometryBuilder.cscGeometryDB_cfi import *
from Geometry.RPCGeometryBuilder.rpcGeometryDB_cfi import *
from Geometry.DTGeometryBuilder.dtGeometryDB_cfi import *

from Geometry.TrackerNumberingBuilder.trackerNumberingSLHCGeometryDB_cfi import *
from Geometry.TrackerGeometryBuilder.trackerSLHCGeometryDB_cfi import *
from Geometry.CommonDetUnit.bareGlobalTrackingGeometry_cfi import *

