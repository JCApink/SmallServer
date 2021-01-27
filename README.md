为什么做这个？

在我看了一些相关书籍以及博客文章之后，特别是看完《Linux高性能服务器编程》之后，对着书上的代码敲了一遍，之后自己就想实现一个，所以自己将书中的部分代码进行整合，之后看了一些大佬的项目，自己尝试理解他们的项目，之后自己就开始实现。

使用的模型：线程池 + IO复用（reactor模型）

![](photo\模型 (2).jpg)

# 项目中遇到的问题：

## 1.如何确保主线程启动的时候，子线程已经初始化完成？

下面是几个代码的核心：

```C++
//主线程执行的地方
void EventThreadLoopPool::startbaseloop(){
    start = true;
    for(int i = 0; i < ThreadNum; ++i){
        //每一个子线程会有一个EventThreadLoop，每个EventThreadLoop会有一个EventLoop
        EventThreadLoop::ptr tmp(new EventThreadLoop());
        eventloops.push_back(tmp->start());
    }
}

//创建子线程
EventLoop* EventThreadLoop::start(){
    assert(!thread.getStart());
    //一开始主线程走这里对子线程进行创建
    thread.start(); 
    {
        MutexLockGuard lock(mutex);
        while(eventloops == nullptr) {
            /*之所以要wait，进行条件变量的处理，是因为虽然我们在后面对子线程初始化的时候有一个countdown(后面
            解释)，但是在完成初始化时，子线程里面的eventloop需要正确的获取到，同样的也是一个初始化的问题*/
            cond.wait();
        }
    }
    return eventloops;
}
//来到这里真正的创建子线程了
void Thread::start(){
    assert(!start_);
    start_ = true;
    ThreadData *data = new ThreadData(Func, p_name, &tid, &countdown);
    //创建子线程
    if(pthread_create(&pthreadId, NULL, &startThread, data)){
        start_ = false;
        delete data;
    }
    else{
        //主线程要等到子线程的变量初始化完以后，才能继续执行。
        /*
        	这里的countdown是我封装的一个类主要使用来解决两个问题：
        	1.主线程在发起多个子线程，等这些子线程完成初始化以后，主线程才能继续执行。
        		通常用于主线程等待多个子线程完成初始化
        	2.主线程发起多个子线程，子线程都等待主线程，主线程完成完成其他一些任务之后，
        		通知所有子线程开始执行。通常用于多个子线程等待主线程发起“起跑”命令。
        	在这里是解决第一个问题
        */
        countdown.wait();
        assert(tid > 0);
    }
}

void* startThread(void* obj){
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

void runInThread(){
        *Tid = CurrentThread::tid();
        Tid = NULL;
    	//子线程已经完成初始化了，唤醒主线程
        Countdown->countDown();
        Countdown = NULL;
        CurrentThread::t_threadName = Name.empty() ? "Thread" : Name.c_str();
    	//子线程函数执行的入口
        Func();
        CurrentThread::t_threadName = "finish";
    }
//子线程执行的地方
void EventThreadLoop::ThreadFunc(){
    EventLoop eventloop;
    {
        MutexLockGuard lock(mutex);
        //获取eventloop对象
        eventloops = &eventloop;
        //唤醒主线程
        cond.notify();
    }
    //子线程开始执行了
    eventloop.loop();
    eventloops = nullptr;
}
```

总结来说就是使用了条件变量来确保子线程完成初始化，条件变量主要是下面几个函数：

```c++
pthread_cond_init(&cond, NULL); //初始化
pthread_cond_destroy(&cond); //销毁
pthread_cond_wait(&cond, Mutex.getMutex()); //进入等待 期间会释放锁，等到被唤醒的时候又会重新获取锁
pthread_cond_broadcast(&cond); //广播唤醒
pthread_cond_signal(&cond); //单个唤醒
```

## 2.这是我看书最难理解的地方，要怎么完成将主线程的任务分发给子线程呢？

因为一般线程池的实现一般使用队列 + 条件变量实现的，当队列有事件发生的时候就会使用条件变量进行唤醒。让线程拿队列中的事件去实现线程池，但是那些没有很好的利用到epoll。最后我通过gdb一步步调试，我先用ab压测只有一次请求，打印请求时的栈上信息，最后发现eventloop是这个模型的精髓。

```c++
//子线程开始执行了
eventloop.loop();

/*前面那里说到每个子线程会在loop中执行，在每一个eventloop会有一个epoll，子线程间的eventloop是互不相关的，
	只有子线程与主线程之间会有同步问题
*/
void EventLoop::loop(){
    assert(!looping);
    assertInCurrentThread();
    looping = true;
    quit = false;
    std::vector<std::shared_ptr<Channel>> channels;
    //std::cout << "EventLoop:102 Fd: " << wakeupfd << std::endl;
    while(!quit){
        channels.clear();
        //子线程阻塞在这里
        channels = epoll->epoll();
        for(auto& channel: channels){
            channel->doAllRevents();
        }
        DoingFunctions();
        epoll->timesolve();
        //超时处理
    }
    looping = false;
}

//等待事件发生
std::vector<std::shared_ptr<Channel>> Epoll::epoll(){
    while(true){
        //阻塞在这里
        int event_count = epoll_wait(epollFd, &*events.begin(), events.size(), EPOLLWAIT);
        if (event_count < 0){
            assert(event_count < 0);
            LOG << "EPOLL WAIT ERROR";
        }
        std::vector<std::shared_ptr<Channel>> channels = getevents(event_count);
        if (event_count > 0){
            return channels;
        }
    }
}


//下面这个函数，是主线程接受新的连接请求，它会一直在accept中等待新的连接
void TcpServer::newConnect(){
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof( client_address );
    int accept_fd;
    while((accept_fd = accept( ListenFd, ( struct sockaddr* )&client_address, &client_addrlength )) > 0){
        if(accept_fd > MAX_FD){
            close(accept_fd);
        }
        //这个拿到的是每一个子线程的eventloop，
        EventLoop* loop = threadpool->getnextloop();
        int ret = setNonBlock(accept_fd);
        assert(ret != -1);
        //因为这里处理的是http的请求，所以每一个连接会有对应的一个http，并且这个http会有一个文件描述符
        std::shared_ptr<HttpData> httpdata(new HttpData(loop, accept_fd));
        httpdata->getChannel()->setHttpData(httpdata);
        //注意：这里的eventloop对象是子线程的，主线程也有自己的eventloop对象
        loop->queueInLoop(std::bind(&HttpData::newConnectEvent, httpdata));
    }
    channel->setEvents(EPOLLIN | EPOLLET);
}

//接下来这里是主线程执行的，但是它执行的eventloop对象是子线程的
void EventLoop::queueInLoop(CallBack function){
    {
        MutexLockGuard lock(mutex);
        LOG << "INFO: QUEUEINLOOP";
        //将需要执行的事件放到函数数组中，等待判断是否属于自己当前的线程应该去处理的事情
        functions.emplace_back(std::move(function));
    }
    /*这里就是判断了，因为主线程它有pid，tgid，对于一个主线程来说，它的pid和tgid就是它自己本身。
    	但是对于进程创建的子线程它的pid是它自己，它的tgid才是主线程。因此可以利用它们的pid不同区
    	分是不是属于自己应该执行的任务。(在eventloop对象中已经保存了它们所属的pid具体值)
    */
    /*
    	这里这样判断一个是是否当前线程在执行，另一个是如果是当前线程在执行，但是此时有另外的IO线程在dongFunction，
    	那么同样需要唤醒
    	
    	简单来说，就是只有在自己IO线程在调用这个才不用唤醒
    */
    if(!InCurrentThread() || doingfunctions){
        //如果当前不是自己的应该处理的，那么就唤醒
        wakeupwrite();
    }
}

//下面这里是程序设计的很巧的一个地方，采用eventfd进行异步唤醒，当然也可以采用管道等方式进行唤醒（但是采用eventfd会更加的高效），它的唤醒过程是向套接字中写入1个字节，此时，阻塞在epoll_wait的对应线程就会被唤醒，来处理这一个事件，它就要读出来，唤醒之后就会处理刚刚放进事件数组中的事件，这样就达到了交换文件描述符的目的了
int creatEventfd() {
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    assert(fd != -1);
    return fd;
}

void EventLoop::wakeupwrite(){
    int one  = 1;
    size_t n = writen(wakeupfd, (char*)&one, sizeof(one));
    if(n != sizeof(one)){
        LOG << "WARN: WRITE IN wakeup NOT sizeof(one) BITS" ;
    }
}

void EventLoop::wakeupread(){
    int one;
    size_t n = readn(wakeupfd, &one ,sizeof(one));
    if(n != sizeof(one)){
        LOG << "WARN: READ IN wakeup NOT sizeof(one) BITS" ;
    }
    wakeupchannel->setEvents(EPOLLIN | EPOLLET);
}


    
```

处理该模型的精髓是正确处理主线程和子线程eventloop对象之间的关系，一方面这样能够提高并发度，减少锁的竞争，锁之间的竞争只会发生在主线程和子线程之间，子线程和子线程间没有锁的竞争。另一方面，弄懂这个，整个reactor模型也就更加的清晰了。

## 3.有点小坑的SIGPIPE信号

一开始我在测试的时候，会发现，当测试完成的时候，进程老是自己退出，这就让我觉得很奇怪，我设计的明明没有让它自己退出，为什么进程会自己退出呢？

后来我就利用gdb进行调试，就会发现会产生一个SIGPIPE信号，因为之前看书有点印象，之后知道原因，原来是因为四次挥手的时候，如果客户端关闭了，但是服务端还有数据要发送，但是客户端已经关了，这是服务端会收到RST报文，此时主机收到一个SIGPIPE信号，那么就会导致进程退出。解决的办法很简单，那就是直接忽略这个信号就行了。

![](photo\SIGPIPE.png)

```c++
void signal_for_sigpipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL)){
            return;
    }
}
```

## 4.遇到了一次请求后，一直在等待，测试没有出结果，但是如果终止进程又会出现结果

造成这样的原因很容易找到，很明显的就是服务端在结束通信的时候没有进行四次挥手的过程，只有客户端发送了FIN，但是服务端响应了ACK，但是并没有进行FIN报文发送，后来发现是由于智能指针的循环引用导致，解决方案是将类中的智能指针改成weak_ptr，之后再进行相应的处理。

# 项目测试结果：

ab 进行压测时，一千个客户端，一百万次请求

<img src="photo\测试结果.png" style="zoom: 80%;" />

