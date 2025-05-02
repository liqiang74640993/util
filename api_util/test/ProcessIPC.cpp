#ifdef __linux__

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>  // 包含 struct ipc_perm 定义的头文件
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h> //msg

#define SHM_SIZE 1024
#define SHM_KEY 1234

#include <exception>
//struct ipc_perm 结构体test
TEST(ProcessIPCTest,IPCPermTest)
{
      int shmid;
      struct shmid_ds shm_info;
      // 获取共享内存段
      shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
      if (shmid == -1) {//not exist
        shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
        if(shmid == -1){
              perror("shmget");
               return;
        }
      }

      // 获取共享内存段的状态信息
      if (shmctl(shmid, IPC_STAT, &shm_info) == -1) {
          perror("shmctl (IPC_STAT)");
          return;
      }
      // 打印权限信息
      printf("Owner UID: %d\n", shm_info.shm_perm.uid);
      printf("Owner GID: %d\n", shm_info.shm_perm.gid);
      printf("Creator UID: %d\n", shm_info.shm_perm.cuid);
      printf("Creator GID: %d\n", shm_info.shm_perm.cgid);
      printf("Permissions: %o\n", shm_info.shm_perm.mode);

      try {

      } catch (std::exception e) {

      }

}

//定义 semun 联合体，用于 semctl 系统调用的第四个参数
union semun {
    int val;//getval ,setval
    struct semid_ds *buf;//IPC_STAT and IPC_SET
    unsigned short *array;
    struct seminfo *__buf;
};
//test sem(信号量的操作)
//SETVAL：设置信号量的值。
//GETVAL:获取信号量的当前值
//IPC_STAT：获取信号量集的状态信息，如权限模式。
//IPC_SET：修改信号量集的权限模式为 0600。
//IPC_RMID：删除信号量集。
TEST(ProcessIPCTest, SEMCtlTest)
{
    int semid;
    union semun arg;
    struct semid_ds sem_info;
    key_t key = ftok("/home/liqiang/source/hello1.txt",'a'); //filepath is not exist
    printf("not exist key:%d\n",key);
    key = ftok("/home/liqiang/source/hello.txt",'a');
    printf("exist key:%d\n",key);

    // 创建信号量集()
       semid = semget(key, 1, IPC_CREAT | 0666);//创建一个信号量
       if (semid == -1) {
           perror("semget");
           exit(EXIT_FAILURE);
       }

       // SETVAL: 设置单个信号量的值为 5
       arg.val = 5;
       if (semctl(semid, 0, SETVAL, arg) == -1) { //0:信号量的编号
           perror("semctl SETVAL");
           exit(EXIT_FAILURE);
       }
       printf("信号量值已设置为 5\n");

       // GETVAL: 获取单个信号量的值
       int sem_value = semctl(semid, 0, GETVAL);
       if (sem_value == -1) {
           perror("semctl GETVAL");
           exit(EXIT_FAILURE);
       }
       printf("当前信号量的值为: %d\n", sem_value);

       // IPC_STAT: 获取信号量集的状态信息
       arg.buf = &sem_info;
       if (semctl(semid, 0, IPC_STAT, arg) == -1) {//
           perror("semctl IPC_STAT");
           exit(EXIT_FAILURE);
       }
       printf("信号量集的权限模式: %o\n", sem_info.sem_perm.mode);

       // IPC_SET: 设置信号量集的状态信息，修改权限模式
       struct semid_ds new_sem_info;
       memcpy(&new_sem_info, &sem_info, sizeof(struct semid_ds));
       new_sem_info.sem_perm.mode = 0600;
       arg.buf = &new_sem_info;
       if (semctl(semid, 0, IPC_SET, arg) == -1) {
           perror("semctl IPC_SET");
           exit(EXIT_FAILURE);
       }

       arg.buf = &sem_info;
       if (semctl(semid, 0, IPC_STAT, arg) == -1) {
           perror("semctl IPC_STAT");
           exit(EXIT_FAILURE);
       }
       printf("修改过后的信号量集的权限模式: %o\n", sem_info.sem_perm.mode);
       printf("sem num:%lu\n",sem_info.sem_nsems);

       // IPC_RMID: 删除信号量集
       if (semctl(semid, 0, IPC_RMID, arg) == -1) {
           perror("semctl IPC_RMID");
           exit(EXIT_FAILURE);
       }
       printf("信号量集已删除\n");
}

// 定义 P 操作（申请资源）
void P(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;
    //semop(semid, &op, 1) 里的 1 表明 op 数组中只有一个 struct sembuf 结构体元素，
    //即只对一个信号量进行一次减 1 操作。要是你想对多个信号量进行操作，或者对同一个信号量进行多次操作，
    //就可以增大 nsops 的值，同时相应地设置 sops 数组。
    if (semop(semid, &op, 1) == -1) {
        perror("semop P");
        exit(EXIT_FAILURE);
    }
}

// 定义 V 操作（释放资源）
void V(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    //只对一个信号量进行一次+1操作
    if (semop(semid, &op, 1) == -1) {
        perror("semop V");
        exit(EXIT_FAILURE);
    }
}

//sem PV Operation
TEST(ProcessIPCTest, SEMOpTest)
{
#define KEY 1234
    int semid;
   // 获取信号量集
   semid = semget(KEY, 1, IPC_CREAT |IPC_EXCL | 0666);
   if (semid == -1) {
       perror("semget");
       exit(EXIT_FAILURE);
   }
   if (semctl(semid, 0, SETVAL, 1) == -1) { //0:信号量的编号
       perror("semctl SETVAL");
       exit(EXIT_FAILURE);
   }

   pid_t pid = fork();
   if(pid > 0){ //parent
       // 申请资源
       sleep(1);
       P(semid);
       printf("parent已获取资源，正在执行临界区代码...\n");
       sleep(1);  // 模拟临界区操作
       printf("parent临界区代码执行完毕，释放资源\n");
       // 释放资源
       V(semid);
       wait(NULL);//wait child

       //信号量是全局对象，删除一次即可
       // IPC_RMID: 删除信号量集
       if (semctl(semid, 0, IPC_RMID) == -1) {
           perror("semctl IPC_RMID");
           exit(EXIT_FAILURE);
       }
       printf("信号量集已删除\n");

   } else{//child
       // 申请资源
       P(semid);
       printf("child已获取资源，正在执行临界区代码...\n");
       sleep(1);  // 模拟临界区操作
       printf("child临界区代码执行完毕，释放资源\n");
       // 释放资源
       V(semid);
   }

}

#define MAX_MSG_SIZE 256
#define MSG_KEY 5678

// 定义消息结构体
typedef struct {
    long msg_type;
    char msg_text[MAX_MSG_SIZE];
} Message;

//消息对了发送消息测试
TEST(ProcessIPCTest, MsgSendTest)
{
   int msqid;
   Message msg;
   // 创建或获取消息队列
   msqid = msgget(MSG_KEY, IPC_CREAT | 0666);
   if (msqid == -1) {
       perror("msgget");
       exit(EXIT_FAILURE);
   }

   // 发送不同类型的消息
   // 发送类型为 1 的消息
   msg.msg_type = 1;
   strcpy(msg.msg_text, "这是类型 1 的消息");
   if (msgsnd(msqid, &msg, strlen(msg.msg_text) + 1, 0) == -1) {
       perror("msgsnd");
       exit(EXIT_FAILURE);
   }
   printf("已发送类型 1 的消息\n");

   // 发送类型为 2 的消息
   msg.msg_type = 2;
   strcpy(msg.msg_text, "这是类型 2 的消息");
   if (msgsnd(msqid, &msg, strlen(msg.msg_text) + 1, 0) == -1) {
       perror("msgsnd");
       exit(EXIT_FAILURE);
   }
   printf("已发送类型 2 的消息\n");
}

//消息队列接受消息和消息队列的状态测试
TEST(ProcessIPCTest, MsgRecvTest)
{
    int msqid;
    Message msg;

    // 获取消息队列
    msqid = msgget(MSG_KEY, 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    struct msqid_ds ds;
    msgctl(msqid, IPC_STAT, &ds);
    std::cout << "current byte:" << ds.__msg_cbytes << std::endl;
    std::cout << "all byte:" <<  ds.msg_qbytes << std::endl;

    // 接收类型为 1 的消息
    if (msgrcv(msqid, &msg, MAX_MSG_SIZE, 0, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    printf("接收到类型1的消息: %s\n", msg.msg_text);

    if (msgrcv(msqid, &msg, MAX_MSG_SIZE, 0, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    printf("接收到类型2消息: %s\n", msg.msg_text);

    msgctl(msqid, IPC_STAT, &ds);

    std::cout << "current byte:" << ds.__msg_cbytes << std::endl;
    std::cout << "all byte:" <<  ds.msg_qbytes << std::endl;
}


#define SHM_KEY 1234
#define SHM_SIZE 1024

TEST(ProcessIPCTest, SHMWriter)
{
    int shmid;
    char *shmaddr;
    // 创建共享内存段
    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // 将共享内存段附加到进程的地址空间
    shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // 向共享内存中写入数据
    strcpy(shmaddr, "Hello, shared memory!");

    // 将共享内存段从进程的地址空间分离
    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

TEST(ProcessIPCTest, SHMReader)
{
    int shmid;
    char *shmaddr;

    // 获取共享内存段
    shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // 将共享内存段附加到进程的地址空间
    shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // 从共享内存中读取数据
    printf("Read from shared memory: %s\n", shmaddr);

    // 将共享内存段从进程的地址空间分离
    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // 删除共享内存段
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}

#endif
