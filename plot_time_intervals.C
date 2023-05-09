/**
 * plot_time_interval.C: ROOT Macro for the deadtime for the readout systems
 * 
 * This macro reads a map of vectors filled manually and creates a PDF file for the publication.
 *
 * usage: root -b plot_time_interval.C 
 *
 */
void plot_time_intervals() {
    std::vector<std::string> text = {"klaus6b10bitLG", "twinpeaks", "citirocLG", "tofpet2b", "drs4"};
    std::vector<float> readout = {0, 1, 2, 3, 4, 5};
    // fit sigmoid L / (1 + expo(-k(x-x0) ), read x0 as deadtime
    std::vector<float> _data = {2, 0, 0, 990, 0}; //ns
    std::vector<float> _err = {0, 0, 0, 19, 0};

    TH1F *hInterval = new TH1F("hInterval", ";Readout system;Deadtime[ns]", text.size(), 0, text.size() );
    for(int i=0; i < text.size(); ++i) {
        hInterval->GetXaxis()->SetBinLabel(i+1, Form("%s", text[i].c_str() ) );
        hInterval->SetBinContent(i+1, _data[i]);
        hInterval->SetBinError(i+1, _err[i]);
    }
    gStyle->SetOptStat(0);
    TCanvas *c = new TCanvas("c", "c");
    hInterval->Draw("E");
    hInterval->SetLineWidth(2);
    c->Draw();
    c->SaveAs("plot_time_intervals.pdf");
}
