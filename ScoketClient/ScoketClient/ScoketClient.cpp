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
	SOCKET ConnectSocket = INVALID_SOCKET;
	char recvBuffer[512];
	//Объявляем переменные для хранения информации о сетевой подсистеме
//задаём настройки для получения адресной информации
//получаем адресный результат дл язаданного хоста
//создаём сокет для соединения

	const char* sendBuffer1 = "Hello from client 1";
	const char* sendBuffer2 = "Hello from client 2";
	//два константных массива символов

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//инициализируем библиотеку сокетов Winsock для использования в программе. Функция WSAStartup() 
	//вызывается с параметрами MAKEWORD(2, 2) для указания версии Winsock (в данном случае 2.2) и указателем на структуру WSAData (&wsaData), 
	//которая будет использоваться для хранения информации о версии библиотеки и других параметрах инициализации. 
	if (result != 0) {
		cout << "WSAStartup failed with result: " << result << endl;
		return 1;
	}
	//Инициализириуем Winsock
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	// обнуляем структуру hints с помощью функции ZeroMemory, затем устанавливает значение поля ai_family структуры hints равным константе AF_INET,которая показывает
	//что необходимо использовать IPV 4
	hints.ai_socktype = SOCK_STREAM;
	//сокет должен быть установлне для передачи данных TCP
	hints.ai_protocol = IPPROTO_TCP;
	//Протокол TCP

	result = getaddrinfo("localhost", "666", &hints, &addrResult);
	if (result != 0) {
		cout << "getaddrinfo failed with error: " << result << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//Получаем информацию об адрессе для указанного хоста
	//Этот код вызывает функцию getaddrinfo для выполнения поиска адреса IP по заданному сервису или порту. В данном случае, передается NULL в качестве параметра для хоста (что означает, что будет использован локальный хост), 
	//"666" в качестве параметра для порта и hints в качестве подсказок для поиска адреса. 
	//Результат выполнения функции будет сохранен в переменной addrResult.
	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		cout << "Socket creation failed" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//Создаём сокет для подключению к серверу
	//Функция socket создает новый сокет с указанными данными о семействе адресов (address family), 
	//типе сокета (socket type) и протоколе. В данном случае эти данные берутся из структуры addrResult, 
	//которая содержит информацию о соединении. После создания сокета функция connect 
	//используется для установления соединения с удаленным адресом.
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		cout << "Unable to connect to server" << endl;
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//попытка установить соединение по сети
	//Данный код устанавливает соединение с сервером, используя сокет ConnectSocket и адрес, указанный в addrResult->ai_addr. 
	//Функция connect() принимает сокет, адрес и длину адреса в качестве параметров и устанавливает соединение с сервером, указанным в адресе.

	cout << "Sent: " << result << " bytes" << endl;
	//Выводим количество байт, которые были отправлены, на экран с сообщением "Sent: X bytes", где X - количество отправленных байт.
	result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
	if (result == SOCKET_ERROR) {
		cout << "Send failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	cout << "Sent: " << result << " bytes" << endl;
	//Выводим количество байт, которые были отправлены, на экран с сообщением "Sent: X bytes", где X - количество отправленных байт.

	result = shutdown(ConnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		cout << "Shutdown failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	//завершаем передачу данных
	//Этот код отправляет сигнал завершения отправки данных на сокете, указанном переменной ConnectSocket
	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(ConnectSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;
		}
		else if (result == 0) {
			cout << "Connection closed" << endl;
		}
		else {
			cout << "Recv failed, error: " << WSAGetLastError() << endl;
		}
	} while (result > 0);
	//  Открываем сокет для соединения, затем в цикле ожидает получения данных от удаленного хоста. При получении данных выводим информацию о количестве и содержимом принятых байт. Если соединение закрыто, выводим сообщение о закрытии соединения. 
	//Данный код устанавливает цикл do-while, который продолжает получать данные с помощью функции recv() до тех пор, пока количество принятых байт больше нуля (result > 0).

	//Внутри цикла проверяется результат вызова recv() и в зависимости от результата выводится соответствующее сообщение :
	//Если result больше нуля, то выводится количество принятых байт и сами данные.
	//Если result равен 0, то выводится сообщение о закрытии соединения.
	//Если результат вызова recv() меньше 0, то выводится сообщение об ошибке, которая вызвала неудачу при вызове recv().
	//Таким образом, данный код осуществляет прием данных с использованием сокета ConnectSocket и выводит
	//информацию о принятых данных или об ошибках при их приеме.

	closesocket(ConnectSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;
	//то же самое что и на сервеной части, закрытие сокета, освобождение памяти

}
