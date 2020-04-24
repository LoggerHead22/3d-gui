#ifndef APPROX_H
#define APPROX_H

#include <iostream>
#include <QDebug>
#include "mls.h"
using namespace std;

#define qLog(...) qDebug()<< #__VA_ARGS__<< "=" <<(__VA_ARGS__)

int approx(double l1, double l2, double alpha, double k, int nx, int ny, int p, double (*f)(double, double)){
    int *error = new int(0);
    parral par(l1 , l2 , alpha, k , nx, ny);

    double *a, *b, *x , *v , *u , *r , *buf ;
    int * I;
    pthread_t *tids;
    Arg *args;

    int N = (nx + 1)*(ny + 1) - par.nx_rect*par.ny_rect;

    tids= new pthread_t[p];
    args= new Arg[p];
    buf = new double[p];
    x = new double[N];
    u = new double[N];
    v = new double[N];
    r = new double[N];

    memset(x , 0 , N*sizeof(double));
    memset(u , 0 , N*sizeof(double));
    memset(v , 0 , N*sizeof(double));
    memset(r , 0 , N*sizeof(double));
    memset(buf , 0 , p*sizeof(double));

    // pthread_barrier_init (&barrier, nullptr, p);

    for(int i=0;i<p;i++){
        args[i].p=p;
        args[i].thr_ind=i;
        args[i].A=&a;
        args[i].b=&b;
        args[i].I=&I;
        args[i].x=x;
        args[i].u=u;
        args[i].v=v;
        args[i].r=r;
        args[i].buf = buf;
        args[i].error=error;
        args[i].nx = par.nx;
        args[i].ny = par.ny;
        args[i].f = f;
        args[i].par = &par;
    }

    //double TIME = get_full_time();

    cout<<"Lets GO"<<endl;
    for(int i=0;i<p;i++){
        if(pthread_create(tids+i,0,&msl_approx,(void *) (args+i))){
            printf("Cannot create thread %d\n",i);
            delete[] tids;
            delete[] args;
            delete[] x;
            delete[] u;
            delete[] v;
            delete[] r;
            delete error;
            abort();
        }
    }

    for(int i=0;i<p;i++) {
        pthread_join(tids[i],0);
    }

    delete[] tids;
    delete[] args;
    delete[] x;
    delete[] u;
    delete[] v;
    delete[] r;
    delete error;

    return 0;
}

#endif // APPROX_H
