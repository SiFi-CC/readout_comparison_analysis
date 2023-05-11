//usage:
// Firstly one needs to obtain a (linear)  paramter from citiroc_peaks_LG.C and change the value of HG to one used in citiroc_peaks_HG.C macro.
//Then change the value of "w" to linear coefficient and "uw" to its error. 
//Then make sure that in line 61,62 the file and list is named corretly as ones created in citiroc_spectroscopy_reader.c to read the data from series of runs. 
//This program results in creating a  function p0 + p1*exp(p2*x + p3*x^2 +p4*x^3) which transfroms energy channels to photoelectron energies for given LG setting.
//The output function should be understood as f(LG) = pe.en [Energy N Channels] meaning f(HG) is number of Energy N Channels that equal photpeak energy.
Double_t LG = 63;
Double_t w = 54.7674;
Double_t uw = 0.0665598;


Double_t calibration(Double_t lg, Double_t off, Double_t a, Double_t lin, Double_t quad, Double_t cube)
    {
        
        
        //cout << "lg functions is " << "((" << off << "+ (" << a << ")*exp(lg*(" << lin << ") + (" << quad << ")*pow(lg,2) + "  << cube << "*pow(lg,3) )) / (" << off <<  "+ (" << a << ")*exp(63*(" << lin << ") + (" << quad << ")*pow(63,2) + " << cube << "*pow(63,3) ) ))*54.7674" << endl; // <-- prints the function
        return
        ((off + (a)*exp(lg*(lin) + (quad)*pow(lg,2) + (cube)*pow(lg,3) )) / (off + (a)*exp(LG*(lin) + (quad)*pow(LG,2) + cube*pow(LG,3) ) ))*54.7674;
    };//5.47674e+01 and 6.65598e-02 are values obtain in peaks_LG.c for lg = LG


//gives sigma(expo)
//expo = a +b*exp(c*x + d)
Double_t expoError(Double_t lg, TMatrixDSym cov, Double_t a, Double_t b, Double_t cc, Double_t d, Double_t e)
{
    TMatrixD g(1,5);    //vector of partial derivatives
    Double_t data[5] = {1, exp(cc*lg + d*pow(lg,2)), b*lg*exp(cc*lg + d*pow(lg,2)), pow(lg,2)*b*exp(cc*lg +d*pow(lg,2)), pow(lg,3)*b*exp(cc*lg +d*pow(lg,2))};
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

//gives error of final function expo()*w/expo(LG)
Double_t calibrationError(Double_t lg, TMatrixDSym cov, Double_t a, Double_t b, Double_t cc, Double_t d, Double_t e, Double_t w, Double_t uw)
{
        Double_t res = sqrt(
            pow(expoError(lg, cov, a, b, cc ,d,e)*w/(a+b*exp(cc*LG + d*pow(LG,2) + e*pow(LG,3) )),2)+
            pow((a+b*exp(cc*lg + d*pow(lg,2)+ e*pow(lg,3)))*uw/(a+b*exp(cc*LG + d*pow(LG,2) + e*pow(LG,3))),2)+
            pow(w*(a+b*exp(cc*lg + d*pow(lg,2) + e*pow(lg,3)))*expoError(LG, cov, a, b, cc, d,e)/pow(a+b*exp(cc*LG + d*pow(LG,2)+ e*pow(LG,3)),2),2));
            //cout << res << endl;
            return res;
}

void ResultAndSigma(Double_t lg, TMatrixDSym cov, Double_t a, Double_t b, Double_t cc, Double_t d, Double_t e, Double_t w, Double_t uw)
{
        Double_t Result = calibration(lg, a ,b, cc, d, e);
        Double_t Sigma = calibrationError(lg, cov, a, b, cc, d, e, w, uw);
        //cout << "f(" << lg << ") = " << Result << endl << "sigma(" << lg << ") = " << Sigma << endl;
        //cout <<"result: "<< Result << ", sigma: " << Sigma << endl;
        cout << Result << " " << Sigma << endl;
}


void citiroc_gaus_LG() //works fine
{
    const int n = 11;
    int nofr = 683;
    TFile *f = TFile::Open("histogram_LG_list.root");
    TList *list = (TList*)f->Get("histlist_LG;1");
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
    
    const int limdown[n] = 
    {
        111,
        126,
        141,
        190,
        219,
        234,
        300,
        408,
        600,
        1150,
        1610
    };
    
    const int limup[n] =  // manipulate this to change the position of the fit center 
    {
        220,
        220,
        300,
        388,
        435,
        547,
        620,
        850,
        1201,
        2041,
        3300
    };
    
    double mean[n];
    double err[n];

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
      
      fit[i][3] = new TF1("t1", "gaus", limdown[i],limup[i]);
      h[i][3]->Fit(fit[i][3],"R");
      mean[i] = fit[i][3]->GetParameter(1);
      err[i] = fit[i][3]->GetParError(1);
      
      h[i][3]->SetLineColor(color[i]);
      h[i][3]->SetContour(100);
      h[i][3]->Draw("hist");
      fit[i][3]->Draw("same");
      gPad->SetGrid(0,1);
    }
    
    //searcjhing for peaks in the 1st histogram
    delete hh1,hh2,ff1;
    for(int i =0; i <n; i++)
    {
        cout << "mean "<<mean[i] << ", error "<< err[i] << endl;
    }
    c->cd(n+1);
    Double_t lgs[n] = {1, 10, 20, 30, 35, 40, 45, 50, 55, 60, 63}; //63 point is tough
    auto gr = new TGraphErrors(n, lgs, mean, 0, err);
    TF1 *pfit = new TF1("fit", "[0] + [1]*exp([2]*x + [3]*x^2 + [4]*x^3)", 1,64); //"[0] + [1]*exp([2]*x + [3]*x^2)"
    pfit->SetParameters(130, 400,0.02,0.0002,0); //(130, 400,0.02,0.0002);
    //pfit->SetParLimits(1,0.1, 10000);
    //pfit->SetParLimits(2,0.1, 10);
    gr->Fit(pfit,"R");
    gr->SetTitle("mean(lg)");
    gr->GetXaxis()->SetTitle("lg");
    gr->GetXaxis()->CenterTitle(true);
    gr->GetYaxis()->SetTitle("mean");
    gr->GetYaxis()->CenterTitle(true);
    
    gr->Draw("ALP");
    pfit->Draw("same");
    
    c->cd(0);
    
    // now we have the parameters of the mean(lg) and linear fit for en.ch(lg), lg = 63
    //calibration(lg) = w*expo/expo(63) ->fitting was done for lg = 63
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
    Double_t w = 54.7674;
    Double_t uw = 0.0665598;
    //const int lg = 40;
    
    //calibration(63,pfit->GetParameter(0), pfit->GetParameter(1), pfit->GetParameter(2), pfit->GetParameter(3));
    
    TFitResultPtr fitResult = gr->Fit(pfit,"S");
    TMatrixDSym cov = fitResult->GetCovarianceMatrix();
    fitResult->Print("V");
    // to find the correspondence of en. channels to p.e. change the first argument lg={1,..,63}
    //ResultAndSigma(63,cov, a ,b, cc, d, e, w, uw);
   for(int i =1; i< 64; i++)
    {
        //cout << "result for lg = " << i << ":  :";
        ResultAndSigma(i,cov,a,b,cc,d,e,w,uw);
    }
    //calibration(63,a,b,cc,d,e);
}
