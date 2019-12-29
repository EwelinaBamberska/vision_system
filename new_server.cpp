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
	int state = 0;
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

int main(int argc, char *argv[]) {
	char* bufff;
	int signs_count = 500, actual_signs_count = 0, one_time_len = 0;
    socklen_t slt;
    struct sockaddr_in s_addr;
    pthread_t tid;

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
			printf("new connection from: %s\n", inet_ntoa((struct 			in_addr)c->caddr.sin_addr));

			if(c->cfd > fdmax) fdmax = c->cfd;
			states_of_clients[clients_number][0] = c->cfd;
			states_of_clients[clients_number][1] = 0;
			clients_number++;
			FD_SET(c->cfd, &rmask);
		}

		for(int i = fd + 1; i <= fdmax && fda > 0; i++){
			if(FD_ISSET(i, &rmask)){
			    if(actual_signs_count!= 0 )
			    printf("STATE %d\n", get_state_of_client(i));
				//read type of client
				if(get_state_of_client(i) == 0){
					bufff = new char[3];
					read(i, bufff, 3);
					printf("%s\n", bufff);
					if (bufff[0] == 's'){
						client_fd = i;
						FD_CLR(i, &rmask);
					}
					set_state_of_client(i, 1);
				}
				
				//read amount of data
				else if(get_state_of_client(i) == 1 && client_fd != i && !if_data_actually_read && !last_line){
					if_data_actually_read = true;
					actual_client = i;
					bufff = new char[12];
				 	read(i, bufff, 12);
				 	int  j = 0;
					signs_count = atoi(bufff);

					set_state_of_client(i, 4);
					FD_SET(client_fd, &wmask);
					FD_CLR(i, &rmask);
				}

                else if(get_state_of_client(i) == 4 && client_fd != i && actual_client == i){
					bufff = new char[5000];

                    one_time_len = read(i, bufff, 5000);
                    actual_signs_count += one_time_len;
                    if(actual_signs_count != 0)
                    printf(" %d         %d\n", actual_signs_count, signs_count);
                    if(actual_signs_count >= signs_count){

                        set_state_of_client(i, 5);
		
                        actual_client = -1;
						last_line = true;
						
                        if_data_actually_read = false;
                        signs_count = 0;
                        actual_signs_count = 0;
                        
                    	FD_SET(client_fd, &wmask);
                        close(i);
                        break;
                    }
                    FD_CLR(i, &rmask);
                    FD_SET(client_fd, &wmask);

				}
			}
		}
		if(FD_ISSET(client_fd, &wmask)) {
		    if(if_data_actually_read || last_line) {
		    	if(last_line)
		    		printf("KSDFSGER\n");
                if (get_state_of_client(client_fd) == 1) {
                    write(client_fd, bufff, 12);
                    char converted_ident[10];
                    int s = my_itoa(converted_ident, actual_client);
                    converted_ident[9] = '\n';

                    write(client_fd, converted_ident, 10);
                    set_state_of_client(client_fd, 4);
                    FD_CLR(client_fd, &wmask);
                    FD_SET(actual_client, &rmask);
                }
                //printf("write\n");
                if (get_state_of_client(client_fd) == 4) {
                    write(client_fd, bufff, one_time_len);
                    last_line = false;
                    FD_CLR(client_fd, &wmask);
                    if(!last_line)
                    	FD_SET(actual_client, &rmask);
                    if(last_line){
                    	set_state_of_client(client_fd, 1);
                    	printf("END\n");
                    }
                }
            }
			}
    	
    }

    close(fd);

    return EXIT_SUCCESS;
}
