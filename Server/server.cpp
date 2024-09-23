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
std::vector<int> read_vector(int sockfd) {
    std::vector<int> data;
    int len;
    recv(sockfd, &len, sizeof(int), 0); // Получить размер массива

    data.resize(len);
    recv(sockfd, &data[0], len * sizeof(int), 0); // Получить данные

    return data;
}

// Функция для запуска Python-скрипта
int run_python_script(const std::string& scriptPath) {
    std::stringstream command;
    command << "python3 " << scriptPath;
    return system(command.str().c_str());
}

void write_to_file(const std::vector<int>& vec, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Ошибка открытия файла для записи" << std::endl;
        return;
    }

    for (const int& num : vec)
        outFile << num << std::endl;  // Запись каждого числа на новой строке

    outFile.close();
}

std::vector<int> read_from_file(const std::string& filename) {
    std::vector<int> vec;
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Ошибка открытия файла для чтения" << std::endl;
        return vec;
    }

    int num;
    while (inFile >> num)
        vec.push_back(num);

    inFile.close();
    return vec;
}

int main() {
    int sockfd, clientfd;
    struct sockaddr_in server_addr{}, client_addr{};
    std::string received_img_arr = "txt/";
    std::string restored_img = "img/";
    std::string script_py = ".py";
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
            std::vector<int> received_img = read_vector(clientfd);
            write_to_file(received_img, received_img_arr);

            // Запустить Python-скрипт
            if (run_python_script(script_py) == -1) {
                perror("system");
                exit(1);
            }

            std::ifstream resultFile(restored_img, std::ios::binary);
            if (!resultFile.is_open()) {
                perror("ifstream");
                exit(1);
            }
            std::string fileContent((std::istreambuf_iterator<char>(resultFile)), std::istreambuf_iterator<char>());
            size_t len = fileContent.length();
            send(clientfd, &len, sizeof(int), 0);
            send(clientfd, fileContent.c_str(), len, 0);

            close(clientfd); // Закрыть сокет клиента
            exit(0); // Завершить дочерний процесс
        }
        else if (pid > 0)
        {
            // Родительский процесс
            close(clientfd); // Закрыть сокет клиента в родительском процессе
            wait(nullptr); // Ожидать завершения дочернего процесса
        }
        else
        {
            perror("fork");
            break;
        }
    }

    close(sockfd);
    return 0;
}


