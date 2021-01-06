//
//  main.cpp
//  test
//
//  Created by 郭尹文 on 2020/11/03.
//  Copyright © 2020 Kevin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


typedef struct
{
    int thread_id;
    int start;
    int end;
    long long int *num_in_cir;
} Arg;


pthread_mutex_t mutexsum; //pthread互斥鎖

void *estimate_num_in_cir(void *arg){
    
    Arg *data = (Arg *)arg;
    int thread_id = data->thread_id;
    int start = data->start;
    int end = data->end;
    long long int *num_in_cir = data->num_in_cir;
    long long int local_num = 0;
    unsigned int seed = 123;
    for(int toss=start; toss<end; toss++){
        /* 產生 [-1 , 1) 的浮點數亂數 */
        double x = 2*((double)rand_r(&seed)) / (RAND_MAX+1.0) -1;
        double y = 2*((double)rand_r(&seed)) / (RAND_MAX+1.0) -1;

        double distance_squared = x*x + y*y;
        if (distance_squared <= 1.0) {
            local_num++;
        }
    }
    
    //****關鍵區域****
    //一次只允許一個thread存取
    pthread_mutex_lock(&mutexsum);
    
    *num_in_cir += local_num;
    pthread_mutex_unlock(&mutexsum);
    //***************
    
    printf("Thread %d did %d to %d \n", thread_id, start, end);
    
    pthread_exit((void *)0);
    
}

int main(int argc, char*argv[]) {

    int numthread = atoi(argv[1]); //讀取thread個數

    long long int totaltoss = atoi(argv[2]); // 讀取總toss數

    pthread_t callThd[numthread]; //宣告建立pthread

    //初始化互斥鎖
    pthread_mutex_init(&mutexsum, NULL);
    
    //設定 pthread 性質是要能 join
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    //每個thread 都可以存取的 num_in_cir
    //因為不同thread都要能存取，故用指標
    long long int *num_in_cir = (long long int*)malloc(sizeof(*num_in_cir));
    *num_in_cir = 0;
    
    int part = totaltoss / numthread;

    Arg arg[numthread]; //每個thread傳入的參數
    for (int i = 0; i < numthread ; i++) {
        //設定傳入參數
        arg[i].thread_id = i;
        arg[i].start = part * i;
        arg[i].end = part * (i+1);
        arg[i].num_in_cir = num_in_cir; //所有thread共用
        
        //建立一個thread, 執行estimate任務，傳入arg[i]指標參數
        pthread_create(&callThd[i], &attr, estimate_num_in_cir, (void *)&arg[i]);
    }

    //回收性質設定
    pthread_attr_destroy(&attr);
    
    void *status;
    for (int i = 0;  i < numthread;  i++) {
        //等待每個thread執行完畢
        pthread_join(callThd[i], &status);
        
    }
    
    printf("%lld\n",*num_in_cir);
    printf("Pi = %.10lf \n", ((double) *num_in_cir/(double)(totaltoss))*4);
    
    //回收互斥鎖
    pthread_mutex_destroy(&mutexsum);
    
    //離開
    pthread_exit(NULL);

}
