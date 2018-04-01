//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: exampleN03.cc,v 1.39 2010/12/01 05:56:17 allison Exp $
// GEANT4 tag $Name: geant4-09-04 $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"

#include "G4PhysListFactory.hh"

#include "RootFileWriter.hh"

#include "G4SystemOfUnits.hh"
#include "G4String.hh"
#include <string> //C++11 std::stoi

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <unistd.h> //getopt()
//#include <getopt.h> // Long options to getopt (GNU extension)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {
    //Parse command line arguments
    int getopt_char;
    G4double target_thick    = 1.0;           //Target thickness  [mm]
    G4String target_material = "G4_Cu";       //Name of target material to use

    G4double detector_distance   = 50.0;     //Detector distance at x=y=0  [mm]
    G4double detector_angle      = 0.0;       //Detectector angle around y-axis
    G4double detector_rotate     = false;

    G4double beam_energy = 200;      // Beam energy [MeV]
    G4String beam_type   = "proton"; // Beam particle type
    G4double beam_offset = 0.0;      // Beam offset (x) [mm]

    G4String physListName = "QGSP_FTFP_BERT"; // Name of physics list to use
    G4int    numEvents    = 0;                // Number of events to generate
    G4bool   useGUI       = false;            // GUI on/off
    G4String filename_out = "output";         // Outout filename

    while ( (getopt_char = getopt(argc,argv, "t:m:d:a:p:n:e:b:x:f:hg")) != -1) {
        switch(getopt_char) {
        case 'h': //Help
            G4cout << "Welcome to MiniScatter!" << G4endl
                   << G4endl
                   << "Usage/options:" << G4endl
                   << "-t <double> : Target thickness [mm],  default/current value = "
                   << target_thick << G4endl
                   << "-m <string> : Target material name,   default/current       = '"
                   << target_material << "'" << G4endl
                   << "-d <double> : Detector distance [mm], default/current value = "
                   << detector_distance << G4endl
                   << "-a <double> : Detector angle [deg],   default/current value = "
                   << detector_angle << G4endl
                   << "-p <string> : Physics list name,      default/current       = '"
                   << physListName << G4endl
                   << "-n <int>    : Run a given number of events automatically"
                   << G4endl
                   << "-e <double> : Beam energy [MeV],      default/current value = "
                   << beam_energy << G4endl
                   << "-b <string> : Particle type,          default/current value = "
                   << beam_type << G4endl
                   << "-x <double> : Beam offset (x) [mm],   default/current value = "
                   << beam_offset << G4endl
                   << "-g : Use a GUI"
                   << "-f <string> : Output filename,        default/current value = "
                   << filename_out << G4endl
                   << G4endl
                   << G4endl;
            G4cout << "Note that if both -g and -n is used, the events are ran before the GUI is opened." << G4endl;
            G4cout << "One may also use one or more arguments which does not include a '-n' -- these are forwarded untouched to Geant4" << G4endl;
            G4cout << "The first argument not in the form '-char' is interpreted as a macro to run. Don't use vis.mac, it will crash." << G4endl;
            G4cout << "Extra arguments are not compatible with -g" << G4endl;

            exit(1);
            break;

        case 'g': //Use GUI
            useGUI = true;
            break;

        case 't': //Target thickness
            try {
                target_thick = std::stod(string(optarg));
            }
            catch (const std::invalid_argument& ia) {
                G4cout << "Invalid argument when reading target thickness" << G4endl
                       << "Got: '" << optarg << "'" << G4endl
                       << "Expected a floating point number! (exponential notation is accepted)" << G4endl;
                exit(1);
            }
            break;

        case 'd': //Detector distance
            try {
                detector_distance = std::stod(string(optarg));
            }
            catch (const std::invalid_argument& ia) {
                G4cout << "Invalid argument when reading detector distance" << G4endl
                       << "Got: '" << optarg << "'" << G4endl
                       << "Expected a floating point number! (exponential notation is accepted)" << G4endl;
                exit(1);
            }
            break;

        case 'a': //Detector angle
            try {
                detector_angle = std::stod(string(optarg));
            }
            catch (const std::invalid_argument& ia) {
                G4cout << "Invalid argument when reading detector angle" << G4endl
                       << "Got: '" << optarg << "'" << G4endl
                       << "Expected a floating point number! (exponential notation is accepted)" << G4endl;
                exit(1);
            }
            detector_rotate = true;
            break;

        case 'p': //Named physics list
            physListName = G4String(optarg);
            break;

        case 'm': //Target material
            target_material = G4String(optarg);
            break;

        case 'n': //Number of events
            try {
                numEvents = std::stoi(string(optarg));
            }
            catch (const std::invalid_argument& ia) {
                G4cout << "Invalid argument when reading number of events" << G4endl
                       << "Got: '" << optarg << "'" << G4endl
                       << "Expected an integer!" << G4endl;
                exit(1);
            }
            break;

        case 'e': //beam energy
            try {
                beam_energy = std::stod(string(optarg));
            }
            catch (const std::invalid_argument& ia) {
                G4cout << "Invalid argument when reading beam energy" << G4endl
                       << "Got: '" << optarg << "'" << G4endl
                       << "Expected a floating point number! (exponential notation is accepted)" << G4endl;
                exit(1);
            }
            break;

        case 'b': //Beam type
            beam_type = G4String(optarg);
            break;

        case 'x': //beam offset (x)
            try {
                beam_offset = std::stod(string(optarg));
            }
            catch (const std::invalid_argument& ia) {
                G4cout << "Invalid argument when reading beam offset" << G4endl
                       << "Got: '" << optarg << "'" << G4endl
                       << "Expected a floating point number! (exponential notation is accepted)" << G4endl;
                exit(1);
            }
            break;

        case 'f': //Output filename
            filename_out = G4String(optarg);
            break;

        default: // WTF?
            G4cout << "Got an unknown getopt_char '" << char(getopt_char) << "' when parsing command line arguments." << G4endl;
            exit(1);
        }
    }
    //Copy remaining arguments to array that is passed to Geant4
    int argc_effective = argc-optind+1;
    char** argv_effective = new char*[argc_effective];
    argv_effective[0] = argv[0]; //First arg is always executable name
    for (int i = optind; i < argc;i++){
        argv_effective[i+1-optind] = argv[i];
    }

    //Print the gotten/default arguments
    G4cout << "MiniScatter got the following command line arguments:" << G4endl
           << "target_thick =  " << target_thick << " [mm]" << G4endl
           << "physListName = '" << physListName << "'" << G4endl
           << "numEvents    =  " << numEvents << G4endl
           << "useGUI       =  " << (useGUI==true ? "yes" : "no") << G4endl;
    G4cout << "Arguments which are passed on to Geant4:" << G4endl;
    for (int i = 0; i < argc_effective; i++) {
        G4cout << i << " '" << argv_effective[i] << "'" << G4endl;
    }

    G4cout << "Starting Geant4..." << G4endl << G4endl;

    G4RunManager * runManager = new G4RunManager;

    // Set mandatory initialization classes
    DetectorConstruction* detector = new DetectorConstruction(target_thick,
                                                              target_material,
                                                              detector_distance,
                                                              detector_angle,
                                                              detector_rotate
                                                              );
    runManager->SetUserInitialization(detector);

    G4int verbose=0;
    G4PhysListFactory plFactory;
    G4VModularPhysicsList* physlist = plFactory.GetReferencePhysList(physListName);
    if (physlist==NULL) {
        G4cerr << "Bad physics list!" << G4endl;
        G4cerr << G4endl;

        G4cerr << "Possiblities:" << G4endl;
        const std::vector<G4String>& listnames_hadr =  plFactory.AvailablePhysLists();
        for (auto l : listnames_hadr) {
            G4cerr << "'" << l << "'" << G4endl;
        }
        G4cerr << G4endl;

        G4cerr << "EM options:" << G4endl;
        const std::vector<G4String>& listnames_em =  plFactory.AvailablePhysListsEM();
        for (auto l : listnames_em) {
            G4cerr << "'" << l << "'" << G4endl;
        }
        G4cerr << G4endl;

        exit(1);
    }
    physlist->SetVerboseLevel(verbose);
    runManager->SetUserInitialization(physlist);
    //physlist->SetDefaultCutValue( 0.00001*mm);
    physlist->SetDefaultCutValue( 0.1*mm);

    // Set user action classes:
    //
    PrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction(detector, beam_energy, beam_type, beam_offset);
    runManager->SetUserAction(gen_action);
    //
    RunAction* run_action = new RunAction;
    runManager->SetUserAction(run_action);
    //
    EventAction* event_action = new EventAction(run_action);
    runManager->SetUserAction(event_action);

    // Initialize G4 kernel
    runManager->Initialize();

    //Set root file output filename
    RootFileWriter::GetInstance()->setFilename(filename_out);

#ifdef G4VIS_USE
    // Initialize visualization
    G4VisManager* visManager = new G4VisExecutive;
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();
#endif

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (argc_effective != 1) { // batch mode
        if (useGUI) {
            G4cout << "UseGUI is not compatible with batch mode!" << G4endl;
            exit(1);
        }
        G4String command = "/control/execute ";
        G4String fileName = argv_effective[1];
        UImanager->ApplyCommand(command+fileName);
    }
    else if (useGUI == true) {  // interactive mode : define UI session
#ifdef G4UI_USE
        G4UIExecutive* ui = new G4UIExecutive(argc_effective,argv_effective);
#ifdef G4VIS_USE
        UImanager->ApplyCommand("/control/execute vis.mac");
#endif
        if (numEvents > 0) {
            G4cout << G4String("'/run/beamOn ") + std::to_string(numEvents) << "'" << G4endl;
            UImanager->ApplyCommand(G4String("/run/beamOn ") + std::to_string(numEvents));
        }

        if (ui->IsGUI())
            ui->SessionStart(); //Returns when GUI is closed.
        delete ui;
#endif
    }

    //Run given number of events
    if (useGUI==false and numEvents > 0) {
        G4cout << G4String("'/run/beamOn ") + std::to_string(numEvents) << "'" << G4endl;
        UImanager->ApplyCommand(G4String("/run/beamOn ") + std::to_string(numEvents));
    }

    // Job termination
    // Free the store: user actions, physics_list and detector_description are
    //                 owned and deleted by the run manager, so they should not
    //                 be deleted in the main() program !
#ifdef G4VIS_USE
    delete visManager;
#endif
    //End comment
    // Delete runManager
    delete runManager;
    G4cout << "runManager deleted" << G4endl;

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
