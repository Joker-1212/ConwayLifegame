# ConwayLifegame++

## Brief Introduction

这里是 2025 级程设进阶 2 班大作业：强化版康威生命游戏 ConwayLifeGame++

在[康威生命游戏](https://baike.baidu.com/item/%E5%BA%B7%E5%A8%81%E7%94%9F%E5%91%BD%E6%B8%B8%E6%88%8F/22668799)的基础上，我们为其添加了如下强化规则：

1. 每个细胞都可以看到附近一定范围内的细胞。
2. 在下一时刻可以存活的细胞，可以选择向八个方向中的任意一个移动。当然，也可以选择不移动。
3. 由于细胞出现了智慧，消耗的资源增加了，每个细胞都有一定概率在下一时刻死亡。
4. 细胞现在被赋予了另一个属性：能量值。每次移动会消耗一定的能量值，每个回合都有一定概率回复一定能量值。没有能量的细胞不可移动
5. 如果多个细胞选择同时挤入一个空方格，由于空间不足，会随机让一个细胞进入该方格，而其余细胞无法移动。
6. 同时，我们抛弃了传统的康威游戏 2-3-3 的规则。现在，你可以自由指定你想要的规则！

在这些强化规则的基础上，我们用 `Pytroch` 实现了强化学习。现在，每个细胞都可以进行自主决策，并以群体发展为目标不断调整策略了！

## Features

1. 使用了 `Pytroch` 作为强化学习框架，以实现更好的训练效果
2. 使用 C++ 编写生命游戏程序内核，并使用 `pybind11` 将其暴露给 Python，实现了生命游戏的高效模拟。
3. 使用轻量开源的 `Dear PyGUI` 实现图形用户界面

## Installation and Compilation

### 1. 安装依赖

#### Python 依赖

使用 Conda 创建环境并安装 Python 依赖：

```bash
conda create -n conway
conda activate conway
conda install --yes --file requirements.txt
pip install dearpygui
```

或者使用 pip：

```bash
pip install -r requirements.txt
pip install dearpygui
```

#### 系统依赖

**Windows:**

- 安装 MSVC 编译器：[Visual Studio Build Tools](https://visualstudio.microsoft.com/zh-hans/downloads/#build-tools-for-visual-studio-2022)

**Linux:**

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install cmake g++ python3-dev python3-pip

# Fedora/RHEL
sudo dnf install cmake gcc-c++ python3-devel python3-pip

# Arch Linux
sudo pacman -S cmake gcc python
```

### 2. 构建项目

#### Windows 构建

运行构建脚本：

```bash
.\build.bat
```

#### Linux 构建

1. 给构建脚本添加执行权限：

```bash
chmod +x build.sh
```

2. 运行构建脚本：

```bash
./build.sh
```

### 3. 验证安装

运行测试脚本验证构建是否成功：

```bash
python test_windows.py
```

或者启动图形界面：

```bash
python GUI.py
```

### 4. 高级选项

如需手动编译或了解详细构建过程，请参阅 [ConwayLifegame++ 项目文档](Document.md)。

# Document

关于该程序的具体使用参见: [ConwayLifegame++ 项目文档](.\Document.md)
