#include <bson/bson.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../lib/mongoose.h"
// #include "db.h"
#include "timer.h"
#include "track_me.h"
#include "util/log.h"
#include "web.h"

static void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    struct mg_http_serve_opts opts = {.root_dir = "./"}; // Serve local dir

    /* 
     * Pages 
     */
    if (mg_match(hm->uri, mg_str("/"), NULL)) {
      create_index_html(is_timer_running() ? "true" : NULL, get_name(),
                        get_client(), get_project(), get_description(),
                        get_duration());
    }

    else if (mg_match(hm->uri, mg_str("/start_timer.html"), NULL)) {
      create_start_timer_html();
    }

    else if (mg_match(hm->uri, mg_str("/stop_timer.html"), NULL)) {
      t_log(INFO, __func__, "Stopping timer...");
      if (!stop_timer()) {
        t_log(INFO, __func__, "Could not stop timer, returning NULL...");
        create_stop_timer_html(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      } else {
        create_stop_timer_html(get_name(), get_client(), get_project(),
                               get_description(), get_start_time(),
                               get_end_time(), get_duration());
      }
    }

    else if (mg_match(hm->uri, mg_str("/edit_timer.html"), NULL)) {
      // TODO this is possible error - is start time really 00:00:00??
      if (strcmp(get_start_time(), "00:00:00") == 0) {
        t_log(INFO, __func__, "Timer not started, can not edit");
        create_edit_html(NULL, NULL, NULL, NULL, NULL, NULL);
      } else {
        create_edit_html(get_name(), get_client(), get_project(),
                         get_description(), get_start_time(), get_end_time());
      }
    }

    /*
     * Post/data endpoints
     */

    else if (mg_match(hm->uri, mg_str("/start_info"), NULL) &&
             (mg_match(hm->method, mg_str("POST"), NULL))) {
      if (start_timer(&hm->body)) {
        create_index_html(is_timer_running() ? "true" : NULL, get_name(),
                          get_client(), get_project(), get_description(),
                          get_duration());
        return mg_http_serve_file(c, ev_data, "./index.html", &opts);
      } else {
        // create_error_page();
        // TODO Error page
        return mg_http_serve_file(c, ev_data, "./error.html", NULL);
      }
    }

    else if (mg_match(hm->uri, mg_str("/duration"), NULL)) {
      return mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                    "{%m:\"%s\"}\n", MG_ESC("duration"), get_duration());
    }

    t_log(INFO, __func__, "Serving local dir");
    mg_http_serve_dir(c, ev_data, &opts);
  }
}

tmpls tmpl_paths;

int main() {
  struct mg_mgr mgr;       // Event manager
  mg_log_set(MG_LL_DEBUG); // Set log level
  mg_mgr_init(&mgr);       // Initialise event manager
  // // char *db_uri = "/this/is/db/URI";
  // // int err = init_db(db_uri);
  // // if (err) {
  // //   t_log(ERROR, __func__, "failed to initialize db. \n");
  // //   return EXIT_FAILURE;
  // // }
  printf("Hello Mongoose!\n");


  char *indx_end = "/index.tmpl";
  char *strt_end = "/start_timer.tmpl";
  char *stop_end = "/stop_timer.tmpl";
  char *edit_end = "/edit_timer.tmpl";

  char *base_path = getenv("TMPL_PATH");
  if (!base_path) base_path = "templates";
  int base_len = strlen(base_path);

  int indx_len = base_len + strlen(indx_end)+1;
  tmpl_paths.index = malloc(indx_len);
  if (!tmpl_paths.index) {
    t_log(ERROR, __func__, "Malloc: could not allocate enough memory.");
    return EXIT_FAILURE;
  }
  int strt_len = base_len + strlen(strt_end)+1;
  tmpl_paths.start = malloc(strt_len);
  if (!tmpl_paths.start) {
    t_log(ERROR, __func__, "Malloc: could not allocate enough memory.");
    return EXIT_FAILURE;
  }
  int stop_len = base_len + strlen(stop_end)+1;
  tmpl_paths.stop = malloc(stop_len);
  if (!tmpl_paths.stop) {
    t_log(ERROR, __func__, "Malloc: could not allocate enough memory.");
    return EXIT_FAILURE;
  }
  int edit_len = base_len + strlen(edit_end)+1;
  tmpl_paths.edit = malloc(edit_len);
  if (!tmpl_paths.edit) {
    t_log(ERROR, __func__, "Malloc: could not allocate enough memory.");
    return EXIT_FAILURE;
  }

  snprintf(tmpl_paths.index, indx_len, "%s%s", base_path, indx_end);
  snprintf(tmpl_paths.start, strt_len, "%s%s", base_path, strt_end);
  snprintf(tmpl_paths.stop, stop_len, "%s%s", base_path, stop_end);
  snprintf(tmpl_paths.edit, edit_len, "%s%s", base_path, edit_end);

  // Setup Mongoose HTTP listener
  mg_http_listen(&mgr, "http://localhost:8000", ev_handler, NULL);

  // Event loop
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }

  // Cleanup
  mg_mgr_free(&mgr);
  clean_up();
  return EXIT_SUCCESS;
}
