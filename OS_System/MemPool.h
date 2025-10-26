#pragma once


class Mem_Func{
protected:
	//�޸� ���� ���� �ʱ�ȭ ���ִ� ����
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

//��� ��� �޸� Ǯ
//��� ������� �޸𸮿����� �ѱ�� ����
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

	//������ ��� �������� �޸𸮿����� ������ ħ���� ����
	//������ �޸𸮸� ��ȯ�Ҷ� ��ȸ������� ã�ƾ��ϱ� ������ O(n) �ӵ��̽��� �߻�
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



		//�⺻ �� 4����Ʈ
		Mem_Manage_Sec() : Mem_Size(4, 4){
			Values = new MemVal[4];
			Init_Mem(this->block_size, this->block_cnt);
		}

		Mem_Manage_Sec(size_t block_size, size_t block_cnt) : Mem_Size(block_size, block_cnt){
			Values = new MemVal[this->block_cnt];
			Init_Mem(this->block_size, this->block_cnt);
		}

		//�Ҵ�
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

		//��ü��ȯ free�ֵ� ���� ��ȯ
		void de_allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				Values[i].is_free = false;
				Mem_Set(Values[i].Data, 0, this->block_size);
			}
		}

		//�Ѱ��� ��ȯ
		void de_allocate(void* Data){
			for (size_t i = 0; i < this->block_cnt; i++){
				if (Values[i].Data == Data){
					Values[i].is_free = false;
					Mem_Set(Values[i].Data, 0, this->block_size);
					//delete�� �ü���� ȣ���ؼ� �Ҹ��ϸ� �޸� Ǯ ���� �ǹ̰� ������(�ӵ��� �������ٴ� �ǹ�)
					//delete Values[i].Data;
					//Values[i].Data = nullptr;
					//Values[i].Data = new char[block_size];
					return;
				}
			}
		}


	};

	//���������� �ϳ��� ū �޸� ������ �������� �����ϴ� ����
	//�ϳ��� ū �޸� ������ ��ΰ� �����ϱ� ������ ħ���� ������ ����
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
		bool* is_free;	//��뿩��
		Mem_Manage_Spd() : Mem_Size(block_size, block_cnt){
			Init_Mem(this->block_size, this->block_cnt);
		}

		Mem_Manage_Spd(size_t block_size, size_t block_cnt) : Mem_Size(block_size, block_cnt){
			Init_Mem(this->block_size, this->block_cnt);
		}

		//�Ҵ�
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

		//��ü��ȯ
		void de_allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				is_free[i] = false;
				Mem_Set(static_cast<void*>(static_cast<char*>(Values)+(i * this->block_size)), 0, this->block_size);
			}
		}

		//�Ѱ��� ��ȯ
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


//���� ũ�� ���� �޸� Ǯ
//����ڰ� ���ϴ� ũ�⸸ŭ �޸� ������ �ѱ�� ����
namespace Dynamic_MemPool{

}

//���� �� ����ȭ �ּ�ȭ �޸� Ǯ
//���°��� free������ ���ս��� ����ȭ�� ���̴� ����
namespace Merge_Fragment_MemPool{

}



