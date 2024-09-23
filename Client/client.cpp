#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

// Функция для преобразования изображения в массив чисел
std::vector<int> imageToArray(const std::string& imagePath) {
    std::ifstream imageFile(imagePath, std::ios::binary);
    if (!imageFile.is_open()) {
        perror("ifstream");
        exit(1);
    }
    std::string fileContent((std::istreambuf_iterator<char>(imageFile)), std::istreambuf_iterator<char>());
    std::vector<int> data(fileContent.begin(), fileContent.end());
    return data;
}

// Функция для отправки массива чисел на сервер
void sendIntVector(int sockfd, const std::vector<int>& data) {
    int len = data.size();
    send(sockfd, &len, sizeof(int), 0);
    send(sockfd, &data[0], len * sizeof(int), 0);
}

// Функция для запуска Python-скрипта
int runPythonScript(const std::string& scriptPath, const std::string& imagePath) {
    std::stringstream command;
    command << "python3 " << scriptPath << " " << imagePath;
    return system(command.str().c_str());
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr{};

    // Создать сокет
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Настроить адрес
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Адрес сервера
    server_addr.sin_port = htons(8080); // Порт сервера

    // Подключиться к серверу
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        return 1;
    }

    // Чтение изображения
    std::string imagePath = "image.png"; // Путь к вашему изображению
    std::vector<int> imageData = imageToArray(imagePath);

    // Запустить Python-скрипт
    if (runPythonScript("client_script.py", imagePath) == -1) {
        perror("system");
        return 1;
    }

    // Отправить данные на сервер
    sendIntVector(sockfd, imageData);

    // Получить результат от сервера
    int len;
    recv(sockfd, &len, sizeof(int), 0); // Получить размер изображения
    std::vector<char> resultData(len);
    recv(sockfd, &resultData[0], len, 0); // Получить изображение

    // Сохранить полученное изображение
    std::ofstream resultFile("received_image.png", std::ios::binary);
    if (!resultFile.is_open()) {
        perror("ofstream");
        return 1;
    }
    resultFile.write(&resultData[0], resultData.size());

    close(sockfd);
    return 0;
}
