#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <sys/ioctl.h>

typedef struct {
    uint8_t key;
    uint8_t pressed;
} InputPacket;

int create_uinput() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_W);
    ioctl(fd, UI_SET_KEYBIT, KEY_A);
    ioctl(fd, UI_SET_KEYBIT, KEY_S);
    ioctl(fd, UI_SET_KEYBIT, KEY_D);
    ioctl(fd, UI_SET_KEYBIT, KEY_ENTER);
    ioctl(fd, UI_SET_KEYBIT, KEY_UP);
    ioctl(fd, UI_SET_KEYBIT, KEY_DOWN);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFT);

    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    strcpy(usetup.name, "doom-virtual-keyboard");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    sleep(1);
    return fd;
}

void send_key(int fd, int keycode, int pressed) {
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = keycode;
    ev.value = pressed;
    write(fd, &ev, sizeof(ev));

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(fd, &ev, sizeof(ev));
}

int map_key(uint8_t key) {
    switch (key) {
        case 'w': return KEY_W;
        case 'a': return KEY_A;
        case 's': return KEY_S;
        case 'd': return KEY_D;
        case '\n': return KEY_ENTER;
        case 'U': return KEY_UP;
        case 'D': return KEY_DOWN;
        case 'R': return KEY_RIGHT;
        case 'L': return KEY_LEFT;
        default: return -1;
    }
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    InputPacket packet;

    int uinput_fd = create_uinput();

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Servidor UDP escuchando en puerto 6000...\n");

    while (1) {
        recv(sock, &packet, sizeof(packet), 0);

        int keycode = map_key(packet.key);
        if (keycode != -1) {
            send_key(uinput_fd, keycode, packet.pressed);
            //printf("Recibido: key=%c, pressed=%d\n", packet.key, packet.pressed);
        }
    }

    return 0;
}