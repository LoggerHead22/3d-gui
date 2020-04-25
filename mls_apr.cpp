#include "mls.h"
#include <QDebug>


bool allThreadsPokushali = false;

const double tetta = 0.7;

void coord_trans( double x, double y, double &x_ , double &y_){
	
	x_ = x + tetta *y;
	y_ = y;
	
}


void print_MSR_matrix(double * A , int * I, int N ){
	int row_ind= 0;
	int row_diff;
	for(int i = 0 ; i < N - 1; i++){
		row_ind = I[i];
		row_diff = I[i + 1] - I[i];
		for(int k = 0; k < N; k++){
			if (i==k)  cout<<A[i]<<"\t";
			
			if(k == I[I[i] + I[i+1] - I[i] - row_diff]){
				cout<<A[row_ind + k]<<"\t";
				row_diff--;
			}else{ cout<< 0<<"\t"; }
		
		}
		cout<<endl;
	}
	row_ind = I[N-1];
	row_diff = I[N -1];
	cout<<A[N-1]<<"\t";
	
	for( int k = 0; k < N; k++){
		if(k == I[row_diff]){
			cout<<A[row_ind + row_diff]<<"\t";
			row_diff++;
		}else{
			cout<<0<<"\t";
		}
	
	}
}

void print_matrix (int N, double *a, int *I)
{
  int pos;
  int q;
  int pr_n = N > 16? 16: N;
  for (int i = 0; i < pr_n; i++)
    {
      pos = I[i];
      int raw_size = I[i+1] - I[i];
      for (int j = 0; j < pr_n; j++)
        {
          if (i != j)
            {
              for (q = 0; q < raw_size; q++)
                {
                  if (I[pos + q] == j)
                    {
                      printf ("%.3f  \t", a[pos + q]);
                      break;
                    }
                }


              if (q == raw_size)
                {
                  printf ("%0.f  \t", 0.);
                }
            }
          else
            {
              printf ("%.3f  \t", a[i]);
            }
        }
      printf ("\n");
    }
}


void print_vector(double* m, int size) {
	int N = 16;
	for (int y = 0; y < ((size < N) ? size : N); y++) {
		cout << m[y] << "  ";
	}
	cout << endl;
}





int get_k(int  nx  , int ny ,int nx_rect, int ny_rect, int i , int j){
	
	return (i <= nx - nx_rect ? i*(ny + 1) + j: (nx + 1 - nx_rect)*(ny + 1) + (i - (nx + 1 - nx_rect))*(ny + 1- ny_rect) + j);
}	


void get_ij(int  nx , int ny, int nx_rect , int ny_rect , int k, int &i , int &j){
	if(k < (ny+1)*(nx + 1 - nx_rect)){
		i = k / (ny + 1);
		j = k - i*(ny + 1);
	}else{
		int l =(k - (ny + 1)*(nx + 1 - nx_rect));
		i = (nx + 1 - nx_rect) +  l / (ny + 1 - ny_rect);
		j = l - (i - (nx + 1 - nx_rect))*(ny + 1 - ny_rect);
	}
}

int get_num_offdiag(int nx, int ny,int nx_rect , int ny_rect , int k){
	int i,j;
	get_ij(nx,ny, nx_rect , ny_rect , k,i,j);

	if( i >=1 && j>=1 && ((i < nx - nx_rect && j < ny)||( i<=nx -1 && j< ny - ny_rect))){
		return 6;
	}else if( (i ==0 && j ==0) || (i == nx && j ==ny - ny_rect)|| (i==nx - nx_rect && j==ny)){
		return  3;
	}else if( i == nx - nx_rect && j== ny - ny_rect){
		return 5;
	}else if( (i ==0 && j ==ny) || (i == nx && j == 0)){
		return 2;
	}else if( (i >=1 && i <=nx -1) || (j >=1 && j <=ny-1)){
		return  4;
	}
	
	cout<<k<<" "<<i<<" "<<j<<endl;
	cout<<"get_num_offdiag"<<endl;
	
	abort();
	return -1000;
	
}

	
int get_non_zeros(int nx, int ny , int nx_rect, int ny_rect){
	int K = (nx + 1)*(ny + 1) - nx_rect*ny_rect;
	
	int nz = 0;
	//LOG(K);
	
	for(int k = 0; k < K; k++){
		nz += get_num_offdiag(nx,ny , nx_rect, ny_rect, k);
		//cout<<"nz "<<nz<<endl;
	}
	return nz;
}

//total_size = K + 1 + nz 

int get_offdiag_elem( int nx , int ny , int k , double *a_diag , double *a , int *I , double * b , parral &par , double (*f) (double,double)){
	int i, j; 
	double hx = par.hx, hy = par.hy;
	int nx_rect = par.nx_rect , ny_rect = par.ny_rect;
	
	get_ij(nx,ny, nx_rect, ny_rect, k,i,j);
	
	#define I(x,y) get_k(nx , ny, nx_rect, ny_rect, x , y)
	
	#define _36_1 par.f_par(par.xs[i] , par.ys[j] ,f)
	
	#define _20_1 par.f_par(par.xs[i] , (par.ys[j + 1] + par.ys[j])/2, f)
	#define _20_2 par.f_par((par.xs[i + 1] + par.xs[i])/2 , (par.ys[j + 1] + par.ys[j])/2, f)
	#define _20_3 par.f_par((par.xs[i + 1] + par.xs[i])/2,par.ys[j] , f)
	#define _20_4 par.f_par(par.xs[i] , (par.ys[j - 1] + par.ys[j])/2, f)
	#define _20_5 par.f_par((par.xs[i - 1] + par.xs[i])/2 ,(par.ys[j - 1] + par.ys[j])/2, f)
	#define _20_6 par.f_par((par.xs[i - 1] + par.xs[i])/2,par.ys[j] , f)

	#define _4_1  par.f_par((par.xs[i] + par.xs[i+1])/2 , par.ys[j+1] ,f)
	#define _4_2  par.f_par( par.xs[i+1] , (par.ys[j+1] + par.ys[j])/2 ,f)
	#define _4_3  par.f_par((par.xs[i + 1] + par.xs[i]) / 2, (par.ys[j] + par.ys[j-1]) / 2 , f)
	#define _4_4  par.f_par((par.xs[i-1] + par.xs[i])/2, par.ys[j-1], f)
	#define _4_5  par.f_par(par.xs[i-1], (par.ys[j] + par.ys[j-1]) / 2 , f)
	#define _4_6  par.f_par((par.xs[i - 1] + par.xs[i]) / 2, (par.ys[j] + par.ys[j + 1]) / 2 , f)

	#define _2_1  par.f_par(par.xs[i]     ,par.ys[j + 1], f)
	#define _2_2  par.f_par(par.xs[i + 1] ,par.ys[j + 1], f)
	#define _2_3  par.f_par(par.xs[i + 1] ,par.ys[j]    , f)
	#define _2_4  par.f_par(par.xs[i]     ,par.ys[j - 1], f)
	#define _2_5  par.f_par(par.xs[i - 1] ,par.ys[j - 1], f)
	#define _2_6  par.f_par(par.xs[i - 1] ,par.ys[j]    , f)
	
	
	if( i >=1 && j>=1 && ((i < nx - nx_rect && j < ny)||( i<=nx -1 && j< ny - ny_rect))){
		*a_diag = 0.5*hx*hy;
		 I[0] = I(i-1,j-1); a[0] = 1./12*hx*hy;
		 I[1] = I(i-1,j);   a[1] = 1./12*hx*hy;
		 I[2] = I(i,j-1);   a[2] = 1./12*hx*hy;
		 I[3] = I(i,j+1);   a[3] = 1./12*hx*hy;
		 I[4] = I(i+1,j);   a[4] = 1./12*hx*hy;
		 I[5] = I(i+1,j+1); a[5] = 1./12*hx*hy;
		 b[I(i,j)] = (36*_36_1 + 
				20*(_20_1 + _20_2 + _20_3 +  _20_4 + _20_5 + _20_6)+
				4*(_4_1 + _4_2 + _4_3 + _4_4 + _4_5 + _4_6) +
				2*(_2_1 + _2_2 + _2_3 + _2_4 + _2_5 + _2_6) )* hx*hy /192;;
				
		return 6;
    }else if ((i==nx && j==ny) || (i==nx && j==ny - ny_rect) || (i==nx - nx_rect && j ==ny)){
		*a_diag = 1./6*hx*hy;
		I[0] = I(i-1,j-1);   a[0] = 1./12*hx*hy; 
		I[1] = I(i-1,j);     a[1] = 1./24*hx*hy;
		I[2] = I(i,j-1);     a[2] = 1./24*hx*hy;
		b[I(i,j)] = (12*_36_1 + 20*(_20_5) + 10*(_20_4 + _20_6) +
					+ 4*(_4_4 + _4_5) + 2*(_2_5) + 1*(_2_4 + _2_6))*hx*hy/192;
		return 3;
	}else if(i == nx - nx_rect && j == ny - ny_rect){
		*a_diag = 1./3*hx*hy;
		I[0] = I(i-1,j-1);   a[0] = 1./12*hx*hy;
		I[1] = I(i-1,j);     a[1] = 1./12*hx*hy;
		I[2] = I(i,j-1);     a[2] = 1./12*hx*hy;
		I[3] = I(i,j+1);     a[3] = 1./24*hx*hy;
		I[4] = I(i+1,j);     a[4] = 1./24*hx*hy;
		b[I(i,j)] = (24*_36_1 + 20*(_20_4 + _20_5 + _20_6) + 10*(_20_1 + _20_3) +
					+ 4*(_4_6 + _4_3 + _4_4 +  _4_5) + 2*(_2_4 + _2_5 + _2_6) + 1*(_2_1 + _2_3))*hx*hy/192;
		return 5;
		
	}else if( (i >=1 && i <=nx -1 && j==0)){
		*a_diag = 0.25*hx*hy;
		I[0] = I(i-1,j);   a[0] = 1./24*hx*hy;
		I[1] = I(i,j+1);   a[1] = 1./12*hx*hy;
		I[2] = I(i+1,j);   a[2] = 1./24*hx*hy;
		I[3] = I(i+1,j+1); a[3] = 1./12*hx*hy;
		b[I(i,j)] = (18*_36_1 + 20*(_20_1 + _20_2) + 10*(_20_6 + _20_3) +
					+ 4*(_4_6 + _4_1 + _4_2) + 2*(_2_1 + _2_2) + 1*(_2_6 + _2_3))*hx*hy/192;
		return 4;
	}else if( (i >=1 && i <=nx -1 && (j==ny || j== ny - ny_rect))){
		*a_diag = 0.25*hx*hy;
		I[0] = I(i-1,j-1);   a[0] = 1./12*hx*hy;
		I[1] = I(i-1,j);     a[1] = 1./24*hx*hy;
		I[2] = I(i,j-1);     a[2] = 1./12*hx*hy;
		I[3] = I(i+1,j);   a[3] = 1./24*hx*hy;
		b[I(i,j)] = (18*_36_1 + 20*(_20_5 + _20_4) + 10*(_20_6 + _20_3) +
					+ 4*(_4_5 + _4_4 + _4_3) + 2*(_2_5 + _2_4) + 1*(_2_6 + _2_3))*hx*hy/192;
		return 4;
	}else if( (i ==0 && j >=1 && j<=ny-1)){
		*a_diag = 0.25*hx*hy;
		I[0] = I(i,j-1);     a[0] = 1./24*hx*hy;
		I[1] = I(i,j+1);     a[1] = 1./24*hx*hy;
		I[2] = I(i+1,j);     a[2] = 1./12*hx*hy;
		I[3] = I(i+1,j+1);   a[3] = 1./12*hx*hy;
		b[I(i,j)] = (18*_36_1 + 20*(_20_2 + _20_3) + 10*(_20_1 + _20_4) +
					+ 4*(_4_1 + _4_2 + _4_3) + 2*(_2_2 + _2_3) + 1*(_2_1 + _2_4))*hx*hy/192;
		return 4; 
	}else if( ((i ==nx || i == nx - nx_rect) && j>=1 && j <= ny - 1)){
		*a_diag = 0.25*hx*hy;
		I[0] = I(i-1,j-1);   a[0] = 1./12*hx*hy;
		I[1] = I(i-1,j);     a[1] = 1./12*hx*hy;
		I[2] = I(i,j-1);     a[2] = 1./24*hx*hy;
		I[3] = I(i,j+1);     a[3] = 1./24*hx*hy;
		b[I(i,j)] = (18*_36_1 + 20*(_20_5 + _20_6) + 10*(_20_1 + _20_4) +
					+ 4*(_4_4 + _4_5 + _4_6) + 2*(_2_5 + _2_6) + 1*(_2_1 + _2_4))*hx*hy/192;
		return 4;
	}else if (i==0 && j==0){
		*a_diag = 1./6*hx*hy;
		I[0] = I(i,j+1);   a[0] = 1./24*hx*hy;
		I[1] = I(i+1,j);   a[1] = 1./24*hx*hy;
		I[2] = I(i+1,j+1); a[2] = 1./12*hx*hy;
		b[I(i,j)] = (12*_36_1 + 20*(_20_2) + 10*(_20_1 + _20_3) +
					+ 4*(_4_1 + _4_2) + 2*(_2_2) + 1*(_2_1 + _2_3))*hx*hy/192;
		return 3;	
	}else if (i==nx && j==0){
		*a_diag = 1./12*hx*hy;
		I[0] = I(i-1,j); a[0] = 1./24*hx*hy;
		I[1] = I(i,j+1); a[1] = 1./24*hx*hy;
		b[I(i,j)] = (6*_36_1 + 10*(_20_1 + _20_6) +
					+ 4*(_4_6) +  1*(_2_1 + _2_6))*hx*hy/192;
		return 2;
	}else if (i==0 && j==ny){
		*a_diag = 1./12*hx*hy;
		I[0] = I(i , j-1);   a[0] = 1./24*hx*hy;
		I[1] = I(i + 1,j);   a[1] = 1./24*hx*hy;
		b[I(i,j)] = (6*_36_1 + 10*(_20_3 + _20_4) +
					+ 4*(_4_3) +  1*(_2_3 + _2_4))*hx*hy/192;
		return 2;
	}
		
		
	cout<<"get_offdiag_elem"<<endl;
	abort();
	  
	return -1000;
	
}

int allocate_MSR_matrix(int nx, int ny ,int nx_rect, int ny_rect,  double *&p_a , int *&p_I){
	double *a;
	int *I;
	
	
	int N = (nx + 1)*(ny+1) - nx_rect*ny_rect;
	int k,l,s = 0;
	int nz = get_non_zeros(nx , ny , nx_rect , ny_rect);
	
	//cout<<"nz "<<nz<< " "<<nx <<" "<<ny<<endl;
	
	int len = N + 1 +nz;
	
	a = new double[len];
	if(!a) return -1;
	I = new int[len];
	if(!I) {
		delete []a; return -2;
	}


	I[0] = N + 1;
	
	for(k =1 ; k <=N; k++){
		l = get_num_offdiag(nx , ny ,nx_rect , ny_rect,  k - 1);
		I[k] = I[k -1] + l;
		s+=l;
		//cout<<"L " << l <<" k"<<k -1<<endl;
	}
	//cout<<"COUNT "<<nz <<" "<<s<<" "<<I[N]<<" "<<len;
	assert(nz == s);
	assert(I[N] == len);
	p_a = a;
	p_I = I;
	
	
	return 0;
} 


void build_MSR_matrix(int nx , int ny, double*a, int *I, double *b, int p , int k , parral &par , double (*f) (double,double)){
	int k1 , k2 , s , sum = 0;
	int N = (nx + 1) * (ny + 1) - par.nx_rect*par.ny_rect;
	k1 = k*N / p;
	k2 = (k + 1)* N / p ;
	
	for(int  l = k1; l < k2; l++){
		s = get_offdiag_elem(nx , ny , l, a + l , a + I[l] , I + I[l] , b , par, f);
		sum+=s;
		// cout<<"thr: "<<k<< "L " << s <<" k"<< l<<endl;
	}
//    cout<<"N "<<N<<" "<<sum<<" "<<k1<<" "<<k2<<endl;

	reduce_sum( p , &sum);
	//pthread_barrier_wait(&barrier);

	assert( N + 1 + sum == I[N]);	
}

void reduce_sum(int p, int *sum){
    static pthread_mutex_t mu=PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in=PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out=PTHREAD_COND_INITIALIZER;

    static int t_in=0;
    static int t_out=0;

	static int sums = -1;
	
    pthread_mutex_lock(&mu);
	if(sum){
		if(sums == -1){
			sums = *sum;
		}else{
			sums+=*sum;
		}
	}
    t_in++;

    if(t_in>=p){
            t_out=0;
            pthread_cond_broadcast(&c_in);
    }else while(t_in<p) pthread_cond_wait(&c_in,&mu);
	
	if(sum){
		*sum = sums;
	}
	
    t_out++;
    if(t_out>=p){
			t_in = 0;
            sums = -1;
            pthread_cond_broadcast(&c_out);
    }else while(t_out<p) pthread_cond_wait(&c_out,&mu);
    pthread_mutex_unlock(&mu);
}










void* msl_approx(void *in_arg) {
    Arg& arg=*((Arg *) in_arg);
  //  static pthread_mutex_t mu=PTHREAD_MUTEX_INITIALIZER;
   // pthread_mutex_lock(&mu);

	/*
    int n_cpus = get_nprocs();

//   cpu_set_t& cpu = *arg.cpu;
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(n_cpus - arg.ind -1,&cpu);
    pthread_setaffinity_np(pthread_self(),sizeof(cpu),&cpu);

	*/
	
	
  //  pthread_mutex_unlock(&mu);
    const int nx = arg.nx;
    const int ny = arg.ny;
	

    const int p = arg.p;
    const int thr_ind = arg.thr_ind;
	parral& par = *(arg.par);

    const int nx_rect = par.nx_rect;
    const int ny_rect = par.ny_rect;
	
	const int N = (nx + 1)*(ny + 1) - nx_rect*ny_rect;
		
	double (*f)(double,double) = arg.f;


    double *&A = *arg.A , *&b = *arg.b;
    int* err = arg.error , *&I = *arg.I;
	double *x = arg.x, *u = arg.u , *v = arg.v , *r = arg.r , *buf = arg.buf;

	if(thr_ind == 0) {
		//cout<<"Im in "<<endl;
		if(allocate_MSR_matrix(nx, ny,nx_rect , ny_rect, A, I)!= 0){
			//cout << thr_ind << ": " << I << "\n";
			//cout<<"very bad"<<endl;
			*err = - 1;
		}
		b = new double[N];
		memset(b,0,N*sizeof(double));
	}

	
	reduce_sum(p);


	build_MSR_matrix(nx , ny , A , I, b, p, thr_ind , par, f);
	reduce_sum(p);

	
	
	
	if(thr_ind == 0){
		cout<<"SPARCE MATRIX"<<endl;
		print_matrix(N , A, I);
		//cout<<"HH"<<hx<<" "<<hy<<endl;
		cout<<endl<<"Vector b: "<<endl;
		print_vector(b , N);
	}

		
	//ITERATION PART 
	
	double b_norm = scalar_prod(b,b,buf,p,thr_ind,N);
	
	
	int iter_count = 0;
	bool not_solved = true;
	int temp;
	
	reduce_sum(p);
	while(not_solved){
		temp = one_solve_step(A,I, x , b,u,v,r,buf, N, b_norm , p , thr_ind);
	
		if(temp == -1){
			iter_count+=MAX_ITER_STEP;
		}else{
			iter_count+=temp;
			break;
		}
		
		
		if(iter_count>MAX_ITER){
			cout<<"SORRY, the number of iterations exceeded : "<<iter_count<<endl;
			iter_count=-1;
			break;
		}
	
	}

	
	
	
	//OUTPUT PART 
	
	
	
	if(thr_ind == 0){
		if(iter_count > 0){
			cout<<endl<<"Vector x: "<<endl;
			print_vector(x , N); 
			
			cout<<"\nITER_COUNT: "<<iter_count<<endl;
		}
		
		delete[] I;
		delete[] A;
		delete[] b;
	}
	
	reduce_sum(p);
	
	double resid = residual_compute(x,par, f, buf, p , thr_ind);
	
	
	if(thr_ind == 0){
		cout<<"RESIDUAL : "<<resid<<endl;
	}

    reduce_sum(p);
    if (thr_ind == 0) {
        allThreadsPokushali = true;
    }
	
	
	return 0 ;
}


double residual_compute(double *x , parral &par, double (*f) (double, double) , double *buf, int p , int k){
	int ny = par.ny , nx = par.nx;
	int nx_rect = par.nx_rect , ny_rect = par.ny_rect;
	int begin = k*(nx+1 - nx_rect) / p;
	int end = (k+1)*(nx+1 - nx_rect ) / p;
	

	buf[k] = 0;
	double resid = 0;
	for(int i = begin ; i <end;i++){
		if(i==nx - nx_rect){
			for( int l = 0 ; l < ny - 1; l++){
				buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l] , f) - x[I(i,l)] ));
				buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , (par.ys[l] + par.ys[l+1])/2 , f) - (x[I(i,l)] + x[I(i,l+1)])/2 ));
				buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l+1], f) - (x[I(i,l+1)]) ));
				//LOG(i);
				//ELOG(abs(par.f_par(par.xs[i] , par.ys[l] , f) - x[I(i,l)] ));
				//ELOG(abs(par.f_par(par.xs[i] , (par.ys[l] + par.ys[l+1])/2 , f) - (x[I(i,l)] + x[I(i,l+1)])/2 ));
				//ELOG(abs(par.f_par(par.xs[i] , par.ys[l+1], f) - (x[I(i,l+1)]) ));
			}
		}else{
			for( int l = 0 ; l < ny - 1; l++){
				buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l] , f) - x[I(i,l)] ));
				buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , (par.ys[l] + par.ys[l+1])/2 , f) - (x[I(i,l)] + x[I(i,l+1)])/2 ));
				buf[k] = max(buf[k] , abs(par.f_par((par.xs[i] + par.xs[i+1])/2 , (par.ys[l] + par.ys[l+1])/2 , f) - (x[I(i,l)] + x[I(i+1,l+1)])/2 ));
				buf[k] = max(buf[k] , abs(par.f_par((par.xs[i] + par.xs[i+1])/2 , par.ys[l], f) - (x[I(i,l)] + x[I(i+1,l)])/2 ));
				buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l+1], f) - (x[I(i,l+1)]) ));
				buf[k] = max(buf[k] , abs(par.f_par((par.xs[i] + par.xs[i+1])/2 , par.ys[l+1], f) - (x[I(i,l+1)] + x[I(i+1,l+1)])/2));
				//buf[k] = max(buf[k] , abs(par.f_par((2*par.xs[i] + par.xs[i+1])/3 , (2*par.ys[l+1] + par.ys[l])/3, f) - (x[I(i,l+1)] + x[I(i+1,l+1)] + x[I(i,l)])/3));
			}
		}
	}
	if(nx_rect!=0){
	int begin = k*(nx_rect) / p;
	int end = (k+1)*(nx_rect ) / p;
	
	
	for(int i = nx  - nx_rect + begin ; i <end;i++){
			if(i==nx){
				for( int l = 0 ; l < ny - ny_rect - 1; l++){
					buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l] , f) - x[I(i,l)] ));
					buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , (par.ys[l] + par.ys[l+1])/2 , f) - (x[I(i,l)] + x[I(i,l+1)])/2 ));
					buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l+1], f) - (x[I(i,l+1)]) ));
				}
			}else{
				for( int l = 0 ; l < ny - ny_rect - 1; l++){
					buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l] , f) - x[I(i,l)] ));
					buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , (par.ys[l] + par.ys[l+1])/2 , f) - (x[I(i,l)] + x[I(i,l+1)])/2 ));
					buf[k] = max(buf[k] , abs(par.f_par((par.xs[i] + par.xs[i+1])/2 , (par.ys[l] + par.ys[l+1])/2 , f) - (x[I(i,l)] + x[I(i+1,l+1)])/2 ));
					buf[k] = max(buf[k] , abs(par.f_par((par.xs[i] + par.xs[i+1])/2 , par.ys[l], f) - (x[I(i,l)] + x[I(i+1,l)])/2 ));
					buf[k] = max(buf[k] , abs(par.f_par(par.xs[i] , par.ys[l+1], f) - (x[I(i,l+1)]) ));
					buf[k] = max(buf[k] , abs(par.f_par((par.xs[i] + par.xs[i+1])/2 , par.ys[l+1], f) - (x[I(i,l+1)] + x[I(i+1,l+1)])/2));
					//buf[k] = max(buf[k] , abs(par.f_par((2*par.xs[i] + par.xs[i+1])/3 , (2*par.ys[l+1] + par.ys[l])/3, f) - (x[I(i,l+1)] + x[I(i+1,l+1)] + x[I(i,l)])/3));
				}
			}
		}
	}
	
	
	reduce_sum(p);
	for(int i = 0 ; i < p; i++){
		resid = max(resid, buf[i]);
	}

	return resid;
}	


void matr_mult_vector(double *A, int *I, double *x, double *b, int p, int k , int N)
{

	int begin = k*N / p;
	int end = (k+1)*N / p;
    for(int i = begin; i < end; i++)
    {

        b[i] = A[i]*x[i];
        for(int j = I[i]; j < I[i+1]; j++)
            b[i] += A[j]*x[I[j]];
    }
	
	
}



void linear_comb(double * x , double *y , double tau , int p , int k , int N){
	int begin = k*N / p;
	int end = (k+1)*N / p;

	for(int i = begin; i < end; i++){
		x[i]-=tau*y[i];
	}
	
}

double scalar_prod(const double *x , const double *y , double  *buf , int p , int k , int N){
	int begin = k*N / p;
	int end = (k+1)*N / p;

	double s = 0;
	
	for(int i = begin; i < end; i++){
		s +=x[i]*y[i];
	}
	buf[k]=s;
	reduce_sum(p);
	
	s = 0;
	
	for(int i = 0; i < p;i++){
		s+=buf[i];
	}
	
	return s;
}


void Jakobi(double *A , double *r , double *v , int p , int k, int N){
	int begin = k*N / p;
	int end = (k+1)*N / p;
	

	for(int i = begin; i < end; i++){
		v[i] = r[i] / A[i];
	}
	
	reduce_sum(p);
}



int one_solve_step(double *A , int *I , double *x , double *b , double * u , double *v, double *r , double *buf, int N,double b_norm,  int p,int k){
	
	int i;
	double c1 , c2 , tau;
	
	matr_mult_vector(A,I ,x,r ,p,k,N);
	
	
	linear_comb(r , b , 1, p , k , N);
	reduce_sum(p);
	
	for( i = 1 ; i <MAX_ITER_STEP ; i++){
		
		Jakobi(A,r,v,p,k,N);
		matr_mult_vector(A,I ,v,u ,p,k,N); 
		reduce_sum(p);
	
		c1 = scalar_prod(u,r,buf,p,k,N);
		reduce_sum(p);
		c2 = scalar_prod(u,u,buf,p,k,N);

	
		if(c1 < EPS*EPS*b_norm && c2 < EPS*EPS*b_norm ){
			return i;
		}
		tau = c1/c2;
		
		linear_comb(x , v , tau, p , k , N);
		linear_comb(r , u , tau, p , k , N);
		
		reduce_sum(p);
	}
	
	return -1;
}




/*
double get_time(){
        struct rusage buf;
        getrusage(RUSAGE_THREAD,&buf);
        return (double)buf.ru_utime.tv_sec+(double)buf.ru_utime.tv_usec/1000000.;
}

double get_full_time(){
        struct timeval buf;
        gettimeofday(&buf,0);
        return (double)buf.tv_sec+(double)buf.tv_usec/1000000.;
}

*/
