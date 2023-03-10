set terminal pdfcairo
set output "filtered_samples.pdf"
set title "Szűrés utáni időtartomány"
set xr [0.000000:0.206601]
plot"samples.txt" u 1:2 t "Real" w l, \
    "samples.txt" u 1:3 t "Imag" w l, \
    "samples.txt" u 1:4 t "Abs" w l