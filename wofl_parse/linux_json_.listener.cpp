/ simple_listener.cpp - NO external dependencies
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <iomanip>

void print_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") << "] ";
}

bool is_json_like(const std::string& data) {
    // Simple check - starts with { and ends with }
    return data.length() > 2 && data[0] == '{' && data.back() == '}';
}

int main() {
    std::cout << "🚀 FastPing Simple UDP Listener\n";
    std::cout << "===============================\n";
    std::cout << "📡 Listening on port 9876...\n\n";

    // Create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "❌ Socket creation failed\n";
        return 1;
    }

    // Allow address reuse
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9876);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "❌ Bind failed\n";
        close(sock);
        return 1;
    }

    char buffer[1024];
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int packet_count = 0;

    while (true) {
        int recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, 
                               (sockaddr*)&client_addr, &addr_len);
        
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            packet_count++;
            
            // Get client IP
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            
            // Print with timestamp and formatting
            print_timestamp();
            std::cout << "📦 PKT#" << packet_count << " FROM " << client_ip 
                     << ":" << ntohs(client_addr.sin_port) << std::endl;
            
            std::string data(buffer);
            
            if (is_json_like(data)) {
                std::cout << "     🟢 JSON: " << data << std::endl;
            } else {
                std::cout << "     📄 RAW:  " << data << std::endl;
            }
            
            std::cout << "     📏 SIZE: " << recv_len << " bytes" << std::endl;
            std::cout << "───────────────────────────────────\n";
        }
    }

    close(sock);
    return 0;
}
