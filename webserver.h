#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <utility>
#include <queue>
#include <thread>

#include "./threadpool/threadpool.h"
#include "./http/http_conn.h"

const int MAX_FD = 65536;           // 最大文件描述符
const int MAX_EVENT_NUMBER = 10000; // 最大事件数
const int TIMESLOT = 5;             // 最小超时单位

class WebServer
{
public:
    WebServer();
    ~WebServer();

    void init(int port, string user, string passWord, string databaseName,
              int log_write, int opt_linger, int trigmode, int sql_num,
              int thread_num, int close_log, int actor_model);

    void thread_pool();
    void sql_pool();
    void log_write();
    void trig_mode();
    void eventListen();
    void eventLoop();
    void timer(int connfd, struct sockaddr_in client_address);
    void adjust_timer(util_timer *timer);
    void deal_timer(util_timer *timer, int sockfd);
    bool dealclientdata();
    bool dealwithsignal(bool &timeout, bool &stop_server);
    void dealwithread(int sockfd);
    void dealwithwrite(int sockfd);
    static void *release_worker(void *arg)
    {
        WebServer *p = (WebServer *)arg;
        while (1)
        {
            p->release_queue_lock.lock();
            p->release_queue_cond.wait(p->release_queue_lock.get()); // 利用条件锁，防止忙等待
            auto x = p->release_queue.front();
            if (p->users[x.second].improv == 1)
            {
                if (p->users[x.second].timer_flag == 1)
                {
                    p->deal_timer(x.first, x.second);
                }
                p->release_queue.pop();
            }
        }
    }

public:
    // 基础
    int m_port;
    char *m_root;
    int m_log_write;
    int m_close_log;
    int m_actormodel;

    int m_pipefd[2];
    int m_epollfd;
    http_conn *users;

    // 数据库相关
    connection_pool *m_connPool;
    string m_user;         // 登陆数据库用户名
    string m_passWord;     // 登陆数据库密码
    string m_databaseName; // 使用数据库名
    int m_sql_num;

    // 线程池相关
    threadpool<http_conn> *m_pool;
    int m_thread_num;

    // epoll_event相关
    epoll_event events[MAX_EVENT_NUMBER];

    int m_listenfd;
    int m_OPT_LINGER;
    int m_TRIGMode;
    int m_LISTENTrigmode;
    int m_CONNTrigmode;

    // 定时器相关
    client_data *users_timer;
    Utils utils;

    // 负责释放sockfd和timer的线程标识
    pthread_t release_thread{};
    std::queue<std::pair<util_timer *, int>> release_queue{}; // 释放队列
    locker release_queue_lock{};                              // 释放队列互斥锁
    cond release_queue_cond{};
};
#endif
