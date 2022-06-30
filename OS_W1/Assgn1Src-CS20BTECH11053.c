/*------------------------------------------------------------

My current program is implementation of parallel execution of threads.
The areas of code to be modified to change this code in to sequential execution of threads is highlighted with #, and the modification is explained at that particular area of code

------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include<time.h>

struct point//Created a structure to work on points
{
  float x_coord;
  float y_coord;
};
int no_of_thrds;//represents the number of threads we are using
int num_of_dest_pnts;//represents the number of destination points we are using
struct point dest_pnts[100000];//destination points from the input file will be stored in to this array
struct point src_pnt;//represents source point
int num_thrds_started=0;//represents the count of threads that started execution
struct point arr[100000];//Nearest point of each thread will be stored in this array
/*-------------------------------------------

Algorithm I used to find nearest point
1) Assume first point of the list of points as the nearest point
2)Check if the next point is nearer than the current nearest point, if it is so make this as the nearest point
3) Repeat point 2 till you have checked for all the points


*****I am dividing destination points among all threads
----------------------------------------------------*/
void *near_pnt()//function that finds the nearest point of each thread and updates arr accordingly
{
  if(num_of_dest_pnts/no_of_thrds!=0)//checks if every thead can get atleast one point
  {
    if(num_thrds_started!=no_of_thrds)//checks whether all the threads are completed or not
    {
      int start_num=(num_thrds_started)*(num_of_dest_pnts%no_of_thrds);//point from dest_pnts whose index is start_num is the starting point of the thread we are in
      num_thrds_started++;//since the current thread has started we are increasing num_thrds_started
      struct point nearst_pnt;//we will store the nearest point in this among those points that belong to the current thread
      nearst_pnt=dest_pnts[start_num];//initialising nearest point to the first point among the points of this thread
      if(num_thrds_started!=no_of_thrds)
      { 
        for(unsigned i=start_num; i<(num_thrds_started)*(num_of_dest_pnts%no_of_thrds);i++)//by the end of this loop nearest point to be stored in arr will be figured out
        {
          if((pow(src_pnt.x_coord-nearst_pnt.x_coord,2)+pow(src_pnt.y_coord-nearst_pnt.y_coord,2))>(pow(src_pnt.x_coord-dest_pnts[i].x_coord,2)+pow(src_pnt.y_coord-dest_pnts[i].y_coord,2)))
          {
            nearst_pnt=dest_pnts[i];//updating nearest point
          }
        }
      }
      else
      {
        for(unsigned i=start_num; i<num_of_dest_pnts;i++)//by the end of this loop nearest point to be stored in arr will be figured out
        {
          if((pow(src_pnt.x_coord-nearst_pnt.x_coord,2)+pow(src_pnt.y_coord-nearst_pnt.y_coord,2))>(pow(src_pnt.x_coord-dest_pnts[i].x_coord,2)+pow(src_pnt.y_coord-dest_pnts[i].y_coord,2)))
          {
            nearst_pnt=dest_pnts[i];//updating nearest point
          }
        }
      }
      arr[num_thrds_started-1]=nearst_pnt;
    }
  }
  else if(num_of_dest_pnts/no_of_thrds==0)//checks if every thead can't get atleast one point, using only a single thread for all points
  {
    num_thrds_started++;//since the current thread has started we are increasing num_thrds_started
    if(num_thrds_started==1)//using only the first thread we are finding nearest point
    {
      struct point nearst_pnt;//we will store the nearest point in this among those points that belong to the current thread
      nearst_pnt=dest_pnts[0]; 
      for(unsigned i=0; i<num_of_dest_pnts;i++)//by the end of this loop nearest point to be stored in arr will be figured out
      {
        if((pow(src_pnt.x_coord-nearst_pnt.x_coord,2)+pow(src_pnt.y_coord-nearst_pnt.y_coord,2))>(pow(src_pnt.x_coord-dest_pnts[i].x_coord,2)+pow(src_pnt.y_coord-dest_pnts[i].y_coord,2)))
        {
          nearst_pnt=dest_pnts[i];//updating nearest point
        }
      }
      arr[num_thrds_started-1]=nearst_pnt;
    }
    
  }
  pthread_exit(0);
}

int main()
{
  char input_file_name[20];//txt file to be passed as input
  int num;//integers from file are read into this variable
  float pnt_coord;//decimals(coordinates) from file are read into this variable
  FILE* fp;//pointer to the input file
  printf("Enter the name of the file:\n");//prompting
  scanf("%s",input_file_name);//taking the input_file_name
  fp=fopen(input_file_name,"r");//opening the file for opening
  fscanf(fp,"%d",&num);//reads no_of_thrds from the input file and stores it at &num
  fseek(fp,2,SEEK_CUR);//moves the file pointer forward by 2 characters
  no_of_thrds=num;//assigning the value to the appropriate variable
 // struct point p;
  fscanf(fp,"%f",&pnt_coord);//reads src_x_coordinate from the input file and stores it at &pnt_coord
  fseek(fp,1,SEEK_CUR);//moves the file pointer forward by 1 characters
  src_pnt.x_coord=pnt_coord;//assigning the value to the appropriate variable
  fscanf(fp, "%f", &pnt_coord);
  fseek(fp,2,SEEK_CUR);//moves the file pointer forward by 2 characters
  src_pnt.y_coord=pnt_coord;//assigning the value to the appropriate variable
  fscanf(fp,"%d",&num);
  fseek(fp,2,SEEK_CUR);//moves the file pointer forward by 2 characters
  num_of_dest_pnts=num;//assigning the value to the appropriate variable
  for(unsigned i=0; i<num_of_dest_pnts; i++)
  {
    fscanf(fp, "%f", &pnt_coord);//reads one of the destination points_x_coordinate from the input file and stores it at &pnt_coord
    fseek(fp,1,SEEK_CUR);//moves the file pointer forward by 1 characters
    dest_pnts[i].x_coord=pnt_coord;//assigning the value to the appropriate variable
    fscanf(fp, "%f", &pnt_coord);//reads one of the destination points_y_coordinate from the input file and stores it at &pnt_coord
    fseek(fp,3,SEEK_CUR);//moves the file pointer forward by 3 characters
    dest_pnts[i].y_coord=pnt_coord;//assigning the value to the appropriate variable
    //printf("%f:%f\n",dest_pnts[i].x_coord,dest_pnts[i].y_coord); 
  }
  pthread_t p_th[no_of_thrds];//initialising an array of threads
  double init_time=clock();//taking the time stamp when the calculation of nearest point starts
  for(unsigned i=0; i<no_of_thrds;i++)
  {
    pthread_create(&p_th[i],NULL, &near_pnt, NULL);//execution of thread p_th[i] starts
    //pthread_join(p_th[i],NULL);//###################uncomment this join statement if you want to perform sequential execution of threads
  }
  for(unsigned i=0; i<no_of_thrds;i++)//#####################comment this loop when you want to perform sequential execution of threads
  {
    pthread_join(p_th[i],NULL);
  }
  if(num_of_dest_pnts/no_of_thrds==0)//enters into this if, if only the first thread is working on all points
  {
    printf("Nearest point is (%f,%f)\n",arr[0].x_coord,arr[0].y_coord);
  }
  else//enters this else if every thread is working on atleast one point
  {
    struct point nearest_point;
    nearest_point=arr[0];
    for(unsigned i=0;i<no_of_thrds;i++)//by the end of this loop nearest point will be figured out
    {
      if((pow(src_pnt.x_coord-nearest_point.x_coord,2)+pow(src_pnt.y_coord-nearest_point.y_coord,2))>(pow(src_pnt.x_coord-arr[i].x_coord,2)+pow(src_pnt.y_coord-arr[i].y_coord,2)))
      {
        nearest_point=arr[i];//updating the nearest point
      }
    }
    printf("Nearest point is (%f,%f)\n",nearest_point.x_coord,nearest_point.y_coord);
  }
  double fin_time=clock();//taking the time stamp when the calculation of nearest point ends
  double calc_time=((-init_time+fin_time)/CLOCKS_PER_SEC)*1000000;//finding the time taken to find the nearest point
  printf("time_taken=%f micro seconds",calc_time);


  return 0;
}