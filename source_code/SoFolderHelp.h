//------------------------------------------------------------
#ifndef _SoFolderHelp_h_
#define _SoFolderHelp_h_
//------------------------------------------------------------
#include <string>
#include <vector>
//------------------------------------------------------------
class SoFolderHelp
{
public:
	
	//将路径进行转换，统一转换为小写，转换\\为/，然后在路径的末尾加上/，
	//同时需要去除头部的'.'和'/'
	static void ConvertPathName(const char* filePath, std::string& convertedFilePath);

	//遍历指定的文件夹及其子文件夹，把文件名（包含相对路径）存储在vector内。
	//--pMainPath 文件夹路径必须以'/'结尾。必须是有效路径。
	//--pSubPath 其值可以为NULL。如果不为NULL，表示要遍历的文件夹为 pMainPath+pSubPath ，路径必须以'/'结尾。
	//--vecFiles 返回值
	static void ScanFolder(const char* pMainPath, const char* pSubPath, std::vector<std::string>& vecFiles);
};
//------------------------------------------------------------
#endif //_SoFolderHelp_h_
//------------------------------------------------------------
