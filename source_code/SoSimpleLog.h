//-----------------------------------------------------------------------------
#ifndef _SoSimpleLog_h_
#define _SoSimpleLog_h_
//-----------------------------------------------------------------------------
class SoSimpleLog
{
public:
	static bool CreateSimpleLog();
	static void ReleaseSimpleLog();
	static SoSimpleLog* Get();

public:
	SoSimpleLog();
	~SoSimpleLog();
	//szFileName是文件名的一部分，完整文件名样式为"[szFileName]_20160705.txt"。
	bool InitSimpleLog(const char* szFileName, bool bOutputDebugString, bool bFlushImmediately);
	void ClearSimpleLog();
	void OutputDebug(const char* pFormat, ...);
	void OutputWarning(const char* pFormat, ...);
	void OutputError(const char* pFormat, ...);

private:
	//
	void AddLogHead(char* pType);
	void AddLogBody(const char* pFormat, const void* pVaList);
	//在与exe同目录下新建一个log文件。
	//该log文件名的末尾会加上时间戳，确保每次都是新的文件。
	bool OpenDiskFile(const char* szFileName);
	//关闭磁盘文件。
	void CloseDiskFile();

private:
	static SoSimpleLog* ms_pInstance;
private:
	void* m_fp;
	//为了保证线程安全，类对象使用自己的字符串缓存。
	char* m_pBuff;
	bool m_bOutputDebugString;
	bool m_bFlushImmediately;
};
//-----------------------------------------------------------------------------
inline SoSimpleLog* SoSimpleLog::Get()
{
	return ms_pInstance;
}
//-----------------------------------------------------------------------------
#define SoLogDebug if(SoSimpleLog::Get()) SoSimpleLog::Get()->OutputDebug
#define SoLogWarning if(SoSimpleLog::Get()) SoSimpleLog::Get()->OutputWarning
#define SoLogError if(SoSimpleLog::Get()) SoSimpleLog::Get()->OutputError
//-----------------------------------------------------------------------------
#endif //_SoSimpleLog_h_
//-----------------------------------------------------------------------------
