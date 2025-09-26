#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ENTRIES 100      // 最大键值对数量
#define MAX_KEY_LEN 11       // 键最大长度（10字符 + 1个结束符）
#define MAX_VALUE_LEN 100    // 值最大长度
#define MAX_LINE_LEN 256     // 每行最大长度

// 键值对结构体
typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValuePair;

KeyValuePair entries[MAX_ENTRIES]; // 存储键值对的数组
int entry_count = 0;               // 当前存储的键值对数量

// 去除字符串首尾的空白字符
void trim_whitespace(char* str) {
    if (str == NULL || *str == '\0') return;

    char* start = str;
    char* end = str + strlen(str) - 1;

    while (isspace((unsigned char)*start)) start++;

    while (end > start && isspace((unsigned char)*end)) end--;

    if (start != str) {
        memmove(str, start, end - start + 1);
    }

    str[end - start + 1] = '\0';
}

// 检查键是否有效（不超过10字符，不含空格）
int is_valid_key(const char* key) {
    if (key == NULL || *key == '\0') {
        return 0; // 空键无效
    }

    if (strlen(key) > 10) {
        return 0; // 超过10字符
    }

    // 检查是否包含空格
    for (int i = 0; key[i] != '\0'; i++) {
        if (isspace((unsigned char)key[i])) {
            return 0; // 包含空格
        }
    }

    return 1; // 键有效
}

// 检查键是否已存在
int key_exists(const char* key) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].key, key) == 0) {
            return 1; // 键已存在
        }
    }
    return 0; // 键不存在
}

// 解析数据文件
int parse_data_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("错误：无法打开文件 '%s'\n", filename);
        printf("请确保文件与程序在同一目录下\n");
        return 0;
    }

    char line[MAX_LINE_LEN];
    int line_number = 0;
    int valid_count = 0;
    int error_count = 0;

    printf("正在解析文件 '%s'...\n", filename);

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;

        // 去除行首尾的空白字符和换行符
        trim_whitespace(line);

        // 跳过空行
        if (line[0] == '\0') {
            continue;
        }

        // 查找冒号分隔符
        char* colon = strchr(line, ':');
        if (colon == NULL) {
            printf("警告：第 %d 行格式错误（缺少冒号）：%s\n", line_number, line);
            error_count++;
            continue;
        }

        // 通过冒号分割前后部分
        *colon = '\0'; 
        char* key = line;
        char* value = colon + 1;

        trim_whitespace(key);
        trim_whitespace(value);

        // 检查键的有效性，是否为空，是否重复
        if (!is_valid_key(key)) {
            printf("警告：第 %d 行键无效：%s\n", line_number, key);
            error_count++;
            continue;
        }

        if (value[0] == '\0') {
            printf("警告：第 %d 行值为空\n", line_number);
            error_count++;
            continue;
        }

        if (key_exists(key)) {
            printf("警告：第 %d 行键重复：%s\n", line_number, key);
            error_count++;
            continue;
        }

        // 存储有效的键值对
        if (entry_count < MAX_ENTRIES) {
            strncpy(entries[entry_count].key, key, MAX_KEY_LEN - 1);
            entries[entry_count].key[MAX_KEY_LEN - 1] = '\0';

            strncpy(entries[entry_count].value, value, MAX_VALUE_LEN - 1);
            entries[entry_count].value[MAX_VALUE_LEN - 1] = '\0';

            entry_count++;
            valid_count++;
        }
        else {
            printf("警告：已达到最大键值对数量限制(%d)，跳过后续行\n", MAX_ENTRIES);
            break;
        }
    }

    fclose(file);

    printf("解析完成！\n");
    printf("有效键值对：%d，错误行：%d，总行数：%d\n", valid_count, error_count, line_number);
    printf("成功加载 %d 个键值对\n\n", entry_count);

    return 1;
}

// 查找键对应的值
const char* find_value(const char* key) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].key, key) == 0) {
            return entries[i].value;
        }
    }
    return NULL; 
}

// 交互式查询循环
void start_query_loop() {
    char input[MAX_LINE_LEN];

    printf("=== 键值查询系统 ===\n");
    printf("输入键名查询对应的值，输入 'Quit' 退出程序\n");
    printf("===================================\n");

    while (1) {
        printf("> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; // 处理Ctrl+Z/Ctrl+D
        }

        trim_whitespace(input);

        if (strcmp(input, "Quit") == 0) {
            printf("再见！\n");
            break;
        }

        if (input[0] == '\0') {
            continue;
        }

        const char* value = find_value(input);
        if (value != NULL) {
            printf("%s\n", value);
        }
        else {
            printf("Error\n");
        }
    }
}

int main() {
    const char* filename = "data.txt";

    if (!parse_data_file(filename)) {
        printf("按回车键退出...");
        getchar();
        return 1;
    }

    start_query_loop();

    return 0;
}