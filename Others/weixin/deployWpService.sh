#!/bin/bash

path=$(cd `dirname $0`;pwd)

#1. NGINX
mkdir -p nginx/config/conf.d
mkdir -p nginx/data
mkdir -p nginx/logs
mkdir -p nginx/ssl

nginx_conf='nginx/config/nginx.conf'
nginx_default_conf='nginx/config/conf.d/default.conf'

#1.1. content of nginx.conf file
echo "user  nginx;" > $nginx_conf
echo -e "worker_processes  1;\n" >> $nginx_conf

echo "error_log  /var/log/nginx/error.log warn;" >> $nginx_conf
echo -e "pid        /var/run/nginx.pid;\n" >> $nginx_conf

#1.1.1. nginx.conf event module 
echo "events {" >> $nginx_conf
echo -e "\tworker_connections 1024;" >> $nginx_conf
echo -e "}\n" >> $nginx_conf

#1.1.2. nginx.conf http module 
echo "http {  " >> $nginx_conf
echo -e "\tinclude       /etc/nginx/mime.types;" >> $nginx_conf
echo -e "\tdefault_type  application/octet-stream;\n" >> $nginx_conf

echo -e "\tlog_format  main  '\$remote_addr - \$remote_user [\$time_local] \"\$request\" '" >> $nginx_conf
echo -e "\t'\$status \$body_bytes_sent \"\$http_referer\" '" >> $nginx_conf
echo -e "\t'\"\$http_user_agent\" \"\$http_x_forwarded_for\"';\n" >> $nginx_conf

echo -e "\taccess_log  /var/log/nginx/access.log  main;\n" >> $nginx_conf

echo -e "\tsendfile        on;" >> $nginx_conf
echo -e "\t#tcp_nopush     on;\n" >> $nginx_conf

echo -e "\tkeepalive_timeout  65;" >> $nginx_conf
echo -e "\t#gzip  on;\n" >> $nginx_conf

echo -e "\tinclude /etc/nginx/conf.d/*.conf;" >> $nginx_conf
echo "}" >> $nginx_conf


#1.2. content of default.conf file
echo "server {" > $nginx_default_conf
echo -e "\tlisten    80;" >> $nginx_default_conf
echo -e "\tlisten 	  443 ssl;" >> $nginx_default_conf
echo -e "\tserver_name  www.buagengen.com;\n" >> $nginx_default_conf
	
#ssl
echo -e "\t#ssl on;" >> $nginx_default_conf
echo -e "\tssl_certificate /ssl/server.crt;" >> $nginx_default_conf
echo -e "\tssl_certificate_key /ssl/server.key;\n" >> $nginx_default_conf
	
echo -e "\tssl_session_cache    shared:SSL:1m;" >> $nginx_default_conf
echo -e "\tssl_session_timeout  5m;\n" >> $nginx_default_conf
	
echo -e "\tssl_protocols  SSLv2 SSLv3 TLSv1.2;\n" >> $nginx_default_conf 
  
echo -e "\tssl_ciphers  HIGH:!aNULL:!MD5;" >> $nginx_default_conf 
echo -e "\tssl_prefer_server_ciphers  on;\n" >> $nginx_default_conf

# 定义首页索引目录和名称
echo -e "\tlocation / {" >> $nginx_default_conf
echo -e "\t\tproxy_pass http://192.168.2.18:8080;" >> $nginx_default_conf
echo -e "\t\tproxy_set_header Host \$host;" >> $nginx_default_conf
echo -e "\t\tproxy_set_header X-Real-IP \$remote_addr;" >> $nginx_default_conf
echo -e "\t\tproxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;" >> $nginx_default_conf
echo -e "\t}\n" >> $nginx_default_conf

echo -e "\terror_page   500 502 503 504  /50x.html;" >> $nginx_default_conf
echo -e "\tlocation = /50x.html {" >> $nginx_default_conf
echo -e "\t\troot   /usr/share/nginx/html;" >> $nginx_default_conf
echo -e "\t}\n" >> $nginx_default_conf

echo -e "}" >> $nginx_default_conf



#Load the nginx image
if [[ ! `docker images | grep nginx` =~ nginx ]]; then
	if [ -f "nginx.tar" ]; then
		echo "Load Nginx Image"
		docker load < nginx.tar
	else
		echo "No nginx.tar available in currently DIR"
		exit -1
	fi
fi


#Stop the nginx if any
if [[ `docker ps -a | grep nginx` =~ wx-nginx ]]; then
	echo "Remove the origin nginx container"
	docker rm -f wx-nginx
fi

#Copy the needed ssl files
cp server.* $path/nginx/ssl

#Start the container
docker run --detach \
	--name wx-nginx \
	-p 443:443\
	-p 80:80 \
	-v $path/nginx/data:/usr/share/nginx/html:rw\
	-v $path/nginx/config/nginx.conf:/etc/nginx/nginx.conf/:rw\
	-v $path/nginx/config/conf.d/default.conf:/etc/nginx/conf.d/default.conf:rw\
	-v $path/nginx/logs:/var/log/nginx/:rw\
	-v $path/nginx/ssl:/ssl/:rw\
	-d nginx

sleep 2
if [[ `docker ps -a | grep nginx` =~ wx-nginx ]]; then
	echo "Nginx Start Success!"
fi


#2. MySQL
mkdir -p mysql/conf
mkdir -p mysql/data

docker_cnf='mysql/conf/docker.cnf'
mysql_cnf='mysql/conf/mysql.cnf'

#2.1 MySQL Docker container configure file content
echo "[mysqld]" > $docker_cnf
echo "skip-host-cache" >> $docker_cnf
echo "skip-name-resolve" >> $docker_cnf

#2.2 MySQL configure file content
echo "[mysql]" > $mysql_cnf

#2.3 Start MySQL container
#Load the mysql image
if [[ ! `docker images | grep mysql` =~ mysql ]]; then
	if [ -f mysql.tar ]; then
		echo "Load mysql image"
		docker load < mysql.tar
	else
		echo "No mysql.tar available in currently DIR"
		exit -1
	fi
fi

#Stop the mysql if any
if [[ `docker ps -a | grep mysql` =~ wx-mysql ]]; then
	echo "Remove the origin mysql container"
	docker rm -f wx-mysql
fi

#Start the container
docker run --name wx-mysql \
	-v $path/mysql/conf:/etc/mysql/conf.d \
	-e MYSQL_ROOT_PASSWORD=/etc/mysql/conf.d \
	-d mysql:latest

sleep 2
if [[ `docker ps -a | grep mysql` =~ wx-mysql ]]; then
	echo "MySQL Start successfully"
fi


#3. WordPress
mkdir -p wordpress

if [[ ! `docker images | grep wordpress` =~ wordpress ]]; then
	if [ -f wordpress.tar ]; then
		echo "Load wordpress image"
		docker load < wordpress.tar
	else
		echo "No wordpress.tar available in currently DIR"
		exit -1
	fi
fi

#Stop the wordpress if any
if [[ `docker ps -a | grep wordpress` =~ wx-wordpress ]]; then
	echo "Remove the origin wordpress container"
	docker rm -f wx-wordpress
fi

#Start the container
docker run --name wx-wordpress \
	--link wx-mysql:mysql \
	-p 8080:80 \
	-d wordpress:latest


sleep 2
if [[ `docker ps -a | grep wordpress` =~ wx-wordpress ]]; then
	echo "wordpress Start successfully"
fi

