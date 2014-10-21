
// FKPackToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FKPackTool.h"
#include "FKPackToolDlg.h"
#include "afxdialogex.h"
#include "FolderCmpStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFKPackToolDlg �Ի���



CFKPackToolDlg::CFKPackToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFKPackToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFKPackToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFKPackToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFKPackToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFKPackToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFKPackToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CFKPackToolDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CFKPackToolDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CFKPackToolDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CFKPackToolDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CFKPackToolDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CFKPackToolDlg::OnBnClickedButton9)
END_MESSAGE_MAP()


// CFKPackToolDlg ��Ϣ�������

BOOL CFKPackToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pFKPacket = new CFKPacket();
	m_pFileList = (CListCtrl*)GetDlgItem( IDC_LIST1 );
	m_pFileList->SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFKPackToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFKPackToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFKPackToolDlg::OnBnClickedButton1()
{
	BROWSEINFO binfo;
	memset(&binfo,0x00,sizeof(binfo));
	binfo.hwndOwner=GetSafeHwnd();
	TCHAR szSrcPath[MAX_PATH]={0};
	binfo.lpszTitle=_T("��ѡ��Դ�ļ�/Ŀ¼");
	binfo.ulFlags=BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;//BIF_BROWSEINCLUDEFILES;//BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist;
	lpDlist=SHBrowseForFolder(&binfo);
	if (NULL!=lpDlist)
	{
		SHGetPathFromIDList(lpDlist,szSrcPath);
		m_vecStrSource.clear();
		m_strSource=szSrcPath;
		GetDlgItem( IDC_EDIT1 )->SetWindowText( m_strSource );
	}
}


void CFKPackToolDlg::OnBnClickedButton2()
{
	TCHAR szFilter[] = _T("FKPacket���ļ� (*.PAK)||");
	CFileDialog FileDlg(FALSE,
		_T("pak"),
		NULL,
		OFN_HIDEREADONLY,
		szFilter,
		NULL,
		0,
		TRUE);
	if (IDOK==FileDlg.DoModal())
	{
		int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
		for( int i = 0; i < n; ++i )
		{
			m_pFileList->DeleteColumn( 0 );
		}
		m_pFileList->DeleteAllItems();
		m_pFileList->InsertColumn(0, "�ļ�·��", LVCFMT_LEFT, 200);
		m_pFileList->InsertColumn(1, "Դ�ļ���С", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(2, "ѹ�����С", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(3, "ѹ����", LVCFMT_LEFT, 60);
		m_pFileList->InsertColumn(4, "�޸�����", LVCFMT_LEFT, 60);

		m_vecStrDestination.clear();
		m_strDestination=FileDlg.GetPathName();
		GetDlgItem( IDC_EDIT2 )->SetWindowText( m_strDestination );
	}
}

// ���
void CFKPackToolDlg::OnBnClickedButton3()
{
	CString strMsg;
	if( m_pFKPacket == NULL )
	{
		strMsg=_T("����Ԥ�ϵĴ��������������");
		MessageBox(strMsg);
		return;
	}
	if( m_vecStrDestination.size() > 0 && m_vecStrSource.size() > 0 && 
		m_vecStrDestination.size() == m_vecStrSource.size() )
	{
		for( unsigned int i = 0; i < m_vecStrSource.size(); ++i )
		{
			m_pFKPacket->Clear();
			if( !m_pFKPacket->CreatePAK( LPCSTR(m_vecStrDestination[i]), LPCSTR(m_vecStrSource[i]) ) )
			{
				m_pFKPacket->Clear();
				strMsg=_T("ѹ��ʧ�ܣ�����ϵ������Ա");
				MessageBox(strMsg);
				return;
			}
		}
	}
	else
	{
		int nSrcType=__CheckSourceFileName(m_strSource);
		if (-1 == nSrcType)
		{
			strMsg=_T("��ѡ����Դ�ļ�/Ŀ¼");
			MessageBox(strMsg);
			return;
		}
		int nDesType=__CheckDestinationFileName(m_strDestination);
		if (-1 == nDesType)
		{
			strMsg=_T("��ѡ��Ŀ���ļ�");
			MessageBox(strMsg);
			return;
		}

		m_pFKPacket->Clear();
		if( !m_pFKPacket->CreatePAK( LPCSTR(m_strDestination), LPCSTR(m_strSource) ) )
		{
			m_pFKPacket->Clear();
			strMsg=_T("ѹ��ʧ�ܣ�����ϵ������Ա");
			MessageBox(strMsg);
			return;
		}
	}

	m_pFKPacket->Clear();
	strMsg=_T("ѹ�����");
	MessageBox(strMsg);
	return;
}


int CFKPackToolDlg::__CheckSourceFileName(CString strFileName)
{
	if (""==strFileName)
		return -1;

	if (PathIsDirectory(strFileName))
	{
		return 0;
	}
	else
	{
		// ����ļ��Ƿ����
		CFileStatus FileStatus;
		if (!CFile::GetStatus(strFileName,FileStatus))
		{
			return -1;
		}

		return 0;
	}
	return -1;
}

int CFKPackToolDlg::__CheckDestinationFileName(CString strFileName)
{
	if (""==strFileName)
		return -1;

	if (PathIsDirectory(strFileName))
	{
		return -1;
	}
	else
	{
		// ���Ŀ¼�Ƿ���ȷ
		int nPosition=strFileName.Find('/');
		if (-1==nPosition)
		{
			nPosition=strFileName.Find('\\');
		}
		if (-1==nPosition)
		{
			return -1;
		}
		CString strPath=strFileName.Left(nPosition);
		if (PathIsDirectory(strPath))
		{
			return 0;
		}

		return -1;
	}
	return -1;
}

// �鿴���ļ�
void CFKPackToolDlg::OnBnClickedButton4()
{
	CString strMsg;
	int nDesType=__CheckDestinationFileName(m_strDestination);
	if (-1 == nDesType)
	{
		strMsg=_T("��ѡ����ļ�");
		MessageBox(strMsg);
		return;
	}
	m_pFKPacket->Clear();
	if( !m_pFKPacket->ReadPAK( LPCSTR(m_strDestination) ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("��ȡ���ļ�ʧ�ܣ�����ϵ������Ա");
		MessageBox(strMsg);
		return;
	}

	int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
	for( int i = 0; i < n; ++i )
	{
		m_pFileList->DeleteColumn( 0 );
	}
	m_pFileList->DeleteAllItems();
	m_pFileList->InsertColumn(0, "�ļ�·��", LVCFMT_LEFT, 200);
	m_pFileList->InsertColumn(1, "Դ�ļ���С", LVCFMT_LEFT, 90);
	m_pFileList->InsertColumn(2, "ѹ�����С", LVCFMT_LEFT, 90);
	m_pFileList->InsertColumn(3, "ѹ����", LVCFMT_LEFT, 60);
	m_pFileList->InsertColumn(4, "�޸�����", LVCFMT_LEFT, 60);

	vector<string> vecFileList = m_pFKPacket->GetAllFileNameInPAK();
	vector<string>::iterator Ite = vecFileList.begin();
	CString tmp;

	int nSrcTotal = 0;
	int nDstTotal = 0;
	for( ; Ite != vecFileList.end(); ++Ite )
	{
		int nIndex = 0;
		m_pFileList->InsertItem( 0, (*Ite).c_str(), 0 );
		int nSrcSize = m_pFKPacket->GetFileSize((*Ite).c_str());
		nSrcTotal += nSrcSize;
		tmp.Format("%d b", nSrcSize);
		m_pFileList->SetItemText( 0, ++nIndex, tmp );
		int nDstSize = m_pFKPacket->GetFileCompressSize((*Ite).c_str());
		nDstTotal += nDstSize;
		tmp.Format("%d b", nDstSize );
		m_pFileList->SetItemText( 0, ++nIndex, tmp );
		int nValue = nDstSize * 100 / nSrcSize;
		tmp.Format("%d%%", nValue );
		m_pFileList->SetItemText( 0, ++nIndex, tmp );

		ENUM_SubPackFileState nType = m_pFKPacket->GetFileChangeType((*Ite).c_str());
		switch ( nType )
		{
		case eFileState_Unknown:
			m_pFileList->SetItemText( 0, ++nIndex, _T("����") );
			break;
		case eFileState_Delete:
			m_pFileList->SetItemText( 0, ++nIndex, _T("ɾ��") );
			break;
		case eFileState_Add:
			m_pFileList->SetItemText( 0, ++nIndex, _T("���") );
			break;
		case eFileState_Change:
			m_pFileList->SetItemText( 0, ++nIndex, _T("����") );
			break;
		default:
			m_pFileList->SetItemText( 0, ++nIndex, _T("δ֪") );
			break;
		}
	}

	m_pFileList->InsertItem( 0, _T("�ܼ�"), 0 );
	tmp.Format("%d Kb", nSrcTotal / 1000);
	m_pFileList->SetItemText( 0, 1, tmp );
	tmp.Format("%d Kb", nDstTotal / 1000);
	m_pFileList->SetItemText( 0, 2, tmp );
	float fValue = (float)( nDstTotal / 1000 ) / (float)(nSrcTotal / 1000);
	tmp.Format("%.2f%%", fValue * 100.0f );
	m_pFileList->SetItemText( 0, 3, tmp );
	return;
}

// ��Ŀ¼���ü���
void CFKPackToolDlg::OnBnClickedButton5()
{
	TCHAR szFilter[] = _T("ԴĿ¼�����ļ� (*.cfg)||");
	CFileDialog FileDlg(FALSE,
		_T("cfg"),
		NULL,
		OFN_HIDEREADONLY,
		szFilter,
		NULL,
		0,
		TRUE);
	if (IDOK==FileDlg.DoModal())
	{
		m_strSource="";
		m_strDestination="";
		m_vecStrSource.clear();
		m_vecStrDestination.clear();

		CString szCfg;
		szCfg=FileDlg.GetPathName();
		int nDirNum = 0;
		nDirNum = GetPrivateProfileInt( "��Ҫ������ļ���", "����", 0, szCfg );
		for( int i = 0; i < nDirNum; ++i )
		{
			CString szTmp;
			CString szSectionName;
			szSectionName.Format("%d", i+1 );
			GetPrivateProfileString( "��Ҫ������ļ���", szSectionName, NULL, szTmp.GetBuffer(100), 100, szCfg );
			m_vecStrSource.push_back( szTmp );
			

			CString szDstTmp;
			szDstTmp.Format("%s.pak", szTmp );
			m_vecStrDestination.push_back( szDstTmp );
		}


		for( unsigned int i = 0; i < m_vecStrSource.size(); ++i )
		{
			CString tmp = m_strSource;
			if( i == 0 )
			{
				m_strSource.Format( "%s", m_vecStrSource[i] );
			}
			else
			{
				m_strSource.Format( "%s%s", tmp, m_vecStrSource[i] );
			}
			m_strSource += ";";
		}
		GetDlgItem( IDC_EDIT1 )->SetWindowText( m_strSource );

		for( unsigned int i = 0; i < m_vecStrDestination.size(); ++i )
		{
			CString tmp = m_strDestination;
			if( i == 0 )
			{
				m_strDestination.Format( "%s", m_vecStrDestination[i] );
			}
			else
			{
				m_strDestination.Format( "%s%s", tmp, m_vecStrDestination[i] );
			}
			m_strDestination += ";";
		}
		GetDlgItem( IDC_EDIT2 )->SetWindowText( m_strDestination );
	}
}

// ѡ�񲹶���
void CFKPackToolDlg::OnBnClickedButton6()
{
	TCHAR szFilter[] = _T("FK�������ļ� (*.PAK)||");
	CFileDialog FileDlg(FALSE,
		_T("pak"),
		NULL,
		OFN_HIDEREADONLY,
		szFilter,
		NULL,
		0,
		TRUE);
	if (IDOK==FileDlg.DoModal())
	{
		int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
		for( int i = 0; i < n; ++i )
		{
			m_pFileList->DeleteColumn( 0 );
		}
		m_pFileList->DeleteAllItems();
		m_pFileList->InsertColumn(0, "�ļ�·��", LVCFMT_LEFT, 200);
		m_pFileList->InsertColumn(1, "Դ�ļ���С", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(2, "ѹ�����С", LVCFMT_LEFT, 90);
		m_pFileList->InsertColumn(3, "ѹ����", LVCFMT_LEFT, 60);
		m_pFileList->InsertColumn(4, "�޸�����", LVCFMT_LEFT, 60);


		m_strPack=FileDlg.GetPathName();
		GetDlgItem( IDC_EDIT3 )->SetWindowText( m_strPack );

		CString strMsg;
		int nDesType=__CheckDestinationFileName(m_strPack);
		if (-1 == nDesType)
		{
			strMsg=_T("��ȡ�������ļ�ʧ�ܣ�����ϵ������Ա");
			MessageBox(strMsg);
			return;
		}
		m_pFKPacket->Clear();
		if( !m_pFKPacket->ReadPAK( LPCSTR(m_strPack) ) )
		{
			m_pFKPacket->Clear();
			strMsg=_T("��ȡ�������ļ�ʧ�ܣ�����ϵ������Ա");
			MessageBox(strMsg);
			return;
		}

		vector<string> vecFileList = m_pFKPacket->GetAllFileNameInPAK();
		vector<string>::iterator Ite = vecFileList.begin();
		CString tmp;

		int nSrcTotal = 0;
		int nDstTotal = 0;
		for( ; Ite != vecFileList.end(); ++Ite )
		{
			int nIndex = 0;
			m_pFileList->InsertItem( 0, (*Ite).c_str(), 0 );
			int nSrcSize = m_pFKPacket->GetFileSize((*Ite).c_str());
			nSrcTotal += nSrcSize;
			tmp.Format("%d b", nSrcSize);
			m_pFileList->SetItemText( 0, ++nIndex, tmp );
			int nDstSize = m_pFKPacket->GetFileCompressSize((*Ite).c_str());
			nDstTotal += nDstSize;
			tmp.Format("%d b", nDstSize );
			m_pFileList->SetItemText( 0, ++nIndex, tmp );
			int nValue = nDstSize * 100 / nSrcSize;
			tmp.Format("%d%%", nValue );
			m_pFileList->SetItemText( 0, ++nIndex, tmp );

			ENUM_SubPackFileState nType = m_pFKPacket->GetFileChangeType((*Ite).c_str());
			switch ( nType )
			{
			case eFileState_Unknown:
				m_pFileList->SetItemText( 0, ++nIndex, _T("����") );
				break;
			case eFileState_Delete:
				m_pFileList->SetItemText( 0, ++nIndex, _T("ɾ��") );
				break;
			case eFileState_Add:
				m_pFileList->SetItemText( 0, ++nIndex, _T("���") );
				break;
			case eFileState_Change:
				m_pFileList->SetItemText( 0, ++nIndex, _T("����") );
				break;
			default:
				m_pFileList->SetItemText( 0, ++nIndex, _T("δ֪") );
				break;
			}
		}

		m_pFileList->InsertItem( 0, _T("�ܼ�"), 0 );
		tmp.Format("%d Kb", nSrcTotal / 1000);
		m_pFileList->SetItemText( 0, 1, tmp );
		tmp.Format("%d Kb", nDstTotal / 1000);
		m_pFileList->SetItemText( 0, 2, tmp );
		float fValue = (float)( nDstTotal / 1000 ) / (float)(nSrcTotal / 1000);
		tmp.Format("%.2f%%", fValue * 100.0f );
		m_pFileList->SetItemText( 0, 3, tmp );
		return;
	}
}

// �ϲ�������
void CFKPackToolDlg::OnBnClickedButton7()
{
	CString strMsg;
	if( m_strPack.IsEmpty() )
	{
		strMsg=_T("��ѡ�񲹶���");
		MessageBox(strMsg);
		return;
	}
	if( m_strDestination.IsEmpty() )
	{
		strMsg=_T("��ѡ��������ע�⣬��Ҫͬʱ������");
		MessageBox(strMsg);
		return;
	}

	m_pFKPacket->Clear();
	if( !m_pFKPacket->ReadPAK( LPCSTR(m_strDestination) ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("��ȡ�����ļ�ʧ�ܣ�����ϵ������Ա");
		MessageBox(strMsg);
		return;
	}

	if( !m_pFKPacket->MergePAH( LPCSTR(m_strPack) ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("�ϲ��������ļ�ʧ�ܣ�����ϵ������Ա");
		MessageBox(strMsg);
		return;
	}

	strMsg=_T("�ϰ���ɣ�����ϵ������Ա");
	MessageBox(strMsg);
	return;
}

// ѡ���ļ���
void CFKPackToolDlg::OnBnClickedButton8()
{
	BROWSEINFO binfo;
	memset(&binfo,0x00,sizeof(binfo));
	binfo.hwndOwner=GetSafeHwnd();
	TCHAR szSrcPath[MAX_PATH]={0};
	binfo.lpszTitle=_T("��ѡ��Ա�Դ�ļ�/Ŀ¼");
	binfo.ulFlags=BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;//BIF_BROWSEINCLUDEFILES;//BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist;
	lpDlist=SHBrowseForFolder(&binfo);
	if (NULL!=lpDlist)
	{
		SHGetPathFromIDList(lpDlist,szSrcPath);
		m_strNewSourceDir=szSrcPath;
		GetDlgItem( IDC_EDIT4 )->SetWindowText( m_strNewSourceDir );

		CString strMsg;
		if( m_strSource.IsEmpty() )
		{
			strMsg=_T("��ѡ��Դ�ļ���");
			MessageBox(strMsg);
			return;
		}
		if( m_strNewSourceDir.IsEmpty() )
		{
			strMsg=_T("��ѡ��Ա��ļ���");
			MessageBox(strMsg);
			return;
		}

		CFldCmpTree tree;
		tree.ParseFolder( m_strSource, m_strNewSourceDir, m_vecPackChangeInfo );

		int n = m_pFileList->GetHeaderCtrl()->GetItemCount();
		for( int i = 0; i < n; ++i )
		{
			m_pFileList->DeleteColumn( 0 );
		}
		m_pFileList->DeleteAllItems();
		m_pFileList->InsertColumn(0, "��������", LVCFMT_LEFT, 60);
		m_pFileList->InsertColumn(1, "������ļ���", LVCFMT_LEFT, 350);

		vector<SPackChangeInfo>::iterator Ite = m_vecPackChangeInfo.begin();
		for( ; Ite != m_vecPackChangeInfo.end(); ++Ite )
		{
			int nIndex = 0;
			switch ( (*Ite).m_eFileState )
			{
			case eFileState_Add:
				m_pFileList->InsertItem( 0, "����", 0 );
				break;
			case eFileState_Delete:
				m_pFileList->InsertItem( 0, "ɾ��", 0 );
				break;
			case eFileState_Change:
				m_pFileList->InsertItem( 0, "����", 0 );
				break;
			default:
				break;
			}

			m_pFileList->SetItemText( 0, ++nIndex, (*Ite).m_szFileFullName );
		}
	}
}

// ���ɲ�����
void CFKPackToolDlg::OnBnClickedButton9()
{
	CString strMsg;
	if( m_vecPackChangeInfo.empty() )
	{
		strMsg=_T("��ѡ��ԭ�ļ��кͶԱ��ļ���");
		MessageBox(strMsg);
		return;
	}
	if( m_pFKPacket == NULL )
	{
		strMsg=_T("����Ԥ�ϵĴ��������������");
		MessageBox(strMsg);
		return;
	}

	CEdit* pVersion = (CEdit*)( GetDlgItem(IDC_EDIT5));
	if( pVersion == NULL )
	{
		strMsg=_T("����Ԥ�ϵĴ��������������");
		MessageBox(strMsg);
		return;
	}
	CString szVersion;
	pVersion->GetWindowText(szVersion);
	int nVersion = _ttoi(szVersion);

	m_pFKPacket->Clear();

	char chPath[MAX_PATH];
	GetModuleFileName(NULL,chPath,MAX_PATH); //�õ�ִ�г�����ļ���������·������
	CString szExePath;
	szExePath.Format("%s",chPath); //ת�����ַ�����
	int nPos=szExePath.ReverseFind('\\');//���ұ��ҵ���һ����\\���ַ��������������±��λ��
	szExePath=szExePath.Left(nPos+1); //�����ַ�����ǰnPos+1���ַ���������\\������

	szVersion.Format("%sPATCH_%d.pak", szExePath, nVersion);
	vector<SPatchFileInfo> vecFileList;
	CString noDirFileName;
	for( unsigned int i = 0; i < m_vecPackChangeInfo.size(); ++i )
	{
		SPatchFileInfo tmp;
		tmp.m_eFileState = m_vecPackChangeInfo[i].m_eFileState;
		noDirFileName = m_vecPackChangeInfo[i].m_szFileFullName;
		if( noDirFileName.Find( m_strSource ) >= 0  )
		{
			noDirFileName.Delete( noDirFileName.Find(m_strSource), m_strSource.GetLength()+1 );
		}
		else if( noDirFileName.Find( m_strNewSourceDir ) >= 0 )
		{
			noDirFileName.Delete( noDirFileName.Find(m_strNewSourceDir), m_strNewSourceDir.GetLength()+1 );
		}
		strcpy( tmp.m_szFillName,  noDirFileName );
		strcpy( tmp.m_szFileFullPath, m_vecPackChangeInfo[i].m_szFileFullName );
		vecFileList.push_back( tmp );
	}
	if( !m_pFKPacket->CreatePAH( LPCSTR(szVersion), vecFileList, nVersion ) )
	{
		m_pFKPacket->Clear();
		strMsg=_T("���ʧ�ܣ�����ϵ������Ա");
		MessageBox(strMsg);
		return;
	}

	strMsg=_T("������");
	MessageBox(strMsg);
	return;
}
