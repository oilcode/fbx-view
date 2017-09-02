# fbx-view

a directx11 demo for loading and rendering fbx format file which exported from 3ds-max

本demo的主要目标是加载并渲染fbx模型。
fbx文件是3ds max软件导出来的模型文件，可以是静态模型，也可以是含有动画信息的动画模型。
Unity3D引擎就是使用fbx文件作为主要的模型文件。
本demo可以正确处理fbx静态模型，也可以正确处理动画模型。


# 关键代码

#2 Loading fbx :
StFBXDefine.cpp
StFBXDefine.h
StFBXManager.cpp
StFBXManager.h
StFBXModel.cpp
StFBXModel.h
StFBXUserDefine.h

#2 Rending fbx :
SoD3DModelFbx.cpp
SoD3DModelFbx.h
SoD3DLogicFlowHelp.cpp


# 操作方法
程序运行后，把fbx文件拖放到程序窗口内，fbx模型就会被加载并渲染出来。
把 png,dds,tga 贴图拖放到程序窗口内，fbx模型就会蒙上该贴图。

