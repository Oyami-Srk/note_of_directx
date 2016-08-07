#  DirectX*11*学习笔记[1]

Author: [*OyaKti(SRK)*](http://blog.qvq.moe/ "OyaKti的弱鸡心得")

Date: 2016-7-2
## DirectX的简介
------
<!--more-->

<span id="home"></span>

[1.DirectX简介](#1)
[2.DirectX与OpenGL对比分析](#2)
[3.DirectX11于DirectX9的对比分析](#3)
[4.DirectX11的组件简介](#4)
[5.参考资料](#5)



* ## <span id="1">什么是DirectX?</span>
  ​	DirectX(Direct eXtension 下文简称DX)是微软公司开发的为多媒体和游戏开发的图形应用程序接口集(APIs),包含了Direct2D,Direct3D等多个子部分,使开发者能访问不同硬件(显卡声卡输入输出设备等)来进行开发而不必担心用户提供硬件硬件访问接口的不同.目前最新版本为DirectX12,适用于Windows 10操作系统之上.
  ​	DX被广泛用于Windows和Xbox的电子游戏开发方面,且仅能运行在这些平台,不过据推断,Linux平台或许将会被支持.
  ​	DX主要基于C++实现,遵循[COM架构](https://zh.wikipedia.org/wiki/COM "维基百科")
  ​	现阶段被广泛使用的DX图形接口的版本为9和11,被广大中国用户所使用的Windows Xp仅支持到DX9版本,DX11被Windows 7及以上版本所支持,DX12目前仅被Windows 10与Xbox One所支持,尚未开发Windows 7/8/8.1版本.

  [返回主页目录](#home)

* ## <span id="2">DirectX与OpenGL的对比分析</span>

  | OpenGL                                | DirectX              |
  | :------------------------------------ | -------------------- |
  | 优异的跨平台性能,可开发Android(GLES),Web(WebGL)等 | 不跨平台,仅限于Windows和Xbox |
  | 广泛应用于工控,医疗等领域                         | 活跃于游戏制作和多媒体娱乐        |
  | 版本更新拖泥带水,包含大量旧版无用API                  | 很少的向前支持,API干净利索      |
  | 非常依赖硬件加速                              | 与OpenGL相比不十分依赖硬件     |
  | 迭代速度相对较慢,十分落后                         | 与时俱进,遥遥领先于OpenGL     |
  | 各厂商独自拓展繁多,良莠不齐                        | 拓展统一,便于使用            |
  | 功能集中,需要大量外部库补全功能                      | 功能多,模块多,依赖外部库较少      |

  [返回主页目录](#home)

* ## <span id="3">DirectX11与DirectX9的对比分析</span>

  | DirectX11          | DirectX9            |
  | ------------------ | ------------------- |
  | 支持多线程渲染            | 仅限单线程渲染             |
  | 仅支持Windows7及以上操作系统 | 支持Windows Xp及以上操作系统 |
  | 比DX9易于使用           | 难学难懂难以使用            |
  | 使用可编程着色器(Shader)   | 使用固定管线              |
  | 其他渲染效果的增强          |                     |

  [返回主页目录](#home)

* ## <span id="3">DirectX11的组件简介</span>

  | 组件            | 简介                                 |
  | ------------- | ---------------------------------- |
  | Direct2D      | 用于2D图形的渲染                          |
  | Direct3D      | 用于3D图形的渲染,是更新最频繁的API,也是学习的重点       |
  | DirectWrite   | 用于字体和文字的渲染                         |
  | DXGI          | 图形基础设施库                            |
  | XAudio2       | 低级音频处理API                          |
  | XACT3         | 高级音频处理API                          |
  | XNA Math      | 数学组件                               |
  | Xinput        | 输入控制API,取缔了DirectInput             |
  | DirectInput   | 输入控制API,被继续保留                      |
  | DirectCompute | DX11新加入的API集,用于统合GPU(图形处理器)进行多线程计算 |
  | DirectSetup   | 游戏程序发布组件                           |

  [返回主页目录](#home)


* #### *<span id="5">参考资料</span>*
  [DirectX的Wiki百科界面](https://zh.wikipedia.org/wiki/DirectX/)

  [OpenGL的Wiki百科界面](https://zh.wikipedia.org/wiki/OpenGL)

  DirectX 11 游戏编程入门(Allen Sherrod and Wendy Jones 译者:looyer@sina.com)
  [返回主页目录](#home)

---

<center>*© 2016 Oyami(SRK)工作室*</center>
