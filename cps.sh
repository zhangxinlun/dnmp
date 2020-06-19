#!/bin/bash
 
#服务名称
SERVICE_NAME=dnmp
#服务目录
SERVICE_HOME=/root/docker/docker-compose-lnmp
 
 
 
case "$1" in
    start)
	docker-compose up -d --build
        echo "==== start Docker-compose DNMP ===="
	docker-compose logs
	docker-compose ps
        ;;
    stop)
        docker-compose down
        echo "==== stop Docker-compose DNMP ===="
        ;;
    status)
        docker-compose ps
        ;;
    restart)
        $0 stop
        sleep 1
        $0 start
        ;;
    *)
        $0 stop
        sleep 1
        $0 start
        ;;
esac
exit 0
