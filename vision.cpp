#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>

struct cln {
    int cfd;
    struct sockaddr_in caddr;
};

fd_set mask;
fd_set wmask;
fd_set rmask;
int fdmax, fda, rc;
char buf [100];
struct timeval timeout;
bool if_data_actually_read = false, last_line = false;
int actual_unique_id = 1;

size_t my_itoa( char *s, unsigned int n )
{
    const unsigned base = 10; 
    unsigned digit = n % base;
    size_t i = 0;

    if ( n /= base ) i += my_itoa( s, n );
	
    s[i++] = digit + '0';

    return i;
} 

int states_of_clients[100][2];
int clients_number = 0;

int get_state_of_client(int id){
	int i = 0;
	for (i = 0; i < clients_number; i++){
		if(states_of_clients[i][0] == id)
			return states_of_clients[i][1];
	}
	return -1;
}

void set_state_of_client(int id, int new_state){
	int i = 0;
	for(i = 0; i < clients_number; i++){
		if(states_of_clients[i][0] == id){
			states_of_clients[i][1] = new_state;
			break;
		}
	}	
}

void delete_client(int id){
	int i = 0;
	bool found = false;
	for(i = 0; i < clients_number; i++){
		if(states_of_clients[i][0] == id && i != clients_number - 1){
			found = true;
		}
		if(found && i < clients_number - 1){
			states_of_clients[i][0] = states_of_clients[i + 1][0];
			states_of_clients[i][1] = states_of_clients[i + 1][0];
		}
	}
	clients_number--;
}

int main(int argc, char *argv[]) {
	char* bufff;
	int signs_count = 500, actual_signs_count = 0, one_time_len = 0;
    socklen_t slt;
    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(1234);
    s_addr.sin_addr.s_addr = INADDR_ANY;

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int on = 1;

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

    bind(fd, (struct sockaddr*) & s_addr,sizeof(s_addr));
    listen(fd, 15);
	
    FD_ZERO(&mask);
    FD_ZERO(&wmask);
    FD_ZERO(&rmask);
    fdmax = fd;
    int client_fd = -1, actual_client = -1;
   
    while(1) {
        struct cln* c = (struct cln*) malloc(sizeof(struct cln));
	   	FD_SET(fd, &rmask);
		wmask = mask;
		timeout.tv_sec = 5 * 60;
		timeout.tv_usec = 0;
		rc = select(fdmax + 1, &rmask, &wmask, (fd_set*) 0, &timeout);
		if(rc == 0){
			printf("Timeout.\n");
			continue;
		}
		fda = rc;
		//accepting new client

		if(FD_ISSET(fd, &rmask)) {
			fda -= 1;

			slt = sizeof(c->caddr);

			c->cfd = accept(fd, (struct sockaddr*)&c->caddr, &slt);
			printf("new connection from: %s\n", inet_ntoa((struct in_addr)c->caddr.sin_addr));

			if(c->cfd > fdmax) fdmax = c->cfd;
			
			states_of_clients[clients_number][0] = c->cfd;
			states_of_clients[clients_number][1] = 0;
			clients_number++;
			
			FD_SET(c->cfd, &rmask);
		}

		for(int i = fd + 1; i <= fdmax && fda > 0; i++){
			if(FD_ISSET(i, &rmask)){
			
				//read type of client
				if(get_state_of_client(i) == 0){
					char* type = new char[3];
					read(i, type, 3);
					printf("cl    %s\n", type);
					//client that waits for data
					if (type[0] == 's'){
					
						//if there is new receiving client open the old one is replaced
						if (client_fd != -1){
							delete_client(client_fd);
							close(client_fd);
						}
						
						client_fd = i;
						FD_CLR(i, &rmask);
						set_state_of_client(i, 1);
					}
					
					//client needs unique id
					if (type[0] == 'i'){
						printf("Id przydzielone\n");
						FD_CLR(i, &rmask);
						FD_SET(i, &wmask);
						set_state_of_client(i, 1);
					}
					
					//client has unique id
					if(type[0] == 'c'){
						set_state_of_client(i, 2);
					}
					
				}
				
				//client waits for turn to send encoded image and send unique id or wants to exit
				else if(get_state_of_client(i) == 2 && client_fd != i && !if_data_actually_read && !last_line && client_fd != -1) {
					
					
					bufff = new char[4];
					read(i, bufff, 4);
					printf("READ ID %s\n", bufff);
					
					//clients wants to exit
					if(bufff[0] == 'e'){
						delete_client(i);
						set_state_of_client(client_fd, 5);
						FD_SET(client_fd, &wmask);
						FD_CLR(i, &rmask);
						close(i);
						continue;
					}
					
					if_data_actually_read = true;
					actual_client = i;
					
					//client can send size of encoded image
					set_state_of_client(i,3);
					
					FD_SET(client_fd, &wmask);
					FD_CLR(i, &rmask);
				}
				
				//read amount of data
				else if(get_state_of_client(i) == 3 && client_fd != i && actual_client == i && client_fd != -1){
					printf("READ\n");
					bufff = new char[12];
				 	read(i, bufff, 12);
					signs_count = atoi(bufff);

					//client can send encoded image
					set_state_of_client(i, 4);
					
					FD_SET(client_fd, &wmask);
					FD_CLR(i, &rmask);
				}
				

                else if(get_state_of_client(i) == 4 && client_fd != i && actual_client == i && client_fd != -1){

					bufff = new char[5000];

                    one_time_len = read(i, bufff, 5000);
                    actual_signs_count += one_time_len;

                    if(actual_signs_count >= signs_count){
                        set_state_of_client(i, 2);
						last_line = true;
                        if_data_actually_read = false;
                        
                        signs_count = 0;
                        actual_signs_count = 0;
                       
                    	FD_SET(client_fd, &wmask);
                    	FD_CLR(i, &rmask);
                        break;
                    }
					FD_CLR(i, &rmask);
                    FD_SET(client_fd, &wmask);

				}
			}
		}
		if(client_fd != -1 && FD_ISSET(client_fd, &wmask)) {
		    if(if_data_actually_read || last_line) {
		    
		    	
                
                //send unique id of client that want to send encoded image
                if(get_state_of_client(client_fd) == 1) {
                	write(client_fd, bufff, 4);
					set_state_of_client(client_fd, 2);
					FD_CLR(client_fd, &wmask);
					FD_SET(actual_client, &rmask);
                }
                
                //send amount of data that will be sent
                else if (get_state_of_client(client_fd) == 2) {
                    write(client_fd, bufff, 12);
                    set_state_of_client(client_fd, 4);
                    FD_CLR(client_fd, &wmask);
                    FD_SET(actual_client, &rmask);
                }

				//send fragment of base64 image format
                else if (get_state_of_client(client_fd) == 4) {
                    write(client_fd, bufff, one_time_len);
                    FD_CLR(client_fd, &wmask);
                    FD_SET(actual_client, &rmask);
                    if(last_line){
                        actual_client = -1;
                    	set_state_of_client(client_fd, 1);
                    	last_line = false;
                    }
                }
            } 
		}
		
		if(get_state_of_client(client_fd) == 5 && FD_ISSET(client_fd, &wmask)) {
        	write(client_fd, bufff, 4);
			set_state_of_client(client_fd, 1);
			FD_CLR(client_fd, &wmask);
        }
		
		for(int i = fd + 1; i <= fdmax && fda > 0; i++){

			if(client_fd != i && FD_ISSET(i, &wmask)){
			
				//send to client it's unique id
				printf("Response with id send\n");
				char unique_id[2];
				my_itoa(unique_id, actual_unique_id);
				actual_unique_id++;
				write(i, unique_id, 2);
				write(i, "\n", 2);
				set_state_of_client(i, 2);
				FD_CLR(i, &wmask);
				FD_SET(i, &rmask);
			}
		}
    	
    }

    close(fd);
    return EXIT_SUCCESS;
}
