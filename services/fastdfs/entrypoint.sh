#!/bin/bash
# set -eo pipefail
# shopt -s nullglob

# 删除配置文件中的空格
sed -i "s/ = /=/g" /etc/fdfs/*.conf
sed -i "s/ =/=/g" /etc/fdfs/*.conf

# 配置 tracker 的路径
sed -i "s|/home/yuqing/fastdfs|$TRACKER_BASE_PATH|g" /etc/fdfs/tracker.conf

# 配置 storage 的路径
sed -i "s|/home/yuqing/fastdfs|$STORAGE_BASE_PATH|g" /etc/fdfs/storage.conf
sed -i "s|/home/yuqing/fastdfs|$STORAGE_BASE_PATH|g" /etc/fdfs/client.conf
sed -i "s|^store_path0.*$|store_path0=$STORAGE_BASE_PATH$PATH0|g" /etc/fdfs/storage.conf

# 配置nginx端口
sed -i "s/http.server_port=.*$/http.server_port=$NGINX_WEB_PORT/g" /etc/fdfs/storage.conf

# storage 端口
sed -i "s/^port=.*$/port=$STORAGE_SERVER_PORT/g" /etc/fdfs/storage.conf

# storage 中的 groupname
sed -i "s/^group_name=group1.*$/group_name=$STORAGE_GROUP_NAME/g" /etc/fdfs/storage.conf

# client.conf 中 tracker_server 的相关配置: ip地址和端口
sed -i "s/http.tracker_server_port=.*$/http.tracker_server_port=$NGINX_WEB_PORT/g" /etc/fdfs/client.conf
sed -i "s/^tracker_server=.*$/tracker_server=$TRACKER_SERVER_IP_ADDRESS:$TRACKER_SERVER_PORT/g" /etc/fdfs/client.conf
sed -i "s/^http.tracker_server_port=.*$/http.tracker_server_port=$NGINX_WEB_PORT/g" /etc/fdfs/client.conf

# storage.conf 中 tracker 节点的配置
sed -i "s/^tracker_server=.*$/tracker_server=${TRACKER_SERVER_IP_ADDRESS}:${TRACKER_SERVER_PORT}/g" /etc/fdfs/storage.conf

# storage.conf 配置子文件数量,默认是256
sed -i "s/^subdir_count_per_path=.*$/subdir_count_per_path=$STORAGE_SUBDIR_COUNT_PER_PATH/g" /etc/fdfs/storage.conf

# nginx使用 mod_fastdfs.conf 配置
sed -i "s|^store_path0.*$|store_path0=$STORAGE_BASE_PATH$PATH0|g" /etc/fdfs/mod_fastdfs.conf
sed -i "s|^url_have_group_name=.*$|url_have_group_name= true|g" /etc/fdfs/mod_fastdfs.conf
sed -i "s/^tracker_server=.*$/tracker_server=$TRACKER_SERVER_IP_ADDRESS:$TRACKER_SERVER_PORT/g" /etc/fdfs/mod_fastdfs.conf
sed -i "s/listen.*$/listen $NGINX_WEB_PORT;/g" /usr/local/nginx/conf/nginx.conf

# http.conf 的默认图片配置
sed -i "s|^http.anti_steal.token_check_fail.*|http.anti_steal.token_check_fail=/etc/fdfs/anti-steal.jpg|g" /etc/fdfs/http.conf

# 创建需要的文件目录
# $TRACKER_BASE_PATH: tracker的根目录,其下放 logs和data
# $STORAGE_BASE_PATH: storage的根目录,logs和data
# $STORAGE_BASE_PATH$PATH0: storage的存储目录
# $STORAGE_BASE_PATH/data: 这个目录要存在,否则报错
mkdir -p $TRACKER_BASE_PATH $STORAGE_BASE_PATH $STORAGE_BASE_PATH$PATH0 $STORAGE_BASE_PATH/data

SERVER=$1

# 如果启动命令中有 tracker,则启动tracker
if [ "${SERVER}" == "tracker" ]; then
  /etc/init.d/fdfs_trackerd start
  tail -F /tracker/logs/trackerd.log
# 如果启动命令中为storage,则启动stroage和nginx
elif [[ "${SERVER}" == "storage" ]]; then
#  /etc/init.d/fdfs_storaged start
#  /usr/local/nginx/sbin/nginx
  tail -F /storage/logs/storaged.log
else
  # 命令为空,默认启动所有的服务
  /etc/init.d/fdfs_trackerd start
  /etc/init.d/fdfs_storaged start
#  /usr/local/nginx/sbin/nginx
#  tail -F /usr/local/nginx/logs/access.log
fi
exec "$@"
