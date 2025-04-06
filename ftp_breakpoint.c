#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

static void usage(void)
{
    printf("ftp_breakpoint [-h] \n");
    printf("  -h : 帮助 \n");
    printf("  -m : 模式 \n");
    printf("    i : 获取ftp服务器上文件的大小 \n");
    printf("    u : 上传文件到ftp服务器 \n");
    printf("    d : 从ftp服务器下载文件 \n");
    printf("  -s : 服务端路径 \n");
    printf("  -f : 本地路径 \n");
    printf("  -b : 是否支持断点续传 \n");
    printf("  -u : 用户名:密码 \n");
}

// 进度回调函数
int ftp_download_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
    // clientp 是用户自定义指针，可通过 CURLOPT_PROGRESSDATA 设置
    // dltotal 是总下载字节数，如果未知则为0
    // dlnow 是已下载字节数
    // ultotal 和 ulnow 是上传相关信息（这里不关心）

    if(dltotal > 0) {
        printf("\r下载进度: %.0f/%.0f bytes (%.2f%%)", dlnow, dltotal, (dlnow/dltotal)*100);
    }

    // 返回非零会中止传输
    return 0;
}

// 上传进度回调函数
static int ftp_upload_callback(void *clientp,curl_off_t dltotal,curl_off_t dlnow,curl_off_t ultotal,curl_off_t ulnow)
{
    // clientp 是用户自定义指针
    // ultotal 是总上传字节数，如果未知则为0
    // ulnow 是已上传字节数
    // dltotal 和 dlnow 是下载相关信息（这里不关心）

    if(ultotal > 0) {
        printf("\r上传进度: %ld/%ld bytes (%.2f%%)", (long)ulnow, (long)ultotal,(double)ulnow/(double)ultotal*100.0);
    } else {
        printf("\r已上传: %ld bytes", (long)ulnow);
    }

    // 返回非零会中止传输
    return 0;
}

/*
    FTP获取文件大小
    server: ftp服务器地址
    return: 文件大小，-1 失败
*/
int ftp_get_file_size(char *server, char *username, char *password)
{
    CURL *curl;
    CURLcode res;
    double file_size = 0.0;

    curl = curl_easy_init();
    if(curl) {
        // 设置FTP URL
        curl_easy_setopt(curl, CURLOPT_URL, server);
        
        // 设置FTP用户名和密码
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
        
        // 只获取头部信息，不下载文件内容
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        
        // 不列出目录内容，只获取文件信息
        curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 0L);
        
        // 执行请求
        res = curl_easy_perform(curl);
        
        if(res == CURLE_OK) {
            // 获取文件大小
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &file_size);
            
            if((res == CURLE_OK) && (file_size > 0.0)) {
                printf("文件大小: %.0f 字节\n", file_size);
                return (int)file_size;
            } else {
                printf("无法获取文件大小或文件大小为0\n");
            }
        } else {
            fprintf(stderr, "请求失败: %s\n", curl_easy_strerror(res));
        }
        
        // 清理
        curl_easy_cleanup(curl);
    }

    return -1;
}

/*
    FTP下载文件
    server: ftp服务器地址
    local: 本地文件路径
    breakpoint: 是否支持断点续传
    return: 0 成功，-1 失败
*/
int ftp_download_file(char *server, char *local, bool breakpoint, char *username, char *password)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    long local_size = 0;

    if(breakpoint){
        if (access(local, F_OK) == 0) {
            // 获取本地已下载文件大小
            fp = fopen(local, "rb");
            if(fp) {
                fseek(fp, 0, SEEK_END);
                local_size = ftell(fp);
                fclose(fp);
                fp = NULL;
            }
            printf("文件存在，已经下载了 %ld 字节，开始断点续传\n", local_size);
        } else {
            printf("文件不存在，开始下载文件\n");
        }
    } else {
        printf("不使用断点续传，开始下载文件\n");
    }

    curl = curl_easy_init();
    if(curl) {
        if(breakpoint) {
            fp = fopen(local, "a"); // 追加模式打开文件
        } else {
            fp = fopen(local, "w"); // 追加模式打开文件
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, server);
        // 设置进度回调函数
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ftp_download_callback);
        // 启用进度回调 (0表示启用)
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)local_size);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        
        // 设置FTP用户名和密码
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
        
        res = curl_easy_perform(curl);
        
        fclose(fp);
        
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return -1;
        }
        
        curl_easy_cleanup(curl);
    }
    printf("文件下载完成！\n");
    return 0;
}

/* 
    FTP上传文件
    server: ftp服务器地址
    local: 本地文件路径
    breakpoint: 是否支持断点续传
    return: 0 成功，-1 失败
*/
int ftp_upload_file(char *server, char *local, bool breakpoint, char *username, char *password)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    struct stat file_info;
    curl_off_t uploaded_size = 0;
    
    // 获取本地文件信息
    if(stat(local, &file_info)) {
        perror("stat failed");
        return -1;
    }
    
    if(breakpoint) {
        // 获取服务器上已上传的文件大小
        int size = ftp_get_file_size(server, username, password);
        if(size > 0) {
            uploaded_size = size;
            printf("文件存在，已经上传了 %ld 字节，开始断点续传\n", uploaded_size);
        } else {
            printf("文件不存在，开始上传文件\n");
        }
    } else {
        printf("不使用断点续传，开始上传文件\n");
    }
    
    curl = curl_easy_init();
    if(curl) {
        fp = fopen(local, "rb");
        if(!fp) {
            perror("fopen failed");
            return -1;
        }
        
        // 移动到已上传的位置
        fseek(fp, uploaded_size, SEEK_SET);
        
        curl_easy_setopt(curl, CURLOPT_URL, server);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)(file_info.st_size - uploaded_size));
        
        // 启用追加模式
        if(breakpoint) {
            curl_easy_setopt(curl, CURLOPT_APPEND, 1L);
        }

        // 设置进度回调
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ftp_upload_callback);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        
        // 设置FTP用户名和密码
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
        
        res = curl_easy_perform(curl);
        
        fclose(fp);
        
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return -1;
        }
        
        curl_easy_cleanup(curl);
    }
    printf("文件上传完成！\n");
    return 0;
}

int main(int argc, char *argv[])
{
	int c;
    char par_mode = 0;
    char server_path[128];
    char local_path[128];
    char username[32];
    char password[32];
    bool breakpoint = false;

    while ((c = getopt(argc, argv, "hbm:s:f:u:")) != -1) {
        switch (c) {
        case 'h':
            usage();
            break;
        case 'm':
            par_mode = optarg[0];
            break;
        case 's':
            strcpy(server_path, optarg);
            break;
        case 'f':
            strcpy(local_path, optarg);
            break;
        case 'u': {
            char *result = strchr(optarg, ':');
            if (result != NULL) {
                strncpy(username, optarg, result - optarg);
                username[result - optarg] = '\0';
                strcpy(password, result + 1);
            } else {
                printf("Error: invalid username and password format. \n");
                usage();
                return -1;
            }
        } break;
        case 'b':
            breakpoint = true;
            break;
        default:
            printf("Error: invalid options. \n");
            usage();
            return -1;
        }
    }

    switch (par_mode)
    {
        case 'u':
            ftp_upload_file(server_path, local_path, breakpoint, username, password);
            break;
        case 'd':
            ftp_download_file(server_path, local_path, breakpoint, username, password);
            break;
        case 'i':
            ftp_get_file_size(server_path, username, password);
            break;
        default:
            break;
    }

	return 0;
}