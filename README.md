
# Linux-5.15 内核代码注释

## Debug说明
使用QEMU + GDB + Vscode调试

QEMU，GDB，Vscode等软件安装参考官方文档

```shell
& make menuconfig

& make all -j$(nproc)

& cd ./debug

& make run_debug
```

QEMU仿真环境中启动内核后，Vscode中使用GDB通过端口1234远程调试

launch.json文件如下:
```json
{
    // 使用 IntelliSense 了解相关属性。 
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) linux",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/vmlinux",
            "miDebuggerServerAddress": "localhost:1234",
            "args": [],
            "stopAtEntry": true,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "将反汇编风格设置为 Intel",
                    "text": "-gdb-set disassembly-flavor att",
                    "ignoreFailures": true
                }
            ]
        }

    ]
}
```

## debug文件夹Makefile target说明

### run_debug
启用qemu仿真当前目录Linux内核，并等待gdb远程连接进行调试

### run_boot
启用qemu仿真当前目录Linux内核，持续运行，日志文件输出到boot.log

### run_init
启用qemu仿真系统Linux内核

### hello_initramfs
输出hello world的init程序

### sh_initramfs
运行shell的init程序