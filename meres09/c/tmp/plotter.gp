set terminal pdfcairo
set output "output/correlated_samples.pdf"
set title "Korrelálás utáni időtartomány"
set xr [-0.010239:0.215020]
plot "tmp/samples.txt" u 1:2 t "Real" w l,\
     "tmp/samples.txt" u 1:3 t "Imag" w l,\
