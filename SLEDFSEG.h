#pragma once
#include "PinInterface.h"
#include <cstddef>
#include <__type_traits/is_same.h>
#include <__type_traits/conditional.h>

#define segA 0x1
#define segB 0x2
#define segC 0x4
#define segD 0x8
#define segE 0x10
#define segF 0x20
#define segG 0x40

#define segDP 0x80

#define segD1 0x100
#define segD2 0x200
#define segD3 0x400
#define segD4 0x800

class NullType{};

template<class TPin, uint8_t POSITION>
struct PW{
	using Pin = TPin;
	static constexpr uint8_t Position = POSITION;
	static constexpr uint16_t PinListMask = 1 << Position;
};
	
template <class... Ts>
struct Typelist
{};
	
// Контейнер для последовательности индексов
template<size_t... Ints>
struct index_sequence {};

// Вспомогательная структура для генерации последовательности
template<size_t N, size_t... Next>
struct make_index_sequence_helper : public make_index_sequence_helper<N - 1, N - 1, Next...> {};

// Специализация для остановки рекурсии (когда N дошло до 0)
template<size_t... Next>
struct make_index_sequence_helper<0, Next...>
{
    using type = index_sequence<Next...>;
};

// Удобный alias
template<size_t N>
using make_index_sequence = typename make_index_sequence_helper<N>::type;

template<uint8_t StartOffset, typename TIndexSeq, typename... Ts>
struct MakePinListImpl;

template<uint8_t StartOffset, size_t... Is, typename... Ts>
struct MakePinListImpl<StartOffset, index_sequence<Is...>, Ts...>
{
	using Result = Typelist< PW<Ts, static_cast<uint8_t>(StartOffset + Is)>... >;
};

template<uint8_t StartPosition, class... Pins>
struct MakePinList{
	using Indicies = make_index_sequence<sizeof...(Pins)>;
	
	using Result = typename MakePinListImpl<StartPosition, Indicies, Pins...>::Result;
};

// Шаблон для дополнения линий соответствующими портами
template <class TList> struct GetPorts;

// Шаг шаблона
template <class... Pins>
struct GetPorts< Typelist<Pins...> >
{
	using Result = Typelist< typename Pins::Pin::Port... >;
};

template <class... Lists> struct Cat;

template <class... Ts, class... Us>
struct Cat<Typelist<Ts...>, Typelist<Us...>> {
    using Result = Typelist<Ts..., Us...>;
};

template <class PortList, class T> struct Erase;

template <class T>
struct Erase<Typelist<>, T>{
	using Result = Typelist<>;
};

template <class T, class... Tail>
struct Erase< Typelist<T, Tail...>, T >
{
	using Result = Typelist<Tail...>;
};

template <class Head, class... Tail, class T>
struct Erase< Typelist<Head, Tail...>, T >
{
	using Result = typename Cat<
		Typelist<Head>, 
		typename Erase<Typelist<Tail...>, T>::Result 
	>::Result;
};

template <class TList> struct NoDuplicates;

template <>
struct NoDuplicates< Typelist<> >
{
	using Result = Typelist<>;
};

template <class Head, class... Tail>
struct NoDuplicates< Typelist<Head, Tail...> >
{
	private:
		using L1 = typename NoDuplicates< Typelist<Tail...> >::Result;
		using L2 = typename Erase<L1, Head>::Result;
	public:
		using Result = typename Cat<Typelist<Head> , L2>::Result;
};

template <class PinList, class TargetPort> struct GetPinsWithPort;

template <class TargetPort>
struct GetPinsWithPort< Typelist<>, TargetPort>
{
	using Result = Typelist<>;
};

template <class CurrentPin, class... RestPinList, class TargetPort> 
struct GetPinsWithPort< Typelist<CurrentPin, RestPinList...>, TargetPort>
{
	private:
		using CurrentPort = typename CurrentPin::Pin::Port;
		using TailResult = typename GetPinsWithPort<
			Typelist<RestPinList...>, TargetPort
		>::Result;
		
		static constexpr bool isMatch = std::is_same<CurrentPort, TargetPort>::value;
		
	public:
		using Result = typename std::conditional<
			isMatch,
			typename Cat< Typelist<CurrentPin>, TailResult>::Result,
			TailResult
		>::type;
};

template <class TList> struct GetPortMask;

template <> struct GetPortMask< Typelist<> > { static constexpr uint16_t value = 0; };

template <class CurrentPin, class... RestPins>
struct GetPortMask< Typelist<CurrentPin, RestPins...> >
{
	static constexpr uint16_t value = (1 << CurrentPin::Pin::Number) | GetPortMask<Typelist<RestPins...> >::value;
};

template <class TList> struct GetInputMask;

template <> struct GetInputMask< Typelist<> > { static constexpr uint16_t value = 0; };

template <class CurrentPin, class... RestPins>
struct GetInputMask< Typelist<CurrentPin, RestPins...> >
{
	static constexpr uint16_t value = (1 << CurrentPin::Position) | GetInputMask<Typelist<RestPins...> >::value;
};

template <class PinList> struct GetInputProjected;

template <>
struct GetInputProjected<Typelist<> >
{
	static inline uint16_t UppendValue(uint16_t){
		return 0;
	}
};

template <class CurrentPin, class... RestPins>
struct GetInputProjected<Typelist<CurrentPin, RestPins...> >
{
	static inline uint16_t UppendValue(uint16_t inputVal){
		uint16_t res = inputVal&CurrentPin::PinListMask;
		if(res>0){
			if(CurrentPin::Position > CurrentPin::Pin::Number) res = res >> (CurrentPin::Position - CurrentPin::Pin::Number);
			else res = res << (CurrentPin::Pin::Number - CurrentPin::Position);
		}
		
		return res | GetInputProjected<Typelist<RestPins...> >::UppendValue(inputVal);
	}
};

template <class PortList, class PinList> struct PortWriteIterator;

template <class PinList>
struct PortWriteIterator<Typelist<>, PinList>
{
	static void Write(uint16_t){}
};

template <class currentPort, class... RestPorts, class PinList>
struct PortWriteIterator<Typelist<currentPort, RestPorts...>, PinList>
{
	using PinsInPort = typename GetPinsWithPort<PinList, currentPort>::Result;
	static constexpr uint16_t PortMask = GetPortMask<PinsInPort>::value;
	static constexpr uint16_t InputMask = GetInputMask<PinsInPort>::value;
	
	static void Write(uint16_t value){
		uint16_t result = GetInputProjected<PinList>::UppendValue(value&InputMask);
		if(result>0) currentPort::ClearAndSet(PortMask, result);
		PortWriteIterator<Typelist<RestPorts...>, PinList>::Write(value);
	}
};	

template <class PINS>
struct PinSet
{
	private:
		using PinsToPorts = typename GetPorts<PINS>::Result;
	public:
		using PinTypeList = PINS;
		using Ports = typename NoDuplicates<PinsToPorts>::Result;
		static void Write(uint16_t value){
			PortWriteIterator<Ports, PINS>::Write(value);
		}
};

template <class PINS> struct InitSLEDFSEG;

template <>
struct InitSLEDFSEG<Typelist<>>
{
	static void initPins(){}
};

template <class CurrentPin, class... RestPins>
struct InitSLEDFSEG<Typelist<CurrentPin, RestPins...> >
{
	static void initPins(){
		CurrentPin::Pin::setMode(MODE_O2);
		CurrentPin::Pin::setCnf(CNF_OPP);
		InitSLEDFSEG<Typelist<RestPins...> >::initPins();
	}
};

/*
First 8 pins - LED A, B, ..., DP
Last 4 pins - Digit pos 1-4

0b4321(dp)GFEDCBA
*/
template <class Pin1, class Pin2, class Pin3, class Pin4, // A, B, C, D
			class Pin5, class Pin6, class Pin7, class Pin8, // E, F, G, DP
				class Pin9, class Pin10, class Pin11, class Pin12> // D1, D2, D3, D4
class SLEDFSEG_Driver : PinSet<typename MakePinList<0, Pin1, Pin2, Pin3, Pin4,
													Pin5, Pin6, Pin7, Pin8,
														Pin9, Pin10, Pin11, Pin12>::Result>
{
	private:
		using PinList = typename MakePinList<0, Pin1, Pin2, Pin3, Pin4,
												Pin5, Pin6, Pin7, Pin8,
													Pin9, Pin10, Pin11, Pin12>::Result;
		using Base = PinSet<PinList>;
	
		static constexpr uint16_t digitMatrix[10] = {
					segF | segA | segB | segC | segD | segE, // 0
					segB | segC, // 1
					segA | segB | segG | segD | segE, // 2
					segA | segB | segG | segC | segD, // 3
					segF | segG | segB | segC, // 4
					segA | segF | segG | segC | segD, // 5
					segF | segA | segG | segC | segD | segE, // 6
					segA | segB | segC, // 7
					segF | segA | segB | segC | segD | segE | segG, // 8
					segF | segA | segB | segC | segD | segG | segDP // 9
		};
	public:
	
		static void inline initPins(){
			InitSLEDFSEG<PinList>::initPins();
		} 
		
		static void showOnPos(uint8_t what, uint8_t where){
			Base::Write(0);
			uint16_t val = digitMatrix[what] | (((~(0x1<<(where-1)))&0xF)<<8);
			Base::Write(val);
		}
		
		static void allLedOff(){
			Base::Write(0xF00);
		}
		
		static void initTimer(){
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Подключение TIM2 к генератору
			RCC->CFGR &= ~RCC_CFGR_HPRE; // Обнуление предделителя на шине AHB
			RCC->CFGR |= RCC_CFGR_HPRE_DIV8; // предделитель шины AHB на 8 (итого частота 9МГц)
			TIM2->PSC = 8999; // предделитель таймера на 9000(999+1) (итого частота 1КГц или же 1000 мс)
			TIM2->ARR = 999; // установка авторезета на 1000(999+1) (раз в секунду)
			TIM2->DIER |= TIM_DIER_UIE; // Включение прерываний на обновление ARR
			NVIC_SetPriority(TIM2_IRQn, 0); // Постановка приоритета для прерывания (чем меньше, тем выше приоритет)
			NVIC_EnableIRQ(TIM2_IRQn); // Включение обработки прерываний по обновлению таймера
			TIM2->EGR |= TIM_EGR_UG; // Генерация прерывания на обновление, дабы запустить загрузку конфигурационных данных
			TIM2->SR &= ~TIM_SR_UIF; // Обнуление вызванного прерывания
			TIM2->CR1 |= TIM_CR1_CEN;
		}
		
		static void drawNumber(uint16_t num){
			showOnPos(num/1000%10,1);
			showOnPos(num/100%10,2);
			showOnPos(num/10%10,3);
			showOnPos(num%10,4);
		}
};

























