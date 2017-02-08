#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Winsock.h>
#include <stdio.h>
#pragma comment(lib, "WSOCK32.LIB") 
#define ReportWSAError(s) printf("Error #%d in %s./n", WSAGetLastError(), #s) 
#include <Windows.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

/*支持7种文件方式
txt   1
html  2
img   3
*/
int getType(string);
DWORD WINAPI worker_thread(LPVOID);

int main(int argc, char* argv[])
{
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	SOCKET m_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	//绑定地址
	serveraddr.sin_port = htons(5592);
	//绑定端口
	if (bind(m_server, (LPSOCKADDR)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		ReportWSAError(bind());
		WSACleanup();
		return 1;
	}
	if (listen(m_server, 5) == SOCKET_ERROR)
	{
		ReportWSAError(listen());
		WSACleanup();
		return 1;
	}

	while (TRUE) // Listening circulation
	{
		int nAddrLen = sizeof(struct sockaddr_in);
		struct sockaddr_in remoteAddr;
		struct ThreadParameter {
			sockaddr_in remoteAddr;
			SOCKET  remoteSocket;
			string base_dir;
		} tp;
		tp.base_dir = "D:/jwsy7/";
		tp.remoteSocket = accept(m_server, (SOCKADDR*)&(tp.remoteAddr), &nAddrLen);
		HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)worker_thread, &tp, 0, NULL);
		//WaitForSingleObject(handle, INFINITE); 
	}
	return 0;
}

int getType(string filename)
{
	string f1 = filename;
	char *p;
	p = const_cast<char *>(strstr(f1.c_str(), "."));
	string b1 = p;
	//cout << "b1" << b1 << endl;
	string b2 = b1.substr(1, b1.length());
	//cout << "b2" << b2 << endl;
	if (strcmp("txt", b2.c_str()) == 0)
		return 1;
	else if (strcmp("html", b2.c_str()) == 0)
		return 2;
	else if (strcmp("jpg", b2.c_str()) == 0)
		return 3;
	//return 4;
}

DWORD WINAPI worker_thread(LPVOID pParam)
{
	struct ThreadParameter {
		sockaddr_in remoteAddr;
		SOCKET  remoteSocket;
		string base_dir;
	};
	ThreadParameter *tp = (ThreadParameter *)pParam;
	SOCKET remoteSocket = tp->remoteSocket;
	sockaddr_in remoteAddr = tp->remoteAddr;
	string base_dir = tp->base_dir;
	char buffer[1024];
	int byteRecv;
	if (remoteSocket == SOCKET_ERROR)
	{
		ReportWSAError(accept());
	}
	byteRecv = recv(remoteSocket, buffer, 1023, 0);
	buffer[byteRecv] = 0;
	printf("buffer:%s\n", buffer);
	int fileornot = 0;//0是响应信息，1是file
	stringstream ss1;
	ss1 << buffer;
	char bf1[1024];
	string filename;
	string directcontent;
	//while ((ss1)&&(ss1.getline(bf1, sizeof(buffer))))
	int postflag = 0; int bodyflag = 0; int realflag = 0;
	while ((ss1) && (ss1.getline(bf1, 1024)))
	{
		int type;
		char *q; char* dopostflag;
		cout <<"bf1:"<< bf1 << endl;
		q = strstr(bf1, "POST");
		if (q != 0){
		//找post信息
			postflag = 1;
			cout << "postflag:" << postflag << endl;
			dopostflag = strstr(bf1, "dopost");//检查是否是dopost
			if (dopostflag == 0){
				const char * header;
				header = "HTTP/1.0 404 Not Found\r\nContent-type: text/plain\r\n\r\nNot found";
				send(remoteSocket, header, strlen(header), 0);
				cerr << "";
				fileornot == 3;
			}
			continue;
		}
		char *flag;
		flag = strstr(bf1, "&");
		//if (postflag == 1 && flag!=0){
		//	postflag = 0;
		//	bodyflag = 1;
		//	cout << "bodyflag:" << bodyflag << endl;
		//	continue;//找到空行，直接处理下一句body体信息。
		//}
		//if (bodyflag){
		//	bodyflag = 0;
		//	realflag = 1;
		//	cout << "realflag:" << bodyflag << endl;
		//	continue;
		//}
		if (postflag && flag!=0){
			cout << "here" << endl;
			bodyflag = 0;
			stringstream ss;
			string loginmes;
			ss << bf1;
			ss >> loginmes;
			
			string loginname = loginmes.substr(loginmes .find("=") + 1);
			loginname = loginname.substr(0, loginname.find("&"));
			string password = loginmes.substr(loginmes.find("&") + 1);
			password = password.substr(password.find("=") + 1);
			cout << "login:" << loginname << endl;
			cout << "pwd:" << password << endl;
			if (loginname == "3140105592" && password == "5592"){
				//发送登录成功文本
				//filename = base_dir + "txt/" + "success.txt";
				//封响应消息
				directcontent = "Login Success!";
			}
			else{
				//发送登录失败文本
				//filename = base_dir + "txt/" + "fail.txt";
				//封装响应消息
				directcontent = "Login Fail!";
			}
			fileornot = 0;
			continue;
		}
		char *p;
		p = strstr(bf1, "GET");
		//找到含有GET的那一行
		if (p != 0)
		{
			string sp1 = p;
			string sp2 = sp1.substr(4, sp1.length() - 13);
			string sp3 = sp2.substr(1, sp2.length() - 2);//第二个参数是指定的长度
			cout << "sp3:" << sp3 << endl;
			//处理post
			if (sp3.find("dopost") != -1){
				cout << "ha" << endl;
				string loginname = sp3.substr(sp3.find("=") + 1);
				loginname = loginname.substr(0, loginname.find("&"));
				string password = sp3.substr(sp3.find("&") + 1);
				password = password.substr(password.find("=") + 1);
				cout << "login:" << loginname << endl;
				cout << "pwd:" << password << endl;
				if (loginname == "3140105592" && password == "5592"){
					//发送登录成功文本
					//filename = base_dir + "txt/" + "success.txt";
					//封响应消息
					directcontent = "Login Success!";
				}
				else{
					//发送登录失败文本
					//filename = base_dir + "txt/" + "fail.txt";
					//封装响应消息
					directcontent = "Login Fail!";
				}
				fileornot = 0;
				continue;
			}
			type = getType(sp3);
			//cout << "type:" << type << endl;
			string sp4 = sp3.substr(4, sp3.length() - 4);
			cout << "sp4:" << sp4 << endl;
			if (type == 1) {
				base_dir += "txt/";
			}
			else if (type == 2) {
				base_dir += "html/";
			}
			else if (type == 3) {
				base_dir += "img/";
			}
			cout << "base:" << base_dir << endl;
			filename = base_dir + sp4;
			cout << "file:" << filename << endl;
			filename = filename.substr(0, filename.length());
			fileornot = 1;
		}

	}
	//不获取文件，直接传输响应消息
	if(fileornot == 0){
		const char *header;
		int total = 0;
		string content = "<html><body><h1>"+directcontent+"</h1></body></html>";
		total += content.length();
		printf("total:%d\n", total);
		char num[10];
		_itoa(total, num, 10);
		printf("num:%s\n", num);
		char header2[100] = "";
		strcat(header2, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ");
		//printf("header2:%s\n", header2);
		strcat(header2, num);
		//printf("header2:%s\n", header2);
		strcat(header2, "\r\nConnection: close\r\n\r\n");
		//printf("header2:%s\n2", header2);
		header = (const char*)header2;
		//printf("header:%s\n", header);
		send(remoteSocket, header, strlen(header), 0);
		send(remoteSocket, content.c_str(), content.length(), 0);
		printf("this is the remoteAddr %s \n", inet_ntoa(remoteAddr.sin_addr));
		closesocket(remoteSocket);
	}
	//下面是获取文件的内容输出出去
	//type为1,2,3  plain,html,img
	if(fileornot == 1){
	int type = getType(filename);
	if (type == 1 || type == 2)
	{
		ifstream in(filename.c_str());
		if (!in)
		{
			const char * header;
			header = "HTTP/1.0 404 Not Found\r\nContent-type: text/plain\r\n\r\nNot found";
			send(remoteSocket, header, strlen(header), 0);
			cerr << "";
		}
		else{
			const char *header;

			int total = 0;
			ifstream in2(filename.c_str());
			string content;
			while (getline(in2, content))
			{
				total += content.length();
			}
			printf("total:%d\n", total);
			char num[10];
			_itoa(total, num, 10);
			printf("num:%s\n", num);

			char header2[100] = "";
			switch (type) {
			case 1:
				strcat(header2, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ");
				strcat(header2, num);
				strcat(header2, "\r\nConnection: close\r\n\r\n");
				header = (const char*)header2;
				break;
			case 2:
				strcat(header2, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ");
				//printf("header2:%s\n", header2);
				strcat(header2, num);
				//printf("header2:%s\n", header2);
				strcat(header2, "\r\nConnection: close\r\n\r\n");
				//printf("header2:%s\n2", header2);
				header = (const char*)header2;
				//printf("header:%s\n", header);
				break;
			default:
				header = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nConnection: close\r\n\r\n";
				break;
			}
			send(remoteSocket, header, strlen(header), 0);
			string get_neirong;
			while (getline(in, get_neirong))
			{
				send(remoteSocket, get_neirong.c_str(), get_neirong.length(), 0);
			}
		}
	}
	else
	{
		const char * header;

		/*int total = 0;
		ifstream in2(filename.c_str());
		string content;
		while (!in2.eof())
		{
		char c[1024];
		in2.read(c, sizeof(char) * 1024);
		total += sizeof(char) * 1024;
		}
		printf("total:%d\n", total);
		char num[10];
		_itoa(total, num, 10);
		printf("num:%s\n", num);*/

		ifstream in(filename.c_str(), ios::binary);
		if (!in)
		{
			header = "HTTP/1.0 404 Not Found\r\nContent-type: text/plain\r\n\r\nNot found";
			send(remoteSocket, header, strlen(header), 0);
			cerr << "";
		}
		else {
			/*char header2[100] = "";
			strcat(header2, "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: ");
			strcat(header2, num);
			printf("header2:%s\n", header2);
			strcat(header2, "\r\nConnection: close\r\n\r\n");
			printf("header2:%s\n", header2);
			header = (const char*)header2;
			printf("header:%s\n", header);*/
			header = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nConnection: close\r\n\r\n";
			send(remoteSocket, header, strlen(header), 0);
			while (!in.eof())
			{
				char c[1024];
				in.read(c, sizeof(char)* 1024);
				send(remoteSocket, c, sizeof(char)* 1024, 0);
			}
		}
	}
	printf("this is the remoteAddr %s \n", inet_ntoa(remoteAddr.sin_addr));
	closesocket(remoteSocket);
	}
}