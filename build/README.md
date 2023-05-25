# fangtian_build

## Intro

方天构建系统是基于 python 实现的构建框架，实现对方天视窗项目的一体化构建。

目前本系统采用 "gn + ninja + Clang" 的构建工具链。

## Get Started

* 项目首次运行前，请执行进行系统环境检查和依赖检查：`./build/prebuild.sh`。
* 项目根目录下直接运行 `./build.sh` 即可开始构建。
* 目前已支持 musl libc + llvm libc++ 编译项目，请运行 `./build.sh --enable-musl` 尝试编译，编译问题如果无法自行解决请反馈给开发人员。

## Usage

* 项目根目录下的 `build.sh` 是 `builder.py` 的 build 子命令快捷方式，用法请参考 `./build.sh -h`：

    ```shell
    usage: builder.py build [-h] [-t {x64,x86}] [-b {debug,release}] [-j JOBS] [--enable-musl] [-cc] [-v] [--escape-build]

    optional arguments:
      -h, --help            show this help message and exit
      -t {x64,x86}, --target-cpu {x64,x86}
                            Specify the target CPU type.
      -b {debug,release}, --build-type {debug,release}
                            Specify the build type
      -j JOBS, --jobs JOBS  Specify the number of jobs to run simultaneously during building.
      --enable-musl         Enable musl C lib. if musl is disabled, you'll use system C lib.
      -cc, --export-compile-commands
                            Export "compile_commands.json" file.
      -v, --verbose         Show all logs.
      --escape-build        Only do "prebuild" operations.
      -l {DEBUG,INFO,WARNING,ERROR,FATAL,CRITICAL}, --log-level {DEBUG,INFO,WARNING,ERROR,FATAL,CRITICAL}
                            Set log level of builder.
    ```

* 当然，我们可以直接运行 build 目录下的 `builder.py` 以尝试其提供的增量功能，包括：代码格式化、环境检查等内容：

    ```shell
    usage: builder.py [-h] [--project-dir PROJECT_DIR] [--version] [-l {DEBUG,INFO,WARNING,ERROR,FATAL,CRITICAL}] {build,format,check} ...

    Fangtian builder for OpenEuler OS.

    positional arguments:
      {build,format,check}  builder subcommands
        build               Build the project
        format              Format C/C++ & GN files
        check               System or project checking

    optional arguments:
      -h, --help            show this help message and exit
      --project-dir PROJECT_DIR
                            Specify the project root dir.
      --version             show program's version number and exit

    Please submit building issues to https://gitee.com/openeuler-graphics/fangtian_build.
    ```

## Dir Structure

```shell
├── build.sh
├── configs
├── builder
│   ├── commands
│   ├── common
├── builder.py
├── gn
│   ├── BUILDCONFIG.gn
│   ├── BUILD.gn
│   ├── configs
│   ├── dotfile.gn
│   ├── fangtian.gni
│   ├── templates
│   └── toolchain
├── prebuild.sh
└── README.md
```

* `build.sh`: 用于项目构建的 Shell 脚本，将会链接到项目根目录。
* `configs`: 包含项目依赖等内容的配置。
* `builder`：构建系统的核心目录。
* `builder.py`：构建系统入口。
* `gn`：GN 相关文件目录。
  * `dotfile.gn`：.gn file，该文件用于标记根目录。将会链接到项目根目录。
  * `BUILD.gn`：主 BUILD.gn，是所有其它 BUILD.gn 的入口点，它将会被链接到项目根目录方便修改。
  * `BUILDCONFIG.gn`：用于配置每个 toolchain 相对应的构建文件执行所需的环境变量。
  * `fangtian.gni`：本系统 GN 构建系统的配置主入口，应该被所有需要构建的仓库下 BUILD.gn 所引入。
  * `configs`：包含本系统 GN 构建所需的 GNI 文件。
  * `templates`：包含本系统 GN 构建所需的 GN 模板。
  * `toolchain`：包含本系统 GN 构建所需的 toolchain 配置。
* `prebuild.sh`：用于项目构建前检查的 Shell 脚本。

## Building System Dependences

### OS

* 目前本构建系统仅支持： **OpenEuler 22.03**。
* 本构建系统验证于： **OpenEuler 22.03 SP1**。

### Prebuilts

* `gn`
* `ninja`
* `libcxx` & `libcxxabi` & `libunwind` in [llvm-project](https://github.com/llvm/llvm-project)

### System Package Dependences

* `python3` (OpenEuler 22.03 built-in)
* `clang`
* `compiler-rt`
* `llvm-devel`

### Python3 Dependences

* `rich` (python3 built-in)
* `toml`： TOML 解析库。

## Guides

* [BUILD.gn 编写指南](docs/gn_guide.md)
* [RPM 包构建指南](docs/build_rpm_package_guide.md)
