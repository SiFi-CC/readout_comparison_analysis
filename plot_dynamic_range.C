/**
 * plot_dynamic_range.C: ROOT Macro for plotting the collection of ADC vs injected pulse height.
 * 
 * This macro reads a map of vectors filled manually and creates a PDF file for the publication.
 *
 * usage: root -b plot_dynamic_range.C 
 *
 */
void plot_dynamic_range() {
    std::vector<float> reference = {1, 10, 20, 30, 60, 100, 200, 500};
    std::map<std::string, std::vector<float> > _data = {
        {"klaus6b10bitLG", {729, 738, 745, 755, 762, 769, 777, 785} },
        {"twinpeaks", {1049, 1200, 1264, 1313, 1353, 1389, 0, 1449} } //missing one data point
    };
    std::map<std::string, std::vector<float> > _err = {
        {"klaus6b10bitLG", {1, 1, 1, 1, 1, 1, 0, 1} },
        {"twinpeaks", {1, 1, 1, 1, 1, 1, 0, 1} }
    };

    TGraphErrors *g1 = new TGraphErrors(reference.size(), &reference[0], &_data["klaus6b10bitLG"][0], 0, &_err["klaus6b10bitLG"][0]);
    g1->SetTitle("Klaus6b 10bit LG");
    g1->SetLineColor(kRed);
    TGraphErrors *g2 = new TGraphErrors(reference.size(), &reference[0], &_data["twinpeaks"][0], 0, &_err["twinpeaks"][0]);
    g2->SetTitle("TwinPeaks");
    g2->SetLineColor(kBlue);

    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle(";Pulse height[mV_{pp}];ADC");
    mg->Add(g1, "AP");
    mg->Add(g2, "AP");
    TCanvas *c = new TCanvas("c", "c");
    mg->Draw("A");
    c->Draw();
    TLegend *legend = c->BuildLegend(0.58, 0.14, 0.88, 0.35);
    c->SaveAs("plot_dynamic_range.pdf");
}
