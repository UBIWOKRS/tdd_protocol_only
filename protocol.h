/*********************************************//*
 * @file	protocol.h
 * @author	seoyoung(from ubiworks Co., Ltd)
 * @version Ver 0.0.1
***********************************************
* Copyright 2020(c) UBIWORKS Co., Ltd
*
* protocol & function for tdd module
* tdd module <-> computer (by serial communication : RS232)
************************************************/




#ifndef	__PROTOCOL_H__
#define	__PROTOCOL_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define NULL		((void *)0)
#define SUCCESS		0
#define ERROR		-1

#define UNKNOWN 'x'
#define NONE 'z'
#define START 0x16
#define RCODE 0x00
#define END 0xF5

#define MAXSUB 10

#define MAXDATA 20

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct	protocol
{
	// Type = START
	unsigned char	Start[4];			//	send 0x16 0x16 0x16 0x16, start of Frame, 4Byte

	// Type = Information
	unsigned char	Source_ID;			//	(not use), 송신측 주소, 1Byte
	unsigned char	Length[2];			//	0x0000 ~ 0xFFFF, Body의 Length 256Byte인 경우 '0x100'로 표시(Big-Edian) Body의 Data길이(CRC포함), 2Byte
	unsigned char	Destination_ID;		//	수신측 주소, 1Byte

	// Type = Body
	unsigned char	Reserved[3];		//	??? unknown data, 3Byte
	unsigned char	Type_CMD;			//	Frame type, 1Byte
	unsigned char	Rcode;				//	send 0x00(Fix data), not use, 1Byte
	unsigned char	LEN[2];				//	0x0000 ~ 0xFFFF, Big-Edian (SUB_DATA length), 2Byte
	//	SUB_DATA put in here
	unsigned char	CRC[2];				//	check error, CRC-CCITT16, 2Byte

	// Type = End
	unsigned char	End;				//	send 0xF5 end of Frame, 1Byte

	unsigned char	SUB_DATA[0];		//	Data, 가변 struct
}GUIprotocol;

/*
*	ERROR Response
*	TYPE : 0xFF, LEN : 0x01
*
*	SUB_DATA	명칭				비고
*
*	0x01		CRC 불일치		CRC ERROR
*	0x02		무효 프레임			정의되어 있지 않은 프레임
*	0x03		범위 초과			SUB_ID 범위 초과
*	0x04		장치 없음			해당 장치가 존재하지 않음
*	0x05		Boot-Rom		중계기의 Boot-Rom 상태
*	0x06		Download 중		다른 운용자가 다운로드를 진행중인 상태
*	0x07		Link Error		Reserved */
typedef enum {crc = 0x01, notDefine, overflowSUB, noDevice, BootRom, download, link} errorType;

/* CMD Type
 * status 				= 0x00 : MAIN 상태조회 명령 / 응답
 * control 				= 0x01 : MAIN 제어 명령 / 응답
 *
 * downloadMessage 		= 0x0B : FPGA Download 1, 2차 통보
 * downloadMessageCheck = 0x0C : FPGA Download 통보 확인
 * downloadData 		= 0x0D : FPGA Download Data
 * downloadDataCheck 	= 0x0E : FPGA Download 확인
 *
 * error 				= 0xFF : 오류 응답 */
typedef enum {status = 0x00, control, downloadMessage = 0x0B, downloadMessageCheck, downloadData, downloadDataCheck, error = 0xFF} cmdType;

/*
 * how to use struct memory allocation : look bellow
 * GUIprotocol*	p = (protocol*)malloc(sizeof(protocol) + sizeof(char) * size);
 * size : last data lenght
 * */

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

unsigned short make_length(unsigned char *arr);

/* char array -> struct */
void to_struct(char* source, GUIprotocol* destination, int GUIprotocolSize);

/* struct -> char array */
void to_array(GUIprotocol *source, char* destination, int GUIprotocolSize);

void send_protocol_data(char *sendData, int size);

unsigned int main_status_response(unsigned short sub_data, char* send_sub_data);

unsigned int check_protocol(GUIprotocol *receiveData, char* send_SUB);

void make_protocol_data(GUIprotocol* recvProtocolData, GUIprotocol* sendProtocolData, char* send_SUB, unsigned int sub_size);

void main_control(unsigned short sub_data);

void ErrorDisplay(char *msg);

void recv_test(char *recvData, int size);

void eco_data_protocol(char *recvData, int dataLength);

void init_protocol_data(GUIprotocol *data, int dataLen);
void print_protocol(GUIprotocol *data, int dataLen);

void crc_check();

void test_make_protocol_data(char* data);

#endif	//	__PROTOCOL_H__
