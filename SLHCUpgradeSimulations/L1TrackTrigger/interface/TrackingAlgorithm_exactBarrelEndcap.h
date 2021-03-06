/// ////////////////////////////////////////
/// Stacked Tracker Simulations          ///
///                                      ///
/// Anders Ryd, Cornell                  ///
/// Emmanuele Salvati, Cornell           ///
/// Nicola Pozzobon,  UNIPD              ///
///                                      ///
/// 2012                                 ///
/// 2013, January                        ///
/// ////////////////////////////////////////

#ifndef TRACKING_ALGO_EXACTBE_H
#define TRACKING_ALGO_EXACTBE_H

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include "SLHCUpgradeSimulations/L1TrackTrigger/interface/TrackingAlgorithm.h"
#include "SLHCUpgradeSimulations/L1TrackTrigger/interface/TrackingAlgorithmRecord.h"

#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementPoint.h"
#include "Geometry/CommonTopologies/interface/Topology.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include <boost/shared_ptr.hpp>
#include <memory>
#include <string>
#include <map>

/** ************************ **/
/**                          **/
/**   DECLARATION OF CLASS   **/
/**                          **/
/** ************************ **/

template< typename T >
class TrackingAlgorithm_exactBarrelEndcap : public TrackingAlgorithm< T >
{
  private :
    /// Data members
    double                       mMagneticField;
    unsigned int                 nSectors;
    unsigned int                 nWedges;

    std::vector< std::vector< double > > tableRPhiBB;
    std::vector< std::vector< double > > tableZBB;
    std::vector< std::vector< double > > tableRPhiBE;
    std::vector< std::vector< double > > tableZBE;
    std::vector< std::vector< double > > tableRPhiEB;
    std::vector< std::vector< double > > tableZEB;
    std::vector< std::vector< double > > tableRPhiEE;
    std::vector< std::vector< double > > tableZEE;

  public:
    /// Constructors
    TrackingAlgorithm_exactBarrelEndcap( const StackedTrackerGeometry *aStackedGeom,
                                         double aMagneticField, unsigned int aSectors, unsigned int aWedges,
                                         std::vector< std::vector< double > > aTableRPhiBB,
                                         std::vector< std::vector< double > > aTableZBB,
                                         std::vector< std::vector< double > > aTableRPhiBE,
                                         std::vector< std::vector< double > > aTableZBE,
                                         std::vector< std::vector< double > > aTableRPhiEB,
                                         std::vector< std::vector< double > > aTableZEB,
                                         std::vector< std::vector< double > > aTableRPhiEE,
                                         std::vector< std::vector< double > > aTableZEE )
      : TrackingAlgorithm< T > ( aStackedGeom, __func__ )
    {
      mMagneticField = aMagneticField;
      nSectors = aSectors;
      nWedges = aWedges;

      tableRPhiBB = aTableRPhiBB;
      tableZBB = aTableZBB;
      tableRPhiBE = aTableRPhiBE;
      tableZBE = aTableZBE;
      tableRPhiEB = aTableRPhiEB;
      tableZEB = aTableZEB;
      tableRPhiEE = aTableRPhiEE;
      tableZEE = aTableZEE;
    }

    /// Destructor
    ~TrackingAlgorithm_exactBarrelEndcap(){}

    /// Seed creation
    void CreateSeeds( std::vector< L1TkTrack< T > > &output,
                      std::map< std::pair< unsigned int, unsigned int >, std::vector< edm::Ptr< L1TkStub< T > > > > *outputSectorMap,
                      edm::Handle< std::vector< L1TkStub< T > > > &input ) const;

    /// Match a Stub to a Seed/Track
    void AttachStubToSeed( L1TkTrack< T > &seed, edm::Ptr< L1TkStub< T > > &candidate ) const;

    /// Return the number of Sectors
    unsigned int ReturnNumberOfSectors() const { return nSectors; } /// Phi
    unsigned int ReturnNumberOfWedges() const { return nWedges; } /// Eta

    /// Return the value of the magnetic field
    double ReturnMagneticField() const { return mMagneticField; }

    /// Fit the Track
    /// Take it from the parent class

}; /// Close class

/** ***************************** **/
/**                               **/
/**   IMPLEMENTATION OF METHODS   **/
/**                               **/
/** ***************************** **/

template< typename T >
void TrackingAlgorithm_exactBarrelEndcap< T >::CreateSeeds( std::vector< L1TkTrack< T > > &output,
                                                            std::map< std::pair< unsigned int, unsigned int >, std::vector< edm::Ptr< L1TkStub< T > > > > *outputSectorMap,
                                                            edm::Handle< std::vector< L1TkStub< T > > > &input ) const
{
  /// Prepare output
  output.clear();

  /// Map the Barrel Stubs per Sector/Wedge
  std::map< std::pair< unsigned int, unsigned int >, std::vector< edm::Ptr< L1TkStub< T > > > > stubBarrelMap;  stubBarrelMap.clear();
  std::map< std::pair< unsigned int, unsigned int >, std::vector< edm::Ptr< L1TkStub< T > > > > stubEndcapMap;  stubEndcapMap.clear();

  /// Map the Barrel Stubs per sector
  outputSectorMap->clear();

  typename std::vector< L1TkStub< T > >::const_iterator inputIter;
  unsigned int j = 0; /// Counter needed to build the edm::Ptr to the L1TkStub

  for ( inputIter = input->begin();
        inputIter != input->end();
        ++inputIter )
  {
    /// Make the pointer to be put in the map and, later on, in the Track
    edm::Ptr< L1TkStub< T > > tempStubPtr( input, j++ );

    /// Calculate Sector
    /// From 0 to nSectors-1
    /// Sector 0 centered on Phi = 0 and symmetric around it
    double stubPhi = TrackingAlgorithm< T >::theStackedTracker->findGlobalPosition( tempStubPtr.get() ).phi();
    stubPhi += M_PI/nSectors;
    if ( stubPhi < 0 )
    {
      stubPhi += 2*M_PI;
    }
    unsigned int thisSector = floor( 0.5*stubPhi*nSectors/M_PI );
    /// Calculate Wedge
    /// From 0 to nWedges-1
    /// 
    double stubEta = TrackingAlgorithm< T >::theStackedTracker->findGlobalPosition( tempStubPtr.get() ).eta();
    stubEta += 2.5; /// bring eta = -2.5 to 0
    //stubEta += 2.5/nWedges;

    /// Accept only stubs within -2.5, 2.5 range
    if ( stubEta < 0.0 || stubEta > 5.0 )
      continue;

    unsigned int thisWedge = floor( stubEta*nWedges/5.0 );

    /// Build the key to the map (by Sector / Wedge)
    std::pair< unsigned int, unsigned int > mapkey = std::make_pair( thisSector, thisWedge );

    /// If an entry already exists for this Sector/Wedge, just add the stub
    /// to the vector, otherwise create the entry
    if ( outputSectorMap->find( mapkey ) == outputSectorMap->end() )
    {
      /// New entry
      std::vector< edm::Ptr< L1TkStub< T > > > tempStubVec;
      tempStubVec.clear();
      tempStubVec.push_back( tempStubPtr );
      outputSectorMap->insert( std::pair< std::pair< unsigned int, unsigned int>, std::vector< edm::Ptr< L1TkStub< T > > > > ( mapkey, tempStubVec ) );
    }
    else
    {
      /// Already existing entry
      outputSectorMap->find( mapkey )->second.push_back( tempStubPtr );
    }

    /// Do the same but separating Barrel and Endcap Stubs
    /// NOTE this is internal to build seeds
    /// The previous one goes into the output
    StackedTrackerDetId detIdStub( inputIter->getDetId() );
    if ( detIdStub.isBarrel() )
    {
      /// If an entry already exists for this key, just add the stub
      /// to the vector, otherwise create the entry
      if ( stubBarrelMap.find( mapkey ) == stubBarrelMap.end() )
      {
        /// New entry
        std::vector< edm::Ptr< L1TkStub< T > > > tempStubVec;
        tempStubVec.clear();
        tempStubVec.push_back( tempStubPtr );
        stubBarrelMap.insert( std::pair< std::pair< unsigned int, unsigned int >, std::vector< edm::Ptr< L1TkStub< T > > > > ( mapkey, tempStubVec ) );
      }
      else
      {
        /// Already existing entry
        stubBarrelMap[mapkey].push_back( tempStubPtr );
      }
    }
    else if ( detIdStub.isEndcap() )
    {
      /// If an entry already exists for this key, just add the stub
      /// to the vector, otherwise create the entry
      if ( stubEndcapMap.find( mapkey ) == stubEndcapMap.end() )
      {
        /// New entry
        std::vector< edm::Ptr< L1TkStub< T > > > tempStubVec;
        tempStubVec.clear();
        tempStubVec.push_back( tempStubPtr );
        stubEndcapMap.insert( std::pair< std::pair< unsigned int, unsigned int >, std::vector< edm::Ptr< L1TkStub< T > > > > ( mapkey, tempStubVec ) );
      }
      else
      {
        /// Already existing entry
        stubEndcapMap[mapkey].push_back( tempStubPtr );
      }
    }
  } /// End of loop over input stubs

  /// Loop over the map
  /// Create Seeds in Barrel
  typename std::map< std::pair< unsigned int, unsigned int >, std::vector< edm::Ptr< L1TkStub< T > > > >::const_iterator mapIter;
  for ( mapIter = stubBarrelMap.begin();
        mapIter != stubBarrelMap.end();
        ++mapIter )
  {
    /// Here we have ALL the stubs in one single Sector/Wedge in the Barrel
    std::vector< edm::Ptr< L1TkStub< T > > > tempStubVec = mapIter->second;

    for ( unsigned int i = 0; i < tempStubVec.size(); i++ )
    {
      StackedTrackerDetId detId1( tempStubVec.at(i)->getDetId() );
      GlobalPoint pos1 = TrackingAlgorithm< T >::theStackedTracker->findAverageGlobalPosition( tempStubVec.at(i)->getClusterPtr(0).get() );

      for ( unsigned int k = i+1; k < tempStubVec.size(); k++ )
      {
        StackedTrackerDetId detId2( tempStubVec.at(k)->getDetId() );
        GlobalPoint pos2 = TrackingAlgorithm< T >::theStackedTracker->findAverageGlobalPosition( tempStubVec.at(k)->getClusterPtr(0).get() );

        /// Skip same layer pairs
        /// Skip pairs with distance larger than 1 layer
        if (( detId2.iLayer() != detId1.iLayer() + 1 ) && ( detId1.iLayer() != detId2.iLayer() + 1 ))
          continue;

        /// In the Barrel
        /// 1/RCurv = 0.003 * B * 1/Pt

        /// Perform standard trigonometric operations
        double deltaPhi = pos1.phi() - pos2.phi();
        if ( fabs(deltaPhi) >= M_PI )
        {
          if ( deltaPhi>0 )
            deltaPhi = deltaPhi - 2*M_PI;
          else
            deltaPhi = 2*M_PI + deltaPhi;
        }

        double distance = sqrt( pos2.perp2() + pos1.perp2() - 2*pos2.perp()*pos1.perp()*cos(deltaPhi) );
        double rInvOver2 = sin(deltaPhi)/distance; /// Sign is maintained to keep track of the charge

        /// Perform cut on Pt
        if ( fabs(rInvOver2) > mMagneticField*0.0015*0.5 ) continue;

        /// Calculate projected vertex
        /// NOTE: cotTheta0 = Pz/Pt
        double rhoPsi1 = asin( pos1.perp()*rInvOver2 )/rInvOver2;
        double rhoPsi2 = asin( pos2.perp()*rInvOver2 )/rInvOver2;
        double cotTheta0 = ( pos1.z() - pos2.z() ) / ( rhoPsi1 - rhoPsi2 );
        double z0 = pos2.z() - rhoPsi2 * cotTheta0;

        /// Perform projected vertex cut
        if ( fabs(z0) > 30.0 ) continue;

        /// Calculate direction at vertex
        double phi0 = pos2.phi() + asin( pos2.perp() * rInvOver2 );

        /// Calculate Pt
        double roughPt = fabs( mMagneticField*0.0015 / rInvOver2 );

        /// Create the Seed in the form of a Track and store it in the output
        std::vector< edm::Ptr< L1TkStub< T > > > tempVec;
        tempVec.push_back( tempStubVec.at(i) );
        tempVec.push_back( tempStubVec.at(k) );
        L1TkTrack< T > tempTrack( tempVec );
        tempTrack.setRInv( 2*rInvOver2 );
        tempTrack.setMomentum( GlobalVector( roughPt*cos(phi0),
                                             roughPt*sin(phi0),
                                             roughPt*cotTheta0 ) );
        tempTrack.setVertex( GlobalPoint( 0, 0, z0 ) );
        tempTrack.setSector( mapIter->first.first );
        tempTrack.setWedge( mapIter->first.second );
        output.push_back( tempTrack );
      }
    } /// End of double loop over pairs of stubs
  } /// End of loop over map elements

  /// Loop over the map
  /// Create Seeds in Endcap
  for ( mapIter = stubEndcapMap.begin();
        mapIter != stubEndcapMap.end();
        ++mapIter )
  {
    /// Here we have ALL the stubs in one single Sector/Wedge in the Endcap
    std::vector< edm::Ptr< L1TkStub< T > > > tempStubVec = mapIter->second;

    for ( unsigned int i = 0; i < tempStubVec.size(); i++ )
    {
      StackedTrackerDetId detId1( tempStubVec.at(i)->getDetId() );
      GlobalPoint pos1 = TrackingAlgorithm< T >::theStackedTracker->findAverageGlobalPosition( tempStubVec.at(i)->getClusterPtr(0).get() );

      for ( unsigned int k = i+1; k < tempStubVec.size(); k++ )
      {
        StackedTrackerDetId detId2( tempStubVec.at(k)->getDetId() );
        GlobalPoint pos2 = TrackingAlgorithm< T >::theStackedTracker->findAverageGlobalPosition( tempStubVec.at(k)->getClusterPtr(0).get() );

        /// Skip same disk pairs
        /// Skip pairs with distance larger than 1 disk
        //if ( detId1.iDisk() == detId2.iDisk() ) continue;
        if (( detId2.iDisk() != detId1.iDisk() + 1 ) && ( detId1.iDisk() != detId2.iDisk() + 1 ))
          continue;

        /// In the Endcap
        /// DPhi/Dz = 0.003 / 2 * B * 1/Pz
        /// ANYWAY find seed as in the Barrel!

        /// Perform standard trigonometric operations
        double deltaPhi = pos1.phi() - pos2.phi();
        if ( fabs(deltaPhi) >= M_PI )
        {
          if ( deltaPhi>0 )
            deltaPhi = deltaPhi - 2*M_PI;
          else
            deltaPhi = 2*M_PI + deltaPhi;
        }

//        double PzInv = fabs(deltaPhi)/( pos2.z() - pos1.z() ) * 1/( 0.0015*mMagneticField );

        double distance = sqrt( pos2.perp2() + pos1.perp2() - 2*pos2.perp()*pos1.perp()*cos(deltaPhi) );
        double rInvOver2 = sin(deltaPhi)/distance; /// Sign is maintained to keep track of the charge

        /// Perform cut on Pt
        if ( fabs(rInvOver2) > mMagneticField*0.0015*0.5 ) continue;

        /// Calculate projected vertex
        /// NOTE: cotTheta0 = Pz/Pt
        double rhoPsi1 = asin( pos1.perp()*rInvOver2 )/rInvOver2;
        double rhoPsi2 = asin( pos2.perp()*rInvOver2 )/rInvOver2;
        double cotTheta0 = ( pos1.z() - pos2.z() ) / ( rhoPsi1 - rhoPsi2 );
        double z0 = pos2.z() - rhoPsi2 * cotTheta0;

        /// Perform projected vertex cut
        if ( fabs(z0) > 30.0 ) continue;

        /// Calculate direction at vertex
        double phi0 = pos2.phi() + asin( pos2.perp() * rInvOver2 );

        /// Calculate Pt
        double roughPt = fabs( mMagneticField*0.0015 / rInvOver2 );

        /// Create the Seed in the form of a Track and store it in the output
        std::vector< edm::Ptr< L1TkStub< T > > > tempVec;
        tempVec.push_back( tempStubVec.at(i) );
        tempVec.push_back( tempStubVec.at(k) );
        L1TkTrack< T > tempTrack( tempVec );
        tempTrack.setRInv( 2*rInvOver2 );
        tempTrack.setMomentum( GlobalVector( roughPt*cos(phi0),
                                             roughPt*sin(phi0),
                                             roughPt*cotTheta0 ) );
        tempTrack.setVertex( GlobalPoint( 0, 0, z0 ) );
        tempTrack.setSector( mapIter->first.first );
        tempTrack.setWedge( mapIter->first.second );
        output.push_back( tempTrack );
      }
    } /// End of double loop over pairs of stubs
  } /// End of loop over map elements
}

/// Match a Stub to a Seed/Track
template< typename T >
void TrackingAlgorithm_exactBarrelEndcap< T >::AttachStubToSeed( L1TkTrack< T > &seed, edm::Ptr< L1TkStub< T > > &candidate ) const
{
  /// Get the track Stubs
  std::vector< edm::Ptr< L1TkStub< T > > > theStubs = seed.getStubPtrs();

  /// Check that the candidate is NOT the one under examination
  for ( unsigned int i = 0; i < theStubs.size(); i++ )
  {
    if ( theStubs.at(i) == candidate )
      return;
  }

  /// Skip if the stub is in one of the seed layers/disks  
  StackedTrackerDetId stDetId0( theStubs.at(0)->getDetId() );
  StackedTrackerDetId stDetId1( theStubs.at(1)->getDetId() );
  StackedTrackerDetId stDetIdCand( candidate->getDetId() );

  if ( stDetId0.isBarrel() && stDetIdCand.isBarrel() )
  {
    if ( stDetId0.iLayer() == stDetIdCand.iLayer() || stDetId1.iLayer() == stDetIdCand.iLayer() )
      return;
  }
  else if ( stDetId0.isEndcap() && stDetIdCand.isEndcap() )
  {
    if ( stDetId0.iSide() && stDetIdCand.iSide() )
    {
      if ( stDetId0.iDisk() == stDetIdCand.iDisk() || stDetId1.iDisk() == stDetIdCand.iDisk() )
        return;
    }
  }

  /// Here we have either Barrel-Barrel with different Layer,
  /// either Endcap-Endcap with different Side/Disk,
  /// either Barrel-Endcap or Endcap-Barrel

  /// Get the track momentum and propagate it
  GlobalVector curMomentum = seed.getMomentum();
  GlobalPoint curVertex = seed.getVertex();
  double curRInv = seed.getRInv();

  /// Get the candidate Stub position
  GlobalPoint candPos = TrackingAlgorithm< T >::theStackedTracker->findGlobalPosition( candidate.get() );

  /// Propagate seed to Barrel candidate
  if ( stDetIdCand.isBarrel() )
  {
    double propPsi = asin( candPos.perp() * 0.5 * curRInv );
    double propPhi = curMomentum.phi() - propPsi;
    double propRhoPsi = 2 * propPsi / curRInv;
    double propZ = curVertex.z() + propRhoPsi * tan( M_PI_2 - curMomentum.theta() );

    /// Calculate displacement
    double deltaPhi = candPos.phi() - propPhi;
    if ( fabs(deltaPhi) >= M_PI )
    {
      if ( deltaPhi>0 )
        deltaPhi = deltaPhi - 2*M_PI;
      else
        deltaPhi = 2*M_PI + deltaPhi;
    }
    double deltaRPhi = fabs( deltaPhi * candPos.perp() );
    double deltaZ = fabs( propZ - candPos.z() );


    /// First get the vector corresponding to the seed SL
    /// Then get in this vector the entry corresponding to the targer SL

    if ( stDetId0.isBarrel() )
    {
      if ( deltaRPhi < (tableRPhiBB.at(stDetId0.iLayer())).at(stDetIdCand.iLayer()) &&
           deltaZ < (tableZBB.at(stDetId0.iLayer())).at(stDetIdCand.iLayer()) )
      {
        seed.addStubPtr( candidate );
      }
    }
    else if ( stDetId0.isEndcap() )
    {
      if ( deltaRPhi < (tableRPhiEB.at(stDetId0.iDisk())).at(stDetIdCand.iLayer()) && 
           deltaZ < (tableZEB.at(stDetId0.iDisk())).at(stDetIdCand.iLayer()) )
      {
        seed.addStubPtr( candidate );
      }
    }
/*
    if ( deltaRPhi < 5 && deltaZ < 8 )
    {
      seed.addStubPtr( candidate );
    }
*/
  }
  /// Propagate to Endcap candidate
  else if ( stDetIdCand.isEndcap() )
  {
    double propPsi = 0.5*( candPos.z() - curVertex.z() ) * curRInv / tan( M_PI_2 - curMomentum.theta() );
    double propPhi = curMomentum.phi() - propPsi; 
    double propRho = 2 * sin( propPsi ) / curRInv;
    double deltaPhi = candPos.phi() - propPhi;

    /// Calculate displacement
    if ( fabs(deltaPhi) >= M_PI )
    {
      if ( deltaPhi>0 )
        deltaPhi = deltaPhi - 2*M_PI;
      else
        deltaPhi = 2*M_PI + deltaPhi;
    }
    double deltaRPhi = fabs( deltaPhi * candPos.perp() );
    double deltaR = fabs( candPos.perp() - propRho );

    if ( stDetId0.isBarrel() )
    {
      if ( deltaRPhi < (tableRPhiBE.at(stDetId0.iLayer())).at(stDetIdCand.iDisk()) && 
           deltaR < (tableZBE.at(stDetId0.iLayer())).at(stDetIdCand.iDisk()) )
      {
        seed.addStubPtr( candidate );
      }
    }
    else if ( stDetId0.isEndcap() )
    {
      if ( deltaRPhi < (tableRPhiEE.at(stDetId0.iDisk())).at(stDetIdCand.iDisk()) && 
           deltaR < (tableZEE.at(stDetId0.iDisk())).at(stDetIdCand.iDisk()) )
      {
        seed.addStubPtr( candidate );
      }
    }
/*
    if ( deltaRPhi < 5 && deltaR < 8 )
    {
      seed.addStubPtr( candidate );
    }
*/

  }
}

/** ********************** **/
/**                        **/
/**   DECLARATION OF THE   **/
/**    ALGORITHM TO THE    **/
/**       FRAMEWORK        **/
/**                        **/
/** ********************** **/

template< typename T >
class ES_TrackingAlgorithm_exactBarrelEndcap : public edm::ESProducer
{
  private:
    /// Data members
    boost::shared_ptr< TrackingAlgorithm< T > > _theAlgo;

    /// Number of Sectors
    unsigned int  mSectors;
    unsigned int  mWedges;

    /// projection windows
    std::vector< std::vector< double > > setRhoPhiWinBB;
    std::vector< std::vector< double > > setZWinBB;
    std::vector< std::vector< double > > setRhoPhiWinBE;
    std::vector< std::vector< double > > setZWinBE;
    std::vector< std::vector< double > > setRhoPhiWinEB;
    std::vector< std::vector< double > > setZWinEB;
    std::vector< std::vector< double > > setRhoPhiWinEE;
    std::vector< std::vector< double > > setZWinEE;

  public:
    /// Constructor
    ES_TrackingAlgorithm_exactBarrelEndcap( const edm::ParameterSet & p )
      : mSectors( p.getParameter< int >("NumSectors") ), mWedges( p.getParameter< int >("NumWedges") )
    {
      std::vector< edm::ParameterSet > vPSet;
      std::vector< edm::ParameterSet >::const_iterator iPSet;

      vPSet = p.getParameter< std::vector< edm::ParameterSet > >("ProjectionWindowsBarrelBarrel");
      for ( iPSet = vPSet.begin(); iPSet != vPSet.end(); iPSet++ )
      {
        setRhoPhiWinBB.push_back( iPSet->getParameter< std::vector< double > >("RhoPhiWin") );
        setZWinBB.push_back( iPSet->getParameter< std::vector< double > >("ZWin") );
      }

      vPSet = p.getParameter< std::vector< edm::ParameterSet > >("ProjectionWindowsBarrelEndcap");
      for ( iPSet = vPSet.begin(); iPSet != vPSet.end(); iPSet++ )
      {
        setRhoPhiWinBE.push_back( iPSet->getParameter< std::vector< double > >("RhoPhiWin") );
        setZWinBE.push_back( iPSet->getParameter< std::vector< double > >("ZWin") );
      }

      vPSet = p.getParameter< std::vector< edm::ParameterSet > >("ProjectionWindowsEndcapBarrel");
      for ( iPSet = vPSet.begin(); iPSet != vPSet.end(); iPSet++ )
      {
        setRhoPhiWinEB.push_back( iPSet->getParameter< std::vector< double > >("RhoPhiWin") );
        setZWinEB.push_back( iPSet->getParameter< std::vector< double > >("ZWin") );
      }

      vPSet = p.getParameter< std::vector< edm::ParameterSet > >("ProjectionWindowsEndcapEndcap");
      for ( iPSet = vPSet.begin(); iPSet != vPSet.end(); iPSet++ )
      {
        setRhoPhiWinEE.push_back( iPSet->getParameter< std::vector< double > >("RhoPhiWin") );
        setZWinEE.push_back( iPSet->getParameter< std::vector< double > >("ZWin") );
      }

      setWhatProduced( this );
    }

    /// Destructor
    virtual ~ES_TrackingAlgorithm_exactBarrelEndcap() {}

    /// Implement the producer
    boost::shared_ptr< TrackingAlgorithm< T > > produce( const TrackingAlgorithmRecord & record )
    {
      /// Get magnetic field
      edm::ESHandle< MagneticField > magnet;
      record.getRecord< IdealMagneticFieldRecord >().get(magnet);
      double mMagneticFieldStrength = magnet->inTesla(GlobalPoint(0,0,0)).z();
      double mMagneticFieldRounded = (floor(mMagneticFieldStrength*10.0 + 0.5))/10.0;

      edm::ESHandle< StackedTrackerGeometry > StackedTrackerGeomHandle;
      record.getRecord< StackedTrackerGeometryRecord >().get( StackedTrackerGeomHandle );

      TrackingAlgorithm< T >* TrackingAlgo =
        new TrackingAlgorithm_exactBarrelEndcap< T >( &(*StackedTrackerGeomHandle),
                                                    mMagneticFieldRounded, mSectors, mWedges,
                                                    setRhoPhiWinBB, setZWinBB,
                                                    setRhoPhiWinBE, setZWinBE,
                                                    setRhoPhiWinEB, setZWinEB,
                                                    setRhoPhiWinEE, setZWinEE );

      _theAlgo = boost::shared_ptr< TrackingAlgorithm< T > >( TrackingAlgo );
      return _theAlgo;
    }

};

#endif

