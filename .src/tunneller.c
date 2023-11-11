#include <stdio.h>                                                                                                                                         
#include <stdlib.h>                                                                                                                                        
#include <string.h>                                                                                                                                        
#include <sys/socket.h>                                                                                                                                    
#include <arpa/inet.h>                                                                                                                                     
#include <netinet/in.h>                                                                                                                                    
#include <unistd.h>                                                                                                                                        
#include <fcntl.h>                                                                                                                                         
                                                                                                                                                           
#define BUFFER_SIZE 1024                                                                                                                                   
#define EOCR "--EOCR--"                                                                                                                                    
                                                                                                                                                           
char tunneller_id[BUFFER_SIZE] = {0};                                                                                                                      
char buffer[BUFFER_SIZE] = {0};                                                                                                                            
char response_buffer[BUFFER_SIZE] = {0};                                                                                                                   
char ack = '.';                                                                                                                                            
                                                                                                                                                           
int client_sockfd;                                                                                                                                         
struct sockaddr_in server_address;                                                                                                                         
                                                                                                                                                           
void error_and_exit(const char* error){                                                                                                                    
        fprintf(stderr, "ERROR : %s\nAborting...\n", error);                                                                                               
        exit(1);                                                                                                                                           
}                                                                                                                                                          
                                                                                                                                                           
void log_warnings(){                                                                                                                                       
        printf("WARNING : Avoid running commands that can harm the client's device\n");                                                                    
        printf("WARNING : Do not attempt using text editors like vim, ed or nano on the client's device\n");                                               
}                                                                                                                                                          
                                                                                                                                                           
                                                                                                                                                           
void tunnel_into(const int PORT){                                                                                                                          
        printf("Enter your username [Could be anything] : ");                                                                                              
        fgets(tunneller_id, BUFFER_SIZE, stdin);                                                                                                           
        if(strlen(tunneller_id)) tunneller_id[strlen(tunneller_id) - 1] = '\0';                                                                            
                                                                                                                                                           
        if((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){                                                                                         
                error_and_exit("Couldn't create socket for establishing connection");                                                                      
        }                                                                                                                                                  
                                                                                                                                                           
        server_address.sin_port = htons(PORT);                                                                                                             
        server_address.sin_family = AF_INET;                                                                                                               
        server_address.sin_addr.s_addr = inet_addr("127.0.0.1");                                                                                           
                                                                                                                                                           
        if(connect(client_sockfd, (struct sockaddr*)(&server_address), sizeof(server_address)) < 0){                                                       
                error_and_exit("Couldn't connect to the client");                                                                                          
        }                                                                                                                                                  
                                                                                                                                                           
        sprintf(buffer, "%s just received access to your session...\n", tunneller_id);                                                                     
        send(client_sockfd, buffer, strlen(buffer), 0);                                                                                                    

        printf("Successfully connected...\n");                                                                                                             

                                                                                                                                                           
        while(1){                                                                                                                                          
                // Get path                                                                                                                                
                bzero(response_buffer, BUFFER_SIZE);                                                                                                       
                recv(client_sockfd, response_buffer, BUFFER_SIZE, 0);                                                                                      
                printf("%s $ ", response_buffer);                                                                                                          
                                                                                                                                                           
                bzero(response_buffer, BUFFER_SIZE);                                                                                                       
                bzero(buffer, BUFFER_SIZE);                                                                                                                

                fgets(buffer, BUFFER_SIZE, stdin);                                                                                                         

                                                                                                                                                           
                send(client_sockfd, buffer, strlen(buffer), 0);                                                                                            

                if(strncmp(buffer, "exit\n", 5) == 0) break;                                                                                               



                while(1){                                                                                                                                  
                        bzero(response_buffer, BUFFER_SIZE);                                                                                               
                        recv(client_sockfd, response_buffer, BUFFER_SIZE, 0);                                                                              

                        send(client_sockfd, &ack, 1, 0);                                                                                                   
                        if(strncmp(response_buffer, EOCR, strlen(EOCR)) == 0) break;                                                                       

                        printf("%s", response_buffer);                                                                                                     
                }                                                                                                                                          

                //printf("Received --EOCR--\n");                                                                                                           
                                                                                                                                                           
        }                                                                                                                                                  
                                                                                                                                                           
        close(client_sockfd);                                                                                                                              
                                                                                                                                                           
}                                                                                                                                                          
                                                                                                                                                           
                                                                                                                                                           
int main(int argc, char* argv[]){                                                                                                                          
                                                                                                                                                           
        if(argc < 2){                                                                                                                                      
                error_and_exit("Missing PORT to connect to\n");                                                                                            
        }                                                                                                                                                  
                                                                                                                                                           
        if(strncmp(argv[1], "-h", 2) == 0){                                                                                                                
                // TODO : Handle -h flag                                                                                                                   
                printf("");                                                                                                                                
                return 0;                                                                                                                                  
        }                                                                                                                                                  
                                                                                                                                                           
        const int PORT = atoi(argv[1]);                                                                                                                    
                                                                                                                                                           
        tunnel_into(PORT);                                                                                                                                 
                                                                                                                                                           
                                                                                                                                                           
        return 0;                                                                                                                                          
                                                                                                                                                           
}                
