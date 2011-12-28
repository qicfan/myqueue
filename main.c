#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/queue.h>
#include <event.h>
#include <evhttp.h>

#include "link.h"
#define MYHTTPD_SIGNATURE "ZEROQ QUEUE V0.0.1"
#define QUEUE_LIST_GET "get"
#define QUEUE_LIST_SET "set"
queue_entry * qe;
// HTTP HANDLER
void httpd_handler(struct evhttp_request *req, void *arg) {
  const char * uri;
  char * decoded_uri = NULL;
  char * action = NULL;
  char * data = NULL;
  uri = evhttp_request_uri(req);
  decoded_uri = evhttp_decode_uri(uri);
  struct evkeyvalq params;
  evhttp_parse_query(decoded_uri, &params);
  // 取得操作
  action = evhttp_find_header(&params, "act");
  data = evhttp_find_header(&params, "data");
  free(decoded_uri);
  if (NULL == data) {
    data = (char *) EVBUFFER_DATA(req->input_buffer);
  }
  // HTTP HEADER
  evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
  evhttp_add_header(req->output_headers, "Content-type", "text/html");
  evhttp_add_header(req->output_headers, "Connection", "close");
  struct evbuffer * buf;
  buf = evbuffer_new();
  if (NULL == action) {
    // no action, put the help
    evbuffer_add_printf(buf, "params error\n");
  } else {
    if (strcmp(QUEUE_LIST_GET, action) == 0) {
      // get
      char * q_val = queue_get(qe);
      evbuffer_add_printf(buf, "%s", q_val);
      free(q_val);
    } else {
      // set
      if (NULL != data) {
        queue_set(qe, data);
        evbuffer_add_printf(buf, "insert success! queue-size:%d", qe->size);
      } else {
        evbuffer_add_printf(buf, "no data into queue! queue-size:%d", qe->size);
      }
    }
  }
  free(action);
  free(data);
  evhttp_send_reply(req, HTTP_OK, "OK", buf);
  evbuffer_free(buf);
}

void show_help() {
  char * help = "written by zeroq (http://zeroq.me)\n-l <ip_addr> interface to listen on, default is 0.0.0.0\n-p <num> port number to listen on, default is 1985\n-d run as a daemon\n-h print this help and exit\n\n";
  fprintf(stderr, help);
}

void signal_handler(int sig) {
  switch (sig) {
    case SIGTERM:
    case SIGHUP:
    case SIGQUIT:
    case SIGINT:
      event_loopbreak();
      break;
  }
}

int main(int argc, char * argv[]) {
  qe = (queue_entry *)malloc(sizeof(queue_entry));
  qe->head = NULL;
  qe->foot = NULL;
  qe->size = 0;
  signal(SIGHUP, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);

  // default params
  char * httpd_option_listen = "0.0.0.0";
  int httpd_option_port = 8080;
  int httpd_option_daemon = 0;
  int httpd_option_timeout = 120; // in seconds
  // get params
  int c;
  while ((c = getopt(argc, argv, "l:p:dt:h")) != -1) {
    switch (c) {
      case 'l':
        httpd_option_listen = optarg;
        break;
      case 'p':
        httpd_option_port = atoi(optarg);
        break;
      case 'd':
        httpd_option_daemon = 1;
        break;
      case 't':
        httpd_option_timeout = atoi(optarg);
        break;
      case 'h':
      default:
        show_help();
        exit(EXIT_SUCCESS);
    }
  }

  if (httpd_option_daemon) {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
      perror("for failed");
      exit(EXIT_FAILURE);
    }
    if (pid > 0) {
      exit(EXIT_SUCCESS);
    }
  }

  event_init();

  struct evhttp * httpd;
  httpd = evhttp_start(httpd_option_listen, httpd_option_port);
  evhttp_set_timeout(httpd, httpd_option_timeout);

  evhttp_set_gencb(httpd, httpd_handler, NULL);
  event_dispatch();
  evhttp_free(httpd);
  evhttp_free(httpd);
  return 0;
}
