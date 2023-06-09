/**
 * klaus6b_resolutions.C: ROOT Macro for drawing the geometric mean of the ADC values from the left and right SiPMs.
 * Also reports the ADC and time resolutions.
 *
 * This macro reads the TTree generated by the Klaus6b acquisition software.
 *
 * usage: root -l -e ".L libDAQ_client.so" klaus6b_resolutions.C
 *
 */
void klaus6b_resolutions(std::string filename = "Na22_dac31.root") {
	std::map<int, int> mapCh = {
		{19, 35}, {20, 28}, {21, 29}, {22, 30}, {23, 31}, {24, 32}, {25, 33}, {26, 34}
		//no correlation seen with {23, 31}
	};
	std::vector<int> vecFibers = {19, 20, 21, 22, 23, 24, 25, 26};
	TFile *f = new TFile(filename.c_str(), "READ");
	TTree *t = static_cast<TTree *>(f->Get("aqu_dump") );
	klaus_acquisition* curr_aqu=NULL;
	t->SetBranchAddress("acquisitions", &curr_aqu);
	std::vector<TH1I *> vecADCL;
	std::vector<TH1I *> vecADCR;
	std::vector<TH1I *> vecADC;
	std::vector<TH1I *> vecTimeDiff;
	std::vector<TH1I *> vecTimeInterval;
	for(int i=0; i<vecFibers.size(); ++i) {
		std::string name = "hL" + std::to_string(i);
		std::string title = ";ADC;Counts/ADC";
		vecADCL.push_back(new TH1I(name.c_str(), title.c_str(), 124, 700, 824) );
		name = "hR" + std::to_string(i);
		title = ";ADC;Counts/ADC";
		vecADCR.push_back(new TH1I(name.c_str(), title.c_str(), 124, 700, 824) );
		name = "hAvg" + std::to_string(i);
		title = ";#sqrt{ADC_{L}*ADC_{R} };Counts/ADC";
		vecADC.push_back(new TH1I(name.c_str(), title.c_str(), 124, 700, 824) );
		name = "hT" + std::to_string(i);
		title = ";#Deltat[ps];Counts/200ps";
		vecTimeDiff.push_back(new TH1I(name.c_str(), title.c_str(), 100, -1E4, 1E4) );
	}
	// Two interface boards for Klaus6b exposes 18 available channels for readout
	for(int i=0; i<18; ++i) {
		vecTimeInterval.push_back(new TH1I(Form("hInterval%d", i+18), ";#Deltat[ps]", 5, 0, 1E3) );
	}
	for(int q=0; q < t->GetEntries(); ++q) {
		t->GetEntry(q);
		std::map<unsigned char, std::list<klaus_event> > data = curr_aqu->data;
		std::vector<int> vPrevTime(18, 0);
		for(std::map<unsigned char, std::list<klaus_event> >::iterator it = data.begin(); it != data.end(); it++) {
			std::list<klaus_event> events = it->second;
			std::vector<int> vCh;
			std::vector<int> vADC;
			std::vector<int> vTime;
			for(std::list<klaus_event>::iterator it = events.begin(); it != events.end(); ++it) {
				klaus_event evt = *it;
				vCh.push_back(evt.channel);
				vADC.push_back(evt.ADC_10b);
				vTime.push_back(evt.time);
			}
			for(int i=0; i < vCh.size(); ++i) {
				vecTimeInterval[vCh[i]-18 ]->Fill(vTime[i] - vPrevTime[i]);
				vPrevTime[vCh[i]-18] = vTime[i];
				//consider only channels from one end of the fiber, essentially their labels
				int j = std::distance(vecFibers.begin(), std::find(vecFibers.begin(), vecFibers.end(), vCh[i]) );
				if(j == vecFibers.size() ) continue;
				size_t pos = std::distance(vCh.begin(), std::find(vCh.begin(), vCh.end(), mapCh[vCh[i] ] ) );
				//check for corresponding channel on the other side of the fiber
				if(pos == vCh.size() ) continue;
				vecADCL[j]->Fill(vADC[i]);
				vecADCR[j]->Fill(vADC[pos]);
				vecADC[j]->Fill(TMath::Sqrt(vADC[i]*vADC[pos]) );
				vecTimeDiff[j]->Fill(vTime[i] - vTime[pos]);
			}
		}
	}
	gStyle->SetOptFit(1);
	TCanvas *c = new TCanvas("adc", "adc", 2000, 1000);
	c->Divide(4, 2);
	for(int i=0; i<vecFibers.size(); ++i) {
		TF1 *fit = new TF1("fit", "gaus(0)+expo(3)", 725, 760);
		fit->SetParNames("Constant", "Mean", "Sigma", "Exp. const.", "Slope");
		fit->SetParameters(1.81950e+03, 7.39586e+02, 3.22179e+00, 7.97026e+01,-9.71896e-02);
		c->cd(i+1);
		vecADC[i]->Draw();
		vecADC[i]->SetLineWidth(2);
		vecADC[i]->Fit(fit, "RS");
		vecADC[i]->GetYaxis()->SetMaxDigits(3);
		vecADCR[i]->Draw("SAME");
		vecADCR[i]->SetLineStyle(kDotted);
		vecADCR[i]->SetLineWidth(2);
		vecADCR[i]->GetYaxis()->SetMaxDigits(3);
		vecADCL[i]->Draw("SAME");
		vecADCL[i]->SetLineStyle(kDashed);
		vecADCL[i]->SetLineWidth(2);
		c->GetPad(i+1)->BuildLegend(0.70, 0.26, 0.88, 0.45);
	}
	c->Draw();
	TCanvas *ct = new TCanvas("time", "time", 2000, 1000);
	ct->Divide(4, 2);
	for(int i=0; i<vecFibers.size(); ++i) {
		TF1 *fit = new TF1("fit", "gaus(0)", -1000, 1000);
		fit->SetParameters(1, 0, 200);
		ct->cd(i+1);
		vecTimeDiff[i]->Draw();
		vecTimeDiff[i]->Fit(fit, "RS");
		vecTimeDiff[i]->GetYaxis()->SetMaxDigits(3);
		vecTimeDiff[i]->SetLineWidth(2);
	}
	ct->Draw();
	TCanvas *ct2 = new TCanvas("interval", "interval", 1500, 1500);
	ct2->Divide(5, 4);
	for(int i=0; i<vecTimeInterval.size(); ++i) {
		ct2->cd(i+1);
		vecTimeInterval[i]->Draw();
		vecTimeInterval[i]->GetYaxis()->SetMaxDigits(3);
		vecTimeInterval[i]->SetLineWidth(2);
	}
	ct2->Draw();
}
