#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <regex.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void rcheck(char* file){
    regex_t regex;
    int reti;
    char msgbuf[100];

    /* Compile regular expression */
    reti = regcomp(&regex, "^([A-z0-9._-+).txt$", REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        return;
    }

    /* Execute regular expression */
    reti = regexec(&regex, file, 0, NULL, 0);
    if (!reti) {
        printf("Match\n");
    }
    else if (reti == REG_NOMATCH) {
        printf("No match\n");
    }
    else {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return;
    }

    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
}


int main() {
  int length, i = 0;
  int fd;
  int wd;
  char buffer[EVENT_BUF_LEN];

  /*creating the INOTIFY instance*/
  fd = inotify_init();

  /*checking for error*/
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }

  /*adding the “/tmp” directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
  wd = inotify_add_watch( fd, "/tmp", IN_CLOSE_WRITE | IN_DELETE | IN_MOVE | IN_OPEN);

  /*read to determine the event change happens on “/tmp” directory. Actually this read blocks until the change event occurs*/ 

    

  /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
  while ( 1 ) {
    length = read( fd, buffer, EVENT_BUF_LEN ); 

    /*checking for error*/
    if ( length < 0 ) {
        perror( "read" );
    }     
      struct inotify_event *event = ( struct inotify_event * ) &buffer[0];
      if ( event->len ) {
        if ( event->mask & IN_CLOSE_WRITE ) {
            if ( event->mask & IN_ISDIR ) {
            printf( "New directory %s IN_CLOSE_WRITE.\n", event->name );
            }
            else {
            rcheck(event->name);
            printf( "New file %s IN_CLOSE_WRITE.\n", event->name );
            }
        }
        else if ( event->mask & IN_DELETE ) {
            if ( event->mask & IN_ISDIR ) {
            printf( "Directory %s deleted.\n", event->name );
            }
            else {
            printf( "File %s deleted.\n", event->name );
            }
        }
        else if(event->mask & IN_MOVE ){
            printf("%s was moved\n", event->name);
        }
        else if(event->mask & IN_OPEN ){
            rcheck(event->name);
            printf("%s was opened\n", event->name);
        }
      }
      memset(&buffer, 0, sizeof(buffer));
    
  }

  /*removing the “/tmp” directory from the watch list.*/
   inotify_rm_watch( fd, wd );

  /*closing the INOTIFY instance*/
   close( fd );

}