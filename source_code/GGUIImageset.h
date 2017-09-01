//----------------------------------------------------------------
#ifndef _GGUIImageset_h_
#define _GGUIImageset_h_
//----------------------------------------------------------------
#include "GGUIDefine.h"
//----------------------------------------------------------------
class SoD3DTexture;
//----------------------------------------------------------------
class GGUIImageset
{
public:
	void AddRect(const SoTinyString& kName, const GGUIRect& kRect);
	int GetRectID(const SoTinyString& kName) const;
	const GGUIRect& GetRect(int nID) const;
	int GetRectCount() const;
	//返回值为ID3D11ShaderResourceView*类型。
	void* GetTexture() const;
	float GetTextureWidth() const;
	float GetTextureHeight() const;

protected:
	friend class GGUIImagesetManager;
	GGUIImageset();
	~GGUIImageset();
	//初始化函数。
	//--nInitRectCount 预分配多少个GGUIRect。
	bool InitImageset(int nInitRectCount);
	void ClearImageset();
	void SetD3DTexture(SoD3DTexture* pD3DTexture);

private:
	typedef std::map<SoTinyString, int> mapName2ID;

private:
	//数组，存储的元素是GGUIRect。
	SoArrayUID m_kRectArray;
	//GGUIRect名字到GGUIRect序号的映射。
	mapName2ID m_kName2IDMap;
	//贴图对象指针。
	SoD3DTexture* m_pD3DTexture;
};
//----------------------------------------------------------------
#endif //_GGUIImageset_h_
//----------------------------------------------------------------
