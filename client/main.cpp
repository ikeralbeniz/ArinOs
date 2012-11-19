#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>
#include <JavaScriptCore/JSValueRef.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sstream>
#include <X11/Xlib.h>
/*
MY TES CLASS


*/

static void arincore_init_cb(JSContextRef ctx, JSObjectRef object)
{
// ...
}

static void arincore_finalize_cb(JSObjectRef object)
{
// ...
}

static const char* arincore_socket(char* jsondata)
{
  int sockfd = 0, n = 0;
  char recvBuff[2048];
  static std::ostringstream strout;

  struct sockaddr_in serv_addr;
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
        return "{'status':'fail', 'error':{'code':'501','message':'Error : Could not create socket'}}";
  }
  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(4102);

  if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
  {
        return "{'status':'fail', 'error':{'code':'502','message':'Error : inet_pton error occured'}}";
  }

  if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
       return "{'status':'fail', 'error':{'code':'503','message':'Error : Connect Faile'}}";
  }

  long readed_bits = 0;

  write(sockfd, jsondata, strlen(jsondata));
  while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
  {
        strout << recvBuff;
        readed_bits = readed_bits + 2048;
  }
  close(sockfd);
  char output[readed_bits];
  strcpy(output,strout.str().c_str());
  return output;
}

static JSValueRef arincore_sync(JSContextRef context,
                       JSObjectRef function,
                       JSObjectRef thisObject,
                       size_t argumentCount,
                       const JSValueRef arguments[],
                       JSValueRef *exception)
{
  if(argumentCount > 0){
    JSStringRef input_string = JSValueToStringCopy(context, arguments[0],NULL);

    size_t strLen = JSStringGetLength(input_string);
    const JSChar* jsstr = JSStringGetCharactersPtr(input_string);

    char str[strLen+1];
    int i;
    for(i=0; i < strLen;i++){
      str[i] = jsstr[i];
    }
    str[strLen] = 0;
    const char* core_response = arincore_socket(str);

    printf("%s",core_response);
    fflush(stdout);


    JSStringRef string = JSStringCreateWithUTF8CString(core_response);
    return JSValueMakeString(context,string);
  }

  return JSValueMakeNull(context);
}




/*

Class methods list/structure

*/

static const JSStaticFunction arincore_staticfuncs[] ={
  { "sync", arincore_sync, kJSPropertyAttributeReadOnly },
  { NULL, NULL, 0 }
};


/*

Clas definition

*/

static const JSClassDefinition arincore_def ={
  0,
  kJSClassAttributeNone,
  "ArinCore",
  NULL,

  NULL,
  arincore_staticfuncs,

  arincore_init_cb,
  arincore_finalize_cb,

  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/************************************************************************************************************************

Ad JS classes

********************************************************************/

static void addJSClasses(JSGlobalContextRef context)
{
  JSClassRef classDef = JSClassCreate(&arincore_def);
  JSObjectRef classObj = JSObjectMake(context, classDef, context);
  JSObjectRef globalObj = JSContextGetGlobalObject(context);
  JSStringRef str = JSStringCreateWithUTF8CString("arincore");
  JSObjectSetProperty(context, globalObj, str, classObj,kJSPropertyAttributeNone, NULL);
}

/******************************************************

Ading the classes on webkit

******************************************/

static void window_object_cleared_cb(WebKitWebView  *web_view,
                                WebKitWebFrame *frame,
                                gpointer        context,
                                gpointer        arg3,
                                gpointer        user_data)

{
  JSGlobalContextRef jsContext = webkit_web_frame_get_global_context(frame);
  addJSClasses(jsContext);
}

static GtkWidget* main_window;
static WebKitWebView* web_view;

static void destroy_cb(GtkWidget* widget, gpointer data)
{
  gtk_main_quit ();
}

static GtkWidget* create_browser()
{
  GtkWidget* scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
  gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (web_view));

  g_signal_connect (G_OBJECT (web_view), "window-object-cleared", G_CALLBACK(window_object_cleared_cb), web_view);

  return scrolled_window;
}

static GtkWidget* create_window()
{
  GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 500, 500);
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_cb), NULL);
  return window;
}


/***************************
X system resolution

*****************************/
int getScreenSize(int *w, int*h)
{

     Display* pdsp = NULL;
     Screen* pscr = NULL;

     pdsp = XOpenDisplay( NULL );
     if ( !pdsp ) {
      fprintf(stderr, "Failed to open default display.\n");
      return -1;
     }

        pscr = DefaultScreenOfDisplay( pdsp );
     if ( !pscr ) {
      fprintf(stderr, "Failed to obtain the default screen of given display.\n");
      return -2;
     }

     *w = pscr->width;
     *h = pscr->height;

     XCloseDisplay( pdsp );
     return 0;
}



int main (int argc, char* argv[])
{
  int w, h;
  getScreenSize(&w, &h);

  gtk_init (&argc, &argv);
  if (!g_thread_supported())
     g_thread_init (NULL);

  GtkWidget* vbox = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), create_browser (), TRUE, TRUE, 0);

  main_window = create_window();
  gtk_container_add(GTK_CONTAINER (main_window), vbox);

  gchar* uri = (gchar*) "file://simple.html";
  webkit_web_view_open(web_view, uri);

  gtk_widget_grab_focus (GTK_WIDGET (web_view));
  //gtk_window_fullscreen (main_window);
  gtk_window_set_title(GTK_WINDOW (main_window),"ArinOs");
  gtk_window_set_default_size(GTK_WINDOW (main_window), w, h);
  gtk_widget_show_all (main_window);
  gtk_main ();

  return 0;
}
