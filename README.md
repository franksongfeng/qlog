# QLog说明文档

## 1. 软件概述

本软件是一个基于C++/Qt框架的开源日志功能库，旨在为开发者提供简单、高效的日志记录解决方案。它支持按照日志级别输出包含时间戳和日志内容的行文本；自动收集JSON类型参数的信号的日志；按名称动态调用INVOKABLE成员函数并记录日志；同时支持自定义文件容量和轮转式文件扩展等功能。

## 2. 功能特性

### 2.1 行日志输出

输出时间戳和自定义的日志内容。

### 2.2 JSON参数日志收集

- 自动收集JSON类型参数的信号的日志，方便记录和追踪程序中的自定义信号。

### 2.3 动态调用INVOKABLE成员函数

- 支持按名称动态调用INVOKABLE成员函数，并自动记录调用过程中的日志信息。

### 2.4 日志文件管理

- **自定义文件容量**：开发者可以根据需要设置日志文件的最大容量。
- **轮转式文件扩展**：当日志文件达到最大容量时，自动进行轮转，生成新的日志文件，避免日志文件过大影响性能。

## 3. 系统要求

- C++编译环境
- Qt开发框架

在Windows 10 + Qt 5.15.2 + MSVC 2019系统环境下调试通过。

## 4. 使用示例

### 4.1 添加日志对象

例如，给MainWindow类加上日志对象

in mainwindow.h:

```c++
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qlog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

Q_INVOKABLE int f_add(int a, int b);  // a invokable method

signals:
    void burn(QJsonObject child);  // a custom JSON signal

private:
    QLog *log;  // log object
};
#endif // MAINWINDOW_H
```

in mainwindow.cpp:

```c++
#include "mainwindow.h"

MainWindow::MainWindow(QObject *parent)
    : QMainWindow(parent)
    , log(new QLog(this))
{
    log->setIO(IO::QDOC);
    log->setLevel(Level::DBG);

    log->iLine("set a logger", "for mainwindow");  // print a log line on INFO level
}

MainWindow::~MainWindow()
{
    delete log;
}

int MainWindow::f_add(int a, int b)
{
    return a + b;
}
```

### 4.2 JSON参数信号收集

发出一个自定义的JSON Object信号

```c++
    QJsonObject child = {{"topic", "birth"}, {"male", True}, {"birthday", "20241007"}};
    burn(child);
```

### 4.3 动态调用INVOKABLE成员函数


```c++
    int sum, a, b;
    a = 100;
    b = 200;
    log->invokeParentRetSlot("f_add", sum, a, b);  // sum should be 300
```

### 4.4 查看日志文件

打开⽇志⽂件"MainWindow.log"，我们可以看到：

```apl
2025-02-20 16:22:15.438|set a logger, for mainwindow

2025-02-20 16:27:33.011|joSignal,{"topic":"birth","male":true,"birthday":"20241007"}

2025-02-20 16:27:50.905|invoke f_add BEFO, 100, 200
2025-02-20 16:27:50.907|invoke f_add POST, 300

```



## 5. 贡献与支持

本软件为开源项目，欢迎广大开发者参与贡献和支持。如有任何问题、建议或改进意见，请通过以下方式与我联系：

- GitHub仓库：https://github.com/franksongfeng/qlog
- 邮箱：43217183@qq.com

