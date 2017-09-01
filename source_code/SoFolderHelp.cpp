//------------------------------------------------------------
#include "SoFolderHelp.h"
#include <io.h>
#include <windows.h>
#include <algorithm>
#include <cctype>
//------------------------------------------------------------
void SoFolderHelp::ConvertPathName(const char* filePath, std::string& convertedFilePath)
{
	if (filePath == NULL)
	{
		return;
	}
	if (*filePath == 0)
	{
		return;
	}

	convertedFilePath = filePath;
	if(!convertedFilePath.empty())
	{
		const char& last = convertedFilePath[convertedFilePath.length() - 1];

		if((last != '/') && (last != '\\'))
		{
			convertedFilePath.append(1, '/');
		}
	}

	unsigned int length = (unsigned int)convertedFilePath.length();
	for (unsigned int iChar = 0; iChar < length; ++iChar)
	{
		if (convertedFilePath[iChar] == '\\')
		{
			convertedFilePath[iChar] = '/';
		}
		else
		{
			convertedFilePath[iChar] = tolower(convertedFilePath[iChar]);
		}
	}

	if ((!convertedFilePath.empty()) && (convertedFilePath[0] == '.'))
	{
		convertedFilePath.erase(0, 1);
	}

	if ((!convertedFilePath.empty()) && (convertedFilePath[0] == '/'))
	{
		convertedFilePath.erase(0, 1);
	}
}
//------------------------------------------------------------
void SoFolderHelp::ScanFolder(const char* pMainPath, const char* pSubPath, std::vector<std::string>& vecFiles)
{
	//遍历当前目录下的所有的文件
	std::string strSubPath;
	if (pSubPath)
	{
		strSubPath = pSubPath;
	}
	std::string strSearchTarget = pMainPath;
	strSearchTarget += pSubPath;
	strSearchTarget += "*.*";
	struct _finddata_t fileInfo;
	//
	long handle = (long)_findfirst(strSearchTarget.c_str(), &fileInfo);
	if (handle != -1)
	{
		while (1)
		{
			if (!(fileInfo.attrib & _A_SUBDIR))
			{
				std::string strSubPathFile = strSubPath + fileInfo.name;
				vecFiles.push_back(strSubPathFile);
				OutputDebugStringA(strSubPathFile.c_str());
				OutputDebugStringA("\n");
			}
			//
			if (_findnext(handle, &fileInfo) == -1)
			{
				break;
			}
		}
		//
		_findclose(handle);
	}

	//遍历当前目录下的所有的子目录，并对每个子目录执行遍历操作
	handle = (long)_findfirst(strSearchTarget.c_str(), &fileInfo);
	if (handle != -1)
	{
		while (1)
		{
			if (fileInfo.attrib & _A_SUBDIR && fileInfo.name[0] != '.')
			{
				std::string name = fileInfo.name;
				std::transform(name.begin(), name.end(), name.begin(), std::tolower);

				std::string strNewSubPath = strSubPath;
				strNewSubPath += name;
				strNewSubPath += '/';
				ScanFolder(pMainPath, strNewSubPath.c_str(), vecFiles);
			}
			//
			if (_findnext(handle, &fileInfo) == -1)
			{
				break;
			}
		}
		//
		_findclose(handle);
	}
}
//------------------------------------------------------------
