# 编译环境
window操作系统
IDE工具 ： VSCode + CMakeTools

# 编译依赖
```
git clone https://github.com/microsoft/vcpkg.git  
cd vcpkg
vcpkg.exe install qt:x64-windows-static         # 这个安装非常大，一共使用了267G
vcpkg.exe install popple:x64-windows-static
```
# 编译配置
需要配置编译链工具
```
mkdir build
cd build 
cmake -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
-DVCPKG_TARGET_TRIPLET=x64-windows-static \
-DCMAKE_BUILD_TYPE=Relase ..
make
```

