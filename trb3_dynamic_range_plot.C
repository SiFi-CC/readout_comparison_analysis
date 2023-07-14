// This code creates histogram of ToT for trb3 with 100 kHz signal in elog 254
// Main peaks with uncertanties (sigma of gaus) are y and y_err, graph g, legend
// Seondary peaks are s and s_err, graph g_s, legend_s
void graph()
{
        //TCanvas *c = New TCanvas("dynamic range ch49", "dynamic range");
        const int N = 20;
        float x[N] = {0};
        for(int i = 0; i < N; i++)
        {
                x[i] = 50 + 25*i;
        }
        //float y[N] = {487.3, 564.0, 641.4, 719.1, 795.7, 870.4, 940.8, 1009.3, 1073.3, 1132.2, 1168.7, 1184.0, 1196.2, 1205.5, 1214.2, 1222.7, 1229.4, 1237.8, 1243.2};
        //float y_er[N] = {3.3, 3.2, 2.9, 2.9, 2.9, 2.8, 3.3, 3.3, 3.4, 3.1, 4.3, 3.1, 4.6, 4.5, 4.3, 3.3, 3.0, 4.2, 3.0};
        float s[N] = {338.3, 365.5, 389.4, 426.2, 451.1, 464.9, 474.1, 482.8, 490.7, 499.2, 508.3, 516.6, 525.7, 534.6, 544.6,555.7,  571.7, 593.4, 619.8};
        float s_err[N] = {3.5, 3.9, 3.7, 4.2, 4.3, 4.2, 4.1, 4.1, 4.0, 3.6, 4.2, 4.2, 3.8, 4.0, 3.8, 4.0, 3.7, 3.5, 3.3};
        //auto g  = new TGraphErrors(N, x ,y,0,y_er);
        //g->GetXaxis()->SetTitle("Amplitude [mV]");
        //g->GetYaxis()->SetTitle("TOT [ns]");
        //g->GetYaxis()->SetRangeUser(400,1250);
        //g->GetXaxis()->SetLimits(30,525);
        //g->Draw("AP*"); 
        //auto f = new TF1("linfit", "[0]*x + [1]", 50, 300);
        //g->Fit(f,"R");

        //auto legend = new TLegend(0.1,0.7,0.48,0.9);
        //if(f->GetParameter(1) >0) {legend->AddEntry(f,TString::Format("%.3fx +%.1f", f->GetParameter(0),f->GetParameter(1)));}                
        //else{legend->AddEntry(f,TString::Format("%.3fx +%.1f", f->GetParameter(0),f->GetParameter(1)));}
        //legend->Draw();
        auto g_s = new TGraphErrors(N,x,s,0,s_err);
        g_s->SetTitle("Secondary peaks");
        g_s->GetXaxis()->SetTitle("Amplitude [mV]");
        g_s->GetYaxis()->SetTitle("ToT [ns]");
        g_s->GetYaxis()->SetRangeUser(300, 640);
        g_s->GetXaxis()->SetLimits(25, 525);
        g_s->Draw("AP*");

}
