# readout_comparison_analysis

PREREQUISITES

The analysis code provided here are very specific to the readout systems and uses their uniquely written
libraries. In general we use the ROOT framework for visualization and fitting and we use the TTree
datastructure wherever convenient.

klaus6b_channel_mapping.C: Creates a 2d histogram where channels attached to the same fiber can be deduced
by visual inspection. This is important for use in klaus6b_resolution.C

klaus6b_resolution.C: Generates the ADC and time resolution histograms for at most eight fibers. Also generates
the time interval between subsequent hits in a channel for deadtime studies.
