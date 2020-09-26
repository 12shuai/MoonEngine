#pragma once
#include <windows.h>
#include <Commdlg.h>
#include <Shlobj.h>
#include <string>
#pragma comment(lib,"Shell32.lib")

class WinDiagMgr {
public:

	static std::string APIENTRY FileDialog() {
	loop:
		OPENFILENAME ofn = { 0 };
		TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���
		ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С
		ofn.hwndOwner = NULL;//ӵ�д��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
		ofn.lpstrFilter = TEXT("All Files\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//���ù���
		ofn.nFilterIndex = 1;//����������
		ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
		ofn.nMaxFile = sizeof(strFilename);//����������
		ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��
		ofn.lpstrTitle = TEXT("Choose a file");//ʹ��ϵͳĬ�ϱ������ռ���
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
		if (GetOpenFileName(&ofn)) {
			//MessageBox(NULL, strFilename, TEXT("selected file"), 0);
			return strFilename;
		} else {
			//MessageBox(NULL, TEXT("please select a file!"), NULL, MB_ICONERROR);
			//goto loop;
		}

		return "";
	}

	// mini folder select dialog
	static std::string OpenFile() {
		TCHAR szBuffer[MAX_PATH] = { 0 };
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = szBuffer;
		bi.lpszTitle = "Select model file:";
		bi.ulFlags = BIF_BROWSEINCLUDEFILES;
		LPITEMIDLIST idl = SHBrowseForFolder(&bi);
		if (NULL == idl) return NULL;
		SHGetPathFromIDList(idl, szBuffer);
		return szBuffer;
	}

	static std::string OpenFolder() {
		TCHAR szBuffer[MAX_PATH] = { 0 };
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = szBuffer;
		bi.lpszTitle = "Select export folder:";
		bi.ulFlags = BIF_RETURNFSANCESTORS;
		LPITEMIDLIST idl = SHBrowseForFolder(&bi);
		if (NULL == idl) return NULL;
		SHGetPathFromIDList(idl, szBuffer);
		return szBuffer;
	}

};
