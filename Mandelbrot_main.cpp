#include<chrono>
#include<complex>
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<vector>
#include<pthread.h>
#include<unistd.h>
using std::complex;
using std::cout;
using std::vector;
const int TOT_PIXELS = 640000,X_PIXELS = 800,MAX_ITER = 10000,X_RANGE = 2.0,Y_RANGE = 2.0;
const complex<double> OFFSET(1.5,1.0);
const int CONVERGE_CONST = 2;
const char color_bar[8][13] =	{
				"\033[40m  \033[0m",
				"\033[41m  \033[0m",
				"\033[42m  \033[0m",
				"\033[43m  \033[0m",
				"\033[44m  \033[0m",
				"\033[45m  \033[0m",
				"\033[46m  \033[0m",
				"\033[47m  \033[0m"
				};

struct thread_data{
	int ith_thread;
	int num_thread;
	vector<int> *mvals;
	thread_data(int ith,int tot_thread):ith_thread(ith),num_thread(tot_thread){mvals = new vector<int>;}
	~thread_data(){delete mvals;}
private:
	thread_data();
};

int mandelbrot(const complex<double> z);
void printOnScreen(const vector<int> & data_vect);
void usingPyplot(const vector<int> & data_vect);
void* thread_works(void *);
	

int main(int argc,char * argv[]){
	std::chrono::steady_clock::time_point ts = std::chrono::steady_clock::now();
	int stat,num_thread = argc>1?atoi(argv[1]):4;
	void *vptr;
	thread_data *rt_data;
	vector<int> mvals;

	pthread_t thread[num_thread];
	for(int i=0;i<num_thread;i++){
		stat = pthread_create(&thread[i],NULL,thread_works,(void *) new thread_data(i,num_thread));
		if(stat) cout<<"Error cannot create thread.\n";
	}
	
	for(int i=0;i<num_thread;i++){
		stat = pthread_join(thread[i],&vptr);
		if(stat) cout<<"Error cannot join thread.\n";
		rt_data = (thread_data*) vptr;
		mvals.insert(mvals.end(),rt_data->mvals->begin(),rt_data->mvals->end());
		delete rt_data;
	}
	if(mvals.size() != TOT_PIXELS) cout<<"size not match\n";
	usingPyplot(mvals);
	std::chrono::steady_clock::time_point te = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_elapse = std::chrono::duration_cast<std::chrono::duration<double> >	(te-ts);
	cout<<"Time elapsed : "<<time_elapse.count()<<"second(s)\n";
}



/*********************************************************/
/*Mandelbrot function                                    */
/*      Find mandelbrot point in complex plan.           */
/*      Convergence condition is given by MAX_ITER and   */
/*      convergence constant.                            */
/*********************************************************/
int mandelbrot(const complex<double> z){
	complex<double> oz(0.0,0.0);
	int count;
	for(count=0;count < MAX_ITER and abs(oz)<= CONVERGE_CONST;count++){
		oz = oz * oz + z;
	}
	if(count==MAX_ITER) return 0;//good looking
	return count;
}

/*********************************************************/
/*Assign for loop to each thread.                        */
/*      ith_thread variable is of type *thread_data,     */
/*      the function return ith_thread variable as       */
/*      the thread end.                                  */
/*********************************************************/

void* thread_works(void * ith_thread){
	thread_data *data = (thread_data*) ith_thread;
	double x,y;
	int pix = ((data->ith_thread)*TOT_PIXELS)/data->num_thread;
	int max_pix = data->ith_thread+1==data->num_thread?TOT_PIXELS:((data->ith_thread+1)*TOT_PIXELS)/data->num_thread;
	for(;pix<max_pix;pix++){
		x = X_RANGE*(double) (pix%X_PIXELS)/X_PIXELS;
		y = Y_RANGE*(double) (pix/X_PIXELS)/(TOT_PIXELS/X_PIXELS);
		data->mvals->push_back(mandelbrot(complex<double>(x,y)-OFFSET));
	}
	pthread_exit((void*)data);
}

/*********************************************************/
/*                                                       */
/*                                                       */
/*                                                       */
/*                                                       */
/*********************************************************/
void usingPyplot(const vector<int> & data_vect){
	FILE * fh;
	char plot_tool[200];
	fh = fopen("mandelbrot_pts.dat","wb");
	vector<int>::iterator it;
	for(int i=0;i<data_vect.size();i++){
		fwrite((void*)&data_vect[i],sizeof(int),1,fh);
	}
	fclose(fh);
	sprintf(plot_tool,"./mandelbrot_plot.py %d",X_PIXELS);
	system(plot_tool);

}
void printOnScreen(const vector<int>&data_vect){
	for(int j=0;j<TOT_PIXELS;j++){
		cout<<color_bar[data_vect[j]%8];
		if( (j+1)%X_PIXELS==0) cout<<'\n';
	}
}
