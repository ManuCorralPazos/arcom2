#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

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
    long int N=0;
    int q=0;
    int vez=10;
    double ck[vez];
    if(argc>1){
        q=atoi(argv[1]);
    }
    N=pow(10, q);

    struct quaternion *a=NULL;
    struct quaternion *b=NULL;
    struct quaternion *dp=NULL;
    a=(struct quaternion*)malloc(N*sizeof(struct quaternion));
    b=(struct quaternion*)malloc(N*sizeof(struct quaternion));
    inicializarAleatorio(a, N);
    inicializarAleatorio(b, N);
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
        for(int i=0; i<N; i+=5){
            *dp=operacionQuaternion(*dp, multiplicarQuaternion(a[i], b[i]));
            *dp=operacionQuaternion(*dp, multiplicarQuaternion(a[i+1], b[i+1]));
            *dp=operacionQuaternion(*dp, multiplicarQuaternion(a[i+2], b[i+2]));
            *dp=operacionQuaternion(*dp, multiplicarQuaternion(a[i+3], b[i+3]));
            *dp=operacionQuaternion(*dp, multiplicarQuaternion(a[i+4], b[i+4]));
        }
        ck[vez]=get_counter();
    }
    
    //Fin computacion
    //printf("dp:\n");
    //imprimirQuaternion(dp, 1);
    double ckmedian=median(10, ck);
    printf("%ld\t%lf\n",N, ckmedian/(double) N); 

}