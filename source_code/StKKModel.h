//----------------------------------------------------------------
#ifndef _StKKModel_h_
#define _StKKModel_h_
//----------------------------------------------------------------
#include "StKKDefine.h"
//----------------------------------------------------------------
class StKKModel
{
public:
	StKKModel();
	~StKKModel();
	bool LoadKkbFile(const char* szFileName);
	bool LoadKkfFile(const char* szFileName);
	//
	void SetModelData(StKKModelData* pModelData);
	void ReserveAnimCount(int nAnimCount);
	void AddAnimation(StKKModelAnimation* pAnim);
	void ClearAllAnimation();
	//
	const StKKModelData* GetModelData() const;
	bool IsBoneDataExist() const;
	const StKKModelAnimation* GetAnimByID(int nAnimID) const;
	int GetKeyFrameIndexByID(int nAnimID, float fTime) const;
	void GetMeshBoundingBox(SoMathFloat3* pMinPos, SoMathFloat3* pMaxPos) const;

protected:
	void ClearKKModel();

private:
	StKKModelData* m_pModelData;
	StKKModelAnimation** m_pAnimList;
	int m_nAnimCount;
};
//----------------------------------------------------------------
#endif //_StKKModel_h_
//----------------------------------------------------------------
