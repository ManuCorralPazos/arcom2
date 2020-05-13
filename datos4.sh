gcc -O0 -o 4.out 4.c -lm -fopenmp

echo -e Hilos'\t'N'\t'Ckmedian > 4resultados.txt

for H in 1 2 3 4
do
    for N in 2 4 6 7
    do
		./4.out $N $H>> 4resultados.txt
    done
done
