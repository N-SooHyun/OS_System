#pragma once


class Mem_Func{
protected:
	//메모리 내부 값을 초기화 해주는 영역
	virtual void Mem_Set(void* Data, size_t Val, size_t block_size){
		char* tmpData = static_cast<char*>(Data);
		for (size_t i = 0; i < block_size; i++){
			tmpData[i] = Val;
		}
	}
};

class Mem_Size{
protected:
	size_t block_size;
	size_t block_cnt;

	Mem_Size() : block_size(0), block_cnt(0){}
	Mem_Size(size_t block_size, size_t block_cnt) : block_size(block_size), block_cnt(block_cnt){}
};

//블록 기반 메모리 풀
//블록 기반으로 메모리영역을 넘기는 개념
namespace Block_MemPool{

	class MemVal{
	public:
		size_t index;
		void* Data;
		bool is_free;

		MemVal(size_t block_size) : is_free(false){ Data = new char[block_size]; }
		MemVal() : Data(nullptr), is_free(false){}
		~MemVal(){
			if (Data != nullptr) {
				delete[] static_cast<char*>(Data);
				Data = nullptr;
			}
		}
	};

	//안전성 기반 논리적으로 메모리영역을 나눠서 침범을 막음
	//단점은 메모리를 반환할때 순회방식으로 찾아야하기 때문에 O(n) 속도이슈가 발생
	class Mem_Manage_Sec : Mem_Func, Mem_Size{
	private:
		void Init_Mem(size_t block_size, size_t block_cnt){
			for (size_t i = 0; i < block_cnt; i++){
				Values[i].Data = new char[block_size];
				Values[i].is_free = false;
				Mem_Set(Values[i].Data, 0, block_size);
			}
		}
	public:
		MemVal* Values;



		//기본 블럭 4바이트
		Mem_Manage_Sec() : Mem_Size(4, 4){
			Values = new MemVal[4];
			Init_Mem(this->block_size, this->block_cnt);
		}

		Mem_Manage_Sec(size_t block_size, size_t block_cnt) : Mem_Size(block_size, block_cnt){
			Values = new MemVal[this->block_cnt];
			Init_Mem(this->block_size, this->block_cnt);
		}

		//할당
		void* allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				if (Values[i].is_free == false){
					Values[i].is_free = true;
					return Values[i].Data;
				}
			}

			std::cout << "Memory Full" << std::endl;

			return nullptr;
		}

		//전체반환 free애들 전부 반환
		void de_allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				Values[i].is_free = false;
				Mem_Set(Values[i].Data, 0, this->block_size);
			}
		}

		//한개만 반환
		void de_allocate(void* Data){
			for (size_t i = 0; i < this->block_cnt; i++){
				if (Values[i].Data == Data){
					Values[i].is_free = false;
					Mem_Set(Values[i].Data, 0, this->block_size);
					//delete로 운영체제를 호출해서 소멸하면 메모리 풀 쓰는 의미가 없어짐(속도가 떨어진다는 의미)
					//delete Values[i].Data;
					//Values[i].Data = nullptr;
					//Values[i].Data = new char[block_size];
					return;
				}
			}
		}


	};

	//물리적으로 하나의 큰 메모리 영역을 여러명이 공유하는 구조
	//하나의 큰 메모리 영역을 모두가 공유하기 때문에 침범의 여지가 있음
	class Mem_Manage_Spd : Mem_Func, Mem_Size{
	private:
		void Init_Mem(size_t block_size, size_t block_cnt){
			Values = new char[block_size * block_cnt];
			is_free = new bool[block_cnt];
			for (size_t i = 0; i < block_cnt; i++){
				is_free[i] = false;
			}
			Mem_Set(Values, 0, block_size*block_cnt);
		}
	public:
		void* Values;
		bool* is_free;	//사용여부
		Mem_Manage_Spd() : Mem_Size(block_size, block_cnt){
			Init_Mem(this->block_size, this->block_cnt);
		}

		Mem_Manage_Spd(size_t block_size, size_t block_cnt) : Mem_Size(block_size, block_cnt){
			Init_Mem(this->block_size, this->block_cnt);
		}

		//할당
		void* allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				if (is_free[i] == false){
					is_free[i] = true;
					return static_cast<void*>(static_cast<char*>(Values)+(i * block_size));
				}
			}
			std::cout << "Memory Full" << std::endl;

			return nullptr;
		}

		//전체반환
		void de_allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				is_free[i] = false;
				Mem_Set(static_cast<void*>(static_cast<char*>(Values)+(i * this->block_size)), 0, this->block_size);
			}
		}

		//한개만 반환
		void de_allocate(void* Data){
			if (Data < Values || Data >= static_cast<char*>(Values)+block_size * block_cnt){
				std::cout << "Invalid pointer" << std::endl;
				return;
			}
			size_t index = (static_cast<char*>(Data)-static_cast<char*>(Values)) / this->block_size;
			is_free[index] = false;
			Mem_Set(static_cast<void*>(static_cast<char*>(Values)+(index * this->block_size)), 0, this->block_size);
		}

	};
}


//동적 크기 지원 메모리 풀
//사용자가 원하는 크기만큼 메모리 영역을 넘기는 개념
namespace Dynamic_MemPool{

}

//병합 및 단편화 최소화 메모리 풀
//남는공간 free영역을 병합시켜 단편화를 줄이는 개념
namespace Merge_Fragment_MemPool{

}



