01.LED
	mcu 선택
	클럭을 RCC 외부 크리스털 선택
	LED 출력을 위한 GPIO 설정
	다운로더/디버거 - 2 Wire
	rtos 태스크에서 1초 LED 점멸

02.UART
	uart2 설정 읽기/쓰기
	printf 함수 래핑
	scanf 함수 래핑
	*스레드에서는 printf 동작되지 않음.(malloc 함수에 의한 stack 문제라고 함) 

	참조: https://memories.tistory.com/111

03.UART-DMA-RX
	목적
	: uart rx를 DMA를 통해서 수신하고자 함.
	
	확인
	: rtos는 스레드가 돌아가고 있기 때문에, 스레드 안에서 DMA 레지스터에 데이터 갯수를 polling 방식으로 확인해서
	  수신 데이터가 있으면, 수신 버퍼로 데이터를 복사하는 방식.
	  USART2_IRQHandler() 인터럽트는 발생하지 않았음.
	  테러텀에서 데이터를 8개를 보내면 HAL_DMA_IRQHandler() 함수에서 인터럽트 발생함.
	  스레드에서 printf 사용하면 hang 걸리는 현상이 있어서 uart 함수를 직접 호출해서 송신 데이터를 보내는 방식으로 변경
	
	참조
	: https://github.com/MaJerle/stm32-usart-uart-dma-rx-tx/blob/main/projects/usart_rx_idle_line_irq_rtos_F4/Src/main.c#L309

04.LED-3-BLINK
	목적
	: default 테스크에서 동작이 계속됨을 확인하기 위해서 LED 3색 점멸하도록 함
	
05.UART-DMA-MSGQ-TASK
	목적
	: 태스크에서 uart dma 수신 데이터를 polling으로 처리하지 않고, dma 인터럽트가 발생하면 메시지를 전송해서 task 안에서 uart 데이터를 수신하는 처리 
	
	확인 
	: DMA Half Transfer 또는 Complete Transfer 인터럽트가 발생하면 메시지 송출.
	  태스크에서 메시지 수신되면 DMA 버퍼에서 수신된 데이터 갯수만큼 다시 송신 처리
	
04.UartThreadReadWrite
	PUTCHAR_PROTOTYPE
	GETCHAR_PROTOTYPE
	UART RX DMA Circular 설정
