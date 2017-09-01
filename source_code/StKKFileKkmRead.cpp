//----------------------------------------------------------------
#include "StKKFileKkmRead.h"
#include "StKKModel.h"
#include "StKKFileKkbRead.h"
#include "StKKFileKkfRead.h"
#include "SoReadFile.h"
#include "SoStringHelp.h"
#include "SoCmdLineHelp.h"
//----------------------------------------------------------------
bool StKKFileKkmRead::ReadKkm(const char* szFileName, StKKModel* pModel)
{
	if (szFileName == 0 || szFileName[0] == 0)
	{
		return false;
	}
	if (pModel == 0)
	{
		return false;
	}
	//
	SoReadFile kFile;
	if (kFile.LoadFile(szFileName) == false)
	{
		return false;
	}

	bool br = true;

	pModel->ReserveAnimCount(3);

	const char* szLine = kFile.GetFirstLine();
	int nLineLen = kFile.GetLineLen();
	while (szLine)
	{
		const char* szNameValue = SoCmdLineHelp::GetValueByKey(szLine, nLineLen, "name");
		if (SoStrCmp(szNameValue, "body") == 0)
		{
			const char* szFileKkb = SoCmdLineHelp::GetValueByKey(szLine, nLineLen, "file");
			StKKModelData* pModelData = new StKKModelData;
			if (StKKFileKkbRead::ReadKkb(szFileKkb, pModelData))
			{
				pModel->SetModelData(pModelData);
			}
			else
			{
				delete pModelData;
				br = false;
				break;
			}
		}
		else if (SoStrCmp(szNameValue, "anim") == 0)
		{
			const int nAnimID = SoCmdLineHelp::GetIntByKey(szLine, nLineLen, "id", -1);
			if (nAnimID != -1)
			{
				const char* szFileKkf = SoCmdLineHelp::GetValueByKey(szLine, nLineLen, "file");
				StKKModelAnimation* pAnim = new StKKModelAnimation;
				if (StKKFileKkfRead::ReadKkf(szFileKkf, pAnim))
				{
					pAnim->nAnimID = nAnimID;
					pModel->AddAnimation(pAnim);
				}
				else
				{
					delete pAnim;
					br = false;
					break;
				}
			}
		}
		szLine = kFile.GetNextLine();
		nLineLen = kFile.GetLineLen();
	}

	return br;
}
//----------------------------------------------------------------
