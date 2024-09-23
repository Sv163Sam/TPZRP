#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/wait.h>

// Функция для чтения массива чисел из сокета
std::vector<int> readIntVector(int sockfd) {
    std::vector<int> data;
    int len;
    recv(sockfd, &len, sizeof(int), 0); // Получить размер массива

    data.resize(len);
    recv(sockfd, &data[0], len * sizeof(int), 0); // Получить данные

    return data;
}

// Функция для запуска Python-скрипта
int runPythonScript(const std::string& scriptPath, const std::vector<int>& data) {
    std::stringstream command;
    command << "python3 " << scriptPath << " ";
    for (int num : data) {
        command << num << " ";
    }

    return system(command.str().c_str());
}

int main() {
    int sockfd, clientfd;
    struct sockaddr_in server_addr, client_addr{};
    socklen_t client_addr_len;

    // Создать сокет
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Настроить адрес
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Слушать все интерфейсы
    server_addr.sin_port = htons(8080); // Порт 8080

    // Привязать сокет к адресу
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return 1;
    }

    // Слушать соединения
    if (listen(sockfd, 5) == -1) {
        perror("listen");
        return 1;
    }

    std::cout << "Сервер запущен на порту 8080\n";

    while (true) {
        // Принять соединение
        client_addr_len = sizeof(client_addr);
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (clientfd == -1) {
            perror("accept");
            continue;
        }

        // Запустить дочерний процесс
        pid_t pid = fork();
        if (pid == 0) {
            // Дочерний процесс
            close(sockfd); // Закрыть сокет сервера в дочернем процессе

            // Получить данные
            std::vector<int> data = readIntVector(clientfd);

            // Запустить Python-скрипт
            if (runPythonScript("server_script.py", data) == -1) {
                perror("system");
                exit(1);
            }

            // Отправить результат клиенту
            // (В этом случае, я предполагаю, что server_script.py сгенерирует файл "result.png")
            std::ifstream resultFile("result.png", std::ios::binary);
            if (!resultFile.is_open()) {
                perror("ifstream");
                exit(1);
            }
            std::string fileContent((std::istreambuf_iterator<char>(resultFile)), std::istreambuf_iterator<char>());
            int len = fileContent.length();
            send(clientfd, &len, sizeof(int), 0);
            send(clientfd, fileContent.c_str(), len, 0);

            close(clientfd); // Закрыть сокет клиента
            exit(0); // Завершить дочерний процесс
        } else if (pid > 0) {
            // Родительский процесс
            close(clientfd); // Закрыть сокет клиента в родительском процессе
            wait(nullptr); // Ожидать завершения дочернего процесса
        } else {
            perror("fork");
            continue;
        }
    }

    close(sockfd);
    return 0;
}


