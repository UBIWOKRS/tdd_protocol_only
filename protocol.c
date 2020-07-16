/*********************************************//*
 * @file	protocol.h
 * @author	seoyoung(from ubiworks Co., Ltd)
 * @version Ver 0.0.1
***********************************************
* Copyright 2020(c) UBIWORKS Co., Ltd
*
* protocol & function for tdd module
* tdd module <-> computer (by serial communication : RS232)
*
* communication type : UART (115200bps, 1 Start bit, 8 Data bit, Non parity, 1 Stop bit)
*
*
*	*** message sequence diagram ***
*
*  RCU							5G TDD Module
* 	|								|
* 	|	상태조회, 제어		명령 전송		|
*	|	------------------------>	|
* 	|								|
* 	|	상태조회, 제어		명령 응답		|
*	|	<------------------------	|
*	|								|
*	|								|
*	|								|
*
*
************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ad9361_api.h"
#include "protocol.h"
// include library	by seoyoung	file location : C:\Xilinx2\Vitis\2019.2\tps\win64\msys64\mingw64\x86_64-w64-mingw32\include
#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"
#include "console.h"

// add to get register
#include "parameters.h"
#include "xil_io.h"

#if 1

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
GUIprotocol* TDDProtocol;

unsigned short	BodyLenght;
unsigned short	SUB_DATA_Length = 0;
unsigned char	CMD;

/******************************************************************************/
/************************* Function Definitions *******************************/
/******************************************************************************/


/**************************************************************************//***
 * @brief protocol member LEN[] array makes to short value
 * unsigned char Length[2] data is an array so make it to unsigned short data
 * Big Edian
 * ex. Length[0] = 0x25, Length[1] = 0x30 => result = 0x2530
 * 0x25 data make it to 0x2500 and then combine with others
 *
 * @param arr - LEN[] data (array)
 *
 * @return length - LEN data value
*******************************************************************************/
unsigned short make_length(unsigned char *arr)
{
	unsigned short result = 0;

	result = (arr[0] << 8) + arr[1];

	return result;
}


/**************************************************************************//***
 * @brief Change GUI protocol data to array data
 *
 * @param source - GUI protocol data which will be turn to array
 * @param destination - save array result
 * @param GUIprotocolSize - GUI protocol size, include real SUB_DATA length
 *
 * @return None
*******************************************************************************/
void to_array(GUIprotocol *source, char* destination, int GUIprotocolSize)
{
	int pos_now = 0;
	int array_count = 0;

	int posSourceID 		= sizeof(source->Start);
	int posLength 			= sizeof(source->Source_ID) 	+ posSourceID;
	int posDestinationID 	= sizeof(source->Length) 		+ posLength;

	int posReserved 		= sizeof(source->Destination_ID) + posDestinationID;
	int posCMD				= sizeof(source->Reserved) 	+ posReserved;
	int posRCODE			= sizeof(source->Type_CMD) 	+ posCMD;
	int posLEN				= sizeof(source->Rcode) 		+ posRCODE;

	//SUB_DATA_Length = make_length(source[posLEN]) - 'a';
	// test dataaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	SUB_DATA_Length = GUIprotocolSize - sizeof(GUIprotocol);

	int posSUB_DATA			= sizeof(source->LEN) 			+ posLEN;
	int posCRC				= SUB_DATA_Length	 			+ posSUB_DATA;

	int posEnd				= sizeof(source->CRC) 			+ posCRC;

	/* -------------------------------------------------------------------------------------------------- Length data is available to use? */
	//SUB_DATA_Length = make_length(source->LEN);

	while(pos_now < GUIprotocolSize)
	{
		array_count = 0;

		// Start[4]
		while(pos_now < posSourceID)
		{
			destination[pos_now] = source->Start[array_count];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// Source_ID
		while(pos_now < posLength)
		{
			destination[pos_now] = source->Source_ID;

			pos_now = pos_now + 1;
		}

		array_count = 0;
		// Length[2]
		while(pos_now < posDestinationID)
		{
			destination[pos_now] = source->Length[array_count];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// Destination_ID
		while(pos_now < posReserved)
		{
			destination[pos_now] = source->Destination_ID;

			pos_now = pos_now + 1;
		}

		array_count = 0;
		// Reserved[3]
		while(pos_now < posCMD)
		{
			destination[pos_now] = source->Reserved[array_count];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// Type_CMD
		while(pos_now < posRCODE)
		{
			destination[pos_now] = source->Type_CMD;

			pos_now = pos_now + 1;
		}

		// RCODE
		while(pos_now < posLEN)
		{
			destination[pos_now] = source->Rcode;

			pos_now = pos_now + 1;
		}

		array_count = 0;
		// LEN[2]
		while(pos_now < posSUB_DATA)
		{
			destination[pos_now] = source->LEN[array_count];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// SUB_DATA[0]
		array_count = 0;
		while(pos_now < posCRC)
		{
			destination[pos_now] = source->SUB_DATA[array_count];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		array_count = 0;
		// CRC[2]
		while(pos_now < posEnd)
		{
			destination[pos_now] = source->CRC[array_count];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// End
		while(pos_now < GUIprotocolSize)
		{
			destination[pos_now] = source->End;

			pos_now = pos_now + 1;
		}
	}
}


/**************************************************************************//***
 * @brief Change array data to GUI protocol data
 *
 * @param source - array which will be turn to GUI protocol data
 * @param destination - save GUI protocol data result
 * @param GUIprotocolSize - GUI protocol size, include real SUB_DATA length
 *
 * @return None
*******************************************************************************/
void to_struct(char* source, GUIprotocol* destination, int GUIprotocolSize)
{
	int pos_now = 0;
	int array_count = 0;

	int posSourceID 		= sizeof(destination->Start);
	int posLength 			= sizeof(destination->Source_ID) 	+ posSourceID;
	int posDestinationID 	= sizeof(destination->Length) 		+ posLength;

	int posReserved 		= sizeof(destination->Destination_ID) + posDestinationID;
	int posCMD				= sizeof(destination->Reserved) 	+ posReserved;
	int posRCODE			= sizeof(destination->Type_CMD) 	+ posCMD;
	int posLEN				= sizeof(destination->Rcode) 		+ posRCODE;

	//SUB_DATA_Length = make_length(source[posLEN]) - 'a';
	// test dataaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	SUB_DATA_Length = GUIprotocolSize - sizeof(GUIprotocol);

	int posSUB_DATA			= sizeof(destination->LEN) 			+ posLEN;
	int posCRC				= SUB_DATA_Length		 			+ posSUB_DATA;

	int posEnd				= sizeof(destination->CRC) 			+ posCRC;

	/* -------------------------------------------------------------------------------------------------- Length data is available to use? */
	//SUB_DATA_Length = make_length(source[posLEN]) - '0';

	while(pos_now < GUIprotocolSize)
	{
		array_count = 0;
		// Start[4]
		while(pos_now < posSourceID)
		{
			destination->Start[array_count] = source[pos_now];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// Source_ID
		while(pos_now < posLength)
		{
			destination->Source_ID = source[pos_now];

			pos_now = pos_now + 1;
		}

		array_count = 0;
		// Length[2]
		while(pos_now < posDestinationID)
		{
			destination->Length[array_count] = source[pos_now];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// Destination_ID
		while(pos_now < posReserved)
		{
			destination->Destination_ID = source[pos_now];

			pos_now = pos_now + 1;
		}

		array_count = 0;
		// Reserved[3]
		while(pos_now < posCMD)
		{
			destination->Reserved[array_count] = source[pos_now];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// Type_CMD
		while(pos_now < posRCODE)
		{
			destination->Type_CMD = source[pos_now];

			pos_now = pos_now + 1;
		}

		// RCODE
		while(pos_now < posLEN)
		{
			destination->Rcode = source[pos_now];

			pos_now = pos_now + 1;
		}

		array_count = 0;
		// LEN[2]
		while(pos_now < posSUB_DATA)
		{
			destination->LEN[array_count] = source[pos_now];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// SUB_DATA[0]
		array_count = 0;
		while(pos_now < posCRC)
		{
			destination->SUB_DATA[array_count] = source[pos_now];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		array_count = 0;
		// CRC[2]
		while(pos_now < posEnd)
		{
			destination->CRC[array_count] = source[pos_now];

			pos_now = pos_now + 1;
			array_count = array_count + 1;
		}

		// End
		while(pos_now < GUIprotocolSize)
		{
			destination->End = source[pos_now];

			pos_now = pos_now + 1;
		}
	}
}


/* send GUIprotocol struct data */
/**************************************************************************//***
 * @brief Send GUI protocol data with UART
 *
 * @param sendData - send protocol data
 * @param size - send data length
 *
 * @return None
*******************************************************************************/
void send_protocol_data(char *sendData, int size)
{
	for(int i = 0; i < size; i++)
	{
		uart_write_char(sendData[i]);

#ifdef DEBUG_PROTOCOL
		//sleep(1);
		console_print("sendData[%d] = 0x%x\n", i, sendData[i]);
#endif
	}

#ifdef DEBUG_PROTOCOL
	console_print("send complete\n");
#endif
}


/*
 * when receive data
 * to_struct
 * Check protocol
 * CMD 값에 따라서 send data function 실행
 * data send */
/**************************************************************************//***
 * @brief Get protocol and give protocol with UART
 * receive protocol data -> passing protocol -> make it to protocol -> send data
 *
 * @param recvData - receive data array
 * @param dataLength - receive data length
 *
 * @return None
*******************************************************************************/
void eco_data_protocol(char *recvData, int dataLength)
{
	GUIprotocol *recvProtocolData = (GUIprotocol*)malloc(sizeof(GUIprotocol) + sizeof(char) * (dataLength - sizeof(GUIprotocol)));

	to_struct(recvData, recvProtocolData, dataLength);

	//print_protocol(recvProtocolData, dataLength);

	char send_sub[MAXSUB];

	unsigned int sub_size = check_protocol(recvProtocolData, send_sub);

	GUIprotocol *sendProtocolData = (GUIprotocol*)malloc(sizeof(GUIprotocol) + sizeof(char) * sub_size);

#ifdef DEBUG_PROTOCOL
	console_print("\n sendProtocolData making complete\n");
#endif

	make_protocol_data(recvProtocolData, sendProtocolData, send_sub, sub_size);

	dataLength = sizeof(GUIprotocol) + sub_size;

#ifdef DEBUG_PROTOCOL
	console_print("\n send protocol data out\n");
#endif

	print_protocol(sendProtocolData, dataLength);

	// ************************************ send ************************************
	char sendData[MAXDATA];

	to_array(sendProtocolData, sendData, dataLength);

#ifdef DEBUG_PROTOCOL
	sendData[dataLength-1] = '\0';
	console_print("send data(sendData string) = %s\n", sendData);
#endif

	send_protocol_data(sendData, dataLength);

#ifdef DEBUG_PROTOCOL
	console_print("\n send protocol data complete\n\n");
#endif

	free(recvProtocolData);
	free(sendProtocolData);
}

/**************************************************************************//***
 * @brief Testing eco, receive and send data with UART
 *
 * @param recvData - receive array data
 * @param size - receive data size
 *
 * @return None
*******************************************************************************/
void recv_test(char *recvData, int size)
{
	GUIprotocol *recvProtocolData = (GUIprotocol*)malloc(sizeof(GUIprotocol) + sizeof(char) * (size - sizeof(GUIprotocol)));
	to_struct(recvData, recvProtocolData, size);

	print_protocol(recvProtocolData, size);

	char destination[MAXDATA];

	to_array(recvProtocolData, destination, size);

	send_protocol_data(destination, size);

	destination[size-1] = '\0';
	console_print("send data(destination string) = %s\n", destination);

	free(recvProtocolData);
}

/* protocolData = make_protocol(data);
 * array = to_array(protocolData);
 * send(array); */
/*
void send_data()
{
}
*/


/**************************************************************************//***
 * @brief Making GUIprotocol data, following GUI protocol with SUB_DATA
 *
 * @param recvProtocolData - receive GUI protocol data
 * @param sendProtocolData - send GUI protocol data
 * @param send_SUB - send SUB_DATA
 * @param sub_size - SUB_DATA length
 *
 * @return None
*******************************************************************************/
void make_protocol_data(GUIprotocol* recvProtocolData, GUIprotocol* sendProtocolData, char* send_SUB, unsigned int sub_size)
{
	/* setting START */
	for(int i = 0; i < sizeof(sendProtocolData->Start); i++)
	{
		sendProtocolData->Start[i] = START;
	}

#ifdef DEBUG_PROTOCOL
	console_print("\n Start OK \n");
#endif

	/* setting Information */
	/* swap source ID <-> destination ID */
	sendProtocolData->Source_ID = recvProtocolData->Destination_ID;
	sendProtocolData->Destination_ID = recvProtocolData->Source_ID;

	sendProtocolData->Length[0] = NONE;
	sendProtocolData->Length[1] = NONE;

	/* setting Body */
	/* reserverd data set as 'x' : unknown data */
	for(int i = 0; i < sizeof(sendProtocolData->Reserved); i++)
	{
		sendProtocolData->Reserved[i] = UNKNOWN;
	}

#ifdef DEBUG_PROTOCOL
	console_print("\n Reserved OK \n");
#endif

	sendProtocolData->Type_CMD = CMD;
	sendProtocolData->Rcode = RCODE;		//	fix data 0x00

	sendProtocolData->LEN[1] = 0x00;
	sendProtocolData->LEN[0] = 0x02;

	for(int i = 0; i < sub_size; i++)
	{
		sendProtocolData->SUB_DATA[i] = send_SUB[i];
	}

#ifdef DEBUG_PROTOCOL
	console_print("\n SUB_DATA OK \n");
#endif

	for(int i = 0; i < sizeof(sendProtocolData->CRC); i++)
	{
		sendProtocolData->CRC[i] = UNKNOWN;
	}

#ifdef DEBUG_PROTOCOL
	console_print("\n CRC OK \n");
#endif

	/* setting End */
	sendProtocolData->End = END;

}


/**************************************************************************//***
 * @brief Making sameple GUIprotocol data for test
 * if receive GUIprotocol data like bellow...
 *
 * @param data - sample receive data (call by address)
 *
 * @return None
*******************************************************************************/
void test_make_protocol_data(char* data)
{
	int count = 0;

	/* setting START */
	for(int i = 0; i < sizeof(TDDProtocol->Start); i++)
	{
		data[count] = START;
		count = count + 1;
	}

	/* setting Information */
	// Source ID
	data[count] = 0x41;		//	'A'
	count = count + 1;

	// Length
	for(int i = 0; i < sizeof(TDDProtocol->Length); i++)
	{
		data[count] = NONE;
		count = count + 1;
	}

	// Destination ID
	data[count] = 0x61;		//	'a'
	count = count + 1;

	/* setting Body */
	/* reserverd data set as 'x' : unknown data */
	// Reserved
	for(int i = 0; i < sizeof(TDDProtocol->Reserved); i++)
	{
		data[count] = UNKNOWN;
		count =count + 1;
	}

	// CMD
	data[count] = status;
	count = count + 1;

	// RCODE
	data[count] = RCODE;		//	fix data 0x00
	count = count + 1;

	// LEN
	data[count] = 0x00;
	count = count + 1;
	data[count] = 0x02;
	count = count + 1;

	// SUB_DATA
	data[count] = 0x00;
	count = count + 1;
	data[count] = 0x21;
	count = count + 1;

	// CRC
	for(int i = 0; i < sizeof(TDDProtocol->CRC); i++)
	{
		data[count] = UNKNOWN;
		count = count + 1;
	}

	/* setting End */
	data[count] = END;
}


/* passing protocol data */
/* @return type : unsigned int
 * @return value : sub data size
 *
 * @function work : checking protocol, find protocol error */
/**************************************************************************//***
 * @brief Checking the error & Passing GUIprotocol data
 *
 * @param receiveData - receive GUIprotocol data
 * @param send_SUB - SUB_DATA that wants to send UART
 *
 * @return SUB_DATA size
*******************************************************************************/
unsigned int check_protocol(GUIprotocol *receiveData, char* send_SUB)
{
#ifdef DEBUG_PROTOCOL
	console_print("\n\n check protocol function\n");
#endif

	// passing data error check
	unsigned int sub_data_size = 0;

	/* passing START data */
	// check start data
	for(int i = 0; i < sizeof(receiveData->Start); i++)
	{
		if(receiveData->Start[i] == 0x16)
		{
			// OK
		}
		else
		{
			// receiveData Start data error!
			ErrorDisplay("Start");
		}
	}

#ifdef DEBUG_PROTOCOL
	console_print("\n Start check\n");
#endif

	/* passing Information data */
	// check source ID
	if(receiveData->Source_ID)
	{

	}

	// need! change to hex data
	BodyLenght = make_length(receiveData->Length);

	// check destination ID
	if(receiveData->Destination_ID)
	{

	}

	/* passing Body data */
	// check reserved
	for(int i = 0; i < sizeof(receiveData->Reserved); i++)
	{
		if(receiveData->Reserved[i])
		{

		}
	}

#ifdef DEBUG_PROTOCOL
	console_print("\n reserved OK \n");
#endif

	// check LEN
	SUB_DATA_Length = make_length(receiveData->LEN);
	if(SUB_DATA_Length == 2)
	{
		// OK
	}
	else
	{
		ErrorDisplay("SUB Data Length Too Short!!!");
	}

	// check SUB_DATA
	char temp_SUB[MAXSUB];

	// make sub data to short data ====================================================================================
	/* unsigned char Length[2] data is an array so make it to unsigned short data
	 * Big Edian
	 * ex. Length[0] = 0x25, Length[1] = 0x30 => result = 0x2530
	 * 0x25 data make it to 0x2500 and then combine with others*/
	unsigned short result = make_length(receiveData->SUB_DATA);
	/*for(int i = 0; i < sizeof(receiveData->SUB_DATA)-1; i++)
	{
		result = (receiveData->SUB_DATA[i] << 8) + result;
	}*/

	switch(receiveData->Type_CMD)
	{
	case status :
		// MAIN 상태조회 명령 / 응답
		// SUB_DATA에 년/월/일/시/분/초 6바이트 시간정보 추가하여 전송

		sub_data_size = main_status_response(result, temp_SUB);

#ifdef DEBUG_PROTOCOL
		console_print("\n sub_data_size = %d\n", sub_data_size);
#endif

		for(int i = 0; i < sub_data_size; i++)
		{
			*(send_SUB + i) = temp_SUB[i];
		}

		CMD = status;

		break;

	case control :
		// MAIN 제어 명령 / 응답
		//main_control();

		CMD = control;

		break;

	case downloadMessage :
		// 중계기 다운로드 1, 2차 통보
		//tdd_download_alarm();

		CMD = downloadMessage;

		break;

	case downloadMessageCheck :
		// 중계기 다운로드 통보 확인
		//tdd_download_check();

		CMD = downloadMessageCheck;

		break;

	case downloadData :
		// 중계기 다운로드 DATA
		//tdd_download();

		CMD = downloadData;

		break;

	case downloadDataCheck :
		// 중계기 다운로드 FPGA Download 확인
		//tdd_downloadDataCheck();

		CMD = downloadDataCheck;

		break;

	case error :
		// 오류 응답

		CMD = error;

		break;

	default:
		// CMD Data ERROR
		ErrorDisplay("CMD");
		break;
	}


	// check RCODE
	if(receiveData->Rcode == 0x00)
	{
		// 소형 장비
		console_print("device type = small device\n");
	}
	else if(receiveData->Rcode == 0x01)
	{
		// 초소형 장비
		console_print("device type = minimal device\n");
	}
	else
	{
		// RCODE data error!
		ErrorDisplay("RCODE");
	}

	// check CRC
	// frame 전송 오류 확인 용, CRC-CCITT 사용
	// 초기 Seed value = 0
	// Reserved ~ SUB_DATA 검사 영역
	// SYNC 필드와 CRC 필드를 제외한 영역 대상 CRC-CCITT 16 : 생성다항식x16 + x12 + x5 + 1(0x1021), 초기치 0

	//check_CRC();

	/* passing END data */
	// check End
	if(receiveData->End == 0xf5)
	{
		// OK
	}
	else
	{
		// END data error!
		ErrorDisplay("END");
	}

	// SUB-DATA Length data send to make protocol function

	return sub_data_size;
}


/* ***************************************************
 * 상태 조회와 응답 설명
 *
 * 상태 조회는 PC에서 중계기의 현재 상태를 파악하기 위해 사용
 * 상태 조회 명령을 받은 장비는 자신의 현재 상태를 응답 프레임에 표시하여 응답
 * ****************************************************/

/* when send data CMD = 0x00
 *
 * 상태 조회
 * 조회 명령시 SUB_DATA에 "년/월/일/시/분/초" 6바이트 시간 정보를 추가 하여 전송 -> 시간 정보는 외부에서 처리 여기서 안함 */
/*
void main_status_confirm()
{}
*/

/* when get CMD = 0x00
 * returnData = SUB_DATA
 * 상태 응답 */
/*
 * @return data		SUB_DATA Size
 * */
/**************************************************************************//***
 * @brief When get CMD = 0x00 data, send response
 * Making SUB_DATA to response
 *
 * @param sub_data - SUB_DATA of protocol member
 * @param send_sub_data - array data that you want to send as SUB_DATA
 *
 * @return SUB_DATA size
*******************************************************************************/
unsigned int main_status_response(unsigned short sub_data, char* send_sub_data)
{
#ifdef DEBUG_PROTOCOL
	console_print("\n main status response Start \n");
	console_print("\n sub_data = 0x%x\n\n", sub_data);
#endif

	// [*(send_sub_data + 1)][*send_sub_data] = [0x41][0x43] => 0x4143
	unsigned int sub_dataSize = 1;
	double Peak_Stay = 0;
	int result_avg_power = 0;

	switch(sub_data)
	{
	/* Repeater ETC : Group 000 ~ 004 */
	case 0x00 :
		// F/W Version No
		// Version 2.0
		*(send_sub_data+1) = 0x00;
		*(send_sub_data) = 0x20;

		sub_dataSize = 2;

		break;

	case 0x01 :
		// FPGA Version No
		// Version 2.0
		*(send_sub_data+1) = 0x00;
		*(send_sub_data) = 0x20;

		sub_dataSize = 2;

		break;

	case 0x02 :
	case 0x03 :
	case 0x04 :
		//Reserve

		break;

	/* Alarm :  Group 005 ~ 006 */
	case 0x05 :
		// Normal : 0, Alarm : 1
		// 0bit : T-Sync Alarm, 2bit ~ 7bit = Reserve

		break;

	case 0x06 :
		// 0~7bit = Reserve

		break;

	/* control (ON / OFF) : Group 007 ~ 009 */
	case 0x07 :
	case 0x08 :
	case 0x09 :
		// 0 ~ 7 bit = Reserve

		break;

	/* Status : Group 010 ~ 026 */
	case 0x10 :
	case 0x11 :
		/* Power Detec Power
		 * signed short (0.1 dBm Step)
		 * 무신호 / 테이블 보다 낮은 레벨일 경우 -100, GUI는 NA 표시
		 * EX) 0.1dBm -> 0x0001, -0.1dBm -> 0xFFFF (Big-Edian)*/

		break;

	case 0x12 :
	case 0x13 :
		/* Slot 비율
		 * unsigned short(단위 : ???) */

		break;

	case 0x14 :
		/* Band Select
		 * 0x00 : 80M
		 * 0x01 : 90M
		 * 0x02 : 100M */

		break;

	case 0x15 :
		// Reserve

		break;

	/*******************************************************************************************************************************/// Highlight
	case 0x16 :
	case 0x17 :
		/* T Sync-0 출력 신호 Delay (front)
		 * unsinged short (단위 : 1ns) */

		break;

	case 0x18 :
	case 0x19 :
		/* T Sync-1 출력 신호 Delay (front)
		 * unsinged short (단위 : 1ns) */

		break;

	case 0x20 :
	case 0x21 :
		/* T Sync-2 출력 신호 Delay (front)
		 * unsinged short (단위 : 1ns) */

		sub_dataSize = 2;

		// 소방과제 : KT(1755) Power Det

		Peak_Stay = 10 * log(10*Xil_In32(GPIO_PEAK_PWR_ST + 0x08));
		console_print("Keep Peak Power = %f dBm\n", (Peak_Stay-193));

		result_avg_power = Peak_Stay / DIV;
		console_print("\n result_avg_power = %d \n", result_avg_power);

		// set power_dBm
		*(send_sub_data+1) = result_avg_power / BYTE_1;
		*(send_sub_data) = result_avg_power % BYTE_1;

		break;
	/************************************************************************************************************************/// Highlight end

	case 0x22 :
		/* TDD MODE
		 * 0x00 : DL, 0x01 : UL, 0x02 : AUTO
		 * T Sync 신호 출력 고정으로 제어(MCU에서 DL로 명령시 High 고정출력, UL로 명령시 Low 고정 출력) */

		break;

	case 0x23 :
		/* 장비 구분
		 * 0 : 소형, 1 : 초소형 */

		break;

	case 0x24 :
	case 0x25 :
	case 0x26 :
		// Reserve

		break;

	/* Level : Group 027 ~ 044 */
	case 0x27 :
	case 0x28 :
		/* DL - UL - Transmission Periodicity
		 * unsigned short (단위 : ???) */

		break;

	case 0x29 :
	case 0x30 :
		/* nrofDownlinkSlots
		 * unsigned short (단위 : ???) */

		break;

	case 0x31 :
	case 0x32 :
		/* nrofDownSymbols
		 * unsigned short (단위 : ???) */

		break;

	case 0x33 :
	case 0x34 :
	case 0x35 :
	case 0x36 :
		/* 중심 주파수 설정
		 * unsigned int (단위 : ???) */

		break;

	case 0x37 :
		/* 대역폭 설정
		 * unsigned char (단위 : ???) */

		break;

	/*******************************************************************************************************************************/// Highlight
	case 0x38 :
	case 0x39 :
		/* T Sync-0 출력 신호 Delay (back)
		 * unsinged short (단위 : 1ns) */

		break;

	case 0x40 :
	case 0x41 :
		/* T Sync-1 출력 신호 Delay (back)
		 * unsinged short (단위 : 1ns) */

		break;

	case 0x42 :
	case 0x43 :
		/* T Sync-2 출력 신호 Delay (back)
		 * unsinged short (단위 : 1ns) */

		break;
	/***************************************************************************************************************************/// Highlight end

	default :
		// Reserve

		break;
	}

	return sub_dataSize;
}


 /* 제어명령 및 제어응답 설명
 *
 * 제어는 필요시 PC에서 중계기의 각종 설정치 등을 변경
 * 제어 명령 프로임에는 제어하는 항목만 중계기 Parameter 형식으로 처리
 * 여러 제어 항목에 대하여 하나의 제어명령으로 처리 가능
 * 중계기에서는 중계기 Parameter 형식의 데이터를 분석하여 해당 항목의 값을 해당 data로 변경하고 응답
 * 제어가 실패하거나 data가 설정 불가능한 경우 오류 응답으로 처리 */
/* when send data CMD = 0x01
 * 제어 명령 */
/**************************************************************************//***
 * @brief When get CMD = 0x01; control command then response data
 *
 * @param sub_data - SUB_DATA of protocol member
 *
 * @return None
*******************************************************************************/
void main_control(unsigned short sub_data)
{
	switch(sub_data)
	{
	case 0x00 :
		/* 0 : Slot 비율 					=> 1 : 변경, 0 : 유지
		 * 1 : Band Select 				=> 1 : 변경, 0 : 유지
		 * 2 : TDD MODE					=> 1 : 변경, 0 : 유지
		 * 3 : T Sync-0 출력 신호 Delay(앞)	=> 1 : 변경, 0 : 유지
		 * 4 : T Sync-1 출력 신호 Delay(앞)	=> 1 : 변경, 0 : 유지
		 * 5 : T Sync-2 출력 신호 Delay(앞)	=> 1 : 변경, 0 : 유지
		 * 6 : 장비구분					=> 1 : 변경, 0 : 유지
		 * 7 : Reserve					=> 1 : 변경, 0 : 유지  */

		break;

	case 0x01 :
	case 0x02 :
		/* Slot 비율
		 * unsigned short (단위 : ???) */

		break;

	case 0x03 :
		/* Band Select
		 * 0x00 : 80M
		 * 0x01 : 90M
		 * 0x02 : 10M */

		break;

	case 0x04 :
		/* TDD MODE
		 * 0x00 : DL, 0x01 : UL, 0x02 : AUTO
		 * T Sync 신호 출력 고정으로 제어 (MCU에서 DL로 명령시 High 고정출력, UL로 명령시 Low 고정 출력) */

		break;

	case 0x05 :
	case 0x06 :
		/* T Sync-0 출력 신호 Delay(앞)
		 * unsigned short (단위 : 1ns) */

		break;

	case 0x07 :
	case 0x08 :
		/* T Sync-1 출력 신호 Delay(앞)
		 * unsigned short (단위 : 1ns) */

		break;

	case 0x09 :
	case 0x10 :
		/* T Sync-2 출력 신호 Delay(앞)
		 * unsigned short (단위 : 1ns) */

		break;

	case 0x11 :
		/* 장비구분 0 : 소형 , 1 : 초소형 */

		break;

	case 0x13 :
		/* 0 : DL-UL-Transmission Periodicity		=> 1 : 변경, 0 : 유지
		 * 1 : nrofDownlinkSlots					=> 1 : 변경, 0 : 유지
		 * 2 : nrofDownSymbols						=> 1 : 변경, 0 : 유지
		 * 3 : 중심주파수 설정 							=> 1 : 변경, 0 : 유지
		 * 4 : 대역폭 설정								=> 1 : 변경, 0 : 유지
		 * 5 : Reserve
		 * 6 : Reserve
		 * 7 : Reserve */

		break;

	case 0x14 :
	case 0x15 :
		/* DL-UL-Transmission Periodicity
		 * unsigned short (단위 : ???) */

		break;

	case 0x16 :
	case 0x17 :
		/* nrofDownlinkSlots
		 * unsigned short (단위 : ???) */

		break;

	case 0x18 :
	case 0x19 :
		/* nrofDownSymbols
		 * unsigned short (단위 : ???) */

		break;

	case 0x20 :
	case 0x21 :
	case 0x22 :
	case 0x23 :
		/* 중심주파수 설정
		 * unsigned int (단위 : ???) */

		break;

	case 0x24 :
		/* 대역폭 설정
		 * unsigned char (단위 : ???) */

		break;

	case 0x28 :
		/* 0 : T Sync-0 출력 신호 Delay(뒤)		=> 1 : 변경, 0 : 유지
		 * 1 : T Sync-1 출력 신호 Delay(뒤)		=> 1 : 변경, 0 : 유지
		 * 2 : T Sync-2 출력 신호 Delay(뒤)		=> 1 : 변경, 0 : 유지
		 * 3 : Reserve						=> 1 : 변경, 0 : 유지
		 * 4 : Reserve						=> 1 : 변경, 0 : 유지
		 * 5 : Reserve						=> 1 : 변경, 0 : 유지
		 * 6 : Reserve						=> 1 : 변경, 0 : 유지
		 * 7 : Reserve						=> 1 : 변경, 0 : 유지  */

		break;

	case 0x29 :
	case 0x30 :
		/* T Sync-0 출력 신호 Delay(뒤)
		 * unsigned short (단위 : 1ns) */

		break;

	case 0x31 :
	case 0x32 :
		/* T Sync-1 출력 신호 Delay(뒤)
		 * unsigned short (단위 : 1ns) */

		break;

	case 0x33 :
	case 0x34 :
		/* T Sync-2 출력 신호 Delay(뒤)
		 * unsigned short (단위 : 1ns) */

		break;

	default:
		/* Reserve */

		break;

	}
}


/* when get data CMD = 0x01
 * 제어 응답
 * 조회 명령의 SUB_DATA를 COPY*/
/*
void main_control_response()
{
}
*/

/**************************************************************************//***
 * @brief Display ERROR data
 *
 * @param msg - message ERROR data
 *
 * @return None
*******************************************************************************/
void ErrorDisplay(char *msg)
{
	console_print("ERROR Message = %s Data ERROR\n", msg);
}


// initial protocol data
/**************************************************************************//***
 * @brief Initialize GUIprotocol data
 *
 * @param data - initial to GUIprotocol data
 * @param dataLen - GUIprotocol data length
 *
 * @return None
*******************************************************************************/
void init_protocol_data(GUIprotocol *data, int dataLen)
{
	unsigned char initData = 0x00;

	// Start[]
	for(int i = 0; i < sizeof(data->Start); i++)
	{
		data->Start[i] = 0x16;
	}

	// Source_ID
	data->Source_ID = initData;
	initData = initData + 1;

	// Length[]
	for(int i = 0; i < sizeof(data->Length); i++)
	{
		data->Length[i] = initData;
		initData = initData + 1;
	}

	// Destination_ID
	data->Destination_ID = initData;
	initData = initData + 1;

	// Reserved[]
	for(int i = 0; i < sizeof(data->Reserved); i++)
	{
		data->Reserved[i] = initData;
		initData = initData + 1;
	}

	// Type_CMD
	data->Type_CMD = initData;
	initData = initData + 1;

	// RCODE
	data->Rcode = initData;
	initData = initData + 1;

	// LEN[]
	for(int i = 0; i < sizeof(data->LEN); i++)
	{
		data->LEN[i] = initData;
		initData = initData + 1;
	}

	// SUB_DATA[]
	for(int i = 0; i < (dataLen - sizeof(GUIprotocol)); i++)
	{
		data->SUB_DATA[i] = initData;
		initData = initData + 1;
	}

	// CRC[]
	for(int i = 0; i < sizeof(data->CRC); i++)
	{
		data->CRC[i] = initData;
		initData = initData + 1;
	}

	// End
	data->End = 0xf5;

	// initial data print
	/*console_print("\n======= initial data out =======\n");
	print_protocol(data, dataLen);
	console_print("\n===== initial complete! ===== \n");*/
}


/* print protocol data to console */
/**************************************************************************//***
 * @brief GUIprotocol data print all to console; UART
 *
 * @param data - GUIprotocol data that wants to print
 * @param dataLen - GUIprotocol data length
 *
 * @return None
*******************************************************************************/
void print_protocol(GUIprotocol *data, int dataLen)
{
	// Start[]
	console_print("\nStart data out\n");
	for(int i = 0; i < sizeof(data->Start); i++)
	{
		console_print("start[%d] = 0x%x\n", i, data->Start[i]);
	}

	// Source_ID
	console_print("\nSource_ID data out\n");
	console_print("Source_ID = 0x%x\n", data->Source_ID);

	// Length[]
	console_print("\nLength data out\n");
	for(int i = 0; i < sizeof(data->Length); i++)
	{
		console_print("Length[%d] = 0x%x\n", i, data->Length[i]);
	}

	// Destination_ID
	console_print("\nDestination_ID data out\n");
	console_print("Destination_ID = 0x%x\n", data->Destination_ID);

	// Reserved[]
	console_print("\nReserved data out\n");
	for(int i = 0; i < sizeof(data->Reserved); i++)
	{
		console_print("Reserved[%d] = 0x%x\n", i, data->Reserved[i]);
	}

	// Type_CMD
	console_print("\nType_CMD data out\n");
	console_print("Type_CMD = 0x%x\n", data->Type_CMD);

	// RCODE
	console_print("\nRCODE data out\n");
	console_print("RCODE = 0x%x\n", data->Rcode);

	// LEN[]
	console_print("\nLEN data out\n");
	for(int i = 0; i < sizeof(data->LEN); i++)
	{
		console_print("LEN[%d] = 0x%x\n", i, data->LEN[i]);
	}

	// SUB_DATA[]
	console_print("\nSUB_DATA data out\n");
	for(int i = 0; i < (dataLen - sizeof(GUIprotocol)); i++)
	{
		console_print("SUB_DATA[%d] = 0x%x\n", i, data->SUB_DATA[i]);
	}

	// CRC[]
	console_print("\nCRC data out\n");
	for(int i = 0; i < sizeof(data->CRC); i++)
	{
		console_print("CRC[%d] = 0x%x\n", i, data->CRC[i]);
	}

	// End
	console_print("\nEnd data out\n");
	console_print("End = 0x%x\n", data->End);
}


#endif
