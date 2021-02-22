#pragma once
#pragma once

using namespace std;

class TcpSocket {

private:
	SOCKET		 sock_;
	char		 buf_;
	vector<char> totalBuf_;
	int			 recvbytes_;

public:
	TcpSocket();
	~TcpSocket();

	void Bind();
	void Listen();
	int Accept();
	int Send();
	int Receive();
	void Close();

	SOCKET&			GetSocket() { return sock_; }
	char			GetBuf() { return buf_; }
	vector<char>	GetTotalBuf() { return totalBuf_; }
	int				GetRecvBytes() { return recvbytes_; }


};