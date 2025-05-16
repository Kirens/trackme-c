#include "web.h"
#include "../lib/ctemplate.h"
#include "util/log.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern tmpls tmpl_paths;

void create_index_html(char *is_running, char *name, char *client,
                       char *project, char *description,
                       char *current_duration) {
  TMPL_varlist *tmpl_vars = NULL;
  tmpl_vars = TMPL_add_var(NULL, "is_running", is_running, "name", name,
                           "client", client, "project", project, "description",
                           description, "duration", current_duration, NULL);

  FILE *index_html_file = fopen("index.html", "w+");
  int err = TMPL_write(tmpl_paths.index, NULL, NULL, tmpl_vars,
                       index_html_file, stderr);
  TMPL_free_varlist(tmpl_vars);
  fclose(index_html_file);

  if (err) {
    t_log(ERROR, __func__, "Failed at writing HTML file");
    // return static error page?
  }
}

void create_start_timer_html() {
  FILE *start_html = fopen("start_timer.html", "w+");
  int err = TMPL_write(tmpl_paths.start, NULL, NULL, NULL,
                       start_html, stderr);
  fclose(start_html);

  if (err) {
    t_log(ERROR, __func__, "Failed at writing HTML file");
    // return static error page?
  }
}

void create_stop_timer_html(char *name, char *client, char *project,
                            char *description, char *start_time, char *end_time,
                            char *final_duration) {
  TMPL_varlist *tmpl_vars = NULL;
  tmpl_vars =
      TMPL_add_var(NULL, "name", name, "client", client, "project", project,
                   "description", description, "start_time", start_time,
                   "end_time", end_time, "duration", final_duration, NULL);

  FILE *stop_html = fopen("stop_timer.html", "w+");
  int err = TMPL_write(tmpl_paths.stop, NULL, NULL, tmpl_vars,
                       stop_html, stderr);
  fclose(stop_html);

  if (err) {
    t_log(ERROR, __func__, "Failed at writing HTML file");
    // return static error page?
  }
}

void create_edit_html(char *name, char *client, char *project,
                      char *description, char *start_time, char *end_time) {
  TMPL_varlist *tmpl_vars = NULL;
  tmpl_vars = TMPL_add_var(NULL, "name", name, "client", client, "project",
                           project, "description", description, "start_time",
                           start_time, "end_time", end_time, NULL);

  FILE *edit_html = fopen("edit_timer.html", "w+");
  int err = TMPL_write(tmpl_paths.edit, NULL, NULL, tmpl_vars,
                       edit_html, stderr);
  fclose(edit_html);

  if (err) {
    t_log(ERROR, __func__, "Failed at writing HTML file");
    // return static error page?
  }
}

void clean_up() {
  remove("edit_timer.html");
  remove("start.html");
  remove("stop.html");
  remove("switch.html");
  remove("index.html");
  remove("error.html");
  t_log(INFO, __func__, "Cleaned up session html files.");
}
