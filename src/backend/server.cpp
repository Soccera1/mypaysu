#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../include/common.h"

// Color codes for server output
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
        // Create the Great Leader account
        users["lenin"] = {"lenin", "revolution", 1917.00};
    }

    bool registerUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(mtx);
        if (users.find(username) != users.end()) {
            return false; // User already exists
        }
        // Every citizen starts with a state subsidy
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

void handle_client(int client_socket) {
    SovietMessage msg;
    while (true) {
        ssize_t bytes_read = recv(client_socket, &msg, sizeof(msg), 0);
        if (bytes_read <= 0) {
            break;
        }

        SovietMessage response;
        std::memset(&response, 0, sizeof(response));
        response.type = RES_ERROR; // Default to error

        switch (msg.type) {
            case REQ_REGISTER:
                if (ledger.registerUser(msg.username, msg.password)) {
                    response.type = RES_OK;
                    snprintf(response.message, sizeof(response.message), "Welcome to the Party, Comrade %s!", msg.username);
                } else {
                    snprintf(response.message, sizeof(response.message), "Identity already claimed by another worker.");
                }
                break;
            case REQ_LOGIN:
                if (ledger.loginUser(msg.username, msg.password)) {
                    response.type = RES_OK;
                    snprintf(response.message, sizeof(response.message), "Access granted. Glory to the revolution!");
                } else {
                    snprintf(response.message, sizeof(response.message), "Invalid credentials. Are you a capitalist spy?");
                }
                break;
            case REQ_BALANCE:
                {
                    double bal = ledger.getBalance(msg.username);
                    if (bal >= 0) {
                        response.type = RES_OK;
                        response.amount = bal;
                        snprintf(response.message, sizeof(response.message), "Your contribution: %.2f SXMR", bal);
                    } else {
                        snprintf(response.message, sizeof(response.message), "User not found.");
                    }
                }
                break;
            case REQ_TRANSFER:
                if (ledger.transfer(msg.username, msg.target_user, msg.amount)) {
                    response.type = RES_OK;
                    snprintf(response.message, sizeof(response.message), "Resources successfully redistributed.");
                } else {
                    snprintf(response.message, sizeof(response.message), "Transfer failed. Insufficient funds or invalid comrade.");
                }
                break;
            default:
                snprintf(response.message, sizeof(response.message), "Unknown directive.");
                break;
        }

        send(client_socket, &response, sizeof(response), 0);
    }
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

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << RED << "=== MyPaySU Central Bureau Online ===" << RESET << std::endl;
    std::cout << GOLD << "Listening on port " << PORT << " for loyal citizens..." << RESET << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        std::thread(handle_client, new_socket).detach();
    }

    return 0;
}
