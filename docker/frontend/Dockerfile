FROM node:18 AS build

WORKDIR /app

# 复制依赖文件
COPY package*.json ./

# 设置镜像源并安装依赖
RUN npm config set registry https://registry.npmmirror.com --global && \
    npm install

# 复制源代码并构建
COPY . .
RUN npm run build

# 生产阶段
FROM nginx:alpine

# 复制构建产物
COPY --from=build /app/dist /usr/share/nginx/html

# 复制自定义 nginx 配置（可选）
COPY nginx.conf /etc/nginx/nginx.conf

EXPOSE 80

# 启动 nginx
CMD ["nginx", "-g", "daemon off;"]