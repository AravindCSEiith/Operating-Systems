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

atomic_flag lock_stream = ATOMIC_FLAG_INIT;//this helps to control the entry to critical section
int k;//number of times each thread executes its critical section

float avg1;//average time for critical section(lambda1)
float avg2;//average time for remainder section(lambda2)
double worst_case_tym=0;//overall worst case time taken by a process to enter cs(initialised to 0)
double wait_tym_sum=0;//overall waiting time sum(initialised to 0)
ofstream tas("TAS ME Output.txt");//creating output file


void *test_cs(void *ptr)//test critical section function
{
  pthread_t pt= *(pthread_t *)ptr;//this will store the thread id in to pt
  double wait_tym;//used to store wait time for each cs execution
  default_random_engine generator;//declaring generator
  exponential_distribution<double> distribution((float(1)/float(avg1)));//for critical section
  exponential_distribution<double> distribution1(float(1)/float(avg2));//for remainder section
  
  for(unsigned i=0; i<k; i++)
  {
    time_t tim=time(0);
    tm *gottime = gmtime(&tim);//initialising a variable to store GMT
    
    tas<<i+1;//--------------printing requests in the output file
    if(i+1==1)
    {
      tas<<"st ";
    }
    else if(i+1==2)
    {
      tas<<"nd ";
    }
    else if(i+1==3)
    {
      tas<<"rd ";
    }
    else{
      tas<<"th ";
    }
    tas<<"CS requested at " <<gottime->tm_hour<<":"<<gottime->tm_min<<":"<<gottime->tm_sec<<" by thread "<<pt<<"\n";//--------printing requests to the output file completed
    double init_time=clock();//marking time to calculate wait time
    while(lock_stream.test_and_set()){
      //this loop runs until it finds lock to be open
    }
    //-critical section started
    
     double fin_time=clock();
    wait_tym=((-init_time+fin_time)/CLOCKS_PER_SEC)*1000;//wait time calculation
    if(worst_case_tym<wait_tym)//updating worst case time
    {
      worst_case_tym=wait_tym;
    }
    wait_tym_sum+=wait_tym;//updating overall waiting time
    gottime = gmtime(&tim);//storing GMT
    
    tas<<i+1;//------------printing enterings to the output file
    if(i+1==1)
    {
      tas<<"st ";
    }
    else if(i+1==2)
    {
      tas<<"nd ";
    }
    else if(i+1==3)
    {
      tas<<"rd ";
    }
    else{
      tas<<"th ";
    }
    tas<<"CS entered at "<<gottime->tm_hour<<":"<<gottime->tm_min<<":"<<gottime->tm_sec<<" by thread "<<pt<<"\n";//---------printing enterings to the output file completed
    
    double w_t_1=distribution(generator);//generating some sleep time value for critical section simulation
    sleep(w_t_1);  //-critical section simulation
    gottime = gmtime(&tim);//storing GMT
    tas<<i+1;//-------------printing exits to the output file
    if(i+1==1)
    {
      tas<<"st ";
    }
    else if(i+1==2)
    {
      tas<<"nd ";
    }
    else if(i+1==3)
    {
      tas<<"rd ";
    }
    else{
      tas<<"th ";
    }
    tas<<"CS exited at "<<gottime->tm_hour<<":"<<gottime->tm_min<<":"<<gottime->tm_sec<<" by thread "<<pt<<"\n"; //------------printing exits to the output file completed   
    lock_stream.clear();//exit section started, unlocking
    gottime = gmtime(&tim);//storing GMT
    
    
    double w_t_2=distribution1(generator);//generating some sleep time value for remainder section simulation
    sleep(w_t_2);//remainder section simulation
    
  }
  pthread_exit(0);//thread exits
  
}


int main()
{
  int num_of_threads;//declaring numbeer of threads  
  ifstream info("inp-params.txt");//declaring a variable to read from the input file
  info>>num_of_threads>>k>>avg1>>avg2;//reading from input file
  
  pthread_t p_th[num_of_threads];//initialising an array of threads 
  
  for(unsigned i=0; i<num_of_threads;i++)
  {
    pthread_create(&p_th[i],NULL, &test_cs, (void *)(&(p_th[i])));//execution of thread p_th[i] starts
  }
  for(unsigned i=0; i<num_of_threads;i++)//thread joinings
  {
    pthread_join(p_th[i],NULL);
  }
  double avg_wait_tym;//declaring overall average waiting time
  avg_wait_tym=wait_tym_sum/(double)(k*num_of_threads);//calculating overall average waiting time
  cout<<"Worst case waiting time = "<<worst_case_tym<<" milliseconds \nAverage waiting time = "<<avg_wait_tym<<" milliseconds\n";//printing worst case time and overall average waiting time

  

  return 0;
}