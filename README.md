# Conway Lifegame(Enhanced version)

## Brief Introduction

这里是 2025 级程设进阶 2 班大作业：强化版康威生命游戏

在[康威生命游戏](https://baike.baidu.com/item/%E5%BA%B7%E5%A8%81%E7%94%9F%E5%91%BD%E6%B8%B8%E6%88%8F/22668799)的基础上，我们为其添加了如下强化规则：

1. 每个细胞都可以看到附近一定范围内的细胞。
2. 在下一时刻可以存活的细胞，可以选择向八个方向中的任意一个移动。当然，也可以选择不移动。
3. 由于细胞出现了智慧，消耗的资源增加了，每个细胞都有一定概率在下一时刻死亡。细胞存活时间越长，死亡概率越大。具体而言：$死亡率 = 基础死亡率 \times \left\lfloor \frac{年龄}{10} \right\rfloor$
4. 细胞现在被赋予了另一个属性：能量值。每次移动会消耗一定的能量值，每个回合都有一定概率回复一定能量值。
5. 如果多个细胞选择同时挤入一个空方格，由于空间不足，会随机让一个细胞进入该方格，而其余细胞无法移动。

在这些强化规则的基础上，我们用 `Pytroch` 实现了强化学习。现在，每个细胞都可以进行自主决策，并以群体发展为目标不断调整策略了！

## Feature

1. 使用了 `Pytroch` 作为强化学习框架，以实现更好的训练效果
2. 使用 C++ 编写程序内核，并使用 `pybind11` 将其暴露给 Python，实现了生命游戏的高效模拟。
3. 使用了开源高效的图形库 `Dear PyGUI` 实现图形化界面

## Installation

在正式运行该项目前，请下载必要的 Python 第三方库（conda 的安装与使用可见 https://blog.csdn.net/ViatorSun/article/details/118578818）：

```
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/main/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/cloud/conda-forge/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/cloud/msys2/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/cloud/bioconda/
# 这一部分是可选的，只要你对自己的网速足够自信
conda create -n conway
conda activate conway
conda install --yes --file .\requirements.txt
pip install dearpygui -i https://mirrors.ustc.edu.cn/pypi/web/simple
```

## Compilation

该项目目前仅支持 `Windows` 下自动编译

1. 安装 MSVC 编译器：[https://visualstudio.microsoft.com/zh-hans/downloads/#build-tools-for-visual-studio-2022](https://visualstudio.microsoft.com/zh-hans/downloads/#build-tools-for-visual-studio-2022)

2. 在终端运行如下命令：

```bash
cd Your\dir\name
.\build.bat
python .\test_windows.py
```
