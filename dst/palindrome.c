#include<stdio.h>

void reverse(char *s, int len) {
    int i,j;
    char temp;
    for(i=0,j=len-1;i<j;i++,j--) {
        temp=s[i];
        s[i]=s[j];
        s[j]=temp;
    }
}

int toString(int n, char *s) {
    int len=0;
    for(;n;n/=10) {
        s[len++] = n % 10;
    }
    reverse(s,len);
    return len;
}

int isPalindrome(char *s, int len) {
    int i,j;
    for(i=0,j=len-1;i<j;i++,j--) {
        if(s[i]!=s[j]) {
            return 0;
        }
    }
    return 1;
}

int main()
{
	int n;
	scanf("%d",&n);
	char s[10];
	int len = toString(n,s);
	if(isPalindrome(s,len)){
		printf("Y");
	}else{
		printf("N");
	}
	return 0;
}
