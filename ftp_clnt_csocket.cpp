#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "ftp_clnt_csocket.h"
#include <afxsock.h>

#include<stdio.h>
#include<time.h>
#include<string>
#include<cstdlib>
#include<thread>
#include<conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

#include<iostream>
using namespace std;

void replylogcode(int code)
{
	switch (code) {
	case 200:
		printf("Command okay");
		break;
	case 500:
		printf("Syntax error, command unrecognized.");
		printf("This may include errors such as command line too long.");
		break;
	case 501:
		printf("Syntax error in parameters or arguments.");
		break;
	case 202:
		printf("Command not implemented, superfluous at this site.");
		break;
	case 502:
		printf("Command not implemented.");
		break;
	case 503:
		printf("Bad sequence of commands.");
		break;
	case 530:
		printf("Not logged in.");
		break;
	}
	printf("\n");
}

int count_time;

class Client :public CSocket
{
private:
	CSocket *ClientSocket;
	char *ip = new char[30];
	bool passive;
	bool isConnected;
	int increase;
	int p;
	char pathClient[BUFSIZ];
	int error;
public:
	Client()
	{
		strcpy(this->pathClient, "C:\\Users\\Admin");
		this->ClientSocket = new CSocket;
		this->Create();
		this->isConnected = false;
		this->passive = false;
		this->increase = 1;
		this->ip = new char[30];
		this->ip[0] = '\0';
	}

	~Client()
	{
		delete this->ClientSocket;
		delete[]this->ip;
		if (this->isConnected == true)
			this->close();
	}

	void connect()
	{
		char *buf = new char[BUFSIZ + 1];
		int tmpres;

		char * str;
		int codeftp;

		memset(buf, 0, sizeof buf);
		while ((tmpres = this->Receive(buf, BUFSIZ, 0)) > 0) {
			sscanf(buf, "%d", &codeftp);
			printf("%s", buf);
			if (codeftp != 220) {
				replylogcode(codeftp);
				exit(1);
			}

			str = strstr(buf, "220");
			if (str != NULL) {
				break;
			}
			memset(buf, 0, tmpres);
		}

		delete buf;
	}

	void login()
	{
		char *buf = new char[BUFSIZ];
		string temp;

		buf[0] = '\0';
		strcpy(buf, "OPTS UTF8 ON");
		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		//username
		printf("User (%s:(none)): ", this->ip);
		memset(buf, 0, BUFSIZ);
		strcpy(buf, "USER ");
		getline(cin, temp);
		strcat(buf, temp.c_str());
		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		//password
		memset(buf, 0, BUFSIZ);
		printf("Password: ", this->ip);
		strcpy(buf, "PASS ");
		getline(cin, temp);
		strcat(buf, temp.c_str());
		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		sscanf(buf, "%d", &error);
		printf("%s", buf);

		delete buf;
	}

	//Connect toi server
	void open(char *IP)
	{
		wchar_t* temp = new wchar_t[30];
		mbstowcs(temp, IP, 30);
		if (this->Connect(temp, 21)) {
			printf("Connected to %s.\n", IP);
			this->isConnected = true;
		}
		else {
			printf("Connection to %s is failed.\n", IP);
			this->isConnected = false;
		}
		delete temp;
	}

	//Dong ket noi voi server
	void close()
	{
		char *buf = new char[BUFSIZ];

		buf[0] = '\0';
		strcat(buf, "QUIT");

		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		this->isConnected = false;

		delete buf;
	}

	//Duy chuyen thu muc
	void cd(char* name)
	{
		char *buf = new char[BUFSIZ];

		buf[0] = '\0';
		strcat(buf, "CWD ");
		strcat(buf, name);

		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		delete buf;
	}

	//Duong dan thu muc hien tai
	void pwd()
	{
		char *buf = new char[BUFSIZ];

		buf[0] = '\0';
		strcat(buf, "XPWD");

		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		delete buf;
	}

	//Tao thu muc rong
	void mkdir(char* name)
	{
		char *buf = new char[BUFSIZ];

		buf[0] = '\0';
		strcat(buf, "XMKD ");
		strcat(buf, name);

		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		delete buf;
	}

	//Xoa tap tin
	void delete_(char *name)
	{
		char *buf = new char[BUFSIZ];

		buf[0] = '\0';
		strcat(buf, "DELE ");
		strcat(buf, name);

		this->Send(buf, strlen(buf) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		delete buf;
	}

	//Xoa  nhieu tap tin
	void mdelete_(char **filename, int count)
	{
		for (int i = 1;i < count;i++) {
			if (strchr(filename[i], '.') != NULL && strchr(filename[i], '*') == NULL) {
				if (strchr(filename[i], '.') != NULL)
					printf("mdelete %s?", filename[i]);
				char n;
				n = getchar();
				if (n == '\n' || n == 'y')
					this->delete_(filename[i]);
				if (n != '\n') cin.ignore();
			}
			else {
				mdeleteAll(filename[i]);
			}
		}
	}

	void mdeleteAll(char* name)
	{
		int dem = 0;
		char** list = new char*[1000];
		for (int j = 0;j < 1000;j++)
			list[j] = new char[BUFSIZ];

		this->getListFile(name, list, dem);
		for (int j = 0;j < dem;j++) {
			char m;
			printf("mdelete %s?", list[j]);
			m = getchar();
			if (m == '\n' || m == 'y') {
				this->delete_(list[j]);
			}
			if (m != '\n') cin.ignore();
		}

		for (int i = 0;i < 1000;i++)
			delete[]list[i];
		delete[]list;
	}

	//Xoa thu muc rong
	void rmdir(char *name)
	{
		char *buf = new char[BUFSIZ];

		buf[0] = '\0';
		strcat(buf, "XRMD ");
		strcat(buf, name);

		this->Send(buf, strlen(buf) + 1, 0);

		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		delete buf;
	}

private:
	//Liet ke cac tap tin va thu muc
	void getListFile(char* folderName, char** listFile, int &count)
	{
		char *buf = new char[BUFSIZ];
		buf[0] = '\0';

		CSocket *dataTrans = new CSocket;
		char* port = new char[50];

		strcpy(port, getPort(this, this->p));
		dataTrans->Create(this->p);

		int error;
		if (dataTrans->Listen(1)) {
			this->Send(port, strlen(port) + 1, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			memset(buf, 0, BUFSIZ);
			strcpy(buf, "NLST ");
			strcat(buf, folderName);
			this->Send(buf, 5 + strlen(folderName) + 1, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			sscanf(buf, "%d", &error);
			if (error == 550) {
				printf("%s", buf);
				return;
			}
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);

			int dem = 0;
			CSocket *connector = new CSocket;
			if (dataTrans->Accept(*connector)) {
				char *msg; int len = BUFSIZ;
				while (len == BUFSIZ) {
					msg = new char[BUFSIZ + 1];
					len = connector->Receive((char*)msg, len, 0);
					strcpy(listFile[dem], msg);
					dem++;
					delete[] msg;
				}
			}

			char* temp = new char[100000];
			temp[0] = '\0';
			for (int i = 0;i < dem;i++)
				strcat(temp, listFile[i]);
			char*word = new char[100];
			word = strtok(temp, "\\\n");
			while (word != NULL) {
				if (strcmp(folderName, word) != 0) {
					strcpy(listFile[count], word);
					listFile[count][strlen(listFile[count]) - 1] = NULL;
					count++;
				}
				word = strtok(NULL, "\\\n");
			}
			delete[]temp;
			delete[]word;
			delete connector;
		}
		dataTrans->Close();

		/*for (int i = 0;i < count;i++)
		printf("%s\n", listFile[i]);*/

		delete[]port;
		delete[]buf;
		delete dataTrans;
	}

	char* getPort(CSocket *socket, int &port_number)
	{
		UINT a;
		CString getport;
		char *port = new char[50];
		CString clientIP;

		this->GetSockName(clientIP, a);
		socket->GetSockName(getport, a);

		/*long nSize = getport.GetLength();
		char* str = new char[nSize];
		memset(str, 0, nSize);
		wcstombs(str, getport, nSize + 1);*/

		long nSize = clientIP.GetLength();
		char* str = new char[nSize];
		memset(str, 0, nSize);
		wcstombs(str, clientIP, nSize + 1);

		int port1, port2;
		a += this->increase;
		port_number = a;
		port1 = a >> 8;
		port2 = a - (port1 << 8);
		memset(port, 0, sizeof port);
		sprintf(port, "PORT %s.%d.%d", str, port1, port2);

		this->increase++;

		delete[]str;
		return port;
	}

public:
	void threadcount()
	{
		count_time = 0;
		while (1)
		{
			count_time++;
			Sleep(1000);
			if (count_time == 7) {
				printf("Time out!\n");
				Sleep(1000);
				exit(0);
			}
		}
	}

	void ls()
	{
		error = 0;
		char *buf = new char[BUFSIZ];
		buf[0] = '\0';

		CSocket *dataTrans = new CSocket;
		char* port = new char[50];

		if (this->passive == false) {
			strcpy(port, getPort(this, this->p));
			dataTrans->Create(this->p);
		}
		else {
			dataTrans->Create();
			wchar_t* temp = new wchar_t[30];
			mbstowcs(temp, this->ip, 30);
			if (dataTrans->Connect(temp, this->p))
				printf("Passive mode was started.\n");
			else {
				printf("Passive mode couldn't started.\n");
				this->passive = false;
				return;
			}
			delete[]temp;
		}

		int received = 0;
		if (dataTrans->Listen(1) && this->passive == false) {
			this->Send(port, strlen(port) + 1, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			printf("%s", buf);
			this->Send("NLST", 5, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			sscanf(buf, "%d %d", &error);
			printf("%s", buf);
			if (strlen(buf) > 70)
				received = 1;
			if (error == 550 || error == 503) return;

			CSocket *connector = new CSocket;
			if (dataTrans->Accept(*connector) == INVALID_SOCKET) {
				printf("Accept is failed.\n");
				return;
			}
			else
			{
				receiveData(*connector);
			}

			delete connector;
		}
		else if (this->passive == true)
		{
			this->Send("NLST", 5, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			sscanf(buf, "%d", &error);
			printf("%s", buf);
			if (error == 550 || error == 503)return;
			if (strlen(buf) > 70)
				received = 1;

			receiveData(*dataTrans);
			this->passive = false;
		}
		dataTrans->Close();

		if (received == 0) {
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			printf("%s", buf);
		}
		memset(buf, 0, BUFSIZ);

		delete[]buf;
		delete[]port;
		delete dataTrans;
	}

	void dir()
	{
		error = 0;
		char *buf = new char[BUFSIZ];
		buf[0] = '\0';

		CSocket *dataTrans = new CSocket;
		char* port = new char[50];

		if (this->passive == false) {
			strcpy(port, getPort(this, this->p));
			dataTrans->Create(this->p);
		}
		else {
			dataTrans->Create();
			wchar_t* temp = new wchar_t[30];
			mbstowcs(temp, this->ip, 30);
			if (dataTrans->Connect(temp, this->p))
				printf("Passive mode was started.\n");
			else {
				printf("Passive mode couldn't started.\n");
				this->passive = false;
				return;
			}
			delete[]temp;
		}

		int received = 0;
		if (dataTrans->Listen(1) && this->passive == false) {
			this->Send(port, strlen(port) + 1, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			printf("%s", buf);
			this->Send("LIST", 5, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			sscanf(buf, "%d", &error);
			printf("%s", buf);
			if (strlen(buf) > 70)
				received = 1;
			if (error == 550 || error == 503)return;

			CSocket *connector = new CSocket;
			if (dataTrans->Accept(*connector)) {
				receiveData(*connector);
			}
			else
				return;

			delete connector;
		}
		else if (this->passive == true) {
			this->Send("LIST", 5, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			sscanf(buf, "%d", &error);
			printf("%s", buf);
			if (strlen(buf) > 70)
				received = 1;
			if (error == 550 || error == 503)return;

			receiveData(*dataTrans);
			this->passive = false;
		}
		dataTrans->Close();

		if (received == 0) {
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			printf("%s", buf);
		}
		memset(buf, 0, BUFSIZ);

		delete[]buf;
		delete[]port;
		delete dataTrans;
	}

private:
	void receiveData(CSocket &dataTrans)
	{
		char *msg; int len = BUFSIZ;
		while (len == BUFSIZ) {
			msg = new char[BUFSIZ + 1];
			len = dataTrans.Receive((char*)msg, BUFSIZ, 0);
			sscanf(msg, "%d", &error);
			msg[len] = '\0';
			printf("%s", msg);
			delete[] msg;
		}
	}

	void receiveDataLoadFile(CSocket &dataTrans, FILE* f)
	{
		char *msg; int len = BUFSIZ;
		while (len!=0/*len == BUFSIZ*/)
		{
			msg = new char[BUFSIZ + 1];
			len = dataTrans.Receive(msg, BUFSIZ, 0);
			fwrite(msg, len, 1, f);
			delete[] msg;
		}
		dataTrans.Close();
	}

	void sendDataUpFile(CSocket &dataTrans, FILE* f)
	{
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *msg; int len = 1024;
		while (size > 0) {
			msg = new char[len + 1];msg[0] = '\0';
			if (size < len) {
				fread(msg, 1, size, f);
				dataTrans.Send((char*)msg, size, 0);
			}
			else {
				fread(msg, 1, len, f);
				dataTrans.Send((char*)msg, len, 0);
			}
			size -= len;

			delete[] msg;
		}
		dataTrans.Close();
	}

	//Thay doi duong dan client
	bool dirExists(char* dirName_in)
	{
		DWORD ftyp = GetFileAttributesA(dirName_in);
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return true;   // this is a directory!

		return false;    // this is not a directory!
	}

public:
	void lcd(char* dir_name) {
		if (strchr(dir_name, ':') != NULL) {
			if (this->dirExists(dir_name)) {
				strcpy(this->pathClient, dir_name);
				printf("Local directory now %s\n", this->pathClient);
			}
			else
				printf("%s: File not found.\n", dir_name);
		}
		else {
			strcat(pathClient, "\\");
			strcat(pathClient, dir_name);
			if (this->dirExists(pathClient))
				printf("Local directory now %s\n", this->pathClient);
			else {
				strcpy(this->pathClient, "C:\\Users\\Admin");
				printf("%s: File not found.\n", dir_name);
			}
		}
	}

private:
	void lcdSetDefault() {
		strcpy(this->pathClient, "C:\\Users\\Admin");
		printf("Local directory now %s.\n", this->pathClient);
	}

	//down file tu server
	void getFileName(char* fileName, char* pathFile) {
		char* temp = new char[strlen(pathFile) + 1];
		strcpy(temp, pathFile);
		char*word = new char[100];word[0] = '\0';
		word = strtok(temp, "\\");
		while (word != NULL) {
			strcpy(fileName, word);
			word = strtok(NULL, "\\");
		}
		delete[]temp;
		delete[]word;
	}

	void mgetAll(char *name) {
		int dem = 0;
		char** list = new char*[1000];
		for (int j = 0;j < 1000;j++)
			list[j] = new char[BUFSIZ];

		this->getListFile(name, list, dem);
		if (dem == 0) {
			printf("Does not have any files to Download.\n");
			return;
		}

		for (int j = 0;j < dem;j++) {
			char m;
			printf("mget %s?", list[j]);
			m = getchar();
			if (m == '\n' || m == 'y') {
				this->get(list[j]);
			}
			if (m != '\n') cin.ignore();
		}

		for (int i = 0;i < 1000;i++)
			delete[]list[i];
		delete[]list;
	}

public:
	void threadReceive() {
		char *msg; int len = BUFSIZ;
		while (1) {
			msg = new char[BUFSIZ + 1];
			if (FD_READ) {
				len = this->Receive((char*)msg, len, 0);
				sscanf(msg, "%d", &error);
				if (error == 425) {
					msg[len] = '\0';
					printf("%s", msg);
					exit(0);
					Sleep(3000);
					//return;
				}
				delete[] msg;
			}
		}
	}

	void get(char *filename)
	{
		if (strchr(filename, '.') == NULL) {
			printf("550 File not found.\n");
			return;
		}

		char *buf = new char[BUFSIZ];
		buf[0] = '\0';
		char*fileName = new char[100];fileName[0] = '\0';
		getFileName(fileName, filename);

		CSocket *dataTrans = new CSocket;
		char* port = new char[50];

		if (AfxSocketInit() == FALSE)
		{
			printf("Failed\n");
			return;
		}
		if (this->passive == false) {
			strcpy(port, getPort(this, this->p));
			if (dataTrans->Create(this->p) == FALSE) {
				printf("Socket create failed\n");
				return;
			}
			if (dataTrans->Listen() == FALSE) {
				printf("Socket listen failed\n");
				return;
			}
		}
		else {
			dataTrans->Create();
			wchar_t* temp = new wchar_t[30];
			mbstowcs(temp, this->ip, 30);
			if (dataTrans->Connect(temp, this->p)==TRUE)
				printf("Passive mode was started.\n");
			else {
				printf("Passive mode couldn't started.\n");
				this->passive = false;
				return;
			}
			delete[]temp;
		}

		if (this->passive == false) {
			this->Send(port, strlen(port) + 1, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			printf("%s", buf);

			memset(buf, 0, BUFSIZ);
			strcat(buf, "RETR ");
			strcat(buf, filename);
			this->Send(buf, 5 + strlen(filename) + 1, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			sscanf(buf, "%d", &error);
			printf("%s", buf);
			if (error == 550 || error == 503) return;
			sscanf(buf, "%d", &error);
			if (error == 550 || error == 503) {
				printf("%s", buf);
				return;
			}
			char* pathfile = new char[100];
			pathfile[0] = '\0';
			strcat(pathfile, this->pathClient);
			strcat(pathfile, "\\");
			strcat(pathfile, fileName);
			FILE *f = fopen(pathfile, "wb");
			if (f) {
				CSocket *connector = new CSocket;
				if (dataTrans->Accept(*connector)) {
					receiveDataLoadFile(*connector, f);
				}
				delete connector;
				delete[]pathfile;
				fclose(f);
			}
		}
		else if (this->passive == true) {
			memset(buf, 0, BUFSIZ);
			strcat(buf, "RETR ");
			strcat(buf, filename);
			this->Send(buf, 5 + strlen(filename) + 1, 0);
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			sscanf(buf, "%d", &error);
			printf("%s", buf);
			if (error == 550 || error == 503) return;
			sscanf(buf, "%d", &error);
			if (error == 550 || error == 503) return;

			char* pathfile = new char[100];
			pathfile[0] = '\0';
			strcat(pathfile, this->pathClient);
			strcat(pathfile, "\\");
			strcat(pathfile, fileName);
			FILE *f = fopen(pathfile, "wb");
			if (f) {
				receiveDataLoadFile(*dataTrans, f);
				fclose(f);
			}
			delete[]pathfile;
			this->passive = false;
		}
		dataTrans->Close();

		if (error != 226) {
			memset(buf, 0, BUFSIZ);
			this->Receive(buf, BUFSIZ, 0);
			printf("%s", buf);
		}
		memset(buf, 0, BUFSIZ);

		delete[]fileName;
		delete[]buf;
		delete[]port;
		delete dataTrans;
	}

	void mget(char **filename, int count) {
		if (count == 1) {
			printf("Remote files\nRemote files mget.\n");
			return;
		}

		char *buf = new char[BUFSIZ];
		buf[0] = '\0';
		this->Send("TYPE A", 7, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);
		memset(buf, 0, BUFSIZ);

		for (int i = 1;i < count;i++) {
			if (strchr(filename[i], '.') != NULL && strchr(filename[i], '*') == NULL) {
				if (strchr(filename[i], '.') != NULL)
					printf("mget %s?", filename[i]);
				char n;
				n = getchar();
				if (n == '\n' || n == 'y')
					this->get(filename[i]);
				if (n != '\n') cin.ignore();
			}
			else {
				mgetAll(filename[i]);
			}
		}

		delete[]buf;
	}

	//Up du lieu len server
private:
	int getPos(char *s, char c) {
		int res = -1;
		int len = strlen(s);
		for (int i = 0;i < len;i++)
			if (s[i] == c)
				res = i;
		return res;
	}

	void normalization(char* s) {
		int len = strlen(s);
		int i;
		for (i = 0;i < len - 3;i++)
			s[i] = s[i + 1];
		s[i - 1] = '\0';
	}

	void getListFileName_Folder(char *folderName, char **listFile, int &count)
	{
		count = 0;
		char* cmd = new char[50];
		char *fileName = new char[30];
		getFileName(fileName, folderName);
		char*path = new char[100];
		strcpy(path, this->pathClient);
		strcat(path, "\\");
		strcat(path, folderName);
		int loai = 0;
		char*tail = new char[10];

		if (strstr(path, "*.") != NULL || strcmp(fileName, "*") == 0) {
			int pos = getPos(path, '*');
			if (strstr(path, "*.") != NULL) {
				loai = 1;
				int dem = 0;
				pos = getPos(path, '.');
				for (int i = pos;i < strlen(path);i++)
					tail[dem++] = path[i];
				tail[dem] = '\0';
				folderName[strlen(folderName) - strlen(tail) - 2] = '\0';
			}
			else
				folderName[strlen(folderName) - 2] = '\0';
			path[pos - 1] = '\0';
		}

		sprintf(cmd, "forfiles /p %s> %s\\data.dll", path, this->pathClient);
		system(cmd);
		sprintf(cmd, "%s\\data.dll", this->pathClient);

		FILE* f = fopen(cmd, "rb");
		if (f) {
			int type = 0;
			if (strcmp(folderName, fileName) == 0) {
				type = 1;
			}

			char* pathname = new char[100];
			char* name = new char[100];
			while (!feof(f)) {
				memset(name, 0, 100);
				memset(pathname, 0, 100);
				fgets(name, 100, f);
				if (name[0] == '\r') name[0] = '\0';
				if (strlen(name) != 0) {
					normalization(name);
					if (loai == 1) {
						if (strstr(name, tail) != NULL) {
							if (type == 0) {
								strcpy(pathname, folderName);
								strcat(pathname, "\\");
							}
							strcat(pathname, name);
							strcpy(listFile[count++], pathname);
						}
					}
					else {
						if (type == 0) {
							strcpy(pathname, folderName);
							strcat(pathname, "\\");
						}
						strcat(pathname, name);
						strcpy(listFile[count++], pathname);
					}
				}
			}
			fclose(f);
			delete[]pathname;
			delete[]name;
		}
		else {
			printf("Cann't get listFile.\n");
			return;
		}

		sprintf(cmd, "del %s\\data.dll", this->pathClient);
		system(cmd);

		delete[]tail;
		delete[]cmd;
		delete[]fileName;
		delete[]path;
	}

	void mputAll(char *name)
	{
		int dem = 0;
		char** list = new char*[1000];
		for (int j = 0;j < 1000;j++)
			list[j] = new char[BUFSIZ];

		this->getListFileName_Folder(name, list, dem);
		if (dem == 0) {
			printf("Does not have any files to upload.\n");
			return;
		}
		for (int j = 0;j < dem;j++) {
			char m;
			printf("mput %s?", list[j]);
			m = getchar();
			if (m == '\n' || m == 'y') {
				this->put(list[j]);
			}
			if (m != '\n') cin.ignore();
		}

		for (int i = 0;i < 1000;i++)
			delete[]list[i];
		delete[]list;
	}

public:
	void put(char *filename)
	{
		char* port;
		CSocket *dataTrans = new CSocket;
		char*FileName = new char[50];
		getFileName(FileName, filename);

		port = new char[50];
		if (this->passive == false) {
			strcpy(port, getPort(this, this->p));
			dataTrans->Create(this->p);
		}
		else {
			dataTrans->Create();
			wchar_t* temp = new wchar_t[30];
			mbstowcs(temp, this->ip, 30);
			if (dataTrans->Connect(temp, this->p))
				printf("Passive mode was started.\n");
			else {
				printf("Passive mode couldn't started.\n");
				this->passive = false;
				return;
			}
			delete[]temp;
		}

		if (dataTrans->Listen(1) && this->passive == false) {
			char* pathfile = new char[100];
			pathfile[0] = '\0';
			strcat(pathfile, this->pathClient);
			strcat(pathfile, "\\");
			strcat(pathfile, filename);

			FILE *f = fopen(pathfile, "rb");
			if (f) {
				char *buf = new char[BUFSIZ];
				buf[0] = '\0';

				this->Send(port, strlen(port) + 1, 0);
				memset(buf, 0, BUFSIZ);
				this->Receive(buf, BUFSIZ, 0);
				printf("%s", buf);
				memset(buf, 0, BUFSIZ);
				strcat(buf, "STOR ");
				strcat(buf, FileName);
				this->Send(buf, 5 + strlen(FileName) + 1, 0);
				memset(buf, 0, BUFSIZ);
				this->Receive(buf, BUFSIZ, 0);
				sscanf(buf, "%d", &error);
				printf("%s", buf);
				if (error == 550 || error == 503) return;

				CSocket *connector = new CSocket;
				if (dataTrans->Accept(*connector)) {
					sendDataUpFile(*connector, f);
				}

				memset(buf, 0, BUFSIZ);
				this->Receive(buf, BUFSIZ, 0);
				printf("%s", buf);
				memset(buf, 0, BUFSIZ);

				delete[]buf;
				delete[]pathfile;
				delete connector;

				fclose(f);
			}
			else {
				printf("%s: File not found.\n", filename);
			}
		}
		else if (this->passive == true) {
			char* pathfile = new char[100];
			pathfile[0] = '\0';
			strcat(pathfile, this->pathClient);
			strcat(pathfile, "\\");
			strcat(pathfile, filename);

			FILE *f = fopen(pathfile, "rb");
			if (f) {
				char *buf = new char[BUFSIZ];
				buf[0] = '\0';

				memset(buf, 0, BUFSIZ);
				strcat(buf, "STOR ");
				strcat(buf, FileName);
				this->Send(buf, 5 + strlen(FileName) + 1, 0);
				memset(buf, 0, BUFSIZ);
				this->Receive(buf, BUFSIZ, 0);
				sscanf(buf, "%d", &error);
				printf("%s", buf);
				if (error == 550 || error == 503) return;

				sendDataUpFile(*dataTrans, f);

				if (FD_READ) {
					memset(buf, 0, BUFSIZ);
					this->Receive(buf, BUFSIZ, 0);
					printf("%s", buf);
				}
				memset(buf, 0, BUFSIZ);

				delete[]buf;
				delete[]pathfile;

				fclose(f);

				this->passive = false;
			}
			else {
				printf("%s: File not found.\n", filename);
			}
		}
		dataTrans->Close();

		delete[]FileName;
		delete[]port;
		delete dataTrans;
	}

	void mput(char **filename, int count)
	{
		if (count == 1) {
			printf("Local files.\nLocal files mput.\n");
			return;
		}

		for (int i = 1;i < count;i++) {
			if (strchr(filename[i], '.') != NULL && strchr(filename[i], '*') == NULL) {
				if (strchr(filename[i], '.') != NULL)
					printf("mput %s?", filename[i]);
				char n;
				n = getchar();
				if (n == '\n' || n == 'y')
					this->put(filename[i]);
				if (n != '\n') cin.ignore();
			}
			else {
				if (strchr(filename[i], '.') == NULL && strchr(filename[i], '*') == NULL) {
					printf("\n");
				}
				else
					mputAll(filename[i]);
			}
		}
	}

	//Chuy?n sang passive mode
	void pasv()
	{
		char *buf = new char[BUFSIZ];
		buf[0] = '\0';
		char* port = getPort(this, this->p);

		this->Send(port, strlen(port) + 1, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);
		this->Send("PASV", 5, 0);
		memset(buf, 0, BUFSIZ);
		this->Receive(buf, BUFSIZ, 0);
		printf("%s", buf);

		char*port1 = new char[50], *port2 = new char[50];
		int count = 0;
		port = strtok(buf, ",)");
		while (port != NULL) {
			if (count == 4)
				strcpy(port1, port);
			if (count == 5)
				strcpy(port2, port);
			port = strtok(NULL, ",)");
			count++;
		}
		this->p = (atoi(port1) << 8) + atoi(port2);
		this->passive = true;

		delete[]port1;
		delete[]port2;
		delete[]port;
	}

	//Chuong trinh command
	void Process() {
		char* temp = new char[300];
		char* cmd = new char[300];

		while (1) {
			int count = 0;
			char **margv = new char*[10];
			for (int i = 0;i < 10;i++) {
				margv[i] = new char[300];
				margv[i][0] = '\0';
			}

			clock_t start = clock();
			printf("ftp>");

			cin.getline(cmd, 300);
			temp = strtok(cmd, " ");

			while (temp != NULL) {
				strcat(margv[count], temp);
				temp = strtok(NULL, " ");
				count++;
			}

			if (this->isConnected) {
				clock_t finish = clock();
				if ((double)(finish - start) / CLOCKS_PER_SEC > 120) {
					printf("Connection closed by remote host. You must quit and open it again.\n");
					//this->isConnected = false;
					this->close();
					continue;
				}

				if (strcmp(margv[0], "quit") == 0 || strcmp(margv[0], "exit") == 0) {
					this->close();
					Sleep(1000);
					return;
				}
				else if (strcmp(margv[0], "cd") == 0) {
					if (count < 2) {
						printf("Remote directory\ncd remote directory.\n");
					}
					else
						this->cd(margv[1]);
				}
				else if (strcmp(margv[0], "pwd") == 0) {
					this->pwd();
				}
				else if (strcmp(margv[0], "mkdir") == 0) {
					if (count < 2) {
						printf("Directory name\nmkdir directory - name\n");
					}
					else
						this->mkdir(margv[1]);
				}
				else if (strcmp(margv[0], "delete") == 0) {
					if (count < 2) {
						printf("Remote file\ndelete remote file.\n");
					}
					else
						this->delete_(margv[1]);
				}
				else if (strcmp(margv[0], "mdelete") == 0) {
					this->mdelete_(margv, count);
				}
				else if (strcmp(margv[0], "rmdir") == 0) {
					if (count < 2) {
						printf("Directory name\nrmdir directory - name\n");
					}
					else
						this->rmdir(margv[1]);
				}
				else if (strcmp(margv[0], "ls") == 0) {
					this->ls();
				}
				else if (strcmp(margv[0], "dir") == 0) {
					this->dir();
				}
				else if (strcmp(margv[0], "lcd") == 0) {
					if (count == 2)
						this->lcd(margv[1]);
					else if (count == 1) {
						this->lcdSetDefault();
					}
					else
						printf("lcd local directory.\n");
				}
				else if (strcmp(margv[0], "get") == 0) {
					this->get(margv[1]);
				}
				else if (strcmp(margv[0], "put") == 0) {
					this->put(margv[1]);
				}
				else if (strcmp(margv[0], "mget") == 0) {
					this->mget(margv, count);
				}
				else if (strcmp(margv[0], "mput") == 0) {
					this->mput(margv, count);
				}
				else if (strcmp(margv[0], "pasv") == 0 || strcmp(margv[0], "passive") == 0) {
					this->pasv();
				}
				else if (strcmp(margv[0], "cls") == 0) {
					system("cls");
				}
				else if (strcmp(margv[0], "open") == 0) {
					printf("Already connected to %s, use disconnect first.\n", this->ip);
				}
				else {
					printf("Invalid command.\n");
				}
			}
			else {
				if (strcmp(margv[0], "lcd") == 0) {
					if (count == 2)
						this->lcd(margv[1]);
					else if (count == 1) {
						this->lcdSetDefault();
					}
					else
						printf("lcd local directory.\n");
				}
				else if (strcmp(margv[0], "open") == 0) {
					if (count == 2) {
						strcpy(this->ip, margv[1]);
						this->open(margv[1]);
					}
					else {
						string temp;
						printf("To (ip): ");
						getline(cin, temp);
						strcpy(this->ip, temp.c_str());

						this->open(this->ip);
					}

					if (this->isConnected) {
						this->connect();
						this->login();
						if (error == 530)
							this->isConnected = false;
					}
				}
				else if (strcmp(margv[0], "quit") == 0 || strcmp(margv[0], "exit") == 0) {
					return;
				}
				else
					printf("Not connected.\n");
			}

			for (int i = 0;i < 10;i++)
				delete[] margv[i];
			delete[] margv;
		}

		delete cmd;
		delete temp;
	}
};

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	//printf("%d, %d, %d\n", EWOULDBLOCK, EAGAIN, SOCKET_ERROR);

	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL) {
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0)) {
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else {
			// TODO: code your application's behavior here.
			// Khoi tao thu vien Socket
			if (AfxSocketInit() == FALSE) {
				cout << "Khong the khoi tao Socket Libraray";
				return FALSE;
			}

			Client mySocket;

			//Xu ly
			mySocket.Process();
		}
	}
	else {
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}//1612829-1612842-1612774
