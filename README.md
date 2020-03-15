# meta
使用libclang来处理c++代码用来做c++反射代码自动生成

## 编译依赖

1. c++ 17
2. libclang  用来parse c++代码 windows下载链接  http://llvm.org/builds/  linux下载链接  https://apt.llvm.org/ 
3. json  https://github.com/nlohmann/json   用来导出数据
4. spdlog  https://github.com/gabime/spdlog 用来打log
5. mustache  https://github.com/kainjow/Mustache  用做代码模板来生成代码
6. any_container https://github.com/huangfeidian/any_container 我自己的一个模板序列化与反序列化库 用来处理encode和decode部分

## 关于反射

这个专题我在知乎上有专栏来介绍，主要有如下几篇：

1. meta项目的起源，自动提取行为树的导出函数接口，实现函数的按名调用 https://zhuanlan.zhihu.com/p/98822590 
2. 分析了现有的c++反射的主要内容和相关解决方案 https://zhuanlan.zhihu.com/p/99017367
3. 分析了qt unreal等框架自带的解决方案 https://zhuanlan.zhihu.com/p/99223617
4. 介绍了如何使用libclang做代码信息标记和提取相关 https://zhuanlan.zhihu.com/p/99427620
5. 详细介绍了一个完整的按名调用是如何实现的 https://zhuanlan.zhihu.com/p/100878076
6. 详细介绍了属性同步是怎么实现的 https://zhuanlan.zhihu.com/p/102034322



## 运行原理

libclang可以作为前端parse c++的代码，获取所有的类型声明、变量声明、函数声明，我们可以把这些声明都抓取出来。

此外我们还可以使用宏去标记函数、变量、类的annotate 属性，这个annotate属性可以被libclang捕捉，但是在其他编译器中只是一个nop。在处理项目代码的时候作为这些c++语法的补充信息，可以导出，我们可以利用这些导出信息，来填充mustache模板，就跟网页模板一样，最后生成合法的c++代码，插入到原始代码之中，最后再进行一遍编译。

## 功能介绍

当前repo下有多个工程，用来提供meta项目的主要使用方法

1. meta项目 dump 所有的类型声明、变量声明、函数声明，生成类型数据库，并支持导出， 这个项目是基础项目，是所有其他项目的依赖

2. generate_enum 用来处理enum的反射 不过对于enum推荐使用 

   [magic_enum]: https://github.com/Neargye/magic_enum

   来操作反射相关

3. generate_encode 用来将class自动生成对应的encode方法，序列化为json

4. generate_decode 用来将class自动生成对应的decode方法，从json反序列化出来

5. generate_attr 用来提供class的变量和函数的按名查找功能，类似于python的getattr

6. generate_rpc 用来提供网络通信之间的rpc接口生成，处理数据的自动打包和解包，类函数的自动调用

7. generate_property 用来生成容器的修改镜像同步方法，这个镜像同步原型参考 https://github.com/huangfeidian/property_sync

8. generate_behavior 用来生成行为树的actions的导出，关于行为树项目，参考 https://github.com/huangfeidian/behavior_tree

9. generate_component 用来实现代码的组件自动注册，参考的是当前项目下的python文件夹

当然，整个meta项目的功能潜力不限于此，只要你能找到合适的自动化代码生成应用场景，都可以继续扩展功能。


