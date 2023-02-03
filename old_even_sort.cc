//include package
#include <mpi.h> //MPI
#include <stdio.h> //atoi
#include <stdlib.h> //atoi, malloc
#include <algorithm> //sort
#include <iostream>//memcpy
#include <cstring>//memcpy

using namespace std;

void MergeTwo(float *localData ,int localDataSize, float *receData,int receDataSize,float *temp,bool smallOrnot)
{   
    if(smallOrnot){
        //Keep smaller data

        //double check
        if(localData[localDataSize-1]<=receData[0]){
            return;
        }
        int localCount = 0;
        int receCount = 0;
     
        //Version1: Use array 
        // while(localCount < localDataSize && receCount < receDataSize ){
        //     if (localData[localCount] < receData[receCount]){
        //         temp[index] = localData[localCount++];
        //         index++;
        //     }else{
        //          temp[index] = receData[receCount++];
        //         index++;
        //     }
               
        // }
        // while(localCount<localDataSize){
        //     temp[index]=localData[localCount++];
        //     index++;
        // }
        // while(receCount<receDataSize){
        //     temp[index]=receData[receCount++];
        //     index++;
        // }
        
        //Version2:Use pointer
        while(localCount<localDataSize && receCount<receDataSize && localCount+receCount< localDataSize){
            if (*(localData+localCount) < *(receData+receCount)){
                    *(temp+localCount+receCount) = *(localData+localCount);
                    localCount++;
                }else{
                    *(temp+localCount+receCount)= *(receData+receCount);
                    receCount++;
                }
        }   
        while(localCount<localDataSize && localCount+receCount < localDataSize){
                *(temp+localCount+receCount) = *(localData+localCount);
                localCount++;
        }
        while(receCount<receDataSize && localCount+receCount < localDataSize){
                *(temp+localCount+receCount)= *(receData+receCount);
                receCount++;
        }

        
    }else{
        //Keep larger data

         //double check
        if(localData[0]>=receData[receDataSize-1]){
            return;
        }
     
        int localCount = localDataSize-1;
        int receCount = receDataSize-1;
        int index = localDataSize-1;
        
        //Version1: Use array
        // while(localCount>=0 && receCount>=0 && index>=0){
        //     if (localData[localCount] < receData[receCount]){
        //         temp[index] = receData[receCount--];
        //         index--;
        //     }else{
        //         temp[index] = localData[localCount--];
        //         index--;
        //     }
                
        // }
        // while(localCount >= 0 && index >=0){
        //     temp[index] = localData[localCount--];
        //     index--;
        // }
        // while(receCount >= 0 && index >=0){
        //     temp[index] = receData[receCount--];
        //     index--;
        // }

        //Version: Use pointer
        while(localCount>=0 && receCount>=0 && index>=0){
            if (*(localData+localCount) < *(receData+receCount)){
                    *(temp+index) = *(receData+receCount);
                    index--;
                    receCount--;
             
                }else{
                    *(temp+index) = *(localData+localCount);
                    index--;
                    localCount--;
                }
        }

        while(localCount>=0 && index >=0){
            *(temp+index) = *(localData+localCount);
            index--;
            localCount--;
        }

        while(receCount>=0 && index>=0){
              *(temp+index)= *(receData+receCount);
                    index--;
                    receCount--;

        }
      

    }
    
    memcpy(localData,temp,sizeof(float)*localDataSize);
}





int main(int argc, char **argv){
    
    //Declare variables 
    int rank, size, rc; // MPI variable 
    MPI_File file_in, file_out; //MPI I/O
    MPI_Group orig_group, new_group;//Create MPI new group
    MPI_Comm new_comm= MPI_COMM_WORLD;//Create MPI new communicator
    //int ranges;// The eliminat range for create MPI new group
    int n, localData_size,recePerv_size, receNext_size, remainder, data_start; // Distribute data 
    int oddPhaseRank, evenPhaseRank; // Set which nodes to exchange data in different phase
    int EVEN_CHANGE=1; //different phase
    int ODD_CHANGE=2; //different phase
    // double Comm_Time,CPU_Time,IO_Time,time_start,time_end,time_temp,all_start,all_end;
	// Comm_Time = CPU_Time = IO_Time = time_start = time_end =all_start=all_end= 0.0;

   
    //Initial MPI
    rc = MPI_Init(&argc,&argv);
    //all_start = MPI_Wtime();
    if(rc!=MPI_SUCCESS){
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD,rc);
    }
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);//return your rank id
    MPI_Comm_size(MPI_COMM_WORLD,&size);//return size of nodes

    //Data amount
    n = atoi(argv[1]);
    
    // Abanton unused nodes by creating new group
    if(n<size){
        //Extract the original group handle
        MPI_Comm_group(MPI_COMM_WORLD,&orig_group);
        int ranges[1][3] = {{n, size-1, 1}};
        //Remove unused nodes to create new group
        MPI_Group_range_excl(orig_group,1,ranges,&new_group);
        //Create new communicator
        MPI_Comm_create(MPI_COMM_WORLD,new_group,&new_comm);
        
        //Terminate the MPI for unused processes
        if(rank>=n || new_comm== MPI_COMM_NULL){
            MPI_Finalize();
            return 0;
        }
        size = n;
    }
    
    //Cauclate the nums that each nodes have to deal with
    localData_size = n / size;
    remainder = n % size;
    
    /*Version1：each node will read average num, and the last one will read average+remainderㄡ
    result: runtime error*/
    // //Set buff and size for merge and receive 
    // if(rank==size-1){
    //     //the tail one will receive average size
    //     recePerv_size=localData_size;
    //     receNext_size=0;
    //     data_start
    //     localData_size+=remainder;
        
    // }else if(rank==size-2){
    //     //next to last will receive the tail one
    //     receNext_size=localData_size+remainder;
    //     recePerv_size=localData_size;
    // }else{
    //     receNext_size=localData_size;
    //     recePerv_size=localData_size;
    // }
    
    /*Version 2: each node which < remainder get average num+1, then the rank which >remainder get the average element.
        ig.9(element)/6(node size)=1...3
        node0~node2 will get 2 nums;
        node3~node5 will get 1 nums;
    */
    if(rank>=remainder){
       // read average nums
       //remainder will distribute to the above node 
       data_start=rank*localData_size+remainder;

    }else{
        //read average+1 elements (to distribute remainder nums)
        ++localData_size;
        data_start=rank*localData_size;

    }
    /*
        follow the above sample
        node2 have to send 2 elements to node3
        node3 have to send 1 elements to node2
    */
    if(rank == remainder){
        receNext_size=localData_size;
        recePerv_size=localData_size+1;
    }else if(rank==remainder-1){
        receNext_size=localData_size-1;
        recePerv_size=localData_size;
    }else{
        receNext_size=localData_size;
        recePerv_size=localData_size;
    }

    // Local data to read from file
    float *data=(float*)malloc(sizeof(float)*localData_size);
    // To merge received data and local data
    float *temp=(float*)malloc(sizeof(float)*localData_size+1);
    // To receive data from another node
    float *recv=(float*)malloc(sizeof(float)*localData_size+1);

    //time_start = MPI_Wtime();
    //MPI I/O: Read data
    MPI_File_open(new_comm,argv[2],MPI_MODE_RDONLY,MPI_INFO_NULL,&file_in);
    MPI_File_read_at(file_in,sizeof(float)*data_start,data,localData_size,MPI_FLOAT,MPI_STATUS_IGNORE);
    MPI_File_close(&file_in);

    //time_end = MPI_Wtime();
	//IO_Time += time_end - time_start;

    // Local sort
    sort(data,data+localData_size);

    // Set which process to exchange data for local
    // odd phase: 
    //      oddsize:*0 12 34 56 ... evensize:*0 12 34 56 *7...
    // even phase: 0 1 2 3
    //      oddsize:01 23 45 67 *8...evensize: 01 23 45 67...
    if(rank==0||rank%2==0){
        oddPhaseRank=rank-1;
        evenPhaseRank=rank+1;
    }else{
        oddPhaseRank=rank+1;
        evenPhaseRank=rank-1;
    }
   
    if (oddPhaseRank <0 || oddPhaseRank == size)
    oddPhaseRank = MPI_PROC_NULL;
    if (evenPhaseRank <0 || evenPhaseRank == size)
    evenPhaseRank = MPI_PROC_NULL;

   int isSortedAll=0;
   int isSorted=0;
	while (!isSortedAll && size>1)
	{
        isSorted=false;
        //even phase
        //In oddsize,  if the last one is even then it don't need to send and receive.(See the above sample which num with *.)
        if(rank==0 ||(rank%2==0 && rank!=size-1)){
            //have to send and receive to next
            //Check the nums on local node and neighborhood node whether is sorted.
            //time_start = MPI_Wtime();
            MPI_Sendrecv(&data[localData_size-1], 1, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE,recv, 1, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE, new_comm, MPI_STATUS_IGNORE);
            //time_end = MPI_Wtime();
			//Comm_Time += time_end - time_start;	
            if(data[localData_size-1]>recv[0]){
                //time_start = MPI_Wtime();
                MPI_Sendrecv(data, localData_size, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE,recv, receNext_size, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE, new_comm, MPI_STATUS_IGNORE);
                //time_end = MPI_Wtime();
			    //Comm_Time += time_end - time_start;	
                MergeTwo(data , localData_size, recv, receNext_size,temp,true);
                isSorted=false;
            }else{
                isSorted=true;
            }
        }else if(rank%2==1){
            //have to send and receive to previous
            //Check the nums on local node and neighborhood node whether is sorted.
            //time_start = MPI_Wtime();
            MPI_Sendrecv(data, 1, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE,recv, 1, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE, new_comm, MPI_STATUS_IGNORE);
            //time_end = MPI_Wtime();
			//Comm_Time += time_end - time_start;
            if(data[0]<recv[0]){
                //have to send and receive to previous
                //time_start = MPI_Wtime();
                MPI_Sendrecv(data, localData_size, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE,recv, recePerv_size, MPI_FLOAT, evenPhaseRank, EVEN_CHANGE, new_comm, MPI_STATUS_IGNORE);
                //time_end = MPI_Wtime();
			    //Comm_Time += time_end - time_start;
                MergeTwo(data , localData_size,  recv,recePerv_size,temp,false);
                isSorted=false;
            }else{
                isSorted=true;
            }
        }
        // odd phase 
        // the first one always not take part in.
        // In evensize, if the last one is odd then it don't need to send and receive data.(See the above sample which num with *.)
        if(rank%2==1 && rank!=size-1){
            //have to send and receive to next
            //Check the nums on local node and neighborhood node whether is sorted.
            //time_start = MPI_Wtime();
            MPI_Sendrecv(&data[localData_size-1], 1, MPI_FLOAT, oddPhaseRank, ODD_CHANGE,recv, 1, MPI_FLOAT, oddPhaseRank, ODD_CHANGE, new_comm, MPI_STATUS_IGNORE);
            // time_end = MPI_Wtime();
			// Comm_Time += time_end - time_start;
            if(data[localData_size-1]>recv[0]){
                //time_start = MPI_Wtime();
                MPI_Sendrecv(data, localData_size, MPI_FLOAT, oddPhaseRank, ODD_CHANGE,recv, receNext_size, MPI_FLOAT, oddPhaseRank, ODD_CHANGE, new_comm, MPI_STATUS_IGNORE);
                //time_end = MPI_Wtime();
			    //Comm_Time += time_end - time_start;
                MergeTwo(data , localData_size,  recv, receNext_size,temp,true);
                isSorted=false;
            }else{
                isSorted=true;
            }
        }else if(rank%2==0 && rank!=0){
            //have to send and receive to previous
            //Check the nums on local node and neighborhood node whether is sorted.
            //time_start = MPI_Wtime();
            MPI_Sendrecv(data, 1, MPI_FLOAT, oddPhaseRank, ODD_CHANGE,recv, 1, MPI_FLOAT, oddPhaseRank, ODD_CHANGE, new_comm, MPI_STATUS_IGNORE);
            //time_end = MPI_Wtime();
			//Comm_Time += time_end - time_start;
            if(data[0]<recv[0]){
                //time_start = MPI_Wtime();
                MPI_Sendrecv(data, localData_size, MPI_FLOAT, oddPhaseRank, ODD_CHANGE,recv, recePerv_size, MPI_FLOAT, oddPhaseRank, ODD_CHANGE, new_comm, MPI_STATUS_IGNORE);
                //time_end = MPI_Wtime();
			    //Comm_Time += time_end - time_start;
                MergeTwo(data , localData_size,  recv,recePerv_size,temp,false);
                isSorted=false;
            }else{
                isSorted=true;
            }
        }
        //MPI_LAND means that if there is a node to send 0(isSorted), then isSortedAll will be false.(And the while loop will keep going).
        //Thus, each node have to send true, then isSortedAll will be true.(Break while loop.)
        MPI_Allreduce(&isSorted, &isSortedAll, 1, MPI_C_BOOL, MPI_LAND, new_comm);
    }

    //time_start = MPI_Wtime();
    //Write the file
    MPI_File_open(new_comm,argv[3],MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL,&file_out);
    MPI_File_write_at(file_out,sizeof(float)*data_start,data,localData_size,MPI_FLOAT,MPI_STATUS_IGNORE);
    MPI_File_close(&file_out);
    // time_end = MPI_Wtime();
	// IO_Time += time_end - time_start;

    free(temp);
    free(data);
    free(recv);

    // time_temp = IO_Time;
	// MPI_Reduce(&time_temp, &IO_Time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	// time_temp = Comm_Time;
	// MPI_Reduce(&time_temp, &Comm_Time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);	


    //all_end = MPI_Wtime();
    MPI_Finalize();

    // if(rank==0){
    //     CPU_Time=all_end-all_start-IO_Time-Comm_Time;

	// 	printf("I/O Time:%lf\n", IO_Time);
	// 	printf("Communication Time:%lf\n", Comm_Time);
	// 	printf("Computation Time:%lf\n", CPU_Time);
	// }


    return 0;


}


