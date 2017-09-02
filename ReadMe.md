# fbx-view

a directx11 demo for loading and rendering fbx format file which exported from 3ds-max <br>
<br>
本demo的主要目标是加载并渲染fbx模型。<br>
fbx文件是3ds max软件导出来的模型文件，可以是静态模型，也可以是含有动画信息的动画模型。<br>
Unity3D引擎就是使用fbx文件作为主要的模型文件。<br>
本demo可以正确处理fbx静态模型，也可以正确处理动画模型。<br>
<br>
<br>
# 关键代码
<br>
### Loading fbx :
StFBXDefine.cpp <br>
StFBXDefine.h <br>
StFBXManager.cpp <br>
StFBXManager.h <br>
StFBXModel.cpp <br>
StFBXModel.h <br>
StFBXUserDefine.h <br>
<br>
### Rending fbx :
SoD3DModelFbx.cpp <br>
SoD3DModelFbx.h <br>
SoD3DLogicFlowHelp.cpp <br>
<br>
<br>
# 操作方法
程序运行后，把fbx文件拖放到程序窗口内，fbx模型就会被加载并渲染出来。 <br>
如果是动画模型，则会播放动画。 <br>
把 png,dds,tga 贴图拖放到程序窗口内，fbx模型就会蒙上该贴图。 <br>
<br>

