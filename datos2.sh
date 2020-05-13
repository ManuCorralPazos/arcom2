gcc -O0 -o 2.out 2.c -lm

echo -e N'\t'Ckmedian > 2resultados.txt


for N in 2 4 6 7
do
		./2.out $N >> 2resultados.txt
done
