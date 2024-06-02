#define WIN32_LEAN_AND_MEAN
//ускоряем компиляцию программы

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
//Подключаем библиотеки для работы с функциями виндовс, создание сетевых приложений, для работы с TCP/IP

using namespace std;


int main()
{
	WSADATA wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ConnectSocket = INVALID_SOCKET;
	char recvBuffer[512];
	//Объявляем переменные для хранения информации о сетевой подсистеме
	//задаём настройки для получения адресной информации
	//получаем адресный результат дл язаданного хоста
	//создаём сокет для прослушивания и соединения
	//принимаем данные от клиента


	const char* sendBuffer = "Hello from server";
	//Этот код создает указатель sendBuffer, который указывает на строку "Hello from server". Данная строка будет использоваться для отправки данных из сервера.

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//инициализируем библиотеку сокетов Winsock для использования в программе. Функция WSAStartup() 
	//вызывается с параметрами MAKEWORD(2, 2) для указания версии Winsock (в данном случае 2.2) и указателем на структуру WSAData (&wsaData), 
	//которая будет использоваться для хранения информации о версии библиотеки и других параметрах инициализации. 
	if (result != 0) {
		cout << "WSAStartup failed with result: " << result << endl;
		return 1;
	}
	//Проверка успешности инициализации Winsock

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	//IPV 4
	hints.ai_socktype = SOCK_STREAM;
	//потоковый сокет
	hints.ai_protocol = IPPROTO_TCP;
	//TCP
	hints.ai_flags = AI_PASSIVE;
	//Указываем критерии при поиске адресса для сокета

	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	//Этот код вызывает функцию getaddrinfo для выполнения поиска адреса IP по заданному сервису или порту. В данном случае, передается NULL в качестве параметра для хоста (что означает, что будет использован локальный хост), 
	//"666" в качестве параметра для порта и hints в качестве подсказок для поиска адреса. 
	//Результат выполнения функции будет сохранен в переменной addrResult.
	if (result != 0) {
		cout << "getaddrinfo failed with error: " << result << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//был ли вызов функции getaddrinfo успешным. Если результат вызова функции не равен нулю, что обычно означает ошибку, то выводится 
	// сообщение об ошибке с указанием кода ошибки, а затем освобождаются ресурсы, связанные с результатом вызова функции getaddrinfo, вызывается функция WSACleanup() 
	//для очистки Winsock API и программа возвращает значение 1, указывающее на ошибку при выполнении программы.

	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << "Socket creation failed" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//Код для создание сокета, который прослушивает входящие соединения
	// использует информацию об адресе (addrResult), которая была получена с помощью функции getaddrinfo. 
	// Если создание сокета завершается успешно, код продолжает выполнение. В противном случае, код выводит сообщение об ошибке ("Socket creation failed"), 
	//освобождает выделенную память для addrResult, завершает использование библиотеки Winsock (WSACleanup) и возвращает 1.


	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	//Этот код означает привязку сокета слушающего сокета (ListenSocket) к конкретному адресу и порту, указанному в addrResult, с использованием функции bind(). В данном случае, 
	//адрес и порт берется из структуры addrResult, которая является результатом функции getaddrinfo(), используемой для получения информации об адресе хоста.
	if (result == SOCKET_ERROR) {
		cout << "Bind failed, error: " << result << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//пытамеся привязать адрес и порт к слушащему сокету 


	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		cout << "Listen failed, error: " << result << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//Прослушиваем входящие соединения
	ConnectSocket = accept(ListenSocket, NULL, NULL);
	if (ConnectSocket == INVALID_SOCKET) {
		cout << "Accept failed, error: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//ожидем от клиента соединения и проверяем было ли оно успешно установленно 
	closesocket(ListenSocket);
	//закрываем сокет

	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(ConnectSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;

			result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
			if (result == SOCKET_ERROR) {
				cout << "Send failed, error: " << result << endl;
				closesocket(ConnectSocket);
				freeaddrinfo(addrResult);
				WSACleanup();
				return 1;
			}
		}
		
		//Принимает сообщения от клиента, выводит колл-во принятых байт 
		//выводит содержимое, а затем отправляет его получателю
		else if (result == 0) {
			cout << "Connection closing" << endl;
		}
		else {
			cout << "Recv failed, error: " << WSAGetLastError() << endl;
			closesocket(ConnectSocket);
			freeaddrinfo(addrResult);
			WSACleanup();
			return 1;
		}
	} while (result > 0);
	//Создаём бесконечный цикл, который выполняет приём данных через сокет, выводит информацию о количестве принятых байт и сами данные, затем отправляет данные обратно клиенту, завершаем код только при ошибке
	result = shutdown(ConnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		cout << "Shutdown failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//Отключаем отправку данных на сокет
	closesocket(ConnectSocket);
	//закрываем сокет
	freeaddrinfo(addrResult);
	//особождаем память
	WSACleanup();
	//завершаме использование библиотеки Winsock
	return 0;
}

