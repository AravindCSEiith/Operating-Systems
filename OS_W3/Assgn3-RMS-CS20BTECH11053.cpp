//In arrays index i represents process i+1
#include <iostream>
#include <fstream>
#include <limits.h>


using namespace std;



int main() {
  int num_of_pro;//number of different processes
  ifstream info("inp-params.txt");//opening file to read contents
  info>>num_of_pro;//reading from file(no of different processes)
  int process_id[num_of_pro];//array to store the process id's of processes
  int processing_tym[num_of_pro];//array to store the processing times of processes
  int period_or_deadline[num_of_pro];//array to store period or deadline of processes
  int dynamic_deadline[num_of_pro];//array to store the deadlines of each type of process, and when ever a process arrives the deadline in this array is modified accordingly 
  int dynamic_processing_tym[num_of_pro];//this array is helpful to handle the preemptd processes once they resume 
  int preemp_check_processing_tym[num_of_pro];//helpful to identify if a process is preempted(initialised to procssing times, but when a process is preempted then this is changed to dynamic processing time, and when a preempted process is resumed, then this value will be back to processing time)
  int k[num_of_pro];//store the number of times each process should execute
  int sum_k=0;//used to store the total number of processes that will arrive at system, also used this value in the condition of loop that's running the algo
  
  int dynamic_k[num_of_pro];//stores the no of processes of each type that are yet to come
  int cur_tym=0;//stores the value of present time
  
  int dynamic_status[num_of_pro];//used to store the status of a particular process at current time(0-process not available to run, 1-process available to run)
  int minimum;//used to store the index of the process that arrives
  int *nearest_deadline_index=&minimum;//will be passed to the mini function as a first paraeter and as a result the index of the process that arrives is stored into minimum
  
  int index_is_priority[num_of_pro];//index 0-1st priority, 1-2nd priority, 2-3rd priority.....
  int value_is_priority[num_of_pro];//value 0-1st priority, 1-2nd priority, 2-3rd priority.....
  int priority_deciding_deadlines[num_of_pro];//used this array to assign the priorities
  int waiting_tym[num_of_pro];//stores the total waiting time of each process, till the current time
  int successful_finishes[num_of_pro];//stores the count of successful finishes of each type of process
  int pro_came=0;//declared this to just store the total number of processes that will arrive at the system
  
  int index_to_run=-1;//used to store the index of highest priority(is -1 if no process is available to run)
  int prev_ind_to_run=-2;//used this value to identify whether to preempt a process or start/resume a process
  for(unsigned i=0;i<num_of_pro;i++)//by the end of this loop all contents of input file will be read and all above arrays are initialised or modified appropriately
  {
    info>>process_id[i];
    info>>processing_tym[i];
    info>>period_or_deadline[i];
    info>>k[i];
    dynamic_k[i]=k[i];
    dynamic_deadline[i]=period_or_deadline[i];
    
    dynamic_status[i]=1;
    dynamic_processing_tym[i]=processing_tym[i];
    preemp_check_processing_tym[i]=processing_tym[i];
    priority_deciding_deadlines[i]=period_or_deadline[i];
    index_is_priority[i]=-1;
    value_is_priority[i]=-1;
    waiting_tym[i]=0;
    successful_finishes[i]=0;
    
  }
 
  for(unsigned i=0; i<num_of_pro; i++)//finding the total no of processes that arrive at system and storing the value value in to sum_k
  {
    sum_k=sum_k+k[i];
  }
  pro_came=sum_k;//modifying pro_came from 0 to sum_k

  
  ofstream rms("RMS-Log.txt");//opening log file to write into
  
  for(unsigned i=0; i<num_of_pro; i++)//this loop will write the details of each process into log file
  {
    rms<<"Process P"<<process_id[i]<<": processing time="<<processing_tym[i]<<"; period or deadline="<<period_or_deadline[i]<<"; joined the system at time 0\n";  
  }
  for (unsigned i=0; i<num_of_pro;i++)//this loop will assign priority values(priority deciding deadlines array) to priority arrays
  {
      int minnimum_value=priority_deciding_deadlines[0];//------------finding the index of process that arrives
      *nearest_deadline_index=0;
      for(unsigned k=0; k<num_of_pro; k++)
      {
        if(priority_deciding_deadlines[k]<minnimum_value)
        {
          minnimum_value=priority_deciding_deadlines[k];
          *nearest_deadline_index=k;
        }
      }
   //mini(nearest_deadline_index, num_of_pro, priority_deciding_deadlines);
    value_is_priority[minimum]=i;
    index_is_priority[i]=minimum;
    priority_deciding_deadlines[minimum]=INT_MAX;
  }
  for(unsigned i=0; sum_k>0; i++)//this loop is the heart of algo I used to run rms
  {
    for(unsigned j=0;j>=0&&j<num_of_pro;j++)//I used this loop to find index_to_run(index of process of highest priority among available processes)
    {
      if(dynamic_status[index_is_priority[j]]==1)
      {
        index_to_run=index_is_priority[j];
        j=-2;
      }
      else if(j==num_of_pro-1)
      {
        index_to_run=-1;
      }
    }
    if(dynamic_processing_tym[prev_ind_to_run]==0)//used to finish a process whose dynamic processing time is 0, instead of preempting
    {
      index_to_run=prev_ind_to_run;
    }
    if(prev_ind_to_run==-2||prev_ind_to_run==index_to_run)//checking the condition whether to start/resume or to preempt a process 
    {
      if(index_to_run!=-1)//checking whether process is available to run or not
      {
        if(preemp_check_processing_tym[index_to_run]==processing_tym[index_to_run]&&dynamic_processing_tym[index_to_run]==processing_tym[index_to_run])//checking if the process is starting and doing necessary modifications
        {
          
          rms<<"Process P"<<index_to_run+1<<" starts execution at time "<<cur_tym<<"\n";
        }
        else if(preemp_check_processing_tym[index_to_run]<processing_tym[index_to_run])//checking if the processing is resuming and doing necessary modifications
        {
          rms<<"Process P"<<index_to_run+1<<" resumes execution at time "<<cur_tym<<"\n";
          preemp_check_processing_tym[index_to_run]=processing_tym[index_to_run];//preemp_check_processing_time modified to processing time to know if it is preempted again
        }
      }

      int minnimum_value=dynamic_deadline[0];//------------finding the index of process that arrives
      *nearest_deadline_index=0;
      for(unsigned k=0; k<num_of_pro; k++)
      {
        if(dynamic_deadline[k]<minnimum_value)
        {
          minnimum_value=dynamic_deadline[k];
          *nearest_deadline_index=k;
        }
      }//---------------------------------------------
      //mini(nearest_deadline_index, num_of_pro, dynamic_deadline);//finding the index of process that arrives
      if(dynamic_processing_tym[index_to_run]<(dynamic_deadline[minimum]-cur_tym)&&index_to_run!=-1)//checking if the process finishes execution and doing modifications accordingly
      {
        cur_tym+=dynamic_processing_tym[index_to_run];
        rms<<"Process P"<<index_to_run+1<<" finishes execution at time "<<cur_tym<<"\n";
        successful_finishes[index_to_run]++;
        waiting_tym[index_to_run]+=(-(dynamic_deadline[index_to_run]-period_or_deadline[index_to_run])+cur_tym-processing_tym[index_to_run]);
        dynamic_k[index_to_run]-=1;
        dynamic_processing_tym[index_to_run]=processing_tym[index_to_run];
        preemp_check_processing_tym[index_to_run]=processing_tym[index_to_run];
        dynamic_status[index_to_run]=0;
        sum_k--;
        prev_ind_to_run=-2;
        if(dynamic_k[index_to_run]==0)//Knowing if the process stops arriving and terminating it by doing necessary modifications
        {
          dynamic_status[index_to_run]=0;
          dynamic_deadline[index_to_run]=INT_MAX;
        }
        
      }
      else
      {
        if(index_to_run==-1)//checking if cpu is idle and doing necessary modifications
        {
          dynamic_status[minimum]=1;
          cur_tym=dynamic_deadline[minimum];
          dynamic_deadline[minimum]+=period_or_deadline[minimum];
          rms<<"CPU is idle till time "<<cur_tym<<"\n";

          for(unsigned i=minimum+1;i<num_of_pro;i++)//doing modifications accordingly if a process arrives at current time
          {
            if(cur_tym==dynamic_deadline[i])
            {
              dynamic_status[i]=1;
              dynamic_deadline[i]+=period_or_deadline[i];
            }
          }
        }
        else if(dynamic_processing_tym[index_to_run]>=(dynamic_deadline[minimum]-cur_tym))//control of program will move into this else if, if a process going to miss deadline or executig process is going to finish at just deadline
        {//--------------------------
          if(dynamic_status[minimum]==1)//checking if the process going to arrive has completed executing(in its previous arrival) or not
          {
            if(index_to_run==minimum && (dynamic_processing_tym[index_to_run]==(dynamic_deadline[minimum]-cur_tym)))//checking if the executing process and arriving process are same, and at the same time also checking if the process going to finish execution and doing modifications accordingly
            {
              rms<<"Process P"<<index_to_run+1<<" finishes execution at time "<<dynamic_deadline[minimum]<<"\n"; 
              successful_finishes[index_to_run]++;
              dynamic_k[minimum]-=1;
              dynamic_status[minimum]=1;
              dynamic_processing_tym[index_to_run]=processing_tym[index_to_run];
              cur_tym=dynamic_deadline[minimum];
              waiting_tym[index_to_run]+=(period_or_deadline[index_to_run]-processing_tym[index_to_run]);
              dynamic_deadline[minimum]+=period_or_deadline[minimum];
              if(dynamic_k[minimum]==0)//Knowing if the process stops arriving and terminating it by doing necessary modifications
              {
                dynamic_deadline[minimum]=INT_MAX;
                dynamic_status[minimum]=0;
              }

            }
            else//control of program enters this else if a process going to miss deadline or if a process missed deadline and doing modifications accordingly based on whether process missed deadline or process will miss deadline
            {
              if(minimum==index_to_run)//identifying if the process about to execute misses deadline
              {
                rms<<"Process P"<<minimum+1<<" will miss deadline, so terminated\n"; 
                
              }
              if(minimum!=index_to_run)//identifying if a process missed deadline
              {
                rms<<"Process P"<<minimum+1<<" missed deadline\n";
              }
              dynamic_k[minimum]-=1;
              if(index_to_run!=minimum)//identifying if a process missed deadline
              {
                dynamic_status[minimum]=1;
              }
              else//identifying if the process about to execute misses deadline
              {
                dynamic_status[minimum]=0;
              }
              dynamic_processing_tym[index_to_run]-=(dynamic_deadline[minimum]-cur_tym);
              if(minimum!=index_to_run)//identifying if a process missed deadline
              {
                cur_tym=dynamic_deadline[minimum];
              }
              if(index_to_run==minimum)//identifying if the process about to execute misses deadline
              {
                waiting_tym[minimum]+=dynamic_processing_tym[minimum]+period_or_deadline[minimum]-processing_tym[minimum];
              }
              else//identifying if a process missed deadline
              {
                waiting_tym[minimum]+=dynamic_processing_tym[minimum]+period_or_deadline[minimum]-processing_tym[minimum];
              }
              
              dynamic_processing_tym[minimum]=processing_tym[minimum];
              if(minimum!=index_to_run)//identifying if a process missed deadline
              {
                dynamic_deadline[minimum]+=period_or_deadline[minimum];
              }
              if(dynamic_k[minimum]==0)//Knowing if the process stops arriving and terminating it by doing necessary modifications
              {
                dynamic_deadline[minimum]=INT_MAX;
                dynamic_status[minimum]=0;
              }
              if(index_to_run==minimum)//identifying if the process about to execute misses deadline
              {
                dynamic_processing_tym[index_to_run]=processing_tym[index_to_run];
              }
            }
            sum_k--;
            preemp_check_processing_tym[minimum]=processing_tym[minimum];
            if(index_to_run==minimum)//identifying if there are any processes at present that missed deadline or that are arrived
            {
              for(unsigned i=index_to_run+1; i<num_of_pro; i++)
              {
                if(cur_tym==dynamic_deadline[i])
                {
                  if (dynamic_status[i]==0)//identifying if a process arrives and doing modifications accordingly
                  {
                    dynamic_status[i]=1;
                    dynamic_deadline[i]+=period_or_deadline[i];
                    dynamic_processing_tym[i]=processing_tym[i];
                  }
                  else if(dynamic_status[i]==1)//identifying if a process missed deadline and doing modifications accordingly
                  {
                    rms<<"Process P"<<i+1<<" missed deadline\n";
                    waiting_tym[i]+=period_or_deadline-processing_tym+dynamic_processing_tym[i];
                    sum_k--;
                    dynamic_k[i]--;
                    dynamic_deadline[i]+=period_or_deadline[i];
                    preemp_check_processing_tym[i]=processing_tym[i];
                    dynamic_processing_tym[i]=processing_tym[i];
                    if(dynamic_k[i]==0)//Knowing if the process stops arriving and terminating it by doing necessary modifications
                    {
                      dynamic_deadline[i]=INT_MAX;
                      dynamic_status[i]=0;
                    }
                  }
                }
              }
            }
          }
          else//control of program enters this else if the process going to arrive is not available to the program currently(i.e. status of that process is 0)
          {
            dynamic_status[minimum]=1;
            dynamic_processing_tym[index_to_run]-=(dynamic_deadline[minimum]-cur_tym);
            cur_tym=dynamic_deadline[minimum];
            dynamic_deadline[minimum]+=period_or_deadline[minimum];
            if(index_to_run==minimum)
            {
              dynamic_processing_tym[index_to_run]=processing_tym[index_to_run];
            }
            if(dynamic_k[minimum]==0)//Knowing if the process stops arriving and terminating it by doing necessary modifications
            {
              dynamic_deadline[minimum]=INT_MAX;
              dynamic_status[minimum]=0;
            }
          }
          prev_ind_to_run=index_to_run;//modifying prev_minimum to use for further rounds of loop
      
          if(index_to_run==-1||index_to_run==minimum)//modifying prev_minimum to use for further rounds of loop
          {
            prev_ind_to_run=-2;
          }
        }
        
      }
      do//using this loop I am identifying the processes that misses deadline
      {
        for(unsigned j=0;j>=0&&j<num_of_pro;j++)
        {
          if(dynamic_status[index_is_priority[j]]==1)
          {
            index_to_run=index_is_priority[j];
            j=-2;
          }
          else if(j==num_of_pro-1)
          {
            index_to_run=-1;
          }
        }
        if(dynamic_deadline[index_to_run]-cur_tym<dynamic_processing_tym[index_to_run]&&index_to_run!=-1)//checking if a process misses deadline
        {
          rms<<"Process P"<<index_to_run+1<<" will miss deadline, so terminated\n";
          dynamic_processing_tym[index_to_run]-=dynamic_deadline[index_to_run]-cur_tym;
          waiting_tym[index_to_run]+=dynamic_processing_tym[index_to_run]+period_or_deadline[index_to_run]-processing_tym[index_to_run];
          dynamic_status[index_to_run]=0;
          dynamic_processing_tym[index_to_run]=processing_tym[index_to_run];
          preemp_check_processing_tym[index_to_run]=processing_tym[index_to_run];
          dynamic_k[index_to_run]--;
          sum_k--;
          if(dynamic_k[index_to_run]==0)//Knowing if the process stops arriving and terminating it by doing necessary modifications
          {
            dynamic_deadline[index_to_run]=INT_MAX;
            dynamic_status[index_to_run]=0;
          }
        }
        
      }while(dynamic_deadline[index_to_run]-cur_tym<dynamic_processing_tym[index_to_run]&&index_to_run!=-1);

    }
    else if(prev_ind_to_run!=index_to_run)//checking if a process is preempted and doing necessary changes accordingly
    {
      rms<<"Process P"<<prev_ind_to_run+1<<" is preempted by process P"<<index_to_run+1<<". Remaining processing time:"<<dynamic_processing_tym[prev_ind_to_run]<<"\n";
      preemp_check_processing_tym[prev_ind_to_run]=dynamic_processing_tym[prev_ind_to_run];
      prev_ind_to_run=-2;
    }
    
  }
  ofstream rms_stats("RMS-Stats.txt");//opening stats file to write into it
  float avg_waiting_tym[num_of_pro];//declared array to store avg waiting times of each process
  for(unsigned i=0; i<num_of_pro; i++)//calculating average_waiting times of each process
  {
    avg_waiting_tym[i]=float(waiting_tym[i])/float(k[i]);
  }
  int succ_count=0;//I am going to use this to store total successful finishes
  for(unsigned i=0;i<num_of_pro;i++)//calculating total successful finishes count
  {
    succ_count+=successful_finishes[i];
  }
  rms_stats<<"Total number of processes came into the system="<<pro_came<<"\n"<<"Total number of processes completed successfully="<<succ_count<<"\n"<<"Total number of processes missed deadlines="<<pro_came-succ_count<<"\n";//writing into the stats file
  for(unsigned i=0; i<num_of_pro; i++)//writing avg waiting time into the stats file
  {
    rms_stats<<"Process P"<<i+1<<": Average waiting time="<<avg_waiting_tym[i]<<" milliseconds\n";
  }
  int overall_waiting_time=0;//used to store overall waiting tims
  float overall_avg_waiting_time=0;//used to store overall avg waiting time
  for(unsigned i=0;i<num_of_pro;i++)//finding overall waiting time
  {
    overall_waiting_time+=waiting_tym[i];
  }
  overall_avg_waiting_time=float(overall_waiting_time)/float(pro_came);//finding overall average waiting time
  rms_stats<<"Overall average waiting time="<<overall_avg_waiting_time<<" milliseconds";//writing into the stats file

  


  return 0;
}