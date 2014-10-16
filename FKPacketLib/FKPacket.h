//*************************************************************************
//	��������:	2014-10-14   19:10
//	�ļ�����:	FKPacket.h
//  �� �� ��:   ������ FreeKnight	
//	��Ȩ����:	MIT
//	˵    ��:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdio.h>
#include "../ZIP/zlib.h"
//-------------------------------------------------------------------------
using namespace std;
//-------------------------------------------------------------------------
typedef char FKCHAR;
//-------------------------------------------------------------------------
class CFKPacket
{
private:
	// ��ͷ
	struct SPacketHead
	{
		FKCHAR				m_szFileID[6];				// ��ͷ FKPAK
		FKCHAR				m_szVersion[4];				// �汾��
		int					m_nFileNum;					// �ļ�����
		bool				m_bIsUseEncrypt;			// �Ƿ��ÿ�ֽڽ��м���
		FKCHAR				m_cEncryptVal;				// ���ܵ�Ԫ
		bool				m_bIsZipComperssed;			// �Ƿ�zipѹ����
	};
	// �ļ���Ԫ��Ϣ
	struct SFileEntry
	{
		FKCHAR				m_szFileName[128];			// �ļ���
		FKCHAR				m_szFileFullPath[256];		// �������ļ�·��
		unsigned int		m_unSize;					// �ļ���С
		unsigned int		m_unCompressedSize;			// �ļ�ѹ����Ĵ�С
		unsigned int		m_unOffset;					// �ļ���pak�е�ƫ����
	};
	// �ļ�β
	struct SFileTail
	{
		unsigned int		m_unFileEntryOffset;		// �ļ���Ԫ��Ϣ���ļ�ͷ��ƫ����
	};
	enum ENUM_FileState
	{
		eFS_Deleted			= -1,
		eFS_Normal			= 0,
		eFS_Added			= 1
	};
private:
	string					m_szPakName;				// ����			
	bool					m_bIsLoadedPak;				// �Ƿ��Ѿ����ر���
	SPacketHead				m_tagPakHead;				// ��ͷ
	vector<SFileEntry>		m_vecFileEntries;			// �ļ������ı�
	vector<ENUM_FileState>	m_vecChanges;				// �ļ�״̬
	SFileTail				m_tagFileTail;				// �ļ�β��Ϣ
private:
	// �ַ����ָ�����Ҫ���ļ�����
	vector<string>			__FileTypes( string p_szTypes );
	// �����ļ���Ԫ��Ϣ
	bool					__CreateEntry( string p_szRootPath, string p_szRelativePath, string p_szFileName );	
	// ��ȡ���ļ����б�
	vector<string>			__GetSubDir( string p_szParentDir, string p_szRootDir );
	// �ж�һ���ļ��Ƿ���Ҫѹ������ΪͼƬ��ʽѹ���ʹ��ͣ������㣬Ĭ�ϲ�ѹ����
	bool					__IsNeedCompress( string p_szFileName );
public:
	CFKPacket();
	~CFKPacket();
public:
	// ���һ��pak
	// �����Ҫ��ָ��������ļ����д�����������������������ƴ��� ".jpg|.png|.bmp"
	bool					CreatePAK( string p_szPakName, string p_szSrcRootPath, bool p_bIsUseCompress = true, bool p_bIsEntry = false, string p_szType = "" );
	// ����һ��pak
	bool					ReadPAK( string p_szPakPath );
public:
	//----------------------------------------------------------
	// ���к��������� ReadPAK() ֮���������
	//----------------------------------------------------------
	// Ϊpak�����/ɾ��һ���ļ�
	// ע�⣺���ú������ RebuildPAK() ����֤���´��
	bool					AppendFile( string p_szRootPath, string p_szRelativePath, string p_szFileName );
	bool					RemoveFile( string p_szFilePath );
	// ���´��
	// ע�⣺��������Ϣ���κθ��ģ���Ȼ����false
	bool					RebuildPAK();
	// ���Ҳ���ȡһ���ļ�ָ�루��ѹ��
	FKCHAR*					GetFileDataFromPAK( string p_szFileName );
	// ���Ҳ���ȡһ���ļ���Ϣ��PAK�е�λ��ָ��
	SFileEntry*				GetFileInfoFromPAK( string p_szFileName );
	// ���ĳ�ļ��ڰ��ڴ�С
	int						GetFileSize( string p_szName );
	int						GetFileCompressSize( string p_szName );
	// ��ð���ȫ���ļ������б�
	vector<string>			GetAllFileNameInPAK();
	// ��ȡ�����ļ�����
	int						GetFileNumInPAK();
	// ���һ��PAK�ڵ��ļ�
	bool					UnpakEntry( string p_szFileName, string p_szFilePath );
	// ���ȫ������
	void					Clear();
};
//-------------------------------------------------------------------------