#include "ssl_aes.h"

int main(int argc, char *argv[])
{
    int user_input = 0;

    if (argc < 2 || argc > 2) {  /* 2 get sample 2 */
        printf("usage error\n");
        return -1;
    }
    if(argv[1][0] == '0') {
        user_input = 0;
    }else if(argv[1][0] == '1'){
        user_input = 1;
    }else
    {
        printf("usage error\n");
        return -1;
    }

    int ret = 0;
    ret = encrypt_func(user_input);
    if(ret == HI_SUCCESS) {
        printf("crypt success!\n");
    }

    return 0;
    

}