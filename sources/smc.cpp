// smc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "smc.h"

#include "MainFrm.h"
#include "smcDoc.h"
#include "smcView.h"
#include "NewProfileDlg.h"
#include "ProfileListDlg.h"

#include "hyperlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmcApp

char szGLOBAL_PROFILE[MAX_PATH] = "jmc.ini";


BEGIN_MESSAGE_MAP(CSmcApp, CWinApp)
	//{{AFX_MSG_MAP(CSmcApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW_PROFILE, OnFileNewProfile)
	ON_COMMAND(ID_FILE_SAVEPROFILE, OnFileSaveprofile)
	ON_COMMAND(ID_FILE_LOADPROFILE, OnFileLoadprofile)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	//}}AFX_MSG_MAP
	// Standard file based document commands
/*	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
*/
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmcApp construction

CSmcApp::CSmcApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSmcApp object

CSmcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSmcApp initialization
const GUID CLSID_VBScript = { 0xb54f3741, 0x5b07, 0x11cf, { 0xa4, 0xb0,  0x0,  0xaa,  0x0,  0x4a,  0x55,  0xe8 } };
const GUID CLSID_JScript = {0xF414C260,0x6AC0,0x11CF,{0xB6, 0xD1, 0x00, 0xAA , 0x00, 0xBB, 0xBB,0x58}};

BOOL CSmcApp::InitInstance()
{
    CoInitialize (NULL);

//vls-begin// base dir
//    GetCurrentDirectory(MAX_PATH, szGLOBAL_PROFILE );
    strcpy(szGLOBAL_PROFILE, szBASE_DIR);
//vls-end//
    if ( szGLOBAL_PROFILE[strlen(szGLOBAL_PROFILE) -1] != '\\'  ) 
        strcat(szGLOBAL_PROFILE, "\\jmc.ini");
    else
        strcat(szGLOBAL_PROFILE, "jmc.ini");

    //  check JMC.ini exist in the windows directory 
/*    char fname[MAX_PATH];
    GetWindowsDirectory(fname, MAX_PATH);
    if ( fname[strlen(fname) -1] != '\\'  ) 
        strcat(fname, "\\jmc.ini");
    else
        strcat(fname, "jmc.ini");

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile (fname, &fd);
    if ( hFind != INVALID_HANDLE_VALUE ) {
        FindClose (hFind);
        CopyFile( fname, szGLOBAL_PROFILE, FALSE );
        DeleteFile(fname);
    }
*/

    UINT  nSize;
    LPBYTE pData;
	// Font initialization
    if ( !::GetPrivateProfileBinary ("Script" , "LANGGUID" ,&pData, &nSize, szGLOBAL_PROFILE) ) {
        m_guidScriptLang = CLSID_JScript;
    }
    else {
        ASSERT(nSize == sizeof(m_guidScriptLang));
        memcpy( &m_guidScriptLang, pData , nSize);
        delete pData;
    }

//vls-begin// base dir    
//    CreateDirectory("settings", NULL);
    CreateDirectory(szSETTINGS_DIR, NULL);
//vls-end//
    
    if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSmcDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSmcView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

    char buff[4096] = "";
    DWORD ret =  ::GetPrivateProfileString("Main", "LangFile", "language.ini", buff, 4096 , szGLOBAL_PROFILE);
	strcpy(langfile,buff);
    ret =  ::GetPrivateProfileString("Main", "LangSect", "English", buff, 4096 , szGLOBAL_PROFILE);
	strcpy(langsect,buff);
	if ( strlen(m_lpCmdLine) ) 
        m_strCurrentProfile = m_lpCmdLine;
    else {
		ret =  ::GetPrivateProfileString("Main", "LastProfile", "Default", buff, 4096 , szGLOBAL_PROFILE);
        m_strCurrentProfile = buff;
    }

//vls-begin// script files
    MakeLocalPath(szPROFILESCRIPT, m_strCurrentProfile, szSETTINGS_DIR);
    strcat(szPROFILESCRIPT, ".scr");
//vls-end//

    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	ASSERT(AfxGetMainWnd());
	((CMainFrame*)AfxGetMainWnd())->RestorePosition();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_cWww2;
	CHyperLink	m_cWww1;
	CHyperLink	m_cWww;
	CHyperLink	m_cEmail;
	CHyperLink	m_cEmail2;
	CHyperLink	m_cEmail3;
	CString	m_strCopyright;
	CString	m_strProductName;
	CString	m_strVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    // m_nIDHelp = CAboutDlg::IDD;
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strCopyright = _T("");
	m_strProductName = _T("");
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_WWW1, m_cWww1);
	DDX_Control(pDX, IDC_WWW2, m_cWww2);
	DDX_Control(pDX, IDC_WWW, m_cWww);
	DDX_Control(pDX, IDC_EMAIL, m_cEmail);
	DDX_Control(pDX, IDC_EMAIL2, m_cEmail2);
	DDX_Control(pDX, IDC_EMAIL3, m_cEmail3);
	DDX_Text(pDX, IDC_COPYRIGHT, m_strCopyright);
	DDX_Text(pDX, IDC_PRODUCT_NAME, m_strProductName);
	DDX_Text(pDX, IDC_VERTION, m_strVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSmcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSmcApp commands

BOOL GetPrivateProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	BYTE** ppData, UINT* pBytes, LPCTSTR lpszFile)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
	ASSERT(ppData != NULL);
	ASSERT(pBytes != NULL);
	*ppData = NULL;
	*pBytes = 0;

    ASSERT(lpszFile != NULL);

    char buff[4096] = "";
    DWORD ret =  ::GetPrivateProfileString(lpszSection, lpszEntry, "", buff, 4096 , lpszFile);

    if ( ret <= 0 ) 
        return FALSE;

	int nLen = strlen(buff);
	*pBytes = nLen/2;
	*ppData = new BYTE[*pBytes];
	for (int i=0;i<nLen;i+=2)
	{
		(*ppData)[i/2] = (BYTE)
			(((buff[i+1] - 'A') << 4) + (buff[i] - 'A'));
	}
	return TRUE;
}

BOOL WritePrivateProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPBYTE pData, UINT nBytes, LPCTSTR lpszFile)
{
	ASSERT(lpszSection != NULL);
	// convert to string and write out
	LPTSTR lpsz = new TCHAR[nBytes*2+1];
	for (UINT i = 0; i < nBytes; i++)
	{
		lpsz[i*2] = (TCHAR)((pData[i] & 0x0F) + 'A'); //low nibble
		lpsz[i*2+1] = (TCHAR)(((pData[i] >> 4) & 0x0F) + 'A'); //high nibble
	}
	lpsz[i*2] = 0;

    BOOL bResult = ::WritePrivateProfileString(lpszSection, lpszEntry, lpsz, lpszFile);
	delete[] lpsz;
	return bResult;
}


BOOL WritePrivateProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue,LPCTSTR lpszFile)
{
    ASSERT(lpszSection);
    ASSERT(lpszEntry);
    ASSERT(lpszFile);

    char buff[32] = "";
    itoa(nValue, buff , 10 );
    
    WritePrivateProfileString(lpszSection, lpszEntry , buff, lpszFile);
    return TRUE;
}

void CSmcApp::OnFileNewProfile() 
{
	CSmcDoc* pDoc = (CSmcDoc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
	ASSERT_KINDOF(CSmcDoc, pDoc);
	
	CNewProfileDlg dlg(AfxGetMainWnd());
	if ( dlg.DoModal() != IDOK ) 
		return;

	pDoc->DoProfileSave();

//vls-begin// base dir
//    CString strProfileIni = dlg.m_strName + ".opt";
    CString strProfileIni(szSETTINGS_DIR);
	strProfileIni += "\\" + dlg.m_strName + ".opt";
//vls-end//
	CString t1,t2;
    t1.LoadString(IDS_QUES_NEW_PROFILE);
    t2.LoadString(IDS_PROFILE_PARAMS_CAPTION);
    if (AfxGetMainWnd()->MessageBox(t1, 
        t2 , MB_YESNO | MB_ICONQUESTION) == IDYES ) {
        // Copy profile settings 
//vls-begin// base dir
//        CString strCurrOptionsFile(".\\settings\\");
//        strCurrOptionsFile += m_strCurrentProfile +".opt";
//        CopyFile(strCurrOptionsFile, strProfileIni, FALSE);
//        
//        CString strCurrHotkeysFile(".\\settings\\");
//        strCurrHotkeysFile += m_strCurrentProfile +".hot";
//        CString strNewProfileHotkeys(".\\settings\\");
//        strNewProfileHotkeys += dlg.m_strName +".hot";
        CString strCurrOptionsFile(szSETTINGS_DIR);
        strCurrOptionsFile += "\\" + m_strCurrentProfile +".opt";
        CopyFile(strCurrOptionsFile, strProfileIni, FALSE);
        
        CString strCurrHotkeysFile(szSETTINGS_DIR);
        strCurrHotkeysFile += "\\" + m_strCurrentProfile +".hot";
        CString strNewProfileHotkeys(szSETTINGS_DIR);
        strNewProfileHotkeys += "\\" + dlg.m_strName +".hot";
//vls-end//
        CopyFile(strCurrHotkeysFile, strNewProfileHotkeys, FALSE);
        
        // Try to copy macro file
        CString strCurrStartFile = GetProfileString("Options" , "AutoLoadFile" , "" );
        if ( strCurrStartFile.GetLength() && dlg.m_strStartFile.GetLength() ) 
            CopyFile(strCurrStartFile, dlg.m_strStartFile, FALSE);
    }

//vls-begin// base dir
//	::WritePrivateProfileString("Options" , "AutoLoadFile" , dlg.m_strStartFile, strProfileIni);
//	::WritePrivateProfileString("Options" , "AutoSaveFile" , dlg.m_strSaveFile, strProfileIni);
    char p[MAX_PATH+2];
    MakeLocalPath(p, dlg.m_strStartFile, szBASE_DIR);
	::WritePrivateProfileString("Options" , "AutoLoadFile" , p, strProfileIni);
    MakeLocalPath(p, dlg.m_strSaveFile, szBASE_DIR);
	::WritePrivateProfileString("Options" , "AutoSaveFile" , p, strProfileIni);
//vls-end//
	::WritePrivateProfileString("Options" , "AutoSaveCommand" , dlg.m_strCommand, strProfileIni);

    m_strCurrentProfile = dlg.m_strName;
//vls-begin// script files
    MakeLocalPath(szPROFILESCRIPT, m_strCurrentProfile, szSETTINGS_DIR);
    strcat(szPROFILESCRIPT, ".scr");
//vls-end//

	OnFileNew();
}

void CSmcApp::OnFileSaveprofile() 
{
	CSmcDoc* pDoc = (CSmcDoc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
	ASSERT_KINDOF(CSmcDoc, pDoc);

	pDoc->DoProfileSave();
    CString t;
    t.LoadString(IDS_PROFILE_SAVED);
    AfxGetMainWnd()->MessageBox(t , ::AfxGetAppName() , MB_OK | MB_ICONINFORMATION);
}


void CSmcApp::OnFileLoadprofile() 
{
	CProfileListDlg dlg(AfxGetMainWnd());

	if ( dlg.DoModal() != IDOK ) 
		return;

	CSmcDoc* pDoc = (CSmcDoc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
	ASSERT_KINDOF(CSmcDoc, pDoc);

	pDoc->DoProfileSave();
    pDoc->DeleteContents();
	
	m_strCurrentProfile = dlg.m_strProfile;
//vls-begin// script files
    MakeLocalPath(szPROFILESCRIPT, m_strCurrentProfile, szSETTINGS_DIR);
    strcat(szPROFILESCRIPT, ".scr");
//vls-end//
	OnFileNew();
}

int CSmcApp::ExitInstance() 
{
	::WritePrivateProfileString("Main", "LastProfile", (LPCSTR)m_strCurrentProfile , szGLOBAL_PROFILE);
	::WritePrivateProfileString("Main", "LangFile", langfile, szGLOBAL_PROFILE);
	::WritePrivateProfileString("Main", "LangSect", langsect, szGLOBAL_PROFILE);
    ::WritePrivateProfileBinary("Script" , "LANGGUID", (LPBYTE)&m_guidScriptLang, sizeof(m_guidScriptLang), szGLOBAL_PROFILE);
	return CWinApp::ExitInstance();
}


void CSmcApp::OnHelpContents() 
{
    WinHelp(0 , HELP_CONTENTS);	
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// load version info 
    char ModuleName[MAX_PATH];
    GetModuleFileName(AfxGetInstanceHandle(), ModuleName, MAX_PATH);


    // Get the version information size for allocate the buffer
    DWORD dwHandle;     
    DWORD dwDataSize = ::GetFileVersionInfoSize(ModuleName, &dwHandle); 
    if ( dwDataSize == 0 )         
        return TRUE;
    // Allocate buffer and retrieve version information
    LPBYTE lpVersionData = new BYTE[dwDataSize]; 
    if (!::GetFileVersionInfo(ModuleName, dwHandle, dwDataSize, (void**)lpVersionData) ) { 
        delete[] lpVersionData;
        return TRUE;    
    }
    // Retrieve the first language and character-set identifier
    UINT nQuerySize;    
    DWORD* pTransTable;
    if (!::VerQueryValue(lpVersionData, _T("\\VarFileInfo\\Translation"),(void **)&pTransTable, &nQuerySize) ) {
        delete[] lpVersionData;
        return TRUE;    
    }
    // Swap the words to have lang-charset in the correct format
    DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));


    LPVOID lpData;    
    CString strBlockName;

    strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"), 
	                     dwLangCharset, "ProductName");
    if ( ::VerQueryValue((void **)lpVersionData, strBlockName.GetBuffer(0), &lpData, &nQuerySize) )        
        m_strProductName = (LPCTSTR)lpData;
    strBlockName.ReleaseBuffer();


    strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"), 
	                     dwLangCharset, "ProductVersion");
    m_strVersion = "Version ";
    if ( ::VerQueryValue((void **)lpVersionData, strBlockName.GetBuffer(0), &lpData, &nQuerySize) )        
        m_strVersion += (LPCTSTR)lpData;
    strBlockName.ReleaseBuffer();

    strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"), 
	                     dwLangCharset, "LegalCopyright");

    if ( ::VerQueryValue((void **)lpVersionData, strBlockName.GetBuffer(0), &lpData, &nQuerySize) )        
        m_strCopyright = (LPCTSTR)lpData;
    strBlockName.ReleaseBuffer();
    
    CString t;
    m_strCopyright += "\nAll rights reserved.";

    UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
