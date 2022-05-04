// True distribution of neutrino-electron angle
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TVector3.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "larcore/CoreUtils/ServiceUtil.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"
#include "nusimdata/SimulationBase/MCTruth.h"
/* #include "larcore/Geometry/Geometry.h" */
/* #include "larcorealg/Geometry/GeometryCore.h" */
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include "TColor.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH2F.h"
#include "TInterpreter.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TStyle.h"
#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

using namespace art;
using namespace std;
// write everything in a non-LArSoft way

TTree* get_rawdigits(vector<string> filelist, size_t nevents=-1) {
  size_t counter = 0;
  auto RawDigitLabel = InputTag("daq");
  auto SimulationLabel = InputTag("NuEScatter");
  /* auto geom = art::ServiceHandle<geo::Geometry>(); */
  // gInterpreter->GenerateDictionary("vector<vector<short>>", "vector");
  TFile * output = new TFile("output.root", "recreate") ;
  TTree* tree = new TTree("tree", "tree");
  int digit_map_size = 2560*12*4492;
  vector<short> digit_map(digit_map_size, 0);
  TVector3 nu_direction;
  TVector3 e_direction;
  TVector3 e_position;
  Double_t nu_energy;
  Double_t e_energy;
  tree->Branch("digit_map", &digit_map);
  tree->Branch("nu_direction", &nu_direction);
  tree->Branch("e_direction", &e_direction);
  tree->Branch("e_position", &e_position);
  tree->Branch("nu_energy", &nu_energy);
  tree->Branch("e_energy", &e_energy);
  for (gallery::Event ev(filelist); !ev.atEnd(); ev.next()) {
    
    if(counter == nevents) break;
    counter++;
    cout<< "Event No. " <<  counter << endl;
    auto const digit_handle =
        ev.getValidHandle<vector<raw::RawDigit>>(RawDigitLabel);
    for (auto digit : *digit_handle) {
      std::vector<short> signal(digit.Samples());
      raw::Uncompress(digit.ADCs(), signal, digit.Compression());
      auto channel_id = digit.Channel();
      // digit_map.at(channel_id) = signal;
      // digit_map.insert(digit_map.end(), signal.begin(), signal.end());
      for(size_t i = 0; i < signal.size(); i++){
        digit_map.at(channel_id * 4492 + i) = signal.at(i);
      }
      /* auto view = geom->View(channel_id); */
      // cout << channel_id << "\t" << signal.size() << endl;
    }
    auto const& truth_blocks =
        ev.getValidHandle<vector<simb::MCTruth>>(SimulationLabel);
    for (auto truth_block : *truth_blocks) {
      // cout << "\tBlock contains " << truth_block.NParticles()
      //      << " particles" << endl;
      for (int i = 0; i < truth_block.NParticles(); i++) {
        auto part = truth_block.GetParticle(i);
        // cout << "\t\t" << part.PdgCode() << endl;
        // electron
        if (part.PdgCode() == 11) {
          e_direction.SetXYZ(part.Px(), part.Py(), part.Pz());
          e_direction.SetMag(1.0);
          e_energy = part.E() * 1000;
          e_position.SetXYZ(part.Position().X(), part.Position().Y(), part.Position().Z());
          continue;
        }
        // neutrino
        if ((abs(part.PdgCode()) == 12 || abs(part.PdgCode()) == 14 ||
             abs(part.PdgCode()) == 16)) {
          nu_direction.SetXYZ(part.Px(), part.Py(), part.Pz());
          nu_direction.SetMag(1.0);
          nu_energy = part.E() * 1000;
          continue;
        }
      }
    }
    tree->Fill();
  }  // end event loop
  output->cd();
  tree->Write();
  return tree;
}

TTree* get_rawdigits() {
  vector<string> filelist;
  filelist.push_back("/work/data/scratch/detsim_27483971_1.root");
  TTree* tree = get_rawdigits(filelist);
  return tree;
}

/**
 * By reading a file that has the name of the root files in them
 * */
TTree* get_rawdigits_filelist(string filepath) {
  std::ifstream in(filepath);
  vector<string> filelist;
  string str;
  while (getline(in, str)) {
    if (str.size() > 0) filelist.push_back(str);
  }
  cout << "Beginning Parsing" << endl;
  TTree* tree = get_rawdigits(filelist);
  return tree;
}


TTree* get_rawdigits_single_file(string filename, size_t nevents=-1){
  vector<string> filelist;
  filelist.push_back(filename);
  TTree* tree = get_rawdigits(filelist, nevents);
  return tree;
}
