TinyWebServer
originated from https://github.com/qinguoyi/TinyWebServer.git


上传文件功能, 已完成，支持MB级别的文件上传

一个Linux下的C++轻量级Web服务器
使用线程池 + 非阻塞socket + epoll(ET和LT均实现) + 事件处理(Reactor和模拟Proactor均实现)的并发模型
使用状态机解析HTTP请求报文，支持解析GET和POST请求
访问服务器数据库实现web端用户注册、登录功能，可以请求服务器图片和视频文件
实现同步/异步日志系统，记录服务器运行状态
