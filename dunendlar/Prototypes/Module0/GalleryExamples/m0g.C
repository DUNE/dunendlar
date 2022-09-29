#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "TFile.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "TCanvas.h"
#include "TAxis.h"

using namespace art;
using namespace std;

// make a poor-man's event display of recob::SpacePoints -- for the ievcount'th event in the file
// spacepoint tag string can be "pandora" or "pmtrack"
// This version colors red the spacepoints that are in a cylinder about an approximate beam axis
// the parameters of the cylinder are hard-coded
// Tom Junk, Fermilab, Sep. 18, 2016.  Based on gallery demos by Marc Paterno.
// Usage:  setup dunetpc and gallery, run root.
//  .L pdspdisp.C++
// pdspdisp("inputfile.root",0);

void
m0g(size_t ievcount=0, std::string const& filename="tmpmodule0.root",  std::string spacepointtagstring="ndlarm0spacepoint")
{

  bool frotate=false;
  bool fprint=false;

  size_t evcounter=0;

  InputTag recobspacepoint_tag{ spacepointtagstring };
  // Create a vector of length 1, containing the given filename.
  vector<string> filenames(1, filename);

  TString outname="event";
  outname += ievcount;
  outname += ".gif+";

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    if (evcounter == ievcount)
      {
	auto const& recobspacepoints = *ev.getValidHandle<vector<recob::SpacePoint>>(recobspacepoint_tag);
	if (!recobspacepoints.empty())
	  {
	    int outroad=0;

	    TGraph2D *gro = new TGraph2D();
            TCanvas *c = new TCanvas("c","TGraph2D Event Display",0,0,800,800);
	    for (size_t isp=0;isp<recobspacepoints.size(); ++isp)
	      {
		double x = recobspacepoints[isp].XYZ()[0];
		double y = recobspacepoints[isp].XYZ()[1];
		double z = recobspacepoints[isp].XYZ()[2];
		if (TMath::Abs(x)<700 && TMath::Abs(y)<700 && TMath::Abs(z)<700)
		  {
                    gro->SetPoint(outroad,z,x,y);
		  }
		++outroad;
	      }

	    TGraph2D *grlimit = new TGraph2D();
	    grlimit->SetPoint(0,-50,-16,-60);
	    grlimit->SetPoint(1,50,16,60);
	    grlimit->SetMarkerColor(0);
	    grlimit->SetMarkerStyle(1);
	    grlimit->Draw("P");

	    gro->SetMarkerColor(1);
	    gro->SetMarkerSize(0.25);
	    gro->SetMarkerStyle(20);
	    gro->Draw("P,SAME");
	    std::string titlestring=spacepointtagstring;
	    titlestring += " SpacePoint Display";
	    grlimit->SetTitle(titlestring.c_str());
	    grlimit->GetXaxis()->SetTitle("Z (cm)");
	    grlimit->GetXaxis()->SetTitleColor(4);
	    grlimit->GetYaxis()->SetTitle("X (cm)");
	    grlimit->GetYaxis()->SetTitleColor(4);
	    grlimit->GetZaxis()->SetTitle("Y (cm)");
	    grlimit->GetZaxis()->SetTitleColor(4);
      	    c->Update();

	    if (frotate)
	      {
		for (size_t iframe=0; iframe<100; ++iframe)
		  {
		    c->SetTheta(10.0 + 0.1*iframe);
		    c->SetPhi(60.0 + 1.0*iframe);
      	            c->Update();
		    if (fprint)
		      {
			c->Print(outname);
		      }
		  }
		if (fprint)
		  {
		    TString outnamepp = outname;
		    outnamepp += "+";
		    c->Print(outnamepp);
		  }
	      }
	  }
      }
    ++evcounter;
  }
}
