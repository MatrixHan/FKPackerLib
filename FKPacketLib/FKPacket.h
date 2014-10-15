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
	};
	// �ļ���Ԫ��Ϣ
	struct SFileEntry
	{
		FKCHAR				m_szFileName[50];			// �ļ���
		FKCHAR				m_szFileFullPath[150];		// �������ļ�·��
		unsigned int		m_unSize;					// �ļ���С
		unsigned int		m_unOffset;					// �ļ���pak�е�ƫ����
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
private:
	// �ַ����ָ�����Ҫ���ļ�����
	vector<string>			__FileTypes( string p_szTypes );
	// �����ļ���Ԫ��Ϣ
	bool					__CreateEntry( string p_szFullPath, string p_szFileName );	
public:
	CFKPacket();
	~CFKPacket();
public:
	// ���һ��pak
	// �����Ҫ��ָ��������ļ����д�����������������������ƴ��� ".jpg|.png|.bmp"
	bool					CreatePAK( string p_szPakName, string p_szSrcRootPath, bool p_bIsEntry = false, string p_szType = "" );
	// ����һ��pak
	bool					ReadPAK( string p_szPakPath );
public:
	//----------------------------------------------------------
	// ���к��������� ReadPAK() ֮���������
	//----------------------------------------------------------
	// Ϊpak�����/ɾ��һ���ļ�
	// ע�⣺���ú������ RebuildPAK() ����֤���´��
	bool					AppendFile( string p_szFilePath );
	bool					RemoveFile( string p_szFilePath );
	// ���´��
	// ע�⣺��������Ϣ���κθ��ģ���Ȼ����false
	bool					RebuildPAK();
	// ���Ҳ���ȡһ���ļ�/�ļ���Ϣ��PAK�е�λ��ָ��
	FKCHAR*					GetFileDataFromPAK( string p_szFileName );
	SFileEntry*				GetFileInfoFromPAK( string p_szFileName );
	// ���ĳ�ļ��ڰ��ڴ�С
	int						GetFileSize( string p_szName );
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