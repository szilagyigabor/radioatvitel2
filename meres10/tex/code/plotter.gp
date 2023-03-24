set terminal pdfcairo
set output "plot.pdf"
set title "Bemeneti IQ mintasorozat"
set yr [0:180]
plot "test.txt" u 1 w l t "I", "test.txt" u 2 w l t "Q", "test.txt" u 3 w l t "Abs", "test.txt" u 4 w l t "Avg"
