#include "types.h"
#include "stat.h"
#include "user.h"
#include "context.h"
#include "drawingAPI.h"
#include "message.h"
#include "bitmap.h"
#include "clickable.h"

struct Context context;

int
main(int argc, char *argv[])
{

    int winid;
    struct Msg msg;
    int isRun = 1;
    int i = 0;
    //PICNODE pic;

    winid = init_context(&context, 400, 300);
    testClickable(context);

    fill_rect(context, 0, 0, context.width, context.height, 0xf800);

//    puts_str(context, "shell: welcome", 0x0, 0, 0);
//    draw_line(context, 0, 0, 50, 50, 0x0);
//    loadBitmap(&pic, "9.bmp");
//    draw_picture(context, pic, 0, 0);


    draw_window(context, "shell");

    while(isRun)
    {
        getMsg(&msg);
        switch(msg.msg_type)
        {
            case MSG_DOUBLECLICK:
                i++;
                puts_str(context, "shell double clicked!", 0x0, 0, 20*i);
                updateWindow(winid, context.addr);
                break;
            case MSG_UPDATE:
                updateWindow(winid, context.addr);
                printf(0, "shell");
                break;
            case MSG_LPRESS:
                i++;
                puts_str(context, "shell clicked!", 0x0, 0, 20*i);
                updateWindow(winid, context.addr);
                break;
            case MSG_KEYDOWN:
                i++;
                printf(0, "key pressed: %s\n", msg.concrete_msg.msg_key.key);
                char temp[2];
                temp[0] = msg.concrete_msg.msg_key.key;
                temp[1] = '\0';
                puts_str(context, temp, 0*0, 0, 20*i);
                updateWindow(winid, context.addr);
                break;
            default:
                break;
        }
    }
    
    free_context(&context, winid);
    exit();
}
