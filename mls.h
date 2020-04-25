#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <fstream>
#include <cassert>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <QVector3D>
#include "triangle.h"
using namespace std;

extern bool allThreadsPokushali;

#define EPS 1e-15

#define LOG(...) std::cout<< #__VA_ARGS__<< " = " <<(__VA_ARGS__)<< "\n"
#define ELOG(...) std::cout << "[" << __FILE__ << ":" << __LINE__ << "] " << #__VA_ARGS__<< " = " <<(__VA_ARGS__)<< "\n"
#define LN       std::cout << "\n";

#define MAX_ITER 10000
#define MAX_ITER_STEP 50


// static pthread_barrier_t barrier;


int get_k(int nx , int ny , int i , int j);
void get_ij(int nx, int ny, int nx_rect, int ny_rect,  int k, int &i , int &j);



struct parral{
public:
	double l1 = 1 , l2 = 1, alpha = 45./(180./M_PI) , k = 0.2 , hx, hy;
	int nx = 10 , ny = 10 , nx_rect = 0 , ny_rect = 0;
    std::vector<double> xs, ys;
	double l1_new , l2_new = l2;
	parral(){};	
	parral(double l1_ , double l2_ , double alpha_ ,  double k_ , int nx_ , int ny_){
		l1 = l1_; l2 = l2_;alpha = alpha_ /(180./M_PI);
		k = k_; nx = nx_; ny = ny_;
		l1_new = sin(alpha)*l1; l2_new = l2;
		hx = l2_new / nx; hy = l1_new / ny;
		nx_rect =k*l2_new/hx;
		ny_rect =k*l1_new/hy;
		
        xs.resize(nx + 1);
        ys.resize(ny + 1);
		compute_grid();
		//cout<<"HH"<<hx<<" "<<hy<<endl;
	}
	
	void compute_grid(){
		for(int i = 0 ; i<=nx; i++){
			xs[i] = hx*i;
		}
		for(int i = 0 ; i<=ny; i++){
			ys[i] = hy*i;
		}
		
	}
	
	
	double f_par(double x, double y , double (*f)(double, double) ){
		return f(x + cos(alpha) / sin(alpha)* y , y);
	}

    QVector3D cord_trans(double  x, double y, double min){
        QVector3D cor (x + cos(alpha) / sin(alpha)*y , min , y);
        return cor;
    }

    vector<Triangle> func_trio(QVector3D eye , double (*f) (double, double), const QPair<double, double>& xRange, const QPair<double, double>& yRange, const QPair<double, double>& zRange, const QVector3D& size) {
        const QVector3D ex = {size.x(), 0, 0};
        const QVector3D ey = {0, size.y(), 0};
        const QVector3D ez = {0, 0, size.z()};

        const QVector3D a = -size / 2;
        const QVector3D b = a + ez;
        const QVector3D c = b + ex;
        const QVector3D d = c - ez;
        const QVector3D e = a + ey;

        const double xDiff = xRange.second - xRange.first;
        const double yDiff = yRange.second - yRange.first;
        const double zDiff = zRange.second - zRange.first;

        // переводит точку из системы координат графика в НСК
        auto toNdc = [&](const QVector3D& p) {
            float x = a.x() + (d.x() - a.x()) * ((p.x() - xRange.first) / xDiff);
            float y = a.y() + (e.y() - a.y()) * ((p.y() - yRange.first) / yDiff);
            float z = a.z() + (b.z() - a.z()) * ((p.z() - zRange.first) / zDiff);
            return QVector3D(x, y, z);
        };

        const int x_size= 63;
        const int y_size = 63;

        double hx_ = l2_new / x_size  , hy_ = l1_new / y_size;
        int x_rect = k*l2_new / (hx_);
        int y_rect = k*l1_new / (hy_);
        //qDebug() << x_rect << y_rect;

        vector<Triangle> trio;
        double cos_ = cos(alpha) , sin_ = sin(alpha);

        int K = (x_size+1)*(y_size + 1) - x_rect*y_rect;
        int i,j;
        for( int k = 0; k < K;k++){
            get_ij(x_size, y_size, x_rect, y_rect, k,i,j);
            if( (i < x_size - x_rect && j < y_size) || (i >= x_size - x_rect && i < x_size && j < y_size - y_rect) ){
                trio.push_back(Triangle(toNdc(QVector3D(i*hx_      + cos_/sin_* hy_*(j   )     , f_par(i*hx_     , hy_*(j   )  , f)    , hy_*(j   ) )) ,
                                        toNdc(QVector3D((i+1)*hx_  + cos_/sin_* hy_*(j+ 1)     , f_par((i+1)*hx_ , hy_*(j+ 1)  , f)    , hy_*(j+ 1) )),
                                        toNdc(QVector3D((i+1)*hx_  + cos_/sin_* hy_*(j   )     , f_par((i+1)*hx_ , hy_*(j   )  , f)    , hy_*(j   ) ))));
                trio.push_back(Triangle(toNdc(QVector3D(i*hx_      + cos_/sin_* hy_*(j   )     , f_par(i*hx_     , hy_*(j   )  , f)    , hy_*(j   ) )) ,
                                        toNdc(QVector3D(i*hx_      + cos_/sin_* hy_*(j+ 1)     , f_par(i*hx_     , hy_*(j+ 1)  , f)    , hy_*(j+ 1) )) ,
                                        toNdc(QVector3D((i+1)*hx_  + cos_/sin_* hy_*(j+ 1)     , f_par((i+1)*hx_ , hy_*(j+ 1)  , f)    , hy_*(j+ 1) ))));

            }
        }
        std::sort(trio.begin(), trio.end(), [&](const Triangle& a, const Triangle& b) {
            return (eye - a.center()).length() > (eye - b.center()).length();
        });
        return trio;

    }

	
};


void coord_trans( double x, double y, double &x_ , double &y_);
void matr_mult_vector(double *A, int *I, double *x, double *b, int p, int k , int N);
void linear_comb(double * x , double *y , double tau , int  p , int k , int N);
double scalar_prod(const double * x , const double *y , double  *buf , int p , int k , int N);
void Jakobi(double *A , double *r , double *v , int p , int k, int N);
int one_solve_step(double *A , int *I , double *x ,  double *b , double * u , double *v, double *r , double *buf, int N,double b_norm,  int p,int k);
double residual_compute(double *x , parral &par, double (*f) (double, double) , double *buf, int p , int k);






int get_num_offdiag(int nx, int ny, int nx_rect, int ny_rect,int k);
int get_non_zeros(int nx, int ny , int nx_rect, int ny_rect);
int get_offdiag_elem( int nx , int ny , int nx_rect, int ny_rect, int k , double *a_diag , double *a , int *I);
int allocate_MSR_matrix(int nx, int ny , double *&p_a , double *&p_I);
int get_offdiag_elem( int nx , int ny , int k , double *a_diag , double *a , int *I , double * b ,double hx, double hy , parral &par , double (*f) (double,double));
void build_MSR_matrix(int nx , int ny, double*a, int *I, double *b, int p , int k , parral &par , double (*f) (double,double));
void* msl_approx(void *in_arg);
void reduce_sum(int p, int *sum = 0);
void print_vector(double* m, int size);
void print_MSR_matrix(double * A , int * I, int N );











struct Arg{
        int nx, ny ,m, p, thr_ind ,*error , **I;
        double **A,**b , *x, *u , *v , *buf , *r , time_thr, fulltime, n_err , *xs , *ys;
		double hx , hy;
		double (*f)(double,double);
		parral* par;
};



