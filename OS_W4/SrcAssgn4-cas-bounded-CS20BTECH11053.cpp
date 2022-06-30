//******Please note that I am printing GMT's(hour:minutes:seconds) in the output file
//including necessary headers
#include <iostream>
#include <limits.h>
#include <fstream>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <atomic>
#include <cmath>
#include <random>

using namespace std;


int k;//number of times each thread executes its critical section
int num_of_threads;//declaring numbeer of threads  

float avg1;//average time for critical section(lambda1)
float avg2;//average time for remainder section(lambda2)
double worst_case_tym=0;//overall worst case time taken by a process to enter cs(initialised to 0)
double wait_tym_sum=0;//overall waiting time sum(initialised to 0)
atomic<int> lock (0);//declaring lock
atomic<bool>* waiting; //declaring waiting array(size not fixed yet)
ofstream bcas("Bounded CAS ME Output.txt");//creating output file


void *test_cs(void *ptr)//test critical section function
{
  unsigned pt= *(unsigned *)ptr;//this will store the index of thread as per declared(in main) pthread array
  
  double wait_tym;//used to store wait time for each cs execution
  default_random_engine generator;//declaring generator
  exponential_distribution<double> distribution((float(1)/float(avg1)));//for critical section
  exponential_distribution<double> distribution1(float(1)/float(avg2));//for remainder section
  
  for(unsigned i=0; i<k; i++)
  {
    waiting[pt] = true;//changing waiting[pt]to true, since it is going to waiting zone
		bool key = false;//declaring key
    time_t tim=time(0);
    tm *gottime = gmtime(&tim);//initialising a variable to store GMT
    
    bcas<<i+1;//--------------printing requests in the output file
    if(i+1==1)
    {
      bcas<<"st ";
    }
    else if(i+1==2)
    {
      bcas<<"nd ";
    }
    else if(i+1==3)
    {
      bcas<<"rd ";
    }
    else{
      bcas<<"th ";
    }
    bcas<<"CS requested at " <<gottime->tm_hour<<":"<<gottime->tm_min<<":"<<gottime->tm_sec<<" by thread "<<pthread_self()<<"\n";//--------printing requests to the output file completed
    int x=0;
    int y=1;
    double init_time=clock();//marking time to calculate wait time
    while (waiting[pt] && !key){ // waiting till lock is released
			int x =0 , y=1;
			key = lock.compare_exchange_strong(x,y);
		}
		waiting[pt] = false;	//changing waiting[pt] to false, since waiting is completed
    //-critical section started
    
    double fin_time=clock();
    wait_tym=((-init_time+fin_time)/CLOCKS_PER_SEC)*1000;//wait time calculation
    if(worst_case_tym<wait_tym)//updating worst case time
    {
      worst_case_tym=wait_tym;
    }
    wait_tym_sum+=wait_tym;//updating overall waiting time
    gottime = gmtime(&tim);//storing GMT
    
    bcas<<i+1;//------------printing enterings to the output file
    if(i+1==1)
    {
      bcas<<"st ";
    }
    else if(i+1==2)
    {
      bcas<<"nd ";
    }
    else if(i+1==3)
    {
      bcas<<"rd ";
    }
    else{
      bcas<<"th ";
    }
    bcas<<"CS entered at "<<gottime->tm_hour<<":"<<gottime->tm_min<<":"<<gottime->tm_sec<<" by thread "<<pthread_self()<<"\n";//------printing enterings to the output file completed
    
    double w_t_1=distribution(generator);//generating some sleep time value for critical section simulation
    sleep(w_t_1);  //-critical section simulation
    gottime = gmtime(&tim);//storing GMT
    bcas<<i+1;//-------------printing exits to the output file
    if(i+1==1)
    {
      bcas<<"st ";
    }
    else if(i+1==2)
    {
      bcas<<"nd ";
    }
    else if(i+1==3)
    {
      bcas<<"rd ";
    }
    else{
      bcas<<"th ";
    }
    bcas<<"CS exited at "<<gottime->tm_hour<<":"<<gottime->tm_min<<":"<<gottime->tm_sec<<" by thread "<<pthread_self()<<"\n";//------printing exits to the output file completed    
    int j = (pt+1)%num_of_threads; //initialised j to decide which next process to enter critical section
  		while((j!=pt) && !waiting[j]) // checking if the threads are waiting(from pt, pt+1, .. till j), if any witing thread is found the looping stops
      {
        if(j>num_of_threads)
        {
          break;
        }
         j = (j+1)%num_of_threads;
      }
  		if (j==pt)
      {
        lock = 0;//setting lock to 0 if no processes are waiting
      }
  		else
      {
        waiting[j] = false;//changing a waiting thread's waiting to false(to let it go to critical section)
      }
    
    double w_t_2=distribution1(generator);//generating sleep time for remainder section simulation
    sleep(w_t_2);//remainder section simulation
    
  }
  pthread_exit(0);//thread exits
  
}


int main()
{
  ifstream info("inp-params.txt");//declaring a variable to read from the input file
  info>>num_of_threads>>k>>avg1>>avg2;//reading from input file
  
  pthread_t p_th[num_of_threads];//initialising an array of threads
  unsigned index_value[num_of_threads];//declaring an array to pass the index of the thread to the function that is executing in thread
  for(unsigned i=0; i<num_of_threads; i++)
  {
    index_value[i]=i;//storing values to index_array
  }
  
  waiting = (atomic<bool>*)malloc(num_of_threads*sizeof(atomic<bool>));//allocating memory to waiting
  for(unsigned i=0; i<num_of_threads;i++)
  {
    pthread_create(&p_th[i],NULL, &test_cs, (void *)(&index_value[i]));//execution of thread p_th[i] starts
  }
  for(unsigned i=0; i<num_of_threads;i++)//thread joinings
  {
    pthread_join(p_th[i],NULL);
  }
  double avg_wait_tym;//declaring overall average waiting time
  avg_wait_tym=wait_tym_sum/(double)(k*num_of_threads);//calculating overall average waiting time
  cout<<"Worst case waiting time = "<<worst_case_tym<<" milliseconds \nAverage waiting time = "<<avg_wait_tym<<" milliseconds\n";//printing worst case time and overall average waiting time

  free(waiting);//freeing the allocated memory

  return 0;
}