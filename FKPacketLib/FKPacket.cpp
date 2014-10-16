#include "FKPacket.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <windows.h>
#include <random>
#include <time.h>

#ifndef _POSIX_
#include "dirent.h"
#else
#include <dirent.h>
#endif
//-------------------------------------------------------------------------
static char g_szVersion[5] = "1.0\0";
//-------------------------------------------------------------------------
vector<string> CFKPacket::__FileTypes( string p_szTypes )
{
	vector<string> vecSplitTypes;
	if( p_szTypes.empty() )
		return vecSplitTypes;
	int nNumTypes = 0;
	size_t unPos = -1;
	
	// �ַ������
	do{
		unPos = p_szTypes.find( '|', unPos + 1 );
		if( unPos == string::npos )
		{
			nNumTypes++;
			break;
		}
		nNumTypes++;
	}while(true);

	string szSplitType;
	unPos = -1;
	for( int i = 0; i < nNumTypes; ++i )
	{
		szSplitType = p_szTypes.substr( unPos + 1, p_szTypes.find('|', unPos + 1) );
		unPos = p_szTypes.find('|', unPos + 1 );
		vecSplitTypes.push_back( szSplitType );
	}
	return vecSplitTypes;
}
//-------------------------------------------------------------------------
bool CFKPacket::__CreateEntry( string p_szRootPath, string p_szRelativePath, string p_szFileName )
{
	ifstream  FileIn;
	SFileEntry tagEntry;

	string szEntryName;
	szEntryName += p_szRootPath;
	szEntryName += "\\";
	szEntryName += p_szRelativePath;
	szEntryName += "\\";
	szEntryName += p_szFileName;
	memcpy( tagEntry.m_szFileName, (p_szRelativePath + "\\" + p_szFileName).c_str(), 128 );
	memcpy( tagEntry.m_szFileFullPath, szEntryName.c_str(), 256 );

	FileIn.open( szEntryName, ifstream::binary | ifstream::ate );
	if( !FileIn.is_open() )
		return false;
	tagEntry.m_unSize = ( unsigned int )FileIn.tellg();
	FileIn.close();
	tagEntry.m_unOffset = 0;		// ��ʱ����֪��ƫ����

	m_vecFileEntries.push_back( tagEntry );
	return true;
}
//-------------------------------------------------------------------------
// ����һ���ļ��е����ļ���
vector<string> CFKPacket::__GetSubDir( string p_szParentDir, string p_szRootDir )
{
	DIR* pDir = NULL;
	dirent* pEntry = NULL;
	vector<string> vecSubDir;

	pDir = opendir( p_szParentDir.c_str() );
	if( pDir )
	{
		while( pEntry = readdir( pDir ) )
		{
			if( pEntry->d_type == DT_DIR && (strcmp(pEntry->d_name, ".") != 0) && (strcmp(pEntry->d_name, "..") != 0) )
			{
				string szSubDir = p_szParentDir + "\\" + pEntry->d_name;							// ����·��
				int nPos = szSubDir.find_first_of( p_szParentDir + "\\", 0 );
				if( nPos == string::npos )
					break;
				string szRelativeDirPath = szSubDir;
				szRelativeDirPath.erase( 0, strlen(p_szRootDir.c_str()) + 1 );		// ��Ը�Ŀ¼�����·��
				vecSubDir.push_back( szRelativeDirPath );
				vector< string > vec = __GetSubDir( szSubDir, p_szRootDir );
				copy( vec.begin(),vec.end(),back_insert_iterator<vector<string> >( vecSubDir ) );
			}
		}
	}
	return vecSubDir;
}
//-------------------------------------------------------------------------
CFKPacket::CFKPacket()
	: m_bIsLoadedPak( false )
{

}
//-------------------------------------------------------------------------
CFKPacket::~CFKPacket()
{
	m_bIsLoadedPak = false;
	m_vecFileEntries.clear();
}
//-------------------------------------------------------------------------
// ���һ��pak
// �����Ҫ��ָ��������ļ����д�����������������������ƴ��� ".jpg|.png|.bmp"
bool CFKPacket::CreatePAK( string p_szPakName, string p_szSrcRootPath, 
						  bool p_bIsEntry, string p_szType )
{
	m_bIsLoadedPak = false;
	m_szPakName = p_szPakName;

	// �������������
	srand((unsigned int)time(NULL));

	ofstream		PAKOut;
	ifstream		FileIn;
	int				nFileNum = 0;

	m_tagPakHead.m_bIsUseEncrypt	= p_bIsEntry;
	m_tagPakHead.m_cEncryptVal		= (char)(rand() % 256);
	memcpy( m_tagPakHead.m_szFileID, "FKPAK\0", 6 );
	memcpy( m_tagPakHead.m_szVersion, g_szVersion, 4 );

	vector<string>	vecCorrectTypes = __FileTypes(p_szType);
	DIR* pDir = NULL;
	dirent* pEntry = NULL;

	// ��ȡĿ��Ŀ¼��ȫ����Ŀ¼
	vector<string> vecAllSubDir = __GetSubDir( p_szSrcRootPath, p_szSrcRootPath );
	// ��Ӹ�Ŀ¼����
	vecAllSubDir.push_back(".");

	// ����ȫ����Ŀ¼
	vector<string>::iterator Ite = vecAllSubDir.begin();
	for( ; Ite != vecAllSubDir.end(); ++Ite )
	{
		pDir = opendir(( p_szSrcRootPath + "\\" + (*Ite) ).c_str());
		if( pDir == NULL )
			continue;
		while( pEntry = readdir( pDir ))
		{
			if( pEntry->d_type != DT_DIR && pEntry->d_type == DT_REG )
			{
				// ����׺
				bool bCorrectType = false;
				if( p_szType.empty() )
				{
					bCorrectType = true;
				}
				else
				{
					for( unsigned int i = 0; i < vecCorrectTypes.size(); ++i )
					{
						string szCompareStr = pEntry->d_name;
						int nFound = szCompareStr.find_last_of('.');
						szCompareStr = szCompareStr.substr( nFound );

						if( !szCompareStr.compare(vecCorrectTypes[i]) )
							bCorrectType = true;
					}
				}

				if( bCorrectType )
				{
					nFileNum++;
					if( !__CreateEntry( p_szSrcRootPath, (*Ite).c_str(), pEntry->d_name ) )
						return false;
				}
			}
		}
	}

	delete pDir, pEntry;

	m_tagPakHead.m_nFileNum	= nFileNum;
	int nBaseOffset = sizeof( SPacketHead ) + ( nFileNum * sizeof(SFileEntry));
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		m_vecFileEntries[i].m_unOffset = nBaseOffset;
		nBaseOffset += m_vecFileEntries[i].m_unSize;
	}

	if( nFileNum )
	{
		PAKOut.open( p_szPakName, ofstream::binary | ofstream::trunc );
		if( !PAKOut.is_open() )
			return false;

		// д��ͷ
		PAKOut.write( (FKCHAR*)&m_tagPakHead, sizeof(SPacketHead) );
		FKCHAR* szBuffer = NULL;

		// д�ļ���Ԫ��Ϣ
		for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
		{
			szBuffer = new FKCHAR[sizeof(SFileEntry)];
			memcpy( szBuffer, &(m_vecFileEntries[i]), sizeof(SFileEntry) );

			// ����
			if( m_tagPakHead.m_bIsUseEncrypt )
			{
				for( unsigned int j = 0; j < sizeof(SFileEntry); ++j )
				{
					szBuffer[j] += m_tagPakHead.m_cEncryptVal;
				}
			}

			PAKOut.write( szBuffer, sizeof(SFileEntry) );
			delete [] szBuffer;
		}

		// д�ļ�
		for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
		{
			int nSize = m_vecFileEntries[i].m_unSize;
			szBuffer = new FKCHAR[nSize];

			FileIn.open( m_vecFileEntries[i].m_szFileFullPath, ifstream::binary );
			if( !FileIn.is_open() )
				return false;

			FileIn.read( szBuffer, nSize );
			// ����
			if( m_tagPakHead.m_bIsUseEncrypt )
			{
				for( int j = 0; j < nSize; ++j )
				{
					szBuffer[j] += m_tagPakHead.m_cEncryptVal;
				}
			}

			PAKOut.write( szBuffer, nSize );
			delete [] szBuffer;
			FileIn.close();
		}

		PAKOut.close();
	}

	if( nFileNum < 1 )
		return false;

	return true;
}
//-------------------------------------------------------------------------
// ����һ��pak
bool CFKPacket::ReadPAK( string p_szPakPath )
{
	ifstream PAKRead;
	PAKRead.open( p_szPakPath, ios::binary );
	if( !PAKRead.is_open() )
		return false;

	PAKRead.read( (FKCHAR*)&m_tagPakHead, sizeof(SPacketHead) );

	// �������
	if(( strcmp(m_tagPakHead.m_szFileID, "FKPAK") != 0 ) ||
		(!( m_tagPakHead.m_nFileNum > 0 )) ||
		( strcmp(m_tagPakHead.m_szVersion, g_szVersion) != 0 ))
	{
		PAKRead.close();
		return false;
	}

	m_vecFileEntries.clear();

	FKCHAR* szBuffer = NULL;
	for( int i = 0; i < m_tagPakHead.m_nFileNum; ++i )
	{
		szBuffer = new FKCHAR[sizeof(SFileEntry)];
		SFileEntry tagEntry;
		PAKRead.read( szBuffer, sizeof(SFileEntry) );
		if( m_tagPakHead.m_bIsUseEncrypt )
		{
			for( int j = 0; j < sizeof(SFileEntry); ++j )
			{
				szBuffer[j] -= m_tagPakHead.m_cEncryptVal;
			}
		}
		memcpy( &tagEntry, szBuffer, sizeof(SFileEntry) );
		m_vecFileEntries.push_back( tagEntry );
		delete [] szBuffer;
	}

	PAKRead.close();
	m_bIsLoadedPak = true;
	return true;
}
//-------------------------------------------------------------------------
// Ϊpak�����һ���ļ�
// ע�⣺���ú������ RebuildPAK() ����֤���´��
bool CFKPacket::AppendFile( string p_szRootPath, string p_szRelativePath, string p_szFileName )
{
	string szFilePath = p_szRelativePath + "\\" + p_szFileName;
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( !szFilePath.compare( m_vecFileEntries[i].m_szFileName ) )
			return false;
	}

	if( !__CreateEntry( p_szRootPath, p_szRelativePath, p_szFileName ))
		return false;

	if( m_vecChanges.empty() )
	{
		m_vecChanges.assign( m_vecFileEntries.size() - 1, eFS_Normal );
	}
	m_vecChanges.push_back( eFS_Added );
	return true;
}
//-------------------------------------------------------------------------
// Ϊpak��ɾ��һ���ļ�
// ע�⣺���ú������ RebuildPAK() ����֤���´��
bool CFKPacket::RemoveFile( string p_szFilePath )
{
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( p_szFilePath.compare( m_vecFileEntries[i].m_szFileName ) == 0 )
		{
			if( m_vecChanges.empty() )
			{
				m_vecChanges.assign( m_vecFileEntries.size() - 1, eFS_Normal );
			}
			m_vecChanges[i] = eFS_Deleted;
			return true;
		}
	}
	return false;
}
//-------------------------------------------------------------------------
// ���´��
// ע�⣺��������Ϣ���κθ��ģ���Ȼ����false
bool CFKPacket::RebuildPAK()
{
	// ���޸�
	if( m_vecChanges.empty() )
		return false;

	if( !m_bIsLoadedPak )
		return false;

	ofstream PAKOut;
	ifstream PAKIn;
	PAKOut.open( m_szPakName + ".new", ofstream::binary );
	if( !PAKOut.is_open() )
		return false;

	int nNumFiles = 0;
	vector<SFileEntry>	vecOriginal( m_vecFileEntries );

	for( unsigned int i = 0; i < m_vecChanges.size(); ++i )
	{
		if( m_vecChanges[i] >= 0 )
		{
			nNumFiles++;
		}
	}

	m_tagPakHead.m_nFileNum = nNumFiles;

	int nOffset = sizeof(SPacketHead) + ( nNumFiles * sizeof(SFileEntry));
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( m_vecChanges[i] == eFS_Deleted )
			continue;
		m_vecFileEntries[i].m_unOffset	= nOffset;
		nOffset += m_vecFileEntries[i].m_unSize;
	}

	PAKOut.write( (FKCHAR*)&m_tagPakHead, sizeof(m_tagPakHead) );
	FKCHAR* szBuffer = NULL;

	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if(m_vecChanges[i] == eFS_Deleted)
			continue;
		szBuffer = new FKCHAR[sizeof(SFileEntry)];
		memcpy( szBuffer, &(m_vecFileEntries[i]), sizeof(SFileEntry) );

		if( m_tagPakHead.m_bIsUseEncrypt )
		{
			for( int j = 0; j < sizeof(SFileEntry); ++j )
			{
				szBuffer[j] += m_tagPakHead.m_cEncryptVal;
			}
		}
		
		PAKOut.write( szBuffer, sizeof(SFileEntry) );
		delete [] szBuffer;
	}

	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( m_vecChanges[i] == eFS_Deleted )
		{
			continue;
		}
		else if( m_vecChanges[i] == eFS_Added )
		{
			PAKIn.open( m_vecFileEntries[i].m_szFileFullPath, ifstream::binary );
		}
		else
		{
			// ��ͨ��
			PAKIn.open( m_szPakName, ifstream::binary );
			PAKIn.seekg( vecOriginal[i].m_unOffset );
		}

		if( !PAKIn.is_open() )
		{
			vecOriginal.clear();
			return false;
		}

		szBuffer = new FKCHAR[ m_vecFileEntries[i].m_unSize ];
		PAKIn.read( szBuffer, m_vecFileEntries[i].m_unSize );
		if( m_vecChanges[i] == eFS_Added )
		{
			if( m_tagPakHead.m_bIsUseEncrypt )
			{
				for( unsigned int j = 0; j < m_vecFileEntries[i].m_unSize; ++j )
				{
					szBuffer[j] += m_tagPakHead.m_cEncryptVal;
				}
			}
		}

		PAKOut.write( szBuffer, m_vecFileEntries[i].m_unSize );
	}

	PAKIn.close();
	delete [] szBuffer;
	vecOriginal.clear();
	PAKOut.close();

	// ɾ���ϰ汾��pak�ļ�
	remove( m_szPakName.c_str() );

	// �޸��°汾��pak�ļ���
	FKCHAR* szFileName = new FKCHAR[150];
	strcpy( szFileName, m_szPakName.c_str() );
	strcat( szFileName, ".new" );
	rename( szFileName, m_szPakName.c_str() );
	delete szFileName;

	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( m_vecChanges[i] == eFS_Deleted )
		{
			m_vecFileEntries.erase( m_vecFileEntries.begin() + i, m_vecFileEntries.begin() + i + 1 );
		}
	}
	m_vecChanges.clear();
	return true;
}
//-------------------------------------------------------------------------
// ���Ҳ���ȡһ���ļ���PAK�е�λ��ָ�루����Ը��ڵ�����λ�ã�
FKCHAR* CFKPacket::GetFileDataFromPAK( string p_szFileName )
{
	SFileEntry* pEntry = GetFileInfoFromPAK( p_szFileName );
	if( pEntry == NULL )
		return NULL;

	FKCHAR* szBuffer = NULL;
	ifstream PAKRead;
	PAKRead.open( m_szPakName, ios::binary );
	if( !PAKRead.is_open() )
		return NULL;

	szBuffer = new FKCHAR[pEntry->m_unSize];
	PAKRead.seekg( pEntry->m_unOffset, ifstream::beg );
	PAKRead.read( szBuffer, pEntry->m_unSize );
	if( m_tagPakHead.m_bIsUseEncrypt )
	{
		for( unsigned int j = 0; j < pEntry->m_unSize; ++j )
		{
			szBuffer[j] -= m_tagPakHead.m_cEncryptVal;
		}
	}
	return szBuffer;
}
//-------------------------------------------------------------------------
// ���Ҳ���ȡһ���ļ���Ϣ��PAK�е�λ��ָ�루����Ը��ڵ�����λ�ã�
CFKPacket::SFileEntry* CFKPacket::GetFileInfoFromPAK( string p_szFileName )
{
	if( !m_bIsLoadedPak )
	{
		return NULL;
	}
	for( int i = 0; i < m_tagPakHead.m_nFileNum; ++i )
	{
		if( strcmp( m_vecFileEntries[i].m_szFileName, p_szFileName.c_str() ) == 0 )
		{
			return &m_vecFileEntries[i];
		}
	}
	return NULL;
}
//-------------------------------------------------------------------------
// ���ĳ�ļ��ڰ��ڴ�С
int CFKPacket::GetFileSize( string p_szName )
{
	if( !m_bIsLoadedPak )
	{
		return -2;
	}
	for( int i = 0; i < m_tagPakHead.m_nFileNum; ++i )
	{
		if( strcmp( m_vecFileEntries[i].m_szFileName, p_szName.c_str() ) == 0 )
		{
			return m_vecFileEntries[i].m_unSize;
		}
	}
	return -1;
}
//-------------------------------------------------------------------------
// ��ð���ȫ���ļ������б�
vector<string> CFKPacket::GetAllFileNameInPAK()
{
	vector<string> vecEntriesName;

	if( !m_bIsLoadedPak )
		return vecEntriesName;

	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		vecEntriesName.push_back( m_vecFileEntries[i].m_szFileName );
	}
	return vecEntriesName;
}
//-------------------------------------------------------------------------
// ��ȡ�����ļ�����
int CFKPacket::GetFileNumInPAK()
{
	if( !m_bIsLoadedPak )
		return -1;
	return m_tagPakHead.m_nFileNum;
}
//-------------------------------------------------------------------------
// ���һ��PAK�ڵ��ļ�
bool CFKPacket::UnpakEntry( string p_szFileName, string p_szFilePath )
{
	ofstream FileOut;
	FileOut.open( p_szFilePath, ofstream::binary | ofstream::trunc );
	if( !FileOut.is_open() )
	{
		return false;
	}

	FKCHAR* szBuffer = GetFileDataFromPAK( p_szFileName );
	int nSize = GetFileSize( p_szFileName );
	if( szBuffer == NULL || nSize <= 0 )
		return false;

	FileOut.write( szBuffer, nSize );
	delete szBuffer;

	FileOut.close();
	return true;
}
//-------------------------------------------------------------------------
// ���ȫ������
void CFKPacket::Clear()
{
	m_bIsLoadedPak = false;
	m_vecFileEntries.clear();
	m_vecChanges.clear();
	m_szPakName.clear();
}
//-------------------------------------------------------------------------