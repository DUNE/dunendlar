#ifndef EDEP_CALC_UTILS_H
#define EDEP_CALC_UTILS_H

#include "Math/Vector4D.h"
#include "Math/Vector3D.h"
#include "lardataobj/Simulation/SimEnergyDeposit.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"
#include "lardataobj/Simulation/SimEnergyDeposit.h"
#include "nusimdata/SimulationBase/MCGeneratorInfo.h"
#include <vector>
#include <map>

using namespace ROOT::Math;


namespace edep_utils{

  inline std::map<simb::Generator_t, std::string> &generatorTable()
  {
    static std::map<simb::Generator_t, std::string> m_generatorTable;
    if(m_generatorTable.empty())
    {
      m_generatorTable[simb::Generator_t::kUnknown] = "Unknown";
      m_generatorTable[simb::Generator_t::kGENIE] = "GENIE";
      m_generatorTable[simb::Generator_t::kCRY] = "CRY";
      m_generatorTable[simb::Generator_t::kGIBUU] = "GIBUU";
      m_generatorTable[simb::Generator_t::kNuWro] = "NuWro";
      m_generatorTable[simb::Generator_t::kMARLEY] = "MARLEY";
      m_generatorTable[simb::Generator_t::kNEUT] = "NEUT";
      m_generatorTable[simb::Generator_t::kCORSIKA] = "CORSIKA";
      m_generatorTable[simb::Generator_t::kGEANT] = "GEANT";
    }
    return m_generatorTable;
  }

  inline std::map<unsigned char, std::pair<int,int>> &processTable()
  {
    static std::map<unsigned char, std::pair<int,int>> m_processTable;
    // 0: "undefined"
    // 1:"hadElastic"
    // 2:"pi-Inelastic"
    // 3:"pi+Inelastic"
    // 4:"kaon-Inelastic"
    // 5:"kaon+Inelastic"
    // 6:"protonInelastic"
    // 7:"neutronInelastic"
    // 8:"CoulombScat"
    // 9:"nCapture"
    // 10: "Transportation"
    if( m_processTable.empty() )
    {
      m_processTable[0] = {  TG4TrajectoryPoint::kProcessNotDefined,       TG4TrajectoryPoint::kProcessNotDefined };
      m_processTable[1] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronElastic };
      m_processTable[2] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronInelastic };
      m_processTable[3] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronInelastic };
      m_processTable[4] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronInelastic };
      m_processTable[5] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronInelastic };
      m_processTable[6] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronInelastic };
      m_processTable[7] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronInelastic };
      m_processTable[8] = {  TG4TrajectoryPoint::kProcessElectromagetic ,  TG4TrajectoryPoint::kSubtypeEMCoulombScattering};
      m_processTable[9] = {  TG4TrajectoryPoint::kProcessHadronic,         TG4TrajectoryPoint::kSubtypeHadronCapture };
      m_processTable[10] = { TG4TrajectoryPoint::kProcessTransportation,   TG4TrajectoryPoint::kSubtypeGeneralStepLimit };
    }

    return m_processTable;
  }


  class NDHitSegment
  {
    public:
      NDHitSegment( double maxSagitta=0.1 /*cm*/, 
          double maxSeparation = 0.1 /*cm*/,
          double maxLength =0.5 /*cm*/,
          int deltaTrackID = -1
          ) : 
        fMaxSagitta(maxSagitta), 
        fMaxSeparation(maxSeparation),
        fMaxLength(maxLength),
        fDeltaTID(deltaTrackID),
        fSegmentInitialized(false)
      {};

      NDHitSegment( art::Ptr<sim::SimEnergyDeposit> &sed,
          double maxSagitta=0.1 /*cm*/, 
          double maxSeparation = 0.1 /*cm*/,
          double maxLength =0.5 /*cm*/,
          int deltaTrackID = -1
          ) : 
        fMaxSagitta(maxSagitta), 
        fMaxSeparation(maxSeparation),
        fMaxLength(maxLength),
        fDeltaTID(deltaTrackID),
        fSegmentInitialized(false)
      {
        this->AddStep(sed);
      };


      /// Add the effects of a part of a step to this hit.
      void AddStep(art::Ptr<sim::SimEnergyDeposit> &sed);

      /// Hits for the same primary particle, in the same physical volume belong
      /// in the same hit.
      bool SameHit(art::Ptr<sim::SimEnergyDeposit> &sed);

      /// Find the maximum separation (the sagitta) between the current hit
      /// segment path points, and the straight line connecting the start and
      /// proposed new stop point. 
      double FindSagitta(art::Ptr<sim::SimEnergyDeposit> &sed);

      /// Find the maximum distance from the hit segment to the new step that is
      /// proposed to be added to the hit segment. This is used to
      /// combine secondaries with a parent track.
      double FindSeparation(art::Ptr<sim::SimEnergyDeposit> &sed);


      /// Data accessor functions
      double NumPhotons() { return fNumPhotons; }
      double NumFPhotons() { return fNumFPhotons; }
      double NumSPhotons() { return fNumSPhotons; }
      double NumElectrons() { return fNumElectrons; }
      double ScintYieldRatio() { return (fNumPhotons>0)? fNumFPhotons/fNumPhotons : 0; }
      double TotalEnergyDeposit(){ return fEnergyDeposit; }
      double SecondaryEnergyDeposit(){ return fSecondaryDeposit; }
      XYZTVector StartPos(){ return fStart; }
      XYZTVector StopPos(){ return fStop; }
      int TrackID(){ return fPrimaryId; }
      int PDG(){ return fContributorPDGs[0]; }
      int NSteps(){ return fSedlist.size(); }
      double TrackLength() { return fTrackLength; }

      std::vector<int> Contributors() { return fContributors; }

      void PrintHit();

      void SetDeltaTrackID(int delta = 0){ fDeltaTID =  delta; }
      int GetDeltaTrackID(){ return fDeltaTID; }

    private:
      
      /// The sagitta tolerance for the segment.
      double fMaxSagitta;

      /// The separation for the segment.
      double fMaxSeparation;

      /// The maximum length between the start and stop points of the segment.
      double fMaxLength;

      /// deltaTrackID --- displace value of track id by delta amount
      int fDeltaTID;


      /// The TrackID for each trajectory that contributed to this hit.  This
      /// could contain the TrackID of the primary particle, but not
      /// necessarily.  
      std::vector<int> fContributors;
      std::vector<int> fContributorPDGs;

      /// The track id of the primary particle.
      int fPrimaryId;

      /// The total energy deposit in this hit.  
      double fEnergyDeposit;

      /// The num Photon/Electron from SimEnergyDeposit 
      int fNumPhotons;
      int fNumFPhotons;
      int fNumSPhotons;
      int fNumElectrons;

      /// The "secondary" energy deposit in this hit.  This is used to help
      /// simulate the recombination of electrons, and is part of the total
      /// energy deposit.
      ///
      /// DETSIM is expected to use this field to save the amount of energy
      /// deposited as opticalphotons.  The remaining energy will be deposited
      /// as ionization.  In this model (in argon), the mean number of quanta
      /// created will be <N_q> = (fEnergyDeposit)/(19.5*eV), N_q should be
      /// fluctuated around <N_q>, N_ph = N_q*fSecondaryDeposit/fEnergyDeposit,
      /// and N_e = N_q - N_ph.  Thd fSecondaryDeposit value already includes
      /// the binomial fluctuation, so don't fluctuate N_ph or N_e.
      double fSecondaryDeposit;

      /// The total charged track length in this hit.  This includes the
      /// contribution from all of the secondary particles (e.g. delta-rays)
      /// that are included in this hit.
      double fTrackLength;

      /// The starting position of the segment.
      XYZTVector fStart;

      /// The stopping position of the segment.
      XYZTVector fStop;

      /// The end points of the steps that make up this hit.  This is used to
      /// make sure that the current hit stays inside of it's allowed
      /// tolerances.
      std::vector<geo::Point_t> fPath;


      /// Check if segment has been initialized
      bool fSegmentInitialized;

      /// The SEDs making up of this hit
      std::vector<art::Ptr<sim::SimEnergyDeposit>> fSedlist;

  };
} // namespace edep_utils


void edep_utils::NDHitSegment::AddStep(art::Ptr<sim::SimEnergyDeposit> &sed){
  //cout to make it build
  geo::Point_t prePos = sed->Start();
  geo::Point_t postPos = sed->End();
  //TODO find stepStatus Equivalent
  // G4StepStatus stepStatus = sed->GetPostStepPoint()->GetStepStatus();
  // G4ParticleDefinition* particle =  sed->GetTrack()->GetDefinition();
  double stepLength = (prePos-postPos).R();
  double trackLength = sed->StepLength();


  int NumElectrons = sed->NumElectrons();
  int NumPhotons = sed->NumPhotons();
  int NumSPhotons = sed->NumSPhotons();
  int NumFPhotons = sed->NumFPhotons();


  double energyDeposit = sed->Energy();
  // As secondary = total * n_photon/(n_q), where n_q = n_photon + n_e
  double nonIonizingDeposit = (NumPhotons>0)?energyDeposit*NumPhotons/(NumPhotons+NumElectrons) : 0;
  energyDeposit -= nonIonizingDeposit;

  if (trackLength < 0.75*stepLength || trackLength < stepLength - 0.1/*cm*/) {
    std::cout<<"Track length shorter than step: " 
        << trackLength << " cm "
        << "<" << stepLength << " cm";
    std::cout<<"    Volume: "
        << "LArSoft"<<std::endl;
    std::cout<<"    Particle: " << sed->TrackID()
        << " Depositing " << energyDeposit << " MeV";
    std::cout<<"    Total Energy: " 
        << "Unimplemented"<<std::endl;
  }

  trackLength = std::max(trackLength,stepLength);

  //std::cout<<"Add Step with " << energyDeposit <<" MeV"
  //    << " in " << "LArSoft"<<std::endl;

  if (energyDeposit <= 0.) {
    std::cout<<"EDepSim::HitSegment:: No energy deposited: " << energyDeposit << std::endl;
  }

  if (trackLength <= 0.) {
    std::cout<<"EDepSim::HitSegment:: No track length: " << trackLength << std::endl;
  }
  if (energyDeposit >= 1 || std::isnan(energyDeposit) ) {
    std::cout<<"EDepSim::HitSegment:: Too much energy deposited: " << energyDeposit << std::endl;
    std::cout<<"                    : (ne,nphoton,pdg,trkid):    " << NumElectrons << ","
                                                                   << NumPhotons <<","
                                                                   << sed->PdgCode()<<","
                                                                   << sed->TrackID() <<std::endl;
  }


  // Occasionally, a neutral particle will produce a particle below
  // threshold, and it will be recorded as generating the hit.  All of the
  // energy should be deposited at the stopping point of the track.
  //
  // TODO: may need to find equivalent
  // if (stepStatus == fPostStepDoItProc
  //     && std::abs(particle->GetPDGCharge()) < 0.1) {
  //   double origStep = stepLength;
  //   geo::Point_t dir = (postPos - prePos).unit();
  //   stepLength = trackLength = std::min(0.5 /*mm*/,0.8*origStep);
  //   prePos = postPos - stepLength*dir;
  //   EDepSimDebug("EDepSim::HitSegment:: " << particle->GetParticleName()
  //       << " Deposited " << energyDeposit << " MeV");
  //   EDepSimDebug("    Original step: " << origStep << " mm");
  //   EDepSimDebug("    New step: " << stepLength << " mm");
  // }

  if (stepLength>fMaxLength || trackLength>fMaxLength) {
    std::cout<<"  Step Length is " 
        << stepLength 
        << " cm and track length is " 
        << trackLength << " cm"<<std::endl;
  }

  /// First make sure the step has been initialized.
  int trackId = sed->TrackID()+GetDeltaTrackID();
  if (!fSegmentInitialized) {
    // std::cout<<"fSegmentInitialized is false, set to true"<<std::endl;
    fSegmentInitialized = true;
    fPrimaryId = trackId;
    fEnergyDeposit    = 0;
    fSecondaryDeposit = 0;
    fNumElectrons     = 0;
    fNumPhotons       = 0;
    fNumSPhotons      = 0;
    fNumFPhotons      = 0;
    fNumElectrons     = 0;

    fStart.SetXYZT(sed->StartX(), sed->StartY(), sed->StartZ(), sed->StartT());
    fPath.push_back(geo::Point_t(fStart.x(),fStart.y(),fStart.z()));
    fStop.SetXYZT(sed->EndX(), sed->EndY(), sed->EndZ(), sed->EndT());
    fPath.push_back(geo::Point_t(fStop.x(),fStop.y(),fStop.z()));

    fSedlist.push_back(sed);
    fContributors.push_back(trackId);
    fContributorPDGs.push_back(sed->PdgCode());
  }
  else {
    // Record the tracks that contribute to this hit.
    // std::cout<<"fSegmentInitialized is true, continue"<<std::endl;
    if (trackId != fContributors.front()) fContributors.push_back(trackId);
    int pdg = sed->PdgCode();
    if (pdg != fContributorPDGs.front()) fContributorPDGs.push_back(pdg);

    // std::cout<<"Check to see if we have a new stopping point."<<std::endl;
    // Check to see if we have a new stopping point.
    // double diff = (fPath.back()-prePos).R();
    // std::cout<<"diff = "<<diff<<std::endl;
    if (trackId == fContributors.front()
        && (fPath.back()-prePos).R()<0.1 /*cm*/) {
      fStop.SetXYZT(sed->EndX(), sed->EndY(), sed->EndZ(), sed->EndT());
      fPath.push_back(geo::Point_t(fStop.x(),fStop.y(),fStop.z()));
      fSedlist.push_back(sed);

    }
  }

  fNumElectrons += NumElectrons;
  fNumPhotons   += NumPhotons;
  fNumSPhotons  += NumSPhotons;
  fNumFPhotons  += NumFPhotons;
  fNumElectrons += NumElectrons;


  fEnergyDeposit += energyDeposit;
  fSecondaryDeposit += nonIonizingDeposit;
  fTrackLength += trackLength;

  //std::cout<<"EDepSim::HitSegment:: Deposit " << sed->PdgCode()
  //    << " adds " << energyDeposit << " MeV"
  //    << " to " << fContributors.front()
  //    << " w/ " << fEnergyDeposit
  //    << " L " << trackLength
  //    << " " << fTrackLength<<std::endl;
}

bool edep_utils::NDHitSegment::SameHit(art::Ptr<sim::SimEnergyDeposit> &sed){
  int trackid = sed->TrackID() + GetDeltaTrackID();
  double endToEnd = (sed->Start() - fPath.front()).R();
  if ( endToEnd > fMaxLength ) 
  {
    return false;
  }
  double epsilon = 0.01; /*seconds ?*/
  double deltaT = std::abs( sed->EndT() - fStop.T() );
  if ( deltaT > epsilon )
  {
    return false;
  }

  if( fContributors.front() == trackid )
  {
    // This is still the same track that initially created this hit.
    // Check to see if the hit should be extended, or if we should start a
    // new segment.
    double sagitta = FindSagitta(sed);
    if (sagitta > fMaxSagitta) {
      return false;
    }
  }
  else
  {
    // This is not the same track that started this hit, but check to see
    // if it is a delta-ray that should be added to this segment.
    double separation = FindSeparation(sed);
    if (separation > fMaxSeparation) {
      return false;
    }
  }

  return true;
}

double edep_utils::NDHitSegment::FindSagitta(art::Ptr<sim::SimEnergyDeposit> &sed)
{
  // What this means: 
  // Basically using the direction of new sed from the last point on the path, 
  // projects each point in the path and check if the transverse component is larger
  // than fMaxSagitta.

  geo::Point_t& back = fPath.back();
  geo::Point_t preStep = sed->Start();
  //TODO: maybe not hard code threshold?
  // Make sure that the step began at the end of the current segment.  If
  // not, return a ridiculously large sagitta.
  if((back-preStep).R() > 0.001 /*cm*/) return (10*100); // 10m, 

  geo::Point_t postStep = sed->End();

  geo::Point_t newDir = (geo::Point_t) (postStep-back).unit();

  //Initialize the maximum sagitta found.
  double maxSagitta = 0.0;

  geo::Point_t front = fPath.front();

  // Loop over the existing path points and see if any would fall outside of
  // the tolerance.
  for (std::vector<geo::Point_t>::iterator p = fPath.begin(); p != fPath.end(); ++p) 
  {
    geo::Point_t delta = (geo::Point_t) ((*p)-front);
    double dist = ( delta.X() * newDir.X() + delta.Y() * newDir.Y() + delta.Z() * newDir.Z() );
    maxSagitta = std::max(maxSagitta,(delta - newDir*dist ).R());
    if (maxSagitta > fMaxSagitta) break;
  }

  return maxSagitta;
}



double edep_utils::NDHitSegment::FindSeparation(art::Ptr<sim::SimEnergyDeposit> &sed)
{
  geo::Point_t& front = fPath.front();
  const geo::Point_t& back = fPath.back();
  const geo::Point_t preStep = sed->Start();
  const geo::Point_t postStep = sed->End();
  auto dir = (back-front).unit();

  // Check to make sure the beginning of the new step isn't after the
  // end of this segment.
  if ((preStep-back).Dot(dir)>0.0) return 1000*1000; /*1000m*/

  // Check to make sure the beginning of the new step isn't before the
  // beginning of this segment.
  auto  frontDelta =  (preStep-front);
  double cosDelta = frontDelta.Dot(dir);
  if (cosDelta<0.0) return 1000*1000; /*1000m*/

  // Find the distance from the segment center line to the initial point of
  // the new step.
  double s1 = (frontDelta - dir*cosDelta).R();


  // Find the distance from the segment center line to the final point of
  // the new step.
  geo::Point_t rearDelta = (geo::Point_t) (postStep-front);
  cosDelta = rearDelta.Dot(dir);
  double s2 = (rearDelta - cosDelta*dir).R();

  return std::max(s1,s2);
}

void edep_utils::NDHitSegment::PrintHit()
{
  std::cout<<"\t\tHit Information:\n"
           <<"\t\t\tN seds: "<<fSedlist.size()<<"\n"
           <<"\t\t\tLength: "<<(fStop-fStart).R()<<" cm\n"
           <<"\t\t\tEnergy: "<<fEnergyDeposit<<" MeV\n"
           <<"\t\t\tNe:     "<<fNumElectrons<<"\n"
           <<"\t\t\tNgamma: "<<fNumPhotons<<std::endl;
}

#endif
