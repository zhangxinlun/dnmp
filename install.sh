#!/bin/bash


echo "---------检测是否安装 docker---------"
if ! type docker > /dev/null 2>&1; then
        echo "Docker 未安装，安装中 。。。";
	yum install -y yum-utils device-mapper-persistent-data lvm2
	sudo yum-config-manager \
		--add-repo \
		http://mirrors.aliyun.com/docker-ce/linux/centos/docker-ce.repo
	yum makecache fast
	yum install docker-ce docker-ce-cli containerd.io -y
	systemctl enable docker
	systemctl start docker
        echo "---------完成安装docker---------"
        docker --version
else
        echo "---------Docker 已安装---------";
fi

echo "---------检测是否安装 docker-compose---------"
if ! type docker-compose > /dev/null 2>&1; then
	echo "Docker-compose 未安装，安装中 。。。";
	sudo curl -L "https://github.com/docker/compose/releases/download/1.26.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
	sudo chmod +x /usr/local/bin/docker-compose
	sudo ln -s /usr/local/bin/docker-compose /usr/bin/docker-compose
	echo "---------完成安装docker-compose---------"
	docker-compose --version
else
	echo "---------Docker-compose 已安装---------";
fi

read -r -p $'---------此脚本将使用当前目录下的docker-compose.yaml 构建nginx,php-fpm,redis,mysql的docker镜像---------\n---------请确认docker-compose.yaml 文件中各服务端口号、IP、数据映射目录是否已配置---------\n---------是否继续构建? [Y/N]--------- ' input
case $input in
    [yY][eE][sS]|[yY])
		echo "---------开始构建docker镜像---------"
		docker-compose up -d --build
		echo "---------镜像 构建成功---------"
		docker-compose ps -a 
		;;

    [nN][oO]|[nN])
		echo "取消构建，退出 。。。"
       	;;

    *)
		echo "错误的输入，退出构建"
		exit 1
		;;
esac
