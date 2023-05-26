# 使用说明

- [1 安装python](#1)
- [2 制作开机动画](#2)
  - [2.1 使用方法](#2.1)
  - [2.2 参数说明](#2.2)
  - [2.3 命令举例](#2.3)
- [3. 播放开机动画](#3)
  - [3.1 使用方法](#3.1)
  - [3.2 参数说明](#3.2)
  - [3.3 命令举例](#3.3)

**<p id="1">**
## 1 安装python
本工具需要安装python 3.8及以上版本，请根据环境选择下载对应的安装包。

Python下载地址：https://www.python.org/downloads/

**<p id="2">**
## 2 制作开机动画

***<p id="2.1">***
### 2.1 使用方法

```bash
python raw_maker.py (-m <*.mp4> | -i <directory>) [-o <directory>] [-d <size>] [-r <angle>] [-f]
```

***<p id="2.2">***
### 2.2 参数说明

```
-h, --help            查看命令帮助并退出
-m <*.mp4>, --mp4 <*.mp4>    
                      制作开机动画所依赖的<*.mp4>源文件
-i <directory>, --image <directory>
                      制作开机动画所依赖的源image文件存放路径<directory>
-o <directory>, --out <directory>
                      制作开机动画.raw 文件的输出路径 <directory>
-d <size>, --display <size>
                      设置开机动画分辨率, 举例:640x480
-r <angle>, --rotate <angle>
                      设置开机动画旋转角度, 举例:90°,180°或270°
-f, --flip            设置开机动画是否翻转

```
***<p id="2.3">***
### 2.3命令举例
1. 通过图片制作开机动画, -i 指定./source图片所在目录
```bash
python raw_maker.py -i ./source -d 640x480
```
2. 通过视频文件制作开机动画, 输出到out目录下并设置分辨率为640x480
```bash
python raw_maker.py -m ./animation.mp4 -o ./out -d 640x480
```

**<p id="3">**
## 3 播放开机动画

***<p id="3.1">***
### 3.1 使用方法

```bash
python raw_player.py [-h] <*.raw>
```

***<p id="3.2">***
### 3.2 参数说明

```
-h, --help            查看命令帮助并退出
```
**<p id="3.3">**
### 3.3 命令举例
播放bootanimation-640x480.raw文件
```bash
python raw_player.py ./bootanimation-640x480.raw
```