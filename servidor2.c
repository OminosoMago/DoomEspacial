#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <string.h>

void emit(int fd, int type, int code, int val){
    struct input_event ie;
    ie.type = type; ie.code = code; ie.value = val;
    ie.time.tv_sec = 0; ie.time.tv_usec = 0;
    write(fd, &ie, sizeof(ie));
}

int main() {
    // Configurar uinput
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_ESC);
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    strcpy(usetup.name, "Virtual Keyboard");
    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    // Configurar socket servidor
    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 1);
    int client = accept(server, NULL, NULL);

    char tecla;
    while(read(client, &tecla, 1) > 0){
        if(tecla == 1){
            emit(fd, EV_KEY, KEY_ESC, 1);
            emit(fd, EV_SYN, SYN_REPORT, 0);
            emit(fd, EV_KEY, KEY_ESC, 0);
            emit(fd, EV_SYN, SYN_REPORT, 0);
        }
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    return 0;
}
