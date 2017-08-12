#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "config.h"
#include "main_window.h"
#include "reactor.h"

GSList *cmd_line_file_list;

/* Call-back to handle a local option on command line */
static gint
handle_local_options (GApplication __unused * app, GVariantDict * options) {
  /* Handling the '--version'/'-V' option */
  if (g_variant_dict_contains (options, "version"))
    {
      fprintf (stdout, "%s - Version %s\n", PROG_NAME, PROG_VERSION);
      return RETURN_SUCCESS;
    }

  return RETURN_FAILURE;
}

/* Call-back to setup application when run by the Desktop environment */
static void
activate_main_window (GApplication * app) {
  GtkWidget * window = main_window_new (app);
  gtk_widget_show_all (window);
}

static void
open_cmdline (GtkApplication __unused * app)
{
  g_print ("Open\n");
}

int
main (int argc, char **argv)
{

  GtkApplication *app =
    gtk_application_new ("NUCLEAR REACTOR",
			 G_APPLICATION_NON_UNIQUE |
			 G_APPLICATION_HANDLES_OPEN);

  /* Connect call-backs on signals */
  g_signal_connect (G_APPLICATION (app), "activate",
		    G_CALLBACK (activate_main_window), NULL);
  g_signal_connect (G_APPLICATION (app), "handle-local-options",
		    G_CALLBACK (handle_local_options), NULL);
  g_signal_connect (G_APPLICATION(app), "open",
		    G_CALLBACK (open_cmdline), NULL);

  /* Adding custom options to command-line */
  const GOptionEntry options[] = {
    {"version", 'V', 0, G_OPTION_ARG_NONE, NULL,
     "Show application's version", NULL},
    {NULL}
  };
  g_application_add_main_option_entries (G_APPLICATION (app), options);

  /* Starting the bar threads */
  start_threads();

  /* Starting the application */
  int status = g_application_run (G_APPLICATION (app), argc, argv);

  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&bar_mutex);
  pthread_cond_destroy(&unstable_state);
  pthread_exit(NULL);

  /* Free memory on quit */
  g_object_unref (app);

  return status;
}
