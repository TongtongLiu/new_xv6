#include "types.h"
#include "stat.h"
#include "user.h"
#include "context.h"
#include "drawingAPI.h"
#include "message.h"

struct Context context;

int
main(int argc, char *argv[])
{
    int winid;
    struct Msg msg;

    winid = init_context(&context, 400, 300); 
    fill_rect(context, 0, 0, context.width, context.height, 0xf800);
    puts_str(context, "shell: welcome", 0x0, 0, 0);

    while(1)
    {
        msg.msg_type = MSG_NONE;
        getMsg(&msg);
        switch(msg.msg_type)
        {
            case MSG_UPDATE:
                updateWindow(winid, context.addr);
                printf(0, "shell");
                break;
            default:
                break;
        }
    }

    free_context(&context, winid);
    exit();
}
