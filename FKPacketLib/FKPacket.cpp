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
bool CFKPacket::__CreateEntry( string p_szRootPath, string p_szRelativePath, string p_szFileName, ENUM_SubPackFileState p_eType )
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
	tagEntry.m_eState = p_eType;
	tagEntry.m_unCompressedSize = 0;	// ��ʱ��֪��ѹ����С
	tagEntry.m_unOffset = 0;			// ��ʱ����֪��ƫ����

	m_vecFileEntries.push_back( tagEntry );
	return true;
}
//-------------------------------------------------------------------------
bool CFKPacket::__CreateEntry( string p_szFileFullName,string p_szFileName, ENUM_SubPackFileState p_eType)
{
	ifstream  FileIn;
	SFileEntry tagEntry;

	//string szEntryName = p_szFileFullName;
	//int nFound = szEntryName.find_last_of('//');
	//if( nFound == string::npos )
	//{
	//	nFound = szEntryName.find_last_of('\\');
	//}
	//string szFileName = szEntryName.substr( nFound + 1 ); 
	memcpy( tagEntry.m_szFileName, p_szFileName.c_str(), 128 );
	memcpy( tagEntry.m_szFileFullPath, p_szFileFullName.c_str(), 256 );

	FileIn.open( p_szFileFullName, ifstream::binary | ifstream::ate );
	if( !FileIn.is_open() )
		return false;
	tagEntry.m_unSize = ( unsigned int )FileIn.tellg();
	FileIn.close();
	tagEntry.m_eState = p_eType;
	tagEntry.m_unCompressedSize = 0;	// ��ʱ��֪��ѹ����С
	tagEntry.m_unOffset = 0;			// ��ʱ����֪��ƫ����

	m_vecFileEntries.push_back( tagEntry );
	return true;
}
//-------------------------------------------------------------------------
// �ж�һ���ļ��Ƿ���Ҫѹ������ΪͼƬ��ʽѹ���ʹ��ͣ������㣬Ĭ�ϲ�ѹ����
bool CFKPacket::__IsNeedCompress( string p_szFileName )
{
	// ����׺
	bool bNeedCompress = true;
	vector<string> vecUncompressFileType;
	vecUncompressFileType.push_back(".jpg");
	vecUncompressFileType.push_back(".png");
	vecUncompressFileType.push_back(".JPG");
	vecUncompressFileType.push_back(".PNG");
	for( unsigned int i = 0; i < vecUncompressFileType.size(); ++i )
	{
		string szCompareStr = p_szFileName;
		int nFound = szCompareStr.find_last_of('.');
		szCompareStr = szCompareStr.substr( nFound );

		if( !szCompareStr.compare(vecUncompressFileType[i]) )
		{
			bNeedCompress = false;
			break;
		}
	}
	return bNeedCompress;
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
						  bool p_bIsUseCompress, bool p_bIsEntry, string p_szType )
{
	m_bIsLoadedPak = false;
	m_szPakName = p_szPakName;

	// �������������
	srand((unsigned int)time(NULL));

	ofstream		PAKOut;
	ifstream		FileIn;
	int				nFileNum = 0;

	m_tagPakHead.m_bIsUseEncrypt	= p_bIsEntry;
	m_tagPakHead.m_bIsZipComperssed	= p_bIsUseCompress;
	m_tagPakHead.m_cEncryptVal		= (char)(rand() % 256);
	m_tagPakHead.m_bIsSubPack		= false;
	m_tagPakHead.m_nCurVersion		= 0;
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
	int nBaseOffset = sizeof( SPacketHead );

	if( nFileNum )
	{
		PAKOut.open( p_szPakName, ofstream::binary | ofstream::trunc );
		if( !PAKOut.is_open() )
			return false;

		// д��ͷ��Ϣ
		PAKOut.write( (FKCHAR*)&m_tagPakHead, sizeof(SPacketHead) );
		FKCHAR* szBuffer = NULL;

		// д�ļ���Ϣ
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

			// ѹ��
			if( m_tagPakHead.m_bIsZipComperssed && __IsNeedCompress(m_vecFileEntries[i].m_szFileFullPath) )
			{
				int nCompressedSize = compressBound( nSize );
				FKCHAR* szCompressedBuf = new FKCHAR[nCompressedSize];
				memset( szCompressedBuf, 0, nCompressedSize );
				uLongf lDstLen = nCompressedSize;
				int nError = compress2( (unsigned char*)szCompressedBuf, &lDstLen, (unsigned char*)szBuffer, nSize, Z_DEFAULT_COMPRESSION );

				if( nError != Z_OK )
				{
					PAKOut.write( szBuffer, nSize );
					m_vecFileEntries[i].m_unCompressedSize = 0;
					nBaseOffset += nSize;
				}
				else
				{
					// ѹ���ɹ�
					m_vecFileEntries[i].m_unCompressedSize = lDstLen;
					PAKOut.write( szCompressedBuf, lDstLen );
					nBaseOffset += lDstLen;
				}

				delete [] szCompressedBuf;
			}
			else
			{
				PAKOut.write( szBuffer, nSize );
				m_vecFileEntries[i].m_unCompressedSize = 0;
				nBaseOffset += nSize;
			}
			delete [] szBuffer;
			FileIn.close();
		}

		// д�ļ���Ԫ��Ϣ
		unsigned int unPos = sizeof( SPacketHead );
		for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
		{
			szBuffer = new FKCHAR[sizeof(SFileEntry)];
			m_vecFileEntries[i].m_unOffset = unPos;
			if( m_vecFileEntries[i].m_unCompressedSize == 0 )
			{
				unPos += m_vecFileEntries[i].m_unSize;
			}
			else
			{
				unPos += m_vecFileEntries[i].m_unCompressedSize;
			}
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

		// д�ļ�β��Ϣ
		{
			szBuffer = new FKCHAR[sizeof(SFileTail)];
			m_tagFileTail.m_unFileEntryOffset = nBaseOffset;
			memcpy( szBuffer, &m_tagFileTail, sizeof( SFileTail ) );
			PAKOut.write( szBuffer, sizeof(SFileTail) );
			delete [] szBuffer;
		}

		PAKOut.close();
	}

	if( nFileNum < 1 )
		return false;

	return true;
}
//-------------------------------------------------------------------------
// ���һ������pak
bool CFKPacket::CreatePAH( string p_szPakName, vector<SPatchFileInfo> p_vecPathFile,
								  int p_nVersion, bool p_bIsUseCompress, bool p_bIsEntry )
{
	m_bIsLoadedPak = false;
	m_szPakName = p_szPakName;

	// �������������
	srand((unsigned int)time(NULL));

	ofstream		PAKOut;
	ifstream		FileIn;
	int				nFileNum = 0;

	m_tagPakHead.m_bIsUseEncrypt	= p_bIsEntry;
	m_tagPakHead.m_bIsZipComperssed	= p_bIsUseCompress;
	m_tagPakHead.m_cEncryptVal		= (char)(rand() % 256);
	m_tagPakHead.m_bIsSubPack		= true;
	m_tagPakHead.m_nCurVersion		= p_nVersion;
	memcpy( m_tagPakHead.m_szFileID, "FKPAK\0", 6 );
	memcpy( m_tagPakHead.m_szVersion, g_szVersion, 4 );

	// ����ȫ���ļ�
	vector<SPatchFileInfo>::iterator Ite = p_vecPathFile.begin();
	for( ; Ite != p_vecPathFile.end(); ++Ite )
	{
		nFileNum++;
		if( !__CreateEntry( (*Ite).m_szFileFullPath, (*Ite).m_szFillName, (*Ite).m_eFileState ) )
			return false;
	}

	m_tagPakHead.m_nFileNum	= nFileNum;
	int nBaseOffset = sizeof( SPacketHead );

	if( nFileNum )
	{
		PAKOut.open( p_szPakName, ofstream::binary | ofstream::trunc );
		if( !PAKOut.is_open() )
			return false;

		// д��ͷ��Ϣ
		PAKOut.write( (FKCHAR*)&m_tagPakHead, sizeof(SPacketHead) );
		FKCHAR* szBuffer = NULL;

		// д�ļ���Ϣ
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

			// ѹ��
			if( m_tagPakHead.m_bIsZipComperssed && __IsNeedCompress(m_vecFileEntries[i].m_szFileFullPath) )
			{
				int nCompressedSize = compressBound( nSize );
				FKCHAR* szCompressedBuf = new FKCHAR[nCompressedSize];
				memset( szCompressedBuf, 0, nCompressedSize );
				uLongf lDstLen = nCompressedSize;
				int nError = compress2( (unsigned char*)szCompressedBuf, &lDstLen, (unsigned char*)szBuffer, nSize, Z_DEFAULT_COMPRESSION );

				if( nError != Z_OK )
				{
					PAKOut.write( szBuffer, nSize );
					m_vecFileEntries[i].m_unCompressedSize = 0;
					nBaseOffset += nSize;
				}
				else
				{
					// ѹ���ɹ�
					m_vecFileEntries[i].m_unCompressedSize = lDstLen;
					PAKOut.write( szCompressedBuf, lDstLen );
					nBaseOffset += lDstLen;
				}

				delete [] szCompressedBuf;
			}
			else
			{
				PAKOut.write( szBuffer, nSize );
				m_vecFileEntries[i].m_unCompressedSize = 0;
				nBaseOffset += nSize;
			}
			delete [] szBuffer;
			FileIn.close();
		}

		// д�ļ���Ԫ��Ϣ
		unsigned int unPos = sizeof( SPacketHead );
		for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
		{
			szBuffer = new FKCHAR[sizeof(SFileEntry)];
			m_vecFileEntries[i].m_unOffset = unPos;
			if( m_vecFileEntries[i].m_unCompressedSize == 0 )
			{
				unPos += m_vecFileEntries[i].m_unSize;
			}
			else
			{
				unPos += m_vecFileEntries[i].m_unCompressedSize;
			}
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

		// д�ļ�β��Ϣ
		{
			szBuffer = new FKCHAR[sizeof(SFileTail)];
			m_tagFileTail.m_unFileEntryOffset = nBaseOffset;
			memcpy( szBuffer, &m_tagFileTail, sizeof( SFileTail ) );
			PAKOut.write( szBuffer, sizeof(SFileTail) );
			delete [] szBuffer;
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


	// ��ȡβ��
	PAKRead.seekg(0,ios_base::end);							// �ƶ����ļ�β  
	istream::pos_type file_size = PAKRead.tellg();			// �ļ�����
	unsigned int unSize = file_size;
	PAKRead.seekg( unSize - sizeof(SFileTail), ios_base::beg );		// �ҵ��ļ�β�ṹ
	PAKRead.read( (FKCHAR*)&m_tagFileTail, sizeof(SFileTail) );
	PAKRead.seekg( m_tagFileTail.m_unFileEntryOffset, ios_base::beg );
	
	// ��ȡ�ļ���Ԫ��Ϣ
	FKCHAR* szBuffer = NULL;
	m_vecFileEntries.clear();
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
	m_szPakName = p_szPakPath;
	return true;
}
//-------------------------------------------------------------------------
// �ϲ�һ��pak
bool CFKPacket::MergePAH( string p_szPahPath )
{
	if( !m_bIsLoadedPak )
		return false;
	// �Լ���ȡ���pah�����кϲ�����
	CFKPacket tagPTH;
	if( !tagPTH.ReadPAK( p_szPahPath ))
	{
		return false;
	}
	vector<string>	vecFile = tagPTH.GetAllFileNameInPAK();
	if( vecFile.size() <= 0 )
		return false;
	vector<string>	vecFileFullName = tagPTH.GetAllFilePathNameInPAK();
	if( vecFileFullName.size() != vecFile.size() )
		return false;

	vector<string>::iterator Ite = vecFile.begin();
	for( int nIndex = 0; Ite != vecFile.end(); ++Ite, ++nIndex )
	{
		ENUM_SubPackFileState eState = tagPTH.GetFileChangeType( (*Ite) );
		switch ( eState )
		{
		case eFileState_Add:
			AppendFile( vecFile.at(nIndex), vecFileFullName.at(nIndex) );
			break;
		case eFileState_Delete:
			RemoveFile( vecFile.at(nIndex), vecFileFullName.at(nIndex) );
			break;
		case eFileState_Change:
			ChangeFile( vecFile.at(nIndex), vecFileFullName.at(nIndex) );
			break;
		case eFileState_Unknown:
		default:
			break;
		}
	}
	if(  tagPTH.GetCurPathVersion() != 0 )
	{
		m_tagPakHead.m_nCurVersion = tagPTH.GetCurPathVersion();
	}

	return RebuildPAK();
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
		m_vecChanges.assign( m_vecFileEntries.size(), eFS_Normal );
	}
	m_vecChanges.push_back( eFS_Added );
	return true;
}
//-------------------------------------------------------------------------
bool CFKPacket::AppendFile( string p_szFileName, string p_szFilePathName )
{
	string szFilePath = p_szFilePathName;
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( !szFilePath.compare( m_vecFileEntries[i].m_szFileFullPath ) )
			return false;
	}

	if( !__CreateEntry( szFilePath, p_szFileName ))
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
bool CFKPacket::RemoveFile( string p_szFileName, string p_szFilePathName )
{
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( p_szFileName.compare( m_vecFileEntries[i].m_szFileName ) == 0 )
		{
			if( m_vecChanges.empty() )
			{
				m_vecChanges.assign( m_vecFileEntries.size(), eFS_Normal );
			}
			m_vecChanges[i] = eFS_Deleted;
			return true;
		}
	}
	return false;
}
//-------------------------------------------------------------------------
bool CFKPacket::ChangeFile( string p_szFileName, string p_szFilePathName )
{
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( p_szFileName.compare( m_vecFileEntries[i].m_szFileName ) == 0 )
		{
			if( m_vecChanges.empty() )
			{
				m_vecChanges.assign( m_vecFileEntries.size(), eFS_Normal );
			}
			m_vecChanges[i] = eFS_Changed;
			break;
		}
	}

	if( !__CreateEntry( p_szFilePathName, p_szFileName ))
		return false;

	if( m_vecChanges.empty() )
	{
		m_vecChanges.assign( m_vecFileEntries.size() - 1, eFS_Normal );
	}
	m_vecChanges.push_back( eFS_Added );
	return false;
}
//-------------------------------------------------------------------------
// ��ȡ������
int CFKPacket::GetCurPathVersion()
{
	if(! m_tagPakHead.m_bIsSubPack )
		return 0;
	return m_tagPakHead.m_nCurVersion;
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
	PAKIn.open( m_szPakName, ios::binary );
	if( !PAKIn.is_open() )
		return false;

	int nNumFiles = 0;
	vector<SFileEntry>	vecOriginal( m_vecFileEntries );

	for( unsigned int i = 0; i < m_vecChanges.size(); ++i )
	{
		if( m_vecChanges[i] != eFS_Deleted && m_vecChanges[i] != eFS_Changed )
		{
			nNumFiles++;
		}
	}

	m_tagPakHead.m_nFileNum = nNumFiles;
	PAKOut.write( (FKCHAR*)&m_tagPakHead, sizeof(m_tagPakHead) );

	int nBaseOffset = sizeof( m_tagPakHead );
	FKCHAR* szBuffer = NULL;

	// �ļ�
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if( m_vecChanges[i] == eFS_Deleted || m_vecChanges[i] == eFS_Changed )
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

		// ѹ��
		if( m_tagPakHead.m_bIsZipComperssed && __IsNeedCompress(m_vecFileEntries[i].m_szFileFullPath) )
		{
			int nCompressedSize = compressBound( m_vecFileEntries[i].m_unSize );
			FKCHAR* szCompressedBuf = new FKCHAR[nCompressedSize];
			memset( szCompressedBuf, 0, nCompressedSize );
			uLongf lDstLen = nCompressedSize;
			int nError = compress2( (unsigned char*)szCompressedBuf, &lDstLen, (unsigned char*)szBuffer, m_vecFileEntries[i].m_unSize, Z_DEFAULT_COMPRESSION );

			if( nError != Z_OK )
			{
				PAKOut.write( szBuffer, m_vecFileEntries[i].m_unSize );
				m_vecFileEntries[i].m_unCompressedSize = 0;
				nBaseOffset += m_vecFileEntries[i].m_unSize;
			}
			else
			{
				// ѹ���ɹ�
				m_vecFileEntries[i].m_unCompressedSize = lDstLen;
				PAKOut.write( szCompressedBuf, lDstLen );
				nBaseOffset += lDstLen;
			}
			delete [] szCompressedBuf;
		}
		else
		{
			PAKOut.write( szBuffer, m_vecFileEntries[i].m_unSize );
			m_vecFileEntries[i].m_unCompressedSize = 0;
			nBaseOffset += m_vecFileEntries[i].m_unSize;
		}
	}

	PAKIn.close();
	delete [] szBuffer;
	vecOriginal.clear();


	// �ļ���Ϣͷ
	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		if(m_vecChanges[i] == eFS_Deleted || m_vecChanges[i] == eFS_Changed)
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

	// д�ļ�β��Ϣ
	{
		szBuffer = new FKCHAR[sizeof(SFileTail)];
		m_tagFileTail.m_unFileEntryOffset = nBaseOffset;
		memcpy( szBuffer, &m_tagFileTail, sizeof( SFileTail ) );
		PAKOut.write( szBuffer, sizeof(SFileTail) );
		delete [] szBuffer;
	}

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
// ���Ҳ���ȡһ���ļ���PAK�е�λ��ָ��
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

	PAKRead.seekg( pEntry->m_unOffset, ifstream::beg );
	if( pEntry->m_unCompressedSize == 0 )
	{
		szBuffer = new FKCHAR[pEntry->m_unSize];
		PAKRead.read( szBuffer, pEntry->m_unSize );
		if( m_tagPakHead.m_bIsUseEncrypt )
		{
			for( unsigned int j = 0; j < pEntry->m_unSize; ++j )
			{
				szBuffer[j] -= m_tagPakHead.m_cEncryptVal;
			}
		}
	}
	else
	{
		FKCHAR* szSrcBuffer = new FKCHAR[pEntry->m_unCompressedSize];
		PAKRead.read( szSrcBuffer, pEntry->m_unCompressedSize );

		szBuffer = new FKCHAR[pEntry->m_unSize];
		// ��ѹ��
		uLongf lDstlen;
		
		memset( szBuffer, 0, pEntry->m_unSize );
		int nRet = uncompress( (unsigned char*)szBuffer, &lDstlen, (unsigned char*)szSrcBuffer, pEntry->m_unCompressedSize );
		delete [] szSrcBuffer;
		if( nRet != Z_OK )
		{
			delete [] szBuffer;
			return NULL;
		}

		if( m_tagPakHead.m_bIsUseEncrypt )
		{
			for( unsigned int j = 0; j < pEntry->m_unSize; ++j )
			{
				szBuffer[j] -= m_tagPakHead.m_cEncryptVal;
			}
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
int CFKPacket::GetFileCompressSize( string p_szName )
{
	if( !m_bIsLoadedPak )
	{
		return -2;
	}
	for( int i = 0; i < m_tagPakHead.m_nFileNum; ++i )
	{
		if( strcmp( m_vecFileEntries[i].m_szFileName, p_szName.c_str() ) == 0 )
		{
			if( m_vecFileEntries[i].m_unCompressedSize == 0 )
			{
				return m_vecFileEntries[i].m_unSize;
			}
			else
			{
				return m_vecFileEntries[i].m_unCompressedSize;
			}
		}
	}
	return -1;
}
//-------------------------------------------------------------------------
// ��ȡĳ�ļ����޸�����
ENUM_SubPackFileState CFKPacket::GetFileChangeType( string p_szName )
{
	if( !m_bIsLoadedPak )
	{
		return eFileState_Unknown;
	}
	for( int i = 0; i < m_tagPakHead.m_nFileNum; ++i )
	{
		if( strcmp( m_vecFileEntries[i].m_szFileName, p_szName.c_str() ) == 0 )
		{
			return m_vecFileEntries[i].m_eState;
		}
	}
	return eFileState_Unknown;
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
vector<string> CFKPacket::GetAllFilePathNameInPAK()
{
	vector<string> vecEntriesName;

	if( !m_bIsLoadedPak )
		return vecEntriesName;

	for( unsigned int i = 0; i < m_vecFileEntries.size(); ++i )
	{
		vecEntriesName.push_back( m_vecFileEntries[i].m_szFileFullPath );
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
	memset( &m_tagPakHead, 0, sizeof(SPacketHead) );
	m_tagFileTail.m_unFileEntryOffset = 0;
}
//-------------------------------------------------------------------------