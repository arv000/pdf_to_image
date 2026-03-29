
```bash
#构建
docker compose up -d --build
#运行
docker compose up -d  
```
# 前端构建docker
```bash
# 构建
docker build -t frontend:1.0 .
# 运行
docker run -d --name frontend-fix -p 80:80 frontend:1.0
```


```bash
 docker rm -f $(docker ps -aq)
```