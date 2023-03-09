set terminal pdfcairo
set output "spektrum.pdf"
plot "fft_utan.txt" u 1:2 title "Real" w l, \
    "fft_utan.txt" u 1:3 title "Imag" w l, \
    "fft_utan.txt" u 1:4 title "Abs" w l

