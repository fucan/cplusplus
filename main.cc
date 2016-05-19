#include <time.h>
#include <stdio.h>
typedef struct structa {
	int a;
	int b;
	char c[10000];
}A;

void swap1(A *left,A *right)
{
	A* temp=left;
	left=right;
	right=temp;
}

void swap2(A &left,A &right)
{
	A temp=left;
	left=right;
	right=temp;
}

int main()
{
	A left,right;
	while (1) {
		struct timespec time_start={0,0},time_end={0,0};
	clock_gettime(CLOCK_REALTIME,&time_start);
	for (int i=0;i<5000;i++) {
		swap2(left,right);
	}
	clock_gettime(CLOCK_REALTIME,&time_end);
	printf("start time %llus,%llu ns\n",time_start.tv_sec,time_start.tv_nsec);
	printf("end time %llus,%llu ns\n",time_end.tv_sec,time_end.tv_nsec);
	printf("duration:%lluns\n",((time_end.tv_sec-time_start.tv_sec)*1000000000+time_end.tv_nsec-time_start.tv_nsec));

	clock_gettime(CLOCK_REALTIME,&time_start);
	for (int i=0;i<5000;i++) {
		swap1(&left,&right);
	}
	clock_gettime(CLOCK_REALTIME,&time_end);
	printf("start time %llus,%llu ns\n",time_start.tv_sec,time_start.tv_nsec);
	printf("end time %llus,%llu ns\n",time_end.tv_sec,time_end.tv_nsec);
	printf("duration:%lluns\n",((time_end.tv_sec-time_start.tv_sec)*1000000000+time_end.tv_nsec-time_start.tv_nsec));
}	
}