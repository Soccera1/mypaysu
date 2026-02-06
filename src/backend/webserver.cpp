#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <thread>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1917
#define RED "\033[31m"
#define GOLD "\033[33m"
#define RESET "\033[0m"

struct User {
    std::string username;
    std::string password;
    double balance;
};

class SovietLedger {
private:
    std::map<std::string, User> users;
    std::mutex mtx;

public:
    SovietLedger() {
        users["lenin"] = {"lenin", "revolution", 19171917.00};
    }

    bool registerUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(mtx);
        if (users.find(username) != users.end()) {
            return false;
        }
        users[username] = {username, password, 100.0};
        std::cout << RED << "[LEDGER] New comrade registered: " << GOLD << username << RESET << std::endl;
        return true;
    }

    bool loginUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = users.find(username);
        if (it != users.end() && it->second.password == password) {
            std::cout << RED << "[LEDGER] Comrade authenticated: " << GOLD << username << RESET << std::endl;
            return true;
        }
        return false;
    }

    double getBalance(const std::string& username) {
        std::lock_guard<std::mutex> lock(mtx);
        if (users.find(username) != users.end()) {
            return users[username].balance;
        }
        return -1.0;
    }

    bool transfer(const std::string& sender, const std::string& receiver, double amount) {
        std::lock_guard<std::mutex> lock(mtx);
        if (amount <= 0) return false;
        
        auto itSender = users.find(sender);
        auto itReceiver = users.find(receiver);

        if (itSender != users.end() && itReceiver != users.end()) {
            if (itSender->second.balance >= amount) {
                itSender->second.balance -= amount;
                itReceiver->second.balance += amount;
                std::cout << RED << "[LEDGER] Wealth redistributed from " << GOLD << sender 
                          << RED << " to " << GOLD << receiver << " (" << amount << " SXMR)" << RESET << std::endl;
                return true;
            }
        }
        return false;
    }
};

SovietLedger ledger;

std::string url_decode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            std::istringstream is(str.substr(i + 1, 2));
            if (is >> std::hex >> value) {
                result += static_cast<char>(value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::map<std::string, std::string> parse_post_data(const std::string& data) {
    std::map<std::string, std::string> params;
    std::istringstream stream(data);
    std::string pair;
    
    while (std::getline(stream, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = url_decode(pair.substr(0, pos));
            std::string value = url_decode(pair.substr(pos + 1));
            params[key] = value;
        }
    }
    return params;
}

std::string create_json_response(bool success, const std::string& message, double amount = 0.0) {
    std::ostringstream json;
    json << "{\"success\":" << (success ? "true" : "false")
         << ",\"message\":\"" << message << "\"";
    if (amount >= 0.0) {
        json << ",\"balance\":" << amount;
    }
    json << "}";
    return json.str();
}

std::string serve_file(const std::string& path) {
    if (path == "/" || path == "/index.html") {
        std::string content = "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\"><title>MyPaySU - Web Edition</title></head><body><h1>MyPaySU - The People's Payment Platform</h1><p>Please create an index.html file in the public directory.</p></body></html>";
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: text/html; charset=utf-8\r\n"
                 << "Content-Length: " << content.length() << "\r\n"
                 << "Connection: close\r\n"
                 << "\r\n"
                 << content;
        return response.str();
    }
    return "";
}

void handle_http_request(int client_socket) {
    char buffer[4096] = {0};
    ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }

    std::string request(buffer, bytes_read);
    std::istringstream request_stream(request);
    std::string method, path, version;
    request_stream >> method >> path >> version;

    std::string response;

    if (method == "GET" && path == "/") {
        std::string html_content;
        std::ifstream html_file("public/index.html");
        if (html_file.is_open()) {
            std::ostringstream ss;
            ss << html_file.rdbuf();
            html_content = ss.str();
            html_file.close();
        } else {
            html_content = serve_file(path);
            send(client_socket, html_content.c_str(), html_content.length(), 0);
            close(client_socket);
            return;
        }

        std::ostringstream resp;
        resp << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: text/html; charset=utf-8\r\n"
             << "Content-Length: " << html_content.length() << "\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << html_content;
        response = resp.str();
    }
    else if (method == "GET" && path.substr(0, 8) == "/public/") {
        std::string file_path = path.substr(1);
        std::ifstream file(file_path, std::ios::binary);
        
        if (file.is_open()) {
            std::ostringstream ss;
            ss << file.rdbuf();
            std::string content = ss.str();
            file.close();

            std::string content_type = "text/plain";
            if (path.find(".css") != std::string::npos) content_type = "text/css";
            else if (path.find(".js") != std::string::npos) content_type = "application/javascript";
            else if (path.find(".html") != std::string::npos) content_type = "text/html";

            std::ostringstream resp;
            resp << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: " << content_type << "; charset=utf-8\r\n"
                 << "Content-Length: " << content.length() << "\r\n"
                 << "Connection: close\r\n"
                 << "\r\n"
                 << content;
            response = resp.str();
        } else {
            std::string not_found = "404 Not Found";
            std::ostringstream resp;
            resp << "HTTP/1.1 404 Not Found\r\n"
                 << "Content-Type: text/plain\r\n"
                 << "Content-Length: " << not_found.length() << "\r\n"
                 << "Connection: close\r\n"
                 << "\r\n"
                 << not_found;
            response = resp.str();
        }
    }
    else if (method == "POST") {
        size_t body_start = request.find("\r\n\r\n");
        std::string body = (body_start != std::string::npos) ? request.substr(body_start + 4) : "";
        auto params = parse_post_data(body);

        std::string json_response;

        if (path == "/api/register") {
            std::string username = params["username"];
            std::string password = params["password"];
            bool success = ledger.registerUser(username, password);
            json_response = create_json_response(success, 
                success ? "Welcome to the Party, Comrade " + username + "!" : "Identity already claimed by another worker.");
        }
        else if (path == "/api/login") {
            std::string username = params["username"];
            std::string password = params["password"];
            bool success = ledger.loginUser(username, password);
            json_response = create_json_response(success,
                success ? "Access granted. 'No amount of political freedom will satisfy the hungry masses.' - V.I. Lenin" : "Invalid credentials. Are you a capitalist spy?");
        }
        else if (path == "/api/balance") {
            std::string username = params["username"];
            double balance = ledger.getBalance(username);
            json_response = create_json_response(balance >= 0, 
                balance >= 0 ? "Balance retrieved" : "User not found", balance);
        }
        else if (path == "/api/transfer") {
            std::string sender = params["username"];
            std::string receiver = params["target_user"];
            double amount = std::stod(params["amount"]);
            bool success = ledger.transfer(sender, receiver, amount);
            
            double new_balance = ledger.getBalance(sender);
            json_response = create_json_response(success,
                success ? "Resources successfully redistributed." : "Transfer failed. Insufficient funds or invalid comrade.", new_balance);
        }
        else {
            json_response = create_json_response(false, "Unknown API endpoint");
        }

        std::ostringstream resp;
        resp << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: application/json\r\n"
             << "Content-Length: " << json_response.length() << "\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << json_response;
        response = resp.str();
    }
    else {
        std::string not_found = "404 Not Found";
        std::ostringstream resp;
        resp << "HTTP/1.1 404 Not Found\r\n"
             << "Content-Type: text/plain\r\n"
             << "Content-Length: " << not_found.length() << "\r\n"
             << "Connection: close\r\n"
             << "\r\n"
             << not_found;
        response = resp.str();
    }

    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << RED << "=== MyPaySU Web Central Bureau Online ===" << RESET << std::endl;
    std::cout << GOLD << "Dedicated to the Great Leader, Vladimir Ilyich Lenin" << RESET << std::endl;
    std::cout << GOLD << "Web server listening on http://localhost:" << PORT << RESET << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        std::thread(handle_http_request, new_socket).detach();
    }

    return 0;
}
