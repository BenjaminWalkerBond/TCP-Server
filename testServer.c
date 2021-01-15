#include <stdio.h> 
#include<unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#include <sys/time.h>
#include <sys/stat.h>



#define MAX 80 
#define PORT 9678
#define SIZE 1024
#define SA struct sockaddr 
#define min(a,b) (((a) < (b)) ? (a) : (b))
#ifdef DEBUG
#define INITIAL_ALLOC 2
#else
#define INITIAL_ALLOC 512
#endif

char *
read_line(FILE *fin) {
    char *buffer;
    char *tmp;
    int read_chars = 0;
    int bufsize = INITIAL_ALLOC;
    char *line = malloc(bufsize);

    if ( !line ) {
        return NULL;
    }

    buffer = line;

    while ( fgets(buffer, bufsize - read_chars, fin) ) {
        read_chars = strlen(line);

        if ( line[read_chars - 1] == '\n' ) {
            line[read_chars - 1] = '\0';
            return line;
        }

        else {
            bufsize = 2 * bufsize;
            tmp = realloc(line, bufsize);
            if ( tmp ) {
                line = tmp;
                buffer = line + read_chars;
            }
            else {
                free(line);
                return NULL;
            }
        }
    }
    return NULL;
}

void deleteline(const char *filename,unsigned int linetodelete){
    FILE *file=fopen(filename,"r");
    FILE *filetemp=fopen("__tempfile__","w");

    char line[255];
    unsigned int countline=0;

    while(fgets(line,255,file)!=NULL){
        if(countline!=linetodelete)
            fputs(line,filetemp);
        countline++;
    }
    fclose(file);
    fclose(filetemp);
    rename("__tempfile__",filename);
}

void write_file(int sockfd){
    printf("in write file");
  int n;
  FILE *fp;
  char *filename = "log.txt";
  char buffer[SIZE];

  fp = fopen(filename, "w");
  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  return;
} 

int send_all(int sock, const void *buf, int len)
{
    const char *pbuf = (const char *) buf;

    while (len > 0)
    {
        int sent = send(sock, pbuf, len, 0);
        if (sent < 1)
        {
            // if the socket is non-blocking, then check
            // the socket error for WSAEWOULDBLOCK/EAGAIN
            // (depending on platform) and if true then
            // use select() to wait for a small period of
            // time to see if the socket becomes writable
            // again before failing the transfer...

            printf("Can't write to socket");
            return -1;
        }

        pbuf += sent;
        len -= sent;
    }

    return 0;
}

void str_server(int sock, char* fn) 
{ 
    char buf[0x1000]; 
    const char* filename = fn;
    struct stat s;

    if (stat(filename, &s) == -1)
    {
        printf("Can't get file info"); 
        return;
    }

    FILE *file = fopen(filename, "rb"); 
    if (!file)
    {
        printf("Can't open file for reading"); 
        return;
    }

    // if you need to handle files > 2GB,
    // be sure to use a 64bit integer, and
    // a host-to-network function that can
    // handle 64bit integers...
    long size = s.st_size;
    long tmp_size = htonl(size);
    if (send_all(sock, &tmp_size, sizeof(tmp_size)) == 0)
    {
        while (size > 0)
        { 
            int rval = fread(buf, 1, min(sizeof(buf), size), file); 
            if (rval < 1)
            {
                printf("Can't read from file");
                break;
            }

            if (send_all(sock, buf, rval) == -1)
                break;

            size -= rval;
        }
    }

    fclose(file);
} 
// Function designed for chat between client and server. 
void handleAction(int sockfd, int act, const char* agent) 
{ 
    char buff[MAX]; 
    int n; 
    char* agentCopy = malloc(100);
    strcpy(agentCopy, agent);
    int replaceFileLine;
    FILE *fp;
    
    printf("Agent currently connecting: %s", agentCopy);
    
    // infinite loop for chat 
    
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(sockfd, buff, sizeof(buff)); 


        

            int i;
            fp  = fopen ("log.txt", "a");
            
        // print buffer which contains the client contents 
        //printf("From client: %s\t To client : ", buff); 
        fprintf(fp, "%d: Recieved a %s action from agent %s\n", time(NULL),buff, agent);
        // client wants is either already active or wants to join
    if( act == 1  || (strcmp(buff, "#JOIN") == 0)){

        if( strcmp(buff, "#JOIN") == 0){
            
            if(act == 1){
                printf("$ALREADY MEMBER\n");
                strcpy(buff, "$ALREADY MEMBER\n");
                send_all(sockfd, buff, sizeof(buff));
                
                fprintf(fp, "%d: Responded to agent %s with $ALREADY MEMBER \n", time(NULL), agentCopy);
                fclose (fp);

                
                
            }
            if(act != 1){
                if(act == 0){
                    
                    int i;
                    FILE *file;
                        file  = fopen ("active.txt", "a");

                        printf("%s\n", agentCopy); 
                        fprintf(file,"%d :", time(NULL));
                        fprintf(file, "%s\n", agentCopy);
                        fclose (file);

                }
                strcpy(buff, "$OK\n");
                send_all(sockfd, buff, sizeof(buff));
                
                fprintf(fp, "%d: Responded to agent %s with $OK \n", time(NULL), agentCopy);
                fclose (fp);
            }
            close(sockfd);
            

        }
        else if( strcmp(buff, "#LEAVE") == 0){
            
            if(act == 1){

                    // read our file, compute times, send in buffer.
                FILE* fin; 
                FILE* fin2;
                char *line;
                char *line2;
                
                fin = fopen("active.txt", "r");
                fin2 = fopen("active1.txt", "w");

                if ( fin ) {
                    while ( line = read_line(fin) ) {
                        if ( strstr(line, agentCopy ) == NULL){

                            fprintf(fin2,"%s\n", line);
                            
                        }
                        free(line);
                    }
                }
                fclose(fin);
                fclose(fin2);
                remove("active.txt");
                rename("active1.txt","active.txt");


                fprintf(fp, "%d: Responded to agent %s with $OK \n", time(NULL), agentCopy);
                fclose (fp);
                strcpy(buff, "$OK\n");
                send_all(sockfd, buff, sizeof(buff));
                printf("$OK\n");
            }else {
                strcpy(buff, "$NOT MEMBER\n");
                send_all(sockfd, buff, sizeof(buff));
                printf("$NOT MEMBER\n");
                fprintf(fp, "%d: Responded to agent %s with $NOT MEMBER \n", time(NULL), agentCopy);
                fclose(fp);
            }
            close(sockfd);
        }
        else if( strcmp(buff, "#LIST") == 0){

            // read our file, compute times, send in buffer.
            FILE* fin; 
            FILE* fin2;
            char *line;
            char *line2;
            
            fin = fopen("active.txt", "r");
            fin2 = fopen("active1.txt", "w");

            if ( fin ) {
                while ( line = read_line(fin) ) {
                    //if ( strstr(line, agentCopy )){

                        char * pch;
                        int temp;
                        //printf ("Splitting string \"%s\" into tokens:\n",line);
                        pch = strtok (line,":");

                        printf ("%s\n",pch);
                        printf("%i \n", atoi(pch));
                        printf("%i \n", time(NULL) - atoi(pch));
                        temp = time(NULL) - atoi(pch);
                        //printf("THIS IT TEMP %i \n", temp);

                        pch = strtok (NULL, ":");
                        //printf("THIS IT IP %s \n", pch);
                        if ( fin2 ) {
                            fprintf(fin2, "<%s,%i> \n", pch, temp);
                        }else{
                            printf("COULDNT OPEN ACTIVE1.txt" );
                        }
                        
                        
                    //}
                    free(line);
                    
                    
                }
            }
            fclose(fin);
            fclose(fin2);

            

            fclose (fp);    
            str_server(sockfd, "active1.txt");
            close(sockfd);
            
        }
        else if( strcmp(buff, "#LOG") == 0){
            
            fprintf(fp, "%d: Received a #LOG action from agent 147.26.143.22 \n", time(NULL));
            fclose (fp);

            printf("[+]Data written in the file successfully.\n");

            str_server(sockfd, "log.txt");
            close(sockfd);
             
        }else {
            close(sockfd);
        }

    }else {
        strcpy(buff, "$NOT MEMBER\n");
        send_all(sockfd, buff, sizeof(buff));
        fprintf(fp, "%d: No response is supplied to agent 147.26.12.11 (agent not active) \n", time(NULL));
            fclose (fp);
            close(sockfd);
    }
        
        free(agentCopy);
   
} 
  
// Driver function 
int main() 
{ 
    

    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 

    while(1){
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
  // SEARCH IF AGENT IS ACTIVE
    int active = 0;
    FILE *fin;
    char *line;
    char str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(cli.sin_addr), str, INET_ADDRSTRLEN);
    if(access( "active.txt", F_OK ) == 0 ){
        fin = fopen("active.txt", "r");

    }else {
        fin = fopen("active.txt","w+");
    }
    

    if ( fin ) {
        while ( line = read_line(fin) ) {
            if ( strstr(line, str )){
                //fprintf(stdout, "%s\n", line);
                active = 1;
            }
            free(line);
        }
    }

    fclose(fin);
        
        
        handleAction(connfd, active, str);
        
       
    }
     
  
    // After chatting close the socket 
    
} 