gcc -O0 -o 1.out 1.c -lm

echo -e N'\t'Ckmedian > 1resultados.txt


for N in 2 4 6 7
do
		./1.out $N >> 1resultados.txt
done
