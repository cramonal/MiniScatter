#include "analysis.hh"

#include "MyEdepHit.hh"
#include "MyTrackerHit.hh"

#include "G4SDManager.hh"

#include "G4Track.hh"

#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>


//#include "TDatabasePDG.h"

using namespace std;
analysis* analysis::singleton = 0;

void analysis::makeHistograms(){
  G4RunManager*     run= G4RunManager::GetRunManager();
  DetectorConstruction* detCon = (DetectorConstruction*)run->GetUserDetectorConstruction();
  G4String rootFileName = "plots/histo_" + 
    std::to_string(detCon->GetTargetThickness()/mm) + "mm_" +
    physListName + ".root";
  G4cout << "Opening ROOT file '" + rootFileName +"'"<<G4endl;
  histFile= new TFile(rootFileName,"RECREATE");
  
  targetEdep = new TH1D("targetEdep","targetEdep",1000,0,3);
  targetEdep->GetXaxis()->SetTitle("Total energy deposit/event [MeV]");
  targetEdep_NIEL = new TH1D("targetEdep_NIEL","targetEdep_NIEL",1000,0,1);
  targetEdep_NIEL->GetXaxis()->SetTitle("Total NIEL/event [MeV]");
  targetEdep_IEL = new TH1D("targetEdep_IEL","targetEdep_IEL",1000,0,3);
  targetEdep_IEL->GetXaxis()->SetTitle("Total ionizing energy deposit/event [MeV]");

  tracker_numParticles = new TH1D("numParticles","numParticles",1001,-0.5,1000.5);
  tracker_numParticles->GetXaxis()->SetTitle("Number of particles / event");
  tracker_angle        = new TH1D("angle","angle",1000,0,CLHEP::pi/3.0);
  tracker_angle->GetXaxis()->SetTitle("Angle of outgoing particles [rad]");
  tracker_energy       = new TH1D("energy","energy",10000,0,10.0);
  tracker_energy->GetXaxis()->SetTitle("Energy of outgoing particles [TeV]");
  
  //  tracker_energyflux_neutrals       = new TH1D("energyflux_neutrals","energyflux_neutrals",1000,0,CLHEP::pi/3.0);
  //  tracker_energyflux_neutrals->GetXaxis()->SetTitle("Energy of outgoing particles [TeV]");

  tracker_protonAngle        = new TH1D("protonAngle","protonAngle",1000,0,CLHEP::pi/3.0);
  tracker_protonAngle->GetXaxis()->SetTitle("Angle of outgoing protons [rad]");
  tracker_protonEnergy       = new TH1D("protonEnergy","protonEnergy",10000,0,10.0);
  tracker_protonEnergy->GetXaxis()->SetTitle("Energy of outgoing protons [TeV]");
}

void analysis::writePerEvent(const G4Event* event){
  
  G4HCofThisEvent* HCE=event->GetHCofThisEvent();
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  
  //**Data from targetEdepSD**
  G4int myTargetEdepSD_CollID = SDman->GetCollectionID("EdepCollection");
  if (myTargetEdepSD_CollID>=0){
    MyEdepHitsCollection* targetEdepHitsCollection = NULL;
    targetEdepHitsCollection = (MyEdepHitsCollection*) (HCE->GetHC(myTargetEdepSD_CollID));
    if (targetEdepHitsCollection != NULL) {
      G4int nEntries = targetEdepHitsCollection->entries();
      G4double edep      = 0.0;
      G4double edep_NIEL = 0.0;
      G4double edep_IEL  = 0.0;
      for (G4int i = 0; i < nEntries; i++){
	edep      += (*targetEdepHitsCollection)[i]->GetDepositedEnergy();
	edep_NIEL += (*targetEdepHitsCollection)[i]->GetDepositedEnergy_NIEL();
	edep_IEL  += (*targetEdepHitsCollection)[i]->GetDepositedEnergy() - 
	  (*targetEdepHitsCollection)[i]->GetDepositedEnergy_NIEL();
      }
      targetEdep->Fill(edep/MeV);
      targetEdep_NIEL->Fill(edep_NIEL/MeV);
      targetEdep_IEL->Fill(edep_IEL/MeV);
    }
    else{
      cout << "targetEdepHitsCollection was NULL!"<<endl;
    }
  }
  else{
    cout << "myTargetEdepSD_CollID was " << myTargetEdepSD_CollID << "<0!"<<endl;
  }

  //**Data from detectorTrackerSD**
  G4int myTrackerSD_CollID = SDman->GetCollectionID("TrackerCollection");
  if (myTargetEdepSD_CollID>=0){
    MyTrackerHitsCollection* trackerHitsCollection = NULL;
    trackerHitsCollection = (MyTrackerHitsCollection*) (HCE->GetHC(myTrackerSD_CollID));
    if (trackerHitsCollection != NULL) {
      G4int nEntries = trackerHitsCollection->entries();

      for (G4int i = 0; i < nEntries; i++){
	G4double energy = (*trackerHitsCollection)[i]->GetTrackEnergy();
	G4double angle = (*trackerHitsCollection)[i]->GetTrackAngle();
      	G4int PDG = (*trackerHitsCollection)[i]->GetPDG();

	//Overall histograms
	tracker_energy->Fill(energy/TeV);
	tracker_angle->Fill(angle);
	
	//Particle type counters
	if (tracker_particleTypes.count(PDG) == 0){
	  tracker_particleTypes[PDG] = 0;
	}
	tracker_particleTypes[PDG] += 1;

	//Per-particletype histograms
	if (PDG == 2212) { //Proton (p+)
	  tracker_protonEnergy->Fill(energy/TeV);
	  tracker_protonAngle->Fill(angle);
	}
      }
      
      tracker_numParticles->Fill(nEntries);

    }
    else{
      cout << "trackerHitsCollection was NULL!"<<endl;
    }
  }
  else{
    cout << "myTrackerSD_CollID was " << myTrackerSD_CollID << "<0!"<<endl;
  }

  
}
void analysis::writeHistograms(){
  targetEdep->Write();
  delete targetEdep; targetEdep = NULL;
  targetEdep_NIEL->Write();
  delete targetEdep_NIEL; targetEdep_NIEL = NULL;
  targetEdep_IEL->Write();
  delete targetEdep_IEL; targetEdep_IEL = NULL;

  tracker_numParticles->Write();
  delete tracker_numParticles; tracker_numParticles = NULL;
  tracker_angle->Write();
  delete tracker_angle; tracker_angle = NULL;
  tracker_energy->Write();
  delete tracker_energy; tracker_energy = NULL;

  tracker_protonAngle->Write();
  delete tracker_protonAngle; tracker_protonAngle = NULL;
  tracker_protonEnergy->Write();
  delete tracker_protonEnergy; tracker_protonEnergy = NULL;
  

  cout << "Got types at tracker:" << endl;
  for(std::map<G4int,G4int>::iterator it=tracker_particleTypes.begin(); it !=tracker_particleTypes.end(); it++){
    cout << it->first << " " << it->second << endl;
  }
  tracker_particleTypes.clear();

  histFile->Write();
  histFile->Close();
  delete histFile; histFile = NULL;
}

void analysis::SetMetadata(const G4String physListName_in){
  this->physListName = physListName_in;
}
