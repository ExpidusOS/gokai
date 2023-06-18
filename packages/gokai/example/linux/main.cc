#include <gokai/flutter/application.h>

int main(int argc, char** argv) {
  g_autoptr(GokaiApplication) app = gokai_application_new(APPLICATION_ID);
  return g_application_run(G_APPLICATION(app), argc, argv);
}