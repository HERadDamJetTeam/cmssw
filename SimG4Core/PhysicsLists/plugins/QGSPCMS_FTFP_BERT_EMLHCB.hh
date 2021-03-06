#ifndef SimG4Core_PhysicsLists_QGSPCMS_FTFP_BERT_EMLHCB_H
#define SimG4Core_PhysicsLists_QGSPCMS_FTFP_BERT_EMLHCB_H
 
#include "SimG4Core/Physics/interface/PhysicsList.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
 
class QGSPCMS_FTFP_BERT_EMLHCB: public PhysicsList {

public:
  QGSPCMS_FTFP_BERT_EMLHCB(G4LogicalVolumeToDDLogicalPartMap& map, const HepPDT::ParticleDataTable * table_, sim::FieldBuilder *fieldBuilder_, const edm::ParameterSet & p);
};

#endif


