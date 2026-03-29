FROM python:3.10-slim

WORKDIR /app

# 正确配置 Debian 源（清华大学镜像）
#RUN cat > /etc/apt/sources.list << "EOF"
#deb https://mirrors.tuna.tsinghua.edu.cn/debian bullseye main contrib non-free
#deb https://mirrors.tuna.tsinghua.edu.cn/debian bullseye-updates main contrib non-free
#deb https://mirrors.tuna.tsinghua.edu.cn/debian bullseye-backports main contrib non-free
#deb https://mirrors.tuna.tsinghua.edu.cn/debian-security bullseye-security main contrib non-free
#EOF

# 删除可能存在的默认源配置
RUN rm -f /etc/apt/sources.list.d/debian.sources

# 更新包列表并安装系统依赖
RUN apt-get update 
RUN pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
RUN pip install --upgrade pip
RUN pip install uvicorn   PyMuPDF

# 复制并安装 Python 依赖
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple

# 复制应用代码
COPY . .

# 启动命令
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
