
# Linux-5.15 内核代码注释

## Debug说明
使用QEMU + GDB + Vscode调试
```shell
& make menuconfig

& make all -j$(nproc)

& cd ./debug

& make run_debug
```
QEMU仿真环境中启动内核后，Vscode中使用GDB通过端口1234远程调试
