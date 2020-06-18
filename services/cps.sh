#!/bin/bash
 
#Jar包名称
JAR_NAME=resouce-0.0.1-SNAPSHOT.jar
#服务名称
SERVICE_NAME=CountServer
#服务目录
SERVICE_HOME=$(cd "$(dirname "$0")"; pwd)
#日志目录
SERVICE_LOGS=$SERVICE_HOME/log
 
#java虚拟机启动参数
JAVA_OPTS="-ms512m -mx512m -Xmn256m -Djava.awt.headless=true -XX:MaxPermSize=128m"
 
#生成进程文件
PID=$SERVICE_NAME.pid
 
#进入服务目录
cd $SERVICE_HOME
 
case "$1" in
    start)
        if [ ! -d $SERVICE_LOGS ]; then
            mkdir "$SERVICE_LOGS"
        else
            echo "$SERVICE_LOGS exists!"
        fi
        nohup java $JAVA_OPTS -jar $JAR_NAME > $SERVICE_LOGS/$SERVICE_NAME.log  2>&1 &
        echo $! > $SERVICE_HOME/$PID
        echo "==== start $SERVICE_NAME ===="
        ;;
    stop)
        kill -9 `cat $SERVICE_HOME/$PID`
        rm -rf $SERVICE_HOME/$PID
        echo "==== stop $SERVICE_NAME ===="
        ;;
    restart)
        $0 stop
        sleep 2
        $0 start
        ;;
    *)
        $0 stop
        sleep 2
        $0 start
        ;;
esac
exit 0
