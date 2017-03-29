Rscript ultimate_throughput_plot.R
Rscript ultimate_rtt_plot.R
BLA=$(date +"%m-%d-%y-%h-%m-%s")
mv new_result_thput.pdf "throughput_result_"${BLA}".pdf"
mv new_result_rtt.pdf "rtt_result_"${BLA}".pdf"
