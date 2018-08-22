set terminal png
set size ratio -1
set output "../output.png"
plot  "circle_30_30_15.txt" notitle with points pointsize 0.2 linecolor rgb "red", \
"circle_80_40_20.txt" notitle with points pointsize 0.2 linecolor rgb "blue",
