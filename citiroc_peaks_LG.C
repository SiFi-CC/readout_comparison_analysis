//usage: this macro works on the list created by citiroc_spectroscopy_reader.C. Make sure that the name of file and list are the same as created ones, along with nofr - number of the first run.
//Peak analysis is performed on 1 chosen measurement 
//Program results in linear function which linear parameter will be used in citiroc_gaus_LG.C macro

#include "TCanvas.h"
#include "TMath.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include <stdlib.h> 


Int_t npeaks = 15; //can change
Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = par[0] + par[1]*x[0];
   for (Int_t p=0;p<npeaks;p++) {
      Double_t norm  = par[3*p+2];
      Double_t mean  = par[3*p+3];
      Double_t sigma = par[3*p+4];
      result += norm*TMath::Gaus(x[0],mean,sigma);
   }
   return result;
}

void bubbleSort(Double_t array[], Double_t error[], int size) {

  // loop to access each array element
  for (int step = 0; step < size - 1; ++step) {
    
    // check if swapping occurs  
    int swapped = 0;
    
    // loop to compare array elements
    for (int i = 0; i < size - step - 1; ++i) {
      
      // compare two array elements
      // change > to < to sort in descending order
      if (array[i] > array[i + 1]) {
        
        // swapping occurs if elements
        // are not in the intended order
        Double_t temp = array[i];
        Double_t temper = error[i];
        array[i] = array[i + 1];
        error[i] = error[i+1]; //
        array[i + 1] = temp;
        error[i+1] = temper; //
        
        swapped = 1;
      }
    }
    
    // no swapping means the array is already sorted
    // so no need for further comparison
    if (swapped == 0) {
      break;
    }
    
  }
}
void printArray(Double_t array[],Double_t error[], int size) {
  for (int i = 0; i < size; ++i) {
    printf("en = %f, error = %f \n ", array[i], error[i]);
  }
  printf("\n");
}


void peaks_LG(Int_t np=20) //Gives more or less fine result with deviations comparable to HG
{
    int nofr = 683;
    TFile *f = TFile::Open("histogram_LG_list.root");
    TList *list = (TList*)f->Get("histlist_LG;1");
    TH1F *hist[11][4]; // for each channel 8,9,10,11
    TH1F *hh = new TH1F();
    for(int i = 0; i <11; i++) //reading histograms from .root file 
    {
        for(int j = 0; j < 4; j++)
        {
            hist[i][j] = (TH1F*)list->FindObject(TString::Format("Run%d_ch%d",nofr+i,8+j));
        }
    }
    //TCanvas *c = new TCanvas("c", "c");
    //h[8][0]->Draw("hist");
    TH1F *h = (TH1F*) hist[10][3]->Clone(); //chosing the histogram to find peaks on
    
    //peak finding
    Int_t npeaks = np;
    Double_t par[3000];
    
    TCanvas *c1 = new TCanvas("HV30","HV30");
    c1->Divide(1,2);
    c1->cd(1);
    h->SetLineColor(kBlack);
    h->Sumw2(0); //drawing hist w/o uncertainties
    h->GetXaxis()->SetTitle("energy ch.");
    h->GetYaxis()->SetTitle("counts");
    h->Draw("hist");
    gPad->SetGrid(50,0);
    TSpectrum *s = new TSpectrum(2*npeaks);
    TH1 *hb = s->Background(h,1,"same"); // iterations of backgorund, more->flat
    hb->SetLineColor(kBlue);
    //return;
    
    h->Add(hb,-1); // substracting background from hist
    TH1F *h2 = (TH1F*)h->Clone("h2"); //h2-> substracted h
    
    Int_t nfound = s->Search(h,3,"",0.01); // (h,5,"",0.01)
    cout<<"nfound = "<<nfound<<endl;
    //return; // to this point its ok
    printf("Found %d candidate peaks to fit\n",nfound);
    //Estimate background using TSpectrum::Background
    //hb->Draw("same");
    if (hb) c1->Update();
    //return;
    if (np <0) return;
    //c1->cd(2);
    int range = 6000;
    TF1 *fline = new TF1("fline","pol1",1400,range); // range
    h->Fit("fline","qn"); 
   
   
    //Loop on all found peaks. Eliminate peaks at the background level
    par[0] = fline->GetParameter(0);
    par[1] = fline->GetParameter(1);
    npeaks = 0;
    Double_t *xpeaks = s->GetPositionX();
   
    Double_t fPositionX[100];
    Double_t fPositionY[100];
    for (int p=0;p<nfound;p++) {
      Double_t xp = xpeaks[p];
      Int_t bin = h->GetXaxis()->FindBin(xp);
      Double_t yp = h->GetBinContent(bin);
      if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
      par[3*npeaks+2] = yp;
      par[3*npeaks+3] = xp; //x-coordinates of peaks
      par[3*npeaks+4] = 3;
      fPositionX[p] = h->GetBinCenter(bin);
      fPositionY[p] = h->GetBinContent(bin);
      npeaks++;
    }
    //trying to set peak markers, fit and hist w/o bckrnd on the same c1
    TPolyMarker * pm = (TPolyMarker*)h2->GetListOfFunctions()->FindObject("TPolyMarker");
    if (pm) {
      h2->GetListOfFunctions()->Remove(pm);
      delete pm;
    }
    pm = new TPolyMarker(nfound, fPositionX, fPositionY);
    h2->GetListOfFunctions()->Add(pm);
    pm->SetMarkerStyle(23);
    pm->SetMarkerColor(kBlue);
    pm->SetMarkerSize(1.3);
   
    printf("Found %d useful peaks to fit\n",npeaks);
    printf("Now fitting: Be patient\n");
    TF1 *fit = new TF1("fit",fpeaks,0,range,2+3*npeaks); // range
    fit->SetLineColorAlpha(kRed, 0.1);
    fit->SetLineWidth(1);
    //we may have more than the default 25 parameters
    TVirtualFitter::Fitter(h2,10+3*npeaks);
    fit->SetParameters(par);
    fit->SetNpx(1000);
    h2->Fit("fit");

   
    //sorting list of points
    int mypeaks = 14;
    Double_t en[mypeaks];
    for(int i = 0; i<mypeaks;i++){en[i] = -1;}
    Double_t errx[mypeaks];
    Double_t pe[mypeaks];
    Double_t erry[mypeaks];
    int repeat = 0;
    for(int i =0; en[mypeaks-1] < 0 ;i++) // i = npeaks
    {    
       cout << "GetParameter  " << fit->GetParameter(3*i+3) << endl;
       if(fit->GetParameter(3*i+3) > 2400 && fit->GetParameter(3*i+3) < 3380) //adjust for specific case
       {
        en[i - repeat] = fit->GetParameter(3*i +3);
        //cout << en[i - repeat] << endl;
        errx[i - repeat] = fit->GetParError(3*i +3);
        pe[i - repeat] = 1+i-repeat;
        erry[i - repeat] = 0;
       }
       else
       {
        repeat++;
       }
    }
   
   
    // sorting works
    int size = sizeof(en) / sizeof(en[0]);
    bubbleSort(en,errx, size);
    printf("Sorted Array in Ascending Order:\n");
    printArray(en,errx, size);
    
    c1->cd(2);
    TGraphErrors *gr = new TGraphErrors(mypeaks,pe,en,erry,errx);
    gr->SetMarkerColor(4);
    gr->SetMarkerStyle(21);
    gr->SetTitle("energy channels(N.p.e)");
    gr->GetXaxis()->SetTitle("N.p.e");
    gr->GetYaxis()->SetTitle("energy ch.");
    TF1 *linfit = new TF1("linear fit", " x*[0] +[1] ", 0,mypeaks+1);
    linfit->SetParameters(250,3000);
    linfit->SetParNames("a", "b");
    gr->Fit("linear fit");
    gr->Draw("ALP");
    linfit=gr->GetFunction("linear fit");
    c1->cd(0);
    return;
}
