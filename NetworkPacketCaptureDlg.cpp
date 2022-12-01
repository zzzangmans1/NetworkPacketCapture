#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#pragma warning (disable : 4996)

/*
	2022-11-30 start

	리버싱 문제 제작

	name값과 키파일을 입력받아

	name값으로 계산 순서를 정하고

	name 값을 연산한다.

	그 값이 keyfile 값과 같으면 통과

*/
// C:\\Users\\leewonjoo\\Desktop\\file.key
uint8_t filename[100] = "C:\\Users\\leewonjoo\\Desktop\\1234.txt";

uint8_t namelengcheck(uint8_t* );
uint32_t filelengcheck(uint8_t* );
uint8_t filecalc(uint8_t*);
uint32_t getseed(uint8_t* );
uint8_t getrand();
uint8_t calc(uint8_t* );
uint8_t byteTobit(uint8_t , size_t, uint8_t*);
uint8_t bitTobyte(uint8_t, size_t, uint8_t*);

uint32_t seed = 0;						// 이름으로 만든 seed 값
size_t length = 0;						// 이름 길이
uint8_t operators[8] = { 0 };			// 연산 순서 저장
uint8_t operand1[8] = { 0 };			// 연산에 사용될 첫 번째 오퍼랜드
uint8_t operand2[8] = { 0 };			// 연산에 사용될 두 번째 오퍼랜드
uint8_t comres1[8] = { 0 };				// 내부 연산 저장될 변수
uint8_t *comres2;						// 파일 NAND 연산 저장될 변수

int main() {

	uint8_t name[8] = { 0 };

	printf("name : ");
	scanf("%s", name);

	printf("Hello\n");
	seed = getseed(name);
	size_t fsize = filelengcheck(filename);
	comres2 = (uint8_t*)malloc(sizeof(uint8_t) * fsize);
	getrand();
	calc(name);
	byteTobit(name[0], 8, comres2);
	bitTobyte(name[0], 8, comres2);
	free(comres2);
	
	return 0;
}
// 이름 길이 반환하는 함수
uint8_t namelengcheck(uint8_t * name)
{
	uint32_t i = 0;
	while (name[i++] != '\0');
	return (i - 1) >= 2 ? (i - 1) : 0;
}

// 파일 길이를 얻는 함수
uint32_t filelengcheck(uint8_t* fname)
{
	FILE* fp = fopen((char*)fname, "rb");
	if (fp == NULL) {
		printf("없는 파일입니다.\n");
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	int fleng = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("파일 길이는 : %d\n", fleng);
	
	fclose(fp);
	return fleng;
}

// 파일 내용으로 연산하는 함수
uint8_t filecalc(uint8_t*)
{

}

// 이름으로 시드값 얻는 함수
uint32_t getseed(uint8_t* name)
{
	length = namelengcheck(name);
	uint32_t retseed = 0;
	if (length >= 2) printf("문자열 길이 : %d 입니다.\n",length);
	else {
		printf("2 미만입니다.\n");
		return 0;
	}
	for (size_t i = 0; i < length; i++)
	{
		retseed += name[i];
	}
	printf("retseed : %d\n", retseed);
	return retseed;
}

// 난수 얻는 함수
uint8_t getrand()
{
	srand(seed);
	uint8_t idx = 0;
	for (size_t i = 0; i < length; i++)
	{
		operators[i] = rand() % 4;
		operand1[i] = rand() % length;
		operand2[i] = rand() % length;
		printf("order : %d\n", operators[i]);
		printf("index : %d\n", operand1[i]);
		printf("index : %d\n", operand2[i]);
	}
	return 0;
}

// 연산 하는 함수
uint8_t calc(uint8_t* name)
{
	printf("res : ");
	for(size_t i =0;i<length;i++)
	{
		uint8_t op1 = operand1[i];
		uint8_t op2 = operand2[i];

		switch (operators[i])
		{
		case 0:
			comres1[i] = name[op1] + name[op2];
			printf("%X + %X = %X[%d]\n", name[op1], name[op2], comres1[i], i);
			break;
		case 1:
			comres1[i] = name[op1] - name[op2];
			printf("%X - %X = %X[%d]\n", name[op1], name[op2], comres1[i], i);
			break;
		case 2:
			comres1[i] = name[op1] * name[op2];
			printf("%X * %X = %X[%d]\n", name[op1], name[op2], comres1[i], i);
			break;
		case 3:
			comres1[i] = name[op1] ^ name[op2];
			printf("%X ^ %X = %X[%d]\n", name[op1], name[op2], comres1[i], i);
			break;
		}
	}

	for (size_t i = 0; i < length; i++)
		printf("%02X ", comres1[i]);
	return 0;
}

// byte -> bit 변경하는 함수
uint8_t byteTobit(uint8_t byte, size_t length, uint8_t* bit)
{
	//printf("\n");
	while (length--) bit[length] = (byte >> (7 - length)) & 1;
	//for (int i = 0; i < 8; i++) printf("%d", bit[i]);
	//printf("\n");
	return 0;
}

// bit -> byte 변경하는 함수
uint8_t bitTobyte(uint8_t byte , size_t length, uint8_t* bit)
{
	while (length--) byte |= bit[length] << (7 - length);
	// printf("%c  = %X\n",byte, byte);
	return 0;
}
