
// FKPackTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFKPackToolApp:
// �йش����ʵ�֣������ FKPackTool.cpp
//

class CFKPackToolApp : public CWinApp
{
public:
	CFKPackToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFKPackToolApp theApp;