#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FLOORS 5
#define ROWS 4
#define COLS 4
#define DAYS 7
#define MAX_USERS 27 // A-Z + Admin
#define FILENAME "library_data.dat"


// 用户类型
typedef enum {
    USER_NORMAL,
    USER_ADMIN
} UserType;

// 用户结构
typedef struct {
    char name[20];
    UserType type;
} User;

// 座位状态
typedef enum {
    STATUS_EMPTY = 0,
    STATUS_RESERVED = 1,
    STATUS_SELF_RESERVED = 2
} SeatStatus;

// 座位结构
typedef struct {
    SeatStatus status;
    char reserved_by; // 预约的用户字母
    time_t reserve_time; // 预约时间
} Seat;

// 图书馆系统
typedef struct {
    Seat seats[FLOORS][ROWS][COLS][DAYS]; // 5层×4行×4列×7天
    User current_user;
    int is_logged_in;
} LibrarySystem;

// 全局系统实例
LibrarySystem library;
// 保存数据到文件
void save_data() {
    FILE* file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("无法保存数据到文件！\n");
        return;
    }

    fwrite(library.seats, sizeof(library.seats), 1, file);
    fclose(file);
    printf("数据已保存！\n");
}

// 从文件加载数据
void load_data() {
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("无保存数据，使用默认数据\n");
        // 初始化所有座位为空
        memset(library.seats, 0, sizeof(library.seats));
        return;
    }

    fread(library.seats, sizeof(library.seats), 1, file);
    fclose(file);
    printf("数据已加载！\n");
}

// 清空所有数据
void clear_data() {
    memset(library.seats, 0, sizeof(library.seats));
    save_data();
    printf("所有数据已清空！\n");
}
// 获取星期几的名称
const char* get_day_name(int day) {
    const char* days[] = { "周日", "周一", "周二", "周三", "周四", "周五", "周六" };
    return days[day];
}

// 显示座位状态
void display_seats(int floor, int day) {
    printf("\n=== 第%d层 - %s ===\n", floor + 1, get_day_name(day));
    printf("    ");
    for (int col = 0; col < COLS; col++) {
        printf("%d   ", col + 1);
    }
    printf("\n");

    for (int row = 0; row < ROWS; row++) {
        printf("%d | ", row + 1);
        for (int col = 0; col < COLS; col++) {
            Seat seat = library.seats[floor][row][col][day];

            if (library.current_user.type == USER_ADMIN) {
                // 管理员视图：显示具体用户
                if (seat.status == STATUS_EMPTY) {
                    printf("0   ");
                }
                else {
                    printf("%c   ", seat.reserved_by);
                }
            }
            else {
                // 普通用户视图
                switch (seat.status) {
                case STATUS_EMPTY:
                    printf("0   ");
                    break;
                case STATUS_RESERVED:
                    printf("1   ");
                    break;
                case STATUS_SELF_RESERVED:
                    printf("2   ");
                    break;
                }
            }
        }
        printf("\n");
    }
}

// 登录功能
void login() {
    char username[20];
    printf("请输入用户名: ");
    scanf("%s", username);

    // 清空输入缓冲区
    while (getchar() != '\n');

    if (strcmp(username, "Admin") == 0) {
        strcpy(library.current_user.name, "Admin");
        library.current_user.type = USER_ADMIN;
        library.is_logged_in = 1;
        printf("管理员登录成功！\n");
    }
    else if (strlen(username) == 1 && isalpha(username[0])) {
        char user_char = toupper(username[0]);
        strcpy(library.current_user.name, username);
        library.current_user.type = USER_NORMAL;
        library.is_logged_in = 1;
        printf("用户 %c 登录成功！\n", user_char);
    }
    else {
        printf("无效用户名！请输入 A-Z \n");
    }
}

// 退出登录
void logout() {
    if (library.is_logged_in) {
        printf("用户 %s 已退出登录\n", library.current_user.name);
        library.is_logged_in = 0;
        memset(&library.current_user, 0, sizeof(library.current_user));
    }
    else {
        printf("当前未登录\n");
    }
}

// 预约座位
void reserve_seat() {
    if (!library.is_logged_in) {
        printf("请先登录！\n");
        return;
    }

    int floor, row, col, day;
    printf("请输入要预约的座位信息（层 行 列 天）: ");
    scanf("%d %d %d %d", &floor, &row, &col, &day);

    // 转换为0-based索引并验证
    floor--; row--; col--; day--;

    if (floor < 0 || floor >= FLOORS || row < 0 || row >= ROWS ||
        col < 0 || col >= COLS || day < 0 || day >= DAYS) {
        printf("无效的输入！\n");
        return;
    }

    Seat* seat = &library.seats[floor][row][col][day];

    if (seat->status != STATUS_EMPTY) {
        printf("该座位已被预约！\n");
        return;
    }

    // 设置预约信息
    seat->status = (library.current_user.type == USER_ADMIN) ? STATUS_RESERVED : STATUS_SELF_RESERVED;
    seat->reserved_by = (library.current_user.type == USER_ADMIN) ? 'A' : toupper(library.current_user.name[0]);
    seat->reserve_time = time(NULL);

    save_data();
    printf("预约成功！\n");
}

// 取消预约
void cancel_reservation() {
    if (!library.is_logged_in) {
        printf("请先登录！\n");
        return;
    }

    int floor, row, col, day;
    printf("请输入要取消预约的座位信息（层 行 列 天）: ");
    scanf("%d %d %d %d", &floor, &row, &col, &day);

    floor--; row--; col--; day--;

    if (floor < 0 || floor >= FLOORS || row < 0 || row >= ROWS ||
        col < 0 || col >= COLS || day < 0 || day >= DAYS) {
        printf("无效的输入！\n");
        return;
    }

    Seat* seat = &library.seats[floor][row][col][day];

    if (seat->status == STATUS_EMPTY) {
        printf("该座位未被预约！\n");
        return;
    }

    if (library.current_user.type == USER_NORMAL &&
        seat->reserved_by != toupper(library.current_user.name[0])) {
        printf("您只能取消自己的预约！\n");
        return;
    }

    seat->status = STATUS_EMPTY;
    seat->reserved_by = '\0';
    seat->reserve_time = 0;

    save_data();
    printf("取消预约成功！\n");
}

// 查看所有预约
void view_all_reservations() {
    if (!library.is_logged_in || library.current_user.type != USER_ADMIN) {
        printf("需要管理员权限！\n");
        return;
    }

    printf("\n=== 所有预约信息 ===\n");
    int count = 0;

    for (int floor = 0; floor < FLOORS; floor++) {
        for (int day = 0; day < DAYS; day++) {
            for (int row = 0; row < ROWS; row++) {
                for (int col = 0; col < COLS; col++) {
                    Seat seat = library.seats[floor][row][col][day];
                    if (seat.status != STATUS_EMPTY) {
                        count++;
                        printf("第%d层 %s (%d,%d) - 用户: %c, 时间: %s",
                            floor + 1, get_day_name(day), row + 1, col + 1,
                            seat.reserved_by, ctime(&seat.reserve_time));
                    }
                }
            }
        }
    }

    if (count == 0) {
        printf("暂无预约记录\n");
    }
}
// 显示主菜单
void show_menu() {
    printf("\n=== 图书馆座位预约系统 ===\n");
    if (library.is_logged_in) {
        printf("当前用户: %s (%s)\n",
            library.current_user.name,
            library.current_user.type == USER_ADMIN ? "管理员" : "普通用户");
    }
    printf("1. 显示座位状态\n");
    printf("2. 预约座位\n");
    printf("3. 取消预约\n");
    if (library.is_logged_in && library.current_user.type == USER_ADMIN) {
        printf("4. 查看所有预约\n");
        printf("5. 清空所有数据\n");
    }
    printf("Login - 登录\n");
    printf("Exit - 退出登录\n");
    printf("Quit - 退出程序\n");
    printf("请选择操作: ");
}

// 处理用户输入
void process_command() {
    char command[20];
    scanf("%s", command);

    if (strcmp(command, "Login") == 0) {
        login();
    }
    else if (strcmp(command, "Exit") == 0) {
        logout();
    }
    else if (strcmp(command, "Quit") == 0) {
        save_data();
        printf("再见！\n");
        exit(0);
    }
    else if (isdigit(command[0])) {
        int choice = atoi(command);

        if (choice == 1) {
            int floor, day;
            printf("请输入要查看的层数和天数（1-5 1-7）: ");
            scanf("%d %d", &floor, &day);
            if (floor >= 1 && floor <= 5 && day >= 1 && day <= 7) {
                display_seats(floor - 1, day - 1);
            }
            else {
                printf("无效的输入！\n");
            }
        }
        else if (choice == 2) {
            reserve_seat();
        }
        else if (choice == 3) {
            cancel_reservation();
        }
        else if (choice == 4 && library.is_logged_in && library.current_user.type == USER_ADMIN) {
            view_all_reservations();
        }
        else if (choice == 5 && library.is_logged_in && library.current_user.type == USER_ADMIN) {
            clear_data();
        }
        else {
            printf("无效的选择或权限不足！\n");
        }
    }
    else {
        printf("无效的命令！\n");
    }

    // 清空输入缓冲区
    while (getchar() != '\n');
}

void init_system() {
    memset(&library, 0, sizeof(library));
    library.is_logged_in = 0;
    load_data();
}

int main() {
    init_system();
    printf("图书馆座位预约系统启动成功！\n");

    while (1) {
        show_menu();
        process_command();
    }

    return 0;
} 
