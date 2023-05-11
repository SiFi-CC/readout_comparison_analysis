//usage:
// Firstly one needs to obtain a (linear)  paramter from citiroc_peaks_HG.C and change the value of HG to one used in citiroc_peaks_HG.C macro.
//Then change the value of "w" to linear coefficient and "uw" to its error. 
//Then make sure that in line 61,62 the file and list is named corretly as ones created in citiroc_spectroscopy_reader.c to read the data from series of runs. 
//This program results in creating a  function p0 + p1*exp(p2*x + p3*x^2 +p4*x^3) which transfroms energy channels to photoelectron energies for given HG setting.
//The output function should be understood as f(HG) = pe.en [Energy N Channels] meaning f(HG) is number of Energy N Channels that equal photpeak energy.

Double_t HG = 55;
Double_t w = 228.93;
Double_t uw = 0.0481;





Double_t calibration(Double_t hg, Double_t off, Double_t a, Double_t lin, Double_t quad, Double_t cube)
    {
        //cout << "hg functions is " << "((" << off << "+ (" << a << ")*exp(hg*(" << lin << ") + (" << quad << ")*pow(hg,2) + "  << cube << "*pow(hg,3) )) / (" << off <<  "+ (" << a << ")*exp(63*(" << lin << ") + (" << quad << ")*pow(63,2) + " << cube << "*pow(63,3) ) ))*228.93" << endl; // <-- prints the function
        return
        ((off + (a)*exp(hg*(lin) + (quad)*pow(hg,2) + (cube)*pow(hg,3) )) / (off + (a)*exp(HG*(lin) + (quad)*pow(HG,2) + cube*pow(HG,3) ) ))*228.93;
    };//228.93 and 0.0481 are values obtain in peaks_HG.c for chosen HG (0-63)


Double_t expoError(Double_t hg, TMatrixDSym cov, Double_t a, Double_t b, Double_t cc, Double_t d, Double_t e)
{
    TMatrixD g(1,5);    //vector of partial derivatives
    Double_t data[5] = {1, exp(cc*hg + d*pow(hg,2)), b*hg*exp(cc*hg + d*pow(hg,2)), pow(hg,2)*b*exp(cc*hg +d*pow(hg,2)), pow(hg,3)*b*exp(cc*hg +d*pow(hg,2))};
    g.SetMatrixArray(data);
    TMatrixD g_t(TMatrixD::kTransposed,g);
    TMatrixD sigma(1,1); // sigma squared 
    sigma = g*cov*g_t;
    //g.Print();
    //g_t.Print();
    //cov.Print();
    //sigma.Print();
    Double_t sval = sqrt(TMatrixDRow(sigma,0)(0)); // TMatrixDRow(sigma40,1)[1]; -> getting value
    //cout << sval << endl;
    return sval;
};

//gives error of final function expo()*w/expo(40)
 Double_t calibrationError(Double_t hg, TMatrixDSym cov, Double_t a, Double_t b, Double_t cc, Double_t d, Double_t e, Double_t w, Double_t uw)
{
        Double_t res = sqrt(
            pow(expoError(hg, cov, a, b, cc ,d,e)*w/(a+b*exp(cc*HG + d*pow(HG,2) + e*pow(HG,3) )),2)+
            pow((a+b*exp(cc*hg + d*pow(hg,2)+ e*pow(hg,3)))*uw/(a+b*exp(cc*HG + d*pow(HG,2) + e*pow(HG,3))),2)+
            pow(w*(a+b*exp(cc*hg + d*pow(hg,2) + e*pow(hg,3)))*expoError(HG, cov, a, b, cc, d,e)/pow(a+b*exp(cc*HG + d*pow(HG,2)+ e*pow(HG,3)),2),2));
            //cout << res << endl;
            return res;
}

void ResultAndSigma(Double_t hg, TMatrixDSym cov, Double_t a, Double_t b, Double_t cc, Double_t d,Double_t e, Double_t w, Double_t uw)
{
        Double_t Result = calibration(hg, a ,b, cc, d,e);
        Double_t Sigma = calibrationError(hg, cov, a, b, cc, d,e, w, uw);
        //cout << "f(" << hg << ") = " << Result << endl << "sigma(" << hg << ") = " << Sigma << endl;
        //cout <<"result: "<< Result << ", sigma: " << Sigma << endl;
        cout << Result << " " << Sigma << endl;
}


void citiroc_gaus_HG() //works fine
{
    const int n = 11;
    int nofr = 661;
    TFile *f = TFile::Open("histogram_HG_list.root");
    TList *list = (TList*)f->Get("histlist_HG;1");
    TH1F *h[11][4]; // for each channel 8,9,10,11 and run
    TF1 *fit[11][4];
    TH1F *hh1 = new TH1F();
    TF1 *ff1;
    for(int i = 0; i <11; i++) //reading histograms from .root file 
    {
        for(int j = 0; j < 4; j++)
        {
            h[i][j] = (TH1F*)list->FindObject(TString::Format("Run%d_ch%d",nofr+i,8+j));
        }
    }
    const int bin = 500;
    const int color[n]=
    {
        1,
        12,
        3,
        4,
        56,
        6,
        7,
        8,
        9,
        49,
        51
    };
    
    
    const int limup[n] =  // manipulate this to change the position of the fit center 
    {
        1237,
        1400,
        1700,
        2317,
        2637,
        2920,
        3630,
        4651,
        6411,
        8000,
        8000
    };
    const int limdown[n] = 
    {
        520,
        540,
        630,
        931,
        1099,
        1174,
        1585,
        1996,
        2762,
        3600,
        4555
    };
    
    double mean[n];
    double err[n];

    int lcount[n] = {0};
    char line[20];
    
    
    
    TCanvas *c = new TCanvas("Gaus","Gaus");
    c->DivideSquare(n+1);
    gStyle->SetOptStat(111111); // 111111 if overflow, 0 if hists on the same graph
    //gStyle->SetOptTitle(kFALSE);
    gPad->SetGrid(0,1);
    //gStyle->SetPalette(kSolar);
    
    TH1F *hh2 = new TH1F();
    TF1 *ff = new TF1();
    for (int i = 0; i< n; i++)      //loop for every histogram
    {
      c->cd(i+1);
      
      fit[i][0] = new TF1("t1", "gaus", limdown[i],limup[i]);
      h[i][0]->Fit(fit[i][0],"R");
      mean[i] = fit[i][0]->GetParameter(1);
      err[i] = fit[i][0]->GetParError(1);
      
      h[i][0]->SetLineColor(color[i]);
      h[i][0]->SetContour(100);
      h[i][0]->Draw("hist");
      fit[i][0]->Draw("same");
      gPad->SetGrid(0,1);

    }
    
    //searcjhing for peaks in the 1st histogram
    delete hh1,hh2,ff1;
    for(int i =0; i <n; i++)
    {
        cout << "mean "<<mean[i] << ", error "<< err[i] << endl;
    }
    c->cd(n+1);
    Double_t hgs[n] = {1, 10, 20, 30, 35, 40, 45, 50, 55, 60, 63}; //63 point is tough
    auto gr = new TGraphErrors(n, hgs, mean, 0, err);
    TF1 *pfit = new TF1("fit", "[0] + [1]*exp([2]*x + [3]*x^2 + [4]*x^3)", 1,60);
    pfit->SetParameters(530, 7,0.029);
    //pfit->SetParLimits(1,0.1, 10000);
    //pfit->SetParLimits(2,0.1, 10);
    gr->Fit(pfit,"R+");
    gr->SetTitle("mean(hg)");
    gr->GetXaxis()->SetTitle("hg");
    gr->GetXaxis()->CenterTitle(true);
    gr->GetYaxis()->SetTitle("mean");
    gr->GetYaxis()->CenterTitle(true);
    gr->GetYaxis()->SetRangeUser(0,10000);
    gr->Draw("ALP");
    pfit->Draw("same");
    
    c->cd(0);
    
    // now we have the parameters of the mean(hg) and linear fit for en.ch(hg), hg = 40
    //calibration(hg) = w*expo/expo(40) ->fitting was done for hg = 40
    //paramter w is given by the program peak.c with it's error uw
    
    //expo = a+b*exp(c*x + dx^2) -> u(expo)
    //expo(40) = t -> u(t)
    //w -> u(w)
    Double_t a = pfit->GetParameter(0);
    Double_t ua = pfit->GetParError(0); 
    Double_t b = pfit->GetParameter(1);
    Double_t ub = pfit->GetParError(1); 
    Double_t cc = pfit->GetParameter(2);
    Double_t ucc = pfit->GetParError(2);
    Double_t d = pfit->GetParameter(3);
    Double_t ud = pfit->GetParError(3);
    Double_t e = pfit->GetParameter(4);
    Double_t ed = pfit->GetParError(4);
    Double_t w = 228.93;
    Double_t uw = 0.0481;
    
    //calibration(40,pfit->GetParameter(0), pfit->GetParameter(1), pfit->GetParameter(2), pfit->GetParameter(3));
    
    TFitResultPtr fitResult = gr->Fit(pfit,"S");
    TMatrixDSym cov = fitResult->GetCovarianceMatrix();
    fitResult->Print("V");
    // to find the correspondence of en. channels to p.e. change the first argument hg={1,..,63}
   // ResultAndSigma(55,cov, a ,b, cc, d, w, uw);
   for(int i =1; i< 64; i++)
    {
        //cout << "result for hg = " << i << ":  :";
        ResultAndSigma(i,cov,a,b,cc,d,e,w,uw);
    }
    //calibration(55,a,b,cc,d,e);
}
