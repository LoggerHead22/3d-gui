#ifndef APPROX_H
#define APPROX_H

#include <iostream>
#include <QDebug>
#include "mls.h"
using namespace std;

#define qLog(...) qDebug()<< #__VA_ARGS__<< "=" <<(__VA_ARGS__)

int approx(int p, pthread_t*& tids, Arg*& args){
    // pthread_barrier_init (&barrier, nullptr, p);

    //double TIME = get_full_time();

    cout<<"Lets GO"<<endl;
    for(int i=0;i<p;i++){
        if(pthread_create(tids+i,0,&msl_approx,(void *) (args+i))){
            printf("Cannot create thread %d\n",i);
            abort();
        }
    }

    for(int i=0;i<p;i++) {
        pthread_join(tids[i],0);
    }

    return 0;
}

#endif // APPROX_H
