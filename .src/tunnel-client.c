#include <stdio.h>                                                                                                                                         
#include <stdlib.h>                                                                                                                                        
#include <string.h>                                                                                                                                        
#include <sys/socket.h>                                                                                                                                    
#include <arpa/inet.h>                                                                                                                                     
#include <netinet/in.h>                                                                                                                                    
#include <unistd.h>                                                                                                                                        
#include <fcntl.h>                                                                                                                                         
                                                                                                                                                           
#define EOCR "--EOCR--"                                                                                                                                    
                                                                                                                                                           
#define BUFFER_SIZE 1024                                                                                                                                   
#define MAX_PATH_SIZE 1024                                                                                                                                 
                                                                                                                                                           
char command_buffer[BUFFER_SIZE] = {0};                                                                                                                    
char buffer[BUFFER_SIZE] = {0};                                                                                                                            
char current_path[BUFFER_SIZE * 2] = {0};                                                                                                                  
char command[BUFFER_SIZE * 2] = {0};                                                                                                                       
char outputs_path[BUFFER_SIZE + 256] = {0};                                                                                                                
char logs_path[BUFFER_SIZE + 256] = {0};                                                                                                                   
char ack = '.';                                                                                                                                            
                                                                                                                                                           
int server_sockfd;                                                                                                                                         
struct sockaddr_in address;                                                                                                                                
                                                                                                                                                           
void error_and_exit(const char* error){                                                                                                                    
        fprintf(stderr, "ERROR : %s\n", error);                                                                                                            
        exit(1);                                                                                                                                           
}                                                                                                                                                          
                                                                                                                                                           
// Properly track path changes                                                                                                                             
void track_path_changes(const char* command){                                                                                                              

        char dir[BUFFER_SIZE] = {0};                                                                                                                       
        int dir_ind = 0;                                                                                                                                   
        for(int i = 1; i < strlen(command); i++){                                                                                                          
                if(command[i-1] == 'c' && command[i] == 'd'){                                                                                              
                        if(i < strlen(command) - 1 && command[i+1] == ' '){                                                                                

                                while(command[++i] == ' ');                                                                                                
                                while(i < strlen(command) && command[i] != ' ' && command[i] != '\n') dir[dir_ind++] = command[i++];                       

                                chdir(dir);                                                                                                                
                                bzero(dir, BUFFER_SIZE);                                                                                                   
                                dir_ind = 0;                                                                                                               
                        }                                                                                                                                  
                }                                                                                                                                          
        }                                                                                                                                                  
        return;                                                                                                                                            
}                                                                                                                                                          
                                                                                                                                                           
void receive_tunneller(const int PORT){                                                                                                                    
                                                                                                                                                           
        if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){                                                                                         
                error_and_exit("Could not create socket to accept connections");                                                                           
        }                                                                                                                                                  
                                                                                                                                                           
        address.sin_port = htons(PORT);                                                                                                                    
        address.sin_family = AF_INET;                                                                                                                      
        address.sin_addr.s_addr = INADDR_ANY;                                                                                                              
                                                                                                                                                           
        // Bind                                                                                                                                            
        if(bind(server_sockfd, (struct sockaddr*)(&address), sizeof(address)) < 0){                                                                        
                error_and_exit("Bind Error, PORT already taken\n");                                                                                        
        }                                                                                                                                                  
                                                                                                                                                           
        // Listen
        if(listen(server_sockfd, 5) < 0){                                                                                                                  
                error_and_exit("Listen Error");                                                                                                            
        }                                                                                                                                                  
                                                                                                                                                           
        int client_sockfd;                                                                                                                                 
        socklen_t address_len = sizeof(address);                                                                                                           
                                                                                                                                                           
        if((client_sockfd = accept(server_sockfd, (struct sockaddr*)(&address), &address_len)) < 0){                                                       
                error_and_exit("Couldn't create socket to communicate with tunneller");                                                                    
        }                                                                                                                                                  

                                                                                                                                                           
        chdir(getenv("HOME"));                                                                                                                             
        getcwd(buffer, BUFFER_SIZE);                                                                                                                       
        sprintf(outputs_path, "%s/out.txt", buffer);                                                                                                       
        sprintf(logs_path, "%s/logs.txt", buffer);                                                                                                         

        FILE* logs_fptr = fopen(logs_path, "w");                                                                                                           
        if(!logs_fptr){                                                                                                                                    
                error_and_exit("Something went wrong!! Unable to perform logging");                                                                        
        }                                                                                                                                                  


        recv(client_sockfd, command_buffer, BUFFER_SIZE, 0);                                                                                               
        fprintf(logs_fptr, "%s", command_buffer);                                                                                                          
                                                                                                                                                           
        while(1){                                                                                                                                          
                                                                                                                                                           
                bzero(current_path, MAX_PATH_SIZE);                                                                                                        
                bzero(command_buffer, BUFFER_SIZE);                                                                                                        
                                                                                                                                                           
                getcwd(current_path, MAX_PATH_SIZE);                                                                                                       
                send(client_sockfd, current_path, strlen(current_path), 0);                                                                                
                                                                                                                                                           
                recv(client_sockfd, command_buffer, BUFFER_SIZE, 0);                                                                                       
                                                                                                                                                           
                if(strncmp(command_buffer, "exit\n", 5) == 0){                                                                                             
                        break;                                                                                                                             
                }                                                                                                                                          
                                                                                                                                                           
                track_path_changes(command_buffer);                                                                                                        
                                                                                                                                                           
                command_buffer[strlen(command_buffer) - 1] = '\0';                                                                                         
                sprintf(command, "%s | cat > %s", command_buffer, outputs_path);                                                                           
                                                                                                                                                           
                                                                                                                                                           
                system(command);                                                                                                                           
                                                                                                                                                           
                FILE* fptr = fopen(outputs_path, "r");                                                                                                     

                while(1){                                                                                                                                  
                        bzero(buffer, BUFFER_SIZE);                                                                                                        
                        size_t blocks_read = fread(buffer, BUFFER_SIZE, 1, fptr);                                                                          
                        send(client_sockfd, buffer, BUFFER_SIZE, 0);                                                                                       
                        recv(client_sockfd, &ack, 1, 0);                                                                                                   
                        if(!blocks_read && feof(fptr) != 0) break;                                                                                         
                }
                fprintf(logs_fptr, "%s\n", command_buffer);                                                                                                
                                                                                                                                                           
                fclose(fptr);                                                                                                                              
                                                                                                                                                           
                // EOCR -> End of Command Result                                                                                                           
                bzero(buffer, BUFFER_SIZE);                                                                                                                
                strcpy(buffer, EOCR);                                                                                                                      
                send(client_sockfd, buffer, strlen(buffer), 0);                                                                                            
                //printf(",\tSent %s\n", buffer);                                                                                                          
                recv(client_sockfd, &ack, 1, 0);                                                                                                           
                                                                                                                                                           
        }                                                                                                                                                  
                                                                                                                                                           
        fclose(logs_fptr);                                                                                                                                 
        close(client_sockfd);                                                                                                                              
        close(server_sockfd);                                                                                                                              
                                                                                                                                                           
}                                                                                                                                                          
                                                                                                                                                           
                                                                                                                                                           
int main(int argc, char* argv[]){                                                                                                                          
                                                                                                                                                           
        if(argc < 2){                                                                                                                                      
                error_and_exit("Please provide the PORT to which the tunneller can connect to");                                                           
        }                                                                                                                                                  
                                                                                                                                                           
        const int PORT = atoi(argv[1]);                                                                                                                    
                                                                                                                                                           
        receive_tunneller(PORT);                                                                                                                           
                                                                                                                                                           
        return 0;                                                                                                                                          
}  
