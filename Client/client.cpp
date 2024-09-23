#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

void send_to_server(int sockfd, const std::vector<int>& data) {
    size_t len = data.size();
    send(sockfd, &len, sizeof(int), 0);
    send(sockfd, &data[0], len * sizeof(int), 0);
}

int run_python_script(const std::string& script_path) {
    std::stringstream command;
    command << "python3 " << script_path;
    return system(command.str().c_str());
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
    int sockfd, width, height;
    int len = 0;
    std::string source_img = "/Users/vladimirskobcov/Desktop/Labs/TPZRP/Img/SWEETIE_EYES.PNG";
    std::string screw_img_arr = "/Users/vladimirskobcov/Desktop/Labs/TPZRP/Txt/SCREW_EYES.txt";
    std::string received_img = "/Users/vladimirskobcov/Desktop/Labs/TPZRP/Img/RECEIVED_RESTORED_EYES.PNG";
    std::string script_py = "/Users/vladimirskobcov/Desktop/Labs/TPZRP/Scripts/noise.py";

    struct sockaddr_in server_addr{};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        return 1;
    }

    std::vector<int> img_arr_after_noise;
    if (run_python_script(script_py) == -1) {
        perror("system");
        return 1;
    }
    else
        img_arr_after_noise = read_from_file(screw_img_arr);

    send_to_server(sockfd, img_arr_after_noise);

    recv(sockfd, &len, sizeof(int), 0);
    std::vector<char> result_img_arr(len);
    recv(sockfd, &result_img_arr[0], len, 0);

    std::ofstream result_file(received_img, std::ios::binary);
    if (!result_file.is_open()) {
        perror("ofstream");
        return 1;
    }
    result_file.write(&result_img_arr[0], (long)result_img_arr.size());

    close(sockfd);
    getchar();
    return 0;
}
