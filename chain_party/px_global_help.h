//
//  px_global_help.h
//  all
//
//  Created by 007 on 2022/1/19.
//

#ifndef px_global_help_h
#define px_global_help_h

#include <string.h>
#include <stdio.h>
#include <codecvt>
#include <locale>
//update on 20220114 add exe path
#include <unistd.h>
#include <mach-o/dyld.h>
#include <sys/types.h>
#include <pwd.h>
//
#include <stdarg.h>
#include <sys/stat.h>
//update on 20220329
#include "ui/gfx/geometry/rect.h"
#include "ui/views/view.h"
//update on 20220817 left panel setting param
//#define LEFT_PANEL_SETTING_MESSAGE "messenger"
//#define LEFT_PANEL_SETTING_MESSAGE_TOP "messenger_top"
//#define LEFT_PANEL_SETTING_MESSAGE_SHOW "messenger_show"
//#define LEFT_PANEL_SETTING_WHATSAPP "whatsapp"
//#define LEFT_PANEL_SETTING_WHATSAPP_TOP "whatsapp_top"
//#define LEFT_PANEL_SETTING_WHATSAPP_SHOW "whatsapp_show"
//#define LEFT_PANEL_SETTING_DISCORD "discord"
//#define LEFT_PANEL_SETTING_DISCORD_TOP "discord_top"
//#define LEFT_PANEL_SETTING_DISCORD_SHOW "discord_show"
//#define LEFT_PANEL_SETTING_DUNE "dune"
//#define LEFT_PANEL_SETTING_DUNE_TOP "dune_top"
//#define LEFT_PANEL_SETTING_DUNE_SHOW "dune_show"
//#define LEFT_PANEL_SETTING_APEBOARD "apeboard"
//#define LEFT_PANEL_SETTING_APEBOARD_TOP "apeboard_top"
//#define LEFT_PANEL_SETTING_APEBOARD_SHOW "apeboard_show"
//#define LEFT_PANEL_SETTING_NFTBANK "nftbank"
//#define LEFT_PANEL_SETTING_NFTBANK_TOP "nftbank_top"
//#define LEFT_PANEL_SETTING_NFTBANK_SHOW "nftbank_show"
//
//update
class GlobalHelp
{
public:
    //static int SUSPENDBAR_WIDTH;
    //update on 20220329
    static gfx::Rect webview_content_rect_;
    static views::View* suspend_view_;
    static char* global_html_content_;
    //
    static void CreateDir(char* path)
    {
        if(access(path,0) == -1)
            //mkdir(path,0775);
            mkdirs(path);
    }
    static int mkdirs(char *dir)
    {
        int i,len;
        char str[512] = {0};
        strcpy(str,dir);//缓存文件路径
        len = strlen(str);
        for(i = 0; i<len; i++)
        {
            if( str[i] == '/')
                {
                str[i] = '\0';
                if(access(str,0) != 0)
                    mkdir(str,0775);
                str[i] ='/';
            }
        }
        if( len > 0 && access(str ,0)!=0) //检测是否创建成功
            mkdir(str,0775);
        return 0;
    }
    static std::string GetAppRunPath()
    {
        unsigned size = 1024;
        char buffer[size];
        memset(buffer,0,size);
        if(_NSGetExecutablePath(buffer, &size)!=0)
            return "./Chromium.app/Contents/MacOS/dhdgffkkebhmkfjojejmpbldmpobfkfo/4.13_0";
        //buffer[size] = '\0';
        /*
        int cnt = readlink("/proc/self/exe", buffer, 1024);
        if(cnt <0 || cnt >= 1024)
            return NULL;
        *strrchr(buffer, '/')=0;
         */
        //getcwd(buffer, 1024);//get root path
        //getwd(buffer);
        std::string temp = buffer;
        temp = temp.substr(0,temp.rfind("/"));
        return temp;
    }
    static std::string GetUsersPath()
    {
        struct passwd* pwd = NULL;
        pwd = getpwuid(getuid());
        if(pwd == NULL)
            return "";
        std::string user = pwd->pw_name;
        std::string path = "/Users/" + user + "/Library/Application Support/PurseBrowser";
        return path;
    }
    static void WriteInfoToFile(char* path,char* text,int mode)
    {
        FILE *fp = NULL;
        if(mode == 0)//create
        fp = fopen(path, "w+");
        else if(mode ==1)//append
            fp = fopen(path,"a");
        if(fp == NULL)
            return;
        fputs(text, fp);
        fclose(fp);
    }
    static std::string GetInfoFromFile(char* path)
    {
        if(access(path,0) != 0)
            return "-1";
        FILE *fp;
            char *str;
            char txt[1000];
            int filesize;
            //打开一个文件
            if ((fp=fopen(path,"r"))==NULL){
                return "";
            }
            //将文件指针移到末尾
            fseek(fp,0,SEEK_END);
            filesize = ftell(fp);//通过ftell函数获得指针到文件头的偏移字节数。
            
            str=(char *)malloc(filesize);//动态分配str内存
        //    str=malloc(filesize);//动态分配str内存
            str[0]=0;//字符串置空
        //    memset(str,filesize*sizeof(char),0);//清空数组,字符串置空第二种用法
            rewind(fp);
            
            while((fgets(txt,1000,fp))!=NULL){//循环读取1000字节,如果没有数据则退出循环
                strcat(str,txt);//拼接字符串
            }
            fclose(fp);
            return str;
    }
    static int ByteToHexstring(unsigned char *in, int len, char *out) {
        for (int i = 0; i < len; i++) {
            if ((in[i] >> 4) >= 10 && (in[i] >> 4) <= 15)
                out[2*i] = (in[i] >> 4) + 'a' - 10;
            else
                out[2*i] = (in[i] >> 4) | 0x30;
            
            if ((in[i] & 0x0f) >= 10 && (in[i] & 0x0f) <= 15)
                out[2*i+1] = (in[i] & 0x0f) + 'a' - 10;
            else
                out[2*i+1] = (in[i] & 0x0f) | 0x30;
        }
        return 0;
    }
    static std::u16string to_utf16( std::string str ) // utf-8 to utf16
    { return std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t >{}.from_bytes(str); }
    std::string to_utf8( std::u16string str16 ) //utf-16 to utf8
    { return std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t >{}.to_bytes(str16); }
    //update on 20220725
    static bool IsEnsAddress(std::string input){
        bool result = false;
        if(input.length() < 5)
            return result;
        if(input.substr(input.length()-4,4) == ".eth")
            result = true;
        return result;
    }
    //钱包地址 eth 20byte
    static bool IsEthAddress(std::string input){
        bool result = false;
        if(input.length() < 3)
            return result;
        auto temp = input.substr(0,2);
        if( temp == "0x" && (input.length()-2) == 40)//20byte
            result = true;
        printf("\nIsHexAddress::%s\n",temp.c_str());
        return result;
    }
    //交易地址 eth
    static bool IsEthTransactionsHashAddress(std::string input){
        bool result = false;
        if(input.length() < 3)
            return result;
        auto temp = input.substr(0,2);
        if( temp == "0x" && (input.length()-2) == 64)//32byte
            result = true;
        printf("\nIsEthHashAddress::%s\n",temp.c_str());
        return result;
    }
    //btc Bech32 42 char 26<= x <=34 char
    static bool IsBtcBech32Address(std::string input){
        bool result = false;
        auto length = input.length();
        if(length < 4)
            return result;
        auto temp = input.substr(0,3);
        if( temp == "bc1" && (26 <= length && length <= 42))
            result = true;
        printf("\nIsBtcBech32Address::%s\n",temp.c_str());
        return result;
    }
    //btc P2SH 34 char 26<= x <=34 char
    static bool IsBtcP2SHAddress(std::string input){
        bool result = false;
        auto length = input.length();
        if(length < 2)
            return result;
        auto temp = input.substr(0,1);
        if( temp == "3" && (26 <= length && length <= 34))
            result = true;
        printf("\nIsBtcP2SHAddress::%s\n",temp.c_str());
        return result;
    }
    //btc P2SH 26<= x <=34 char
    static bool IsBtcP2PKHAddress(std::string input){
        bool result = false;
        auto length = input.length();
        if(length < 2)
            return result;
        auto temp = input.substr(0,1);
        if( temp == "1" && (26 <= length && length <= 34))
            result = true;
        printf("\nIsBtcP2PKHAddress::%s\n",temp.c_str());
        return result;
    }
    //btc  transactions hash  64 char
    static bool IsBtcTransactionsHashAddress(std::string input){
        bool result = false;
        if(input.length() < 3)
            return result;
        if((int)input.find_first_of("/") != -1 && input.length() == 64)//32byte
            result = true;
        printf("\nIsBtcTransactionsHashAddress::%s\n",input.c_str());
        return result;
    }
    //ipfs address suport 原始数据添加元数据封装成IPFS文件 -> 计算SHA2-256 -> 封装成multihash -> 转换成Base58
    static bool IsIpfsAddress(std::string input){
        bool result = false;
        auto length = input.length();
        if(length < 2)
            return result;
        auto temp = input.substr(0,2);
        if( temp == "Qm" && (26 <= length && length <= 46))
            result = true;
        printf("\nIsIpfsAddress::%s\n",temp.c_str());
        return result;
    }
};

#endif /* px_global_help_h */
