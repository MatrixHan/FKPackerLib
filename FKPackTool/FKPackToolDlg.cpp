
// FKPackToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FKPackTool.h"
#include "FKPackToolDlg.h"
#include "afxdialogex.h"

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
	m_pFileList->InsertColumn(0, "�ļ�·��", LVCFMT_LEFT, 200);
	m_pFileList->InsertColumn(1, "ʵ�ʴ�С", LVCFMT_LEFT, 90);
	m_pFileList->InsertColumn(2, "ѹ����С", LVCFMT_LEFT, 90);
	m_pFileList->InsertColumn(3, "ѹ����", LVCFMT_LEFT, 60);

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
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		szFilter,
		NULL,
		0,
		TRUE);
	if (IDOK==FileDlg.DoModal())
	{
		m_strDestination=FileDlg.GetPathName();
		GetDlgItem( IDC_EDIT2 )->SetWindowText( m_strDestination );
	}
}

// ���
void CFKPackToolDlg::OnBnClickedButton3()
{
	CString strMsg;
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

	if( m_pFKPacket == NULL )
	{
		strMsg=_T("����Ԥ�ϵĴ��������������");
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
	}

	m_pFileList->InsertItem( 0, _T("�ܼ�"), 0 );
	tmp.Format("%d Kb", nSrcTotal / 1000);
	m_pFileList->SetItemText( 0, 1, tmp );
	tmp.Format("%d Kb", nDstTotal / 1000);
	m_pFileList->SetItemText( 0, 2, tmp );
	float fValue = (float)( nDstTotal / 1000 ) / (float)(nSrcTotal / 1000);
	tmp.Format("%.2f%%", fValue * 100.0f );
	m_pFileList->SetItemText( 0, 3, tmp );

	strMsg=_T("�������");
	MessageBox(strMsg);
	return;
}
