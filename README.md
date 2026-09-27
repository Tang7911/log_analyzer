# Log Analyzer ——轻量级日志采集与分析系统

该项目用于对linux系统的日志进行采集、分析，得到错误数据、延迟时间等数据，协助系统优化。

## 功能特性

- 特性1：C++读取日志文件，逐行截取日志的每个字段存入vector容器，再将处理好的数据转换为CSV格式存入
- 特性2：数据暂时保存在CSV文件内，后续将增加存入SQLlite的功能
- 特性3：Python用来生成伪日志和利用Pandas、Matplotlib等库进行数据分析，生成可视化图表
- 特性4：产出对系统日志的可视化图表分析

## 系统架构

|![](./jiagou.png)

## 技术栈

- C++、Python、Pandas、Matplotlib、linux

## 目录结构

```text
log_analyzer __________________________
            |
            |_Analysis_________________
            |          |_analyze.ipynb
            |
            |_Convert____________
            |         |_main
            |         |_main.cpp
            |
            |_Data________________________
            |      |_action_bar.png
            |      |_app.log
            |      |_generate_logs.py
            |      |_latency_his.png
            |      |_level_pie.png
            |      |_output.csv
            |      |_timestamp_error.png
            |
            |_gitignore
            |_jiagou.png
            |_README.md
```

## 快速开始

### 1. 生成模拟日志
```bash
    python generate_logs.py
```

### 2. 编译并运行 C++ 编译器
```bash
    g++ main.cpp -o main
```

### 3. 运行 Python 分析
```bash
    用 jupyter notebook 运行 analyze.ipynb
```

## 数据展示

### 日志级别分布
![Level Distribution](./Data/level_pie.png)

### 动作调用统计
![Action Distribution](./Data/action_bar.png)

### 耗时分布
![Latency Distribution](./Data/latency_hist.png)

### 日志量时间趋势
![Time Trend](./Data/timestamp_error.png)

## 后续计划

- [1]增加存入SQLlite
- [2]对异常日志的预警