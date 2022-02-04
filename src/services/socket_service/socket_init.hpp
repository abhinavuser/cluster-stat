#include <iostream>
#include <fstream>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include <vector>
#include <string>
#include <mutex>
#include <thread>

#include <json.hpp>

#define PORT 8080 

using namespace std;
using nlohmann::json;

namespace socket_service{
    std::vector<std::string> json_buf;
    std::vector<json> json_data;
    
    /* host data */
    std::vector<std::string> host_id, ip_addresses;

    /* buffers for parsed json data */
    std::vector<float> cpu_buf;
    std::vector<int> mem_buf;
    std::vector<float> mem_percent_buf;
    std::vector<float> temp_buf;
    std::vector<bool> brownout_buf;

    std::vector<std::vector<std::string>> config_init(std::string config_path){
        std::vector<std::vector<std::string>> config_template;  
        std::ifstream config_handler;

        config_handler.open(config_path);

        if(config_handler.is_open()){
            while (config_handler.good()){
                std::string tmp;
                std::getline(config_handler, tmp, ' ');   
                host_id.push_back(tmp);

                tmp = "";
                std::getline(config_handler, tmp);   
                ip_addresses.push_back(tmp);
            }     

            config_template.push_back(host_id);
            config_template.push_back(ip_addresses);

        }
        else{
            cout << "Error! could not open config file\n";
        }

        return config_template;
    }

    void socket_task(int id, std::string ip_address){
        int sock = 0;
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        std::mutex m;

        const char* address = ip_address.c_str();

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            cout << "socket creation error\nsocket for " << address << " could not be created\n";
            exit(1);
        }

        if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0) 
        {
            cout << "address: " << address << " is invalid\n";
            exit(1);
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            cout << "could not connect to host: " << address << "\n";
            exit(1);
        }

        while(1){
            char buf[128] = {0};
            usleep(1000000);
            int read_status = read(sock, buf, 128);

            m.lock();
            json_buf[id] = buf;
            m.unlock();
        }
    }

    std::vector<std::unique_ptr<std::thread>> socket_thread_init(std::vector<std::vector<std::string>> config){

        cout << "detected " << config[1].size() << " targets\n";

        std::vector<std::unique_ptr<std::thread>> socket_threads;
        
        for(int i=0; i<config[1].size(); i++){
            cout << "spawning thread no. " << i << endl;
            json data;
            json_buf.push_back("");
            json_data.push_back(data);

            /* empty initialization */
            cpu_buf.push_back(0.0);
            mem_buf.push_back(0);
            mem_percent_buf.push_back(0.0);
            temp_buf.push_back(0.0);
            brownout_buf.push_back(false);

            socket_threads.emplace_back(new std::thread(socket_task, i, config[1][i]));
        }

        usleep(5*1000*1000); // 5s delay for thread syncronization

        return socket_threads;
    }

    void socket_init(){
        std::mutex m;
        json data;

        std::vector<std::vector<std::string>> config = config_init("config.txt");
        
        std::vector<std::unique_ptr<std::thread>> threads = socket_thread_init(config);

        while(1){
            for(int j=0; j<config[1].size(); j++){
                m.lock();

                if(json::accept(json_buf[j])){
                    data = json::parse(json_buf[j]);
                    /* writing buf data */
                    cpu_buf[j] = data["cpu_usage"];
                    mem_buf[j] = data["memory_usage"];
                    mem_percent_buf[j] = data["memory_percent"];
                    temp_buf[j] = data["temp"];
                    brownout_buf[j] = false;

                    json_data[j] = data;
                }

                else{
                    brownout_buf[j] = true; 
                }
                m.unlock();
            }

            usleep(1000000);
        }
    }
}