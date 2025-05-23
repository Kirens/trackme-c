#ifndef WEB_H
#define WEB_H

typedef struct tmpls {
  char* index;
  char* start;
  char* stop;
  char* edit;
} tmpls;

void create_index_html(char* is_running, char *name, char *client, char *project,
                       char *description, char *duration);
void create_start_timer_html();
void create_edit_html(char *name, char *client, char *project,
                      char *description, char *start_time, char *end_time);
void create_stop_timer_html(char *name, char *client, char *project,
                            char *description, char *start_time, char *end_time,
                            char *final_duration);
void clean_up();

#endif // !WEB_H
