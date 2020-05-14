#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <immintrin.h>

struct quaternion{
    double uno;
    double dos;
    double tres;
    double cuatro;
};

/* Initialize the cycle counter */

 static unsigned cyc_hi = 0;
 static unsigned cyc_lo = 0;


 /* Set *hi and *lo to the high and low order bits of the cycle counter.
 Implementation requires assembly code to use the rdtsc instruction. */
 void access_counter(unsigned *hi, unsigned *lo){
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1" /* Read cycle counter */
        : "=r" (*hi), "=r" (*lo) /* and move results to */
        : /* No input */ /* the two outputs */
        : "%edx", "%eax");
 }

 /* Record the current value of the cycle counter. */
 void start_counter(){
    access_counter(&cyc_hi, &cyc_lo);
 }

 /* Return the number of cycles since the last call to start_counter. */
 double get_counter(){
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
        fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}

double mhz(int verbose, int sleeptime){
    double rate;
    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6*sleeptime);
    if (verbose)
    //printf("%.1f\n", rate);
    return rate;
}
int cmpfunc (const void * a, const void * b){
  if (*(double*)a > *(double*)b)
    return 1;
  else if (*(double*)a < *(double*)b)
    return -1;
  else
    return 0;
}
double median(int n, double x[]) {
    qsort(x,n,sizeof(double), cmpfunc);
    for(int i=0;i<n;i++)
	//printf("i=%d, %lf\n",i,x[i]);
    if(n%2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        return((x[n/2] + x[n/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        return x[n/2];
    }
}

void inicializarACero(struct quaternion *a, long int N){
    for(long int i=0; i<N; i++){
        a[i].uno=0.0;
        a[i].dos=0.0;
        a[i].tres=0.0;
        a[i].cuatro=0.0;
    }
    return;
}
void inicializarAleatorio(struct quaternion *a, long int N){
    double num=0.0;
    for(long int i=0; i<N; i++){
        num=drand48();
        a[i].uno=num;
        num=drand48();
        a[i].dos=num;
        num=drand48();
        a[i].tres=num;
        num=drand48();
        a[i].cuatro=num;
    }
    return;
}
void imprimirQuaternion(struct quaternion *a, long int N){
    for(long int i=0; i<N; i++){
        printf("(%lf, %lf, %lf, %lf)\n", a[i].uno, a[i].dos, a[i].tres, a[i].cuatro);
    }
    return;
}

struct quaternion sumarQuaternion(struct quaternion a, struct quaternion b){
    struct quaternion r;
    r.uno=a.uno+b.uno;
    r.dos=a.dos+b.dos;
    r.tres=a.tres+b.tres;
    r.cuatro=a.cuatro+b.cuatro;
    return r;
}

struct quaternion multiplicarQuaternion(struct quaternion a, struct quaternion b){
    struct quaternion r;
    r.uno=a.uno*b.uno-a.dos*b.dos-a.tres*b.tres-a.cuatro*b.cuatro;
    r.dos=a.uno*b.dos+a.dos*b.uno+a.tres*b.cuatro-a.cuatro*b.tres;
    r.tres=a.uno*b.tres-a.dos*b.cuatro+a.tres*b.uno+a.cuatro*b.dos;
    r.cuatro=a.uno*b.cuatro+a.dos*b.tres-a.tres*b.dos+a.cuatro*b.uno;
    return r;
}

struct quaternion operacionQuaternion(struct quaternion dp, struct quaternion c){
    struct quaternion r;
    r.uno=dp.uno+(c.uno*c.uno-c.dos*c.dos-c.tres*c.tres-c.cuatro*c.cuatro);
    r.dos=dp.dos+(2.0*c.uno*c.dos);
    r.tres=dp.tres+(2.*c.uno*c.tres);
    r.cuatro=dp.cuatro+(2*c.uno*c.cuatro);
    return r;
}


int main(int argc, char **argv){
    printf(" ");
    //srand48(time(NULL));  
    srand48(1);
    long int N=1;
    int q=0;
    int vez=10;
    double ck[vez];
    if(argc>1){
        q=atoi(argv[1]);
    }
    N=pow(10, q);
    printf("%ld\n", N);
       
    struct quaternion *__restrict__ a=NULL;
     printf("%ld\n", N);
    
    struct quaternion *__restrict__ b=NULL;
    struct quaternion *__restrict__ dp=NULL;
    
    a=(struct quaternion*)malloc(N*sizeof(struct quaternion));
    b=(struct quaternion*)malloc(N*sizeof(struct quaternion));
    printf("%ld\n", N);
    inicializarAleatorio(a, N);
    inicializarAleatorio(b, N);
    printf("%ld\n", N);
    dp=(struct quaternion*)malloc(sizeof(struct quaternion));
    inicializarACero(dp, 1);
    //printf("a:\n");
    //imprimirQuaternion(a, N);
    //printf("b:\n"),
    //imprimirQuaternion(b, N);   
    mhz(1, 1);
    //Computacion
    for(vez=0; vez<10; vez++){
        start_counter();
        inicializarACero(dp, 1);
        //for(int i=0; i<N; i++){
          //  *dp=operacionQuaternion(*dp, multiplicarQuaternion(a[i], b[i]));
        //}
        __m256d dUno;
        __m256d dDos;
        __m256d dTres;
        __m256d dCuatro;
        for(int i=0; i<N; i+=4){
            __m256d aUno=_mm256_set_pd(a[i+3].uno, a[i+2].uno, a[i+1].uno, a[i].uno);
            __m256d aDos=_mm256_set_pd(a[i+3].dos, a[i+2].dos, a[i+1].dos, a[i].dos);
            __m256d aTres=_mm256_set_pd(a[i+3].tres, a[i+2].tres, a[i+1].tres, a[i].tres);
            __m256d aCuatro=_mm256_set_pd(a[i+3].cuatro, a[i+2].cuatro, a[i+1].cuatro, a[i].cuatro);

            __m256d bUno=_mm256_set_pd(b[i+3].uno, b[i+2].uno, b[i+1].uno, b[i].uno);
            __m256d bDos=_mm256_set_pd(b[i+3].dos, b[i+2].dos, b[i+1].dos, b[i].dos);
            __m256d bTres=_mm256_set_pd(b[i+3].tres, b[i+2].tres, b[i+1].tres, b[i].tres);
            __m256d bCuatro=_mm256_set_pd(b[i+3].cuatro, b[i+2].cuatro, b[i+1].cuatro, b[i].cuatro);
        
            __m256d cUno=_mm256_mul_pd(aDos, bDos);
            cUno=_mm256_fmadd_pd(aTres, bTres, cUno);
            cUno=_mm256_fmadd_pd(aCuatro, bCuatro, cUno);
            cUno=_mm256_fmsub_pd(aUno, bUno, cUno);

            //cUno=_mm256_add_pd(aTres, bTres);
            //cUno=_mm256_add_pd(aCuatro, bCuatro);
            //cUno=_mm256_sub_pd(aUno, bUno);

            __m256d cDos=_mm256_mul_pd(aTres, bTres);
            cDos=_mm256_fmadd_pd(aUno, bDos, cDos);
            cDos=_mm256_fmadd_pd(aDos, bUno, cDos);
            cDos=_mm256_fmsub_pd(aTres, bCuatro, cDos);

            //cDos=_mm256_add_pd(aUno, bDos);
            //cDos=_mm256_add_pd(aDos, bUno);
            //cDos=_mm256_sub_pd(aTres, bCuatro);

            __m256d cTres=_mm256_mul_pd(aDos, bCuatro);
            cTres=_mm256_fmadd_pd(aUno, bTres, cTres);
            cTres=_mm256_fmadd_pd(aTres, bUno, cTres);
            cTres=_mm256_fmsub_pd(aCuatro, bDos, cTres);

            //cTres=_mm256_add_pd(aUno, bTres);
            //cTres=_mm256_add_pd(aTres, bUno);
            //cTres=_mm256_sub_pd(aCuatro,bDos);

            __m256d cCuatro=_mm256_mul_pd(aTres, bDos);
            cCuatro=_mm256_fmadd_pd(aUno, bCuatro, cCuatro);
            cCuatro=_mm256_fmadd_pd(aDos, bTres, cCuatro);
            cCuatro=_mm256_fmsub_pd(aCuatro, bUno, cCuatro);

            //cCuatro=_mm256_add_pd(aUno, bCuatro);
            //cCuatro=_mm256_add_pd(aDos, bTres);
            //cCuatro=_mm256_sub_pd(aCuatro, bUno);

            __m256d eUno=_mm256_mul_pd(cDos, cDos);
            eUno=_mm256_fmadd_pd(cTres, cTres, eUno);    
            eUno=_mm256_fmadd_pd(cCuatro, cCuatro, eUno);
            eUno=_mm256_fmsub_pd(cUno, cDos, eUno);

            //eUno=_mm256_add_pd(cTres, cTres);
            //eUno=_mm256_add_pd(cCuatro, cCuatro);
            //eUno=_mm256_add_pd(cUno, cDos);
          
            dUno=_mm256_add_pd(eUno, dUno);
            
            __m256d eDos=_mm256_mul_pd(cUno, cDos);
            eDos=_mm256_add_pd(eDos, eDos);
            dDos=_mm256_add_pd(eDos, dDos);

            __m256d eTres=_mm256_mul_pd(cUno, cTres);
            eTres=_mm256_add_pd(eTres, eTres);
            dTres=_mm256_add_pd(eTres, dTres);

            __m256d eCuatro=_mm256_mul_pd(cUno, cCuatro);
            eCuatro=_mm256_add_pd(eCuatro, eCuatro);
            dCuatro=_mm256_add_pd(eCuatro, dCuatro);
        }
    
        __m256d suma12=_mm256_add_pd(dUno, dDos);
        __m256d suma34=_mm256_hadd_pd(dTres, dCuatro);

        __m128d aux12 =_mm256_castpd256_pd128(suma12);
        __m256d parcial1=_mm256_insertf128_pd(suma34, aux12, 0);

        __m256d parcial2=_mm256_permute2f128_pd(suma12, suma34, 0x12);

        __m256d r=_mm256_add_pd(parcial1,parcial2);

        _mm256_store_pd((double*)dp, r);

        ck[vez]=get_counter();
    }
    
    //Fin computacion
    //printf("dp:\n");
    //imprimirQuaternion(dp, 1);
    double ckmedian=median(10, ck);
    printf("%ld\t%lf\n",N, ckmedian/(double) N); 

}